#include <algorithm>
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecDTag.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecDTag.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecDTagCnv.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"

#include <vector>


using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

EvtRecDTagCnv::EvtRecDTagCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecDTagCol";
   m_adresses.push_back(&m_evtRecDTagCol);
   m_evtRecDTagCol = 0;
}

StatusCode EvtRecDTagCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "EvtRecDTagCnv");
  log << MSG::DEBUG << "EvtRecDTagCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the EvtRecDTag Collection
  EvtRecDTagCol* evtRecDTagCol = new EvtRecDTagCol;
  refpObject = evtRecDTagCol;

  if ( ! m_evtRecDTagCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecDTagCnv" << endreq;
     return sc;
  }

  SmartDataPtr<EvtRecTrackCol> evtRecTrackCol(dataSvc, EventModel::EvtRec::EvtRecTrackCol);
  if ( ! evtRecTrackCol) {
     log << MSG::INFO << "Could not find EvtRecTrackCol" << endreq;
  }

  // convert
  TIter evtRecDTagIter(m_evtRecDTagCol);
  TEvtRecDTag* evtRecDTagRoot = 0;
  while ( (evtRecDTagRoot = (TEvtRecDTag*)evtRecDTagIter.Next() ) ) {
     EvtRecDTag* evtRecDTag = new EvtRecDTag();
     m_common.m_rootEvtRecDTagMap[evtRecDTagRoot] = evtRecDTag;
     
     // set ....
     evtRecDTag->setdecayMode( (EvtRecDTag::DecayMode)evtRecDTagRoot->decayMode() );
     evtRecDTag->settype( (EvtRecDTag::SelType)evtRecDTagRoot->type() );
     evtRecDTag->setbeamE( evtRecDTagRoot->beamE() );
     evtRecDTag->setmass( evtRecDTagRoot->mass() );
     evtRecDTag->setmBC( evtRecDTagRoot->mBC() );
     evtRecDTag->setdeltaE( evtRecDTagRoot->deltaE() );
     evtRecDTag->setcharge( evtRecDTagRoot->charge() );
     evtRecDTag->setcharm( evtRecDTagRoot->charm() );
     evtRecDTag->setnumOfChildren( evtRecDTagRoot->numOfChildren() );
     
     HepLorentzVector p4(evtRecDTagRoot->px(),evtRecDTagRoot->py(),evtRecDTagRoot->pz(),evtRecDTagRoot->pe());
     evtRecDTag->setp4(p4);
     
     
     const std::vector<int>& tracks=evtRecDTagRoot->tracks();
     const std::vector<int>& showers=evtRecDTagRoot->showers();
     const std::vector<int>& otherTracks=evtRecDTagRoot->otherTracks();
     const std::vector<int>& otherShowers=evtRecDTagRoot->otherShowers();
     const std::vector<int>& pionId=evtRecDTagRoot->pionId();
     const std::vector<int>& kaonId=evtRecDTagRoot->kaonId();
     
     for(unsigned int i=0; i<tracks.size();i++)
       evtRecDTag->addTrack(
			    dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(tracks[i]))
			    );
     
     for(unsigned int i=0; i<showers.size();i++)
       evtRecDTag->addShower(
			    dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(showers[i]))
			    );
     
     
     for(unsigned int i=0; i<otherTracks.size();i++)
       evtRecDTag->addOtherTrack(
				 dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(otherTracks[i]))
				 );
     
          
     for(unsigned int i=0; i<otherShowers.size();i++)
       evtRecDTag->addOtherShower(
				  dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(otherShowers[i]))
				  );
     
     for(unsigned int i=0; i<pionId.size();i++)
       evtRecDTag->addPionId(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(pionId[i]))
			     );
     
     for(unsigned int i=0; i<kaonId.size();i++)
       evtRecDTag->addKaonId(
			     dynamic_cast<EvtRecTrack*>(evtRecTrackCol->containedObject(kaonId[i]))
			     );
          
     
     
     evtRecDTagCol->push_back(evtRecDTag);
  }

  delete m_evtRecDTagCol;
  m_evtRecDTagCol = 0;
  return StatusCode::SUCCESS;
}

