#include <algorithm>
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecVeeVertex.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecVeeVertexCnv.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

EvtRecVeeVertexCnv::EvtRecVeeVertexCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecVeeVertexCol";
   m_adresses.push_back(&m_evtRecVeeVertexCol);
   m_evtRecVeeVertexCol = 0;
}

StatusCode EvtRecVeeVertexCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "EvtRecVeeVertexCnv");
  log << MSG::DEBUG << "EvtRecVeeVertexCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the EvtRecVeeVertex Collection
  EvtRecVeeVertexCol* evtRecVeeVertexCol = new EvtRecVeeVertexCol;
  refpObject = evtRecVeeVertexCol;

  if ( ! m_evtRecVeeVertexCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecVeeVertexCnv" << endreq;
     return sc;
  }

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(dataSvc, EventModel::EvtRec::EvtRecTrackCol);
  if ( ! evtRecTrackCol) {
     log << MSG::INFO << "Could not find EvtRecTrackCol" << endreq;
  }

  // convert
  TIter evtRecVeeVertexIter(m_evtRecVeeVertexCol);
  TEvtRecVeeVertex* evtRecVeeVertexRoot = 0;
  while ( (evtRecVeeVertexRoot = (TEvtRecVeeVertex*)evtRecVeeVertexIter.Next() ) ) {
     EvtRecVeeVertex* evtRecVeeVertex = new EvtRecVeeVertex();
     m_common.m_rootEvtRecVeeVertexMap[evtRecVeeVertexRoot] = evtRecVeeVertex;
     // set ....
     evtRecVeeVertex->setVertexId( evtRecVeeVertexRoot->vertexId() );
     evtRecVeeVertex->setVertexType( evtRecVeeVertexRoot->vertexType() );
     evtRecVeeVertex->setChi2( evtRecVeeVertexRoot->chi2() );
     evtRecVeeVertex->setNdof( evtRecVeeVertexRoot->ndof() );
     evtRecVeeVertex->setMass( evtRecVeeVertexRoot->mass() );

     HepVector wTrackParameter(7, 0);
     for (int i = 0; i < 7; i++) {
       wTrackParameter[i] = evtRecVeeVertexRoot->w(i);
     }
     evtRecVeeVertex->setW( wTrackParameter );

     HepSymMatrix EwTrackParameter(7, 0);
     int i = 0;
     for (int j = 0; j < 7; j++) {
      for (int k = j; k < 7; k++) {
        EwTrackParameter[j][k] = EwTrackParameter[k][j] = evtRecVeeVertexRoot->Ew(i);
        i++;
       }
     }
     evtRecVeeVertex->setEw( EwTrackParameter );

     std::pair<int, int> pairId;
     pairId.first = evtRecVeeVertexRoot->pair(0);
     pairId.second = evtRecVeeVertexRoot->pair(1);
     evtRecVeeVertex->setPair( pairId );
     evtRecVeeVertex->setNCharge( evtRecVeeVertexRoot->nCharge() );
     evtRecVeeVertex->setNTracks( evtRecVeeVertexRoot->nTracks() );
     int evtRecTrackId0 = evtRecVeeVertexRoot->daughter(0);
     if ( evtRecTrackId0 >= 0) {
        evtRecVeeVertex->addDaughter(
            dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(evtRecTrackId0)), 0
              );
     }
     int evtRecTrackId1 = evtRecVeeVertexRoot->daughter(1);
     if ( evtRecTrackId1 >= 0) {
        evtRecVeeVertex->addDaughter(
            dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(evtRecTrackId1)), 1
              );
     }

     evtRecVeeVertexCol->push_back(evtRecVeeVertex);
  }

  delete m_evtRecVeeVertexCol;
  m_evtRecVeeVertexCol = 0;
  return StatusCode::SUCCESS;
}

