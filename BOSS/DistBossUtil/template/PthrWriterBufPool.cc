/**********************************************************
  +-> 1      2      3      4      5      6  ...  PoolSize
  |               ahead current  next               |
  |_________________________________________________|
 
  0, a looped buffer pool
  1, we set semIn as (PoolSize-1), so to protect the
  current event buffer not be changed by writeEvent
  2, we can't update the current event buffer until we
  move to the next event(in thread_writing()).
  3, the ahead event buffer is ready to update when we
  at the beginning of current event.
**********************************************************/

#include <iostream>


template <typename Writer, int PoolSize>
PthrWriterBufPool<Writer, PoolSize>::PthrWriterBufPool(typename Writer::WriterArgType& arg)
   : m_inLoop(-1),
     m_outLoop(-1)
{
   if ( PoolSize < 2 ) {
      std::cout << "[PthrWriterBufPool] The PoolSize of buffer must > 1" << std::endl;
      exit(1);
   }

   // use(PoolSize-1) to protect the current event buffer
   sem_init(&m_semIn,  0, (PoolSize-1));
   sem_init(&m_semOut, 0, 0);
   sem_init(&m_semFinalize, 0, 0);

   for ( int i = 0; i < PoolSize; ++i ) {
      // buffer in the loop for each event
      m_buf[i] = new AutoEnlargeBuffer(128*1024);
   }

   m_writer = new Writer(arg);

   // create an individual thread for writing
   pthread_create(&m_tid, NULL, thread_writing, (void*)this);
}

template <typename Writer, int PoolSize>
PthrWriterBufPool<Writer, PoolSize>::~PthrWriterBufPool()
{
   // wait for the end of thread_writing
   sem_wait(&m_semFinalize);

   for ( int i = 0; i < PoolSize; ++i ) {
      delete m_buf[i];
   }

   sem_destroy(&m_semIn);
   sem_destroy(&m_semOut);
   sem_destroy(&m_semFinalize);

   delete m_writer;
}

template <typename Writer, int PoolSize>
int PthrWriterBufPool<Writer, PoolSize>::writeEvent(void* pevt, int size)
{
   if ( m_writer->stat() != 0 ) return m_writer->stat();

   sem_wait(&m_semIn);

   int index = (++m_inLoop) % PoolSize;

   m_buf[index]->copy( pevt, size );

   sem_post(&m_semOut);

   return 0;
}

template <typename Writer, int PoolSize>
int PthrWriterBufPool<Writer, PoolSize>::stat()
{
   return m_writer->stat();
}

template <typename Writer, int PoolSize>
void* PthrWriterBufPool<Writer, PoolSize>::thread_writing(void* arg)
{
   PthrWriterBufPool<Writer, PoolSize>* pthis = (PthrWriterBufPool<Writer, PoolSize>*)arg;

   uint32_t        index;
   AutoEnlargeBuffer* pbuf;


   while ( true ) {
      // the ahead event buffer is ready for update now
      sem_post(&(pthis->m_semIn));
      // waiting for an event that ready to use
      sem_wait(&(pthis->m_semOut));

      index = (++(pthis->m_outLoop)) % PoolSize;

      pbuf = pthis->m_buf[index];

      try {
	 pthis->m_writer->writeEvent( pbuf->data(), pbuf->size() );
      }
      catch (ReachEndOfFileList& e) {
	 //std::cout << "[PthrWriterBufPool] Finalized Successfully!" << std::endl;
	 break;
      }
      catch (RawExMessage& e) {
	 e.print();
	 break;
      }
      catch ( ... ) {
	 std::cout << "[PthrWriterBufPool] Catch unexpected exception !" << std::endl;
	 exit(1);
      }
   }


   pthis->m_writer->close();

   sem_post(&(pthis->m_semFinalize));

   pthread_exit(NULL);
}
