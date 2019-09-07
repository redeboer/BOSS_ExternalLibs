#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"

#include "TMath.h"
#include "TH1F.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/Helix.h"
#include "ParticleID/ParticleID.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include <vector>

#include "DQAKsKpiDEDXAlg/DQAKsKpiDEDX.h"

const double mpi = 0.13957;
const double mk  = 0.493677;
const double mks0 = 0.497648;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

//boost
const  HepLorentzVector p_cms(0.034067, 0.0, 0.0, 3.097);
const  Hep3Vector u_cms = -p_cms.boostVector();
 //declare one counter
static int counter[10]={0,0,0,0,0,0,0,0,0,0};

/////////////////////////////////////////////////////////////////////////////

DQAKsKpiDEDX::DQAKsKpiDEDX(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=5.0);
  declareProperty("Vz0cut", m_vz0cut=20.0);
  declareProperty("Vr1cut", m_vr1cut=1.0);
  declareProperty("Vz1cut", m_vz1cut=5.0);
  declareProperty("Vctcut", m_cthcut=0.93);
  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaAngCut", m_gammaAngCut=20.0);
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAKsKpiDEDX::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  if(service("THistSvc", m_thsvc).isFailure()) {
      log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
      return StatusCode::FAILURE;
  }
  
  // "DQAHist" is fixed
  // "DQAKsKpi" is control sample name, just as ntuple case.
  TH1F* hks_dl = new TH1F("ks_dl", "ks_dl", 300, -5.0, 25.0);
  if(m_thsvc->regHist("/DQAHist/DQAKsKpiDEDX/hks_dl", hks_dl).isFailure()) {
      log << MSG::ERROR << "Couldn't register ks_dl" << endreq;
  }

  TH1F* hks_m = new TH1F("ks_m", "ks_m", 200,0.4, 0.6);
  if(m_thsvc->regHist("/DQAHist/DQAKsKpiDEDX/hks_m", hks_m).isFailure()) {
      log << MSG::ERROR << "Couldn't register ks_m" << endreq;
  }

  TH1F* hkspi_m = new TH1F("kspi_m", "kspi_m", 200,0.6, 2.6);
  if(m_thsvc->regHist("/DQAHist/DQAKsKpiDEDX/hkspi_m", hkspi_m).isFailure()) {
      log << MSG::ERROR << "Couldn't register kspi_m" << endreq;
  }

  TH1F* hks_p = new TH1F("ks_p", "ks_p", 100,0.0, 1.5);
  if(m_thsvc->regHist("/DQAHist/DQAKsKpiDEDX/hks_p", hks_p).isFailure()) {
      log << MSG::ERROR << "Couldn't register ks_p" << endreq;
  }

  TH1F* hkpi_m = new TH1F("kpi_m", "kpi_m", 200,0.6, 2.6);
  if(m_thsvc->regHist("/DQAHist/DQAKsKpiDEDX/hkpi_m", hkpi_m).isFailure()) {
      log << MSG::ERROR << "Couldn't register kpi_m" << endreq;
  }

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the control sample name, the first letter is in upper format. eg. "Rhopi"
  NTuplePtr nt(ntupleSvc(), "DQAFILE/KsKpiDEDX");
  if ( nt ) m_tuple = nt;
  else {
      m_tuple = ntupleSvc()->book("DQAFILE/KsKpiDEDX", CLID_ColumnWiseTuple, "KsKpiDEDX ntuple");
      if( m_tuple ) {
          status = m_tuple->addItem("runNo",   m_runNo);
          status = m_tuple->addItem("event",   m_event);
//          status = m_tuple->addItem("Nchrg",   m_nchrg);
//          status = m_tuple->addItem("Nneu",    m_nneu);

          status = m_tuple->addItem("npip", m_npip);
          status = m_tuple->addItem("npim", m_npim);
          status = m_tuple->addItem("nkp",  m_nkp);
          status = m_tuple->addItem("nkm",  m_nkm);
          status = m_tuple->addItem("np",   m_np);
          status = m_tuple->addItem("npb",  m_npb);

          status = m_tuple->addItem("vfits_chi",  m_vfits_chi); 
          status = m_tuple->addItem("vfits_vx",   m_vfits_vx); 
          status = m_tuple->addItem("vfits_vy",   m_vfits_vy); 
          status = m_tuple->addItem("vfits_vz",   m_vfits_vz); 
          status = m_tuple->addItem("vfits_vr",   m_vfits_vr); 

          status = m_tuple->addItem("vfitp_chi",  m_vfitp_chi);
          status = m_tuple->addItem("vfitp_vx",   m_vfitp_vx); 
          status = m_tuple->addItem("vfitp_vy",   m_vfitp_vy); 
          status = m_tuple->addItem("vfitp_vz",   m_vfitp_vz); 
          status = m_tuple->addItem("vfitp_vr",   m_vfitp_vr); 

          status = m_tuple->addItem("vfit2_chi",  m_vfit2_chi);
          status = m_tuple->addItem("vfit2_mks",  m_vfit2_mks);
          status = m_tuple->addItem("vfit2_ct",   m_vfit2_ct);
          status = m_tuple->addItem("vfit2_dl",   m_vfit2_dl);
          status = m_tuple->addItem("vfit2_dle",  m_vfit2_dle);   

          status = m_tuple->addItem("chi2_fs4c", m_chi2_fs4c);
          status = m_tuple->addItem("mks_fs4c",  m_mks_fs4c);
          status = m_tuple->addItem("mkspi_fs4c",m_mkspi_fs4c);
          status = m_tuple->addItem("mksk_fs4c", m_mksk_fs4c);
          status = m_tuple->addItem("mkpi_fs4c", m_mkpi_fs4c);

          status = m_tuple->addItem("4c_chi2",   m_4c_chi2);
          status = m_tuple->addItem("4c_mks",    m_4c_mks);
          status = m_tuple->addItem("4c_mkspi",  m_4c_mkspi);
          status = m_tuple->addItem("4c_mksk",   m_4c_mksk);
          status = m_tuple->addItem("4c_mkpi",   m_4c_mkpi);
          status = m_tuple->addItem("4c_ks_px",  m_4c_ks_px);
          status = m_tuple->addItem("4c_ks_py",  m_4c_ks_py);
          status = m_tuple->addItem("4c_ks_pz",  m_4c_ks_pz);
          status = m_tuple->addItem("4c_ks_p",   m_4c_ks_p);
          status = m_tuple->addItem("4c_ks_cos", m_4c_ks_cos);

          status = m_tuple->addItem("NGch",    m_ngch, 0, 10);
          status = m_tuple->addIndexedItem("pidcode",    m_ngch, m_pidcode);
          status = m_tuple->addIndexedItem("pidprob",    m_ngch, m_pidprob);
          status = m_tuple->addIndexedItem("pidchiDedx", m_ngch, m_pidchiDedx);
          status = m_tuple->addIndexedItem("pidchiTof1", m_ngch, m_pidchiTof1);
          status = m_tuple->addIndexedItem("pidchiTof2", m_ngch, m_pidchiTof2); 

          status = m_tuple->addIndexedItem("charge",m_ngch, m_charge);
          status = m_tuple->addIndexedItem("vx0",   m_ngch, m_vx0);
          status = m_tuple->addIndexedItem("vy0",   m_ngch, m_vy0);
          status = m_tuple->addIndexedItem("vz0",   m_ngch, m_vz0);
          status = m_tuple->addIndexedItem("vr0",   m_ngch, m_vr0);

          status = m_tuple->addIndexedItem("vx",    m_ngch, m_vx);
          status = m_tuple->addIndexedItem("vy",    m_ngch, m_vy);
          status = m_tuple->addIndexedItem("vz",    m_ngch, m_vz);
          status = m_tuple->addIndexedItem("vr",    m_ngch, m_vr);

          status = m_tuple->addIndexedItem("px",    m_ngch, m_px);
          status = m_tuple->addIndexedItem("py",    m_ngch, m_py);
          status = m_tuple->addIndexedItem("pz",    m_ngch, m_pz);
          status = m_tuple->addIndexedItem("p",     m_ngch, m_p);
          status = m_tuple->addIndexedItem("cost",  m_ngch, m_cost);

          status = m_tuple->addIndexedItem("probPH", m_ngch, m_probPH);
          status = m_tuple->addIndexedItem("normPH", m_ngch, m_normPH);
          status = m_tuple->addIndexedItem("chie",   m_ngch, m_chie);
          status = m_tuple->addIndexedItem("chimu",  m_ngch, m_chimu);
          status = m_tuple->addIndexedItem("chipi",  m_ngch, m_chipi);
          status = m_tuple->addIndexedItem("chik",   m_ngch, m_chik);
          status = m_tuple->addIndexedItem("chip",   m_ngch, m_chip);
          status = m_tuple->addIndexedItem("ghit",   m_ngch, m_ghit);
          status = m_tuple->addIndexedItem("thit",   m_ngch, m_thit);

          status = m_tuple->addIndexedItem("e_emc",  m_ngch, m_e_emc);

          status = m_tuple->addIndexedItem("qual_etof", m_ngch, m_qual_etof);
          status = m_tuple->addIndexedItem("tof_etof",  m_ngch, m_tof_etof);
          status = m_tuple->addIndexedItem("te_etof",   m_ngch, m_te_etof);
          status = m_tuple->addIndexedItem("tmu_etof",  m_ngch, m_tmu_etof);
          status = m_tuple->addIndexedItem("tpi_etof",  m_ngch, m_tpi_etof);
          status = m_tuple->addIndexedItem("tk_etof",   m_ngch, m_tk_etof);
          status = m_tuple->addIndexedItem("tp_etof",   m_ngch, m_tp_etof);

          status = m_tuple->addIndexedItem("qual_btof1", m_ngch, m_qual_btof1);
          status = m_tuple->addIndexedItem("tof_btof1",  m_ngch, m_tof_btof1);
          status = m_tuple->addIndexedItem("te_btof1",   m_ngch, m_te_btof1);
          status = m_tuple->addIndexedItem("tmu_btof1",  m_ngch, m_tmu_btof1);
          status = m_tuple->addIndexedItem("tpi_btof1",  m_ngch, m_tpi_btof1);
          status = m_tuple->addIndexedItem("tk_btof1",   m_ngch, m_tk_btof1);
          status = m_tuple->addIndexedItem("tp_btof1",   m_ngch, m_tp_btof1);

          status = m_tuple->addIndexedItem("qual_btof2", m_ngch, m_qual_btof2);
          status = m_tuple->addIndexedItem("tof_btof2",  m_ngch, m_tof_btof2);
          status = m_tuple->addIndexedItem("te_btof2",   m_ngch, m_te_btof2);
          status = m_tuple->addIndexedItem("tmu_btof2",  m_ngch, m_tmu_btof2);
          status = m_tuple->addIndexedItem("tpi_btof2",  m_ngch, m_tpi_btof2);
          status = m_tuple->addIndexedItem("tk_btof2",   m_ngch, m_tk_btof2);
          status = m_tuple->addIndexedItem("tp_btof2",   m_ngch, m_tp_btof2);

      } else {
          log << MSG::ERROR << "Can not book N-tuple:" << long(m_tuple) << endreq;
      }
  }

