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
#include "VertexFit/SecondVertexFit.h"
#include "ParticleID/ParticleID.h"

#include "DQAinclLamda/incllambda.h"

#include <vector>

const double ecms = 3.097;
const double mpi = 0.13957;
const double mp = 0.93827203;
const double mlam = 1.115683;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
//                          e         mu         pi      K         p

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            Lambda             +         X                               //
//              |-->p+- pi-+                                               //
//                                                                         //
//                                                     xugf 2009.06        //
/////////////////////////////////////////////////////////////////////////////

incllambda::incllambda(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  
  m_tuple1 = 0;
  for(int i = 0; i < 10; i++) m_pass[i] = 0;

//Declare the properties  
  declareProperty("Vr0cut", m_vr0cut=20.0);
  declareProperty("Vz0cut", m_vz0cut=50.0);
  declareProperty("CheckDedx", m_checkDedx = 1);
  declareProperty("CheckTof",  m_checkTof = 1);

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode incllambda::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  
  StatusCode status;

 if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }
  // "DQAHist" is fixed
  TH1F* incllambda_mass = new TH1F("InclLam_mass","INCLUSIVE_LAMBDA_MASS",150,1.11,1.125);
   incllambda_mass->GetXaxis()->SetTitle("M_{p#pi} (GeV)");
   incllambda_mass->GetYaxis()->SetTitle("Nentries/0.1MeV/c^{2}");
  if(m_thsvc->regHist("/DQAHist/InclLam/InclLam_mass", incllambda_mass).isFailure()) {
      log << MSG::ERROR << "Couldn't register incllambda_mass" << endreq;
  }

//*****************************************

    NTuplePtr nt1(ntupleSvc(), "DQAFILE/InclLam");
    if ( nt1 ) m_tuple1 = nt1;
    else {
     m_tuple1 = ntupleSvc()->book ("DQAFILE/InclLam", CLID_ColumnWiseTuple, "incllam Ntuple");
      if ( m_tuple1 )    {
	status = m_tuple1->addItem ("npi", m_npi);
	status = m_tuple1->addItem ("np", m_np);
	status = m_tuple1->addItem ("lxyz", m_lxyz);
	status = m_tuple1->addItem ("ctau", m_ctau);
	status = m_tuple1->addItem ("elxyz", m_elxyz);
	status = m_tuple1->addItem ("lamchi", m_chis);
	status = m_tuple1->addItem ("mlamraw", m_lamRawMass);
	status = m_tuple1->addItem ("plam", m_plam);
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
StatusCode incllambda::execute() {
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

  Vint ilam, ip, ipi, iGood;
  iGood.clear();
  ilam.clear();
  ip.clear();
  ipi.clear();

  Vp4 ppi, pp;
  ppi.clear();
  pp.clear();

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
    pid->identify(pid->onlyPion() | pid->onlyProton());  // seperater Pion/Kaon
    pid->calculate();
    if(!(pid->IsPidInfoValid())) continue;
//    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    if(!(*itTrk)->isMdcKalTrackValid()) continue ;
    RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();

     if(mdcKalTrk->charge() == 0) continue; 

    if(pid->probPion() > 0.001 && (pid->probPion() > pid->probProton())){
      mdcKalTrk->setPidType(RecMdcKalTrack::pion);
      ipi.push_back(iGood[i]);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      ppi.push_back(ptrk);
    }
    if(pid->probProton() > 0.001 && (pid->probProton() > pid->probPion())){
      mdcKalTrk->setPidType(RecMdcKalTrack::proton);
      ip.push_back(iGood[i]);
      HepLorentzVector ptrk;
      ptrk.setPx(mdcKalTrk->px());
      ptrk.setPy(mdcKalTrk->py());
      ptrk.setPz(mdcKalTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mp*mp));
      pp.push_back(ptrk);
    }
  }
  
  m_pass[2] += 1;

      int npi = ipi.size();
      int np = ip.size();
       m_npi = npi;
       m_np = np;
  if(npi < 1 || np <1) return sc;

  m_pass[3] += 1;
//
//****** Second Vertex Check************
//
  double chi=999.;
  double delm;

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

   int ipion=-1, iproton=-1;

  VertexFit *vtxfit0 = VertexFit::instance();
  SecondVertexFit *vtxfit = SecondVertexFit::instance();

  for(unsigned int i1 = 0; i1 < ipi.size(); i1++) {
    RecMdcKalTrack *piTrk = (*(evtRecTrkCol->begin()+ipi[i1]))->mdcKalTrack();
     piTrk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpitrk(mpi, piTrk->helix(), piTrk->err());

    for(unsigned int i2 = 0; i2 < ip.size(); i2++) {
      RecMdcKalTrack *protonTrk = (*(evtRecTrkCol->begin()+ip[i2]))->mdcKalTrack();
       protonTrk->setPidType(RecMdcKalTrack::proton);
      WTrackParameter wptrk(mp, protonTrk->helix(), protonTrk->err());

       int NCharge = 0;
       NCharge =piTrk->charge() + protonTrk->charge();

//      cout << "TotNcharge = " << NCharge << endl;

       if(NCharge != 0) continue;

      vtxfit0->init();
      vtxfit0->AddTrack(0, wpitrk);
      vtxfit0->AddTrack(1, wptrk);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) continue;
      vtxfit0->BuildVirtualParticle(0);

      vtxfit->init();
//      vtxfit->setPrimaryVertex(bs);
      vtxfit->AddTrack(0, vtxfit0->wVirtualTrack(0));
      vtxfit->setVpar(vtxfit0->vpar(0));
      if(!vtxfit->Fit()) continue;

      //double mass = vtxfit->p4par().m();
      //if(mass < m_massRangeLower) continue;
      //if(mass > m_massRangeHigher) continue;
      if(vtxfit->chisq() > chi) continue;
//       if(fabs((vtxfit->p4par()).m()-mlam) > delm) continue;
      delm = fabs((vtxfit->p4par()).m()-mlam);
      chi = vtxfit->chisq();
      ipion=ipi[i1];
      iproton=ip[i2];
    }
  }
