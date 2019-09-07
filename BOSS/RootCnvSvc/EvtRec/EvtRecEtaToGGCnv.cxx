#include <algorithm>
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecEtaToGG.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecEtaToGG.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEtaToGGCnv.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include <vector>


using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

EvtRecEtaToGGCnv::EvtRecEtaToGGCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecEtaToGGCol";
   m_adresses.push_back(&m_evtRecEtaToGGCol);
   m_evtRecEtaToGGCol = 0;
}

StatusCode EvtRecEtaToGGCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "EvtRecEtaToGGCnv");
  log << MSG::DEBUG << "EvtRecEtaToGGCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the EvtRecEtaToGG Collection
  EvtRecEtaToGGCol* evtRecEtaToGGCol = new EvtRecEtaToGGCol;
  refpObject = evtRecEtaToGGCol;

  if ( ! m_evtRecEtaToGGCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecEtaToGGCnv" << endreq;
     return sc;
  }

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(dataSvc, EventModel::EvtRec::EvtRecTrackCol);
  if ( ! evtRecTrackCol) {
     log << MSG::INFO << "Could not find EvtRecTrackCol" << endreq;
  }

  // convert
  TIter evtRecEtaToGGIter(m_evtRecEtaToGGCol);
  TEvtRecEtaToGG* evtRecEtaToGGRoot = 0;
  while ( (evtRecEtaToGGRoot = (TEvtRecEtaToGG*)evtRecEtaToGGIter.Next() ) ) {
     EvtRecEtaToGG* evtRecEtaToGG = new EvtRecEtaToGG();
     m_common.m_rootEvtRecEtaToGGMap[evtRecEtaToGGRoot] = evtRecEtaToGG;
     
     // set ....
     evtRecEtaToGG->setUnconMass( evtRecEtaToGGRoot->unconMass() );
     evtRecEtaToGG->setChisq( evtRecEtaToGGRoot->chisq() );
     
     HepLorentzVector hiP4(evtRecEtaToGGRoot->hiPxfit(),evtRecEtaToGGRoot->hiPyfit(),evtRecEtaToGGRoot->hiPzfit(),evtRecEtaToGGRoot->hiPefit());
     HepLorentzVector loP4(evtRecEtaToGGRoot->loPxfit(),evtRecEtaToGGRoot->loPyfit(),evtRecEtaToGGRoot->loPzfit(),evtRecEtaToGGRoot->loPefit());
     evtRecEtaToGG->setHiPfit(hiP4);
     evtRecEtaToGG->setLoPfit(loP4);
     
     int hiEnGammatrk=evtRecEtaToGGRoot->hiEnGamma();
     int loEnGammatrk=evtRecEtaToGGRoot->loEnGamma();
     evtRecEtaToGG->setHiEnGamma(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(hiEnGammatrk))
			    );
     evtRecEtaToGG->setLoEnGamma(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(loEnGammatrk))
			    );
     
     
     evtRecEtaToGGCol->push_back(evtRecEtaToGG);
  }

  delete m_evtRecEtaToGGCol;
  m_evtRecEtaToGGCol = 0;
  return StatusCode::SUCCESS;
}

StatusCode EvtRecEtaToGGCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  MsgStream log(msgSvc(), "EvtRecEtaToGGCnv");
  log << MSG::DEBUG << "EvtRecEtaToGGCnv::DataObjectToTObject" << endreq;

  EvtRecEtaToGGCol* evtRecEtaToGGCol = dynamic_cast<EvtRecEtaToGGCol*> (obj);
  if ( ! evtRecEtaToGGCol ) {
    log << MSG::ERROR << "Could not downcast to EvtRecEtaToGGCol" << endreq;
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
    log << MSG::ERROR << "EvtRecEtaToGGCnv: Could not downcast to TDS EvtRecObject" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
  TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

  const TObjArray* m_evtRecEtaToGGCol = recEvt->getEvtRecEtaToGGCol();
  if ( ! m_evtRecEtaToGGCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecEtaToGGCnv" << endreq;
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
  recEvt->clearEvtRecEtaToGGCol();
  EvtRecEtaToGGCol::const_iterator evtRecEtaToGG = evtRecEtaToGGCol->begin();

  for ( ; evtRecEtaToGG != evtRecEtaToGGCol->end(); evtRecEtaToGG++) {
     EvtRecEtaToGG* ptr = *evtRecEtaToGG;
     TEvtRecEtaToGG* evtRecEtaToGGRoot = new TEvtRecEtaToGG();

     // set ...
     evtRecEtaToGGRoot->setUnconMass( ptr->unconMass() );
     evtRecEtaToGGRoot->setChisq( ptr->chisq() );
     
     
     HepLorentzVector hiP4=ptr->hiPfit();
     HepLorentzVector loP4=ptr->loPfit();
     evtRecEtaToGGRoot->setHiPxfit(hiP4.x());
     evtRecEtaToGGRoot->setHiPyfit(hiP4.y());
     evtRecEtaToGGRoot->setHiPzfit(hiP4.z());
     evtRecEtaToGGRoot->setHiPefit(hiP4.t());
     
     evtRecEtaToGGRoot->setLoPxfit(loP4.x());
     evtRecEtaToGGRoot->setLoPyfit(loP4.y());
     evtRecEtaToGGRoot->setLoPzfit(loP4.z());
     evtRecEtaToGGRoot->setLoPefit(loP4.t());
     
     
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					evtRecTrackColend, ptr->hiEnGamma());
     //assert(it != evtRecTrackColend);
     evtRecEtaToGGRoot->setHiEnGamma(it - evtRecTrackColbegin );
     
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     it = find(evtRecTrackColbegin, 
					evtRecTrackColend, ptr->loEnGamma() );
     //assert(it != evtRecTrackColend);
     evtRecEtaToGGRoot->setLoEnGamma(it - evtRecTrackColbegin );
     
     
     
     recEvt->addEvtRecEtaToGG(evtRecEtaToGGRoot);
  }

  return StatusCode::SUCCESS;
}
