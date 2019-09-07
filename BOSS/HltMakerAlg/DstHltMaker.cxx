#include "HltMakerAlg/DstHltMaker.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "HltEvent/HltEvent.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/DstHltInf.h"

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

DstHltMaker::DstHltMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
 // declareProperty("FittingMethod", m_fittingMethod = 2);
 // declareProperty("ConfigFile",  m_configFile = "MDCConfig.xml");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstHltMaker::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  m_ntot=0;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstHltMaker::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Part 1: Get the event header, print out event and run number
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "DstHltMaker: retrieved event: " <<"  Event Number  "<< eventId << "  run: " <<runId  << endreq;
  
  StatusCode sc = StatusCode::SUCCESS ;

  // register ReconEvent Data Object to TDS; 2005/09/14
  //HltEvent* hltEvt = new HltEvent();
  //sc = eventSvc()->registerObject(EventModel::Hlt::Event, hltEvt);  

  //Part 2: Retrieve  
  DstHltInf* dstHltInf = new DstHltInf();
  SmartDataPtr<HltInf> recHltInf(eventSvc(),"/Event/Hlt/HltInf");
  //SmartDataPtr<DstHltInf> recHltInf(eventSvc(),"/Event/Hlt/DstHltInf");
  if (!recHltInf) {
    log << MSG::DEBUG << "Could not find HltInf" << endreq;
    //return StatusCode::SUCCESS;
  } else {
    dstHltInf->setEventType(recHltInf->getEventType());
    dstHltInf->setAlgProcess(recHltInf->getAlgProcess());
    dstHltInf->setCriteriaTable(recHltInf->getCriteriaTable());
    dstHltInf->setVersion(recHltInf->getVersion());
    dstHltInf->setTotalEnergy(recHltInf->getTotalEnergy());
    dstHltInf->setNumber(recHltInf->getNumber());
    dstHltInf->setNCON(recHltInf->getNCON());
    m_ntot++;
  }
  DataObject *aDstHltInfEvent;
  eventSvc()->findObject("/Event/Hlt/DstHltInf", aDstHltInfEvent);
  if(aDstHltInfEvent!=NULL) {
    SmartDataPtr<DstHltInf> hltInf(eventSvc(),"/Event/Hlt/DstHltInf");
    log<<MSG::DEBUG<<"old type="<<hltInf->getEventType()<<", alg="<<hltInf->getAlgProcess()
       <<", criteria="<<hltInf->getCriteriaTable()<<", version="<<hltInf->getVersion()
       <<", energy="<<hltInf->getTotalEnergy()<<endreq;
    //then unregister DstHltInf
    sc = eventSvc()->unregisterObject("/Event/Hlt/DstHltInf");
    delete aDstHltInfEvent;   //it must be delete to avoid memory leakage                                            
    log << MSG::INFO << "Old DstHltInf has been unregistered" << endreq;
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister DstHltInf" << endreq;
      return( StatusCode::FAILURE);
    }
  }
  sc = eventSvc()->registerObject(EventModel::Hlt::DstHltInf,dstHltInf);
  
  log << MSG::DEBUG <<"Event No."<<eventId<<" = "
      << dstHltInf->getEventType()<<"  "
      << dstHltInf->getAlgProcess()<<"  "
      << dstHltInf->getCriteriaTable()<<"  "
      << dstHltInf->getVersion()<<"  "
      << dstHltInf->getTotalEnergy()<<endreq;

  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstHltMaker::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize DstHltMaker" << endreq;
  log << MSG::ALWAYS << m_ntot << " events are converted." <<endreq;

  return StatusCode::SUCCESS;
}
