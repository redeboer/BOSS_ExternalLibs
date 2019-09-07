#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "EmcRawEvent/EmcDigi.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcRawEvent/MdcDigi.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "DTagSkim/DTagSkim.h"

#include <vector>
#include "mysql.h" 


/////////////////////////////////////////////////////////////////////////////

DTagSkim::DTagSkim(const std::string& name, ISvcLocator* pSvcLocator) :  
  Algorithm(name, pSvcLocator) {
  //Declare the properties  
  declareProperty("WriteDst",         m_ifwritedst=true);
  declareProperty("WriteRec",         m_ifwriterec=false);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DTagSkim::initialize() {
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode sc;

  log << MSG::INFO << "creating sub-algorithms...." << endreq;

  
  sc =  createSubAlgorithm( "EventWriter", "WriteDst", m_subalg1);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Error creating Sub-Algorithm WriteDst" <<endreq;
    return sc;
  } else {
    log << MSG::INFO << "Success creating Sub-Algorithm WriteDst" <<endreq;
  }


  
  sc =  createSubAlgorithm( "EventWriter", "WriteRec", m_subalg2);
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Error creating Sub-Algorithm WriteRec" <<endreq;
    return sc;
  } else {
    log << MSG::INFO << "Success creating Sub-Algorithm WriteRec" <<endreq;
  }
 
 

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DTagSkim::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;


  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if(!eventHeader)
    {
      cout<<"  eventHeader  "<<endl;
      return StatusCode::FAILURE;
    }
  
  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();

  
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  if(!evtRecEvent ) {
    cout<<"  evtRecEvent  "<<endl;
    return StatusCode::FAILURE;
  }


  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
    << evtRecEvent->totalCharged() << " , "
    << evtRecEvent->totalNeutral() << " , "
    << evtRecEvent->totalTracks() <<endreq;
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  if(!evtRecTrkCol){
    cout<<"  evtRecTrkCol  "<<endl;
    return StatusCode::FAILURE;
  }

  if(evtRecEvent->totalTracks()!=evtRecTrkCol->size()) return StatusCode::SUCCESS;


  SmartDataPtr<EvtRecDTagCol> evtRecDTagCol(eventSvc(), EventModel::EvtRec::EvtRecDTagCol);
  if ( ! evtRecDTagCol ) {
    log << MSG::FATAL << "Could not find EvtRecDTagCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  
  if(evtRecDTagCol->size()==0){
    //cout<<"no D candidates found in this event"<<endl;
    return StatusCode::SUCCESS;
  }
  //else{
  //cout<<"there are D candidates found in this event"<<endl;
  //}

  // -------- Write to root file
  if(m_ifwritedst) m_subalg1->execute();
  if(m_ifwriterec) m_subalg2->execute();
      

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DTagSkim::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  
  return StatusCode::SUCCESS;
}

