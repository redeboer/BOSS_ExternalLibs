/**********************************************************
  +-> 1      2      3      4      5      6  ...  PoolSize
  |               ahead current  next               |
  |_________________________________________________|
 
  0, a looped buffer pool
  1, we set semIn as (PoolSize-1), so to protect the
  current event buffer not be changed by thread_filling
  2, we can't update the current event buffer until we
  move to the next event(call nextEvent()).
  3, the ahead event buffer is ready to update when we
  at the beginning of current event.
**********************************************************/

#include <iostream>


template <typename Reader, int PoolSize>
PthrReaderBufPool<Reader, PoolSize>::PthrReaderBufPool(typename Reader::ReaderArgType& arg)
   : m_inLoop(-1),
     m_outLoop(-1),
     m_RawFileException(0)
{
   if ( PoolSize < 2 ) {
      std::cout << "[PthrReaderBufPool] The PoolSize of buffer must > 1" << std::endl;
      exit(1);
   }

   // use(PoolSize-1) to protect the current event buffer
   sem_init(&m_semIn,  0, (PoolSize-1));
   sem_init(&m_semOut, 0, 0);
   pthread_mutex_init(&m_lock, NULL);

   sem_init(&m_semSyn, 0, 0);

   for ( int i = 0; i < PoolSize; ++i ) {
      // buffer in the loop for each event
      m_buf[i] = new AutoEnlargeBuffer(128*1024);
   }

   m_reader = new Reader(arg);

   // create an individual thread to fill to buffer
   pthread_create(&m_tid, NULL, thread_filling, (void*)this);
}

template <typename Reader, int PoolSize>
PthrReaderBufPool<Reader, PoolSize>::~PthrReaderBufPool()
{
   for ( int i = 0; i < PoolSize; ++i ) {
      delete m_buf[i];
   }

   delete m_RawFileException;

   sem_destroy(&m_semIn);
   sem_destroy(&m_semOut);

   delete m_reader;
}

template <typename Reader, int PoolSize>
const uint32_t* PthrReaderBufPool<Reader, PoolSize>::nextEvent()
{
   if ( m_outLoop < 0 ) sem_post(&m_semSyn);

   // the ahead event buffer is ready for update now
   sem_post(&m_semIn);
   // waiting for a ready to use event
   sem_wait(&m_semOut);

   pthread_mutex_lock(&m_lock);
   int inLoop = m_inLoop;
   pthread_mutex_unlock(&m_lock);

   if ( m_RawFileException != 0 && m_outLoop >= inLoop ) {
      if ( dynamic_cast<RawExMessage*>(m_RawFileException) ) {
	 throw RawExMessage(*(RawExMessage*)m_RawFileException);
      }
      if ( dynamic_cast<ReachEndOfFileList*>(m_RawFileException) ) {
	 throw ReachEndOfFileList(*(ReachEndOfFileList*)m_RawFileException);
      }
   }

   ++m_outLoop;

   const uint32_t* pevt = (uint32_t*)m_buf[ m_outLoop%PoolSize ]->data();
   return pevt;
}

template <typename Reader, int PoolSize>
const uint32_t* PthrReaderBufPool<Reader, PoolSize>::currentEvent() const
{
   //can't be called before any nextEvent() call
   const uint32_t* pevt = (uint32_t*)m_buf[ m_outLoop%PoolSize ]->data();
   return pevt;
}

template <typename Reader, int PoolSize>
uint32_t PthrReaderBufPool<Reader, PoolSize>::runNo()
{
   return m_reader->runNo();
}

template <typename Reader, int PoolSize>
std::string PthrReaderBufPool<Reader, PoolSize>::currentFile()
{
   return m_reader->currentFile();
}

template <typename Reader, int PoolSize>
uint32_t PthrReaderBufPool<Reader, PoolSize>::stat()
{
   return m_reader->stat();
}

template <typename Reader, int PoolSize>
void* PthrReaderBufPool<Reader, PoolSize>::thread_filling(void* arg)
{
   PthrReaderBufPool<Reader, PoolSize>* pthis = (PthrReaderBufPool<Reader, PoolSize>*)arg;

   uint32_t        index;
   const uint32_t* pevt;

   sem_wait(&(pthis->m_semSyn));

   while ( true ) {
      // waiting for a buffer that ready to update
      sem_wait(&(pthis->m_semIn));

      try {
	 pevt = pthis->m_reader->nextEvent();
      }
      catch (RawExMessage& e) {
	 pthis->m_RawFileException = new RawExMessage(e);
	 break;
      }
      catch (ReachEndOfFileList& e) {
	 pthis->m_RawFileException = new ReachEndOfFileList(e);
	 break;
      }
      catch ( ... ) {
	 std::cout << "[PthrReaderBufPool] Catch unexpected exception !" << std::endl;
	 exit(1);
      }

      pthread_mutex_lock(&(pthis->m_lock));
      index = (++(pthis->m_inLoop)) % PoolSize;
      pthread_mutex_unlock(&(pthis->m_lock));

      pthis->m_buf[index]->copy( (void*)pevt, pevt[1]*sizeof(uint32_t) );

      // post an event that ready to use
      sem_post(&(pthis->m_semOut));
   }

   sem_post(&(pthis->m_semOut));

   pthread_exit(NULL);
}
