// File and Version Information:
// Description:
//      EventCnv is the concrete converter for the event header on the TDS /Event
//

#define EventCnv_CXX 

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"

#include "EventModel/Event.h"
#include "RootCnvSvc/EventCnv.h"
#include "RootCnvSvc/RootCnvSvc.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<EventCnv> s_factory;
//const ICnvFactory& EventCnvFactory = s_factory;

EventCnv::EventCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
// Here we associate this converter with the /Event path on the TDS.
  //declareObject("/Event", objType(), "","");


    MsgStream log(msgSvc(), "EventCnv");
    log << MSG::DEBUG << "EventCnv::EventCnv()" << endreq;
}

StatusCode EventCnv::createObj(IOpaqueAddress *addr, DataObject*& refpObject) {
    // Purpose and Method:  This converter creates an  Event on
    // the TDS. Registering was already done in GlastEventSelector
    // Information is dummy as long as no real datafile has been read
    m_common.clear();
    MsgStream log(msgSvc(), "EventCnv");
    log << MSG::DEBUG << "createObj" << endreq;
    Event::EventH * evh = new Event::EventH();
    refpObject=evh;
    return StatusCode::SUCCESS;
}


//StatusCode EventCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
StatusCode EventCnv::createRep(DataObject* obj, RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "EventCnv");
  log << MSG::DEBUG << "EventCnv::DataObjectToObject" << endreq;

  Event::EventH * evh=dynamic_cast<Event::EventH *> (obj);
  if (!evh) {
       log << MSG::ERROR << "Could not downcast Event" << endreq;
       return StatusCode::FAILURE;
  }
  // nothing to do, will be done in lower converters
  return StatusCode::SUCCESS;
}














