#ifndef PTHR_READER_BUF_POOL_H
#define PTHR_READER_BUF_POOL_H

#include "DistBossUtil/AutoEnlargeBuffer.h"
#include "IRawFile/RawFileExceptions.h"
#include "IRawFile/IRawReader.h"
#include <semaphore.h>
#include <pthread.h>
#include <vector>
#include <string>

template <typename Reader, int PoolSize = 8>
class PthrReaderBufPool : public IRawReader
{
   public :

      PthrReaderBufPool(typename Reader::ReaderArgType& arg);
      virtual ~PthrReaderBufPool();

      inline const uint32_t*  nextEvent();
      inline const uint32_t*  currentEvent() const;
      inline uint32_t         runNo();
      inline std::string      currentFile();
      inline uint32_t         stat();


   private :

      static void*  thread_filling(void* arg);

      PthrReaderBufPool();  //stop default


   private :

      int                 m_inLoop;
      int                 m_outLoop;
      AutoEnlargeBuffer*  m_buf[PoolSize];

      Reader*             m_reader;

      pthread_t           m_tid;
      sem_t               m_semIn;
      sem_t               m_semOut;
      pthread_mutex_t     m_lock;

      sem_t               m_semSyn;

      // copy of possible exceptions
      RawFileException*   m_RawFileException;
};

#include "DistBossUtil/template/PthrReaderBufPool.cc"

#endif
