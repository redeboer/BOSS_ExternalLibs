#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

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

#include "TH1F.h"
// #include "TMath.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"

using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"

#include "VertexFit/IVertexDbSvc.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "ParticleID/ParticleID.h"
#include "TH1F.h"
//
#include "JsiLL/JsiLL.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;

const double mpi     = 0.13957;
const double mk      = 0.493677;
const double mproton = 0.938272;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////

JsiLL::JsiLL(const std::string& name, ISvcLocator* pSvcLocator) :
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
StatusCode JsiLL::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the control sample name, the first letter is in upper format. eg. "Rhopi"
  NTuplePtr nt(ntupleSvc(), "DQAFILE/JsiLL");
  if ( nt ) m_tuple = nt;
   else {
      m_tuple = ntupleSvc()->book("DQAFILE/JsiLL", CLID_ColumnWiseTuple, "JsiLL ntuple");
      if( m_tuple ) {
          status = m_tuple->addItem("runNo", m_runNo);
          status = m_tuple->addItem("event", m_event);
          status = m_tuple->addItem("chisq", m_chisq);
          status = m_tuple->addItem("mLambda", m_mLambda);
          status = m_tuple->addItem("mLambdabar", m_mLambdabar);
          status = m_tuple->addItem("pLambda", m_pLambda);
          status = m_tuple->addItem("pLambdabar", m_pLambdabar);

      } else {
          log << MSG::ERROR << "Can not book N-tuple:" << long(m_tuple) << endreq;
      }
  }

  if(service("THistSvc", m_thsvc).isFailure()) {
    log << MSG::ERROR << "Couldn't get THistSvc" << endreq;
    return StatusCode::FAILURE;
  }

  // "DQAHist" is fixed
  // "Rhopi" is control sample name, just as ntuple case.
  TH1F* hrxy = new TH1F("Rxy", "Rxy distribution", 110, -1., 10.);
  if(m_thsvc->regHist("/DQAHist/JsiLL/hrxy", hrxy).isFailure()) {
      log << MSG::ERROR << "Couldn't register Rxy" << endreq;
  }
  TH1F* hz = new TH1F("z", "z distribution", 200, -100., 100.);
  if(m_thsvc->regHist("/DQAHist/JsiLL/hz", hz).isFailure()) {
      log << MSG::ERROR << "Couldn't register z" << endreq;
  }
  
  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode JsiLL::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // DQA
  // Add the line below at the beginning of execute()
  //
  setFilterPassed(false);

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  m_runNo=eventHeader->runNumber();
  m_event=eventHeader->eventNumber();
  log << MSG::DEBUG <<"run, evtnum = "
      << m_runNo << " , "
      << m_event <<endreq;


  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
      << evtRecEvent->totalCharged() << " , "
      << evtRecEvent->totalNeutral() << " , "
      << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);

  if(evtRecEvent->totalNeutral()>100) {
      return StatusCode::SUCCESS;
  }

  Vint iGood, iplus, iminus;
  iGood.clear();
  iplus.clear();
  iminus.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  Hep3Vector xorigin(0,0,0);
 
  IVertexDbSvc*  vtxsvc;
  Gaudi::svcLocator()->service("VertexDbSvc", vtxsvc);
  if(vtxsvc->isVertexValid()){ 
  double* dbv = vtxsvc->PrimaryVertex();
  double*  vv = vtxsvc->SigmaPrimaryVertex();
    xorigin.setX(dbv[0]);
    xorigin.setY(dbv[1]);
    xorigin.setZ(dbv[2]);
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

    if(fabs(z0) >= m_vz0cut) continue;
    if(Rxy >= m_vr0cut) continue;
//    if(fabs(m_Vct)>=m_cthcut) continue;

    // DQA
    TH1 *h(0);
    if (m_thsvc->getHist("/DQAHist/JsiLL/hrxy", h).isSuccess()) {
        h->Fill(Rxy);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hrxy" << endreq;
    }
    if (m_thsvc->getHist("/DQAHist/JsiLL/hz", h).isSuccess()) {
        h->Fill(z0);
    } else {
        log << MSG::ERROR << "Couldn't retrieve hz" << endreq;
    }

//     iGood.push_back((*itTrk)->trackId());
    iGood.push_back(i);
    nCharge += mdcTrk->charge();
    if (mdcTrk->charge() > 0) {
        iplus.push_back(i);
    } else {
        iminus.push_back(i);
    }
  }
  
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
  if((nGood != 4)||(nCharge!=0)){
      return StatusCode::SUCCESS;
  }


