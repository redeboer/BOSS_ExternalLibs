#ifndef PTHR_WRITER_BUF_POOL_H
#define PTHR_WRITER_BUF_POOL_H

#include "DistBossUtil/AutoEnlargeBuffer.h"
#include "IRawFile/RawFileExceptions.h"
#include "IRawFile/IRawWriter.h"
#include <semaphore.h>
#include <pthread.h>

template <typename Writer, int PoolSize = 8>
class PthrWriterBufPool : virtual IRawWriter
{
   public :

      PthrWriterBufPool(typename Writer::WriterArgType& arg);
      virtual ~PthrWriterBufPool();

      int writeEvent(void* pevt, int size);
      int stat();


   private :

      static void*  thread_writing(void* arg);

      PthrWriterBufPool();  //stop default


   private :

      int                 m_inLoop;
      int                 m_outLoop;
      AutoEnlargeBuffer*  m_buf[PoolSize];

      Writer*             m_writer;

      pthread_t           m_tid;
      sem_t               m_semIn;
      sem_t               m_semOut;

      sem_t               m_semFinalize;
};

#include "DistBossUtil/template/PthrWriterBufPool.cc"

#endif
