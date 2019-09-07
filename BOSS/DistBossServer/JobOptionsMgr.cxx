#include "DistBossServer/JobOptionsMgr.h"

#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Parsers.h"  //in "GaudiSvc/JobOptionsSvc"
#include "JobOptionsSvc.h"  //in "GaudiSvc/JobOptionsSvc"
#include <fstream>

#define CheckSTATUS(msg) \
   if ( sc.isFailure() ) { \
      std::cout << "[JobOptionsMgr] " << msg << std::endl; \
      exit(1); \
   }

#define MyPARSE(var) \
{ \
   sc = Gaudi::Parsers::parse(var, pprop->toString()); \
   if ( sc.isFailure() ) { \
      std::cout << "[JobOptionsMgr] Failed to parse " << #var << std::endl; \
      exit(1); \
   } \
}


JobOptionsMgr::JobOptionsMgr(const std::string& optname)
   : m_optName(optname),
     m_iFileType(-1),
     m_evtMax(10),
     m_dstFile("digi.root"),
     m_recFile("digi.root")
{
   IService* svc;
   StatusCode sc = Gaudi::svcLocator()->service("JobOptionsSvc", svc);
   CheckSTATUS("Failed to retrieve the JobOptionsSvc");
   m_svc = dynamic_cast<JobOptionsSvc*>(svc);

   sc = m_svc->setProperty( StringProperty("TYPE", "FILE") );
   sc = m_svc->setProperty( StringProperty("PATH", optname) );
   CheckSTATUS("Failed to set the property of DistBossServer");

   sc = m_svc->initialize();
   CheckSTATUS("Failed to initialize JobOptionsSvc");

   const Property* pprop = getProperty("ApplicationMgr", "EvtMax");
   if ( pprop != NULL ) {
      MyPARSE( m_evtMax );
   }
   
   if ( (pprop = getProperty("RawDataInputSvc", "InputFiles")) != NULL ) {
      m_iFileType = 0;
      MyPARSE( m_inputFiles );
   }
   else if ( (pprop = getProperty("BesSim", "TDSFlag")) != NULL ) {
      m_iFileType = 1;
      // to be continued ...
   }
   else if ( (pprop = getProperty("EventCnvSvc", "digiRootInputFile")) != NULL ) {
      m_iFileType = 2;
      MyPARSE( m_inputFiles );
   }
   else {
      std::cout << "[JobOptionsMgr] Unknown Job Type!" << std::endl;
      exit(1);
   }

   if ( (pprop = getProperty("EventPreSelect","WriteDst")) != NULL ) {
      bool status;
      MyPARSE(status);
      if ( status ) {
	 m_oFileTypes.push_back(3);
	 if ( (pprop = getProperty("WriteDst", "digiRootOutputFile")) != NULL ) {
	    MyPARSE( m_dstFile );
	 }
      }
   }

   if ( (pprop = getProperty("EventPreSelect","WriteRec")) != NULL ) {
      bool status;
      MyPARSE(status);
      if ( status ) {
	 m_oFileTypes.push_back(4);
	 if ( (pprop = getProperty("WriteRec", "digiRootOutputFile")) != NULL ) {
	    MyPARSE( m_recFile );
	 }
      }
   }

   if ( (pprop = getProperty("NTupleSvc", "Output")) != NULL ) {
      m_oFileTypes.push_back(5);
      MyPARSE( m_tupleOpts );
   }

   if ( (pprop = getProperty("THistSvc", "Output")) != NULL ) {
      m_oFileTypes.push_back(6);
      MyPARSE( m_histOpts);
   }
}

JobOptionsMgr::~JobOptionsMgr()
{
   StatusCode sc = m_svc->finalize();
   CheckSTATUS("Failed to finalize JobOptionsSvc!");
}

#define ignorePROPERTY(client, pname) \
   if ( clients[i] == client && property->name() == pname ) continue

