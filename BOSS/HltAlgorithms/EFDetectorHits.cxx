#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/IJobOptionsSvc.h"
//#include "GaudiKernel/ISvcLocator.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
#include "EmcRawEvent/EmcDigi.h"
#include "Identifier/EmcID.h"
#include "TofRawEvent/TofDigi.h"
#include "Identifier/TofID.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/MdcID.h"
#include "MucRawEvent/MucDigi.h"
#include "Identifier/MucID.h"
#include "HltAlgorithms/EFDetectorHits.h"
#include "CLHEP/Random/RandFlat.h"

using namespace Event;

EFDetectorHits::EFDetectorHits(const std::string& name, ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator) {
 
  int output=m_output/100000;
#if 1
  //m_propMgr.declareProperty("OutputLevel", output);
  //cout << "OutputLevel="<<output<<";  name="<<name<<endl;
  // Get a pointer to the Job Options Service
  //IJobOptionsSvc* jobSvc;
  //Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
  //jobSvc->setMyProperties("EFDetector", &m_propMgr);
  //cout << "OutputLevel="<<output<<";  name="<<name<<endl;
#endif
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);

  m_nmdc = new CriteriaItemValue;
  m_nmuc = new CriteriaItemValue;
  m_ntof = new CriteriaItemValue;
  m_nemc = new CriteriaItemValue;
  m_derr = new CriteriaItemValue;
  m_rndm = new CriteriaItemValue;
}

EFDetectorHits::~EFDetectorHits() {
  delete m_nmdc;
  delete m_nmuc;
  delete m_ntof;
  delete m_nemc;
  delete m_derr;
  delete m_rndm;
}

StatusCode EFDetectorHits::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("nmdc", m_nmdc);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(nmdc) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("nmuc", m_nmuc);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(nmuc) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("ntof", m_ntof);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(ntof) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("nemc", m_nemc);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(nemc) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("derr", m_derr);
  if(sc.isFailure()) {
    log << MSG::ERROR << "m_HltStoreSvc->put(derr) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("rndm", m_rndm);
  if(sc.isFailure()) {
    log << MSG::ERROR << "m_HltStoreSvc->put(rndm) wrong" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode EFDetectorHits::execute() {
  
  //reset();

  MsgStream log(msgSvc(), name());
  
  // Part 1: Get the event header, print out event number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "eventNo-->" << eventHeader->eventNumber()<<endreq;

  //Part 2: Retrieve Hits Collection 
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
  if (!mdcDigiCol) {
    log << MSG::FATAL << "Could not find Mdc digi!!" << endreq;
    return( StatusCode::FAILURE);
  }

  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
  if (!tofDigiCol) {
    log << MSG::FATAL << "Could not find Tof digi!!" << endreq;
    return( StatusCode::FAILURE);
  }
  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
    log << MSG::FATAL << "Could not find Emc digi!!" << endreq;
    return( StatusCode::FAILURE);
  }
  if(m_rawDigiSvc){
    if(m_rawDigiSvc->isOnlineMode()){
      emcDigiCol=&(m_rawDigiSvc->getEmcDigiVec(EmcRawDataProvider::DropLow|EmcRawDataProvider::CutTime|EmcRawDataProvider::DropHot|EmcRawDataProvider::DropDead));
    }
  }
  
  SmartDataPtr<MucDigiCol> mucDigiCol(eventSvc(),"/Event/Digi/MucDigiCol");
  if (!mucDigiCol) {
    log << MSG::FATAL << "Could not find Muc digi!!" << endreq;
    return( StatusCode::FAILURE);
  }

  //Part 3: Get information of hits
  unsigned int nhitsMdc=mdcDigiCol->size();
  if(m_rawDigiSvc){
    if(m_rawDigiSvc->isOnlineMode()){
      nhitsMdc=m_rawDigiSvc->getMdcDigiVec(0x38000).size();
    }
  }
  unsigned int nhitsEmc=emcDigiCol->size();
  unsigned int nhitsMuc=mucDigiCol->size();
  unsigned int nhitsTof=tofDigiCol->size();

  double rndm=CLHEP::RandFlat::shoot();
  
  log << MSG::INFO << " Number of hits in MDC:EMC:MUC:TOF is " << nhitsMdc
      << ":" << nhitsEmc << ":" << nhitsMuc << ":" <<nhitsTof <<";  " 
      << "random number:" << rndm <<endreq;

  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_nmdc->setValue(nhitsMdc);
  m_nmuc->setValue(nhitsMuc);
  m_ntof->setValue(nhitsTof);
  m_nemc->setValue(nhitsEmc);

  m_rndm->setValue(rndm);

  m_ef->addToEFVec(nhitsMdc, 5);
  m_ef->addToEFVec(nhitsTof, 25);
  m_ef->addToEFVec(nhitsEmc, 30);
  m_ef->addToEFVec(nhitsMuc, 52);
 
  m_ef->addToEFVec(0xFFFFFFF7,37);   //nsec init                                                                          
 
  m_ef->setVecBit(true, 0, 0);
  m_ef->addToEFVec(1, 1);
  m_ef->addToEFVec(1<<8, 1);
  m_ef->addToEFVec(1<<16, 1);
  m_ef->addToEFVec(1<<24, 1);

  m_run=1;

  return StatusCode::SUCCESS;
}

StatusCode EFDetectorHits::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFDetectorHits::reset() {
  
  if(m_run){
    m_nmdc->reset();
    m_nmuc->reset();
    m_ntof->reset();
    m_nemc->reset();
    m_derr->reset();
    m_rndm->reset();
    m_run=0;
  }
  return;
}
