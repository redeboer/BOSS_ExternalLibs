#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "VertexFit/IVertexDbSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"



#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
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
#include "DQAPi2p2Alg/DQAPi2p2.h"

#include "VertexFit/KinematicFit.h"
#include "VertexFit/KalmanKinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/Helix.h"
#include "ParticleID/ParticleID.h"

#include "McTruth/McParticle.h"   
   
#include <vector>
//const double twopi = 6.2831853;
//const double pi = 3.1415927;
const double mpi = 0.13957;
const double mkaon = 0.493677;
const double mproton = 0.938272;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//const double velc = 29.9792458;  tof_path unit in cm.
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;


/////////////////////////////////////////////////////////////////////////////

DQAPi2p2::DQAPi2p2(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  //Declare the properties  
  declareProperty("saventuple", m_saventuple=false);

  declareProperty("Vr0cut", m_vr0cut=1.0);
  declareProperty("Vz0cut", m_vz0cut=5.0);
  declareProperty("EnergyThreshold", m_energyThreshold=0.04);
  declareProperty("GammaPhiCut", m_gammaPhiCut=20.0);
  declareProperty("GammaThetaCut", m_gammaThetaCut=20.0);
  declareProperty("GammaAngleCut", m_gammaAngleCut=20.0);
  declareProperty("Test4C", m_test4C = 1);
  declareProperty("Test5C", m_test5C = 1);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAPi2p2::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  Ncut0=Ncut1=Ncut2=Ncut3=Ncut4=Ncut5=Ncut6=0;  

  StatusCode status;


  NTuplePtr nt0(ntupleSvc(), "DQAFILE/pi2p2");
  if ( nt0 ) m_tuple0 = nt0;
  else {
    m_tuple0 = ntupleSvc()->book ("DQAFILE/pi2p2", CLID_ColumnWiseTuple, "ks N-Tuple example");
    if ( m_tuple0 )    {
      status = m_tuple0->addItem ("nrun",   m_nrun);
      status = m_tuple0->addItem ("nrec",   m_nrec);
      
      status = m_tuple0->addItem ("nGam",   m_nGam);
      status = m_tuple0->addItem ("nGood",   m_nGood);
      status = m_tuple0->addItem ("nCharge",   m_nCharge);
      status = m_tuple0->addItem ("npi",   m_npi); 
      status = m_tuple0->addItem ("nkaon",   m_nkaon);
      status = m_tuple0->addItem ("nproton",   m_nproton);

      status = m_tuple0->addItem ("dedxchi_e",   4, m_dedxchi_e);
      status = m_tuple0->addItem ("dedxchi_mu",   4, m_dedxchi_mu);
      status = m_tuple0->addItem ("dedxchi_pi",   4, m_dedxchi_pi);
      status = m_tuple0->addItem ("dedxchi_kaon",   4, m_dedxchi_kaon);
      status = m_tuple0->addItem ("dedxchi_proton",   4, m_dedxchi_proton);

      status = m_tuple0->addItem ("tofchi_e",   4,  m_tofchi_e);
      status = m_tuple0->addItem ("tofchi_mu",   4,  m_tofchi_mu);
      status = m_tuple0->addItem ("tofchi_pi",   4,  m_tofchi_pi);
      status = m_tuple0->addItem ("tofchi_kaon",   4,  m_tofchi_kaon);
      status = m_tuple0->addItem ("tofchi_proton",   4,  m_tofchi_proton);

      status = m_tuple0->addItem ("trackfitchi",   4,    m_trackfitchi);
      status = m_tuple0->addItem ("dedxngoodhit",   4,  m_dedxngoodhit);
      status = m_tuple0->addItem ("trackfitndof",   4,    m_trackfitndof);

      status = m_tuple0->addItem ("index_pmiss",   4,    m_index_pmiss);
      status = m_tuple0->addItem ("index_pbmiss",   4,    m_index_pbmiss);
      status = m_tuple0->addItem ("index_pipmiss",   4,    m_index_pipmiss);
      status = m_tuple0->addItem ("index_pimmiss",   4,    m_index_pimmiss);

      
      status = m_tuple0->addItem ("istat_pmiss",   m_istat_pmiss);
      status = m_tuple0->addItem ("istat_pbmiss",   m_istat_pbmiss);
      status = m_tuple0->addItem ("istat_pipmiss",   m_istat_pipmiss);
      status = m_tuple0->addItem ("istat_pimmiss",   m_istat_pimmiss);

      status = m_tuple0->addItem ("mpmiss",   m_mpmiss);
      status = m_tuple0->addItem ("mpbmiss",   m_mpbmiss);
      status = m_tuple0->addItem ("mpipmiss",   m_mpipmiss);
      status = m_tuple0->addItem ("mpimmiss",   m_mpimmiss);

      status = m_tuple0->addItem ("ppmiss",     m_ppmiss);
      status = m_tuple0->addItem ("ppbmiss",    m_ppbmiss);
      status = m_tuple0->addItem ("ppipmiss",   m_ppipmiss);
      status = m_tuple0->addItem ("ppimmiss",   m_ppimmiss);


      status = m_tuple0->addItem ("ptrk_pmiss", 4,    m_ptrk_pmiss);
      status = m_tuple0->addItem ("ptrk_pbmiss", 4,   m_ptrk_pbmiss);
      status = m_tuple0->addItem ("ptrk_pipmiss", 4,  m_ptrk_pipmiss);
      status = m_tuple0->addItem ("ptrk_pimmiss", 4,  m_ptrk_pimmiss);

      status = m_tuple0->addItem ("id_pmiss", 3,  m_id_pmiss);
      status = m_tuple0->addItem ("id_pbmiss", 3,  m_id_pbmiss);
      status = m_tuple0->addItem ("id_pipmiss", 3,  m_id_pipmiss);
      status = m_tuple0->addItem ("id_pimmiss", 3,  m_id_pimmiss);


       status = m_tuple0->addItem ("eop", 4,  m_eop);
    
      
    
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple0) << endmsg;
      return StatusCode::FAILURE;
    }
  }


 StatusCode sc;
  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAPi2p2::execute() {
  
//  std::cout << "execute()" << std::endl;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNo=eventHeader->runNumber();
  int event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
      << runNo << " , "
      << event <<endreq;
   m_nrun=runNo;
   m_nrec= event;

   setFilterPassed(false);

  if ((event%100000)==0) {cout <<"event: "<< event << endl ; }
  Ncut0++;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  //  log << MSG::INFO << "get event tag OK" << endreq;
    log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

 SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
 double temp_p_pp,temp_p_pm;

  int t_j=0; 

  //
  // check x0, y0, z0, r0
  // suggest cut: |z0|<5 && r0<1
  //
  Vint iGood, ipip, ipim , ikaonp, ikaonm, iprotonp,iprotonm;
  iGood.clear();
  ipip.clear();
  ipim.clear();
  ikaonp.clear();
  ikaonm.clear();
  iprotonp.clear();
  iprotonm.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  int nCharge = 0;

  Hep3Vector xorigin(0,0,0);
  
  //if (m_reader.isRunNumberValid(runNo)) 
   IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){
  double* dbv = vtxsvc->PrimaryVertex(); 
  double*  vv = vtxsvc->SigmaPrimaryVertex();  
//    HepVector dbv = m_reader.PrimaryVertex(runNo);
//    HepVector vv = m_reader.SigmaPrimaryVertex(runNo);
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
  }

  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
    double pch=mdcTrk->p();
    double x0=mdcTrk->x();
    double y0=mdcTrk->y();
    double z0=mdcTrk->z();
    double phi0=mdcTrk->helix(1);
    double xv=xorigin.x();
    double yv=xorigin.y();
    double Rxy=(x0-xv)*cos(phi0)+(y0-yv)*sin(phi0);
