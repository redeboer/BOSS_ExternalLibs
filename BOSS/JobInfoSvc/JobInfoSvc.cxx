#include "JobInfoSvc/JobInfoSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "xmlrpc++0.7/src/XmlRpc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


using namespace XmlRpc;


JobInfoSvc::JobInfoSvc(const std::string& name, ISvcLocator* pSvcLocator) :
  Service(name, pSvcLocator) {
    declareProperty("xmlrpcServer",	m_xmlrpcServer = "202.122.37.68");
    declareProperty("xmlrpcPort",	m_xmlrpcPort = 8080);
    declareProperty("xmlrpcUrl",	m_xmlrpcUrl = "/bemp/xmlrpc");
    declareProperty("xmlrpcMethod",	m_xmlrpcMethod = "SetJobInfo.setEvtNum");
  }

StatusCode JobInfoSvc::initialize(){
  MsgStream log( msgSvc(), name() );
  log << MSG::INFO << "in initialize" << endreq;

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ){
    incsvc -> addListener(this, "BeginEvent", priority);
  }

  m_outputFileName = getJobOutputFile();
  m_count = 0;

  //Set initialize value of real event number
  xmlrpc(-1);

  return StatusCode::SUCCESS;
}

StatusCode JobInfoSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IJobInfoSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IJobInfoSvc*)this;
  }else{ 
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void JobInfoSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );

  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() == "BeginEvent" ){
    log << MSG::DEBUG << "Begin Event" << endreq;
    //count execute time
    m_count++;
  }
}


StatusCode JobInfoSvc::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize" << endreq;

  //Keep this line for log file output please!
  std::cout<< "JobInfoSvc: totle event number =  "<< m_count << std::endl;

  //Save event number to job manage database
  xmlrpc(m_count);

  return StatusCode::SUCCESS;
}

int JobInfoSvc::xmlrpc(int evtNum){
  MsgStream log(msgSvc(), name());

  XmlRpcClient c(m_xmlrpcServer.c_str(), m_xmlrpcPort, m_xmlrpcUrl.c_str());
  XmlRpcValue args, result;

  args[0] = m_outputFileName; 
  args[1] = evtNum;

  if (args[0] != "" && c.execute(m_xmlrpcMethod.c_str(), args, result)){
    log << MSG::INFO << " set evtNum = "<< evtNum << endreq;
  }else{
    log << MSG::ERROR<< " Error in execute "<< m_xmlrpcMethod << endreq;
    return -1;
  }

  return 0;
}

std::string JobInfoSvc::getJobOutputFile(){
  MsgStream log(msgSvc(), name());
  std::string outputFileName = "";

  IJobOptionsSvc* jobSvc; 
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);

  const std::vector<const Property*>* properties_event = jobSvc->getProperties("EventCnvSvc");
  if (properties_event != NULL) {
    for (unsigned int i = 0; i < properties_event->size(); i++) {
      if ((*properties_event)[i]->name() == "digiRootOutputFile") {
	outputFileName = (*properties_event)[i]->toString();
	break;
      }
    }
  }

  const std::vector<const Property*>* properties_root = jobSvc->getProperties("RootCnvSvc");
  if (properties_root != NULL) {
    for (unsigned int i = 0; i < properties_root->size(); i++) {
      if ((*properties_root)[i]->name() == "digiRootOutputFile") {
	outputFileName = (*properties_root)[i]->toString();
	break;
      }
    }
  }

  //FIXME for RawDataCnvSvc

  return outputFileName;
}
