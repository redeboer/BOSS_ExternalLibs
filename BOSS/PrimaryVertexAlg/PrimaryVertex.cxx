#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EvtRecEvent/EvtRecPrimaryVertex.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/HTrackParameter.h"
#include "VertexFit/KalmanVertexFit.h"
#include "VertexFit/BField.h"
#include "VertexFit/FastVertexFit.h"
#include "ParticleID/ParticleID.h"
#include "PrimaryVertexAlg/PrimaryVertex.h"

#include <assert.h>
#include "TMath.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include <map>
#include <iostream>
#include <fstream>

using CLHEP::HepLorentzVector;
using namespace std;

const double xmass[5] = {0.000511, 0.105658, 0.139570, 0.493677, 0.938272}; //GeV
const double ecms = 3.097;
const double mpi0 = 0.134976;
const double momega = 0.78265;

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

//*******************************************************************************
PrimaryVertex::PrimaryVertex(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm (name, pSvcLocator)
{
  // Declare the properties
  declareProperty("Output", m_output = 0);
  declareProperty("TrackNumberCut", m_trackNumberCut = 1);
  declareProperty("CosThetaCut", m_cosThetaCut=0.93);
  declareProperty("Vz0Cut", m_vz0Cut = 40.0);
  // fit Method
  declareProperty("FitMethod", m_fitMethod = 0);
  // for global method
  declareProperty("Chi2CutOfGlobal", m_globalChisqCut = 200.0);

  // for Kalman method
  declareProperty("ChisqCut", m_chisqCut = 200.0);
  declareProperty("TrackIteration", m_trackIteration = 5);
  declareProperty("VertexIteration", m_vertexIteration = 5);
  declareProperty("Chi2CutforTrkIter", m_chi2CutforTrkIter = 0.1);
  declareProperty("FreedomCut", m_freedomCut = 1);
  declareProperty("Chi2CutforSmooth", m_chi2CutforSmooth = 200.0);
  //declareProperty("MinDistance", m_minDistance = 100.0);
  //declareProperty("MinPointX", m_minPointX = 0.1);
  //declareProperty("MinPointY", m_minPointY = 0.1);
}

//*******************************************************************************
StatusCode PrimaryVertex::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;
 
  for(int i = 0; i < 15; i++){
    m_sel_number[i] = 0;
  }

  StatusCode status;

  if (m_output == 1) {
    NTuplePtr nt1(ntupleSvc(), "FILE999/glbvtx");
    if(nt1) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILE999/glbvtx", CLID_ColumnWiseTuple, "global vertex");
      if(m_tuple1) {
        status = m_tuple1->addItem("gvx", m_gvx);
        status = m_tuple1->addItem("gvy", m_gvy);
        status = m_tuple1->addItem("gvz", m_gvz);
        status = m_tuple1->addItem("chig", m_chig);
        status = m_tuple1->addItem("ndofg", m_ndofg);
        status = m_tuple1->addItem("probg", m_probg);
      } else {
        log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple1) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt2(ntupleSvc(), "FILE999/chisq"); //chisq of Kalman filter 
    if(nt2) m_tuple2 = nt2;
    else {
      m_tuple2 = ntupleSvc()->book ("FILE999/chisq", CLID_ColumnWiseTuple, "chi-square of ");
      if(m_tuple2) {
        status = m_tuple2->addItem("chis", m_chis);
        status = m_tuple2->addItem("probs", m_probs);
        status = m_tuple2->addItem("chif", m_chif);
        status = m_tuple2->addItem("probf", m_probf);
      } else {
        log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple2) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt3(ntupleSvc(), "FILE999/Pull");
    if(nt3) m_tuple3 = nt3;
    else {
      m_tuple3 = ntupleSvc()->book ("FILE999/Pull", CLID_ColumnWiseTuple, "Pull");
      if(m_tuple3) {
        status = m_tuple3->addItem("pull_drho", m_pull_drho);
        status = m_tuple3->addItem("pull_phi", m_pull_phi);
        status = m_tuple3->addItem("pull_kapha", m_pull_kapha);
        status = m_tuple3->addItem("pull_dz", m_pull_dz);
        status = m_tuple3->addItem("pull_lamb", m_pull_lamb);
        status = m_tuple3->addItem("pull_momentum", m_pull_momentum);
      } else {
        log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple3) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt4(ntupleSvc(), "FILE999/kalvtx"); 
    if(nt4) m_tuple4 = nt4;
    else {
      m_tuple4 = ntupleSvc()->book ("FILE999/kalvtx", CLID_ColumnWiseTuple, "kalman vertex");
      if(m_tuple4) {
        status = m_tuple4->addItem("kvx", m_kvx);
        status = m_tuple4->addItem("kvy", m_kvy);
        status = m_tuple4->addItem("kvz", m_kvz);
        status = m_tuple4->addItem("chik", m_chik);
        status = m_tuple4->addItem("ndofk", m_ndofk);
        status = m_tuple4->addItem("probk", m_probk);
      } else {
        log << MSG::ERROR << "Cannot book N-tuple:" << long(m_tuple4) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  } //end if output

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;
}
//*******************************************************************************

StatusCode PrimaryVertex::RegisterEvtRecPrimaryVertex(
    EvtRecPrimaryVertex* aNewEvtRecPrimaryVertex, MsgStream& log) {
  DataObject *aEvtRecEvent;
  eventSvc()->findObject("/Event/EvtRec",aEvtRecEvent);
  if (aEvtRecEvent==NULL) {
    aEvtRecEvent = new EvtRecEvent();
    StatusCode sc = eventSvc()->registerObject("/Event/EvtRec",aEvtRecEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register EvtRecEvent" <<endreq;
      return StatusCode::FAILURE;
    }
  }
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
  DataObject *aEvtRecPrimaryVertex;
  eventSvc()->findObject("/Event/EvtRec/EvtRecPrimaryVertex",aEvtRecPrimaryVertex);
  if(aEvtRecPrimaryVertex != NULL) {
    //IDataManagerSvc *dataManSvc = dynamic_cast<IDataManagerSvc*> (eventSvc());
    dataManSvc->clearSubTree("/Event/EvtRec/EvtRecPrimaryVertex");
    eventSvc()->unregisterObject("/Event/EvtRec/EvtRecPrimaryVertex");
  }
  StatusCode sc = eventSvc()->registerObject("/Event/EvtRec/EvtRecPrimaryVertex",
                                                  aNewEvtRecPrimaryVertex);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EvtRecPrimaryVertex in TDS!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

///////////////////////////////////////////////////
//  Select good charged tracks in MDC ( no PID )
//////////////////////////////////////////////////
void PrimaryVertex::SelectGoodChargedTracks(
    SmartDataPtr<EvtRecEvent>& recEvent,
    SmartDataPtr<EvtRecTrackCol>& recTrackCol,
    Vint& icp, Vint& icm, Vint& iGood) {
  assert(recEvent->totalCharged() <= recTrackCol->size());
  for (unsigned int i = 0; i < recEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = recTrackCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    if (fabs(cos(mdcTrk->theta())) >= m_cosThetaCut) continue;
    if (fabs(mdcTrk->z()) >= m_vz0Cut) continue;
    iGood.push_back((*itTrk)->trackId());
    if (mdcTrk->charge() > 0) {
      icp.push_back((*itTrk)->trackId());
    } else if (mdcTrk->charge() < 0) {
      icm.push_back((*itTrk)->trackId());
    }
  }
}

void InitVertexParameter(VertexParameter& vxpar) {
  HepPoint3D vx(0., 0., 0.);
  HepSymMatrix Evx(3, 0);
  double bx = 1E+6; 
  double by = 1E+6;
  double bz = 1E+6;
  Evx[0][0] = bx*bx;
  Evx[1][1] = by*by;
  Evx[2][2] = bz*bz;
  vxpar.setVx(vx);
  vxpar.setEvx(Evx);
}

//***************************************************************************
StatusCode PrimaryVertex::execute() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endmsg;

  cout.precision(20);
  ///////////////////////////////////////////
  //  Read REC information
  //////////////////////////////////////////
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  SmartDataPtr<EvtRecEvent> recEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> recTrackCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  log << MSG::DEBUG << "run and event = " << eventHeader->runNumber() 
                                           << " " << eventHeader->eventNumber() << endreq;
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
          << recEvent->totalCharged() << " , "  << recEvent->totalNeutral() << " , "
          << recEvent->totalTracks() <<endreq;
  m_sel_number[0]++;
  /*
  if (eventHeader->eventNumber() % 1000 == 0) {
    cout << "Event number = " << eventHeader->eventNumber() << endl;
  }*/

  EvtRecPrimaryVertex* aNewEvtRecPrimaryVertex = new EvtRecPrimaryVertex;

  StatusCode sc = RegisterEvtRecPrimaryVertex(aNewEvtRecPrimaryVertex, log);
  if (sc != StatusCode::SUCCESS) {
    return sc;
  }
  
  // Cut 1 : for anything sample, at least 3 charged tracks
  if (recEvent->totalCharged() < m_trackNumberCut) {
    return StatusCode::SUCCESS;
  }
  m_sel_number[1]++;

  Vint icp, icm, iGood;
  SelectGoodChargedTracks(recEvent, recTrackCol, icp, icm, iGood);

  // Cut 2 : for anything sample, at least 3 good charged tracks 
  if (iGood.size() < m_trackNumberCut) {
    return StatusCode::SUCCESS;
  } 
  m_sel_number[2]++;

  //  Vertex Fit
  VertexParameter vxpar;
  InitVertexParameter(vxpar);

  // For Global Vertex Fitting
  if (m_fitMethod == 0) {
    VertexFit* vtxfit = VertexFit::instance();
    vtxfit->init();
    Vint tlis, trkidlis;
    for (int i = 0; i < iGood.size(); i++) {
      int trk_id = iGood[i];
      EvtRecTrackIterator itTrk = recTrackCol->begin() + trk_id;
      RecMdcKalTrack *kalTrk = (*itTrk)->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      WTrackParameter wtrk(xmass[2], kalTrk->helix(), kalTrk->err());
      vtxfit->AddTrack(i ,wtrk);
      tlis.push_back(i);
      trkidlis.push_back(trk_id);
    } 
    vtxfit->AddVertex(0, vxpar, tlis);
    if(!vtxfit->Fit(0)) return StatusCode::SUCCESS;  //FIXME
    if(vtxfit->chisq(0) > m_globalChisqCut) return StatusCode::SUCCESS; //FIXME
    if (m_output == 1) {
      m_chig = vtxfit->chisq(0);
      m_ndofg = 2 * iGood.size() - 3;
      m_probg = TMath::Prob(m_chig, m_ndofg);
      HepVector glvtx = vtxfit->Vx(0);
      m_gvx = glvtx[0];
      m_gvy = glvtx[1];
      m_gvz = glvtx[2];
      m_tuple1->write();
    }
/*
    cout << "========  After global vertex fitting =============" << endl;
    cout << "Event number = " << eventHeader->eventNumber() << endl;
    cout << "NTracks: " << iGood.size() << endl;
    cout << "Chisq: " << vtxfit->chisq(0) << endl;
    cout << "Ndof: " << 2 * iGood.size() - 3 << endl;
    cout << "FitMethod: "<< " 0 " << endl;
    cout << "Vertex position: " << vtxfit->Vx(0)<< endl;
    cout << "Vertex resolution: " << vtxfit->Evx(0) << endl;
    cout << "track id list : " << endl;
    for (int j = 0; j < trkidlis.size(); j++) {
      cout << trkidlis[j] << "  ";
    }
    cout << " " << endl; */

    aNewEvtRecPrimaryVertex->setNTracks(iGood.size());
    aNewEvtRecPrimaryVertex->setTrackIdList(trkidlis);
    aNewEvtRecPrimaryVertex->setChi2(vtxfit->chisq(0));
    aNewEvtRecPrimaryVertex->setNdof(2 * iGood.size() - 3);
    aNewEvtRecPrimaryVertex->setFitMethod(0);
    aNewEvtRecPrimaryVertex->setVertex(vtxfit->Vx(0));
    aNewEvtRecPrimaryVertex->setErrorVertex(vtxfit->Evx(0));
    aNewEvtRecPrimaryVertex->setIsValid(true);

  } else if (m_fitMethod == 1) { 
    // For Kalman Vertex Fitting
    KalmanVertexFit *kalvtx = KalmanVertexFit::instance();
    kalvtx->init();
    kalvtx->initVertex(vxpar);
    kalvtx->setChisqCut(m_chisqCut);
    kalvtx->setTrackIteration(m_trackIteration);
    kalvtx->setVertexIteration(m_vertexIteration);
    kalvtx->setTrackIterationCut(m_chi2CutforTrkIter);
    for(int i = 0; i < iGood.size(); i++) {
      int trk_id = iGood[i];
      EvtRecTrackIterator itTrk = recTrackCol->begin()+trk_id;
      RecMdcKalTrack *kalTrk = (*itTrk)->mdcKalTrack();
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      HTrackParameter htrk(kalTrk->helix(), kalTrk->err(), trk_id, 2);
      kalvtx->addTrack(htrk);
    }
    kalvtx->filter();

    //int freedomCut = -3 + 2*m_trackNumberCut;
    if(kalvtx->ndof() >=  m_freedomCut) {   //Cut : add 2 when add every track
      //for(int i = 0; i < kalvtx->numTrack(); i++) {
      for(int i = 0; i < iGood.size(); i++) {
        if (m_output == 1) {
          m_chif = kalvtx->chiF(i);
          m_chis = kalvtx->chiS(i);
          m_probs = TMath::Prob(m_chis, 2);
          m_probf = TMath::Prob(m_chif, 2);
          m_tuple2->write();
        }
        if(kalvtx->chiS(i) > m_chi2CutforSmooth) kalvtx->remove(i);
      }
    }
    if(kalvtx->ndof() >= m_freedomCut) { //FIXME  to Rhopi is 0 , others are 1
      for(int i = 0; i < iGood.size(); i++) {
        kalvtx->smooth(i);

        HepVector Pull(5, 0);
        Pull = kalvtx->pull(i);
        if (m_output == 1) {
          m_pull_drho = Pull[0];
          m_pull_phi = Pull[1];
          m_pull_kapha = Pull[2];
          m_pull_dz = Pull[3];
          m_pull_lamb = Pull[4];
          m_pull_momentum = kalvtx->pullmomentum(i);
          m_tuple3->write();
        }
      }
      if (m_output == 1) {
        m_chik = kalvtx->chisq();
        m_ndofk = kalvtx->ndof();
        m_probk = TMath::Prob(m_chik, m_ndofk);
        HepVector xp(3, 0);
        xp = kalvtx->x();
        m_kvx = xp[0];
        m_kvy = xp[1];
        m_kvz = xp[2];
        m_tuple4->write();
      }

      m_sel_number[3]++;
    /* 
      cout << "========  After Kalman vertex fitting =============" << endl;
      cout << "NTracks: " << kalvtx->numTrack() << endl;
      cout << "Chisq: " << kalvtx->chisq() << endl;
      cout << "Ndof: " <<  kalvtx->ndof() << endl;
      cout << "FitMethod: "<< " 1 " << endl;
      cout << "Vertex position: " << kalvtx->x() << endl;
      cout << "Vertex resolution: " << kalvtx->Ex() << endl;
      for (int j = 0; j < (kalvtx->trackIDList()).size(); j++) {
        cout << kalvtx->trackIDList()[j] << "  ";
      }
      cout << " " << endl;*/

      aNewEvtRecPrimaryVertex->setNTracks(kalvtx->numTrack());
      aNewEvtRecPrimaryVertex->setTrackIdList(kalvtx->trackIDList()); 
      aNewEvtRecPrimaryVertex->setChi2(kalvtx->chisq());
      aNewEvtRecPrimaryVertex->setNdof(kalvtx->ndof());
      aNewEvtRecPrimaryVertex->setFitMethod(1);
      aNewEvtRecPrimaryVertex->setVertex(kalvtx->x());
      aNewEvtRecPrimaryVertex->setErrorVertex(kalvtx->Ex());
      aNewEvtRecPrimaryVertex->setIsValid(true);

    }
  }
  return StatusCode::SUCCESS;
}

//***************************************************************************

StatusCode PrimaryVertex::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  log << MSG::ALWAYS << "==================================" << endreq;
  log << MSG::ALWAYS << "survived event :";
  for(int i = 0; i < 10; i++){
    log << MSG::ALWAYS << m_sel_number[i] << " ";
  }
  log << MSG::ALWAYS << endreq;
  log << MSG::ALWAYS << "==================================" << endreq;
  return StatusCode::SUCCESS;
}
  