//    m_vx0 = x0;
//    m_vy0 = y0;
//    m_vz0 = z0;
//    m_vr0 = Rxy;


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

    if(fabs(Rvz0) >= m_vz0cut) continue;
    if(fabs(Rvxy0) >= m_vr0cut) continue;
    if(cos(mdcTrk->theta())>0.93) continue;
    if(pch>5) continue;

    iGood.push_back(i);
    nCharge += mdcTrk->charge();
    if(t_j<4) 
    {
    if((*itTrk)->isMdcDedxValid())
    {
      RecMdcDedx* dedxTrk = (*itTrk)->mdcDedx();

      m_dedxchi_e[t_j] = dedxTrk->chiE();
      m_dedxchi_mu[t_j] = dedxTrk->chiMu();
      m_dedxchi_pi[t_j] = dedxTrk->chiPi();
      m_dedxchi_kaon[t_j] = dedxTrk->chiK();
      m_dedxchi_proton[t_j] = dedxTrk->chiP();

      m_dedxngoodhit[t_j]  = dedxTrk->numGoodHits();
    }
    if((*itTrk)->isTofTrackValid()) 
    {
      SmartRefVector<RecTofTrack> tofTrkCol=(*itTrk)->tofTrack();
      SmartRefVector<RecTofTrack>::iterator iter_tof = tofTrkCol.begin();
      for(;iter_tof!=tofTrkCol.end();iter_tof++){
        TofHitStatus* status =new TofHitStatus;
        status->setStatus( (*iter_tof)->status() );
        if(status->is_cluster()){
          double time=(*iter_tof)->tof();
          double sigma=1.1*(*iter_tof)->sigma(0)/1000;
          double expE_tof=(*iter_tof)->texpElectron();
          double expMu_tof=(*iter_tof)->texpMuon();
          double expPi_tof=(*iter_tof)->texpPion();
          double expK_tof=(*iter_tof)->texpKaon();
          double expP_tof=(*iter_tof)->texpProton();

          if( sigma!=0 ){

            m_tofchi_e[t_j] = ( time- expE_tof  );/// sigma;
            m_tofchi_mu[t_j] = ( time- expMu_tof);/// sigma;
            m_tofchi_pi[t_j] = ( time-  expPi_tof );/// sigma;
            m_tofchi_kaon[t_j] = ( time-  expK_tof );/// sigma;
            m_tofchi_proton[t_j] = ( time-  expP_tof );/// sigma;
          }

        }
        delete status;
      }
    }

    t_j++;
    }

  }
  
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();
  m_nGood=nGood;
  m_nCharge=nCharge;
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
  if((nGood != 4)||(nCharge!=0)){
    return StatusCode::SUCCESS;
  }
  Ncut1++;

    for(int i = 0; i < nGood; i++) {
    m_eop[i] = 5.;
    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    double p = mdcTrk->p();
    m_eop[i] = 6.;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    m_eop[i]=eraw/p;
    }
  
  Vint iGam;
  iGam.clear();
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    if(i>=evtRecTrkCol->size())break;
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z());
    // find the nearest charged track
    double dthe = 200.;
    double dphi = 200.;
    double dang = 200.; 
    for(int j = 0; j < evtRecEvent->totalCharged(); j++) {
      EvtRecTrackIterator jtTrk = evtRecTrkCol->begin() + j;
      if(!(*jtTrk)->isExtTrackValid()) continue;
      RecExtTrack *extTrk = (*jtTrk)->extTrack();
      if(extTrk->emcVolumeNumber() == -1) continue;
      Hep3Vector extpos = extTrk->emcPosition();
      //      double ctht = extpos.cosTheta(emcpos);
      double angd = extpos.angle(emcpos);
      double thed = extpos.theta() - emcpos.theta();
      double phid = extpos.deltaPhi(emcpos);
      thed = fmod(thed+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
      phid = fmod(phid+CLHEP::twopi+CLHEP::twopi+pi, CLHEP::twopi) - CLHEP::pi;
      if(angd < dang){
        dang = angd;
        dthe = thed;
        dphi = phid;
      }
    }
    if(dang>=200) continue;
    double eraw = emcTrk->energy();
    dthe = dthe * 180 / (CLHEP::pi);
    dphi = dphi * 180 / (CLHEP::pi);
    dang = dang * 180 / (CLHEP::pi);
    if(eraw < m_energyThreshold) continue;
//    if((fabs(dthe) < m_gammaThetaCut) && (fabs(dphi)<m_gammaPhiCut) ) continue;
    if(fabs(dang) < m_gammaAngleCut) continue;
    double dtime = emcTrk->time();

     if(dtime<0) continue;
     if(dtime>14) continue;

    //
    // good photon cut will be set here
    //
    iGam.push_back((*itTrk)->trackId());
  }
  
  //
  // Finish Good Photon Selection
  //
  int nGam = iGam.size();
  m_nGam=nGam;

  log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral()<<endreq;
