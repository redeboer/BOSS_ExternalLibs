#include "MdcDummyAlg/MdcDummy.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "McTruth/DecayMode.h"

/*#include "McTruth/McParticle.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
*/
#include <vector>
#include <iostream>

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

MdcDummy::MdcDummy(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
  declareProperty("FittingMethod", m_fittingMethod = 2);
  declareProperty("ConfigFile",  m_configFile = "MDCConfig.xml");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcDummy::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;


  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcDummy::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Part 1: Get the event header, print out event and run number
  int event, run;
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "MdcDummy: retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;
  
  int digiId;

  //Part 2: Retrieve MC truth 
  SmartDataPtr<DecayMode> decayMode(eventSvc(),"/Event/MC/DecayMode");
  if (!decayMode) {
    log << MSG::FATAL << "Could not find DecayMode" << endreq;
    return( StatusCode::FAILURE);
  }
  unsigned int size = 10;
  int decayId[10];
  decayMode->getData(decayId, size);
 
  for (unsigned int index =0; index<10; index++) {
    log << MSG::INFO 
        << " Decay Mode  = " << decayId[index]
        << endreq;   
  }

  //Part 3: Retrieve MDC digi 
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
  if (!mdcDigiCol) {
    log << MSG::FATAL << "Could not find MDC digi" << endreq;
    return( StatusCode::FAILURE);
  }
 
  MdcDigiCol::iterator iter1 = mdcDigiCol->begin();
  digiId = 0; 
  for (;iter1 != mdcDigiCol->end(); iter1++, digiId++) {
    log << MSG::INFO << "MDC digit No: " << digiId << endreq;
 
    log << MSG::INFO 
        << " time_channel = " << (*iter1)->getTimeChannel()
        << " charge_channel = " << (*iter1)->getChargeChannel() 
        << endreq;   
  }
 
 
  //Part 4: Retrieve TOF digi 
  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
  if (!tofDigiCol) {
    log << MSG::FATAL << "Could not find TOF digi" << endreq;
    return( StatusCode::FAILURE);
  }
 
  TofDigiCol::iterator iter2 = tofDigiCol->begin();
  digiId = 0; 
  for (;iter2 != tofDigiCol->end(); iter2++, digiId++) {
    log << MSG::INFO << "TOF digit No: " << digiId << endreq;

  }
  
  //Part 5: Retrieve EMC digi 
  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
    log << MSG::FATAL << "Could not find EMC digi" << endreq;
    return( StatusCode::FAILURE);
  }
 
  EmcDigiCol::iterator iter3 = emcDigiCol->begin();
  digiId = 0; 
  for (;iter3 != emcDigiCol->end(); iter3++, digiId++) {
    log << MSG::INFO << "Emc digit No: " << digiId << endreq;
 
    log << MSG::INFO 
        << " time_channel = " << (*iter3)->getTimeChannel()
        << " charge_channel = " << (*iter3)->getChargeChannel() 
        << endreq;   
  }
 

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcDummy::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
