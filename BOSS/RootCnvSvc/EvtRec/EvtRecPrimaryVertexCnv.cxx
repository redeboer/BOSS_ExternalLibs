#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecPrimaryVertex.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecPrimaryVertex.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPrimaryVertexCnv.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

EvtRecPrimaryVertexCnv::EvtRecPrimaryVertexCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecPrimaryVertex";
   m_adresses.push_back(&m_evtRecPrimaryVertex);
   m_evtRecPrimaryVertex = 0;
}

StatusCode EvtRecPrimaryVertexCnv::TObjectToDataObject(DataObject*& refpObject) {
   MsgStream log(msgSvc(), "EvtRecPrimaryVertexCnv");
   log << MSG::DEBUG << "EvtRecPrimaryVertexCnv::TObjectToDataObject" << endreq;

   // create the TDS location for the EvtRecPrimaryVertex
   EvtRecPrimaryVertex* evtRecPrimaryVertex = new EvtRecPrimaryVertex;
   refpObject = evtRecPrimaryVertex;

   if ( ! m_evtRecPrimaryVertex ) return StatusCode::SUCCESS;
   // convert
   evtRecPrimaryVertex->setIsValid (m_evtRecPrimaryVertex->isValid() );
   evtRecPrimaryVertex->setNTracks (m_evtRecPrimaryVertex->nTracks() );
   evtRecPrimaryVertex->setTrackIdList (m_evtRecPrimaryVertex->trackIdList() );
   evtRecPrimaryVertex->setChi2    (m_evtRecPrimaryVertex->chi2() );
   evtRecPrimaryVertex->setNdof    (m_evtRecPrimaryVertex->ndof() );
   evtRecPrimaryVertex->setFitMethod (m_evtRecPrimaryVertex->fitMethod() );

   HepVector vtx(3, 0);
   for (Int_t i = 0; i < 3; i++) {
     vtx[i] = m_evtRecPrimaryVertex->vertex(i);
   }

   HepSymMatrix Evtx(3, 0);
   Evtx[0][0] = m_evtRecPrimaryVertex->errorVertex(0);
   Evtx[0][1] = Evtx[1][0] = m_evtRecPrimaryVertex->errorVertex(1);
   Evtx[0][2] = Evtx[2][0] = m_evtRecPrimaryVertex->errorVertex(2);
   Evtx[1][1] = m_evtRecPrimaryVertex->errorVertex(3);
   Evtx[1][2] = Evtx[2][1] = m_evtRecPrimaryVertex->errorVertex(4);
   Evtx[2][2] = m_evtRecPrimaryVertex->errorVertex(5);

   evtRecPrimaryVertex->setVertex  (vtx);
   evtRecPrimaryVertex->setErrorVertex (Evtx);

   delete m_evtRecPrimaryVertex;
   m_evtRecPrimaryVertex = 0;
   return StatusCode::SUCCESS;
}

StatusCode EvtRecPrimaryVertexCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {

   MsgStream log(msgSvc(), "EvtRecPrimaryVertexCnv");
   log << MSG::DEBUG << "EvtRecPrimaryVertexCnv::DataObjectToTObject" << endreq;

   EvtRecPrimaryVertex* evtRecPrimaryVertex = dynamic_cast<EvtRecPrimaryVertex*> (obj);
   if ( ! evtRecPrimaryVertex ) {
      log << MSG::ERROR << "Could not downcast to EvtRecPrimaryVertex" << endreq;
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
      log << MSG::ERROR << "EvtRecPrimaryVertexCnv: Could not downcast to TDS EvtRecObject" << endreq;
   }

   IOpaqueAddress* addr;
   m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
   TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

   //recEvt->clearEvtRecPrimaryVertex();

   TEvtRecPrimaryVertex* m_evtRecPrimaryVertex = const_cast<TEvtRecPrimaryVertex*>( recEvt->getEvtRecPrimaryVertex() );

   m_evtRecPrimaryVertex->setIsValid (evtRecPrimaryVertex->isValid() );
   m_evtRecPrimaryVertex->setNTracks (evtRecPrimaryVertex->nTracks() );
   m_evtRecPrimaryVertex->setTrackIdList (evtRecPrimaryVertex->trackIdList() );
   m_evtRecPrimaryVertex->setChi2    (evtRecPrimaryVertex->chi2() );
   m_evtRecPrimaryVertex->setNdof    (evtRecPrimaryVertex->ndof() );
   m_evtRecPrimaryVertex->setFitMethod (evtRecPrimaryVertex->fitMethod() );

   Double_t vtx[3];
   for (Int_t i = 0; i < 3; i++) {
     vtx[i] = evtRecPrimaryVertex->vertex()[i];
   }
   Double_t Evtx[6];
   Evtx[0] = evtRecPrimaryVertex->errorVertex()[0][0];
   Evtx[1] = evtRecPrimaryVertex->errorVertex()[0][1];
   Evtx[2] = evtRecPrimaryVertex->errorVertex()[0][2];
   Evtx[3] = evtRecPrimaryVertex->errorVertex()[1][1];
   Evtx[4] = evtRecPrimaryVertex->errorVertex()[1][2];
   Evtx[5] = evtRecPrimaryVertex->errorVertex()[2][2];
   m_evtRecPrimaryVertex->setVertex  (vtx);
   m_evtRecPrimaryVertex->setErrorVertex (Evtx);

   return StatusCode::SUCCESS;
}
