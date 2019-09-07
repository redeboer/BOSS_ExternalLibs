#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/ITHistSvc.h"
//#include "GaudiKernel/IHistogramSvc.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "EventModel/EventHeader.h"

#include "TMath.h"
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

#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/SecondVertexFit.h"

//
#include "DQAinclKs/inclks.h"

#include <vector>

const double mk0 = 0.497648;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//                          e         mu         pi      K         p

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            Ks              +         X                                  //
//            |-->pi+ pi-                                                  //
//                                                                         //
//                                                     xugf 2009.06        //
/////////////////////////////////////////////////////////////////////////////

inclks::inclks(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  m_tuple1 = 0;
  for(int i = 0; i < 10; i++) m_pass[i] = 0;

//Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=10.0);
  declareProperty("Vz0cut", m_vz0cut=50.0);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);

}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclks::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // "DQAHist" is fixed
 TH1F*  inclks_mass = new TH1F("InclKs_mass","INCLUSIVE_Ks_MASS",70,0.46,0.53); 
     inclks_mass->GetXaxis()->SetTitle("M_{#pi#pi} (GeV)");
     inclks_mass->GetYaxis()->SetTitle("Nentries/0.1MeV/c^{2}");
  if(m_thsvc->regHist("/DQAHist/InclKs/InclKs_mass", inclks_mass).isFailure()) {
      log << MSG::ERROR << "Couldn't register inclks_mass" << endreq;
  }

//*****************************************

    NTuplePtr nt1(ntupleSvc(), "DQAFILE/InclKs");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("DQAFILE/InclKs", CLID_ColumnWiseTuple, "ksklx Ntuple");
      if ( m_tuple1 )    {
	status = m_tuple1->addItem ("npip", m_npip);
	status = m_tuple1->addItem ("npim", m_npim);
	status = m_tuple1->addItem ("ctau", m_ctau);
	status = m_tuple1->addItem ("lxyz", m_lxyz);
	status = m_tuple1->addItem ("elxyz", m_elxyz);
	status = m_tuple1->addItem ("kschi", m_chis);
	status = m_tuple1->addItem ("mksraw", m_ksRawMass);
	status = m_tuple1->addItem ("pk0", m_pk0);

      }
      else    { 
	log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
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
StatusCode inclks::execute() {
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

  Vint iks, ipip, ipim, iGood;
  iGood.clear();
  iks.clear();
  ipip.clear();
  ipim.clear();

  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

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
   pid->identify(pid->onlyPion() | pid->onlyKaon());  // seperater Pion/Kaon
//   pid->identify(pid->onlyPion());
//    pid->identify(pid->onlyKaon());
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue ;
    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();
//    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

    if(pid->probPion() < 0.001 || (pid->probPion() < pid->probKaon())) continue;
//    if(pid->probPion() < 0.001) continue;
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+xmass[2]*xmass[2]));

    if(mdcKalTrk->charge() >0) {
      ipip.push_back(iGood[i]);
      ppip.push_back(ptrk);
    } else {
      ipim.push_back(iGood[i]);
      ppim.push_back(ptrk);
    }
  }

  m_pass[2] += 1;
      int npip = ipip.size();
      int npim = ipim.size();
       m_npip=npip;
       m_npim=npim;

  if(npip < 1 || npim <1) return sc;

  m_pass[3] += 1;

//
//****** Second Vertex Check************
//
  double chi, delm;
  double chisq=999.;
//  double delm=100.;

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

