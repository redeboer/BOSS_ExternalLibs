#ifndef DISTBOSS_WRITER_RPC_H
#define DISTBOSS_WRITER_RPC_H

#include "DistBossUtil/PthrWriterBufPool.h"
#include "DistBossServer/RpcInterface.h"
#include "dis.hxx"
#include <map>
#include <string>

template<class Writer>
class WriterRpc : virtual public DimRpc, virtual public RpcInterface
{
   public :

      WriterRpc(const std::string& svrName, const std::string& fname);
      virtual ~WriterRpc();

      int wait_to_end();


   private :

      void rpcHandler();

      void clearBak(int clientId);

      WriterRpc();  //stop default


   private :

      int  m_evtDone;

      int  m_inCode;
      int  m_outCode;

      int   m_size;
      void* m_pEvt;

      PthrWriterBufPool<Writer>*   m_writer;

      std::string m_svrName;

      std::map<int, AutoEnlargeBuffer*> m_evtBak;
};

#include "DistBossServer/template/WriterRpc.cc"

#endif
