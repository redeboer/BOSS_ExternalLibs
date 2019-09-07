#include <vector>

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

#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "ParticleID/ParticleID.h"

//
#include "EvtSelExample/EvtSelExample.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepLorentzVector;

const double mpi = 0.13957;
const double mk = 0.493677;
const double xmass[5] = {0.000511, 0.105658, 0.139570,0.493677, 0.938272};
const double velc = 299.792458;   // tof path unit in mm
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

/////////////////////////////////////////////////////////////////////////////

EvtSelExample::EvtSelExample(const std::string& name, ISvcLocator* pSvcLocator) :
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
StatusCode EvtSelExample::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  StatusCode status;

  // DQA
  // The first directory specifier must be "DQAFILE"
  // The second is the control sample name, the first letter is in upper format. eg. "Rhopi"
  NTuplePtr nt(ntupleSvc(), "DQAFILE/Rhopi");
  if ( nt ) m_tuple = nt;
  else {
      m_tuple = ntupleSvc()->book("DQAFILE/Rhopi", CLID_ColumnWiseTuple, "Rhopi ntuple");
      if( m_tuple ) {
          status = m_tuple->addItem("runNo", m_runNo);
          status = m_tuple->addItem("event", m_event);
      } else {
          log << MSG::ERROR << "Can not book N-tuple:" << long(m_tuple) << endreq;
      }
  }

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;


}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EvtSelExample::execute() {
  
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

  Vint iGood, ipip, ipim;
  iGood.clear();
  ipip.clear();
  ipim.clear();
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
    iGood.push_back((*itTrk)->trackId());
    nCharge += mdcTrk->charge();
    if (mdcTrk->charge() > 0) {
        ipip.push_back((*itTrk)->trackId());
    } else {
        ipim.push_back((*itTrk)->trackId());
    }
  }
  
  //
  // Finish Good Charged Track Selection
  //
  int nGood = iGood.size();
  log << MSG::DEBUG << "ngood, totcharge = " << nGood << " , " << nCharge << endreq;
  if((nGood != 2)||(nCharge!=0)){
      return StatusCode::SUCCESS;
  }

  Vint iGam;
  iGam.clear();
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
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

//      if(fabs(thed) < fabs(dthe)) dthe = thed;
//      if(fabs(phid) < fabs(dphi)) dphi = phid;
      if(angd < dang) {
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
    if(dang < m_gammaAngCut) continue;
//    if((fabs(dthe) < m_gammaThetaCut) && (fabs(dphi)<m_gammaPhiCut) ) continue;
    //
    // good photon cut will be set here
    //
    iGam.push_back((*itTrk)->trackId());
  }
  
  //
  // Finish Good Photon Selection
  //
  int nGam = iGam.size();

  log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral()<<endreq;
  if(nGam<2){
    return StatusCode::SUCCESS;
  }

  // Vertex Fit

  // Kinamatic Fit

  // finale selection
  
  m_tuple->write();

  ////////////////////////////////////////////////////////////
  // DQA
  // set tag and quality

  // Pid: 1 - electron, 2 - muon, 3 - pion, 4 - kaon, 5 - proton
  
  (*(evtRecTrkCol->begin()+ipip[0]))->setPartId(2);
  (*(evtRecTrkCol->begin()+ipim[0]))->setPartId(2);
  // Quality: defined by whether dE/dx or TOF is used to identify particle
  // 0 - no dE/dx, no TOF (can be used for dE/dx and TOF calibration)
  // 1 - only dE/dx (can be used for TOF calibration)
  // 2 - only TOF (can be used for dE/dx calibration)
  // 3 - Both dE/dx and TOF
  (*(evtRecTrkCol->begin()+ipip[0]))->setQuality(0);
  (*(evtRecTrkCol->begin()+ipim[0]))->setQuality(0);

  // DQA
  // Add the line below at the end of execute(), (before return)
  //
  setFilterPassed(true);
  ////////////////////////////////////////////////////////////
  
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EvtSelExample::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}


