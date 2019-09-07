#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/PropertyMgr.h"

#include "T0Dummy/T0Dummy.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "McTruth/McParticle.h"
#include "EvTimeEvent/RecEsTime.h"

using namespace Event;

T0Dummy::T0Dummy(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator)
{

}

StatusCode T0Dummy::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode T0Dummy:: execute(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return StatusCode::FAILURE;
  }

  int eventNo=eventHeader->eventNumber();
  int runNo=eventHeader->runNumber();


  if(runNo>0){
    log<<MSG::FATAL<<" Run No:"<<runNo<<" > 0"<<endreq;
    return StatusCode::FAILURE;
  }

  SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
  if(!mcParticleCol){
    log<<MSG::FATAL<<"Could not retrieve McParticelCol"<<endreq;
    return StatusCode::FAILURE;
  }

  double mcTestime=-999.;
  Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
  for (;iter_mc != mcParticleCol->end(); iter_mc++){
    if((*iter_mc)->primaryParticle()){
      mcTestime=(*iter_mc)->initialPosition().t();
    }
  }

  //clear RecEsTimeCol
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
  DataObject *aRecEsTime;
  eventSvc()->findObject("/Event/Recon/RecEsTimeCol",aRecEsTime);
  if(aRecEsTime!=NULL){
    dataManSvc->clearSubTree("/Event/Recon/RecEsTimeCol");
    eventSvc()->unregisterObject("/Event/Recon/RecEsTimeCol");
  }

  // register event start time to TDS
  RecEsTimeCol *aRecEsTimeCol = new RecEsTimeCol;
  StatusCode sc = eventSvc()->registerObject("/Event/Recon/RecEsTimeCol", aRecEsTimeCol);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Could not register RecEsTimeCol" << endreq;
    return StatusCode::FAILURE;
  }

  RecEsTime *arecestime = new RecEsTime;
  arecestime->setTest(mcTestime);
  arecestime->setStat(-999);

  //std::cout<< " T0Dummy: "<< mcTestime<< std::endl;
  aRecEsTimeCol->push_back(arecestime);

  return StatusCode::SUCCESS;
}

StatusCode T0Dummy::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;

}