//  HepPoint3D bvx(0., 0., 0.);
//  HepSymMatrix bEvx(3, 0);
//  bEvx[0][0] = 0.038*0.038;
//  bEvx[1][1] = 0.00057*0.00057;
//  bEvx[2][2] = 1.5*1.5;
//  VertexParameter bs;
//  bs.setVx(bvx);
//  bs.setEvx(bEvx);

   int ip1=-1, ip2=-1;

  VertexFit *vtxfit0 = VertexFit::instance();
  SecondVertexFit *vtxfit = SecondVertexFit::instance();

  for(int i1 = 0; i1 < ipip.size(); i1++) {
    RecMdcKalTrack *pipTrk = (*(evtRecTrkCol->begin()+ipip[i1]))->mdcKalTrack();
    pipTrk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpiptrk(xmass[2], pipTrk->helix(), pipTrk->err());

    for(int i2 = 0; i2 < ipim.size(); i2++) {
      RecMdcKalTrack *pimTrk = (*(evtRecTrkCol->begin()+ipim[i2]))->mdcKalTrack();
      pimTrk->setPidType(RecMdcKalTrack::pion);
      WTrackParameter wpimtrk(xmass[2], pimTrk->helix(), pimTrk->err());

      vtxfit0->init();
      vtxfit0->AddTrack(0, wpiptrk);
      vtxfit0->AddTrack(1, wpimtrk);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) continue;
      vtxfit0->BuildVirtualParticle(0);

      vtxfit->init();
      vtxfit->AddTrack(0, vtxfit0->wVirtualTrack(0));
      vtxfit->setVpar(vtxfit0->vpar(0));
      if(!vtxfit->Fit()) continue;
      chi = vtxfit->chisq();

//      if(fabs((vtxfit->p4par().m())-mk0) > delm) continue;
      if(chi > chisq) continue;
      delm = fabs((vtxfit->p4par().m())-mk0);
      chisq = chi;
      ip1=ipip[i1];
      ip2=ipim[i2];
    }
  }
//
//   Kshort check
//
      if(ip1==-1 || ip2==-1) return sc;
  m_pass[4] += 1;

    RecMdcKalTrack *pi1Trk = (*(evtRecTrkCol->begin()+ip1))->mdcKalTrack();
    pi1Trk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpi1trk(xmass[2], pi1Trk->helix(), pi1Trk->err());

    RecMdcKalTrack *pi2Trk = (*(evtRecTrkCol->begin()+ip2))->mdcKalTrack();
    pi2Trk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpi2trk(xmass[2], pi2Trk->helix(), pi2Trk->err());
      vtxfit0->init();
      vtxfit0->AddTrack(0, wpi1trk);
      vtxfit0->AddTrack(1, wpi2trk);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) return sc;
      vtxfit0->BuildVirtualParticle(0);

      vtxfit->init();
      vtxfit->AddTrack(0, vtxfit0->wVirtualTrack(0));
      vtxfit->setVpar(vtxfit0->vpar(0));
      if(!vtxfit->Fit()) return sc;

      m_ksRawMass = vtxfit->p4par().m();
      m_ctau = vtxfit->ctau();
      m_lxyz = vtxfit->decayLength();
      m_elxyz = vtxfit->decayLengthError();
      m_chis = vtxfit->chisq();
      m_pk0 = vtxfit->p4par().rho();

    // DQA
      if(m_lxyz>0.4 && m_chis<20.){

    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/InclKs/InclKs_mass", h).isSuccess()) {
        h->Fill(m_ksRawMass);
    } else {
        log << MSG::ERROR << "Couldn't retrieve inclks_mass" << endreq;
    }
       }

     m_tuple1->write();
////////////////////////////////////
//DQA
// set tag and quality
  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
//  (*(evtRecTrkCol->begin()+ip1))->setPartId(3);
//  (*(evtRecTrkCol->begin()+ip2))->setPartId(3);
  (*(evtRecTrkCol->begin()+ip1))->tagPion();
  (*(evtRecTrkCol->begin()+ip2))->tagPion();
   // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*(evtRecTrkCol->begin()+ip1))->setQuality(2);
  (*(evtRecTrkCol->begin()+ip2))->setQuality(2);
//--------------------------------------------------
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode inclks::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << "Total Entries : " << m_pass[0] << endreq;
  log << MSG::INFO << "Qtot Cut : " << m_pass[1] << endreq;
  log << MSG::INFO << "PID : " << m_pass[2] << endreq;
  log << MSG::INFO << "NPI : " << m_pass[3] << endreq;
  log << MSG::INFO << "Second Vertex Cut : " << m_pass[4] << endreq;
  return StatusCode::SUCCESS;
}