//  for(int i = 0; i < nGood; i++) {
//    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i];
//    if(!(*itTrk)->isMdcKalTrackValid())
//      return StatusCode::SUCCESS;
//    }
  
    int pidp0 = 5, pidp1 = 3, pidm0 = 5, pidm1 = 3;  //PID
  
    RecMdcKalTrack *itTrkp   = (*(evtRecTrkCol->begin() + iplus[0]))->mdcKalTrack(); 
    RecMdcKalTrack *itTrkpip = (*(evtRecTrkCol->begin() + iplus[1]))->mdcKalTrack();
    RecMdcKalTrack *itTrkpb  = (*(evtRecTrkCol->begin() + iminus[0]))->mdcKalTrack();
    RecMdcKalTrack *itTrkpim = (*(evtRecTrkCol->begin() + iminus[1]))->mdcKalTrack();

// p() is not filled during reconstruction
    double tmppp, tmpppb, tmpppip, tmpppim;
    tmppp = sqrt(itTrkp->px()*itTrkp->px() + itTrkp->py()*itTrkp->py() + itTrkp->pz()*itTrkp->pz());
    tmpppb = sqrt(itTrkpb->px()*itTrkpb->px() + itTrkpb->py()*itTrkpb->py() + itTrkpb->pz()*itTrkpb->pz());
    tmpppip = sqrt(itTrkpip->px()*itTrkpip->px() + itTrkpip->py()*itTrkpip->py() + itTrkpip->pz()*itTrkpip->pz());
    tmpppim = sqrt(itTrkpim->px()*itTrkpim->px() + itTrkpim->py()*itTrkpim->py() + itTrkpim->pz()*itTrkpim->pz());


    if ( tmppp < tmpppip ) {
        itTrkp   = (*(evtRecTrkCol->begin() + iplus[1]))->mdcKalTrack();
        itTrkpip = (*(evtRecTrkCol->begin() + iplus[0]))->mdcKalTrack(); 
	pidp0 = 3;
    	pidp1 = 5;
        double tmp;
	tmp = tmppp;
	tmppp = tmpppip;
	tmpppip = tmp;
    }     
    if ( tmpppb < tmpppim ) {
        itTrkpb  = (*(evtRecTrkCol->begin() + iminus[1]))->mdcKalTrack();
        itTrkpim = (*(evtRecTrkCol->begin() + iminus[0]))->mdcKalTrack(); 
	pidm0 = 3;
	pidm1 = 5;
	double tmp;
	tmp = tmpppb;
	tmpppb = tmpppim;
	tmpppim = tmp;
    }     

    if ( tmppp < 0.7 || tmppp  >1.1) return StatusCode::SUCCESS; 
    if ( tmpppb  < 0.7 || tmpppb >1.1) return StatusCode::SUCCESS; 
    if ( tmpppip > 0.35) return StatusCode::SUCCESS; 
    if ( tmpppim > 0.35) return StatusCode::SUCCESS; 
    


    itTrkp->setPidType(RecMdcKalTrack::proton);  
    itTrkpb->setPidType(RecMdcKalTrack::proton);  
    itTrkpip->setPidType(RecMdcKalTrack::pion);  
    itTrkpim->setPidType(RecMdcKalTrack::pion);  


   m_chisq = 9999.0;
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

  VertexFit* vtxfita0 = VertexFit::instance();
  SecondVertexFit *vtxfita = SecondVertexFit::instance();
  VertexFit* vtxfitb0 = VertexFit::instance();
  SecondVertexFit *vtxfitb = SecondVertexFit::instance();
  VertexFit* vtxfit = VertexFit::instance();
  
    WTrackParameter wpip = WTrackParameter(mpi, itTrkpip->getZHelix(), itTrkpip->getZError());
    WTrackParameter wpim = WTrackParameter(mpi, itTrkpim->getZHelix(), itTrkpim->getZError());
    WTrackParameter wp   = WTrackParameter(mproton, itTrkp->getZHelixP(), itTrkp->getZErrorP());
    WTrackParameter wpb  = WTrackParameter(mproton, itTrkpb->getZHelixP(), itTrkpb->getZErrorP());

  vtxfita0->init();
  vtxfita0->AddTrack(0, wp);
  vtxfita0->AddTrack(1, wpim);
  vtxfita0->AddVertex(0, vxpar, 0, 1);
  if(!vtxfita0->Fit(0)) return StatusCode::SUCCESS;
 vtxfita0->Swim(0);  
 vtxfita0->BuildVirtualParticle(0);
  vtxfita->init();
  vtxfita->AddTrack(0, vtxfita0->wVirtualTrack(0));
  vtxfita->setVpar(vtxfita0->vpar(0));
  if(!vtxfita->Fit()) return StatusCode::SUCCESS; 

  WTrackParameter wLambda    = vtxfita->wpar();   

  vtxfitb0->init();
  vtxfitb0->AddTrack(0, wpb);
  vtxfitb0->AddTrack(1, wpip);
  vtxfitb0->AddVertex(0, vxpar, 0, 1);
  if(!vtxfitb0->Fit(0)) return StatusCode::SUCCESS;
  vtxfitb0->Swim(0); 
  vtxfitb0->BuildVirtualParticle(0);

  vtxfitb->init();
  vtxfitb->AddTrack(0, vtxfitb0->wVirtualTrack(0));
  vtxfitb->setVpar(vtxfitb0->vpar(0));
  if(!vtxfitb->Fit()) return StatusCode::SUCCESS; 

  WTrackParameter wLambdabar = vtxfitb->wpar();
     
  vtxfit->init();
  vtxfit->AddTrack(0,   wLambda);
  vtxfit->AddTrack(1,   wLambdabar);
  vtxfit->AddVertex(0, vxpar,0, 1); 
  if(!vtxfit->Fit(0)) return StatusCode::SUCCESS;
  vtxfit->Swim(0);
  WTrackParameter wwLambda    = vtxfit->wtrk(0);
  WTrackParameter wwLambdabar = vtxfit->wtrk(1);


  // Kinamatic Fit

   KinematicFit* kmfit = KinematicFit::instance();


    HepLorentzVector ecms(0.034065,0.0,0.0,3.0969);
    const Hep3Vector u_cms = -ecms.boostVector();

    kmfit->init();
    kmfit->AddTrack(0, wwLambda);
    kmfit->AddTrack(1, wwLambdabar);
    kmfit->AddFourMomentum(0, ecms);

    if(!kmfit->Fit())  return StatusCode::SUCCESS;
      m_chisq = kmfit->chisq();
      if(m_chisq > 50)  return StatusCode::SUCCESS;
      HepLorentzVector kmf_pLambda    = kmfit->pfit(0);
      HepLorentzVector kmf_pLambdabar = kmfit->pfit(1);

  kmf_pLambda.boost(u_cms);
  kmf_pLambdabar.boost(u_cms);
  m_mLambda    = kmf_pLambda.m();
  m_mLambdabar = kmf_pLambdabar.m();
  m_pLambda    = kmf_pLambda.rho();
  m_pLambdabar = kmf_pLambdabar.rho();

  if(fabs(m_mLambda-1.1157)>0.003||fabs(m_mLambdabar-1.1157)>0.003) return StatusCode::SUCCESS;
  // finale selection
  
  m_tuple->write();
//      return StatusCode::SUCCESS;

  ////////////////////////////////////////////////////////////
  // DQA
  // set tag and quality

  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
  (*(evtRecTrkCol->begin()+iplus[0]))->setPartId(pidp0);
  (*(evtRecTrkCol->begin()+iplus[1]))->setPartId(pidp1);
  (*(evtRecTrkCol->begin()+iminus[0]))->setPartId(pidm0);
  (*(evtRecTrkCol->begin()+iminus[1]))->setPartId(pidm1);
  // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*(evtRecTrkCol->begin()+iplus[0]))->setQuality(0);
  (*(evtRecTrkCol->begin()+iplus[1]))->setQuality(0);
  (*(evtRecTrkCol->begin()+iminus[0]))->setQuality(0);
  (*(evtRecTrkCol->begin()+iminus[1]))->setQuality(0);

  // DQA
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);
  ////////////////////////////////////////////////////////////
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode JsiLL::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