//  if(nGam<1){
//    return StatusCode::SUCCESS;
//  }
  Ncut2++;

  //
  // Assign 4-momentum to each photon
  // 

  //
  // Assign 4-momentum to each charged track
  //
    Vp4 pCh;
    pCh.clear();
    Vint iPid,iCh;
    iPid.clear();
    iCh.clear();
    int npi=0,npip=0,npim=0;
    int nkaon=0,nkaonp=0,nkaonm=0;
    int nproton=0,nprotonp=0,nprotonm=0;
  ParticleID *pid = ParticleID::instance();
  
  for(int i = 0; i < nGood; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    //    if(pid) delete pid;
    pid->init();
    pid->setMethod(pid->methodProbability());
//    pid->setMethod(pid->methodLikelihood());  //for Likelihood Method  

    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys( pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system  pid->useDedx()
    pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());    // seperater Pion/Kaon
    //  pid->identify(pid->onlyPion());
    //  pid->identify(pid->onlyKaon());
    pid->calculate();
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
//    m_ptrk_pid = mdcTrk->p();
//    m_cost_pid = cos(mdcTrk->theta());
    iCh.push_back(mdcTrk->charge());
    if(!(pid->IsPidInfoValid()))
    {
      iPid.push_back(0);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcTrk->px());
      ptrk.setPy(mdcTrk->py());
      ptrk.setPz(mdcTrk->pz());
      double p3 = ptrk.vect().mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      pCh.push_back(ptrk);

    }
    if(!(pid->IsPidInfoValid())) continue;

