#ifndef DIM_RPC_READER_H
#define DIM_RPC_READER_H

#include <pthread.h>
#include <string>
#include <stdint.h>

class AutoEnlargeBuffer;
class DimRpcInfo;

class DimRpcReader
{
   public :

      typedef const std::string ReaderArgType;

      DimRpcReader(ReaderArgType& name);
      virtual ~DimRpcReader();

      const uint32_t*  nextEvent();
      const uint32_t*  currentEvent() const;
      uint32_t         runNo();
      std::string      currentFile();
      uint32_t         stat();


   private :

      DimRpcReader();  //stop default


   private :

      int                 m_inCode;
      int                 m_outCode;
      AutoEnlargeBuffer  *m_buffer;

      DimRpcInfo         *m_rpc;

      static pthread_mutex_t  m_rpcLock;
};

#endif
