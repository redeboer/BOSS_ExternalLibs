#include <algorithm>
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecPi0.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecPi0.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPi0Cnv.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include <vector>


using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

EvtRecPi0Cnv::EvtRecPi0Cnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecPi0Col";
   m_adresses.push_back(&m_evtRecPi0Col);
   m_evtRecPi0Col = 0;
}

StatusCode EvtRecPi0Cnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "EvtRecPi0Cnv");
  log << MSG::DEBUG << "EvtRecPi0Cnv::TObjectToDataObject" << endreq;

  // create the TDS location for the EvtRecPi0 Collection
  EvtRecPi0Col* evtRecPi0Col = new EvtRecPi0Col;
  refpObject = evtRecPi0Col;

  if ( ! m_evtRecPi0Col ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecPi0Cnv" << endreq;
     return sc;
  }

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(dataSvc, EventModel::EvtRec::EvtRecTrackCol);
  if ( ! evtRecTrackCol) {
     log << MSG::INFO << "Could not find EvtRecTrackCol" << endreq;
  }

  // convert
  TIter evtRecPi0Iter(m_evtRecPi0Col);
  TEvtRecPi0* evtRecPi0Root = 0;
  while ( (evtRecPi0Root = (TEvtRecPi0*)evtRecPi0Iter.Next() ) ) {
     EvtRecPi0* evtRecPi0 = new EvtRecPi0();
     m_common.m_rootEvtRecPi0Map[evtRecPi0Root] = evtRecPi0;
     
     // set ....
     evtRecPi0->setUnconMass( evtRecPi0Root->unconMass() );
     evtRecPi0->setChisq( evtRecPi0Root->chisq() );
     
     HepLorentzVector hiP4(evtRecPi0Root->hiPxfit(),evtRecPi0Root->hiPyfit(),evtRecPi0Root->hiPzfit(),evtRecPi0Root->hiPefit());
     HepLorentzVector loP4(evtRecPi0Root->loPxfit(),evtRecPi0Root->loPyfit(),evtRecPi0Root->loPzfit(),evtRecPi0Root->loPefit());
     evtRecPi0->setHiPfit(hiP4);
     evtRecPi0->setLoPfit(loP4);
     
     int hiEnGammatrk=evtRecPi0Root->hiEnGamma();
     int loEnGammatrk=evtRecPi0Root->loEnGamma();
     evtRecPi0->setHiEnGamma(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(hiEnGammatrk))
			    );
     evtRecPi0->setLoEnGamma(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(loEnGammatrk))
			    );
     
     
     evtRecPi0Col->push_back(evtRecPi0);
  }

  delete m_evtRecPi0Col;
  m_evtRecPi0Col = 0;
  return StatusCode::SUCCESS;
}

StatusCode EvtRecPi0Cnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  MsgStream log(msgSvc(), "EvtRecPi0Cnv");
  log << MSG::DEBUG << "EvtRecPi0Cnv::DataObjectToTObject" << endreq;

  EvtRecPi0Col* evtRecPi0Col = dynamic_cast<EvtRecPi0Col*> (obj);
  if ( ! evtRecPi0Col ) {
    log << MSG::ERROR << "Could not downcast to EvtRecPi0Col" << endreq;
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
    log << MSG::ERROR << "EvtRecPi0Cnv: Could not downcast to TDS EvtRecObject" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
  TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

  const TObjArray* m_evtRecPi0Col = recEvt->getEvtRecPi0Col();
  if ( ! m_evtRecPi0Col ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecPi0Cnv" << endreq;
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
  recEvt->clearEvtRecPi0Col();
  EvtRecPi0Col::const_iterator evtRecPi0 = evtRecPi0Col->begin();

  for ( ; evtRecPi0 != evtRecPi0Col->end(); evtRecPi0++) {
     EvtRecPi0* ptr = *evtRecPi0;
     TEvtRecPi0* evtRecPi0Root = new TEvtRecPi0();

     // set ...
     evtRecPi0Root->setUnconMass( ptr->unconMass() );
     evtRecPi0Root->setChisq( ptr->chisq() );
     
     
     HepLorentzVector hiP4=ptr->hiPfit();
     HepLorentzVector loP4=ptr->loPfit();
     evtRecPi0Root->setHiPxfit(hiP4.x());
     evtRecPi0Root->setHiPyfit(hiP4.y());
     evtRecPi0Root->setHiPzfit(hiP4.z());
     evtRecPi0Root->setHiPefit(hiP4.t());
     
     evtRecPi0Root->setLoPxfit(loP4.x());
     evtRecPi0Root->setLoPyfit(loP4.y());
     evtRecPi0Root->setLoPzfit(loP4.z());
     evtRecPi0Root->setLoPefit(loP4.t());
     
     
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					evtRecTrackColend, ptr->hiEnGamma());
     //assert(it != evtRecTrackColend);
     evtRecPi0Root->setHiEnGamma(it - evtRecTrackColbegin );
     
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     it = find(evtRecTrackColbegin, 
					evtRecTrackColend, ptr->loEnGamma() );
     //assert(it != evtRecTrackColend);
     evtRecPi0Root->setLoEnGamma(it - evtRecTrackColbegin );
     
     
     
     recEvt->addEvtRecPi0(evtRecPi0Root);
  }

  return StatusCode::SUCCESS;
}
