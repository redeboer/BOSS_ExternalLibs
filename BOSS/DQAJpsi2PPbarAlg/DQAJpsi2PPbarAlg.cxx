#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"

// #include "TMath.h"
#include "TH1F.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "VertexFit/IVertexDbSvc.h"

#include "ParticleID/ParticleID.h"


#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "DQAJpsi2PPbarAlg/DQAJpsi2PPbarAlg.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

#include <vector>
//const double twopi = 6.2831853;
//const double pi = 3.1415927;
const double mpi0 = 0.134977;
const double mks0 = 0.497648;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//const double velc = 29.9792458;  tof_path unit in cm.
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

//boosted
//const HepLorentzVector p_cms(0.0407, 0.0, 0.0, 3.686);
const HepLorentzVector p_cms(0.034067,0.0,0.0,3.097);
const Hep3Vector u_cms = -p_cms.boostVector();


//declare one counter
static int counter[10]={0,0,0,0,0,0,0,0,0,0};
/////////////////////////////////////////////////////////////////////////////

DQAJpsi2PPbarAlg::DQAJpsi2PPbarAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=5.0);
  declareProperty("Vz0cut", m_vz0cut=20.0);
  declareProperty("Vr1cut", m_vr1cut=1.0);
  declareProperty("Vz1cut", m_vz1cut=10.0);
  declareProperty("Vctcut", m_cthcut=0.93);
  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
  declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
  declareProperty("Test4C", m_test4C = 1);
  declareProperty("Test5C", m_test5C = 1);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);
  declareProperty("GammaAngCut", m_gammaAngCut=20.0);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAJpsi2PPbarAlg::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;



  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // "DQAHist" is fixed
  // "DQAJpsi2PPbar" is control sample name, just as ntuple case.
  TH1F* hbst_p = new TH1F("bst_p", "bst_p", 80, 1.15, 1.31);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/hbst_p", hbst_p).isFailure()) {
      log << MSG::ERROR << "Couldn't register bst_p" << endreq;
  }

  TH1F* hbst_cos = new TH1F("bst_cos", "bst_cos", 20, -1.0, 1.0);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/hbst_cos", hbst_cos).isFailure()) {
      log << MSG::ERROR << "Couldn't register bst_cos" << endreq;
  }

  TH1F* hmpp = new TH1F("mpp", "mpp", 100, 3.05, 3.15);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/hmpp", hmpp).isFailure()) {
      log << MSG::ERROR << "Couldn't register mpp" << endreq;
  }

  TH1F* hangle = new TH1F("angle", "angle", 50, 175.0, 180.);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/hangle", hangle).isFailure()) {
      log << MSG::ERROR << "Couldn't register angle" << endreq;
  }

  TH1F* hdeltatof = new TH1F("deltatof", "deltatof", 50, -10., 10.);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/hdeltatof", hdeltatof).isFailure()) {
      log << MSG::ERROR << "Couldn't register deltatof" << endreq;
  }

  TH1F* he_emc1 = new TH1F("e_emc1", "e_emc1", 100, 0.0, 2.0);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/he_emc1", he_emc1).isFailure()) {
      log << MSG::ERROR << "Couldn't register e_emc1" << endreq;
  }

  TH1F* he_emc2 = new TH1F("e_emc2", "e_emc2", 100, 0.0, 2.0);
  if(m_thsvc->regHist("/DQAHist/DQAJpsi2PPbar/he_emc2", he_emc2).isFailure()) {
      log << MSG::ERROR << "Couldn't register e_emc2" << endreq;
  }

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the control sample name, the first letter is in upper format. eg. "DQAJpsi2PPbar"

  NTuplePtr nt1(ntupleSvc(), "DQAFILE/DQAJpsi2PPbar");
  if ( nt1 ) m_tuple = nt1;
  else {
    m_tuple = ntupleSvc()->book ("DQAFILE/DQAJpsi2PPbar", CLID_ColumnWiseTuple, "N-Tuple");
    if ( m_tuple )    {
      status = m_tuple->addItem ("runNo",  m_runNo);
      status = m_tuple->addItem ("event",  m_event);
      status = m_tuple->addItem ("Nchrg",  m_nchrg);
      status = m_tuple->addItem ("Nneu",   m_nneu);
      status = m_tuple->addItem ("ngch",   m_ngch, 0, 10);

      status = m_tuple->addIndexedItem ("charge",m_ngch, m_charge);
      status = m_tuple->addIndexedItem ("vx0",    m_ngch, m_vx0);
      status = m_tuple->addIndexedItem ("vy0",    m_ngch, m_vy0);
      status = m_tuple->addIndexedItem ("vz0",    m_ngch, m_vz0);
      status = m_tuple->addIndexedItem ("vr0",    m_ngch, m_vr0);

      status = m_tuple->addIndexedItem ("vx",    m_ngch, m_vx);
      status = m_tuple->addIndexedItem ("vy",    m_ngch, m_vy);
      status = m_tuple->addIndexedItem ("vz",    m_ngch, m_vz);
      status = m_tuple->addIndexedItem ("vr",    m_ngch, m_vr);

      status = m_tuple->addIndexedItem ("px",    m_ngch, m_px);
      status = m_tuple->addIndexedItem ("py",    m_ngch, m_py);
      status = m_tuple->addIndexedItem ("pz",    m_ngch, m_pz);
      status = m_tuple->addIndexedItem ("p",     m_ngch, m_p );
      status = m_tuple->addIndexedItem ("cos",   m_ngch, m_cos);

      status = m_tuple->addIndexedItem ("bst_px",    m_ngch, m_bst_px) ;
      status = m_tuple->addIndexedItem ("bst_py",    m_ngch, m_bst_py) ;
      status = m_tuple->addIndexedItem ("bst_pz",    m_ngch, m_bst_pz) ;
      status = m_tuple->addIndexedItem ("bst_p",     m_ngch, m_bst_p)  ;
      status = m_tuple->addIndexedItem ("bst_cos",   m_ngch, m_bst_cos);


      status = m_tuple->addIndexedItem ("chie"   , m_ngch, m_chie)   ;
      status = m_tuple->addIndexedItem ("chimu"  , m_ngch, m_chimu)  ;
      status = m_tuple->addIndexedItem ("chipi"  , m_ngch, m_chipi)  ;
      status = m_tuple->addIndexedItem ("chik"   , m_ngch, m_chik)   ;
      status = m_tuple->addIndexedItem ("chip"   , m_ngch, m_chip)   ;
      status = m_tuple->addIndexedItem ("ghit"   , m_ngch, m_ghit)   ;
      status = m_tuple->addIndexedItem ("thit"   , m_ngch, m_thit)   ;
      status = m_tuple->addIndexedItem ("probPH" , m_ngch, m_probPH) ;
      status = m_tuple->addIndexedItem ("normPH" , m_ngch, m_normPH) ;

      status = m_tuple->addIndexedItem ("e_emc"   , m_ngch, m_e_emc) ;


      status = m_tuple->addIndexedItem ("qual_etof" , m_ngch,   m_qual_etof  );
      status = m_tuple->addIndexedItem ("tof_etof"  , m_ngch,   m_tof_etof   );
      status = m_tuple->addIndexedItem ("te_etof"   , m_ngch,   m_te_etof    );
      status = m_tuple->addIndexedItem ("tmu_etof"  , m_ngch,   m_tmu_etof   );
      status = m_tuple->addIndexedItem ("tpi_etof"  , m_ngch,   m_tpi_etof   );
      status = m_tuple->addIndexedItem ("tk_etof"   , m_ngch,   m_tk_etof    );
      status = m_tuple->addIndexedItem ("tp_etof"   , m_ngch,   m_tp_etof    );

      status = m_tuple->addIndexedItem ("qual_btof1", m_ngch,   m_qual_btof1 );
      status = m_tuple->addIndexedItem ("tof_btof1" , m_ngch,   m_tof_btof1  );
      status = m_tuple->addIndexedItem ("te_btof1"  , m_ngch,   m_te_btof1   );
      status = m_tuple->addIndexedItem ("tmu_btof1" , m_ngch,   m_tmu_btof1  );
      status = m_tuple->addIndexedItem ("tpi_btof1" , m_ngch,   m_tpi_btof1  );
      status = m_tuple->addIndexedItem ("tk_btof1"  , m_ngch,   m_tk_btof1   );
      status = m_tuple->addIndexedItem ("tp_btof1"  , m_ngch,   m_tp_btof1   );

      status = m_tuple->addIndexedItem ("qual_btof2", m_ngch,   m_qual_btof2 );
      status = m_tuple->addIndexedItem ("tof_btof2" , m_ngch,   m_tof_btof2  );
      status = m_tuple->addIndexedItem ("te_btof2"  , m_ngch,   m_te_btof2   );
      status = m_tuple->addIndexedItem ("tmu_btof2" , m_ngch,   m_tmu_btof2  );
      status = m_tuple->addIndexedItem ("tpi_btof2" , m_ngch,   m_tpi_btof2  );
      status = m_tuple->addIndexedItem ("tk_btof2"  , m_ngch,   m_tk_btof2   );
      status = m_tuple->addIndexedItem ("tp_btof2"  , m_ngch,   m_tp_btof2   );

      status = m_tuple->addIndexedItem ("dedx_pid", m_ngch,   m_dedx_pid);
      status = m_tuple->addIndexedItem ("tof1_pid", m_ngch,   m_tof1_pid);
      status = m_tuple->addIndexedItem ("tof2_pid", m_ngch,   m_tof2_pid);
      status = m_tuple->addIndexedItem ("prob_pi", m_ngch,   m_prob_pi  );
      status = m_tuple->addIndexedItem ("prob_k",  m_ngch,   m_prob_k   );
      status = m_tuple->addIndexedItem ("prob_p",  m_ngch,   m_prob_p   );

      status = m_tuple->addItem ( "np",   m_np        );
      status = m_tuple->addItem ( "npb",  m_npb       );

      status = m_tuple->addItem ( "m2p",    m_m2p   );
      status = m_tuple->addItem ( "angle",  m_angle );
      status = m_tuple->addItem ( "deltatof", m_deltatof );

      status = m_tuple->addItem ( "vtx_m2p",    m_vtx_m2p   );
      status = m_tuple->addItem ( "vtx_angle",  m_vtx_angle );

      status = m_tuple->addItem ( "m_chi2_4c",   m_chi2_4c  );
      status = m_tuple->addItem ( "m_m2p_4c",    m_m2p_4c   );
      status = m_tuple->addItem ( "m_angle_4c",  m_angle_4c );

    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAJpsi2PPbarAlg::execute() {
  

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  setFilterPassed(false);
  
  counter[0]++;
  log << MSG::INFO << "counter[0]=" << counter[0]<< endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");   
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  m_runNo   = eventHeader->runNumber();
  m_event = eventHeader->eventNumber();
  m_nchrg = evtRecEvent->totalCharged();
  m_nneu  = evtRecEvent->totalNeutral(); 

    log << MSG::INFO <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  //
  // check x0, y0, z0, r0
  // suggest cut: |z0|<10 && r0<1
  //

  Vint iGood, iptrk, imtrk;
  iGood.clear();
  iptrk.clear();
  imtrk.clear();
  int nCharge = 0;
  Hep3Vector xorigin(0,0,0);
    
  //if (m_reader.isRunNumberValid(runNo)) {
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if  (vtxsvc->isVertexValid()){
      double* dbv = vtxsvc->PrimaryVertex(); 
      double*  vv = vtxsvc->SigmaPrimaryVertex();  
      //    HepVector dbv = m_reader.PrimaryVertex(runNo);
      //    HepVector vv = m_reader.SigmaPrimaryVertex(runNo);
      xorigin.setX(dbv[0]);
      xorigin.setY(dbv[1]);
      xorigin.setZ(dbv[2]);
      log << MSG::INFO 
	  <<"xorigin.x="<<xorigin.x()<<", "
	  <<"xorigin.y="<<xorigin.y()<<", "
	  <<"xorigin.z="<<xorigin.z()<<", "
	  <<endreq  ;
  }
  
  for (int i = 0; i < evtRecEvent->totalCharged(); i++){
      EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcTrackValid()) continue;
      if(!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      double x0=mdcTrk->x();
      double y0=mdcTrk->y();
      double z0=mdcTrk->z();
      double phi0=mdcTrk->helix(1);
      double xv=xorigin.x();
      double yv=xorigin.y();
      double zv=xorigin.z();
      double rv=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
      double cost = cos(mdcTrk->theta());
      if(fabs(z0-zv) >= m_vz1cut) continue;
      if(fabs(rv) >= m_vr1cut) continue;
      //if(fabs(cost) >= m_cthcut ) continue; 
      
      iGood.push_back(i);
      nCharge += mdcTrk->charge();

    if (mdcTrk->charge() > 0) {
        iptrk.push_back(i);
    } else {
        imtrk.push_back(i);
    }  
  }
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();
  m_ngch = iGood.size(); 
  log << MSG::INFO << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
  if((nGood != 2) || (nCharge!=0) ){
    return StatusCode::SUCCESS;
  }
  counter[1]++;
  log << MSG::INFO << "counter[1]=" << counter[1]<< endreq;

  /////////////////////////////////////////////////
  //   PID 
  /////////////////////////////////////////////////

  Vint ipp, ipm;   
  ipp.clear();  
  ipm.clear();

  Vp4  p_pp, p_pm;   
  p_pp.clear();  
  p_pm.clear();

  int ii=-1 ; 

  ParticleID *pid = ParticleID::instance();
  for(int i = 0; i < nGood; i++) {

    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    if(!(*itTrk)->isMdcTrackValid()) continue; 
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack(); 
    if(!(*itTrk)->isMdcKalTrackValid()) continue; 
    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack(); 

    //    if(pid) delete pid;
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    //    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2() | pid->useTofE() | pid->useTofQ()); 
                                                                      // use PID sub-system
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system
    //    pid->identify(pid->onlyProton());
    pid->identify(pid->onlyPionKaonProton());    // seperater Pion/Kaon/Proton
    //    pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    //    pid->identify(pid->onlyPion());
    //    pid->identify(pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;

    double prob_pi = pid->probPion();
    double prob_k  = pid->probKaon();
    double prob_p  = pid->probProton();

    //    if(pid->probPion() < 0.001 || (pid->probPion() < pid->probKaon())) continue;
    //    if(pid->probPion() < 0.001) continue;
    if (prob_p > prob_pi && prob_p > prob_k) {

      HepLorentzVector pkaltrk;

      mdcKalTrk->setPidType(RecMdcKalTrack::proton);
      pkaltrk.setPx(mdcKalTrk->px());
      pkaltrk.setPy(mdcKalTrk->py());
      pkaltrk.setPz(mdcKalTrk->pz());
      double p3 = pkaltrk.mag();
      pkaltrk.setE(sqrt(p3*p3+xmass[4]*xmass[4]));

      if(mdcTrk->charge() >0) {
	ii = 0; 
	ipp.push_back(iGood[i]);
	p_pp.push_back(pkaltrk);
      } else {
	ii = 1 ; 
	ipm.push_back(iGood[i]);
	p_pm.push_back(pkaltrk);
      }

      m_dedx_pid[ii] = pid->chiDedx(2);
      m_tof1_pid[ii] = pid->chiTof1(2);
      m_tof2_pid[ii] = pid->chiTof2(2);
      m_prob_pi[ii] = pid->probPion();
      m_prob_k[ii]  = pid->probKaon();
      m_prob_p[ii]  = pid->probProton();
      
    }
  } // with PID 

  m_np  = ipp.size();
  m_npb = ipm.size();

  //if(m_np*m_npb != 1) return SUCCESS;

  counter[2]++;
  log << MSG::INFO << "counter[2]=" << counter[2]<< endreq;
 
  ///////////////////////////////////////////////
  // read information for good charged tracks   
  ///////////////////////////////////////////////
  Vp4 p_ptrk, p_mtrk;
  p_ptrk.clear(), p_mtrk.clear();
  RecMdcKalTrack *ppKalTrk = 0 , *pmKalTrk = 0 ; 
    
   ii = -1 ; 
  for(int i = 0; i < m_ngch; i++ ){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + iGood[i];
    if(!(*itTrk)->isMdcTrackValid()) continue; 
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack(); 
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcKalTrack *mdcKalTrk = (*itTrk)->mdcKalTrack();
    
    mdcKalTrk->setPidType(RecMdcKalTrack::proton);

    if (mdcTrk->charge() > 0 ) {
      ii = 0 ; 
      ppKalTrk = mdcKalTrk ;
    }else{
      ii = 1 ; 
      pmKalTrk = mdcKalTrk ;
    }

    m_charge[ii] = mdcTrk->charge(); 

    double x0=mdcKalTrk->x();
    double y0=mdcKalTrk->y();
    double z0=mdcKalTrk->z();
    double phi0=mdcTrk->helix(1);
    double xv=xorigin.x();
    double yv=xorigin.y();
    double zv=xorigin.z();
    double rv=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);

    m_vx0[ii]    = x0 ;
    m_vy0[ii]    = y0 ;
    m_vz0[ii]    = z0 ;
    m_vr0[ii]    = sqrt((x0*x0)+(y0*y0)) ;
    
    m_vx[ii]    = x0-xv ;
    m_vy[ii]    = y0-yv ;
    m_vz[ii]    = fabs(z0-zv) ;
    m_vr[ii]    = fabs(rv) ;
    
    mdcKalTrk->setPidType(RecMdcKalTrack::proton);
    m_px[ii]   = mdcKalTrk->px();
    m_py[ii]   = mdcKalTrk->py();
    m_pz[ii]   = mdcKalTrk->pz();
    m_p[ii]    = sqrt(m_px[ii]*m_px[ii] + m_py[ii]*m_py[ii] + m_pz[ii]*m_pz[ii]);
    m_cos[ii]  = m_pz[ii]/m_p[ii];
    double temp_e = sqrt(m_p[ii]*m_p[ii] + xmass[4]*xmass[4]); 
    HepLorentzVector temp_p(m_px[ii],m_py[ii],m_pz[ii],temp_e); 
    if (i==0){
      p_ptrk.push_back(temp_p); 
    }else{
      p_mtrk.push_back(temp_p); 
    }

     
    double ptrk = m_p[ii];
    if ((*itTrk)->isMdcDedxValid()) { //Dedx information 
      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();
      m_chie[ii]  = dedxTrk->chiE();
      m_chimu[ii] = dedxTrk->chiMu();
      m_chipi[ii] = dedxTrk->chiPi();
      m_chik[ii]  = dedxTrk->chiK();
      m_chip[ii]  = dedxTrk->chiP();
      m_ghit[ii]  = dedxTrk->numGoodHits();
      m_thit[ii]  = dedxTrk->numTotalHits();
      m_probPH[ii]= dedxTrk->probPH();
      m_normPH[ii]= dedxTrk->normPH();

    }
    
    if((*itTrk)->isEmcShowerValid()) {
      RecEmcShower *emcTrk = (*itTrk)->emcShower();
      m_e_emc[ii] = emcTrk->energy();
    }
    
    
    if((*itTrk)->isTofTrackValid()) {

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
          m_te_etof[ii]    = tof - texp[0];
          m_tmu_etof[ii]   = tof - texp[1];
          m_tpi_etof[ii]   = tof - texp[2];
          m_tk_etof[ii]    = tof - texp[3];
          m_tp_etof[ii]    = tof - texp[4];
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
            m_te_btof1[ii]    = tof - texp[0];
            m_tmu_btof1[ii]   = tof - texp[1];
            m_tpi_btof1[ii]   = tof - texp[2];
            m_tk_btof1[ii]    = tof - texp[3];
            m_tp_btof1[ii]    = tof - texp[4];

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
            m_te_btof2[ii]    = tof - texp[0];
            m_tmu_btof2[ii]   = tof - texp[1];
            m_tpi_btof2[ii]   = tof - texp[2];
            m_tk_btof2[ii]    = tof - texp[3];
            m_tp_btof2[ii]    = tof - texp[4];
          } 
        }
        delete status; 
      } 
    }
  }
  counter[3]++;
  log << MSG::INFO << "counter[3]=" << counter[3]<< endreq;


  //boosted   mdcKalTrk
  //cout <<"before p_ptrk[0]="<<p_ptrk[0]<<endl;
  //cout <<"before p_mtrk[0]="<<p_mtrk[0]<<endl;
  //cout <<"_m2p = "<<(p_ptrk[0] + p_mtrk[0]).m()<<endl ;
  p_ptrk[0].boost(u_cms);
  p_mtrk[0].boost(u_cms);
  for (int i=0; i<=1; i++ ) {
    HepLorentzVector p; 
    if (i==0) p = p_ptrk[0]; 
    if (i==1) p = p_mtrk[0]; 
    
    m_bst_px[i] = p.px(); 
    m_bst_py[i] = p.py(); 
    m_bst_pz[i] = p.pz(); 
    m_bst_p[i]  = p.rho(); 
    m_bst_cos[i]= p.cosTheta(); 
  }

  m_m2p = (p_ptrk[0] + p_mtrk[0]).m();
  //cout <<"after p_ptrk[0]="<<p_ptrk[0]<<endl;
  //cout <<"after p_mtrk[0]="<<p_mtrk[0]<<endl;
  //cout <<"_m2p = "<<(p_ptrk[0] + p_mtrk[0]).m()<<endl ;
  m_angle = (p_ptrk[0].vect()).angle((p_mtrk[0]).vect()) * 180.0/(CLHEP::pi) ;
 
  //cout <<"m_angle="<<m_angle<<endl; 
  //cout <<"m_e_emc="<<m_e_emc[0]<<endl; 

  double deltatof = 0.0 ; 
  if(m_tof_btof1[0]*m_tof_btof1[1] != 0.0) deltatof+=m_tof_btof1[0]-m_tof_btof1[1];
  if(m_tof_btof2[0]*m_tof_btof2[1] != 0.0) deltatof+=m_tof_btof2[0]-m_tof_btof2[1];
  m_deltatof = deltatof ;  



  // Vertex Fit
    
  // Kinamatic Fit
      
  // finale selection
  if (fabs(m_bst_p[0]-1.232)>0.02 ) {return StatusCode::SUCCESS ; }
  if (fabs(m_bst_p[1]-1.232)>0.02 ) {return StatusCode::SUCCESS ; }
  if (fabs(m_deltatof)>4.0) {return StatusCode::SUCCESS ; }
  if (m_angle<178.0)    {return StatusCode::SUCCESS ; }
  if (m_e_emc[0]>0.7) {return StatusCode::SUCCESS ; }       

  counter[4]++;
  log << MSG::INFO << "counter[4]=" << counter[4]<< endreq;
  
  // DQA

  (*(evtRecTrkCol->begin()+iptrk[0]))->tagProton(); 	 
  (*(evtRecTrkCol->begin()+imtrk[0]))->tagProton();

  // Quality: defined by whether dE/dx or TOF is used to identify particle 	 
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration) 	 
  // 1 - only dE/dx (can be used for TOF calibration) 	 
  // 2 - only TOF (can be used for dE/dx calibration) 	 
  // 3 - Both dE/dx and TOF 	 
  (*(evtRecTrkCol->begin()+iptrk[0]))->setQuality(0); 	 
  (*(evtRecTrkCol->begin()+imtrk[0]))->setQuality(0);

  setFilterPassed(true);
  
  TH1 *h(0);
  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/hbst_p", h).isSuccess()) {
      h->Fill(m_bst_p[0]);
  } else {
      log << MSG::ERROR << "Couldn't retrieve hbst_p" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/hbst_cos", h).isSuccess()) {
      h->Fill(m_bst_cos[0]);
  } else {
      log << MSG::ERROR << "Couldn't retrieve hbst_cos" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/hmpp", h).isSuccess()) {
      h->Fill(m_m2p);
  } else {
      log << MSG::ERROR << "Couldn't retrieve hmpp" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/hangle", h).isSuccess()) {
      h->Fill(m_angle);
  } else {
      log << MSG::ERROR << "Couldn't retrieve hangle" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/hdeltatof", h).isSuccess()) {
      h->Fill(m_deltatof);
  } else {
      log << MSG::ERROR << "Couldn't retrieve hdeltatof" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/he_emc1", h).isSuccess()) {
      h->Fill(m_e_emc[0]);
  } else {
      log << MSG::ERROR << "Couldn't retrieve he_emc1" << endreq;
  }

  if (m_thsvc->getHist("/DQAHist/DQAJpsi2PPbar/he_emc2", h).isSuccess()) {
      h->Fill(m_e_emc[1]);
  } else {
      log << MSG::ERROR << "Couldn't retrieve he_emc2" << endreq;
  }




  
  m_tuple -> write();

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAJpsi2PPbarAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  
  std::cout << "*********** Singal.cxx *****************" << std::endl;
  std::cout << "the totale events is      " << counter[0] << std::endl;
  std::cout << "select good charged track " << counter[1] << std::endl;
  std::cout << "PID                       " << counter[2] << std::endl;
  std::cout << "inform. for charged track " << counter[3] << std::endl;
  std::cout << "after all selections      " << counter[4] << std::endl;
  std::cout << "****************************************" << std::endl;

  return StatusCode::SUCCESS;
}

