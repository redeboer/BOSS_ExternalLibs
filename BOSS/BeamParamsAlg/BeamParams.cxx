//
// To find the primary vertex in the inclusive sample
//                             
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
#include "VertexFit/VertexFit.h"
#include "VertexFit/HTrackParameter.h"
#include "VertexFit/KalmanVertexFit.h"
#include "ParticleID/ParticleID.h"
#include "BeamParamsAlg/BeamParams.h"
#include "GaudiKernel/ITHistSvc.h"
#include <assert.h>
#include "TMath.h"
#include "TH2D.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TPad.h"
#include <map>
#include <iostream>
#include <fstream>

using CLHEP::HepLorentzVector;
using namespace std;

const double xmass[5] = {0.000511, 0.105658, 0.139570, 0.493677, 0.938272}; //GeV
const double ecms = 3.097;

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

//*******************************************************************************
BeamParams::BeamParams(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm (name, pSvcLocator)
{
  // Declare the properties
  //declareProperty("RunNo", m_runNo = 9999);

  declareProperty("ParVer", m_parVer = 1);
  declareProperty("TrackNumberCut", m_trackNumberCut = 1);
  declareProperty("Vz0Cut", m_vz0Cut = 50.0);
  declareProperty("CosThetaCut", m_cosThetaCut=0.93);

  // particle identification
  declareProperty("OptionPID", m_pid = 0);
  declareProperty("PidUseDedx", m_useDedx = true);
  declareProperty("PidUseTof1", m_useTof1 = true);
  declareProperty("PidUseTof2", m_useTof2 = true);
  declareProperty("PidUseTofE", m_useTofE = false);
  declareProperty("PidUseTofQ", m_useTofQ = false);
  declareProperty("PidUseEmc", m_useEmc = false);
  declareProperty("PidUseMuc", m_useMuc = false);

  // vertex finding
  declareProperty("OptionVertexFind", m_vertexFind = 0);
  declareProperty("MinDistance", m_minDistance = 100.0);
  declareProperty("MinPointX", m_minPointX = 0.1);
  declareProperty("MinPointY", m_minPointY = 0.1);
  declareProperty("MeanPointX", m_meanPointX = 0.1);
  declareProperty("MeanPointY", m_meanPointY = -0.25);

  // Kalman vertex fitting
  declareProperty("ChisqCut", m_chisqCut = 200.0);
  declareProperty("TrackIteration", m_trackIteration = 5);
  declareProperty("VertexIteration", m_vertexIteration = 5);
  declareProperty("Chi2CutforTrkIter", m_chi2CutforTrkIter = 0.1);
  declareProperty("Chi2CutforSmooth", m_chi2CutforSmooth = 200.0);

  // output file name
  declareProperty("HadronFile", m_hadronFile = 0);
  declareProperty("DstFileName", m_fileNameDst = std::string("dst.txt"));
  declareProperty("HadronFileName", m_fileNameHadron = std::string("hadron.txt"));
  declareProperty("FigsName", m_figsName = std::string("figs.ps"));
}

//*******************************************************************************
StatusCode BeamParams::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;
   
  StatusCode sc=service("THistSvc", m_thsvc);
  if(sc.isFailure()) {
	  log << MSG::FATAL << "Couldn't get THistSvc" << endreq;
          exit(1);
  }

  for(int i = 0; i < 15; i++){
    m_sel_number[i] = 0;
  }

  //////////////////////
  //   Book histogram
  /////////////////////
  m_vertex_x = new TH1D("x_of_vertex", "x of vertex", 200, -5, 5);
  m_vertex_y = new TH1D("y_of_vertex", "y of vertex", 200, -5, 5);
  m_vertex_z = new TH1D("z_of_vertex", "z of vertex", 200, -10, 10);
  m_vertex_x_kal = new TH1D("x_of_vertex_in_kal", "x of vertex in kal", 200, -5, 5);
  m_vertex_y_kal = new TH1D("y_of_vertex_in_kal", "y of vertex in kal", 200, -5, 5);
  m_vertex_z_kal = new TH1D("z_of_vertex_in_kal", "z of vertex in kal", 200, -10, 10);
  if(m_thsvc->regHist("/DQAHist/zhsVER/x_of_vertex",m_vertex_x).isFailure()){
	  log << MSG::FATAL << "Couldn't register x of vertex " << endreq;
          exit(1);
  }
  if(m_thsvc->regHist("/DQAHist/zhsVER/y_of_vertex",m_vertex_y).isFailure()){
	  log << MSG::FATAL << "Couldn't register y of vertex" << endreq;
          exit(1);
  }
  if(m_thsvc->regHist("/DQAHist/zhsVER/z_of_vertex",m_vertex_z).isFailure()){
	  log << MSG::FATAL << "Couldn't register z of vertex" << endreq;
          exit(1);
  }
  if(m_thsvc->regHist("/DQAHist/zhsVER/x_of_vertex_in_kal",m_vertex_x_kal).isFailure()){
	  log << MSG::FATAL << "Couldn't register x of vertex in kal" << endreq;
          exit(1);
  }
  if(m_thsvc->regHist("/DQAHist/zhsVER/y_of_vertex_in_kal",m_vertex_y_kal).isFailure()){
	  log << MSG::FATAL << "Couldn't register y of vertex in kal" << endreq;
          exit(1);
  }
  if(m_thsvc->regHist("/DQAHist/zhsVER/z_of_vertex_in_kal",m_vertex_z_kal).isFailure()){
	  log << MSG::FATAL << "Couldn't register z of vertex in kal" << endreq;
          exit(1);
  }
  // end book

  StatusCode status;

  NTuplePtr nt1(ntupleSvc(), "FILE1/minid");// minimal distance 
  if(nt1) m_tuple1 = nt1;
  else {
    m_tuple1 = ntupleSvc()->book ("FILE1/minid", CLID_ColumnWiseTuple, "minimal distance");
    if(m_tuple1) {
      status = m_tuple1->addItem("xc", m_xc);
      status = m_tuple1->addItem("yc", m_yc);
      status = m_tuple1->addItem("zc", m_zc);
      status = m_tuple1->addItem("mind", m_mind);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple1) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt2(ntupleSvc(), "FILE1/chisq"); //chisq of Kalman filter 
  if(nt2) m_tuple2 = nt2;
  else {
    m_tuple2 = ntupleSvc()->book ("FILE1/chisq", CLID_ColumnWiseTuple, "chi-square of ");
    if(m_tuple2) {
      status = m_tuple2->addItem("chis", m_chis);
      status = m_tuple2->addItem("probs", m_probs);
      status = m_tuple2->addItem("chif", m_chif);
      status = m_tuple2->addItem("probf", m_probf);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple2) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt3(ntupleSvc(), "FILE1/kalvtx"); 
  if(nt3) m_tuple3 = nt3;
  else {
    m_tuple3 = ntupleSvc()->book ("FILE1/kalvtx", CLID_ColumnWiseTuple, "kalman vertex");
    if(m_tuple3) {
      status = m_tuple3->addItem("kvx", m_kvx);
      status = m_tuple3->addItem("kvy", m_kvy);
      status = m_tuple3->addItem("kvz", m_kvz);
      status = m_tuple3->addItem("chik", m_chik);
      status = m_tuple3->addItem("ndofk", m_ndofk);
      status = m_tuple3->addItem("probk", m_probk);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple3) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt4(ntupleSvc(), "FILE1/glbvtx");
  if(nt4) m_tuple4 = nt4;
  else {
    m_tuple4 = ntupleSvc()->book ("FILE1/glbvtx", CLID_ColumnWiseTuple, "global vertex");
    if(m_tuple4) {
      status = m_tuple4->addItem("gvx", m_gvx);
      status = m_tuple4->addItem("gvy", m_gvy);
      status = m_tuple4->addItem("gvz", m_gvz);
      status = m_tuple4->addItem("chig", m_chig);
      status = m_tuple4->addItem("ndofg", m_ndofg);
      status = m_tuple4->addItem("probg", m_probg);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple4) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt5(ntupleSvc(), "FILE1/Pull");
  if(nt5) m_tuple5 = nt5;
  else {
    m_tuple5 = ntupleSvc()->book ("FILE1/Pull", CLID_ColumnWiseTuple, "Pull");
    if(m_tuple5) {
      status = m_tuple5->addItem("pull_drho", m_pull_drho);
      status = m_tuple5->addItem("pull_phi", m_pull_phi);
      status = m_tuple5->addItem("pull_kapha", m_pull_kapha);
      status = m_tuple5->addItem("pull_dz", m_pull_dz);
      status = m_tuple5->addItem("pull_lamb", m_pull_lamb);
      status = m_tuple5->addItem("pull_momentum", m_pull_momentum);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple5) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt6(ntupleSvc(), "FILE1/MdcTrack");
  if(nt6) m_tuple6 = nt6;
  else {
    m_tuple6 = ntupleSvc()->book ("FILE1/MdcTrack", CLID_ColumnWiseTuple, "MdcTrack");
    if(m_tuple6) {
      status = m_tuple6->addItem("MdcTrkX", m_mdcTrk_x);
      status = m_tuple6->addItem("MdcTrkY", m_mdcTrk_y);
      status = m_tuple6->addItem("MdcTrkZ", m_mdcTrk_z);
      status = m_tuple6->addItem("MdcTrkR", m_mdcTrk_r);
      status = m_tuple6->addItem("MdcTrkDrho", m_mdcTrk_dr);
      status = m_tuple6->addItem("Rxy", m_rxy);
      status = m_tuple6->addItem("MdcKalTrkZ", m_mdcKalTrk_z);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple6) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  NTuplePtr nt7(ntupleSvc(), "FILE1/PullG");
  if(nt7) m_tuple7 = nt7;
  else {
    m_tuple7 = ntupleSvc()->book ("FILE1/PullG", CLID_ColumnWiseTuple, "Pull");
    if(m_tuple7) {
      status = m_tuple7->addItem("gpull_drho", m_gpull_drho);
      status = m_tuple7->addItem("gpull_phi", m_gpull_phi);
      status = m_tuple7->addItem("gpull_kapha", m_gpull_kapha);
      status = m_tuple7->addItem("gpull_dz", m_gpull_dz);
      status = m_tuple7->addItem("gpull_lamb", m_gpull_lamb);
    } else {
      log << MSG::FATAL << "Cannot book N-tuple:" << long(m_tuple7) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;
}

#define DEB cout << __FILE__ << ":" << __LINE__ << endl;

//***************************************************************************
StatusCode BeamParams::execute() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endmsg;

  ///////////////////
  //  Read REC data
  //////////////////
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  SmartDataPtr<EvtRecEvent> recEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> recTrackCol(eventSvc(), EventModel::EvtRec::EvtRecTrackCol);
  log << MSG::DEBUG << "run and event = " << eventHeader->runNumber() 
                                        << " " << eventHeader->eventNumber() << endreq;
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
          << recEvent->totalCharged() << " , "
          << recEvent->totalNeutral() << " , "
      << recEvent->totalTracks() <<endreq;
  
  m_runNo = eventHeader->runNumber();
  //FIXME : cout
  if (eventHeader->eventNumber() % 5000 == 0) {
    cout << "Event number = " << eventHeader->eventNumber()<<" run number = "<< m_runNo << endl;
  }
  m_sel_number[0]++;

  // Cut 1 : Number of tracks. For anything sample, at least 3 charged tracks
  if (recEvent->totalCharged() < m_trackNumberCut) return StatusCode::SUCCESS;
  m_sel_number[1]++;

  ///////////////////////////////////////////////////
  //  Select good charged tracks in MDC ( option for PID )
  //////////////////////////////////////////////////
  Vint icp, icm, iGood, jGood;
  icp.clear();
  icm.clear();
  iGood.clear();
  jGood.clear();

  map<int, int> ParticleType; //0:e, 1:mu, 2:pi, 3:K, 4:p
  ParticleID* pid = ParticleID::instance();

  for(unsigned int i = 0; i < recEvent->totalCharged(); i++) {
    jGood.push_back(0);
    EvtRecTrackIterator itTrk = recTrackCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    if(!(*itTrk)->isMdcKalTrackValid()) continue;
    RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();
    if (fabs(cos(mdcTrk->theta())) >= m_cosThetaCut) continue;
    if (fabs(mdcTrk->z()) >= m_vz0Cut) continue;
    //iGood.push_back((*itTrk)->trackId());
    iGood.push_back(i);

    if (m_pid == 0) {
      if (mdcTrk->charge() > 0) {
        //icp.push_back((*itTrk)->trackId());
        icp.push_back(i);
      } else if (mdcTrk->charge() < 0) {
        //icm.push_back((*itTrk)->trackId());
        icm.push_back(i);
      }
    } else if (m_pid == 1) {
      ParticleType[i] = 2; // FIXME default value is Pion ?
      // pid info
      pid->init();
      pid->setChiMinCut(4);
      pid->setRecTrack(*itTrk);
      pid->setMethod(pid->methodProbability());
      //pid->setMethod(pid->methodLikelihood());    
      // use PID sub-system
      if(m_useDedx)pid->usePidSys(pid->useDedx());
      if(m_useTof1)pid->usePidSys(pid->useTof1());
      if(m_useTof2)pid->usePidSys(pid->useTof2());
      if(m_useTofE)pid->usePidSys(pid->useTofE());
      if(m_useTofQ)pid->usePidSys(pid->useTofQ());
      if(m_useEmc)pid->usePidSys(pid->useEmc());
      if(m_useMuc)pid->usePidSys(pid->useMuc());
      pid->identify(pid->onlyElectron());
      pid->identify(pid->onlyMuon());
      pid->identify(pid->onlyPion());
      pid->identify(pid->onlyKaon());
      pid->identify(pid->onlyProton());
      pid->calculate();
      if(!pid->IsPidInfoValid()) continue;
      double prob_value[5];
      prob_value[0] = pid->probElectron();
      prob_value[1] = pid->probMuon();
      prob_value[2] = pid->probPion();
      prob_value[3] = pid->probKaon();
      prob_value[4] = pid->probProton();

      int max = 0;
      for (int i = 1; i < 5; i++) {
        if (prob_value[i] > prob_value[max]) {
          max = i;
        }
      }
    
//cout << "PID : n = " << n << endl;
 
      if(max == 0) ParticleType[i] = 0; //m_sel_number[3]++;}
      if(max == 1) ParticleType[i] = 1; //m_sel_number[4]++;}
      if(max == 2) ParticleType[i] = 2; //m_sel_number[5]++;}
      if(max == 3) ParticleType[i] = 3;//m_sel_number[6]++;}
      if(max == 4) ParticleType[i] =4;//m_sel_number[7]++;}
    }

    // fill z position of track parameters 
    RecMdcKalTrack *kalTrk = (*itTrk)->mdcKalTrack();
    RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
    m_mdcTrk_x = mdcTrk->x();
    m_mdcTrk_y = mdcTrk->y();
    m_mdcTrk_z = mdcTrk->helix(3);
    m_mdcTrk_r = mdcTrk->r();
    m_mdcTrk_dr = mdcTrk->helix(0);
    m_mdcKalTrk_z = kalTrk->helix()[3];
    m_tuple6->write();
  }

  // Cut 2 : for anything sample, at least 3 good charged tracks 
  if (iGood.size() < m_trackNumberCut) return StatusCode::SUCCESS;
  m_sel_number[2]++;

  /////////////////////
  //   Vertex Finding
  ////////////////////
  if (m_vertexFind == 1) {
    int nGood = iGood.size();
    for(int i1 = 0; i1 < nGood; i1++) {
      int id_trk1 = iGood[i1];
      EvtRecTrackIterator trk1 = (recTrackCol->begin()+id_trk1);
      RecMdcKalTrack *kalTrk1 = (*trk1)->mdcKalTrack();
      //FIXME default set is ?
      HTrackParameter htrk1, htrk2;
      if (m_pid == 0) {
        RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
        htrk1 = HTrackParameter(kalTrk1->helix(), kalTrk1->err(), id_trk1, 2);
      } else if (m_pid == 1) {
        RecMdcKalTrack::setPidType((RecMdcKalTrack::PidType)ParticleType[id_trk1]);
        htrk1 = HTrackParameter(kalTrk1->helix(),kalTrk1->err(), id_trk1, ParticleType[id_trk1]);
      }
      for(int i2 = i1+1; i2 < nGood; i2++) {
        int id_trk2 = iGood[i2];
        EvtRecTrackIterator trk2 = (recTrackCol->begin()+id_trk2);
        RecMdcKalTrack *kalTrk2 = (*trk2)->mdcKalTrack();
        //FIXME default set is ?
        if (m_pid == 0) {
          RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
          htrk2 = HTrackParameter(kalTrk2->helix(), kalTrk2->err(), id_trk2, 2);
        } else if (m_pid == 1) {
          RecMdcKalTrack::setPidType((RecMdcKalTrack::PidType)ParticleType[id_trk2]);
          htrk2 = HTrackParameter(kalTrk2->helix(),kalTrk2->err(),id_trk2,ParticleType[id_trk2]);
        }
        HepPoint3D cross(0., 0., 0.);
        double dist = htrk1.minDistanceTwoHelix(htrk2, cross);
        m_mind = dist;
        m_xc = cross.x();
        m_yc = cross.y();
        m_zc = cross.z();
        m_tuple1->write();
	
         if(sqrt(cross.x()*cross.x()+cross.y()*cross.y())>2)continue;
        
	if(fabs(dist) > m_minDistance) continue;  // Cut condition 2
       // double cross_cut = (cross.x() - m_meanPointX) * (cross.x() - m_meanPointX)
       //                              /m_minPointX/m_minPointX
       //                   + (cross.y() - m_meanPointY) * (cross.y() - m_meanPointY)
       //                              /m_minPointY/m_minPointY;
       //                             //FIXME sigma value from Gaussian fit //0.071/0.059 
        double cross_cut=0.;
	if(cross_cut < 3.5 * 3.5) {   //Cut condition 3
          jGood[id_trk1] = 1;
          jGood[id_trk2] = 1;
        } //end if
      }
    }

    iGood.clear();
    for(int i =0; i < jGood.size(); i++) {
      if(jGood[i] == 1) iGood.push_back(i);
    }

  } //end if vertex finding

  if (iGood.size() >= 2) m_sel_number[3]++;
  if (iGood.size() >= 3) m_sel_number[4]++;

  ///////////////////////////////
  //  Vertex Fit
  //////////////////////////////
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

  VertexFit* vtxfit = VertexFit::instance();
  // Step 1: using MdcKalTrk parameters
  vtxfit->init();
  Vint tlis;
  tlis.clear();
  for(int i = 0; i < iGood.size(); i++) {
    int trk_id = iGood[i];
    EvtRecTrackIterator itTrk = recTrackCol->begin()+trk_id;
    RecMdcKalTrack *kalTrk = (*itTrk)->mdcKalTrack();
    if (m_pid == 0) {
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      WTrackParameter wtrk(xmass[2], kalTrk->helix(), kalTrk->err());
      vtxfit->AddTrack(i ,wtrk);
    } else if (m_pid == 1) {
      RecMdcKalTrack::setPidType((RecMdcKalTrack::PidType)ParticleType[trk_id]);
      WTrackParameter wtrk(xmass[ParticleType[trk_id]], kalTrk->helix(), kalTrk->err());
      vtxfit->AddTrack(i ,wtrk);
    }
    tlis.push_back(i);
  }
  //if(iGood.size() >= m_trackNumberCut) { //at least N tracks
    vtxfit->AddVertex(0, vxpar, tlis);
    if(!vtxfit->Fit(0)) return StatusCode::SUCCESS;
    m_chig = vtxfit->chisq(0);
    m_ndofg = 2 * iGood.size() - 3;
    m_probg = TMath::Prob(m_chig, m_ndofg);
    HepVector glvtx = vtxfit->Vx(0);
    m_gvx = glvtx[0];
    m_gvy = glvtx[1];
    m_gvz = glvtx[2];
    m_tuple4->write();
 
    if(!(m_vertex_x->Fill(glvtx[0]))){
	    log << MSG::FATAL << "Couldn't Fill x of vertex" << endreq;
	    exit(1);
    }
    if(!(m_vertex_y->Fill(glvtx[1]))){
            log << MSG::FATAL << "Couldn't Fill y of vertex" << endreq;
	    exit(1);
    }
    if(!(m_vertex_z->Fill(glvtx[2]))){
	  log << MSG::FATAL << "Couldn't Fill z of vertex" << endreq;
	  exit(1);
    }
  //}
  for (int j = 0; j < iGood.size(); j++) {
    HepVector pull(5, 0);
    bool success = vtxfit->pull(0, j, pull);
    if (success) {
      m_gpull_drho = pull[0];
      m_gpull_phi = pull[1];
      m_gpull_kapha = pull[2];
      m_gpull_dz = pull[3];
      m_gpull_lamb = pull[4];
      m_tuple7->write();
    } 
  }
 
  // Step 2 : Kalman Vertex Fitting
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
    if (m_pid == 0) {
      RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);
      HTrackParameter htrk(kalTrk->helix(), kalTrk->err(), trk_id, 2);
      kalvtx->addTrack(htrk);
    } else if (m_pid == 1) {
      RecMdcKalTrack::setPidType((RecMdcKalTrack::PidType)ParticleType[trk_id]);
      HTrackParameter htrk(kalTrk->helix(), kalTrk->err(), trk_id, ParticleType[trk_id]);
      kalvtx->addTrack(htrk);
    }
  }
  kalvtx->filter();
  //int freedomCut = -3 + 2*m_trackNumberCut;
  int freedomCut = 1;
  if(kalvtx->ndof() >= freedomCut) {  //Cut condition 5 //add 2 when add every track
    //for(int i = 0; i < kalvtx->numTrack(); i++) {
    for(int i = 0; i < iGood.size(); i++) {
      m_chif = kalvtx->chiF(i);
      m_chis = kalvtx->chiS(i);
      m_probs = TMath::Prob(m_chis, 2);
      m_probf = TMath::Prob(m_chif, 2);
      m_tuple2->write();

      if(kalvtx->chiS(i) > m_chi2CutforSmooth) kalvtx->remove(i);
    }
  }
  if(kalvtx->ndof() >= freedomCut) { //FIXME  to Rhopi is 0 , others are 1
    for(int i = 0; i < iGood.size(); i++) {
      kalvtx->smooth(i);

      HepVector Pull(5, 0);
      Pull = kalvtx->pull(i);
      m_pull_drho = Pull[0];
      m_pull_phi = Pull[1];
      m_pull_kapha = Pull[2];
      m_pull_dz = Pull[3];
      m_pull_lamb = Pull[4];
      m_pull_momentum = kalvtx->pullmomentum(i);
      m_tuple5->write();
    }

    m_chik = kalvtx->chisq();
    m_ndofk = kalvtx->ndof();
    m_probk = TMath::Prob(m_chik, m_ndofk);
    HepVector xp(3, 0);
    xp = kalvtx->x();
    m_kvx = xp[0];
    m_kvy = xp[1];
    m_kvz = xp[2];
    m_tuple3->write();

    if(!(m_vertex_x_kal->Fill(xp[0]))){
	    log << MSG::FATAL << "Couldn't Fill kal x of vertex" << endreq;
	    exit(1);
    }
    if(!(m_vertex_y_kal->Fill(xp[1]))){
	    log << MSG::FATAL << "Couldn't Fill kal y of vertex" << endreq;
	    exit(1);
    }
    if(!(m_vertex_z_kal->Fill(xp[2]))){
	    log << MSG::FATAL << "Couldn't Fill kal z of vertex" << endreq;
	    exit(1);
    }

    m_sel_number[3]++;
  }
  return StatusCode::SUCCESS;
}

//***************************************************************************

StatusCode BeamParams::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  /*TF1 *func = new TF1("func", "gaus", -0.6, 0.6);
  m_vertex_x->Fit("func", "NR+");
  Double_t MeanX = func->GetParameter(1);
  Double_t SigmaX = func->GetParameter(2);
  Double_t ErrMeanX = func->GetParError(1);
  Double_t ErrSigmaX = func->GetParError(2);

  TF1 *funcY = new TF1("funcY", "gaus", -0.6, 0.1);
  m_vertex_y->Fit("funcY", "NR+");
  Double_t MeanY = funcY->GetParameter(1);
  Double_t SigmaY = funcY->GetParameter(2);
  Double_t ErrMeanY = funcY->GetParError(1);
  Double_t ErrSigmaY = funcY->GetParError(2);

  TF1 *funcZ = new TF1("funcZ", "gaus", -6, 6);
  m_vertex_z->Fit("funcZ", "NR+");
  Double_t MeanZ = funcZ->GetParameter(1);
  Double_t SigmaZ = funcZ->GetParameter(2);
  Double_t ErrMeanZ = funcZ->GetParError(1);
  Double_t ErrSigmaZ = funcZ->GetParError(2);

  TCanvas *myC = new TCanvas("myC", "myC", 1200, 400);
  TPad *background = (TPad*)gPad;
  background->SetFillColor(10);
  myC->Divide(3,1);
  myC->cd(1);

  m_vertex_x_kal->Fit("func", "R+");
  Double_t MeanXKal = func->GetParameter(1);
  Double_t SigmaXKal = func->GetParameter(2);
  Double_t ErrMeanXKal = func->GetParError(1);
  Double_t ErrSigmaXKal = func->GetParError(2);

  myC->cd(2);
  m_vertex_y_kal->Fit("funcY", "R+");
  Double_t MeanYKal = funcY->GetParameter(1);
  Double_t SigmaYKal = funcY->GetParameter(2);
  Double_t ErrMeanYKal = funcY->GetParError(1);
  Double_t ErrSigmaYKal = funcY->GetParError(2);

  myC->cd(3);
  m_vertex_z_kal->Fit("funcZ", "R+");
  Double_t MeanZKal = funcZ->GetParameter(1);
  Double_t SigmaZKal = funcZ->GetParameter(2); 
  Double_t ErrMeanZKal = funcZ->GetParError(1);
  Double_t ErrSigmaZKal = funcZ->GetParError(2);
  
  cout << "Kal: Mean X, Y, Z = "<<MeanXKal << "  " << MeanYKal << "  " << MeanZKal << endl;
  cout << "Kal: Sigma X, Y, Z = "<<SigmaXKal<<"  " <<SigmaYKal <<"  " << SigmaZKal << endl;
  cout << "Gl: Mean X, Y, Z = " << MeanX << "  " << MeanY << "  " << MeanZ << endl;
  cout << "Gl: Sigma X, Y, Z = " << SigmaX << "  " << SigmaY << "  " << SigmaZ << endl;

  //////////////////////////////////////////////////////////////////
  // Output a TXT file and a .ps figure for storing the fit results  
  //////////////////////////////////////////////////////////////////
  const char *file_name, *figs_name;
  if (m_hadronFile == 0) {
    file_name = m_fileNameDst.c_str();
  } else if (m_hadronFile == 1) {
    file_name = m_fileNameHadron.c_str();
  }

  figs_name = m_figsName.c_str();
  myC->SaveAs(figs_name);

  ofstream file(file_name);
 
  file << getenv("BES_RELEASE") << " " << m_parVer << " " << m_runNo << endl;  
  file << MeanX << " " << MeanY << " " << MeanZ << " "<< SigmaX << " "<< SigmaY << " " << SigmaZ << endl;
  file << ErrMeanX << " " << ErrMeanY<< " " << ErrMeanZ << " " << ErrSigmaX << " " << ErrSigmaY << " " << ErrSigmaZ << endl;
  file << MeanXKal << " "<< MeanYKal << " "<< MeanZKal << " "<< SigmaXKal << " " << SigmaYKal << " " << SigmaZKal << endl;
  file << ErrMeanXKal << " " << ErrMeanYKal<< " " << ErrMeanZKal << " " << ErrSigmaXKal << " " << ErrSigmaYKal << " " << ErrSigmaZKal << endl;*/

  /*delete m_vertex_x;
  delete m_vertex_y;
  delete m_vertex_z;
  delete m_vertex_x_kal;
  delete m_vertex_y_kal;
  delete m_vertex_z_kal;
  */
  ////////////////////////////////////////////////
  log << MSG::ALWAYS << "==================================" << endreq;
  log << MSG::ALWAYS << "survived event :";
  for(int i = 0; i < 10; i++){
    log << MSG::ALWAYS << m_sel_number[i] << " ";
  }
  log << MSG::ALWAYS << endreq;
  log << MSG::ALWAYS << "==================================" << endreq;
  return StatusCode::SUCCESS;
}
  