StatusCode EvtRecDTagCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  MsgStream log(msgSvc(), "EvtRecDTagCnv");
  log << MSG::DEBUG << "EvtRecDTagCnv::DataObjectToTObject" << endreq;

  EvtRecDTagCol* evtRecDTagCol = dynamic_cast<EvtRecDTagCol*> (obj);
  if ( ! evtRecDTagCol ) {
    log << MSG::ERROR << "Could not downcast to EvtRecDTagCol" << endreq;
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
    log << MSG::ERROR << "EvtRecDTagCnv: Could not downcast to TDS EvtRecObject" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
  TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

  const TObjArray* m_evtRecDTagCol = recEvt->getEvtRecDTagCol();
  if ( ! m_evtRecDTagCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecDTagCnv" << endreq;
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
  recEvt->clearEvtRecDTagCol();
  EvtRecDTagCol::const_iterator evtRecDTag = evtRecDTagCol->begin();

  for ( ; evtRecDTag != evtRecDTagCol->end(); evtRecDTag++) {
     EvtRecDTag* ptr = *evtRecDTag;
     TEvtRecDTag* evtRecDTagRoot = new TEvtRecDTag();

     // set ...
     evtRecDTagRoot->setdecayMode( ptr->decayMode() );
     evtRecDTagRoot->settype( ptr->type() );
     evtRecDTagRoot->setbeamE( ptr->beamE() ); 
     evtRecDTagRoot->setmass( ptr->mass() );
     evtRecDTagRoot->setmBC( ptr->mBC() );
     evtRecDTagRoot->setdeltaE( ptr->deltaE() );
     evtRecDTagRoot->setcharge( ptr->charge() );
     evtRecDTagRoot->setcharm( ptr->charm() );
     evtRecDTagRoot->setnumOfChildren( ptr->numOfChildren() );
     
     HepLorentzVector p4=ptr->p4();
     evtRecDTagRoot->setpx(p4.x());
     evtRecDTagRoot->setpy(p4.y());
     evtRecDTagRoot->setpz(p4.z());
     evtRecDTagRoot->setpe(p4.t());
     
     

     SmartRefVector<EvtRecTrack> tracks=ptr->tracks();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<tracks.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					  evtRecTrackColend, tracks[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addTrack(it - evtRecTrackColbegin );
     }
     

     SmartRefVector<EvtRecTrack> showers=ptr->showers();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<showers.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					  evtRecTrackColend, showers[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addShower(it - evtRecTrackColbegin );
     }
     
     
     SmartRefVector<EvtRecTrack> otherTracks=ptr->otherTracks();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<otherTracks.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					  evtRecTrackColend, otherTracks[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addOtherTrack(it - evtRecTrackColbegin );
     }

     SmartRefVector<EvtRecTrack> otherShowers=ptr->otherShowers();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<otherShowers.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin, 
					  evtRecTrackColend, otherShowers[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addOtherShower(it - evtRecTrackColbegin );
     }

     SmartRefVector<EvtRecTrack> pionId=ptr->pionId();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<pionId.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin,
                                          evtRecTrackColend, pionId[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addPionId(it - evtRecTrackColbegin );
     }

     SmartRefVector<EvtRecTrack> kaonId=ptr->kaonId();
     //  assert(evtRecTrackColbegin != evtRecTrackColend);
     for(unsigned int i=0; i<kaonId.size();i++){
       EvtRecTrackCol::iterator it = find(evtRecTrackColbegin,
                                          evtRecTrackColend, kaonId[i]);
       //assert(it != evtRecTrackColend);
       evtRecDTagRoot->addKaonId(it - evtRecTrackColbegin );
     }
     
     
     recEvt->addEvtRecDTag(evtRecDTagRoot);
  }

  return StatusCode::SUCCESS;
}
