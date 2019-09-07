#include "McEventSelector/McEventCnv.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/StatusCode.h"
#include "McEventSelector/McAddress.h"
#include "McEventSelector/McCnvSvc.h"
#include "EventModel/EventHeader.h"

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<McEventCnv> s_factory;
//const ICnvFactory& McEventCnvFactory = s_factory;

McEventCnv::McEventCnv(ISvcLocator* svcloc) :
  Converter(McCnvSvc::storageType(), classID(), svcloc) { 
}

StatusCode
McEventCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj) {
  McAddress *pMcAddr;
  pMcAddr = dynamic_cast<McAddress*>(pAddr);
 
  Event::EventHeader *eventHeader = new Event::EventHeader;
  eventHeader->setEventNumber(pMcAddr->eventNumber());
  eventHeader->setRunNumber(pMcAddr->runNumber());
  pObj = eventHeader;

  return StatusCode::SUCCESS;    
}


unsigned char McEventCnv::storageType() {
  return McCnvSvc::storageType();
}    

long McEventCnv::repSvcType() const {
  return McCnvSvc::storageType();
}