//--------end of book--------
  
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAKsKpiDEDX::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // DQA
  // Add the line below at the beginning of execute()
//  setFilterPassed(false);

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_runNo = eventHeader->runNumber();
  m_event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
      << m_runNo << " , "
      << m_event <<endreq;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
//  m_nchrg = evtRecEvent->totalCharged();
//  m_nneu  = evtRecEvent->totalNeutral(); 
  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  if(evtRecEvent->totalNeutral()>100) {
      return StatusCode::SUCCESS;
  }

  Vint iGood;
  iGood.clear();

  Hep3Vector xorigin(0,0,0);
 
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){ 
  double* dbv = vtxsvc->PrimaryVertex();
  double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
    log << MSG::INFO
        <<"xorigin.x="<<xorigin.x()<<", "
        <<"xorigin.y="<<xorigin.y()<<", "
        <<"xorigin.z="<<xorigin.z()<<", "
        <<endreq  ;
  }

  int nCharge = 0;
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if (!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      
      double pch =mdcTrk->p();
      double x0  =mdcTrk->x(); 
      double y0  =mdcTrk->y(); 
      double z0  =mdcTrk->z(); 
      double phi0=mdcTrk->helix(1);
      double xv=xorigin.x();
      double yv=xorigin.y();
      double Rxy=fabs((x0-xv)*cos(phi0)+(y0-yv)*sin(phi0));
      double vx0 = x0;
      double vy0 = y0;
      double vz0 = z0-xorigin.z();
      double vr0 = Rxy;
      double Vct=cos(mdcTrk->theta());
      
      HepVector a = mdcTrk->helix();
      HepSymMatrix Ea = mdcTrk->err();
      HepPoint3D point0(0.,0.,0.);   // the initial point for MDC recosntruction
      HepPoint3D IP(xorigin[0],xorigin[1],xorigin[2]); 
      VFHelix helixip(point0,a,Ea); 
      helixip.pivot(IP);
      HepVector vecipa = helixip.a();
      double  Rvxy0=fabs(vecipa[0]);  //the nearest distance to IP in xy plane
      double  Rvz0=vecipa[3];         //the nearest distance to IP in z direction
      double  Rvphi0=vecipa[1];
//      m_rvxy0=Rvxy0;
//      m_rvz0=Rvz0;
//      m_rvphi0=Rvphi0;

      if(fabs(Rvz0) >= m_vz0cut) continue;
      if(fabs(Rvxy0) >= m_vr0cut) continue;

//    if(fabs(Vct)>=m_cthcut) continue;
//    iGood.push_back((*itTrk)->trackId());
      iGood.push_back(i);
      nCharge += mdcTrk->charge();
  }
  
  //
  // Finish Good Charged Track Selection
  //
  int m_ngch = iGood.size();
  log << MSG::DEBUG << "ngood, totcharge = " << m_ngch << " , " << nCharge << endreq;
  if((m_ngch != 4)||(nCharge!=0)){
      return StatusCode::SUCCESS;
  }

  // Particle ID
  // 
  Vint ipip, ipim, ikp, ikm, ipp, ipm;
  ipip.clear();
  ipim.clear();
  ikp.clear();
  ikm.clear();
  ipp.clear();
  ipm.clear();

  Vp4 p_pip, p_pim, p_kp, p_km, p_pp, p_pm ;
  p_pip.clear();
  p_pim.clear();
  p_kp.clear();
  p_km.clear();
  p_pp.clear();
  p_pm.clear();

  ParticleID *pid = ParticleID::instance();
  for(int i = 0; i < m_ngch; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    //    if(pid) delete pid;
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
//    pid->usePidSys(pid->useDedx()); // just use dedx PID
    pid->usePidSys( pid->useTof1() | pid->useTof2() | pid->useTofE() | pid->useTofQ());
    pid->identify(pid->onlyPionKaonProton());                  // seperater Pion/Kaon/Proton
    //    pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    //    pid->identify(pid->onlyPion());
    //    pid->identify(pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;

    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    RecMdcKalTrack* mdcKalTrk = 0 ; 
    if((*itTrk)->isMdcKalTrackValid()) mdcKalTrk = (*itTrk)->mdcKalTrack(); 

    double  prob_pi = pid->probPion();
    double  prob_K  = pid->probKaon();
    double  prob_p  = pid->probProton();
    //    std::cout << "prob "<< prob_pi << ", "<< prob_K << ", "<< prob_p << std::endl; 
    HepLorentzVector ptrk;
    ptrk.setPx(mdcTrk->px()) ; 
    ptrk.setPy(mdcTrk->py()) ;
    ptrk.setPz(mdcTrk->pz()) ; 
    double p3 = ptrk.mag()   ; 
    
    if (prob_pi > prob_K && prob_pi > prob_p) {
      m_pidcode[i]=2;
      m_pidprob[i]=pid->prob(2);
      m_pidchiDedx[i]=pid->chiDedx(2);
      m_pidchiTof1[i]=pid->chiTof1(2);
      m_pidchiTof2[i]=pid->chiTof2(2);    	
      ptrk.setE(sqrt(p3*p3 + xmass[2]*xmass[2])) ; 
      if(mdcTrk->charge() > 0) {
	ipip.push_back(iGood[i]);
	p_pip.push_back(ptrk);
      }
      if (mdcTrk->charge() < 0) {
	ipim.push_back(iGood[i]);
	p_pim.push_back(ptrk); 
      }
    }

    if (prob_K > prob_pi && prob_K > prob_p) {
      m_pidcode[i]=3;
      m_pidprob[i]=pid->prob(3);
      m_pidchiDedx[i]=pid->chiDedx(3);
      m_pidchiTof1[i]=pid->chiTof1(3);
      m_pidchiTof2[i]=pid->chiTof2(3);    	
      ptrk.setE(sqrt(p3*p3 + xmass[3]*xmass[3])) ; 
      if(mdcTrk->charge() > 0) {
	ikp.push_back(iGood[i]);
	p_kp.push_back(ptrk);
      }
      if (mdcTrk->charge() < 0) {
	ikm.push_back(iGood[i]);
	p_km.push_back(ptrk); 
      }
    }

    if (prob_p > prob_pi && prob_p > prob_K) {
      m_pidcode[i]=4;
      m_pidprob[i]=pid->prob(4);
      m_pidchiDedx[i]=pid->chiDedx(4);
      m_pidchiTof1[i]=pid->chiTof1(4);
      m_pidchiTof2[i]=pid->chiTof2(4);    	
      ptrk.setE(sqrt(p3*p3 + xmass[4]*xmass[4])) ; 
      if(mdcTrk->charge() > 0) {
	ipp.push_back(iGood[i]);
	p_pp.push_back(ptrk);
      }
      if (mdcTrk->charge() < 0) {
	ipm.push_back(iGood[i]);
	p_pm.push_back(ptrk); 
      }
    }
  }

  m_npip= ipip.size() ; 
  m_npim= ipim.size() ;
  m_nkp = ikp.size() ; 
  m_nkm = ikm.size() ;
  m_np  = ipp.size() ; 
  m_npb = ipm.size() ;

