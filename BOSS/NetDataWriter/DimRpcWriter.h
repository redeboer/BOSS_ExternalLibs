#ifndef DIM_RPC_WRITER_H
#define DIM_RPC_WRITER_H

#include <pthread.h>
#include <string>

class AutoEnlargeBuffer;
class DimRpcInfo;

class DimRpcWriter
{
   public :

      typedef const std::string WriterArgType;

      DimRpcWriter(WriterArgType& name);
      virtual ~DimRpcWriter();

      int       writeEvent(void *pevt, int size);
      int       close();

      int       stat();


   private :

      DimRpcWriter();  //stop default


   private :

      int                 m_inCode;
      DimRpcInfo         *m_rpc;

      static pthread_mutex_t  m_rpcLock;
};

#endif
