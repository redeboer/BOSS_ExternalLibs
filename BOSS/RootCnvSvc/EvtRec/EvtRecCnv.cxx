#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "RootEventData/TEvtRecObject.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"

TEvtRecObject* EvtRecCnv::m_objWrite = 0;

EvtRecCnv::EvtRecCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "";
}

StatusCode EvtRecCnv::initialize() {
   StatusCode status = RootEventBaseCnv::initialize();

   m_cnvSvc->setEvtRecCnv(this);

   return status;
}

StatusCode EvtRecCnv::TObjectToDataObject(DataObject*& refpObject) {
   MsgStream log(msgSvc(), "EvtRecCnv");
   log << MSG::DEBUG << "EvtRecCnv::TObjectToDataObject" << endreq;

   // create the TDS location for the EvtRecObject
   EvtRecObject* evtRecObject = new EvtRecObject;
   refpObject = evtRecObject;

   return StatusCode::SUCCESS;
}

StatusCode EvtRecCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {

   MsgStream log(msgSvc(), "EvtRecCnv");
   log << MSG::DEBUG << "EvtRecCnv::DataObjectToTObject" << endreq;

   if ( m_common.m_evtRecObject != NULL ) return StatusCode::SUCCESS;

   EvtRecObject* evtRecObject = dynamic_cast<EvtRecObject*> (obj);
   if ( ! evtRecObject ) {
      log << MSG::ERROR << "Could not downcast to EvtRecObject" << endreq;
      return StatusCode::FAILURE;
   }

   m_objWrite = new TEvtRecObject();
   m_common.m_evtRecObject = m_objWrite;

   // create branch if not yet done
   if ( m_branchNrEvtRec < 0 ) {
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TEvtRecObject",m_objWrite->ClassName(),&m_objWrite,m_branchNrEvtRec);

      if ( sc.isFailure() ) {
        cout<<"Could not create branch TEvtRecObject"<<endl;
	log << MSG::ERROR << "Could not create branch TEvtRecObject" << endreq;
	return sc;
      }
   }

   m_objWrite->initialize();

   return StatusCode::SUCCESS;
}
