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

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"
#include "EvTimeEvent/RecEsTime.h"

//#include "VertexFit/VertexDbSvc.h"
#include "ParticleID/ParticleID.h"
//
#include "DQAEvent/DQAEvent.h"
#include "DQA_Dedx/DQA_Dedx.h"

#include "TH2F.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;


/////////////////////////////////////////////////////////////////////////////

DQA_Dedx::DQA_Dedx(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("GetNtuple",m_getntuple=false);
  declareProperty("DstFileOnly", m_dstfile=false);
  
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_Dedx::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the sub-system name: MDC, DEDX, TOF, EMC, MUC, TRG
  // user can define more directory, such as DAQFILE/MDC/Bhabha.
  
  
  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }



  NTuplePtr nt_bb(ntupleSvc(), "DQAFILE/DEDX");
  if ( nt_bb ) m_bb_tuple = nt_bb;
  else {
    m_bb_tuple = ntupleSvc()->book("DQAFILE/DEDX/Bhabha", CLID_ColumnWiseTuple, "Dedx ntuple");
    if( m_bb_tuple ) {
      status = m_bb_tuple->addItem("runNo", m_bb_runNo);
      status = m_bb_tuple->addItem("event", m_bb_event);
      status = m_bb_tuple->addItem("p", m_bb_p);
      status = m_bb_tuple->addItem("costh", m_bb_costh);
      status = m_bb_tuple->addItem("t0", m_bb_t0);
      status = m_bb_tuple->addItem("chiE",m_bb_chiE);
      status = m_bb_tuple->addItem("chiMu",m_bb_chiMu);
      status = m_bb_tuple->addItem("chiPi",m_bb_chiPi);
      status = m_bb_tuple->addItem("chiK",m_bb_chiK);
      status = m_bb_tuple->addItem("chiP",m_bb_chiP);
      status = m_bb_tuple->addItem("nhit", m_bb_nhit);
      status = m_bb_tuple->addItem("ndedxhit", m_bb_ndedxhit);
      status = m_bb_tuple->addItem("dEdx", m_bb_dEdx); 
    } else {
      log << MSG::ERROR << "Can not book N-tuple:" << long(m_bb_tuple) << endreq;
    }
  }
  
  TH1F* h_dedx_bb = new TH1F("Dedx_bhabha", "dEdx distribution of bhabha samples", 150, 0., 1500.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/dedx", h_dedx_bb).isFailure()) {
      log << MSG::ERROR << "Couldn't register Dedx_bhabha" << endreq;
  }
  TH1F* h_dedxbarrel_bb = new TH1F("Dedx_Barrel_bhabha", "dEdx distribution of barrel bhabha samples", 150, 0., 1500.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/dedx_barrel", h_dedxbarrel_bb).isFailure()) {
      log << MSG::ERROR << "Couldn't register Dedx_Barrel_bhabha" << endreq;
  }
  TH1F* h_chie_bb = new TH1F("ChiE_bhabha", "ChiE distribution of bhabha samples", 200, -20., 20.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/chiE", h_chie_bb).isFailure()) {
      log << MSG::ERROR << "Couldn't register chiE_bhabha" << endreq;
  }
  TH1F* h_nhitdedx_bb = new TH1F("Nhitdedx", "dEdx used hits", 40, 0., 60.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/nhitdedx", h_nhitdedx_bb).isFailure()) {
      log << MSG::ERROR << "Couldn't register Nhitdedx" << endreq;
  }
  TH1F* h_nhit_bb = new TH1F("Nhit", "Total hits", 40, 0., 60.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/nhit", h_nhit_bb).isFailure()) {
      log << MSG::ERROR << "Couldn't register Nhit" << endreq;
  }
  TH2F* h_dedxvsp_bb=new TH2F("dEdx vs momentume","dEdx vs momentume", 200,0,2.5,200,0,2000);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/dedx_p", h_dedxvsp_bb).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs p" << endreq;
  }
  TH2F* h_dedxvscos_bb=new TH2F("dEdx vs costheta","dEdx vs costheta", 100,-1,1,150,0,1500);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/dedx_costh", h_dedxvscos_bb).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs costh" << endreq;
  }
  TH2F* h_dedxvst0_bb=new TH2F("dEdx vs t0","dEdx vs t0", 250,0,2500,150,0,1500);
  if(m_thsvc->regHist("/DQAHist/Dedx/Bhabha/dedx_t0", h_dedxvst0_bb).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs t0" << endreq;
  }
  

  NTuplePtr nt_du(ntupleSvc(), "DQAFILE/DEDX");
  if ( nt_du ) m_bb_tuple = nt_du;
  else {
    m_du_tuple = ntupleSvc()->book("DQAFILE/DEDX/Dimu", CLID_ColumnWiseTuple, "Dedx ntuple");
    if( m_du_tuple ) {
      status = m_du_tuple->addItem("runNo", m_du_runNo);
      status = m_du_tuple->addItem("event", m_du_event);
      status = m_du_tuple->addItem("p", m_du_p);
      status = m_du_tuple->addItem("costh", m_du_costh);
      status = m_du_tuple->addItem("t0", m_du_t0);
      status = m_du_tuple->addItem("chiE",m_du_chiE);
      status = m_du_tuple->addItem("chiMu",m_du_chiMu);
      status = m_du_tuple->addItem("chiPi",m_du_chiPi);
      status = m_du_tuple->addItem("chiK",m_du_chiK);
      status = m_du_tuple->addItem("chiP",m_du_chiP);
      status = m_du_tuple->addItem("nhit", m_du_nhit);
      status = m_du_tuple->addItem("ndedxhit", m_du_ndedxhit);
      status = m_du_tuple->addItem("dEdx", m_du_dEdx); 
    } else {
      log << MSG::ERROR << "Can not book N-tuple:" << long(m_du_tuple) << endreq;
      }
  }
  
  
  TH1F* h_dedx_du = new TH1F("Dedx_dimu", "dEdx distribution of bhabha samples", 150, 0., 1500.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/dedx", h_dedx_du).isFailure()) {
      log << MSG::ERROR << "Couldn't register Dedx_dimu" << endreq;
  }
  TH1F* h_dedxbarrel_du = new TH1F("Dedx_Barrel_dimu", "dEdx distribution of barrel bhabha samples", 150, 0., 1500.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/dedx_barrel", h_dedxbarrel_du).isFailure()) {
      log << MSG::ERROR << "Couldn't register Dedx_Barrel_dimu" << endreq;
  }
  TH1F* h_chimu_du = new TH1F("ChiE_dimu", "ChiE distribution of bhabha samples", 200, -20., 20.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/chiMu", h_chimu_du).isFailure()) {
      log << MSG::ERROR << "Couldn't register chiMu_dimu" << endreq;
  }
  TH1F* h_nhitdedx_du = new TH1F("Nhitdedx", "dEdx used hits", 40, 0., 60.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/nhitdedx", h_nhitdedx_du).isFailure()) {
      log << MSG::ERROR << "Couldn't register Nhitdedx" << endreq;
  }
  TH1F* h_nhit_du = new TH1F("Nhit", "Total hits", 40, 0., 60.);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/nhit", h_nhit_du).isFailure()) {
      log << MSG::ERROR << "Couldn't register Nhit" << endreq;
  }
  TH2F* h_dedxvsp_du=new TH2F("dEdx vs momentume","dEdx vs momentume", 200,0,2.5,200,0,2000);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/dedx_p", h_dedxvsp_du).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs p" << endreq;
  }
  TH2F* h_dedxvscos_du=new TH2F("dEdx vs costheta","dEdx vs costheta", 100,-1,1,150,0,1500);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/dedx_costh", h_dedxvscos_du).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs costh" << endreq;
  }
  TH2F* h_dedxvst0_du=new TH2F("dEdx vs t0","dEdx vs t0", 200,0,2500,150,0,1500);
  if(m_thsvc->regHist("/DQAHist/Dedx/Dimu/dedx_t0", h_dedxvst0_du).isFailure()) {
    log << MSG::ERROR << "Couldn't register dedx vs t0" << endreq;
  }
  

  

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_Dedx::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_bb_runNo=eventHeader->runNumber();
  m_bb_event=eventHeader->eventNumber();
  
  m_du_runNo=eventHeader->runNumber();
  m_du_event=eventHeader->eventNumber();
  
  log << MSG::DEBUG <<"run, evt num = "
      << m_bb_runNo << " , "
      << m_bb_event <<endreq;


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
      log << MSG::DEBUG << i << " " << (*itTrk)->quality() << endreq;
      // get the: required particle through the track's PID
      // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
      // if ( (*itTrk)->partId() != 1 ) continue; // only e+, e-
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
      
      
      TH1 *h(0);
      TH2 *h2(0);
      //initinalize all values
      m_bb_runNo=-999;
      m_bb_event=-999;
      m_bb_p=-999;
      m_bb_costh=-999;
      m_bb_t0=-999;
      m_bb_chiE=-999;
      m_bb_chiMu=-999;
      m_bb_chiPi=-999;
      m_bb_chiK=-999;
      m_bb_chiP=-999;
      m_bb_nhit=-999;
      m_bb_ndedxhit=-999;
      m_bb_dEdx=-999; 
      
      
      if((*itTrk)->isMdcKalTrackValid()){
	RecMdcKalTrack *mdcTrk = (*itTrk)->mdcKalTrack();
	mdcTrk->setPidType(RecMdcKalTrack::electron);
	if ( mdcTrk->charge() > 0 ) {
	  log << MSG::DEBUG << "is electron" << endreq;
	} else {
	  log << MSG::DEBUG << "is positron" << endreq;
	}
	double x0  =mdcTrk->x(); 
	double y0  =mdcTrk->y(); 
	double z0  =mdcTrk->z(); 
	double Rxy=sqrt(x0*x0+y0*y0);
	
	m_bb_p=mdcTrk->p()*mdcTrk->charge();
	m_bb_costh=cos(mdcTrk->theta());
	

	double tes=-999;
        if (!m_dstfile) {	
	    
	    //get event start time
	    SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
	    if( aevtimeCol &&  aevtimeCol->size()>0 ){
	      RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
	      for(; iter_evt!=aevtimeCol->end(); iter_evt++){
	        tes = (*iter_evt)->getTest();
	        //esTimeflag = (*iter_evt)->getStat();
	      }
	    }
	    m_bb_t0=tes;
        }

	//get dedx information
	if((*itTrk)->isMdcDedxValid()){
	  RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
	  m_bb_chiE   = dedxTrk->chi(0);
	  m_bb_chiMu  = dedxTrk->chi(1);
	  m_bb_chiPi  = dedxTrk->chi(2);
	  m_bb_chiK   = dedxTrk->chi(3);
	  m_bb_chiP   = dedxTrk->chi(4);
	  m_bb_ndedxhit   = dedxTrk->numGoodHits();
	  m_bb_nhit   = dedxTrk->numTotalHits();
	  m_bb_dEdx = dedxTrk->probPH();
	  
	  //fill histograms
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/dedx", h).isSuccess()) {
	    h->Fill(dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx" << endreq;
	  }
	  if(fabs(cos(mdcTrk->theta()))<0.83){
	    if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/dedx_barrel", h).isSuccess()) {
	      h->Fill(dedxTrk->probPH());
	    } 
	    else {
	      log << MSG::ERROR << "Couldn't retrieve dedx_barrel" << endreq;
	    }
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/chiE", h).isSuccess()) {
	    h->Fill(dedxTrk->chiE());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve chiE" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/nhitdedx", h).isSuccess()) {
	    h->Fill( dedxTrk->numGoodHits());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve nhitdedx" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/nhit", h).isSuccess()) {
	    h->Fill( dedxTrk->numTotalHits());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve nhit" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/dedx_p", h2).isSuccess()) {
	    h2->Fill(mdcTrk->p(), dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx_p" << endreq;
	  } 
	  if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/dedx_costh", h2).isSuccess()) {
	    h2->Fill(cos(mdcTrk->theta()), dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx_costh" << endreq;
	  } 
          if (!m_dstfile) {
	      if (m_thsvc->getHist("/DQAHist/Dedx/Bhabha/dedx_t0", h2).isSuccess()) {
	        h2->Fill(tes, dedxTrk->probPH());
	      } 
	      else {
	        log << MSG::ERROR << "Couldn't retrieve dedx_t0" << endreq;
	      } 
          }

	}//end of valid dedx track
      }//end of valid mdc track
      }//end of looping track
  }//end of bhabha tag event
  
  
  if ( dqaevt->Dimu() ) {
    log << MSG::INFO << "Dimu event" << endreq;
    SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
    for(int i = 0; i < evtRecEvent->totalCharged(); i++){
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      log << MSG::DEBUG << i << " " << (*itTrk)->quality() << endreq;
      // get the: required particle through the track's PID
      // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
      //if ( (*itTrk)->partId() != 2 ) continue; // only mu+, mu-
      if ( !(*itTrk)->isMuon() ) continue; // only mu+, mu-
      // if you want to do dE/dx or TOF study, select track with no bias
      // Quality: defined by whether dE/dx or TOF is used to identify particle
      // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
      // 1 - only dE/dx (can be used for TOF calibration)
      // 2 - only TOF (can be used for dE/dx calibration)
      // 3 - Both dE/dx and TOF          
      int qual = (*itTrk)->quality();
      if ( qual != 0 && qual != 2) continue; // no dE/dx PID is used in selection
      //          if ( qual != 0 && qual != 1) continue; // no TOF PID is used in the selection
      
      TH1 *h(0);
      TH2 *h2(0);
      //initinalize all values
      m_du_runNo=-999;
      m_du_event=-999;
      m_du_p=-999;
      m_du_costh=-999;
      m_du_t0=-999;
      m_du_chiE=-999;
      m_du_chiMu=-999;
      m_du_chiPi=-999;
      m_du_chiK=-999;
      m_du_chiP=-999;
      m_du_nhit=-999;
      m_du_ndedxhit=-999;
      m_du_dEdx=-999; 
      
      
      if((*itTrk)->isMdcKalTrackValid()){
	RecMdcKalTrack *mdcTrk = (*itTrk)->mdcKalTrack();
	mdcTrk->setPidType(RecMdcKalTrack::muon);
	if ( mdcTrk->charge() > 0 ) {
	  log << MSG::DEBUG << "is muon+" << endreq;
	} else {
	  log << MSG::DEBUG << "is muon-" << endreq;
	}
	double x0  =mdcTrk->x(); 
	double y0  =mdcTrk->y(); 
	double z0  =mdcTrk->z(); 
	double Rxy=sqrt(x0*x0+y0*y0);
	
	m_du_p=mdcTrk->p()*mdcTrk->charge();
	m_du_costh=cos(mdcTrk->theta());
	
	
	double tes=-999;
        if (!m_dstfile) {
	    //get event start time
	    SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
	    if( aevtimeCol &&  aevtimeCol->size()>0 ){
	      RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
	      for(; iter_evt!=aevtimeCol->end(); iter_evt++){
	        tes = (*iter_evt)->getTest();
	        //esTimeflag = (*iter_evt)->getStat();
	      }
	    }
	    m_du_t0=tes;
        }
	
	//get dedx information
	if((*itTrk)->isMdcDedxValid()){
	  RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
	  m_du_chiE   = dedxTrk->chi(0);
	  m_du_chiMu  = dedxTrk->chi(1);
	  m_du_chiPi  = dedxTrk->chi(2);
	  m_du_chiK   = dedxTrk->chi(3);
	  m_du_chiP   = dedxTrk->chi(4);
	  m_du_ndedxhit   = dedxTrk->numGoodHits();
	  m_du_nhit   = dedxTrk->numTotalHits();
	  m_du_dEdx = dedxTrk->probPH();
	  
	  //fill histograms
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/dedx", h).isSuccess()) {
	    h->Fill(dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx" << endreq;
	  }
	  if(fabs(cos(mdcTrk->theta()))<0.83){
	    if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/dedx_barrel", h).isSuccess()) {
	      h->Fill(dedxTrk->probPH());
	    } 
	    else {
	      log << MSG::ERROR << "Couldn't retrieve dedx_barrel" << endreq;
	    }
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/chiMu", h).isSuccess()) {
	    h->Fill(dedxTrk->chiE());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve chiMu" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/nhitdedx", h).isSuccess()) {
	    h->Fill( dedxTrk->numGoodHits());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve nhitdedx" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/nhit", h).isSuccess()) {
	    h->Fill( dedxTrk->numTotalHits());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve nhit" << endreq;
	  }
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/dedx_p", h2).isSuccess()) {
	    h2->Fill(mdcTrk->p(), dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx_p" << endreq;
	  } 
	  if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/dedx_costh", h2).isSuccess()) {
	    h2->Fill(cos(mdcTrk->theta()), dedxTrk->probPH());
	  } 
	  else {
	    log << MSG::ERROR << "Couldn't retrieve dedx_costh" << endreq;
	  } 
          if (!m_dstfile) {
	      if (m_thsvc->getHist("/DQAHist/Dedx/Dimu/dedx_t0", h2).isSuccess()) {
	        h2->Fill(tes, dedxTrk->probPH());
	      } 
	      else {
	        log << MSG::ERROR << "Couldn't retrieve dedx_t0" << endreq;
	      } 
          }

	}//end of valide dedx track
      }//end of valide mdc track
    }//end of looping track
  }//end of dimu tag event
  
  if(m_getntuple){
    m_bb_tuple->write();
    m_du_tuple->write();
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQA_Dedx::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


