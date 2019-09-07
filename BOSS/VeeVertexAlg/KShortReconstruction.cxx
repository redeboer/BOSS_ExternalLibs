//
// KShortReconstruction -> pi+ pi- Reconstruction
//

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/Event.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "EventModel/EventHeader.h"
#include "EventModel/Event.h"
#include "EventModel/EventModel.h"
#include "McTruth/McParticle.h"
#include "ParticleID/ParticleID.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h"
#include "VeeVertexAlg/KShortReconstruction.h"
#include <vector>

typedef std::vector<int> Vint;
const double mpi =  0.13957;
//*******************************************************************************************
KShortReconstruction::KShortReconstruction(const std::string& name, ISvcLocator* pSvcLocator):
Algorithm(name, pSvcLocator)  {
  //Declare the properties
  
  declareProperty("Vz0Cut", m_vzCut = 20.0);
  declareProperty("CosThetaCut", m_cosThetaCut=0.93);
  declareProperty("ChisqCut", m_chisqCut = 100.0);
  /*
  declareProperty("PidUseDedx", m_useDedx = true);
  declareProperty("PidUseTof1", m_useTof1 = true);
  declareProperty("PidUseTof2", m_useTof2 = true);
  declareProperty("PidUseTofE", m_useTofE = false);
  declareProperty("PidUseTofQ", m_useTofQ = false);
  declareProperty("PidUseEmc", m_useEmc = false);
  declareProperty("PidProbCut", m_PidProbCut= 0.001);
  declareProperty("MassRangeLower", m_massRangeLower = 0.45);
  declareProperty("MassRangeHigher", m_massRangeHigher = 0.55);
  */
}

//******************************************************************************************
StatusCode KShortReconstruction::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" <<endreq;
  
  return StatusCode::SUCCESS;
}

//********************************************************************************************
StatusCode KShortReconstruction::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode KShortReconstruction::registerEvtRecVeeVertexCol(
    EvtRecVeeVertexCol* aNewEvtRecVeeVertexCol, MsgStream& log) {
  StatusCode sc = eventSvc()->registerObject("/Event/EvtRec/EvtRecVeeVertexCol",
                                             aNewEvtRecVeeVertexCol);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EvtRecVeeVertexCol in TDS!" << endreq;
  }
  return sc;
}

//*********************************************************************************************
StatusCode KShortReconstruction::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  StatusCode sc;

  //////////////////
  // Read REC data
  /////////////////
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  SmartDataPtr<EvtRecEvent> recEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> recTrackCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  log << MSG::DEBUG << "run and event = " << eventHeader->runNumber()
                                        << " " << eventHeader->eventNumber() << endreq;
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
          << recEvent->totalCharged() << " , "
          << recEvent->totalNeutral() << " , "
      << recEvent->totalTracks() <<endreq;
  int evtNo = eventHeader->eventNumber();
  //if (eventHeader->eventNumber() % 1000 == 0) {
  //  cout << "Event number = " << evtNo << endl;
  //}

  SmartDataPtr<EvtRecVeeVertexCol> veeVertexCol(eventSvc(),
                                                EventModel::EvtRec::EvtRecVeeVertexCol);
  if (!veeVertexCol) {
    veeVertexCol = new EvtRecVeeVertexCol;
    sc = registerEvtRecVeeVertexCol(veeVertexCol, log);
    if (sc != StatusCode::SUCCESS) {
      return sc;
    }
  }

  //////////////////////////////////////////////
  //  No PID : identify positive and negative
  ///////////////////////////////////////////////
  Vint ipip, ipim, iGood;
  for (unsigned int i = 0; i < recEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = recTrackCol->begin() + i;
//    EvtRecTrack* track = *itTrk;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    if (fabs(cos(mdcTrk->theta())) >= m_cosThetaCut) continue;
    if (fabs(mdcTrk->z()) >= m_vzCut) continue;
    iGood.push_back(i);
    if (mdcTrk->charge() > 0) ipip.push_back(i);
    if (mdcTrk->charge() < 0) ipim.push_back(i);
  }
  // Cut on good charged tracks 
  //if (iGood.size() < 2) return StatusCode::SUCCESS;
  if (ipip.size() < 1 || ipim.size() < 1) return StatusCode::SUCCESS;

  ///////////////////////////////////
  //  Vertex Fit
  //////////////////////////////////
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
 
  VertexFit *vtxfit0 = VertexFit::instance();

  for(unsigned int i1 = 0; i1 < ipip.size(); i1++) {
    int ip1 = ipip[i1];
    RecMdcKalTrack *pipKalTrk = (*(recTrackCol->begin()+ip1))->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
    WTrackParameter wpiptrk(mpi, pipKalTrk->helix(), pipKalTrk->err());

    for(unsigned int i2 = 0; i2 < ipim.size(); i2++) {
      int ip2 = ipim[i2];
      RecMdcKalTrack *pimKalTrk = (*(recTrackCol->begin()+ip2))->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      WTrackParameter wpimtrk(mpi, pimKalTrk->helix(), pimKalTrk->err());
   
      vtxfit0->init();
      vtxfit0->AddTrack(0, wpiptrk);
      vtxfit0->AddTrack(1, wpimtrk);
      vtxfit0->AddVertex(0, vxpar, 0, 1);
      if(!(vtxfit0->Fit(0))) continue;
      vtxfit0->BuildVirtualParticle(0);
      if(vtxfit0->chisq(0) > m_chisqCut) continue;
      WTrackParameter wKshort = vtxfit0->wVirtualTrack(0); //FIXME
      std::pair<int, int> pair;
      pair.first = 3;
      pair.second = 3;

      EvtRecTrack* track0 = *(recTrackCol->begin() + ip1);
      EvtRecTrack* track1 = *(recTrackCol->begin() + ip2);

      EvtRecVeeVertex* KsVertex = new EvtRecVeeVertex;
      KsVertex->setVertexId(310);
      KsVertex->setVertexType(0);
      KsVertex->setChi2(vtxfit0->chisq(0));
      KsVertex->setNdof(1);
      KsVertex->setMass(wKshort.p().m());
      KsVertex->setW(wKshort.w());
      KsVertex->setEw(wKshort.Ew());
      KsVertex->setPair(pair);
      KsVertex->setNCharge(0);
      KsVertex->setNTracks(2);
      KsVertex->addDaughter(track0, 0);
      KsVertex->addDaughter(track1, 1);
      veeVertexCol->push_back(KsVertex);
  /* 
      cout << "============= After Vertex fitting ===========" << endl; 
      cout << "Event number = " << evtNo << endl;
      cout << "chi2 = " << vtxfit0->chisq(0) << endl;
      cout << "mass = " << wKshort.p().m() << endl;
      cout << "w = " << wKshort.w() << endl;
      cout << "Ew = " << wKshort.Ew() << endl;
      cout << "track 0 : Z : " << track0->mdcTrack()->z() << endl;
      cout << "track 1 : Z : " << track1->mdcTrack()->z() << endl; */
    }
  }

  return StatusCode::SUCCESS;
}