//  if(pid->probPion() < 0.001 || (pid->probPion() < pid->probKaon())) continue;
//    if(pid->probPion() < 0.001) continue;
//    if(pid->pdf(2)<pid->pdf(3)) continue; //  for Likelihood Method(0=electron 1=muon 2=pion 3=kaon 4=proton) 

    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
//    if (!(*itTrk)->isMdcKalTrackValid()) continue;
//    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion

     HepLorentzVector ptrk;
      ptrk.setPx(mdcTrk->px());
      ptrk.setPy(mdcTrk->py());
      ptrk.setPz(mdcTrk->pz());

     if((pid->probPion() >= pid->probKaon())&&(pid->probPion() >= pid->probProton()))
     {npi=npi+1;
      iPid.push_back(2);
      if((*itTrk)->isMdcKalTrackValid())
      {
        RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
      }
      double p3 = ptrk.vect().mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      pCh.push_back(ptrk);
      if(mdcTrk->charge()>0) npip++;
      if(mdcTrk->charge()<0) npim++;

     }
     else if((pid->probKaon() >= pid->probPion())&&(pid->probKaon() >= pid->probProton()))
     {nkaon=nkaon+1;
      iPid.push_back(3);
      if((*itTrk)->isMdcKalTrackValid())
      {
        RecMdcKalTrack::setPidType  (RecMdcKalTrack::kaon);
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
      }
      double p3 = ptrk.vect().mag();
      ptrk.setE(sqrt(p3*p3+mkaon*mkaon));
      pCh.push_back(ptrk); 
     }
     else if((pid->probProton() >= pid->probPion())&&(pid->probProton() >= pid->probKaon()))
     {nproton=nproton+1;
      iPid.push_back(4);
      if((*itTrk)->isMdcKalTrackValid())
      {
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      }
      double p3 = ptrk.vect().mag();
      ptrk.setE(sqrt(p3*p3+mproton*mproton));
      pCh.push_back(ptrk);
      if(mdcTrk->charge()>0) nprotonp++;
      if(mdcTrk->charge()<0) nprotonm++;

     }
     else
     {
      iPid.push_back(0);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcTrk->px());
      ptrk.setPy(mdcTrk->py());
      ptrk.setPz(mdcTrk->pz());
      double p3 = ptrk.vect().mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      pCh.push_back(ptrk);
