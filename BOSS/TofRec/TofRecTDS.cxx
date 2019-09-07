#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "ReconEvent/ReconEvent.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TofRec/TofTrack.h"
#include "TofRec/TofRecTDS.h"

StatusCode TofRecTDS::InitializeTDS() {
  if( RegisterReconEvent() == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( RegisterNullRecTofTrackCol() == StatusCode::FAILURE ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}


StatusCode TofRecTDS::RegisterNullRecTofTrackCol() {
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "TofRecTDS");
  log << MSG::INFO << "Initialize: TofRecTDS::RegisterNullRecTofTrack()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  RecTofTrackCol* recTofTrackCol = new RecTofTrackCol;

// Check whether the RecTofTrackCol has been registered.
  StatusCode sc;
  DataObject *aRecTofTrackCol;
  eventSvc->findObject( "/Event/Recon/RecTofTrackCol", aRecTofTrackCol );
  if( aRecTofTrackCol != NULL ) {
    sc = eventSvc->unregisterObject( "/Event/Recon/RecTofTrackCol" );
    if( sc != StatusCode::SUCCESS ) {
      log << MSG::FATAL << "Initialize: Could not unregister RecTofTrack collection!" << endreq;
      delete recTofTrackCol;
      return StatusCode::FAILURE;
    }
  }
  delete aRecTofTrackCol;

  sc = eventSvc->registerObject("/Event/Recon/RecTofTrackCol", recTofTrackCol);
  if( sc != StatusCode::SUCCESS ) {
    log << MSG::FATAL << "Initialize: TofRec could not register Null Rec TOF track collection!" << endreq;
    delete recTofTrackCol;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TofRecTDS::RegisterTDS( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, bool forCalibration, std::string calibData ) {
  //  if( RegisterReconEvent() == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( RegisterRecTofTrackCol( tofTrackVec ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if( forCalibration ) {
    if( RegisterRecBTofCalHitCol( runNumber, eventNumber, tofTrackVec, calibData ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
    if( RegisterRecETofCalHitCol( runNumber, eventNumber, tofTrackVec, calibData ) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}


StatusCode TofRecTDS::RegisterReconEvent() {
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "TofRecTDS");

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

// Register Reconstructed Track To TDS
  DataObject *aReconEvent ;
  eventSvc->findObject("/Event/Recon",aReconEvent);
  if(aReconEvent==NULL) {
    log << MSG::INFO << "TofRecTDS::RegisterReconEvent()" << endreq;
    aReconEvent = new ReconEvent();
    StatusCode sc = eventSvc->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "TofRec could not register ReconEvent!" <<endreq;
      return StatusCode::FAILURE;
    }
  } 
  return StatusCode::SUCCESS;
}


StatusCode TofRecTDS::RegisterRecTofTrackCol( std::vector<TofTrack*>*& tofTrackVec ) {
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "TofRecTDS");
  log << MSG::INFO << "TofRecTDS::RegisterRecTofTrack()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  RecTofTrackCol* recTofTrackCol = new RecTofTrackCol;

  std::vector<TofTrack*>::iterator iter = tofTrackVec->begin();
  for( ; iter < tofTrackVec->end(); iter++ ) {
    (*iter)->convert2RecTofTrackCol( recTofTrackCol );
  }

// Check whether the RecTofTrackCol has been registered.
  StatusCode sc;
  DataObject *aRecTofTrackCol;
  eventSvc->findObject( "/Event/Recon/RecTofTrackCol", aRecTofTrackCol );
  if( aRecTofTrackCol != NULL ) {

    sc = eventSvc->unregisterObject( "/Event/Recon/RecTofTrackCol" );

    delete aRecTofTrackCol;

    if( sc != StatusCode::SUCCESS ) {
      log << MSG::FATAL << "Could not unregister RecTofTrack collection!" << endreq;

      RecTofTrackCol::iterator iter_tof = recTofTrackCol->begin();
      for( ; iter_tof!=recTofTrackCol->end(); iter_tof++ ) {
	delete (*iter_tof);
      }
      recTofTrackCol->clear();
      delete recTofTrackCol;

      std::vector<TofTrack*>::iterator iter_data = tofTrackVec->begin();
      for( ; iter_data!=tofTrackVec->end(); iter_data++ ) {
	delete (*iter_data);
      }
      tofTrackVec->clear();
      delete tofTrackVec;

      return StatusCode::FAILURE;
    }
  }

  sc = eventSvc->registerObject("/Event/Recon/RecTofTrackCol", recTofTrackCol);
  if( sc != StatusCode::SUCCESS ) {
    log << MSG::FATAL << "TofRec could not register Rec TOF track collection!" << endreq;
    RecTofTrackCol::iterator iter_tof = recTofTrackCol->begin();
    for( ; iter_tof!=recTofTrackCol->end(); iter_tof++ ) {
      delete (*iter_tof);
    }
    recTofTrackCol->clear();
    delete recTofTrackCol;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode TofRecTDS::RegisterRecBTofCalHitCol( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, std::string m_calibData ) {
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "TofRecTDS");
  log << MSG::INFO << "TofRecTDS::RegisterRecBTofCalHitCol()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  RecBTofCalHitCol* recBTofCalHitCol = new RecBTofCalHitCol;

  std::vector<TofTrack*>::iterator iter = tofTrackVec->begin();
  for( ; iter < tofTrackVec->end(); iter++ ) {
    ExtTrackCase hitcase = (*iter)->hitCase();
    //    if( ( hitcase == InnerLayer ) || ( hitcase == OuterLayer ) || ( hitcase == DoubleLayer ) ) {
    if( ( hitcase == InnerLayer ) || ( hitcase == DoubleLayer ) ) {
      (*iter)->convert2RecBTofCalHitColBarrel( runNumber, eventNumber, recBTofCalHitCol, m_calibData );
    }
    if( ( hitcase == EastEndcapMRPC ) || ( hitcase == WestEndcapMRPC ) ) {
      (*iter)->convert2RecBTofCalHitColETF( runNumber, eventNumber, recBTofCalHitCol, m_calibData );
    }
  }

// Check whether the RecBTofCalHitCol has been registered.
  StatusCode scb;
  DataObject *aRecBTofCalHitCol;
  eventSvc->findObject( "/Event/Recon/RecBTofCalhitCol", aRecBTofCalHitCol );
  if( aRecBTofCalHitCol != NULL ) {
    scb = eventSvc->unregisterObject( "/Event/Recon/RecBTofCalHitCol" );
    if( scb != StatusCode::SUCCESS ) {
      log << MSG::FATAL << "Could not unregister RecBTofCalHit collection!" << endreq;

      RecBTofCalHitCol::iterator iter_btof = recBTofCalHitCol->begin();
      for( ; iter_btof!=recBTofCalHitCol->end(); iter_btof++ ) {
	delete (*iter_btof);
      }
      recBTofCalHitCol->clear();
      delete recBTofCalHitCol;

      std::vector<TofTrack*>::iterator iter_data = tofTrackVec->begin();
      for( ; iter_data!=tofTrackVec->end(); iter_data++ ) {
	delete (*iter_data);
      }
      tofTrackVec->clear();
      delete tofTrackVec;

      return StatusCode::FAILURE;
    }
  }

  StatusCode sh1 = eventSvc->registerObject("/Event/Recon/RecBTofCalHitCol", recBTofCalHitCol);
  if(sh1!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "TofRec could not register BTOF CalHit collection!!" << endreq;
    RecBTofCalHitCol::iterator iter_btof = recBTofCalHitCol->begin();
    for( ; iter_btof!=recBTofCalHitCol->end(); iter_btof++ ) {
      delete (*iter_btof);
    }
    recBTofCalHitCol->clear();
    delete recBTofCalHitCol;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

  
StatusCode TofRecTDS::RegisterRecETofCalHitCol( int runNumber, int eventNumber, std::vector<TofTrack*>*& tofTrackVec, std::string m_calibData ) {
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "TofRecTDS");
  log << MSG::INFO << "TofRecTDS::RegisterRecETofCalHitCol()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  //sunss add 08/10/16

  if( tofTrackVec->size() != 2 ) return StatusCode::SUCCESS;
  std::vector<TofTrack*>::iterator iter1 = tofTrackVec->begin();
  std::vector<TofTrack*>::iterator iter2 = iter1+1;
  if( (*iter1)->tdc1() < 0.0 || (*iter2)->tdc1() < 0.0 ) return StatusCode::SUCCESS;
  if( abs( (*iter1)->tdc1() - (*iter2)->tdc1() ) > 2.0 ) return StatusCode::SUCCESS;

  //sunss add 08/10/16

  RecETofCalHitCol* recETofCalHitCol = new RecETofCalHitCol;

  std::vector<TofTrack*>::iterator iter = tofTrackVec->begin();
  for( ; iter < tofTrackVec->end(); iter++ ) {
    ExtTrackCase hitcase = (*iter)->hitCase();
    if( ( hitcase == EastEndcap ) || ( hitcase == WestEndcap ) ) {
      (*iter)->convert2RecETofCalHitCol( runNumber, eventNumber, recETofCalHitCol, m_calibData );
    }
  }

// Check whether the RecETofCalHitCol has been registered.
  StatusCode sce;

  DataObject *aRecETofCalHitCol;
  eventSvc->findObject( "/Event/Recon/RecETofCalhitCol", aRecETofCalHitCol );
  if( aRecETofCalHitCol != NULL ) {
    sce = eventSvc->unregisterObject( "/Event/Recon/RecETofCalHitCol" );
    if( sce != StatusCode::SUCCESS ) {
      log << MSG::FATAL << "Could not unregister RecETofCalHit collection!" << endreq;

      RecETofCalHitCol::iterator iter_etof = recETofCalHitCol->begin();
      for( ; iter_etof!=recETofCalHitCol->end(); iter_etof++ ) {
	delete (*iter_etof);
      }
      recETofCalHitCol->clear();
      delete recETofCalHitCol;

      std::vector<TofTrack*>::iterator iter_data = tofTrackVec->begin();
      for( ; iter_data!=tofTrackVec->end(); iter_data++ ) {
	delete (*iter_data);
      }
      tofTrackVec->clear();
      delete tofTrackVec;

      return StatusCode::FAILURE;
    }
  }

  StatusCode sh2 = eventSvc->registerObject("/Event/Recon/RecETofCalHitCol",recETofCalHitCol);
  if(sh2!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "TofRec could not register ETOF CalHit collection!!" << endreq;
    RecETofCalHitCol::iterator iter_etof = recETofCalHitCol->begin();
    for( ; iter_etof!=recETofCalHitCol->end(); iter_etof++ ) {
      delete (*iter_etof);
    }
    recETofCalHitCol->clear();
    delete recETofCalHitCol;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