void JobOptionsMgr::clientOptsTemplate(const std::string& svrName)
{
   //Gaudi::Parsers::Catalogue* m_clientCatalogue = new Gaudi::Parsers::Catalogue;
	 SvcCatalog* m_clientCatalogue = new SvcCatalog;

   StatusCode sc;

   std::vector<std::string> clients = m_svc->getClients();
   for ( unsigned int i = 0; i < clients.size(); ++i ) {
      const std::vector<const Property*>* properties = m_svc->getProperties(clients[i]);
      for ( unsigned int j = 0; j < properties->size(); ++j ) {
	 const Property* property = properties->at(j);

	 ignorePROPERTY("ApplicationMgr", "EvtMax");

	 ignorePROPERTY("RawDataInputSvc", "InputFiles");
	 //ignorePROPERTY("BesRndmGenSvc", "RndmSeed");  //placeholder for simulation
	 ignorePROPERTY("EventCnvSvc", "digiRootInputFile");

	 ignorePROPERTY("WriteDst", "digiRootOutputFile");
	 ignorePROPERTY("WriteRec", "digiRootOutputFile");
	 //ignorePROPERTY("NTupleSvc", "Output");  //placeholder for NTupleSvc
	 //ignorePROPERTY("THistSvc", "Output");  //placeholder for THistSvc

	 StringProperty* tmp= new StringProperty(property->name(), property->toString());
	 sc = m_clientCatalogue->addProperty(clients[i], tmp);
	 if ( sc.isFailure() ) {
			StringProperty* tmp2 = new StringProperty(property->name(), '"'+property->toString()+'"');
	    m_clientCatalogue->addProperty(clients[i], tmp2);
	 }
      }
   }

	 StringProperty* tmpservername = new StringProperty("ServerName", '"'+svrName+'"');
   m_clientCatalogue->addProperty("DistBoss", tmpservername);
   if ( m_iFileType == 0 ) {
		  StringProperty* tmprunmode = new StringProperty("RunMode", "3");
      m_clientCatalogue->addProperty("RawDataInputSvc", tmprunmode);
   }
   //else if ( m_iFileType == 1 ) {  //placeholder for simulation
   //   m_clientCatalogue->addProperty("BesRndmGenSvc", "RunMode", "3");
   //}
   //else if ( m_iFileType == 2 ) {
   //   m_clientCatalogue->addProperty("EventCnvSvc", "RunMode", "3");
   //}
   for ( unsigned int i = 0; i < m_oFileTypes.size(); ++i ) {
      int type = m_oFileTypes[i];
      if ( type == 3 ) {
	 m_clientCatalogue->addProperty("WriteDst", new StringProperty("RunMode", "3"));
      }
      else if ( type == 4 ) {
	 m_clientCatalogue->addProperty("WriteRec", new StringProperty("RunMode", "3"));
      }
      //else if ( type == 5 ) {
      //   m_clientCatalogue->addProperty("NTupleSvc", "RunMode", "3");
      //}
      //else if ( type == 6 ) {
      //   m_clientCatalogue->addProperty("THistSvc", "RunMode", "3");
      //}
   }

   std::string client_opt_name = m_optName + ".distboss_client";
   std::ofstream os(client_opt_name.c_str(), std::ios::out|std::ios::trunc);
   if ( ! os ) {
      std::cout << "[JobOptionsMgr] Failed to open client_template file!" << std::endl;
      exit(1);
   }

   m_clientCatalogue->fillStream( os );

   delete m_clientCatalogue;
}

const Property* JobOptionsMgr::getProperty(const std::string& client, const std::string& name)
{
   const Property* pprop = NULL;

   const std::vector<const Property*>* properties = m_svc->getProperties(client);
   if ( properties != NULL ) {
      for ( unsigned int i = 0; i < properties->size(); ++i ) {
	 if ( properties->at(i)->name() == name) {
	    pprop = properties->at(i);
	    break;
	 }
      }
   }

   return pprop;
}
