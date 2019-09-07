#include "RecMakerAlg/RecMakerAlg.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TofRecEvent/RecTofTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHit.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ExtEvent/RecExtTrack.h" 
#include "ExtEvent/ExtMucHit.h"
#include "MdcRecEvent/RecMdcKalTrack.h"

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"

//#include "DstEvent/DstEvent.h"
//#include "DstEvent/DstTofTrack.h"
#include "DstEvent/DstEmcShower.h"
#include "DstEvent/DstMdcTrack.h"
#include "DstEvent/DstMucTrack.h"
#include "DstEvent/DstMdcDedx.h"
//#include "DstEvent/DstExtTrack.h"  
#include "DstEvent/DstMdcKalTrack.h" 

/////////////////////////////////////////////////////////////////////////////

RecMakerAlg::RecMakerAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RecMakerAlg::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RecMakerAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Get the event header, print out event and run number
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "event: " << eventId 
      << "  run: " <<runId  << endreq;
  
  StatusCode sc = StatusCode::SUCCESS ;
  int trackID;

  // Retrieve Mdc Track 
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
  if(!recMdcTrackCol)
  {
    log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
    SmartDataPtr<DstMdcTrackCol> dstMdcTrackCol(eventSvc(),"/Event/Dst/DstMdcTrackCol");
    if (!dstMdcTrackCol) {
      log << MSG::INFO << "Could not find DstMdcTrackCol" << endreq;
    } 
    else {
      RecMdcTrackCol* recMdcTrackCol = new RecMdcTrackCol();
      DstMdcTrackCol::iterator iter_mdc = dstMdcTrackCol->begin();
      trackID = 0;
      for (;iter_mdc != dstMdcTrackCol->end(); iter_mdc++, trackID++) {
        RecMdcTrack* recMdcTrack = new RecMdcTrack(); 
        *recMdcTrack = **iter_mdc;
        (*recMdcTrackCol).push_back(recMdcTrack);
        log << MSG::INFO 
          	<< " Mdc Track ID = " << trackID
	          << " Mdc Track Nster = " << (*iter_mdc)->nster()
	          << endreq; 
      } 
      sc = eventSvc()->registerObject(EventModel::Recon::RecMdcTrackCol,recMdcTrackCol);
    }
  }

  // Retrieve MdcKal track 
  SmartDataPtr<RecMdcKalTrackCol> recMdcKalTrackCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
  if (!recMdcKalTrackCol) {
    log << MSG::INFO << "Could not find RecMdcKalTrackCol" << endreq;
    SmartDataPtr<DstMdcKalTrackCol> dstMdcKalTrackCol(eventSvc(),"/Event/Dst/DstMdcKalTrackCol");
    if(!dstMdcKalTrackCol) {
      log << MSG::INFO << "Could not find DstMdcKalTrackCol" << endreq;
    }
    else {
      RecMdcKalTrackCol* recMdcKalTrackCol = new RecMdcKalTrackCol();
      DstMdcKalTrackCol::iterator iter_mdc = dstMdcKalTrackCol->begin();
      trackID = 0;
      for (;iter_mdc != dstMdcKalTrackCol->end(); iter_mdc++, trackID++) {
        RecMdcKalTrack* recMdcKalTrack = new RecMdcKalTrack();
        *recMdcKalTrack = **iter_mdc;
        (*recMdcKalTrackCol).push_back(recMdcKalTrack);
        log << MSG::INFO
            << " MdcKalTrack ID = " << trackID
            << " MdcKalTrack Nster = " << (*iter_mdc)->nster()
            << " MdcKalTrack poca = " << (*iter_mdc)->poca()
            << endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecMdcKalTrackCol, recMdcKalTrackCol);	
    }
  }

  // Retrieve MdcDedx 
  SmartDataPtr<RecMdcDedxCol> recMdcDedxCol(eventSvc(),"/Event/Recon/RecMdcDedxCol");
  if (!recMdcDedxCol) {
    log <<MSG::INFO << "Could not find RecMdcDedxCol" << endreq;
    SmartDataPtr<DstMdcDedxCol> dstMdcDedxCol(eventSvc(),"/Event/Dst/DstMdcDedxCol");
    if(!dstMdcDedxCol) {
      log <<MSG::INFO << "Could not find DstMdcDedxCol" << endreq;
    }
    else {
      RecMdcDedxCol* recMdcDedxCol = new RecMdcDedxCol();
      DstMdcDedxCol::iterator iter_mdc = dstMdcDedxCol->begin();
      trackID=0;
      for (;iter_mdc != dstMdcDedxCol->end(); iter_mdc++, trackID++) {
        RecMdcDedx* recMdcDedx = new RecMdcDedx();
        *recMdcDedx= **iter_mdc;
        (*recMdcDedxCol).push_back(recMdcDedx);
        log << MSG::INFO
            << " MdcDedx ID = " << trackID
            << " MdcDedx " 
            << endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecMdcDedxCol, recMdcDedxCol);
    }
  } 
  
  // Retrieve Ext track 
  SmartDataPtr<RecExtTrackCol> recExtTrackCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
  if (!recExtTrackCol) {
    log <<MSG::INFO << "Could not find RecExtTrackCol" << endreq;
    SmartDataPtr<DstExtTrackCol> dstExtTrackCol(eventSvc(),"/Event/Dst/DstExtTrackCol");
    if(!dstExtTrackCol) {
      log <<MSG::INFO << "Could not find DstExtTrackCol" << endreq;
    }
    else {
      RecExtTrackCol* recExtTrackCol = new RecExtTrackCol();
      DstExtTrackCol::iterator iter_ext = dstExtTrackCol->begin();
      trackID=0;
      for (;iter_ext != dstExtTrackCol->end(); iter_ext++, trackID++) {
        RecExtTrack* recExtTrack = new RecExtTrack();
        *recExtTrack = **iter_ext;
        (*recExtTrackCol).push_back(recExtTrack);
        log << MSG::INFO
            << " ExtTrack ID = " << trackID
            << endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecExtTrackCol, recExtTrackCol);
    }
  }
      
  // Retrieve Tof Track 
  SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc(),"/Event/Recon/RecTofTrackCol");
  if ( !tofTrackCol ) {
    log << MSG::INFO << "Could not find TofTrackCol" << endreq;
    SmartDataPtr<DstTofTrackCol> dstTofTrackCol(eventSvc(),"/Event/Dst/DstTofTrackCol");
    if(!dstTofTrackCol) {
      log << MSG::INFO << "Could not find DstTofTrackCol" << endreq;
    }
    else {
      RecTofTrackCol* recTofTrackCol = new RecTofTrackCol();
      DstTofTrackCol::iterator iter_tof = dstTofTrackCol->begin();
      for (;iter_tof != dstTofTrackCol->end(); iter_tof++) {
        RecTofTrack* recTofTrack = new RecTofTrack();
        *recTofTrack = **iter_tof;
        (*recTofTrackCol).push_back(recTofTrack);
        log << MSG::INFO
	    << "Tof Track ID = " << (*iter_tof)->tofTrackID()
	    << " Track ID = " << (*iter_tof)->trackID()
	    << " Tof Counter ID = "<<(*iter_tof)->tofID()
	    << " Quality = "<< (*iter_tof)->quality()
            << endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecTofTrackCol, recTofTrackCol);
    }
  }


  // Retrieve Emc shower 
  SmartDataPtr<RecEmcShowerCol> recEmcShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!recEmcShowerCol) {
    log << MSG::INFO << "Could not find RecEmcShowerCol" << endreq;
    SmartDataPtr<DstEmcShowerCol> dstEmcShowerCol(eventSvc(),"/Event/Dst/DstEmcShowerCol");
    if(!dstEmcShowerCol) {
      log << MSG::INFO << "Could not find DstEmcShowerCol" << endreq;
    }
    else {
      RecEmcShowerCol* recEmcShowerCol = new RecEmcShowerCol();
      DstEmcShowerCol::iterator iter_emc = dstEmcShowerCol->begin();
      trackID=0;
      for (;iter_emc != dstEmcShowerCol->end(); iter_emc++, trackID++) {
        RecEmcShower* recEmcShower = new RecEmcShower();
        *recEmcShower = **iter_emc;
        (*recEmcShowerCol).push_back(recEmcShower);
        log << MSG::INFO
            << " EmcShower ID = " << trackID
            << " EmcShower energy = " << (*iter_emc)->energy()
            << endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecEmcShowerCol, recEmcShowerCol);
    }
  }

  // Retrieve Muc Track 
  SmartDataPtr<RecMucTrackCol> recMucTrackCol(eventSvc(),"/Event/Recon/RecMucTrackCol");
  if (!recMucTrackCol) {
    log <<MSG::INFO << "Could not find RecMucTrackCol" << endreq;
    SmartDataPtr<DstMucTrackCol> dstMucTrackCol(eventSvc(),"/Event/Dst/DstMucTrackCol");
    if(!dstMucTrackCol) {
      log <<MSG::INFO << "Could not find DstMucTrackCol" << endreq;
    }
    else {
      RecMucTrackCol* recMucTrackCol = new RecMucTrackCol();
      DstMucTrackCol::iterator iter_muc = dstMucTrackCol->begin();
      trackID=0;
      for (;iter_muc != dstMucTrackCol->end(); iter_muc++, trackID++) {
        RecMucTrack* recMucTrack = new RecMucTrack();
        *recMucTrack = **iter_muc;
        (*recMucTrackCol).push_back(recMucTrack);
        log << MSG::INFO
            << " MucTrack ID = "<< trackID
            <<endreq;
      }
      sc = eventSvc()->registerObject(EventModel::Recon::RecMucTrackCol, recMucTrackCol);
    }
  }
  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RecMakerAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