//
//   Lammda check
//
      if(ipion==-1 || iproton==-1) return sc;
  m_pass[4] += 1;

    RecMdcKalTrack *pionTrk = (*(evtRecTrkCol->begin()+ipion))->mdcKalTrack();
     pionTrk->setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpiontrk(mpi, pionTrk->helix(), pionTrk->err());

    RecMdcKalTrack *protonTrk = (*(evtRecTrkCol->begin()+iproton))->mdcKalTrack();
     protonTrk->setPidType(RecMdcKalTrack::proton);
    WTrackParameter wprotontrk(mp, protonTrk->helix(), protonTrk->err());
      vtxfit0->init();
      vtxfit0->AddTrack(0, wpiontrk);
      vtxfit0->AddTrack(1, wprotontrk);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) return sc;
      vtxfit0->BuildVirtualParticle(0);

      vtxfit->init();
//      vtxfit->setPrimaryVertex(bs);
      vtxfit->AddTrack(0, vtxfit0->wVirtualTrack(0));
      vtxfit->setVpar(vtxfit0->vpar(0));
      if(!vtxfit->Fit()) return sc;

      m_lamRawMass = vtxfit->p4par().m();
      m_ctau = vtxfit->ctau();
      m_lxyz = vtxfit->decayLength();
      m_elxyz = vtxfit->decayLengthError();
      m_chis = vtxfit->chisq();
      m_plam = vtxfit->p4par().rho();

      if(m_lxyz>3.5){

    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/InclLam/InclLam_mass", h).isSuccess()) {
        h->Fill(m_lamRawMass);
    } else {
        log << MSG::ERROR << "Couldn't retrieve incllam_mass" << endreq;
    }

        }
    m_tuple1->write();
////////////////////////////////////
//DQA
// set tag and quality
  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
//  (*(evtRecTrkCol->begin()+ipion))->setPartId(3);
//  (*(evtRecTrkCol->begin()+iproton))->setPartId(5);
  (*(evtRecTrkCol->begin()+ipion))->tagPion();
  (*(evtRecTrkCol->begin()+iproton))->tagProton();
   // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*(evtRecTrkCol->begin()+ipion))->setQuality(1);
  (*(evtRecTrkCol->begin()+iproton))->setQuality(1);

//--------------------------------------------------
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode incllambda::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  log << MSG::INFO << "Total Entries : " << m_pass[0] << endreq;
  log << MSG::INFO << "Qtot Cut : " << m_pass[1] << endreq;
  log << MSG::INFO << "Nks : " << m_pass[2] << endreq;
  log << MSG::INFO << "Good Ks cut : " << m_pass[3] << endreq;
  return StatusCode::SUCCESS;
}