//      cout<<"errrrrrrrrrrrrrrr"<<endl;
     }
    
  }
  m_npi = npi;
  m_nkaon = nkaon;
  m_nproton = nproton;
//  int npip = ipip.size();
//  int npim = ipim.size();
//  if(npip*npim != 1) return SUCCESS;
  m_istat_pmiss=0;
  m_istat_pbmiss=0;
  m_istat_pipmiss=0;
  m_istat_pimmiss=0;
  for(int i=0;i<4;i++)
  {
     m_ptrk_pmiss[i]=5;
     m_ptrk_pbmiss[i]=5;
     m_ptrk_pipmiss[i]=5;
     m_ptrk_pimmiss[i]=5;
  }
  for(int i=0; i<3; i++)
  {
   m_id_pmiss[i]=0;
   m_id_pbmiss[i]=0;
   m_id_pipmiss[i]=0;
   m_id_pimmiss[i]=0;
  }
////////////////////////////////////////////////////////////////////////
//classify
/////////////////////////////////////////////////////  
  HepLorentzVector ecms(0.03406837,0,0,3.0971873);
  HepLorentzVector pmiss;
  m_mpmiss=5.;
  m_mpbmiss=5.;
  m_mpipmiss=5.;
  m_mpimmiss=5.;
  m_ppmiss=5.;
  m_ppbmiss=5.;
  m_ppipmiss=5.;
  m_ppimmiss=5.;

  int istat_nhit;

  if((npip==1)&&(npim==1)&&(nprotonm==1))
  {
    pmiss.setPx(0);
    pmiss.setPy(0);
    pmiss.setPz(0);
    pmiss.setE(0);
    istat_nhit=0;

    int j=0;
    for(int i = 0; i < nGood; i++)
    {
    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
//    if(!(*itTrk)->isMdcTrackValid()) continue;
   RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
//    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
    double p;
    double eraw;
    if((*itTrk)->isEmcShowerValid()) 
    {
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    eraw = emcTrk->energy();
    }

//  pi+  pi-  anti-proton and miss proton     
     if((iPid[i]*iCh[i])== 2 ) 
     {
      m_index_pmiss[j]=i;
      pmiss=pmiss+pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit = 1;
      j++;
     }
     else if((iPid[i]*iCh[i])== -2 ) 
     {
      m_index_pmiss[j]=i;
      pmiss=pmiss+pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit = 1;

      j++;
     }
     else if((iPid[i]*iCh[i])== -4 ) 
     { 
      m_index_pmiss[j]=i;
      pmiss=pmiss+pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit = 1; 

      j++;
     }
     else
     {
        if(nGood==4)
        {
          m_index_pmiss[3]=i;
          RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
          p = mdcKalTrk->p();
          HepLorentzVector ptrk;
          ptrk.setPx(mdcKalTrk->px());
          ptrk.setPy(mdcKalTrk->py());
          ptrk.setPz(mdcKalTrk->pz());
          double p3 = ptrk.vect().mag();
          ptrk.setE(sqrt(p3*p3+mproton*mproton));
          m_ptrk_pmiss[0]=ptrk.px();
          m_ptrk_pmiss[1]=ptrk.py();
          m_ptrk_pmiss[2]=ptrk.pz();
          m_ptrk_pmiss[3]=ptrk.e();

        }
     }
    }// end loop of good charge particle 
  
    for(int i = 0; i < nGood; i++) {
      if((iPid[i]*iCh[i])== 2 ) continue;
      if((iPid[i]*iCh[i])== -2 ) continue;
      if((iPid[i]*iCh[i])== -4 ) continue;
      if(iCh[i]<0) continue;
//      if(nGood==6) protonpTrk = (*(evtRecTrkCol->begin()+iGood[i]))->mdcKalTrack();
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    for(int ii=0; ii<3; ii++)
    {
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    if(ii==0)
    {    pid->usePidSys(pid->useDedx() ); }// use dedx only
    else if(ii==1)
    {  pid->usePidSys( pid->useTof1() | pid->useTof2()); }// use tof only
    else if(ii==2)
    {  pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); }// use both dedx and tof

    pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if((pid->probProton() >= pid->probPion())&&(pid->probProton() >= pid->probKaon())) m_id_pmiss[ii]++;
    }
  }

    pmiss = ecms - pmiss;
    m_mpmiss= pmiss.m();
    m_ppmiss = pmiss.rho();
    
    if(fabs(m_mpmiss-mproton)<0.02&&istat_nhit==0)
    {
     m_istat_pmiss=1;
     (*(evtRecTrkCol->begin()+ iGood[m_index_pmiss[3]]))->setPartId(3);
      (*(evtRecTrkCol->begin()+ iGood[m_index_pmiss[3]]))->setQuality(0);

    }

  Ncut3++; 
  }//end miss proton
  if((npip==1)&&(npim==1)&&(nprotonp==1))
  {
    pmiss.setPx(0);
    pmiss.setPy(0);
    pmiss.setPz(0);
    pmiss.setE(0);
    istat_nhit=0;
   
    int j=0;
    for(int i = 0; i < nGood; i++)
    {
    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
//    if(!(*itTrk)->isMdcTrackValid()) continue;
   RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
//    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
    double p;
    double eraw;
    if((*itTrk)->isEmcShowerValid()) 
    {
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    eraw = emcTrk->energy();
    }
//    m_eop[i]=eraw/p;

//  pi+  pi-  proton and miss anti-proton     
     if((iPid[i]*iCh[i])== 2 ) 
     {
      m_index_pbmiss[j] = i;
      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else if((iPid[i]*iCh[i])== -2 ) 
     {
      m_index_pbmiss[j] = i;

      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else if((iPid[i]*iCh[i])== 4 ) 
     { 
      m_index_pbmiss[j] = i;
      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else
     {
        if(nGood==4)
        {
          m_index_pbmiss[3] = i;
          RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
          p = mdcKalTrk->p();
          HepLorentzVector ptrk;
          ptrk.setPx(mdcKalTrk->px());
          ptrk.setPy(mdcKalTrk->py());
          ptrk.setPz(mdcKalTrk->pz());
          double p3 = ptrk.vect().mag();
          ptrk.setE(sqrt(p3*p3+mproton*mproton));
          m_ptrk_pbmiss[0]=ptrk.px();
          m_ptrk_pbmiss[1]=ptrk.py();
          m_ptrk_pbmiss[2]=ptrk.pz();
          m_ptrk_pbmiss[3]=ptrk.e();

        }
     }
    }// end loop of good charge particle 
  
    for(int i = 0; i < nGood; i++) {
      if((iPid[i]*iCh[i])== 2 ) continue;
      if((iPid[i]*iCh[i])== -2 ) continue;
      if((iPid[i]*iCh[i])== 4 ) continue;
      if(iCh[i]>0) continue;
//      if(nGood==6) protonpTrk = (*(evtRecTrkCol->begin()+iGood[i]))->mdcKalTrack();
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    for(int ii=0; ii<3; ii++)
    {
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    if(ii==0)
    {    pid->usePidSys(pid->useDedx() ); }// use dedx only
    else if(ii==1)
    {  pid->usePidSys( pid->useTof1() | pid->useTof2()); }// use tof only
    else if(ii==2)
    {  pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); }// use both dedx and tof

    pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if((pid->probProton() >= pid->probPion())&&(pid->probProton() >= pid->probKaon())) m_id_pbmiss[ii]++;
    }
  }
    pmiss = ecms - pmiss;
    m_mpbmiss= pmiss.m();
    m_ppbmiss = pmiss.rho();
    if(fabs(m_mpbmiss-mproton)<0.02&&istat_nhit==0)
    {
      m_istat_pbmiss=1;
      (*(evtRecTrkCol->begin()+ iGood[m_index_pbmiss[3]]))->setPartId(3);
      (*(evtRecTrkCol->begin()+ iGood[m_index_pbmiss[3]]))->setQuality(0);
    }
  Ncut4++; 
  }//end miss anti-proton
  int initial_pip, initial_pim;
//***************************************
////   test the istat of jpsi mass 
//****************************************  
  
 if((npim==1)&&(nprotonp==1)&&(nprotonm==1))
  {
    pmiss.setPx(0);
    pmiss.setPy(0);
    pmiss.setPz(0);
    pmiss.setE(0);
    istat_nhit=0;
   
    int j=0;
    for(int i = 0; i < nGood; i++)
    {
    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
//    if(!(*itTrk)->isMdcTrackValid()) continue;
   RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
//    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
    double p;
    double eraw;
    if((*itTrk)->isEmcShowerValid()) 
    {
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    eraw = emcTrk->energy();
    }
//    m_eop[i]=eraw/p;

//  pi+  pi-  proton and miss anti-proton     
    if((iPid[i]*iCh[i])== 4 ) 
     {
      m_index_pipmiss[j] = i;
      pmiss = pmiss +pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit =1;

      j++;
     }
     else if((iPid[i]*iCh[i])== -4 ) 
     {
      m_index_pipmiss[j] = i;

      pmiss = pmiss +pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else if((iPid[i]*iCh[i])== -2 ) 
     { 
      m_index_pipmiss[j] = i;

      pmiss = pmiss +pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else
     {
        if(nGood==4)
        {
          m_index_pipmiss[3] = i;

          RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
          p = mdcKalTrk->p();
          HepLorentzVector ptrk;
          ptrk.setPx(mdcKalTrk->px());
          ptrk.setPy(mdcKalTrk->py());
          ptrk.setPz(mdcKalTrk->pz());
          double p3 = ptrk.vect().mag();
          ptrk.setE(sqrt(p3*p3+mpi*mpi));
          m_ptrk_pipmiss[0]=ptrk.px();
          m_ptrk_pipmiss[1]=ptrk.py();
          m_ptrk_pipmiss[2]=ptrk.pz();
          m_ptrk_pipmiss[3]=ptrk.e();

        }
     }
    }// end loop of good charge particle 
  
    for(int i = 0; i < nGood; i++) {
      if((iPid[i]*iCh[i])== 4 ) continue;
      if((iPid[i]*iCh[i])== -4 ) continue;
      if((iPid[i]*iCh[i])== -2 ) continue;
      if(iCh[i]<0) continue;
//      if(nGood==6) protonpTrk = (*(evtRecTrkCol->begin()+iGood[i]))->mdcKalTrack();
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    for(int ii=0; ii<3; ii++)
    {
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    if(ii==0)
    {    pid->usePidSys(pid->useDedx() ); }// use dedx only
    else if(ii==1)
    {  pid->usePidSys( pid->useTof1() | pid->useTof2()); }// use tof only
    else if(ii==2)
    {  pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); }// use both dedx and tof

    pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if((pid->probPion() >= pid->probProton())&&(pid->probPion() >= pid->probKaon())) m_id_pipmiss[ii]++;
    }
  }
    pmiss = ecms - pmiss;
    m_mpipmiss = pmiss.m();
    m_ppipmiss = pmiss.rho();
    if(fabs(m_mpipmiss-mpi)<0.05&&istat_nhit==0)
    {
      m_istat_pipmiss=1;
      (*(evtRecTrkCol->begin()+ iGood[m_index_pipmiss[3]]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ iGood[m_index_pipmiss[3]]))->setQuality(0);

    }
  Ncut5++; 
  }//end miss pip
 if((npip==1)&&(nprotonp==1)&&(nprotonm==1))
  {
    pmiss.setPx(0);
    pmiss.setPy(0);
    pmiss.setPz(0);
    pmiss.setE(0);
    istat_nhit=0;
 
    int j=0;
    for(int i = 0; i < nGood; i++)
    {
    EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + iGood[i];
//    if(!(*itTrk)->isMdcTrackValid()) continue;
   RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
//    RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
    double p;
    double eraw;
    if((*itTrk)->isEmcShowerValid()) 
    {
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    eraw = emcTrk->energy();
    }
//    m_eop[i]=eraw/p;

//  pi+  pi-  proton and miss anti-proton     
    
    
    if((iPid[i]*iCh[i])== 4 ) 
     {
      m_index_pimmiss[j]=i;
      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit=1;

      j++;
     }
     else if((iPid[i]*iCh[i])== -4 ) 
     {
       m_index_pimmiss[j]=i;

      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::proton);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit =1;

      j++;
     }
     else if((iPid[i]*iCh[i])== 2 ) 
     { 
      m_index_pimmiss[j]=i;
      pmiss = pmiss + pCh[i];
      RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
      p = mdcKalTrk->p();
      if(m_dedxngoodhit[i]<20) istat_nhit = 1;

      j++;
     }
     else
     {
        if(nGood==4)
        {
          m_index_pimmiss[3]=i;
          RecMdcKalTrack::setPidType  (RecMdcKalTrack::pion);
          p = mdcKalTrk->p();
          HepLorentzVector ptrk;
          ptrk.setPx(mdcKalTrk->px());
          ptrk.setPy(mdcKalTrk->py());
          ptrk.setPz(mdcKalTrk->pz());
          double p3 = ptrk.vect().mag();
          ptrk.setE(sqrt(p3*p3+mpi*mpi));
          m_ptrk_pimmiss[0]=ptrk.px();
          m_ptrk_pimmiss[1]=ptrk.py();
          m_ptrk_pimmiss[2]=ptrk.pz();
          m_ptrk_pimmiss[3]=ptrk.e();

        }
     }
    }// end loop of good charge particle 
  
    for(int i = 0; i < nGood; i++) {
      if((iPid[i]*iCh[i])== 4 ) continue;
      if((iPid[i]*iCh[i])== -4 ) continue;
      if((iPid[i]*iCh[i])== 2 ) continue;
      if(iCh[i]>0) continue;
//      if(nGood==6) protonpTrk = (*(evtRecTrkCol->begin()+iGood[i]))->mdcKalTrack();
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 
    for(int ii=0; ii<3; ii++)
    {
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    if(ii==0)
    {    pid->usePidSys(pid->useDedx() ); }// use dedx only
    else if(ii==1)
    {  pid->usePidSys( pid->useTof1() | pid->useTof2()); }// use tof only
    else if(ii==2)
    {  pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); }// use both dedx and tof

    pid->identify(pid->onlyPion() | pid->onlyKaon() | pid->onlyProton());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if((pid->probPion() >= pid->probProton())&&(pid->probPion() >= pid->probKaon())) m_id_pimmiss[ii]++;
    }
  }
    pmiss = ecms - pmiss;
    m_mpimmiss = pmiss.m();
    m_ppimmiss = pmiss.rho();
    if(fabs(m_mpimmiss-mpi)<0.05&&istat_nhit==0)
    {
      m_istat_pimmiss=1;
      (*(evtRecTrkCol->begin()+ iGood[m_index_pimmiss[3]]))->setPartId(2);
      (*(evtRecTrkCol->begin()+ iGood[m_index_pimmiss[3]]))->setQuality(0);

    }
  Ncut6++; 
  }//end miss pip


 
  if(m_istat_pmiss!=1 &&m_istat_pbmiss!=1&&m_istat_pipmiss!=1 && m_istat_pimmiss!=1)
  {return StatusCode::SUCCESS;}
  

//  Ncut3++;
  if(m_saventuple) m_tuple0->write();

   setFilterPassed(true);

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DQAPi2p2::finalize() {
  cout<<"total number:         "<<Ncut0<<endl;
  cout<<"nGood==2, nCharge==0: "<<Ncut1<<endl;
  cout<<"nGam>=2:              "<<Ncut2<<endl;
  cout<<"Pass Pid:             "<<Ncut3<<endl;
  cout<<"Pass 4C:              "<<Ncut4<<endl;
  cout<<"Pass 5C:              "<<Ncut5<<endl;
  cout<<"J/psi->pi+ pi- p andti-p:      "<<Ncut6<<endl;
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

