#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"

#include "TH1F.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

//#include "VertexFit/VertexDbSvc.h"
#include "ParticleID/ParticleID.h"
//
#include "DQAEvent/DQAEvent.h"
#include "DQAFillEx/DQAFillEx.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;


/////////////////////////////////////////////////////////////////////////////

DQAFillEx::DQAFillEx(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAFillEx::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the sub-system name: MDC, DEDX, TOF, EMC, MUC, TRG
  // user can define more directory, such as DAQFILE/MDC/Bhabha.
  NTuplePtr nt(ntupleSvc(), "DQAFILE/MDC");
  if ( nt ) m_tuple = nt;
  else {
      m_tuple = ntupleSvc()->book("DQAFILE/MDC", CLID_ColumnWiseTuple, "MDC ntuple");
      if( m_tuple ) {
          status = m_tuple->addItem("runNo", m_runNo);
          status = m_tuple->addItem("event", m_event);
      } else {
          log << MSG::ERROR << "Can not book N-tuple:" << long(m_tuple) << endreq;
      }
  }

  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // "DQAHist" is fixed
  // "Rhopi" is the sub-system name, just as ntuple case.
  TH1F* hrxy = new TH1F("Rxy", "Rxy distribution", 110, -1., 10.);
  if(m_thsvc->regHist("/DQAHist/MDC/hrxy", hrxy).isFailure()) {
      log << MSG::ERROR << "Couldn't register Rxy" << endreq;
  }
  TH1F* hz = new TH1F("z", "z distribution", 200, -100., 100.);
  if(m_thsvc->regHist("/DQAHist/MDC/hz", hz).isFailure()) {
      log << MSG::ERROR << "Couldn't register z" << endreq;
  }

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAFillEx::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_runNo=eventHeader->runNumber();
  m_event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
      << m_runNo << " , "
      << m_event <<endreq;


  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);

  SmartDataPtr<DQAEvent::DQAEvent> dqaevt(eventSvc(), "/Event/DQATag");
  if ( dqaevt ) {
      log << MSG::INFO << "success get DQAEvent" << endreq;
  } else {
      log << MSG::ERROR << "Error accessing DQAEvent" << endreq;
      return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "event tag = " << dqaevt->EventTag() << endreq;

  // get the required control sample with DQA tag
  if ( dqaevt->Bhabha() ) {
      log << MSG::INFO << "Bhabha event" << endreq;
      SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
      for(int i = 0; i < evtRecEvent->totalCharged(); i++){
          EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
          log << MSG::DEBUG << i << " " << (*itTrk)->partId() << " "
              << (*itTrk)->quality() << endreq;
          // get the required particle through the track's PID
          // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
          // if ( (*itTrk)->partId() != 1 ) continue; // only e+, e-
          // PID: isElectron(), isMuon(), isPion(), isKaon(), isProton()
          if ( !(*itTrk)->isElectron() ) continue; // only e+, e-
          // if you want to do dE/dx or TOF study, select track with no bias
          // Quality: defined by whether dE/dx or TOF is used to identify particle
          // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
          // 1 - only dE/dx (can be used for TOF calibration)
          // 2 - only TOF (can be used for dE/dx calibration)
          // 3 - Both dE/dx and TOF          
          int qual = (*itTrk)->quality();
          if ( qual != 0 && qual != 2) continue; // no dE/dx PID is used in selection
//          if ( qual != 0 && qual != 1) continue; // no TOF PID is used in the selection
          RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
          if ( mdcTrk->charge() > 0 ) {
              log << MSG::DEBUG << "is electron" << endreq;
          } else {
              log << MSG::DEBUG << "is positron" << endreq;
          }
          double x0  =mdcTrk->x(); 
          double y0  =mdcTrk->y(); 
          double z0  =mdcTrk->z(); 
          double Rxy=sqrt(x0*x0+y0*y0);
          // DQA
          TH1 *h(0);
          if (m_thsvc->getHist("/DQAHist/MDC/hrxy", h).isSuccess()) {
              h->Fill(Rxy);
          } else {
              log << MSG::ERROR << "Couldn't retrieve hrxy" << endreq;
          }
          if (m_thsvc->getHist("/DQAHist/MDC/hz", h).isSuccess()) {
              h->Fill(z0);
          } else {
              log << MSG::ERROR << "Couldn't retrieve hz" << endreq;
          }
      }
  }
  
  m_tuple->write();
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAFillEx::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


