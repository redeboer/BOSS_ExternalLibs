#ifndef DISTBOSS_READER_RPC_H
#define DISTBOSS_READER_RPC_H

#include "DistBossUtil/PthrReaderBufPool.h"
#include "DistBossServer/RpcInterface.h"
#include "dis.hxx"
#include <string>
#include <map>
#include <vector>
#include <semaphore.h>

template<class Reader>
class ReaderRpc : virtual public DimRpc, virtual public RpcInterface
{
   public :

      ReaderRpc(const std::string& svrName, const std::vector<std::string>& fnames, int evtMax);
      virtual ~ReaderRpc();

      int wait_to_end();


   private :

      void rpcHandler();

      ReaderRpc();  //stop default


   private :

      int  m_evtDone;
      int  m_evtMax;

      int  m_stopFlag;

      int  m_inCode;
      int  m_outCode;
      const uint32_t* m_pEvt;

      PthrReaderBufPool<Reader>* m_freader;

      sem_t  m_sem;

      std::string m_svrName;

      std::map<int, AutoEnlargeBuffer*> m_evtBak;
};

#include "DistBossServer/template/ReaderRpc.cc"

#endif
