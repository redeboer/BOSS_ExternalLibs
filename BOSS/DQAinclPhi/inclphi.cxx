#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

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
//#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/ITHistSvc.h"

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

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

#include "DQAinclPhi/inclphi.h"

#include <vector>

const double ecms = 3.097;
const double mk = 0.493677;
const double mphi = 1.01946;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
//                          e         mu         pi      K         p

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            phi              +         X                                 //
//             |-->K+ K-                                                   //
//                                                                         //
//                                                     xugf 2009.06        //
/////////////////////////////////////////////////////////////////////////////
inclphi::inclphi(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  m_tuple1 = 0;
  m_tuple2 = 0;
  for(int i = 0; i < 10; i++) m_pass[i] = 0;

//Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=5.0);
  declareProperty("Vz0cut", m_vz0cut=10.0);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclphi::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // "DQAHist" is fixed
    TH1F*  inclphi_mass = new TH1F("InclPhi_mass","INCLUSIVE_PHI_MASS",80,1.01,1.05);
     inclphi_mass->GetXaxis()->SetTitle("M_{KK} (GeV)");
     inclphi_mass->GetYaxis()->SetTitle("Nentries/0.5MeV/c^{2}");
  if(m_thsvc->regHist("/DQAHist/InclPhi/InclPhi_mass", inclphi_mass).isFailure()) {
      log << MSG::ERROR << "Couldn't register inclphi_mass" << endreq;
  }

//*****************************************

    NTuplePtr nt1(ntupleSvc(), "DQAFILE/InclPhi");
    if ( nt1 ) m_tuple1 = nt1;
    else {
    m_tuple1 = ntupleSvc()->book ("DQAFILE/InclPhi1", CLID_ColumnWiseTuple, "inclphi Ntuple");
      if ( m_tuple1 )    {
	status = m_tuple1->addItem ("mphiall",      m_mphiall);
	status = m_tuple1->addItem ("mpphiall",     m_pphiall);
      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
	return StatusCode::FAILURE;
      }
    }
    NTuplePtr nt2(ntupleSvc(), "DQAFILE/InclPhi2");
    if ( nt2 ) m_tuple2 = nt2;
    else {
    m_tuple2 = ntupleSvc()->book ("DQAFILE/InclPhi2", CLID_ColumnWiseTuple, "inclphi Ntuple");
      if ( m_tuple2 )    {
	status = m_tuple2->addItem ("nkp",      m_nkp);
	status = m_tuple2->addItem ("nkm",      m_nkm);
	status = m_tuple2->addItem ("ncharge",  m_ncharge);
	status = m_tuple2->addItem ("difchi0",  m_difchi);
	status = m_tuple2->addItem ("mmphi",    m_mphi);
	status = m_tuple2->addItem ("pphi",     m_pphi);
	status = m_tuple2->addItem ("pk1",      m_pk1);
      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
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
StatusCode inclphi::execute() {
  StatusCode sc = StatusCode::SUCCESS;  

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // DQA
  // Add the line below at the beginning of execute()
  //
  setFilterPassed(false);

  m_pass[0] += 1;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  m_pass[1] += 1;

  Vint ikp, ikm, iGood;
  iGood.clear();
  ikp.clear();
  ikm.clear();

  Vp4 pkp, pkm;
  pkp.clear();
  pkm.clear();

  int TotCharge = 0;
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
    if(fabs(mdcTrk->z()) >= m_vz0cut) continue;
    if(mdcTrk->r() >= m_vr0cut) continue;
    iGood.push_back(i);
    TotCharge += mdcTrk->charge();
  }
  //
  // Finish Good Charged Track Selection
  //
    int nGood = iGood.size();

  //
  // Charge track number cut
  //

  if((nGood < 2) || (TotCharge!=0)) return sc;

  m_pass[2] += 1;

  //
  // Assign 4-momentum to each charged track
  //
  ParticleID *pid = ParticleID::instance();
  for(int i = 0; i < nGood; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
    //    if(pid) delete pid;
    pid->init();
    pid->setMethod(pid->methodProbability());
    pid->setChiMinCut(4);
    pid->setRecTrack(*itTrk);
    pid->usePidSys(pid->useDedx() | pid->useTof1() | pid->useTof2()); // use PID sub-system
   pid->identify(pid->onlyPion() | pid->onlyKaon());    // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue ;
    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
//    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

    if(pid->probKaon() < 0.001 || (pid->probKaon() < pid->probPion())) continue;
//    if(pid->probPion() < 0.001) continue;
      mdcKalTrk->setPidType(RecMdcKalTrack::kaon);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mk*mk));
    if(mdcKalTrk->charge() >0) {
      ikp.push_back(iGood[i]);
      pkp.push_back(ptrk);
    } else {
      ikm.push_back(iGood[i]);
      pkm.push_back(ptrk);
    }
  }

  m_pass[4] += 1;
      int nkp = ikp.size();
      int nkm = ikm.size();

       m_nkp=nkp;
       m_nkm=nkm;
       m_ncharge=nGood;


  if(nkp < 1 || nkm <1) return sc;

  m_pass[5] += 1;

//
//**************** Phi Finding ************
//
//

  HepLorentzVector pphi, pTot;
  Vint iphi;
  iphi.clear();

  double difchi0=99999.0;
     int ixk1 = -1;
     int ixk2 = -1;

  for (int i = 0; i < nkm; i++) {
  for (int j = 0; j < nkp; j++) {

         pphi  = pkm[i] + pkp[j];
         m_mphiall = pphi.m();
         m_pphiall = pphi.rho();
        m_tuple1->write(); 

          double difchi =fabs(pphi.m()-mphi);
          if(difchi < difchi0){
           difchi0 = difchi;
            ixk1 = i;
            ixk2 = j;
         }  //good phi
  }          //K+
  }          //K-

         m_difchi = difchi0;

     if (ixk1 == -1) return sc;

  m_pass[6] += 1;

     pTot = pkm[ixk1] + pkp[ixk2];

       m_pk1 =  pkm[ixk1].m();
       m_mphi = pTot.m();
       m_pphi = pTot.rho();

    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/InclPhi/InclPhi_mass", h).isSuccess()) {
        h->Fill(pTot.m());
    } else {
        log << MSG::ERROR << "Couldn't retrieve inclphi_mass" << endreq;
    }
        m_tuple2->write(); 
////////////////////////////////////
//DQA
// set tag and quality
  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
//  (*(evtRecTrkCol->begin()+ikm[ixk1]))->setPartId(4);
//  (*(evtRecTrkCol->begin()+ikp[ixk2]))->setPartId(4);
  (*(evtRecTrkCol->begin()+ikm[ixk1]))->tagKaon();
  (*(evtRecTrkCol->begin()+ikp[ixk2]))->tagKaon();
   // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*(evtRecTrkCol->begin()+ikm[ixk1]))->setQuality(3);
  (*(evtRecTrkCol->begin()+ikp[ixk2]))->setQuality(3);
//--------------------------------------------------
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclphi::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << "Total Entries : " << m_pass[0] << endreq;
  log << MSG::INFO << "dstCol, trkCol: " << m_pass[1] << endreq;
  log << MSG::INFO << "Ncharge Cut : " << m_pass[2] << endreq;
  log << MSG::INFO << "TOF dEdx : " << m_pass[3] << endreq;
  log << MSG::INFO << "PID : " << m_pass[4] << endreq;
  log << MSG::INFO << "NK Cut : " << m_pass[5] << endreq;
  log << MSG::INFO << "Nphi select : " << m_pass[6] << endreq;
  return StatusCode::SUCCESS;
}