StatusCode EvtRecVeeVertexCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  MsgStream log(msgSvc(), "EvtRecVeeVertexCnv");
  log << MSG::DEBUG << "EvtRecVeeVertexCnv::DataObjectToTObject" << endreq;

  EvtRecVeeVertexCol* evtRecVeeVertexCol = dynamic_cast<EvtRecVeeVertexCol*> (obj);
  if ( ! evtRecVeeVertexCol ) {
    log << MSG::ERROR << "Could not downcast to EvtRecVeeVertexCol" << endreq;
    return StatusCode::FAILURE;
  }

  DataObject* evt;
  m_eds->findObject(EventModel::EvtRec::Event, evt);
  if ( evt == NULL ) {
     log << MSG::ERROR << "Could not get EvtRecObject in TDS" << endreq;
     return StatusCode::FAILURE;
  }
  EvtRecObject* devtTds = dynamic_cast<EvtRecObject*> (evt);
  if ( ! devtTds ) {
    log << MSG::ERROR << "EvtRecVeeVertexCnv: Could not downcast to TDS EvtRecObject" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
  TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

  const TObjArray* m_evtRecVeeVertexCol = recEvt->getEvtRecVeeVertexCol();
  if ( ! m_evtRecVeeVertexCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecVeeVertexCnv" << endreq;
     return sc;
  }

  EvtRecTrackCol::iterator    evtRecTrackColbegin, evtRecTrackColend;

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(dataSvc, EventModel::EvtRec::EvtRecTrackCol);
  if ( ! evtRecTrackCol) {
     log << MSG::INFO << "Could not find EvtRecTrackCol" << endreq;
  }
  else {
     evtRecTrackColbegin = evtRecTrackCol->begin();
     evtRecTrackColend = evtRecTrackCol->end();
  }

  // convert
  recEvt->clearEvtRecVeeVertexCol();
  EvtRecVeeVertexCol::const_iterator evtRecVeeVertex = evtRecVeeVertexCol->begin();

  for ( ; evtRecVeeVertex != evtRecVeeVertexCol->end(); evtRecVeeVertex++) {
     EvtRecVeeVertex* ptr = *evtRecVeeVertex;
     TEvtRecVeeVertex* evtRecVeeVertexRoot = new TEvtRecVeeVertex();
     // set ...
     evtRecVeeVertexRoot->setVertexId( ptr->vertexId() );
     evtRecVeeVertexRoot->setVertexType( ptr->vertexType() );
     evtRecVeeVertexRoot->setChi2( ptr->chi2() );
     evtRecVeeVertexRoot->setNdof( ptr->ndof() );
     evtRecVeeVertexRoot->setMass( ptr->mass() );

     double wTrackParameter[7];
     for (int i = 0; i < 7; i++) {
       wTrackParameter[i] = ptr->w()[i];
     }
     evtRecVeeVertexRoot->setW( wTrackParameter );
     double EwTrackParameter[28];
     int i = 0;
     for (int j = 0; j < 7; j++) {
      for (int k = j; k < 7; k++) {
        EwTrackParameter[i] = ptr->Ew()[j][k];
        i++;
       }
     }
     assert(i == 28);
     evtRecVeeVertexRoot->setEw( EwTrackParameter );
     int pairId[2];
     pairId[0] = ptr->pair(0);
     pairId[1] = ptr->pair(1);
     evtRecVeeVertexRoot->setPair( pairId );
     evtRecVeeVertexRoot->setNCharge( ptr->nCharge() );
     evtRecVeeVertexRoot->setNTracks( ptr->nTracks() );
     
     int daughterId[2];
     if ( ptr->daughter(0).target() != NULL && ptr->daughter(1).target() != NULL ) {
       assert(evtRecTrackColbegin != evtRecTrackColend);
       EvtRecTrackCol::iterator it0 = find(evtRecTrackColbegin, 
                                         evtRecTrackColend, ptr->daughter(0).target());
       EvtRecTrackCol::iterator it1 = find(evtRecTrackColbegin, 
                                         evtRecTrackColend, ptr->daughter(1).target());
       assert(it0 != evtRecTrackColend);
       assert(it1 != evtRecTrackColend);
       daughterId[0] = it0 - evtRecTrackColbegin;
       daughterId[1] = it1 - evtRecTrackColbegin;
       evtRecVeeVertexRoot->setDaughter( daughterId );
     }
     recEvt->addEvtRecVeeVertex(evtRecVeeVertexRoot);
  }

  return StatusCode::SUCCESS;
}