//  cout<<"m_npip*m_npim: "<<m_npip*m_npim<<endl;
  if( m_npip*m_npim != 2 ) { 
      return StatusCode::SUCCESS;  
  } 
//  cout<<"m_nkp+m_nkm: "<<m_nkp+m_nkm<<endl;
  if( m_nkp+m_nkm != 1 ) { 
      return StatusCode::SUCCESS;  
  }
//  cout<<"haha"<<endl;

  // Vertex Fit
  HepPoint3D vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double bx = 1E+6;
  double by = 1E+6;
  double bz = 1E+6;
  Evx[0][0] = bx*bx;
  Evx[1][1] = by*by;
  Evx[2][2] = bz*bz;

  VertexParameter vxpar;
  vxpar.setVx(vx);
  vxpar.setEvx(Evx);

  VertexFit *vtxfit_s = VertexFit::instance(); // S second vertex 
  VertexFit *vtxfit_p = VertexFit::instance(); // P primary vertex 
  SecondVertexFit *vtxfit2 = SecondVertexFit::instance();

  RecMdcKalTrack *pi1KalTrk, *pi2KalTrk, *pi3KalTrk, *k1KalTrk; 
  RecMdcKalTrack *pipKalTrk, *pimKalTrk, *piKalTrk, *kKalTrk; 
  WTrackParameter wks,vwks; 

  double chi_temp = 999.0; 
  double mks_temp = 10.0 ; 
  bool okloop=false; 
  for(unsigned int i1 = 0; i1 < m_npip; i1++) { //pion plus
    RecMdcKalTrack *pi1KalTrk = (*(evtRecTrkCol->begin()+ipip[i1]))-> mdcKalTrack();
    pi1KalTrk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpi1trk(xmass[2], pi1KalTrk->getZHelix(), pi1KalTrk->getZError());

    for(unsigned int i2 = 0; i2 < m_npim; i2++) { //pion minus
      RecMdcKalTrack *pi2KalTrk = (*(evtRecTrkCol->begin()+ipim[i2]))-> mdcKalTrack();
      pi2KalTrk->setPidType(RecMdcKalTrack::pion);
      WTrackParameter wpi2trk(xmass[2], pi2KalTrk->getZHelix(), pi2KalTrk->getZError());
   
      vtxfit_s->init();
      vtxfit_s->AddTrack(0, wpi1trk);
      vtxfit_s->AddTrack(1, wpi2trk);
      vtxfit_s->AddVertex(0, vxpar, 0, 1);

      if(!(vtxfit_s->Fit(0))) continue;
      vtxfit_s->BuildVirtualParticle(0);
      m_vfits_chi = vtxfit_s->chisq(0);
      WTrackParameter wkshort = vtxfit_s->wVirtualTrack(0);
      VertexParameter vparks  = vtxfit_s->vpar(0);

      m_vfits_vx = (vparks.Vx())[0]; 
      m_vfits_vy = (vparks.Vx())[1]; 
      m_vfits_vz = (vparks.Vx())[2]; 
      m_vfits_vr = sqrt(m_vfits_vx*m_vfits_vx + m_vfits_vy*m_vfits_vy) ;

      if ( m_npip == 2 ) {
	int j = i1 ;
	int jj =  ( i1 == 1 ) ? 0 : 1; 
	pi3KalTrk   = (*(evtRecTrkCol->begin()+ipip[jj]))->mdcKalTrack();
	k1KalTrk    = (*(evtRecTrkCol->begin()+ikm[0]))->mdcKalTrack();
      }
      if (m_npim == 2 ) {
	int j = i2 ;
	int jj =  ( i2 == 1 ) ? 0 : 1; 
	pi3KalTrk   = (*(evtRecTrkCol->begin()+ipim[jj]))->mdcKalTrack();
	k1KalTrk    = (*(evtRecTrkCol->begin()+ikp[0]))->mdcKalTrack();
      }

      pi3KalTrk->setPidType(RecMdcKalTrack::pion);
      WTrackParameter wpi3trk( xmass[2], pi3KalTrk->getZHelix(), pi3KalTrk->getZError());
      k1KalTrk->setPidType(RecMdcKalTrack::kaon);
      WTrackParameter wk1trk( xmass[3], k1KalTrk->getZHelixK(), k1KalTrk->getZErrorK());
   
      vtxfit_p->init();
//      vtxfit_p->AddTrack(0, wkshort);
      vtxfit_p->AddTrack(0, wpi3trk);
      vtxfit_p->AddTrack(1, wk1trk);
      vtxfit_p->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit_p->Fit(0))) continue;

      m_vfitp_chi = vtxfit_p->chisq(0) ; 

      VertexParameter primaryVpar = vtxfit_p->vpar(0);
      m_vfitp_vx = (primaryVpar.Vx())[0]; 
      m_vfitp_vy = (primaryVpar.Vx())[1]; 
      m_vfitp_vz = (primaryVpar.Vx())[2]; 
      m_vfitp_vr = sqrt(m_vfitp_vx*m_vfitp_vx + m_vfitp_vy*m_vfitp_vy); 

      vtxfit2->init();
      vtxfit2->setPrimaryVertex(vtxfit_p->vpar(0));
      vtxfit2->AddTrack(0, wkshort);
      vtxfit2->setVpar(vtxfit_s->vpar(0));
      if(!vtxfit2->Fit()) continue;

      if ( fabs(((vtxfit2->wpar()).p()).m()-mks0) < mks_temp ) {
	mks_temp = fabs(((vtxfit2->wpar()).p()).m()-mks0) ; 
      
	okloop = true; 

	wks = vtxfit2->wpar();
	m_vfit2_mks = (wks.p()).m(); 
	m_vfit2_chi = vtxfit2->chisq();
	m_vfit2_ct  = vtxfit2->ctau();
	m_vfit2_dl  = vtxfit2->decayLength();
	m_vfit2_dle = vtxfit2->decayLengthError();

	pipKalTrk = pi1KalTrk ; 
	pimKalTrk = pi2KalTrk ; 
	piKalTrk  = pi3KalTrk ; 
	kKalTrk   = k1KalTrk ; 

      }
    }
  }

  if (! okloop ) { 
      return StatusCode::SUCCESS;
  }

  pipKalTrk->setPidType(RecMdcKalTrack::pion);
  pimKalTrk->setPidType(RecMdcKalTrack::pion);
  piKalTrk->setPidType(RecMdcKalTrack::pion);
  kKalTrk->setPidType(RecMdcKalTrack::kaon);

  WTrackParameter wpip(xmass[2], pipKalTrk->getZHelix(), pipKalTrk->getZError()); //pi+ from Ks
  WTrackParameter wpim(xmass[2], pimKalTrk->getZHelix(), pimKalTrk->getZError()); //pi- from Ks

  WTrackParameter wpi(xmass[2], piKalTrk->getZHelix(), piKalTrk->getZError());
  WTrackParameter wk(xmass[3],  kKalTrk->getZHelixK(), kKalTrk->getZErrorK());

  //
  // check good charged track's infomation
  int ii ;
  for(int j=0; j<m_ngch; j++){
      m_charge[j] = 9999.0;
      m_vx0[j] = 9999.0;
      m_vy0[j] = 9999.0;
      m_vz0[j] = 9999.0;
      m_vr0[j] = 9999.0;

      m_vx[j] = 9999.0;
      m_vy[j] = 9999.0;
      m_vz[j] = 9999.0;
      m_vr[j] = 9999.0;

      m_px[j] = 9999.0;
      m_py[j] = 9999.0;
      m_pz[j] = 9999.0;
      m_p[j] = 9999.0;
      m_cost[j] = 9999.0;

      m_probPH[j] = 9999.0;
      m_normPH[j] = 9999.0;
      m_chie[j] = 9999.0;
      m_chimu[j] = 9999.0;
      m_chipi[j] = 9999.0;
      m_chik[j] = 9999.0;
      m_chip[j] = 9999.0;
      m_ghit[j] = 9999.0;
      m_thit[j] = 9999.0;

      m_e_emc[j] = 9999.0;

      m_qual_etof[j] = 9999.0;
      m_tof_etof[j] = 9999.0;
      m_te_etof[j] = 9999.0;
      m_tmu_etof[j] = 9999.0;
      m_tpi_etof[j] = 9999.0;
      m_tk_etof[j] = 9999.0;
      m_tp_etof[j] = 9999.0;

      m_qual_btof1[j] = 9999.0;
      m_tof_btof1[j] = 9999.0;
      m_te_btof1[j] = 9999.0;
      m_tmu_btof1[j] = 9999.0;
      m_tpi_btof1[j] = 9999.0;
      m_tk_btof1[j] = 9999.0;
      m_tp_btof1[j] = 9999.0;

      m_qual_btof2[j] = 9999.0;
      m_tof_btof2[j] = 9999.0;
      m_te_btof2[j] = 9999.0;
      m_tmu_btof2[j] = 9999.0;
      m_tpi_btof2[j] = 9999.0;
      m_tk_btof2[j] = 9999.0;
      m_tp_btof2[j] = 9999.0;

      m_pidcode[j] = 9999.0;  	
      m_pidprob[j] = 9999.0;
      m_pidchiDedx[j] = 9999.0;
      m_pidchiTof1[j] = 9999.0;
      m_pidchiTof2[j] = 99999.0; 
  }

  for(int i = 0; i < m_ngch; i++ ){

    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
    if(!(*itTrk)->isMdcTrackValid()) continue;  //  MDC information 
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();

    if ( mdcKalTrk == pipKalTrk ) {
      ii = 0 ;
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
    }
    if ( mdcKalTrk == pimKalTrk ) {
      ii = 1 ;
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
    }  
    if ( mdcKalTrk == piKalTrk  ) {
      ii = 2 ;
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
    }  
    if ( mdcKalTrk == kKalTrk   ) {
      ii = 3 ;  
      mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
    }

    m_charge[ii] = mdcTrk->charge();
    double x0=mdcTrk->x();
    double y0=mdcTrk->y();
    double z0=mdcTrk->z();
    double phi0=mdcTrk->helix(1);
    double xv=xorigin.x();
    double yv=xorigin.y();
    double zv=xorigin.z();
    double rv=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);

    m_vx0[ii] = x0-xv ;
    m_vy0[ii] = y0-yv ;
    m_vz0[ii] = z0-zv ;
    m_vr0[ii] = rv ;

    x0=mdcKalTrk->x();
    y0=mdcKalTrk->y();
    z0=mdcKalTrk->z();
    rv=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
    
    m_vx[ii] = x0-xv ;
    m_vy[ii] = y0-yv ;
    m_vz[ii] = z0-zv ;
    m_vr[ii] = rv ;

    m_px[ii]   = mdcKalTrk->px();
    m_py[ii]   = mdcKalTrk->py();
    m_pz[ii]   = mdcKalTrk->pz();
    m_p[ii]    = mdcKalTrk->p();
    m_cost[ii] = mdcKalTrk->pz()/mdcKalTrk->p();
      
    double ptrk = mdcKalTrk->p() ; 
    if((*itTrk)->isMdcDedxValid()) { // DEDX information 
      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
      m_probPH[ii]= dedxTrk->probPH();
      m_normPH[ii]= dedxTrk->normPH();
      
      m_chie[ii]  = dedxTrk->chiE();
      m_chimu[ii] = dedxTrk->chiMu();
      m_chipi[ii] = dedxTrk->chiPi();
      m_chik[ii]  = dedxTrk->chiK();
      m_chip[ii]  = dedxTrk->chiP();
      m_ghit[ii]  = dedxTrk->numGoodHits();
      m_thit[ii]  = dedxTrk->numTotalHits();
    }
      
    if((*itTrk)->isEmcShowerValid()) {
      RecEmcShower *emcTrk = (*itTrk)->emcShower();
      m_e_emc[ii] = emcTrk->energy();
    }

    if((*itTrk)->isTofTrackValid()) {  //TOF information
      SmartRefVector<RecTofTrack> tofTrkCol = (*itTrk)->tofTrack();
      
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();

      for(;iter_tof != tofTrkCol.end(); iter_tof++ ) { 
        TofHitStatus *status = new TofHitStatus; 
        status->setStatus((*iter_tof)->status());

        if(!(status->is_barrel())){//endcap
          if( !(status->is_counter()) ) continue; // ? 
          if( status->layer()!=0 ) continue;//layer1
          double path=(*iter_tof)->path(); // ? 
          double tof  = (*iter_tof)->tof();
          double ph   = (*iter_tof)->ph();
          double rhit = (*iter_tof)->zrhit();
          double qual = 0.0 + (*iter_tof)->quality();
          double cntr = 0.0 + (*iter_tof)->tofID();
          double texp[5];
          for(int j = 0; j < 5; j++) {
            double gb = ptrk/xmass[j];
            double beta = gb/sqrt(1+gb*gb);
            texp[j] = path /beta/velc;
          }
	  
          m_qual_etof[ii]  = qual;
	  m_tof_etof[ii]   = tof ; 
	}
        else {//barrel
          if( !(status->is_counter()) ) continue; // ? 
          if(status->layer()==1){ //layer1
            double path=(*iter_tof)->path(); // ? 
            double tof  = (*iter_tof)->tof();
            double ph   = (*iter_tof)->ph();
            double rhit = (*iter_tof)->zrhit();
            double qual = 0.0 + (*iter_tof)->quality();
            double cntr = 0.0 + (*iter_tof)->tofID();
            double texp[5];
            for(int j = 0; j < 5; j++) {
              double gb = ptrk/xmass[j];
              double beta = gb/sqrt(1+gb*gb);
              texp[j] = path /beta/velc;
            }
	    
            m_qual_btof1[ii]  = qual;
	    m_tof_btof1[ii]   = tof ; 
          }

          if(status->layer()==2){//layer2
            double path=(*iter_tof)->path(); // ? 
            double tof  = (*iter_tof)->tof();
            double ph   = (*iter_tof)->ph();
            double rhit = (*iter_tof)->zrhit();
            double qual = 0.0 + (*iter_tof)->quality();
            double cntr = 0.0 + (*iter_tof)->tofID();
            double texp[5];
            for(int j = 0; j < 5; j++) {
              double gb = ptrk/xmass[j];
              double beta = gb/sqrt(1+gb*gb);
              texp[j] = path /beta/velc;
            }
 
            m_qual_btof2[ii]  = qual;
	    m_tof_btof2[ii]   = tof ; 
          } 
	}
      }
    }
  }

  // Kinamatic Fit
  KinematicFit * kmfit = KinematicFit::instance();
  
  double ecms  = 3.097; 
  double chisq = 9999.;
    m_4c_chi2  = 9999.;
    m_4c_mks   = 10.0;
    m_4c_mkspi = 10.0;
    m_4c_mksk  = 10.0;
    m_4c_mkpi  = 10.0;
    m_4c_ks_px = 10.0;
    m_4c_ks_py = 10.0;
    m_4c_ks_pz = 10.0;
    m_4c_ks_p  = 10.0;
    m_4c_ks_cos= 10.0;
    
    kmfit->init();
    kmfit->AddTrack(0, wpi);
    kmfit->AddTrack(1, wk);
    kmfit->AddTrack(2, wks);
    kmfit->AddFourMomentum(0, p_cms);
    bool oksq = kmfit->Fit();
    if(oksq) {
      chisq = kmfit->chisq(); 

      HepLorentzVector pk    = kmfit->pfit(1);
      HepLorentzVector pks   = kmfit->pfit(2);
      HepLorentzVector pkspi = kmfit->pfit(0) + kmfit->pfit(2);
      HepLorentzVector pksk  = kmfit->pfit(1) + kmfit->pfit(2);
      HepLorentzVector pkpi  = kmfit->pfit(0) + kmfit->pfit(1);
      
      pks.boost(u_cms); 
      pkspi.boost(u_cms); 
      pksk.boost(u_cms);
      pkpi.boost(u_cms); 

      m_4c_chi2  = chisq ; 
      m_4c_mks   = pks.m(); 
      m_4c_mkspi = pkspi.m(); 
      m_4c_mksk  = pksk.m(); 
      m_4c_mkpi  = pkpi.m(); 
      
      m_4c_ks_px = pks.px() ; 
      m_4c_ks_py = pks.py() ; 
      m_4c_ks_pz = pks.pz() ; 
      m_4c_ks_p = (pks.vect()).mag() ;
      m_4c_ks_cos = m_4c_ks_pz/m_4c_ks_p ;

    }

    chisq = 9999.;
    m_chi2_fs4c  = 9999.;
    m_mks_fs4c   = 10.0;
    m_mkspi_fs4c = 10.0;
    m_mksk_fs4c  = 10.0;
    m_mkpi_fs4c  = 10.0;

    kmfit->init();
    kmfit->AddTrack(0, wpip);
    kmfit->AddTrack(1, wpim);
    kmfit->AddTrack(2, wpi);
    kmfit->AddTrack(3, wk);
    kmfit->AddFourMomentum(0, p_cms);
    oksq = kmfit->Fit();
    if(oksq) {
      chisq =  kmfit->chisq();

      HepLorentzVector pks = kmfit->pfit(0) + kmfit->pfit(1);
      HepLorentzVector pkspi = pks  + kmfit->pfit(2);
      HepLorentzVector pksk  = pks  + kmfit->pfit(3);
      HepLorentzVector pkpi  = kmfit->pfit(2) + kmfit->pfit(3);

      pks.boost(u_cms); 
      pkspi.boost(u_cms); 
      pksk.boost(u_cms);
      pkpi.boost(u_cms); 
      
      m_chi2_fs4c  = chisq ;
      m_mks_fs4c   = pks.m(); 
      m_mkspi_fs4c = pkspi.m(); 
      m_mksk_fs4c  = pksk.m(); 
      m_mkpi_fs4c  = pkpi.m(); 
    }

    // finale selection
    if(chisq > 20) { return StatusCode::SUCCESS; }  //4C chi2
    if(m_vfit2_dl < 0.5) { return StatusCode::SUCCESS; } // Ks decay length
    if(fabs(m_4c_mks-mks0) > 0.01) { return StatusCode::SUCCESS; } // Ks mass
    if(m_4c_mkspi < 1.25) { return StatusCode::SUCCESS; } // Kspi mass

    // DQA
    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/DQAKsKpiDEDX/hks_dl", h).isSuccess()) {
        h->Fill(m_vfit2_dl);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hks_dl" << endreq;
    }

    if (m_thsvc->getHist("/DQAHist/DQAKsKpiDEDX/hks_m", h).isSuccess()) {
        h->Fill(m_4c_mks);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hks_m" << endreq;
    }

    if (m_thsvc->getHist("/DQAHist/DQAKsKpiDEDX/hkspi_m", h).isSuccess()) {
        h->Fill(m_4c_mkspi);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hkspi_m" << endreq;
    }

    if (m_thsvc->getHist("/DQAHist/DQAKsKpiDEDX/hks_p", h).isSuccess()) {
        h->Fill(m_4c_ks_p);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hks_p" << endreq;
    }

    if (m_thsvc->getHist("/DQAHist/DQAKsKpiDEDX/hkpi_m", h).isSuccess()) {
        h->Fill(m_4c_mkpi);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hkpi_m" << endreq;
    }

  ////////////////////////////////////////////////////////////
  // DQA
  // set tag and quality

  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
  if(m_npip==2 && m_npim==1){
      (*(evtRecTrkCol->begin()+ipip[0]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ipip[1]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ipim[0]))->setPartId(2);      
      (*(evtRecTrkCol->begin()+ikm[0]))->setPartId(4);
  }
  if(m_npip==1 && m_npim==2){
      (*(evtRecTrkCol->begin()+ipip[0]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ipim[0]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ipim[1]))->setPartId(2);      
      (*(evtRecTrkCol->begin()+ikp[0]))->setPartId(4);
  }
  // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  if(m_npip==2 && m_npim==1){
      (*(evtRecTrkCol->begin()+ipip[0]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ipip[1]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ipim[0]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ikm[0]))->setQuality(2);
  }
  if(m_npip==1 && m_npim==2){
      (*(evtRecTrkCol->begin()+ipip[0]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ipim[0]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ipim[1]))->setQuality(2);
      (*(evtRecTrkCol->begin()+ikp[0]))->setQuality(2);
  }
  // DQA
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);
  ////////////////////////////////////////////////////////////
 
  m_tuple->write();

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAKsKpiDEDX::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


