#ifndef DISTBOSS_SERVER_H
#define DISTBOSS_SERVER_H

#include "DistBossServer/JobOptionsMgr.h"
#include "DistBossServer/RpcInterface.h"
#include "DistBossServer/ServerExitHandler.h"
#include "DistBossServer/ServerErrorHandler.h"

class DistBossServer
{
   public :

      DistBossServer(const std::string& jobOpt);
      ~DistBossServer();

      const std::string& serverName() const { return m_serverName; }

      int run();


   private :

      DistBossServer();  //stop default


   private :

      JobOptionsMgr               m_optMgr;
      RpcInterface*               m_readerRpc;
      std::vector<RpcInterface*>  m_writerRpcs;

      ServerExitHandler*          m_exitHandler;
      ServerErrorHandler*         m_errorHandler;

      static const std::string  m_serverName;
      static std::string        autoServerName();

      // planned services @ server side
      enum SvcType {
	 RawEvtSvc,
	 SimSeedSvc,
	 RootEvtSvc,
	 WriteDst,
	 WriteRec,
	 WriteTuple,
	 WriteHist
      };
      static const std::string  m_svc[7];
};

#endif
