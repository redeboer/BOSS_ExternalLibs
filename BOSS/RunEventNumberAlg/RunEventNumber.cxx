#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "RunEventNumberAlg/RunEventNumber.h"
#include <time.h>

time_t rawtime;


/////////////////////////////////////////////////////////////////////////////


RunEventNumber::RunEventNumber(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("Period", m_period = 0);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RunEventNumber::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode sc;
  
  numEvents = 0;
  numRuns = 0;
  prevRun = 0;

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RunEventNumber::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  numEvents++;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNum=eventHeader->runNumber();
  int eventNum=eventHeader->eventNumber();

  if(runNum != prevRun) {
    numRuns++;
    prevRun = runNum;
  }

  if((m_period != 0)&&(numEvents % m_period == 0)) {
    time ( &rawtime );
    std::cout << " RunEventNumberAlg: Run, event, time = " 
	      << runNum << ", " << eventNum << ", " << asctime(localtime(&rawtime));
  }

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RunEventNumber::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  std::cout << std::endl;
  std::cout << " RunEventNumberAlg: Number of processed runs = " << numRuns << std::endl;
  std::cout << " RunEventNumberAlg: Number of processed events = " << numEvents << std::endl;
  std::cout << std::endl;

  return StatusCode::SUCCESS;
}

