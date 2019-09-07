#include "MdcNavigation/MdcNavigation.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventNavigator/EventNavigator.h"
#include <cmath>
#include "HepPDT/ParticleDataTable.hh"
#include "HepPDT/ParticleData.hh"
#include "GaudiKernel/IPartPropSvc.h"
#include "CLHEP/Geometry/Point3D.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcGeom/Constants.h"

#include "MdcGeom/BesAngle.h"
#include "TrkBase/HelixTraj.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "TrkBase/TrkPocaBase.h"
#include "TrkBase/TrkPoca.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcData/MdcHit.h"

#include "Identifier/MdcID.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EvTimeEvent/RecEsTime.h"

#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
using namespace Event;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif




using namespace std;
using namespace Event;
const double epsilon      =   0.000000001;

MdcNavigation::MdcNavigation(const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator) {
    declareProperty("hist", m_hist = 0);
    declareProperty("nMcHit", m_nMcHit= 5);
    declareProperty("mc", m_mc = 1);

    declareProperty("maxMdcDigi",   m_maxMdcDigi= 0);
    declareProperty("keepBadTdc",   m_keepBadTdc= 0);
    declareProperty("dropHot",      m_dropHot= 0);
    declareProperty("keepUnmatch",  m_keepUnmatch= 0);

    declareProperty("poca", m_poca = false);
    declareProperty("doSag", m_doSag = false);

    declareProperty("d0Cut", m_d0Cut= 1.);
    declareProperty("z0Cut", m_z0Cut= 10.);
    declareProperty("debug", m_debug = 0);

  }


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdcNavigation::initialize(){
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  t_nTk = 0;
  //Initailize magnetic filed 
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
    return StatusCode::FAILURE;
  }


  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  sc = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!sc.isSuccess() || 0 == p_PartPropSvc) {
    log << MSG::ERROR << " Could not initialize PartPropSvc" << endreq;
    return sc;
  }

  m_particleTable = p_PartPropSvc->PDT();

  IRawDataProviderSvc* irawDataProviderSvc;
  sc = service ("RawDataProviderSvc", irawDataProviderSvc);
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*> (irawDataProviderSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  }


  if (m_hist){ 
    sc = bookNTuple();
    if (!sc.isSuccess()) { 
      log << MSG::WARNING << " Could not book NTuple" << endreq; 
      m_hist = 0;
    }
  }

  keepedParticles = new int(211);


  return StatusCode::SUCCESS;
}

StatusCode MdcNavigation::beginRun(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in beginRun()" << endreq;

  m_gm = MdcDetector::instance(m_doSag);
  if(NULL == m_gm) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcNavigation::execute() {
  setFilterPassed(false);
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  // Get EventNavigator from the TDS
  SmartDataPtr<EventNavigator> navigator (eventSvc(),"/Event/Navigator");
  if( ! navigator ) {
    log << MSG::WARNING<< " Unable to retrieve EventNavigator" << endreq;
    m_rawData = true;
  }
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(), "/Event/Recon/RecMdcTrackCol");
  SmartDataPtr<RecMdcHitCol> recMdcHitCol(eventSvc(), "/Event/Recon/RecMdcHitCol");

  // get eventNo, t0 and MdcDigi
  if(m_hist){
    sc = fillInit();
    if ( sc!=StatusCode::SUCCESS ) {
      return StatusCode::FAILURE;
    }
  }
  if (m_mc){
    // Get McParticleCol 
    SmartDataPtr<Event::McParticleCol> mcParticles(eventSvc(),"/Event/MC/McParticleCol");
    SmartDataPtr<Event::MdcMcHitCol> mcHit(eventSvc(),"/Event/MC/MdcMcHitCol");
    if( ! mcParticles ) {
      log << MSG::WARNING<< " Unable to retrieve McParticleCol" << endreq;
    }else{
      // For each McParticle ...       
      t_mcTkNum = 0;
      McParticleCol::iterator it= mcParticles->begin();
      log <<MSG::INFO << "mcParticles size = "<<mcParticles->size() << endreq;//yzhang debug
      for(; it!= mcParticles->end(); it++ ) {     
	//int tkId = (*it)->trackIndex();
	t_mcTkNum++;
      } 
    }
  }
  t_mcTkNum = 0;
  t_recTkNum = 0;
  //for each rec track

  if (!recMdcTrackCol){
    log << MSG::WARNING<< " Unable to retrieve recMdcTrackCol" << endreq;
    return StatusCode::SUCCESS;
  }
  t_recTkNum = recMdcTrackCol->size();

  //=============loop over tracks==============
  RecMdcTrackCol::iterator it = recMdcTrackCol->begin(); 
  for(;it != recMdcTrackCol->end(); it++ ) {   
    if ( m_mc && navigator ) {
      McParticleVector particles = navigator->getMcParticles(*it);
      t_mcTkNum = particles.size();
      RecMdcTrackVector tracks = navigator->getMdcTracks(particles[0]);
      // for FIRST parent particle
      if ( sc!=StatusCode::SUCCESS ) return StatusCode::FAILURE;
    }
    sc = fillRecTrack(*it, t_mcTkNum, t_recTkNum);
    t_nTk++;
    if ( sc!=StatusCode::SUCCESS ) return StatusCode::FAILURE;
  }//end of loop over tracks

  //=============loop over hits==============
  fillRecHits(*recMdcHitCol);


  if(m_hist){fillEvent();}

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdcNavigation::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  std::cout<< "nTk ==  "<<t_nTk << std::endl;//yzhang debug
  delete keepedParticles; //FIXME
  return StatusCode::SUCCESS;
}

