#include <algorithm>
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EvtRecEvent/EvtRecObject.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "RootEventData/TEvtRecObject.h"
#include "RootEventData/TEvtRecTrack.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecTrackCnv.h"

EvtRecTrackCnv::EvtRecTrackCnv(ISvcLocator* svc)
   : RootEventBaseCnv(classID(), svc)
{
   m_rootBranchname = "m_evtRecTrackCol";
   m_adresses.push_back(&m_evtRecTrackCol);
   m_evtRecTrackCol = 0;
}

StatusCode EvtRecTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "EvtRecTrackCnv");
  log << MSG::DEBUG << "EvtRecTrackCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the EvtRecTrack Collection
  EvtRecTrackCol* evtRecTrackCol = new EvtRecTrackCol;
  refpObject = evtRecTrackCol;

  if ( ! m_evtRecTrackCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecTrackCnv" << endreq;
     return sc;
  }

  /*SmartDataPtr<RecMdcTrackCol> mdcTrackCol(dataSvc, EventModel::Recon::RecMdcTrackCol);
  if ( ! mdcTrackCol ) {
     log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
  }

  SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(dataSvc, EventModel::Recon::RecMdcKalTrackCol);
  if ( ! mdcKalTrackCol ) {
     log << MSG::INFO << "Could not find RecMdcKalTrackCol" << endreq;
  }

  SmartDataPtr<RecMdcDedxCol> mdcDedxCol(dataSvc, EventModel::Recon::RecMdcDedxCol);
  if ( ! mdcDedxCol ) {
     log << MSG::INFO << "Could not find RecMdcDedxCol" << endreq;
  }

  SmartDataPtr<RecExtTrackCol> extTrackCol(dataSvc, EventModel::Recon::RecExtTrackCol);
  if ( ! extTrackCol ) {
     log << MSG::INFO << "Could not find RecExtTrackCol" << endreq;
  }

  SmartDataPtr<RecTofTrackCol> tofTrackCol(dataSvc, EventModel::Recon::RecTofTrackCol);
  if ( ! tofTrackCol ) {
     log << MSG::INFO << "Could not find RecTofTrackCol" << endreq;
  }

  SmartDataPtr<RecEmcShowerCol> emcShowerCol(dataSvc, EventModel::Recon::RecEmcShowerCol);
  if ( ! emcShowerCol ) {
     log << MSG::INFO << "Could not find RecEmcShowerCol" << endreq;
  }

  SmartDataPtr<RecMucTrackCol> mucTrackCol(dataSvc, EventModel::Recon::RecMucTrackCol);
  if ( ! mucTrackCol ) {
     log << MSG::INFO << "Could not find RecMucTrackCol" << endreq;
  }*/

  //-----------------Dst to Rec convert in case of no Rec data in TDS
  int trackID;

  // Retrieve Mdc Track 
  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(dataSvc, EventModel::Recon::RecMdcTrackCol);
  if(!mdcTrackCol)
  {
    log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
    SmartDataPtr<DstMdcTrackCol> dstMdcTrackCol(dataSvc,"/Event/Dst/DstMdcTrackCol");
    if (!dstMdcTrackCol) {
      log << MSG::INFO << "Could not find DstMdcTrackCol" << endreq;
    } 
    else {
      RecMdcTrackCol* mdcTrackCol = new RecMdcTrackCol();
      DstMdcTrackCol::iterator iter_mdc = dstMdcTrackCol->begin();
      trackID = 0;
      for (;iter_mdc != dstMdcTrackCol->end(); iter_mdc++, trackID++) {
        RecMdcTrack* recMdcTrack = new RecMdcTrack(); 
        *recMdcTrack = **iter_mdc;
        (*mdcTrackCol).push_back(recMdcTrack);
        log << MSG::INFO 
          	<< " Mdc Track ID = " << trackID
	          << " Mdc Track Nster = " << (*iter_mdc)->nster()
	          << endreq; 
      } 
      sc = dataSvc->registerObject(EventModel::Recon::RecMdcTrackCol,mdcTrackCol);
    }
  }

  // Retrieve MdcKal track 
  SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(dataSvc, EventModel::Recon::RecMdcKalTrackCol);
  if (!mdcKalTrackCol) {
    log << MSG::INFO << "Could not find RecMdcKalTrackCol" << endreq;
    SmartDataPtr<DstMdcKalTrackCol> dstMdcKalTrackCol(dataSvc,"/Event/Dst/DstMdcKalTrackCol");
    if(!dstMdcKalTrackCol) {
      log << MSG::INFO << "Could not find DstMdcKalTrackCol" << endreq;
    }
    else {
      RecMdcKalTrackCol* mdcKalTrackCol = new RecMdcKalTrackCol();
      DstMdcKalTrackCol::iterator iter_mdc = dstMdcKalTrackCol->begin();
      trackID = 0;
      for (;iter_mdc != dstMdcKalTrackCol->end(); iter_mdc++, trackID++) {
        RecMdcKalTrack* recMdcKalTrack = new RecMdcKalTrack();
        *recMdcKalTrack = **iter_mdc;
        (*mdcKalTrackCol).push_back(recMdcKalTrack);
        log << MSG::INFO
            << " MdcKalTrack ID = " << trackID
            << " MdcKalTrack Nster = " << (*iter_mdc)->nster()
            << " MdcKalTrack poca = " << (*iter_mdc)->poca()
            << endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecMdcKalTrackCol, mdcKalTrackCol);	
    }
  }

  // Retrieve MdcDedx 
  SmartDataPtr<RecMdcDedxCol> mdcDedxCol(dataSvc, EventModel::Recon::RecMdcDedxCol);
  if (!mdcDedxCol) {
    log <<MSG::INFO << "Could not find RecMdcDedxCol" << endreq;
    SmartDataPtr<DstMdcDedxCol> dstMdcDedxCol(dataSvc,"/Event/Dst/DstMdcDedxCol");
    if(!dstMdcDedxCol) {
      log <<MSG::INFO << "Could not find DstMdcDedxCol" << endreq;
    }
    else {
      RecMdcDedxCol* mdcDedxCol = new RecMdcDedxCol();
      DstMdcDedxCol::iterator iter_mdc = dstMdcDedxCol->begin();
      trackID=0;
      for (;iter_mdc != dstMdcDedxCol->end(); iter_mdc++, trackID++) {
        RecMdcDedx* recMdcDedx = new RecMdcDedx();
        *recMdcDedx= **iter_mdc;
        (*mdcDedxCol).push_back(recMdcDedx);
        log << MSG::INFO
            << " MdcDedx ID = " << trackID
            << " MdcDedx " 
            << endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecMdcDedxCol, mdcDedxCol);
    }
  } 
  
  // Retrieve Ext track 
  SmartDataPtr<RecExtTrackCol> extTrackCol(dataSvc, EventModel::Recon::RecExtTrackCol);
  if (!extTrackCol) {
    log <<MSG::INFO << "Could not find RecExtTrackCol" << endreq;
    SmartDataPtr<DstExtTrackCol> dstExtTrackCol(dataSvc,"/Event/Dst/DstExtTrackCol");
    if(!dstExtTrackCol) {
      log <<MSG::INFO << "Could not find DstExtTrackCol" << endreq;
    }
    else {
      RecExtTrackCol* extTrackCol = new RecExtTrackCol();
      DstExtTrackCol::iterator iter_ext = dstExtTrackCol->begin();
      trackID=0;
      for (;iter_ext != dstExtTrackCol->end(); iter_ext++, trackID++) {
        RecExtTrack* recExtTrack = new RecExtTrack();
        *recExtTrack = **iter_ext;
        (*extTrackCol).push_back(recExtTrack);
        log << MSG::INFO
            << " ExtTrack ID = " << trackID
            << endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecExtTrackCol, extTrackCol);
    }
  }
      
  // Retrieve Tof Track 
  SmartDataPtr<RecTofTrackCol> tofTrackCol(dataSvc, EventModel::Recon::RecTofTrackCol);
  if ( !tofTrackCol ) {
    log << MSG::INFO << "Could not find TofTrackCol" << endreq;
    SmartDataPtr<DstTofTrackCol> dstTofTrackCol(dataSvc,"/Event/Dst/DstTofTrackCol");
    if(!dstTofTrackCol) {
      log << MSG::INFO << "Could not find DstTofTrackCol" << endreq;
    }
    else {
      RecTofTrackCol* tofTrackCol = new RecTofTrackCol();
      DstTofTrackCol::iterator iter_tof = dstTofTrackCol->begin();
      for (;iter_tof != dstTofTrackCol->end(); iter_tof++) {
        RecTofTrack* recTofTrack = new RecTofTrack();
        *recTofTrack = **iter_tof;
        (*tofTrackCol).push_back(recTofTrack);
        log << MSG::INFO
	    << "Tof Track ID = " << (*iter_tof)->tofTrackID()
	    << " Track ID = " << (*iter_tof)->trackID()
	    << " Tof Counter ID = "<<(*iter_tof)->tofID()
	    << " Quality = "<< (*iter_tof)->quality()
            << endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecTofTrackCol, tofTrackCol);
    }
  }


  // Retrieve Emc shower 
  SmartDataPtr<RecEmcShowerCol> emcShowerCol(dataSvc, EventModel::Recon::RecEmcShowerCol);
  if (!emcShowerCol) {
    log << MSG::INFO << "Could not find RecEmcShowerCol" << endreq;
    SmartDataPtr<DstEmcShowerCol> dstEmcShowerCol(dataSvc,"/Event/Dst/DstEmcShowerCol");
    if(!dstEmcShowerCol) {
      log << MSG::INFO << "Could not find DstEmcShowerCol" << endreq;
    }
    else {
      RecEmcShowerCol* emcShowerCol = new RecEmcShowerCol();
      DstEmcShowerCol::iterator iter_emc = dstEmcShowerCol->begin();
      trackID=0;
      for (;iter_emc != dstEmcShowerCol->end(); iter_emc++, trackID++) {
        RecEmcShower* recEmcShower = new RecEmcShower();
        *recEmcShower = **iter_emc;
        (*emcShowerCol).push_back(recEmcShower);
        log << MSG::INFO
            << " EmcShower ID = " << trackID
            << " EmcShower energy = " << (*iter_emc)->energy()
            << endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecEmcShowerCol, emcShowerCol);
    }
  }

  // Retrieve Muc Track 
  SmartDataPtr<RecMucTrackCol> mucTrackCol(dataSvc, EventModel::Recon::RecMucTrackCol);
  if (!mucTrackCol) {
    log <<MSG::INFO << "Could not find RecMucTrackCol" << endreq;
    SmartDataPtr<DstMucTrackCol> dstMucTrackCol(dataSvc,"/Event/Dst/DstMucTrackCol");
    if(!dstMucTrackCol) {
      log <<MSG::INFO << "Could not find DstMucTrackCol" << endreq;
    }
    else {
      RecMucTrackCol* mucTrackCol = new RecMucTrackCol();
      DstMucTrackCol::iterator iter_muc = dstMucTrackCol->begin();
      trackID=0;
      for (;iter_muc != dstMucTrackCol->end(); iter_muc++, trackID++) {
        RecMucTrack* recMucTrack = new RecMucTrack();
        *recMucTrack = **iter_muc;
        (*mucTrackCol).push_back(recMucTrack);
        log << MSG::INFO
            << " MucTrack ID = "<< trackID
            <<endreq;
      }
      sc = dataSvc->registerObject(EventModel::Recon::RecMucTrackCol, mucTrackCol);
    }
  }

  // convert
  TIter evtRecTrackIter(m_evtRecTrackCol);
  TEvtRecTrack* evtRecTrackRoot = 0;
  while ( (evtRecTrackRoot = (TEvtRecTrack*)evtRecTrackIter.Next() ) ) {
     EvtRecTrack* evtRecTrack = new EvtRecTrack();
     m_common.m_rootEvtRecTrackMap[evtRecTrackRoot] = evtRecTrack;
     // set ....
     evtRecTrack->setTrackId( evtRecTrackRoot->trackId() );
     evtRecTrack->setPartId( evtRecTrackRoot->partId() );
     evtRecTrack->setQuality( evtRecTrackRoot->quality() );

     int mdcTrackId = evtRecTrackRoot->mdcTrackId();
     if ( mdcTrackId >= 0 ) {
	evtRecTrack->setMdcTrack(
	      dynamic_cast<RecMdcTrack*>(mdcTrackCol->containedObject(mdcTrackId))
	      );
     }

     int mdcKalTrackId = evtRecTrackRoot->mdcKalTrackId();
     if ( mdcKalTrackId >= 0 ) {
	evtRecTrack->setMdcKalTrack(
	      dynamic_cast<RecMdcKalTrack*>(mdcKalTrackCol->containedObject(mdcKalTrackId))
	      );
     }

     int mdcDedxId = evtRecTrackRoot->mdcDedxId();
     if ( mdcDedxId >= 0 ) {
	evtRecTrack->setMdcDedx(
	      dynamic_cast<RecMdcDedx*>(mdcDedxCol->containedObject(mdcDedxId))
	      );
     }

     int extTrackId = evtRecTrackRoot->extTrackId();
     if ( extTrackId >= 0 ) {
	evtRecTrack->setExtTrack(
	      dynamic_cast<RecExtTrack*>(extTrackCol->containedObject(extTrackId))
	      );
     }

     const std::vector<int>& tofTrackIds = evtRecTrackRoot->tofTrackIds();
     for ( unsigned int i = 0; i < tofTrackIds.size(); i++) {
	RecTofTrack* tofTrack = dynamic_cast<RecTofTrack*>(tofTrackCol->containedObject(tofTrackIds[i]));
	evtRecTrack->addTofTrack(tofTrack);
     }

     int emcShowerId = evtRecTrackRoot->emcShowerId();
     if ( emcShowerId >= 0 ) {
	evtRecTrack->setEmcShower(
	      dynamic_cast<RecEmcShower*>(emcShowerCol->containedObject(emcShowerId))
	      );
     }

     int mucTrackId = evtRecTrackRoot->mucTrackId();
     if ( mucTrackId >= 0 ) {
	evtRecTrack->setMucTrack(
	      dynamic_cast<RecMucTrack*>(mucTrackCol->containedObject(mucTrackId))
	      );
     }

     evtRecTrackCol->push_back(evtRecTrack);
  }

  delete m_evtRecTrackCol;
  m_evtRecTrackCol = 0;
  return StatusCode::SUCCESS;
}

