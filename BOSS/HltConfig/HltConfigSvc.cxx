#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/IIncidentSvc.h"
//#include "GaudiKernel/Incident.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "HltConfig/HltConfigSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"

#include <iostream>
#include <fstream>
#include <utility>

//static SvcFactory<HltConfigSvc> s_factory;
//const SvcFactory& HltConfigSvcFactory = s_factory;
using namespace HltProcessor;

HltConfigSvc::HltConfigSvc( const std::string& name, ISvcLocator* sl ) : 
  Service(name, sl),
  m_dbsvc(0)
{   
  m_propMgr.declareProperty( "FromDB", m_fromDatabase = true);
  m_propMgr.declareProperty( "seqListFile", m_seqListFile = "myseqlist.xml");
  m_propMgr.declareProperty( "sigListFile", m_sigListFile = "mysiglist.xml");  
    
  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("HltConfigSvc", &m_propMgr);
  jobSvc->release();
}


HltConfigSvc::~HltConfigSvc() {
  
}

StatusCode HltConfigSvc::queryInterface (const InterfaceID& riid, void** ppvIF ) 
{
  if ( IID_IHltConfigSvc.versionMatch(riid) ) { 
//    *ppvIF = static_cast<IHltConfigSvc*> (this); 
    *ppvIF = (HltConfigSvc*)this; 
  } else { 
    return Service::queryInterface(riid, ppvIF) ; 
  }
  return StatusCode::SUCCESS;
}


void HltConfigSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::INFO << "handle: " << inc.type() << endreq;

  if ( inc.type() == "NewRun" ){
    if(m_fromDatabase){
      log << MSG::DEBUG << "start read from database" << endreq;
      StatusCode sc = GenFileFromDatabase();
      if (sc .isFailure() ) {
	log << MSG::ERROR << "Unable to read config from DatabaseSvc " << endreq;
	return;
      }
      //if(MyFrame::instance()) MyFrame::destruct();
      log << MSG::INFO << "sequenceListFileLocation= " << m_seqListFile << endreq;
      log << MSG::INFO << "signatureListFileLocation= " << m_sigListFile << endreq;
      MyFrame::instance()->init(m_seqListFile, m_sigListFile);
      MyFrame::instance()->final();
    }
  }
}

StatusCode HltConfigSvc::initialize ( ) 
{

  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << ": Start of initialisation" << endreq;

  IIncidentSvc* incsvc;
  StatusCode sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ){
    incsvc -> addListener(this, "NewRun", priority);
  }
  else{
    log << MSG::FATAL << "Cannot find IncidentSvc" << endreq;
    return sc;
  }

  if(m_fromDatabase){
    sc = serviceLocator()->service("DatabaseSvc",m_dbsvc,true);
    if (sc .isFailure() ) {
      log << MSG::ERROR << "Unable to find DatabaseSvc " << endreq;
      return sc;
    }
  }

  sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
    return sc;
  }

  if (MyFrame::instance() == 0)
    log << MSG::INFO << "MyFrame::instance() == 0" << endreq;
  if(!m_fromDatabase){
    log << MSG::ALWAYS << "Read config from files" << endreq;
    MyFrame::instance()->init(m_seqListFile, m_sigListFile);
    // Finalize Frame. New version with singleton
    MyFrame::instance()->final();
  }
  return StatusCode::SUCCESS;
}


const std::vector<std::pair<Signature*,Sequence*> >& HltConfigSvc::retrieve()
{
// Get the final vector object and play around with it.     
//std::vector<std::pair<Signature*,Sequence*> > m_Vector = MyFrame::instance()->getTablesVector();
  return MyFrame::instance()->getTablesVector();
}

StatusCode HltConfigSvc::finalize ( ) 
{
  MsgStream log(messageService(), name());
  //MyFrame::destruct();
  log << MSG::INFO << name() << " finalized successfully" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode HltConfigSvc::GenFileFromDatabase(){
  MsgStream log(messageService(), name());
  if(!m_dbsvc){
    log << MSG::FATAL << " m_dbsvc=0" << endreq;
    return StatusCode::FAILURE;
  } 
  
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  
  log << MSG::INFO << "run=" << run << endreq;

  char stmt1[200];
  sprintf(stmt1,"select efconfig_id from RunParams where run_number = %d",abs(run));
 
  DatabaseRecordVector res;
  int row_no = m_dbsvc->query("RunInfo",stmt1,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  long int efId=atoi((*res[0])["efconfig_id"]);

  char stmt2[200];
  sprintf(stmt2,"select Seqlist_dtd,Siglist_dtd,Seqlist_xml,Siglist_xml from EFConfInfo where EFConfId = %ld", efId);

  res.clear();
  row_no = m_dbsvc->query("configdb",stmt2,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  
  long int seq_dtd,sig_dtd,seq_xml,sig_xml;
  seq_dtd=atoi((*res[0])["Seqlist_dtd"]);
  seq_dtd=18;
  sig_dtd=atoi((*res[0])["Siglist_dtd"]);
  sig_dtd=15;
  seq_xml=atoi((*res[0])["Seqlist_xml"]);
  sig_xml=atoi((*res[0])["Siglist_xml"]);

  ofstream outfile;

  char stmt3[200];
  sprintf(stmt3,"select FileName,FileText from EFfile where EFfileId = %ld", seq_dtd);
  res.clear();
  row_no = m_dbsvc->query("configdb",stmt3,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  outfile.open((*res[0])["FileName"],ios_base::out);
  outfile << (*res[0])["FileText"];
  outfile.close();

  char stmt4[200];
  sprintf(stmt4,"select FileName,FileText from EFfile where EFfileId = %ld", sig_dtd);
  res.clear();
  row_no = m_dbsvc->query("configdb",stmt4,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  outfile.open((*res[0])["FileName"],ios_base::out);
  outfile << (*res[0])["FileText"];
  outfile.close();

  char stmt5[200];
  sprintf(stmt5,"select FileName,FileText from EFfile where EFfileId = %ld", seq_xml);
  res.clear();
  row_no = m_dbsvc->query("configdb",stmt5,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  outfile.open((*res[0])["FileName"],ios_base::out);
  outfile << (*res[0])["FileText"];
  outfile.close();
  m_seqListFile = string((*res[0])["FileName"]);
  
  char stmt6[200];
  sprintf(stmt6,"select FileName,FileText from EFfile where EFfileId = %ld", sig_xml);
  res.clear();
  row_no = m_dbsvc->query("configdb",stmt6,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    return StatusCode::FAILURE;
  }
  outfile.open((*res[0])["FileName"],ios_base::out);
  outfile << (*res[0])["FileText"];
  outfile.close();
  m_sigListFile = string((*res[0])["FileName"]);

  return StatusCode::SUCCESS;  
}
