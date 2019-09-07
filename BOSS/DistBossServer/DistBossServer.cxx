#include "DistBossServer/DistBossServer.h"
#include "DistBossServer/WriterRpc.h"
#include "DistBossServer/ReaderRpc.h"
#include "RootFile/RootFileWriter.h"
#include "RawFile/RawFileReader.h"
#include "IRawFile/RawFileExceptions.h"
#include <sstream>
#include <iostream>
#include <unistd.h>

const std::string  DistBossServer::m_serverName = DistBossServer::autoServerName();
const std::string  DistBossServer::m_svc[7] = {
   m_serverName + "/RawEvtSvc",
   m_serverName + "/SimSeedSvc",
   m_serverName + "/RootEvtSvc",
   m_serverName + "/WriteDst",
   m_serverName + "/WriteRec",
   m_serverName + "/WriteTuple",
   m_serverName + "/WriteHist"
};


std::string DistBossServer::autoServerName()
{
   std::stringstream spid;
   spid << getpid();

   char hname[64];
   gethostname(hname, 64);

   std::string name("/DistBoss/P");
   name += spid.str() + "@" + hname;

   return name;
}

DistBossServer::DistBossServer(const std::string& jobOpt)
   : m_optMgr(jobOpt)
{
   if ( m_optMgr.evtMax() == 0 ) {
      std::cout << "[DistBossServer] EvtMax = 0, exit now!" << std::endl;
      exit(0);
   }

   std::cout << "[DistBossServer] Starting server: " << m_serverName << std::endl;

   switch ( m_optMgr.inputType() ) {
      case ( 0 ) :
	 m_readerRpc = new ReaderRpc<RawFileReader>(m_svc[0], m_optMgr.inputFiles(), m_optMgr.evtMax());
	 break;
      case ( 1 ) :
	 std::cout << "[DistBossServer] Simulation is not supported by DistBoss yet!" << std::endl;
	 exit(1);
      case ( 2 ) :
	 //m_readerRpc = new ReaderRpc<RootFileReader>(m_svc[2], m_optMgr.inputFiles(), m_optMgr.evtMax());
	 //break;
	 std::cout << "[DistBossServer] Root input files are not supported by DistBoss yet!" << std::endl;
	 exit(1);
      default :
	 assert( false );  //we shouldn't come here
   }

   const std::vector<int>& outputs = m_optMgr.outputTypes();
   for ( std::vector<int>::const_iterator it = outputs.begin(); it != outputs.end(); ++it ) {
      switch ( *it ) {
	 case ( 3 ) :
	    m_writerRpcs.push_back(new WriterRpc<RootFileWriter>(m_svc[3], m_optMgr.dstFile()));
	    break;
	 case ( 4 ) :
	    m_writerRpcs.push_back(new WriterRpc<RootFileWriter>(m_svc[4], m_optMgr.recFile()));
	    break;
	 case ( 5 ) :
	    //std::cout << "[DistBossServer] Ntuple outputs is not supported by DistBoss yet!" << std::endl;
	    break;
	 case ( 6 ) :
	    //std::cout << "[DistBossServer] Hist outputs is not supported by DistBoss yet!" << std::endl;
	    break;
	 default :
	    assert(false);  //we shouldn't come here
      }
   }

   m_optMgr.clientOptsTemplate( m_serverName );

   m_exitHandler = new ServerExitHandler();
   m_errorHandler = new ServerErrorHandler();
}

DistBossServer::~DistBossServer()
{
   delete m_readerRpc;

   for ( unsigned int i = 0; i < m_writerRpcs.size(); ++i ) {
      delete m_writerRpcs[i];
   }

   std::cout << "[DistBossServer] Server stopped." << std::endl;

   delete m_exitHandler;
}

int DistBossServer::run()
{
   DimServer::addExitHandler( m_exitHandler );
   DimServer::addErrorHandler( m_errorHandler );

   DimServer::start(m_serverName.c_str());

   // we'd better check the return value of wait_to_end() here
   int nClients = m_readerRpc->wait_to_end();

   for ( unsigned int i = 0; i < m_writerRpcs.size(); ++i ) {
      nClients = m_writerRpcs[i]->wait_to_end();
   }

   return nClients;
}