StatusCode EvtRecTrackCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  MsgStream log(msgSvc(), "EvtRecTrackCnv");
  log << MSG::DEBUG << "EvtRecTrackCnv::DataObjectToTObject" << endreq;

  EvtRecTrackCol* evtRecTrackCol = dynamic_cast<EvtRecTrackCol*> (obj);
  if ( ! evtRecTrackCol ) {
    log << MSG::ERROR << "Could not downcast to EvtRecTrackCol" << endreq;
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
    log << MSG::ERROR << "EvtRecTrackCnv: Could not downcast to TDS EvtRecObject" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getEvtRecCnv()->createRep(evt, addr);
  TEvtRecObject* recEvt = m_cnvSvc->getEvtRecCnv()->getWriteObject();

  const TObjArray* m_evtRecTrackCol = recEvt->getEvtRecTrackCol();
  if ( ! m_evtRecTrackCol ) return StatusCode::SUCCESS;

  IDataProviderSvc* dataSvc = 0;
  sc = serviceLocator()->getService("EventDataSvc",
	IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if ( sc.isFailure() ) {
     log << MSG::FATAL << "Could not get EventDataSvc in EvtRecTrackCnv" << endreq;
     return sc;
  }

  RecMdcTrackCol::iterator    mdcTrackColbegin,    mdcTrackColend;
  RecMdcKalTrackCol::iterator mdcKalTrackColbegin, mdcKalTrackColend;
  RecMdcDedxCol::iterator     mdcDedxColbegin,     mdcDedxColend;
  RecExtTrackCol::iterator    extTrackColbegin,    extTrackColend;
  RecTofTrackCol::iterator    tofTrackColbegin,    tofTrackColend;
  RecEmcShowerCol::iterator   emcShowerColbegin,   emcShowerColend;
  RecMucTrackCol::iterator    mucTrackColbegin,    mucTrackColend;

  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(dataSvc, EventModel::Recon::RecMdcTrackCol);
  if ( ! mdcTrackCol ) {
     log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
  }
  else {
     mdcTrackColbegin = mdcTrackCol->begin();
     mdcTrackColend   = mdcTrackCol->end();
  }

  SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(dataSvc, EventModel::Recon::RecMdcKalTrackCol);
  if ( ! mdcKalTrackCol ) {
     log << MSG::INFO << "Could not find RecMdcKalTrackCol" << endreq;
  }
  else {
     mdcKalTrackColbegin = mdcKalTrackCol->begin();
     mdcKalTrackColend   = mdcKalTrackCol->end();
  }

  SmartDataPtr<RecMdcDedxCol> mdcDedxCol(dataSvc, EventModel::Recon::RecMdcDedxCol);
  if ( ! mdcDedxCol ) {
     log << MSG::INFO << "Could not find RecMdcDedxCol" << endreq;
  }
  else {
     mdcDedxColbegin = mdcDedxCol->begin();
     mdcDedxColend   = mdcDedxCol->end();
  }

  SmartDataPtr<RecExtTrackCol> extTrackCol(dataSvc, EventModel::Recon::RecExtTrackCol);
  if ( ! extTrackCol ) {
     log << MSG::INFO << "Could not find RecExtTrackCol" << endreq;
  }
  else {
     extTrackColbegin = extTrackCol->begin();
     extTrackColend   = extTrackCol->end();
  }

  SmartDataPtr<RecTofTrackCol> tofTrackCol(dataSvc, EventModel::Recon::RecTofTrackCol);
  if ( ! tofTrackCol ) {
     log << MSG::INFO << "Could not find RecTofTrackCol" << endreq;
  }
  else {
     tofTrackColbegin = tofTrackCol->begin();
     tofTrackColend   = tofTrackCol->end();
  }

  SmartDataPtr<RecEmcShowerCol> emcShowerCol(dataSvc, EventModel::Recon::RecEmcShowerCol);
  if ( ! emcShowerCol ) {
     log << MSG::INFO << "Could not find RecEmcShowerCol" << endreq;
  }
  else {
     emcShowerColbegin = emcShowerCol->begin();
     emcShowerColend   = emcShowerCol->end();
  }

  SmartDataPtr<RecMucTrackCol> mucTrackCol(dataSvc, EventModel::Recon::RecMucTrackCol);
  if ( ! mucTrackCol ) {
     log << MSG::INFO << "Could not find RecMucTrackCol" << endreq;
  }
  else {
     mucTrackColbegin = mucTrackCol->begin();
     mucTrackColend   = mucTrackCol->end();
  }

  // convert
  recEvt->clearEvtRecTrackCol();
  EvtRecTrackCol::const_iterator evtRecTrack = evtRecTrackCol->begin();

  for ( ; evtRecTrack != evtRecTrackCol->end(); evtRecTrack++) {
     EvtRecTrack* ptr = *evtRecTrack;
     TEvtRecTrack* evtRecTrackRoot = new TEvtRecTrack();
     // set ...
     evtRecTrackRoot->setTrackId( ptr->trackId() );
     evtRecTrackRoot->setPartId( ptr->partId() );
     evtRecTrackRoot->setQuality( ptr->quality() );

     if ( ptr->isMdcTrackValid() ) {
	RecMdcTrackCol::iterator it = find(mdcTrackColbegin, mdcTrackColend, ptr->mdcTrack());
	evtRecTrackRoot->setMdcTrackId( it - mdcTrackColbegin );
     }
     if ( ptr->isMdcKalTrackValid() ) {
	RecMdcKalTrackCol::iterator it = find(mdcKalTrackColbegin, mdcKalTrackColend, ptr->mdcKalTrack());
	evtRecTrackRoot->setMdcKalTrackId( it - mdcKalTrackColbegin );
     }
     if ( ptr->isMdcDedxValid() ) {
	RecMdcDedxCol::iterator it = find(mdcDedxColbegin, mdcDedxColend, ptr->mdcDedx());
	evtRecTrackRoot->setMdcDedxId( it - mdcDedxColbegin );
     }
     if ( ptr->isExtTrackValid() ) {
	RecExtTrackCol::iterator it = find(extTrackColbegin, extTrackColend, ptr->extTrack());
	evtRecTrackRoot->setExtTrackId( it - extTrackColbegin );
     }
     if ( ptr->isTofTrackValid() ) {
	SmartRefVector<RecTofTrack> tofTrackVec = ptr->tofTrack();
	for ( unsigned int i = 0; i < tofTrackVec.size(); i++ ) {
	   RecTofTrackCol::iterator it = find(tofTrackColbegin, tofTrackColend, tofTrackVec[i]);
	   evtRecTrackRoot->addTofTrackId( it - tofTrackColbegin );
	}
     }
     if ( ptr->isEmcShowerValid() ) {
	RecEmcShowerCol::iterator it = find(emcShowerColbegin, emcShowerColend, ptr->emcShower());
	evtRecTrackRoot->setEmcShowerId( it - emcShowerColbegin );
     }
     if ( ptr->isMucTrackValid() ) {
	RecMucTrackCol::iterator it = find(mucTrackColbegin, mucTrackColend, ptr->mucTrack());
	evtRecTrackRoot->setMucTrackId( it - mucTrackColbegin );
     }

     recEvt->addEvtRecTrack(evtRecTrackRoot);
  }

  return StatusCode::SUCCESS;
}