// routine to calculate momentum using helix parameters of fitted track
Hep3Vector MdcNavigation::momentum(const RecMdcTrack* trk) {
  // double dr   = trk->helix(0);
  double fi0  = trk->helix(1);
  double cpa  = trk->helix(2);
  double tanl = trk->helix(4);

  double pxy = 0.;
  if(cpa != 0) pxy = 1/fabs(cpa);

  double px = pxy * (-sin(fi0));
  double py = pxy * cos(fi0);
  double pz = pxy * tanl;

  Hep3Vector p;
  p.set(px, py, pz); // MeV/c
  return p;
}

StatusCode MdcNavigation::fillRecTrack(const RecMdcTrack* tk, int mcTkNum, int recTkNum){

  //int mcTkId = m_na_mcTkId;
  //m_na_nDigi = nDigiTk[mcTkId];//fill # of hit in this mc track
  m_na_nEvtNoise = nNoise;//fill # of noise hit in this event
  //m_na_mcTkNum = mcTkNum;// how many mc track are this track include
  m_na_recTkNum = recTkNum;//how many rec track are this track's mc particle include
  CLHEP::Hep3Vector rec_mom = momentum(tk);
  // fill rec momentum
  m_na_p  = rec_mom.mag();
  m_na_pt = rec_mom.perp();
  m_na_pz = rec_mom.z();
  //cout << "MSG::INFO Retrieved McParticles for for RecMdcTrack # " << it->getId()
  //	<< " of reconstructed momentum " << rec_mom << " GeV/c" << endl;
  //five param and it's error matrix
  m_na_d0    = tk->helix(0);
  m_na_phi0  = tk->helix(1);
  m_na_cpa   = tk->helix(2);
  m_na_z0    = tk->helix(3);
  m_na_tanl  = tk->helix(4);

  if( m_na_d0 > m_d0Cut ) {
    std::cout<<__FILE__<<" "<<__LINE__<<" evtNo: "<<t_eventNo<<" d0:"<<std::setw(5)<<m_na_d0<<">"<<m_d0Cut<<  std::endl;
    setFilterPassed(true);
  }
  if( m_na_z0 > m_z0Cut ) {
    std::cout<<__FILE__<<" "<<__LINE__<<" evtNo: "<<t_eventNo<<" z0:"<<std::setw(5)<<m_na_z0<<">"<<m_z0Cut<<  std::endl;
    setFilterPassed(true);
  }
  //const CLHEP::HepSymMatrix tkErrM = tk->err();
  m_na_d0E   = tk->err(0);
  m_na_phi0E = tk->err(2);
  m_na_cpaE  = tk->err(5);
  m_na_z0E   = tk->err(9);
  m_na_tanlE = tk->err(14);
  m_na_q = tk->charge();
  //fill track about
  m_na_chi2   = tk->chi2();
  //m_na_nHit   = tk->getNhits();
  m_na_nDof   = tk->ndof();
  if ( m_na_nDof > 0 ) {
    m_na_chi2Dof = m_na_chi2/(float)m_na_nDof;
  } else {
    m_na_chi2Dof = 200.;
  }
  m_na_chi2Prob = probab(m_na_nDof, m_na_chi2);
  m_na_nSt    = tk->nster();
  m_na_fiTerm = tk->getFiTerm(); 

  if (tk->stat()==0){
    t_trkRecoTk++;
  }else if(tk->stat()==1){
    t_curlTk++;
  }else if(tk->stat()==2){
    t_patRecTk++;
  }else if(tk->stat()==3){
    t_xRecTk++;
  }
  m_na_tkStat = tk->stat();

  ////----fill rec Hit
  //int ihit = 0;
  //int layerEff[43];
  //for (int ii=0;ii<43;ii++){
  //  layerEff[ii]=0;
  //}
  int noiseHit=0;
  int matchHit=0;
  int nAct = 0;
  HitRefVec hl = tk->getVecHits();
  HitRefVec::iterator hitIt = hl.begin();
  for (;hitIt!=hl.end();++hitIt){
    RecMdcHit* h = *hitIt;
    if ( !h ) continue;
    if (h->getStat()!=0) nAct++;
    //  //fill residual
    //  double m_na_lr = h->getFlagLR();
    //  double ddrift = -999;double ddoca = -999;
    //  ddoca = h->getDoca();
    //  if( 0 == m_na_lr ){ ddrift = h->getDriftDistLeft();
    //  }else{ ddrift = h->getDriftDistRight();}
    //  m_na_resid[ihit] = fabs(ddrift) - fabs(ddoca);
    //  if( 0 == m_na_lr ){   m_na_resid[ihit] *= -1.0;}
    //  m_na_driftD[ihit] = ddrift;
    //  m_na_driftT[ihit] = h->getDriftT();
    //  m_na_doca[ihit] = ddoca;
    //  m_na_entra[ihit] = h->getEntra();
    //  m_na_zhit[ihit] = h->getZhit();
    //  m_na_chi2add[ihit] = h->getChisqAdd();
    //  m_na_flaglr[ihit] = h->getFlagLR();
    //  m_na_hitStat[ihit] = h->getStat();
    //  m_na_Tdc[ihit] = h->getTdc();
    //  m_na_Adc[ihit] = h->getAdc();
    //  m_na_act[ihit] = h->getStat();


    int tlayer = MdcID::layer(h->getMdcId());
    int twire = MdcID::wire(h->getMdcId());
    //  m_na_layer[ihit] = tlayer;
    //  m_na_wire[ihit] = twire;
    //  m_na_gwire[ihit] = Constants::nWireBeforeLayer[tlayer] + twire;
    //  if (0==layerEff[tlayer]){
    //    layerEff[tlayer]=1;
    //    g_layerEff->fill(tlayer);
    //  }

    if(havedigi[tlayer][twire]<0){ 
      ++noiseHit;
    }
    //else{
    //if(havedigi[tlayer][twire] == mcTkId) ++matchHit;
    //    m_na_hitTkId[ihit] = havedigi[tlayer][twire]; 
    //}
    //  ++ihit;
  }
  m_na_nAct = nAct;
  m_na_nNoise = noiseHit; 
  m_na_nMatch = matchHit;
  g_tupleMc->write();
  //------------------------------------------
  // fill Rec Track
  //------------------------------------------

  if (m_poca){
    uint32_t getDigiFlag = 0;
    getDigiFlag += m_maxMdcDigi;
    if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
    if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
    if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
    MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
    MdcDigiCol::iterator iter = mdcDigiVec.begin();
    for (;iter != mdcDigiVec.end(); iter++) {
      poca((*iter),tk->helix(),tk->err());
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MdcNavigation::bookNTuple(){
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;
  g_layerEff = histoSvc()->book( "layerEff", "layerEff",43,-0.5,42.5 );

  NTuplePtr nt1(ntupleSvc(), "MdcNavigation/rec");
  if ( nt1 ) { g_tupleMc = nt1;}
  else {
    g_tupleMc = ntupleSvc()->book ("MdcNavigation/rec", CLID_ColumnWiseTuple, "rec and delta with mc truth");
    if ( g_tupleMc ) {
      sc = g_tupleMc->addItem ("tkStat",  m_na_tkStat);
      sc = g_tupleMc->addItem ("p",       m_na_p);
      sc = g_tupleMc->addItem ("pt",      m_na_pt);
      sc = g_tupleMc->addItem ("pz",      m_na_pz);
      sc = g_tupleMc->addItem ("d0",      m_na_d0);
      sc = g_tupleMc->addItem ("phi0",    m_na_phi0);
      sc = g_tupleMc->addItem ("cpa",     m_na_cpa);
      sc = g_tupleMc->addItem ("z0",      m_na_z0);
      sc = g_tupleMc->addItem ("tanl",    m_na_tanl);
      sc = g_tupleMc->addItem ("d0E",     m_na_d0E);
      sc = g_tupleMc->addItem ("phi0E",   m_na_phi0E);
      sc = g_tupleMc->addItem ("cpaE",    m_na_cpaE);
      sc = g_tupleMc->addItem ("z0E",     m_na_z0E);
      sc = g_tupleMc->addItem ("tanlE",   m_na_tanlE);
      sc = g_tupleMc->addItem ("q",       m_na_q);

      sc = g_tupleMc->addItem ("dP",      m_na_dP);
      sc = g_tupleMc->addItem ("dPt",     m_na_dPt);
      sc = g_tupleMc->addItem ("dPz",     m_na_dPz);
      sc = g_tupleMc->addItem ("dD0",     m_na_dD0);
      sc = g_tupleMc->addItem ("dPhi0",   m_na_dPhi0);
      sc = g_tupleMc->addItem ("dCpa",    m_na_dCpa);
      sc = g_tupleMc->addItem ("dZ0",     m_na_dZ0);
      sc = g_tupleMc->addItem ("dTanl",   m_na_dTanl);

      sc = g_tupleMc->addItem ("d0Res",   m_na_d0Res);
      sc = g_tupleMc->addItem ("phiRes",  m_na_phi0Res);
      sc = g_tupleMc->addItem ("z0Res",   m_na_z0Res);
      sc = g_tupleMc->addItem ("tanlRes", m_na_tanlRes);
      sc = g_tupleMc->addItem ("cpaRes",  m_na_cpaRes);

      sc = g_tupleMc->addItem ("recTkNum",m_na_recTkNum);
      sc = g_tupleMc->addItem ("mcTkId",  m_na_mcTkId);
      sc = g_tupleMc->addItem ("mcTkNum", m_na_mcTkNum);
      sc = g_tupleMc->addItem ("evtNo",   m_na_evtNo);
      sc = g_tupleMc->addItem ("nSt",     m_na_nSt);
      sc = g_tupleMc->addItem ("nDof",    m_na_nDof);
      sc = g_tupleMc->addItem ("chi2",    m_na_chi2);
      sc = g_tupleMc->addItem ("chi2Dof", m_na_chi2Dof);
      sc = g_tupleMc->addItem ("chi2Porb",m_na_chi2Prob);
      sc = g_tupleMc->addItem ("fiTerm",  m_na_fiTerm);
      sc = g_tupleMc->addItem ("nMatch",  m_na_nMatch);
      sc = g_tupleMc->addItem ("nAct",    m_na_nAct);
      sc = g_tupleMc->addItem ("nTkNoise",m_na_nNoise);
      sc = g_tupleMc->addItem ("nEvtNoise",m_na_nEvtNoise);
      sc = g_tupleMc->addItem ("nHit",    m_na_nHit, 0, 10000);
      sc = g_tupleMc->addItem ("nDigi",   m_na_nDigi, 0, 10000);
      sc = g_tupleMc->addIndexedItem ("resid",    m_na_nHit, m_na_resid);
      sc = g_tupleMc->addIndexedItem ("driftD",   m_na_nHit, m_na_driftD);
      sc = g_tupleMc->addIndexedItem ("driftT",   m_na_nHit, m_na_driftT);
      sc = g_tupleMc->addIndexedItem ("doca",     m_na_nHit, m_na_doca);
      sc = g_tupleMc->addIndexedItem ("entra",    m_na_nHit, m_na_entra);
      sc = g_tupleMc->addIndexedItem ("zhit",     m_na_nHit, m_na_zhit);
      sc = g_tupleMc->addIndexedItem ("chi2add",  m_na_nHit, m_na_chi2add);
      sc = g_tupleMc->addIndexedItem ("flaglr",   m_na_nHit, m_na_flaglr);
      sc = g_tupleMc->addIndexedItem ("hitStat",  m_na_nHit, m_na_hitStat);
      sc = g_tupleMc->addIndexedItem ("tdc",      m_na_nHit, m_na_Tdc);
      sc = g_tupleMc->addIndexedItem ("adc",      m_na_nHit, m_na_Adc);
      sc = g_tupleMc->addIndexedItem ("act",      m_na_nHit, m_na_act);
      sc = g_tupleMc->addIndexedItem ("layer",    m_na_nHit, m_na_layer);
      sc = g_tupleMc->addIndexedItem ("wire",     m_na_nHit, m_na_wire);
      sc = g_tupleMc->addIndexedItem ("gwire",    m_na_nHit, m_na_gwire);
      sc = g_tupleMc->addIndexedItem ("hitTkId",  m_na_nHit, m_na_hitTkId);
      sc = g_tupleMc->addIndexedItem ("digiTkId", m_na_nDigi, m_na_digiTkId);
      sc = g_tupleMc->addIndexedItem ("mclayer",  m_na_nDigi, m_na_digiLayer);
    } else {   
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(g_tupleMc) << endmsg;
      return StatusCode::FAILURE;
    }
  }//end book of g_tupleMc
  NTuplePtr nt3(ntupleSvc(), "MdcNavigation/evt");
  if ( nt3 ) { g_tupleEvt = nt3;}
  else {
    g_tupleEvt = ntupleSvc()->book ("MdcNavigation/evt", CLID_ColumnWiseTuple, "event");
    if ( g_tupleEvt ) {
      sc = g_tupleEvt->addItem ("nTdsTk",	m_na_t3recTk);
      sc = g_tupleEvt->addItem ("trkReco",	m_na_t3TrkReco);
      sc = g_tupleEvt->addItem ("curlFinder",	m_na_t3Curl);
      sc = g_tupleEvt->addItem ("patRec",	m_na_t3PatRec);
      sc = g_tupleEvt->addItem ("xRec",		m_na_t3XRec);
      sc = g_tupleEvt->addItem ("mcTkNum",	m_na_t3mcTk);
      sc = g_tupleEvt->addItem ("evtNo",	m_na_t3evtNo);
      sc = g_tupleEvt->addItem ("t0",		m_na_t3t0);
      sc = g_tupleEvt->addItem ("t0Truth",	m_na_t3t0Truth);
      sc = g_tupleEvt->addItem ("t0Stat",	m_na_t3t0Stat);
      sc = g_tupleEvt->addItem ("runNo",	m_na_t3runNo);
      sc = g_tupleEvt->addItem ("nDigi",	m_na_t3nDigi, 0, 10000);
      sc = g_tupleEvt->addIndexedItem ("layer", m_na_t3nDigi, m_na_t3layer);
      sc = g_tupleEvt->addIndexedItem ("wire",  m_na_t3nDigi, m_na_t3wire);
      sc = g_tupleEvt->addIndexedItem ("gwire", m_na_t3nDigi, m_na_t3gwire);
      sc = g_tupleEvt->addIndexedItem ("rt",    m_na_t3nDigi, m_na_t3rt);
      sc = g_tupleEvt->addIndexedItem ("rtNot0",m_na_t3nDigi, m_na_t3rtNot0);
      sc = g_tupleEvt->addIndexedItem ("rc",    m_na_t3nDigi, m_na_t3rc);
      sc = g_tupleEvt->addIndexedItem ("phi",   m_na_t3nDigi, m_na_t3phi);
      sc = g_tupleEvt->addIndexedItem ("ovfl",  m_na_t3nDigi, m_na_t3ovfl);
      sc = g_tupleEvt->addIndexedItem ("tNum",  m_na_t3nDigi, m_na_t3tNum);
    }
  }
}

StatusCode MdcNavigation::fillInit(){
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  //int mcTkId = m_na_mcTkId;
  //m_na_nDigi = nDigiTk[mcTkId];//fill # of hit in this mc track
  t_trkRecoTk = 0;
  t_curlTk = 0;
  t_patRecTk = 0;
  t_xRecTk = 0;
  //-------------Get event header
  SmartDataPtr<Event::EventHeader> evtHead(eventSvc(),"/Event/EventHeader");
  if (evtHead) {
    t_runNo = evtHead->runNumber();
    t_eventNo = evtHead->eventNumber();
  }else{
    log << MSG::WARNING<< "Could not retrieve event header" << endreq;
    return StatusCode::FAILURE;
  }
  if(m_debug) std::cout<< "evtNo:"<<t_eventNo << std::endl;

  //------------Get event time
  t_t0 = -1;
  t_t0Stat = -1;
  SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");

  if (!aevtimeCol || aevtimeCol->size()==0) {
    log << MSG::WARNING << "Could not find RecEsTimeCol" << endreq;
  }else{
    RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
    t_t0 = (*iter_evt)->getTest();
    t_t0Stat = (*iter_evt)->getStat();
  } 

  //------------- Get McDigi collection
  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  if ((mdcDigiVec.size()==0)) {
    log << MSG::WARNING << t_eventNo <<"No digi or could not find event in MdcDigiVec" << endreq;
  } 

  for (int ii=0;ii<43;ii++){ 
    for (int jj=0;jj<288;jj++){
      havedigi[ii][jj]= -99;//no hit or noise 
    } 
  }

  for(int imc=0;imc<100;imc++){
    nDigiTk[imc]=0;
    digiLayer[imc]=0;
  }

  for(int ii=0;ii<43;ii++) for(int jj=0;jj<288;jj++) multiTdcCount[ii][jj]=0;
  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  for (;iter != mdcDigiVec.end(); iter++ ) {
    double t = RawDataUtil::MdcTime((*iter)->getTimeChannel());
    double c = (*iter)->getChargeChannel();
    int l = MdcID::layer((*iter)->identify());
    int w = MdcID::wire((*iter)->identify());
    multiTdcCount[l][w]++;
  }

  int t_i=0;
  iter = mdcDigiVec.begin();
  for (;iter != mdcDigiVec.end(); iter++,++t_i ) {
    double t = RawDataUtil::MdcTime((*iter)->getTimeChannel());
    double c = (*iter)->getChargeChannel();
    int l = MdcID::layer((*iter)->identify());
    int w = MdcID::wire((*iter)->identify());
    if(!m_rawData){
      int tkid = (*iter)->getTrackIndex();
      havedigi[l][w]= tkid;
      if (g_tupleMc){
	//m_na_digiTkId[t_i]  = tkid;
      }
      if(tkid>-1){
	++nDigiTk[tkid];
      }else{
	nNoise++;
      }
    }
    //if (g_tupleMc) m_na_digiLayer[t_i] = l;
  }
  return sc; 
}

StatusCode MdcNavigation::skipMcParticle(const McParticle* it, int nKindKeeped, int* pid){

  MsgStream log(msgSvc(), name());

  int pdg_code = (*it).particleProperty();
  if (fabs(pdg_code)>=8) {
    const HepPDT::ParticleData* particles = m_particleTable->particle(abs(pdg_code));
    if( ! particles ){
      log<<MSG::WARNING<<"Exotic particle found with PDG code "<<pdg_code<<endreq;
    }else{
      // skip neutrals
      if( particles->charge() == 0 ){ 
	log<<MSG::INFO<<"Skip charge == 0 mc particle "<<pdg_code<<endreq;
	return StatusCode::FAILURE; 
      }
    }
  }

  int mcTkId = (*it).trackIndex();
  int nMcHit=0;
  SmartDataPtr<Event::MdcMcHitCol> mcMdcMcHitCol(eventSvc(),"/Event/MC/MdcMcHitCol");
  if (!mcMdcMcHitCol) {
    log << MSG::INFO << "Could not find MdcMcHit" << endreq;
  }else{
    Event::MdcMcHitCol::iterator iter_mchit = mcMdcMcHitCol->begin();
    for (;iter_mchit != mcMdcMcHitCol->end(); iter_mchit++ ) {
      int iMcTk =  (*iter_mchit)->getTrackIndex();
      if (mcTkId == iMcTk) nMcHit++;
    }
  }
  if(nMcHit<m_nMcHit) return StatusCode::FAILURE;//5  for default

  bool keeped = false;
  for (int i=0; i<nKindKeeped; i++){
    if (abs(pdg_code) == pid[i]) keeped = true;
  }

  if (!keeped) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}//end of skipMcParticle()

StatusCode MdcNavigation::fillEvent(){
  if (!g_tupleEvt) return StatusCode::FAILURE;
  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);

  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  int t_i =0;
  for (;iter != mdcDigiVec.end(); iter++,++t_i ) {
    double t = RawDataUtil::MdcTime((*iter)->getTimeChannel());
    double c = (*iter)->getChargeChannel();
    int l = MdcID::layer((*iter)->identify());
    int w = MdcID::wire((*iter)->identify());
    if(g_tupleEvt){
      m_na_t3layer[t_i] = l;
      m_na_t3wire[t_i] = w;
      m_na_t3gwire[t_i] = Constants::nWireBeforeLayer[l] + w;
      m_na_t3rt[t_i] = t;
      m_na_t3rtNot0[t_i] = t - t_t0;
      m_na_t3rc[t_i] = c;
      const MdcDigi* digi =const_cast<const MdcDigi*> (*iter);
      m_na_t3ovfl[t_i] = (*iter)->getOverflow();
      m_na_t3tNum[t_i] = ((*iter)->getOverflow()&0xF0)>>4;
    }
  }
  if(g_tupleEvt) m_na_t3nDigi = t_i;

  m_na_t3TrkReco = t_trkRecoTk;
  m_na_t3Curl = t_curlTk;
  m_na_t3PatRec= t_patRecTk;
  m_na_t3XRec= t_xRecTk;

  m_na_t3t0 = t_t0;
  m_na_t3t0Stat = t_t0Stat;

  m_na_t3recTk = t_recTkNum;
  m_na_t3mcTk = t_mcTkNum;

  m_na_t3evtNo = t_eventNo;
  m_na_t3runNo = t_runNo;
  g_tupleEvt->write();

  return StatusCode::SUCCESS;
}

double MdcNavigation::poca(const MdcDigi* aDigi,const HepVector helixPar,const HepSymMatrix errMat){
  int lay   = MdcID::layer(aDigi->identify());
  int wire  = MdcID::wire(aDigi->identify());
  //======from five track parameter to calculate the exact position=====//
  double ALPHA_loc,rho,pt,kappa,phiIn;
  int charge;
  double Bz = m_pIMF->getReferField()*1000.;
  ALPHA_loc = 333.567*Bz; //magnetic field const
  charge    = ( kappa >=0 ) ? 1 : -1 ;
  rho       = ALPHA_loc/kappa ;
  pt        = fabs( 1.0 / kappa);
  HepVector helix(helix);
  helix[0] = -helixPar[0];    //cm
  helix[1] = helixPar[1]+ pi/2;
  helix[2] = -1./helixPar[2];
  helix[3] = helixPar[3];    //cm
  helix[4] = helixPar[4];
  HelixTraj* m_helixTraj;
  MdcSagTraj* m_wireTraj_I;
  MdcSWire*  m_mdcSWire_I;
  TrkPoca* m_trkPoca;
  TrkPoca* m_trkPoca_I;
  TrkPoca* m_trkPoca_O;
  MdcSagTraj* m_wireTraj_O;
  MdcSWire*  m_mdcSWire_O;
  m_helixTraj = new HelixTraj(helix,errMat);
  //---------------wire in and wire out of this layer---------------------
  const MdcLayer* layPtr = m_gm->Layer(lay);
  double fltLenIn = layPtr->rMid(); 
  phiIn = helix[1] + helix[2]*fltLenIn;//phi0 + omega * L
  BesAngle tmp(phiIn - layPtr->phiEPOffset());
  int wlow = (int)floor(layPtr->nWires() * tmp.rad() / twopi );
  int wbig = (int)ceil(layPtr->nWires() * tmp.rad() / twopi );
  if (tmp == 0 ){ wlow = -1; wbig = 1; }
  int wireIn,wireOut;
  wireIn = wbig;
  wireOut = wlow;
  std::cout<<" in MdcNavigation  lay/4  "<<lay/4<<" phi "<<tmp<<" wire "<<wireIn<<" "<<wireOut<<std::endl;
}

StatusCode MdcNavigation::fillRecHits(RecMdcHitCol& hitCol){
  int ihit=0;
  RecMdcHitCol::iterator itHit = hitCol.begin(); 
  for(;itHit != hitCol.end(); itHit++ ) {   
    RecMdcHit* h = *itHit;
    if ( !h ) continue;
    double m_na_lr = h->getFlagLR();
    double ddrift = -999;
    double ddoca = -999;
    ddoca = h->getDoca();
    if( 0 == m_na_lr ){ ddrift = h->getDriftDistLeft();
    }else{ ddrift = h->getDriftDistRight();}
    m_na_resid[ihit] = fabs(ddrift) - fabs(ddoca);
    if( 0 == m_na_lr ){ m_na_resid[ihit] *= -1.0;}
    m_na_driftD[ihit] = ddrift;
    m_na_driftT[ihit] = h->getDriftT();
    m_na_doca[ihit] = ddoca;
    m_na_entra[ihit] = h->getEntra();
    m_na_zhit[ihit] = h->getZhit();
    m_na_chi2add[ihit] = h->getChisqAdd();
    m_na_flaglr[ihit] = h->getFlagLR();
    m_na_hitStat[ihit] = h->getStat();
    m_na_Tdc[ihit] = h->getTdc();
    m_na_Adc[ihit] = h->getAdc();
    m_na_act[ihit] = h->getStat();
    int tlayer = MdcID::layer(h->getMdcId());
    int twire = MdcID::wire(h->getMdcId());
    m_na_layer[ihit] = tlayer;
    m_na_wire[ihit] = twire;
    m_na_gwire[ihit] = Constants::nWireBeforeLayer[tlayer] + twire;
    ++ihit;
  }//end of loop over hits
  m_na_nHit = ihit;
  return StatusCode::SUCCESS;
}

double MdcNavigation::probab(const int& ndof, const double& chisq){

  //constants
  static double srtopi=2.0/sqrt(2.0*M_PI);
  static double upl=100.0;

  double prob=0.0;
  if(ndof<=0) {return prob;}
  if(chisq<0.0) {return prob;}
  if(ndof<=60) {
    //full treatment
    if(chisq>upl) {return prob;}
    double sum=exp(-0.5*chisq);
    double term=sum;

    int m=ndof/2;
    if(2*m==ndof){
      if(m==1){return sum;}
      for(int i=2; i<=m;i++){
	term=0.5*term*chisq/(i-1);
	sum+=term;
      }//(int i=2; i<=m)
      return sum;
      //even

    }else{
      //odd
      double srty=sqrt(chisq);
      double temp=srty/M_SQRT2;
      prob=erfc(temp);
      if(ndof==1) {return prob;}
      if(ndof==3) {return (srtopi*srty*sum+prob);}
      m=m-1;
      for(int i=1; i<=m; i++){
	term=term*chisq/(2*i+1);
	sum+=term;
      }//(int i=1; i<=m; i++)
      return (srtopi*srty*sum+prob);

    }//(2*m==ndof)

  }else{
    //asymtotic Gaussian approx
    double srty=sqrt(chisq)-sqrt(ndof-0.5);
    if(srty<12.0) {prob=0.5*erfc(srty);};

  }//ndof<30

  return prob;
}//endof probab
