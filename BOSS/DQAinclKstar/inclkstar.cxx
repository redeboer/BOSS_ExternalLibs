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
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Bootstrap.h"

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

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

#include "DQAinclKstar/inclkstar.h"

#include <vector>

const double ecms = 3.097;
const double mpi = 0.13957;
const double mk = 0.493677;
const double mkstar0 = 0.896;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//                          e         mu         pi      K         p
const double velc = 299.792458;   // tof path unit in mm

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            Kstar              +         X                               //
//              |-->K + pi                                                 //
//                                                                         //
//                                                     xugf 2009.06        //
/////////////////////////////////////////////////////////////////////////////

inclkstar::inclkstar(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  m_tuple2 = 0;
  for(int i = 0; i < 10; i++) m_pass[i] = 0;

//Declare the properties  
  //Declare the properties
  declareProperty("Vr0cut", m_vr0cut=5.0);
  declareProperty("Vz0cut", m_vz0cut=10.0);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclkstar::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // "DQAHist" is fixed
     TH1F* inclkstar_mass = new TH1F("InclKstar_mass","INCLUSIVE_Kstar_MASS",65,0.65,1.3);
     inclkstar_mass->GetXaxis()->SetTitle("M_{K#pi} (GeV)");
     inclkstar_mass->GetYaxis()->SetTitle("Nentries/10MeV/c^{2}");

  if(m_thsvc->regHist("/DQAHist/InclKstar/InclKstar_mass", inclkstar_mass).isFailure()) {
      log << MSG::ERROR << "Couldn't register InclKstar_mass" << endreq;
  }

//*****************************************
    NTuplePtr nt2(ntupleSvc(), "DQAFILE/InclKstar");
    if ( nt2 ) m_tuple2 = nt2;
    else {
   m_tuple2 = ntupleSvc()->book ("DQAFILE/InclKstar", CLID_ColumnWiseTuple, "inclkstar Ntuple");
      if ( m_tuple2 )    {
	status = m_tuple2->addItem ("nkaonm",   m_nkm);
	status = m_tuple2->addItem ("nkaonp",   m_nkp);
	status = m_tuple2->addItem ("npionp",   m_npip);
	status = m_tuple2->addItem ("npionm",   m_npim);
	status = m_tuple2->addItem ("ncharge",  m_ncharge);
	status = m_tuple2->addItem ("difchikp", m_difchikp);
	status = m_tuple2->addItem ("difchikm", m_difchikm);
	status = m_tuple2->addItem ("mkstarkp", m_kstarkp);
	status = m_tuple2->addItem ("mkstarkm", m_kstarkm);
	status = m_tuple2->addItem ("mkstar",   m_mkstar);
	status = m_tuple2->addItem ("pkstar",   m_pkstar);
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
StatusCode inclkstar::execute() {
  StatusCode sc = StatusCode::SUCCESS;  

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // DQA
  // Add the line below at the beginning of execute()
  //
  setFilterPassed(false);

  m_pass[0] += 1;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  Vint iGood, ikm, ikp, ipip, ipim, iGam;
  iGood.clear();
  ikm.clear();
  ikp.clear();
  ipip.clear();
  ipim.clear();
  iGam.clear();

  Vp4 ppip, ppim, pkm, pkp;
  ppip.clear();
  ppim.clear();
  pkm.clear();
  pkp.clear();

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

  m_pass[1] += 1;

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
//   pid->identify(pid->onlyPion());
//    pid->identify(pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
//    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    if(!(*itTrk)->isMdcKalTrackValid()) continue ;
    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
    if(pid->probKaon() > 0.001 && (pid->probKaon() > pid->probPion())){
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

    if(pid->probPion() > 0.001 && (pid->probPion() > pid->probKaon())){
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
        HepLorentzVector ptrk;
        ptrk.setPx(mdcKalTrk->px());
        ptrk.setPy(mdcKalTrk->py());
        ptrk.setPz(mdcKalTrk->pz());
        double p3 = ptrk.mag();
        ptrk.setE(sqrt(p3*p3+mpi*mpi));
      if(mdcKalTrk->charge() >0) {
        ipip.push_back(iGood[i]);
        ppip.push_back(ptrk);
      } else {
        ipim.push_back(iGood[i]);
        ppim.push_back(ptrk);
      }
    }
  }
      int npip = ipip.size();
      int npim = ipim.size();
      int nkm = ikm.size();
      int nkp = ikp.size();

       m_nkm=nkm;
       m_nkp=nkp;
       m_npip=npip;
       m_npim=npim;
       m_ncharge=nGood;
  m_pass[3] += 1;

  if(npip < 1 && npim < 1) return sc;
  if(nkp < 1 && nkm < 1) return sc;

  m_pass[4] += 1;

//
//**************** Kstar Finding ************
//
//
  HepLorentzVector pkstar, pkstar1, ppi1, ppi2, pTot;
  Vint ikstar;
  ikstar.clear();

  double difchi0=99999.0;
     int ixpim = -1;
     int ixkp = -1;

  for (int i = 0; i < npim; i++) {
  for (int j = 0; j < nkp; j++) {
         pkstar  = ppim[i] + pkp[j];
          double difchi =fabs(pkstar.m()-mkstar0);
          if(difchi < difchi0){
           difchi0 = difchi;
            ixpim = i;
            ixkp = j;
         }  //good kstar
  }          //K+
  }          //pi-

         m_difchikp = difchi0;

     if(ixpim != -1) m_kstarkp=(ppim[ixpim] + pkp[ixkp]).m();

  double difchi1=99999.0;
     int ixpip = -1;
     int ixkm = -1;

  for (int ii = 0; ii < npip; ii++) {
  for (int jj = 0; jj < nkm; jj++) {
         pkstar1  = ppip[ii] + pkm[jj];
          double difchi2 =fabs(pkstar1.m()-mkstar0);
          if(difchi2 < difchi1){
           difchi1 = difchi2;
            ixpip = ii;
            ixkm = jj;
         }  //good kstar
  }          //K-
  }          //pi+

         m_difchikm = difchi1;

     if(ixpip != -1) m_kstarkm=(ppip[ixpip] + pkm[ixkm]).m();

//       cout << "m_kstarkm==========" << m_kstarkm <<endl; 
     if(ixpip == -1 && ixpim == -1) return sc;

     if(difchi0 < difchi1){
        pTot = ppim[ixpim] + pkp[ixkp];

     } else{ 
        pTot = ppip[ixpip] + pkm[ixkm];

    }
          m_mkstar = pTot.m();
          m_pkstar = pTot.rho();

    // DQA
    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/InclKstar/InclKstar_mass", h).isSuccess()) {
        h->Fill(m_mkstar);
    } else {
        log << MSG::ERROR << "Couldn't retrieve InclKstar_mass" << endreq;
    }

     m_tuple2->write();
////////////////////////////////////
//DQA

     if(ixpim != -1){
//  (*(evtRecTrkCol->begin()+ipim[ixpim]))->setPartId(3);
//  (*(evtRecTrkCol->begin()+ikp[ixkp]))->setPartId(4);
  (*(evtRecTrkCol->begin()+ipim[ixpim]))->tagPion();
  (*(evtRecTrkCol->begin()+ikp[ixkp]))->tagKaon();
   (*(evtRecTrkCol->begin()+ipim[ixpim]))->setQuality(3);
  (*(evtRecTrkCol->begin()+ikp[ixkp]))->setQuality(3);
    } else {
//  (*(evtRecTrkCol->begin()+ipip[ixpip]))->setPartId(3);
//  (*(evtRecTrkCol->begin()+ikm[ixkm]))->setPartId(4);
  (*(evtRecTrkCol->begin()+ipip[ixpip]))->tagPion();
  (*(evtRecTrkCol->begin()+ikm[ixkm]))->tagKaon();
   (*(evtRecTrkCol->begin()+ipip[ixpip]))->setQuality(3);
  (*(evtRecTrkCol->begin()+ikm[ixkm]))->setQuality(3);
   }
//--------------------------------------------------
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);


  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclkstar::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << "Total Entries : " << m_pass[0] << endreq;
  log << MSG::INFO << "TOF,dEdx: " << m_pass[1] << endreq;
  log << MSG::INFO << "Ncharge Cut : " << m_pass[2] << endreq;
  log << MSG::INFO << "PID : " << m_pass[3] << endreq;
  log << MSG::INFO << "Npi and Nk Cut : " << m_pass[4] << endreq;
  return StatusCode::SUCCESS;
}

