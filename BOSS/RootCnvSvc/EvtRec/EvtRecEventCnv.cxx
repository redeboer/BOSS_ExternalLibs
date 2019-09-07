#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEventCnv.h"

EvtRecEventCnv::EvtRecEventCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecEvent";
   m_adresses.push_back(&m_evtRecEvent);
   m_evtRecEvent = 0;
}

StatusCode EvtRecEventCnv::TObjectToDataObject(DataObject*& refpObject) {
   MsgStream log(msgSvc(), "EvtRecEventCnv");
   log << MSG::DEBUG << "EvtRecEventCnv::TObjectToDataObject" << endreq;

   // create the TDS location for the EvtRecEvent
   EvtRecEvent* evtRecEvent = new EvtRecEvent;
   refpObject = evtRecEvent;

   if ( ! m_evtRecEvent ) return StatusCode::SUCCESS;
   // convert
   evtRecEvent->setTotalTracks ( m_evtRecEvent->totalTracks() );
   evtRecEvent->setTotalCharged( m_evtRecEvent->totalCharged() );
   evtRecEvent->setTotalNeutral( m_evtRecEvent->totalNeutral() );
   evtRecEvent->setNumberOfVee ( m_evtRecEvent->numberOfVee() );
   evtRecEvent->setNumberOfPi0 ( m_evtRecEvent->numberOfPi0() );

   delete m_evtRecEvent;
   m_evtRecEvent = 0;

   return StatusCode::SUCCESS;
}

StatusCode EvtRecEventCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {

   MsgStream log(msgSvc(), "EvtRecEventCnv");
   log << MSG::DEBUG << "EvtRecEventCnv::DataObjectToTObject" << endreq;

   EvtRecEvent* evtRecEvent = dynamic_cast<EvtRecEvent*> (obj);
   if ( ! evtRecEvent ) {
      log << MSG::ERROR << "Could not downcast to EvtRecEvent" << endreq;
      return StatusCode::FAILURE;
   }

   DataObject* evt = NULL;
   StatusCode sc = m_eds->findObject(EventModel::EvtRec::Event, evt);
   if ( sc.isFailure() ) {
      log << MSG::ERROR << "Could not get EvtRecObject in TDS" << endreq;
      return StatusCode::FAILURE;
   }

   EvtRecObject* devtTds = dynamic_cast<EvtRecObject*> (evt);
   if ( ! devtTds ) {
      log << MSG::ERROR << "EvtRecEventCnv: Could not downcast to TDS EvtRecObject" << endreq;
   }

   IOpaqueAddress* addr;
   m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
   TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

   TEvtRecEvent* m_evtRecEvent = const_cast<TEvtRecEvent*>( recEvt->getEvtRecEvent() );

   m_evtRecEvent->setTotalTracks ( evtRecEvent->totalTracks() );
   m_evtRecEvent->setTotalCharged( evtRecEvent->totalCharged() );
   m_evtRecEvent->setTotalNeutral( evtRecEvent->totalNeutral() );
   m_evtRecEvent->setNumberOfVee ( evtRecEvent->numberOfVee() );
   m_evtRecEvent->setNumberOfPi0 ( evtRecEvent->numberOfPi0() );

   return StatusCode::SUCCESS;
}
