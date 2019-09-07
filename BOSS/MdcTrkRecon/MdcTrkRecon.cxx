#include "MdcTrkRecon/MdcTrkRecon.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/INTupleSvc.h"
#include "EventModel/EventHeader.h"
#include "ReconEvent/ReconEvent.h" 
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "RawEvent/RawDataUtil.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitMap.h"
#include "MdcData/MdcHitUse.h"
#include "MdcData/MdcHitOnTrack.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcGeom/MdcDetector.h"
#include "EvTimeEvent/RecEsTime.h"

#include "MdcTrkRecon/MdcSegData.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcSegFinder.h"
#include "MdcTrkRecon/MdcTrackList.h"
#include "MdcTrkRecon/MdcTrackListCsmc.h"
#include "TrkFitter/TrkContextEv.h"
#include "BField/BField.h"
#include "MdcTrkRecon/MdcHistItem.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/GmsList.h"
#include "MdcGeom/Constants.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRep.h"
#include "TrkBase/TrkDifTraj.h"
#include "TrkFitter/TrkHelixFitter.h"

#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h" 
#include "MdcPrintSvc/IMdcPrintSvc.h"
#include "MdcTrkRecon/MdcSegUsage.h"


#ifdef MDCPATREC_TIMETEST
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include "BesTimerSvc/BesTimer.h"
#endif

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
using namespace Event;

//-----------------------------------------

/////////////////////////////////////////////////////////////////////////////

MdcTrkRecon::MdcTrkRecon(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_hitData(0), m_segs(0), m_tracks(0), m_segFinder(0)  
{ 
  //Declare the properties--------------------------------
  declareProperty("FittingMethod",  m_fittingMethod = 2);
  declareProperty("ConfigFile",     m_configFile = "MDCConfig.xml");
  declareProperty("OnlyUnusedHits", m_onlyUnusedHits = 0);
  declareProperty("PoisonHits",     m_poisonHits = 0);
  declareProperty("doLineFit",      m_doLineFit = false);	
  declareProperty("paramFile",      m_paramFile = "params"); 
  declareProperty("pdtFile",        m_pdtFile = "pdt.table");
  declareProperty("allHit",         m_allHit = 1);
  declareProperty("hist",           m_hist= false);
  declareProperty("recForEsTime",   m_recForEsTime= false);
  declareProperty("tryBunch",       m_tryBunch = false);
  declareProperty("d0Cut",          m_d0Cut= -999.);
  declareProperty("z0Cut",          m_z0Cut= -999.);
  declareProperty("dropTrkPt",      m_dropTrkPt = -999.);
  declareProperty("debug",          m_debug= 0); 
  declareProperty("mcHist",         m_mcHist= false); 
  declareProperty("fieldCosmic",    m_fieldCosmic= false); 
  declareProperty("doSag",	    m_doSagFlag= false);
  declareProperty("arbitrateHits",  m_arbitrateHits = true);

  declareProperty("helixHitsSigma", m_helixHitsSigma);
  //for fill hist 
  declareProperty("getDigiFlag",    m_getDigiFlag = 0);
  declareProperty("maxMdcDigi",     m_maxMdcDigi = 0);
  declareProperty("keepBadTdc",     m_keepBadTdc = 0);
  declareProperty("dropHot",        m_dropHot= 0);
  declareProperty("keepUnmatch",    m_keepUnmatch = 0);
  declareProperty("minMdcDigi",	    m_minMdcDigi = 0);
  declareProperty("selEvtNo",	    m_selEvtNo);
  declareProperty("combineTracking",m_combineTracking = false);//yzhang 2010-05-28 
  declareProperty("noInner",m_noInner = false);//yzhang 2016-10-12 

#ifdef MDCPATREC_RESLAYER
  declareProperty("resLayer",m_resLayer= -1);
#endif

}

MdcTrkRecon::~MdcTrkRecon()
{ 
  m_segs.reset(0);
  m_segFinder.reset(0);
  m_hitData.reset(0);
  m_tracks.reset(0); 
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcTrkRecon::beginRun(){
  MsgStream log(msgSvc(), name());  
  log << MSG::INFO << "in beginRun()" << endreq;

  //Detector geometry 
  _gm = MdcDetector::instance(m_doSagFlag);

  if(NULL == _gm) return StatusCode::FAILURE;

  //Initialize segList
  m_segs.reset( new MdcSegList(_gm->nSuper(), m_flags.segPar) );

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcTrkRecon::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  StatusCode sc;

  //Pdt
  Pdt::readMCppTable(m_pdtFile);

  //Flag and Pars
  m_flags.readPar(m_paramFile);
  m_flags.lHist = m_hist;
  m_flags.setDebug(m_debug);
  for(int i=0;i<43;i++) TrkHelixFitter::nSigmaCut[i] = m_helixHitsSigma[i];
  if (!m_doLineFit) MdcTrackListBase::m_d0Cut = m_d0Cut;
  if (!m_doLineFit) MdcTrackListBase::m_z0Cut = m_z0Cut;
  MdcTrackListBase::m_ptCut = m_dropTrkPt;
  if (m_debug>0) {
    std::cout<< "MdcTrkRecon d0Cut "<<m_d0Cut<< " z0Cut "<< 
      m_z0Cut<<" ptCut "<<m_dropTrkPt << std::endl;
  }

  //Initailize magnetic filed 
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << name() << " Unable to open magnetic field service"<<endreq;
  }
  m_bfield = new BField(m_pIMF);
  log << MSG::INFO <<name() << " field z = "<<m_bfield->bFieldNominal()<< endreq;

  //Initialize RawDataProviderSvc
  IRawDataProviderSvc* irawDataProviderSvc;
  sc = service ("RawDataProviderSvc", irawDataProviderSvc);
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*> (irawDataProviderSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << name()<<" Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  } 

  //Initailize MdcPrintSvc
  IMdcPrintSvc* imdcPrintSvc;
  sc = service ("MdcPrintSvc", imdcPrintSvc);
  m_mdcPrintSvc = dynamic_cast<MdcPrintSvc*> (imdcPrintSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcPrintSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  //Initialize hitdata
  m_hitData.reset( new MdcSegData( ignoringUsedHits() ));

  //Initialize segFinder
  if (m_flags.findSegs) {
    m_segFinder.reset( new MdcSegFinder(m_flags.segPar.useAllAmbig) );
  }

  //Initialize trackList
  if ( m_doLineFit ){
    m_tracks.reset( new MdcTrackListCsmc(m_flags.tkParTight) );
  } else {
    m_tracks.reset( new MdcTrackList(m_flags.tkParTight) );
  }
  m_tracks->setNoInner(m_noInner);

  //Book NTuple and Histograms
  if (m_flags.lHist){ 
    StatusCode sc = bookNTuple();
    if (!sc.isSuccess()) { m_flags.lHist = 0;}
  }

  //yzhang for HelixFitter debug
  if(7 == m_flags.debugFlag())TrkHelixFitter::m_debug = true;

  t_iExexute = 0;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcTrkRecon::execute() {
  setFilterPassed(false); 

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  //Initialize
  if(m_hist>0){
    for (int ii=0;ii<43;ii++){ 
      for (int jj=0;jj<288;jj++){ 
	haveDigiTk[ii][jj] = -2; 
	haveDigiPt[ii][jj] = -2; 
	haveDigiTheta[ii][jj] = -999.; 
	haveDigiPhi[ii][jj] = -999.; 
	haveDigiDrift[ii][jj] = -999.; 
	haveDigiAmbig[ii][jj] = -999.; 
	recHitMap[ii][jj] = 0; 
	mcDrift[ii][jj] = -99.; 
	mcX[ii][jj] = -99.; 
	mcY[ii][jj] = -99.; 
	mcZ[ii][jj] = -99.; 
	mcLR[ii][jj] = -99.; 
	hitPoisoned[ii][jj]=-99;
      } 
    }
  }


  TrkContextEv context(m_bfield);
#ifdef MDCPATREC_TIMETEST
  m_timer[1]->start();
  ti_nHit=0;
#endif
  //------------------read event header--------------
  SmartDataPtr<Event::EventHeader> evtHead(eventSvc(),"/Event/EventHeader");
  if (!evtHead) {
    log << MSG::FATAL<< "Could not retrieve event header" << endreq;
    return( StatusCode::FAILURE);
  }
  t_eventNo = evtHead->eventNumber(); 
  if(m_debug!=0)std::cout<<t_iExexute<<" run "<<evtHead->runNumber()<<" evt "<<t_eventNo<<std::endl;
  t_iExexute++;

  if (m_selEvtNo.size() >0){
    std::vector<int>::iterator it = m_selEvtNo.begin();
    for (; it!=m_selEvtNo.end(); it++){
      if((*it)== t_eventNo) setFilterPassed(true);
    }
  }
  //------------------get event start time-----------
  double t0 = 0.;
  t_t0 = -1.;
  t_t0Stat = -1;
  bool iterateT0 = false;
  const int nBunch = 3;//3 bunch/event
  double iBunch = 0 ; //form 0 to 2
  double BeamDeltaTime = 24. / nBunch;// 8ns
  SmartDataPtr<RecEsTimeCol> evTimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!evTimeCol || evTimeCol->size()==0) { 
    log << MSG::WARNING <<" run "<<evtHead->runNumber() <<" evt "<<t_eventNo<< " Could not find RecEsTimeCol" << endreq;
    if(m_fieldCosmic){//yzhang temp for bes3 csmc test 
      return StatusCode::SUCCESS;
    } 
  }
  RecEsTimeCol::iterator iter_evt = evTimeCol->begin();
  // skip RecEsTimeCol no rec data
  if (iter_evt != evTimeCol->end()){
    t_t0Stat = (*iter_evt)->getStat();
    t_t0 = (*iter_evt)->getTest();

    if (m_doLineFit){
      //t0 -= m_t0Const;
      if (abs(t_t0)<0.00001 || (t_t0 < 0) || (t_t0 > 9999.0) ){
	log << MSG::WARNING << "Skip with t0 = "<<t_t0 << endreq;
	return StatusCode::SUCCESS;//for bes3 cosmic test
      }
    }
    t0 =  t_t0*1.e-9;
    if(m_debug>0) std::cout<<name()<<" t0  "<<t0<<"   "<<std::endl;
  }


restart:
  if ( !m_recForEsTime && m_tryBunch) {
    if ( t_t0Stat < 10 ) return StatusCode::SUCCESS;
  }
  if ( m_tryBunch ){
    if ( iBunch > (nBunch - 1) ) return StatusCode::SUCCESS;
    if ( t_t0Stat < 0 ){ iterateT0 = true; }
    if ( iterateT0 ){
      //double EventDeltaTime = 24.;//24ns/event
      if ( (t_t0Stat > -1) && (fabs( iBunch * BeamDeltaTime - t_t0) < 2.) ){
	++iBunch;
	goto restart;
      }else{ t0 = iBunch * BeamDeltaTime *1.e-9;}
      ++iBunch;
    }
  } 

  SmartDataPtr<MdcHitMap> hitMap(eventSvc(),"/Event/Hit/MdcHitMap");
  if (!hitMap) {
    log << MSG::FATAL << "Could not retrieve MDC hit map" << endreq;
    return( StatusCode::FAILURE);
  }
  //---------------------Read an event--------------
  SmartDataPtr<MdcHitCol> hitCol(eventSvc(),"/Event/Hit/MdcHitCol");
  if (!hitCol) {
    log << MSG::FATAL << "Could not retrieve MDC hit list" << endreq;
    return( StatusCode::FAILURE);
  }
  StatusCode sc;

  //---------- register RecMdcTrackCol and RecMdcHitCol to tds---------
  DataObject *aReconEvent;
  eventSvc()->findObject("/Event/Recon",aReconEvent);
  if(aReconEvent==NULL) {
    aReconEvent = new ReconEvent();
    StatusCode sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return( StatusCode::FAILURE);
    } 
  }

  DataObject *aTrackCol;
  DataObject *aRecHitCol;
  //yzhang 2010-05-28 
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
  if(!m_combineTracking){
    eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
    if(aTrackCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
      eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
    }
    eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
    if(aRecHitCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
      eventSvc()->unregisterObject("/Event/Recon/RecMdcHitCol");
    }
  }

  RecMdcTrackCol* trackList;
  eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
  if (aTrackCol) {
    trackList = dynamic_cast<RecMdcTrackCol*> (aTrackCol);
  }else{
    trackList = new RecMdcTrackCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcTrackCol, trackList);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcTrack collection" <<endreq;
      return StatusCode::FAILURE;
    }
  }
  RecMdcHitCol* hitList;
  eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
  if (aRecHitCol) {
    hitList = dynamic_cast<RecMdcHitCol*> (aRecHitCol);
  }else{
    hitList = new RecMdcHitCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcHitCol, hitList);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcHit collection" <<endreq;
      return StatusCode::FAILURE;
    }
  }

  if(m_debug>0) std::cout<<name()<<" t0  "<<t0<<"   "<<std::endl;
  m_hitData->loadevent(hitCol, hitMap, t0);
  t_nDigi = hitCol->size();

  if (poisoningHits()) {
    m_hitData->poisonHits(_gm,m_debug);
  }

  if((m_hist>0) && m_tupleWireEff){
    for(int i=0;i<m_hitData->nhits();i++){
      const MdcHit* thisHit = m_hitData->hit(i);
      int l = thisHit->layernumber();
      int w = thisHit->wirenumber();
      if(m_hitData->segUsage().get(thisHit)->deadHit()){
	hitPoisoned[l][w] = 1;
      }else{
	hitPoisoned[l][w] = 0;
      }
    }
  }

#ifdef MDCPATREC_TIMETEST
  SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
  if(!mcParticleCol){
    log << MSG::INFO << "Could not retrieve McParticelCol" << endreq;
  }
  m_mcTkNum = mcParticleCol->size();
#endif

  
  if(m_debug>1) dumpDigi();
  //--------------------------------------------------------------------------
  // Segment finding
  //--------------------------------------------------------------------------
  int nSegs = 0;
  if (m_flags.findSegs) {
    // Form track segments in superlayers
    nSegs = m_segFinder->createSegs(_gm, *m_segs, m_hitData->segUsage(), 
	m_hitData->hitMap(), t0);
  }
  log << MSG::INFO << " Found " << nSegs << " segments" << endreq;
  if (m_debug>1){
    std::cout<<"------------------------------------------------"<<std::endl;
    std::cout<<"==event "<<t_eventNo<< " Found " << nSegs << " segments" << std::endl;
    m_segs->plot();
  }

  if (m_flags.lHist||m_flags.segPar.lPrint) {
    fillSegList();
  } 

  //--------------------------------------------------------------------------
  // Combine segments to form track
  //-------------------------------------------------------------------------- 
  int nTracks  = 0;
  int nDeleted = 0;
  if (m_flags.findTracks && m_flags.findSegs) {
    if (nSegs > 2) {
      nTracks = m_tracks->createFromSegs(m_segs.get(), m_hitData->hitMap(),
	  _gm, context, t0); 
    }

    if(m_arbitrateHits) nDeleted = m_tracks->arbitrateHits();
    int nKeep = nTracks - nDeleted;

    if (m_debug>0 && (nTracks - nDeleted)>0){
      std::cout<< "MdcTrkRecon: evt "<< setw(5)<<t_eventNo
	<<" nDigi "<<setw(4)<<t_nDigi<< " t0 "<<setw(5)<<t_t0
	<< " keep  "<< nKeep <<" track(s)"; 
      if (nDeleted!=0){ std::cout <<",deleted " <<nDeleted; }
      std::cout<< std::endl;//yzhang debug
    }else{
      if (m_debug>0){
	std::cout<< "MdcTrkRecon: evt "<< setw(5)<<t_eventNo
	  <<" nDigi "<<setw(4)<<t_nDigi<< " t0 "<<setw(5)<<t_t0
	  <<" found 0 track "<< endl;
      }
    }

    if (m_flags.lHist) t_nRecTk = nKeep;

#ifdef MDCPATREC_RESLAYER
    m_tracks->setResLayer(m_resLayer);
#endif 
    if (m_flags.lHist){ fillTrackList(); }
    // Stick the found tracks into the list in TDS
    m_tracks->store(trackList, hitList);
    //if (m_debug>1) { dumpTdsTrack(trackList); }
    if(m_debug>1){
      std::cout<<name()<<" print track list  "<<std::endl;
      m_mdcPrintSvc->printRecMdcTrackCol();
    }

    //if(nKeep != (int)trackList->size()) std::cout<<__FILE__<<" "<<__LINE__<<"!!!!!!!!!!!!!!!!! nKeep != nTdsTk"<<  std::endl;
#ifdef MDCPATREC_TIMETEST
    RecMdcTrackCol::iterator iter = trackList->begin();
    for (;iter != trackList->end(); iter++) {
      MdcTrack* tk = *iter;
      ti_nHit += tk->getNhits(); 
    }
#endif

  } 
  //-------------End track-finding------------------ 
  m_segs->destroySegs();

  //if ( t_eventNo% 1000 == 0) { 
  //std::cout<<"evt "<<t_eventNo<< " Found " << trackList->size() << " track(s)" <<endl;//yzhang debug
  //}
  if(m_flags.lHist && m_mcHist) fillMcTruth();
#ifdef MDCPATREC_TIMETEST
  m_timer[1]->stop(); 
  //cout << "m_timer[1]->elapsed()::" << m_timer[1]->elapsed() << endl;
  //cout << "m_timer[1]->mean()::" << m_timer[1]->mean() << endl;
  m_eventTime = m_timer[1]->elapsed();
  m_t5recTkNum = m_tracks->length();
  m_t5EvtNo = t_eventNo;
  m_t5nHit = ti_nHit;
  m_t5nDigi = t_nDigi;
  sc = m_tupleTime->write();
#endif
  // for event start time, if track not found try another t0
  if ( m_tryBunch ){
    if ( nTracks <1 ){ iterateT0 = true; 
      goto restart;
    }
  }
  if (m_flags.lHist ) fillEvent();

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcTrkRecon::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  m_tracks.reset(0);
#ifdef MDCPATREC_TIMETEST
  m_timersvc->print();
#endif
  return StatusCode::SUCCESS;
}

StatusCode MdcTrkRecon::bookNTuple(){
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;


  //--------------book histogram------------------  
  h_sfHit  = histoSvc()->book( "sfHit", "Hits after segment finding",46,-1.,44. );
  //g_actHit = histoSvc()->book( "actHit", "Active hits",46,-1.,44. );
  //g_hitEff = histoSvc()->book( "hitEff", "Hit efficiency in the event",100,-0.5,1.2 );
  g_residVsLayer = histoSvc()->book( "residVsLayer", "Residual vs. layer",60,-5,50.,100,-0.5,1.2 );
  g_residVsDoca  = histoSvc()->book( "residVsDoca", "Residial vs. doca",50,-2.,2.,100,-0.5,1.2 );
  //segment
  //g_segChi2  = histoSvc()->book( "segChi2", "chi2 per dof in segment finding",101,-1.,100. );
  g_cirTkChi2  = histoSvc()->book( "cirTkChi2", "chi2 per dof in circle finding",21,-1. ,20. );
  g_3dTkChi2 = histoSvc()->book( "chi2Helix", "maxChisq dof helix fit" ,51.,-1.,50);
  g_nSigAdd = histoSvc()->book( "nSigAdd", "max allowed n sigma to add hit to existing segment",50,0,50 );
  g_z0Cut = histoSvc()->book( "z0Cut", "z0 for including stereo segs in cluster",100,0,600 );
  g_ctCut = histoSvc()->book( "ctCut", "ct for including stereo segs in cluster",100,-50,50 );
  g_delCt = histoSvc()->book( "delCt", "del ct cut forming seg group",100,-20,20 );
  g_delZ0 = histoSvc()->book( "delZ0", "del z0 cut forming seg group",100,-60,60 );
  g_phiDiff = histoSvc()->book( "phiDiff", "phidiff mult for drop dup seg",100,-0.5,6.5 );
  g_slopeDiff = histoSvc()->book( "slopeDiff", "slopediff for drop dup seg",100,-0.3,0.3 );
  g_maxSegChisqAx = histoSvc()->book( "maxSegChisqAx", "max chisqDof ax seg combine" ,1000,0.,1000.);
  g_maxSegChisqSt = histoSvc()->book( "maxSegChisqSt", "max chisqDof st seg combine" ,200,-10.,200.);
  g_pickHitWire= histoSvc()->book( "pickHitWire", "nWire of pickHit" ,600,-288,288 );

  //for(int i=0;i<11;i++){
  //char title1[80],title2[80];
  //sprintf(title1,"segChi2Pat3%d",i);
  //sprintf(title2,"segChi2Pat4%d",i);
  //g_segChi2SlayPat3[i]  = histoSvc()->book( title1, "chi2/dof per layer of pat3",710,-10.,700. );
  //g_segChi2SlayPat4[i]  = histoSvc()->book( title2, "chi2/dof per layer of pat4",710,-10.,700. );
  //}

  //book tuple of wire efficiency
  NTuplePtr ntWireEff(ntupleSvc(), "MdcWire/mc");
  if ( ntWireEff ) { m_tupleWireEff = ntWireEff;}
  else {
    m_tupleWireEff = ntupleSvc()->book ("MdcWire/mc", CLID_ColumnWiseTuple, "MdcWire");
    if ( m_tupleWireEff ) {
      sc = m_tupleWireEff->addItem ("tkId",	 m_we_tkId);
      sc = m_tupleWireEff->addItem ("pdg",	 m_we_pdg);
      sc = m_tupleWireEff->addItem ("primary",	 m_we_primary);
      sc = m_tupleWireEff->addItem ("layer",     m_we_layer);
      sc = m_tupleWireEff->addItem ("wire",      m_we_wire);
      sc = m_tupleWireEff->addItem ("gwire",     m_we_gwire);
      sc = m_tupleWireEff->addItem ("poisoned",  m_we_poisoned);
      sc = m_tupleWireEff->addItem ("seg",       m_we_seg);
      sc = m_tupleWireEff->addItem ("track",     m_we_track);
      sc = m_tupleWireEff->addItem ("pt",	 m_we_pt);
      sc = m_tupleWireEff->addItem ("theta",	 m_we_theta);
      sc = m_tupleWireEff->addItem ("phi",	 m_we_phi);
      //sc = m_tupleWireEff->addItem ("tdc",       m_we_tdc);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcWire/mc" << endmsg;
    }
  }

  //book tuple of test
  NTuplePtr ntt(ntupleSvc(), "MdcRec/test");
  if ( ntt ) { m_tuplet = ntt;}
  else {
    m_tuplet = ntupleSvc()->book ("MdcRec/test", CLID_ColumnWiseTuple, "MdcTrkRecon t particle");
    if ( m_tuplet ) {
      sc = m_tuplet->addItem ("layer",	 m_tl);
      sc = m_tuplet->addItem ("wire",	 m_tw);
      sc = m_tuplet->addItem ("phi",	 m_tphi);
      sc = m_tuplet->addItem ("dphi",	 m_tdphi);
      sc = m_tuplet->addItem ("dncell",	 m_tdncell);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcRec/test" << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of MC truth 
  NTuplePtr ntMc(ntupleSvc(), "MdcMc/mcTk");
  if ( ntMc ) { m_tupleMc = ntMc;}
  else {
    m_tupleMc = ntupleSvc()->book ("MdcMc/mcTk", CLID_ColumnWiseTuple, "MdcTrkRecon Mc particle");
    if ( m_tupleMc ) {
      sc = m_tupleMc->addItem ("evtNo",	 m_tMcEvtNo);
      sc = m_tupleMc->addItem ("nDigi",	 m_tMcNTk);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcMc/mcTk" << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of MC truth 
  NTuplePtr ntMcHit(ntupleSvc(), "MdcMc/mcHit");
  if ( ntMcHit ) { m_tupleMcHit = ntMcHit;}
  else {
    m_tupleMcHit = ntupleSvc()->book ("MdcMc/mcHit", CLID_ColumnWiseTuple, "MdcTrkRecon Mc hit");
    if ( m_tupleMcHit ) {
      sc = m_tupleMcHit->addItem ("tkId", m_tMcTkId);
      sc = m_tupleMcHit->addItem ("pid",  m_tMcPid);
      sc = m_tupleMcHit->addItem ("px",   m_tMcPx);
      sc = m_tupleMcHit->addItem ("py",   m_tMcPy);
      sc = m_tupleMcHit->addItem ("pz",   m_tMcPz);
      sc = m_tupleMcHit->addItem ("d0",   m_tMcD0);
      sc = m_tupleMcHit->addItem ("z0",   m_tMcZ0);
      sc = m_tupleMcHit->addItem ("theta",m_tMcTheta);
      sc = m_tupleMcHit->addItem ("fiterm",m_tMcFiTerm);
      sc = m_tupleMcHit->addItem ("nMcHit",	 m_tMcNHit, 0, 6796);
      sc = m_tupleMcHit->addIndexedItem ("layer",m_tMcNHit, m_tMcLayer);
      sc = m_tupleMcHit->addIndexedItem ("wire", m_tMcNHit, m_tMcWire);
      sc = m_tupleMcHit->addIndexedItem ("drift",m_tMcNHit, m_tMcDrift);
      sc = m_tupleMcHit->addIndexedItem ("x",    m_tMcNHit, m_tMcX);
      sc = m_tupleMcHit->addIndexedItem ("y",    m_tMcNHit, m_tMcY);
      sc = m_tupleMcHit->addIndexedItem ("z",    m_tMcNHit, m_tMcZ);
      sc = m_tupleMcHit->addIndexedItem ("lr",    m_tMcNHit, m_tMcLR);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcMc/mcHit" << endmsg;
      //return StatusCode::FAILURE;
    }
  }
  //book tuple of recnstruction results
  NTuplePtr nt1(ntupleSvc(), "MdcRec/rec");
  if ( nt1 ) { m_tuple1 = nt1;}
  else {
    m_tuple1 = ntupleSvc()->book ("MdcRec/rec", CLID_ColumnWiseTuple, "MdcTrkRecon results");
    if ( m_tuple1 ) {
      sc = m_tuple1->addItem ("t0",      m_t0);
      sc = m_tuple1->addItem ("t0Stat",  m_t0Stat);
      sc = m_tuple1->addItem ("t0Truth", m_t0Truth);

      sc = m_tuple1->addItem ("nTdsTk",  m_nTdsTk);
      sc = m_tuple1->addItem ("nMcTk",   m_nMcTk);

      sc = m_tuple1->addItem ("p",       m_p);
      sc = m_tuple1->addItem ("pt",      m_pt);
      sc = m_tuple1->addItem ("nSlay",   m_nSlay);
      sc = m_tuple1->addItem ("pz",      m_pz);
      sc = m_tuple1->addItem ("d0",      m_d0);
      sc = m_tuple1->addItem ("phi0",    m_phi0);
      sc = m_tuple1->addItem ("cpa",     m_cpa);
      sc = m_tuple1->addItem ("z0",      m_z0);
      sc = m_tuple1->addItem ("tanl",    m_tanl);
      sc = m_tuple1->addItem ("q",       m_q);
      sc = m_tuple1->addItem ("pocax",   m_pocax);
      sc = m_tuple1->addItem ("pocay",   m_pocay);
      sc = m_tuple1->addItem ("pocaz",   m_pocaz);

      sc = m_tuple1->addItem ("evtNo",   m_evtNo);
      sc = m_tuple1->addItem ("nSt",     m_nSt);
      sc = m_tuple1->addItem ("nAct",    m_nAct);
      sc = m_tuple1->addItem ("nDof",    m_nDof);
      sc = m_tuple1->addItem ("chi2",    m_chi2);
      sc = m_tuple1->addItem ("tkId",    m_tkId);
      sc = m_tuple1->addItem ("nHit",    m_nHit, 0, 6796);
      sc = m_tuple1->addIndexedItem ("resid",    m_nHit, m_resid);
      sc = m_tuple1->addIndexedItem ("sigma",    m_nHit, m_sigma);
      sc = m_tuple1->addIndexedItem ("driftD",   m_nHit, m_driftD);
      sc = m_tuple1->addIndexedItem ("driftT",   m_nHit, m_driftT);
      sc = m_tuple1->addIndexedItem ("doca",     m_nHit, m_doca);
      sc = m_tuple1->addIndexedItem ("entra",    m_nHit, m_entra);
      sc = m_tuple1->addIndexedItem ("ambig",    m_nHit, m_ambig);
      sc = m_tuple1->addIndexedItem ("fltLen",   m_nHit, m_fltLen);
      sc = m_tuple1->addIndexedItem ("tof",      m_nHit, m_tof);
      sc = m_tuple1->addIndexedItem ("act",      m_nHit, m_act);
      sc = m_tuple1->addIndexedItem ("tdc",      m_nHit, m_tdc);
      sc = m_tuple1->addIndexedItem ("adc",      m_nHit, m_adc);
      sc = m_tuple1->addIndexedItem ("layer",    m_nHit, m_layer);
      sc = m_tuple1->addIndexedItem ("wire",     m_nHit, m_wire);
      sc = m_tuple1->addIndexedItem ("x",        m_nHit, m_x);
      sc = m_tuple1->addIndexedItem ("y",        m_nHit, m_y);
      sc = m_tuple1->addIndexedItem ("z",        m_nHit, m_z);
      sc = m_tuple1->addIndexedItem ("dx",       m_nHit, m_dx);
      sc = m_tuple1->addIndexedItem ("dy",       m_nHit, m_dy);
      sc = m_tuple1->addIndexedItem ("dz",       m_nHit, m_dz);
      sc = m_tuple1->addIndexedItem ("dDriftD",  m_nHit, m_dDriftD);
      sc = m_tuple1->addIndexedItem ("dlr",	 m_nHit, m_dlr);
      sc = m_tuple1->addIndexedItem ("cellWidth",m_nHit, m_cellWidth);//for pickHits tuning
    } else {
      log << MSG::ERROR << "Cannot book tuple MdcRec/rec" << endmsg;
      //return StatusCode::FAILURE;
    }
  }
  //book tuple of segment
  NTuplePtr ntSeg(ntupleSvc(), "MdcSeg/seg");
  if ( ntSeg ) { m_tupleSeg = ntSeg;}
  else {
    m_tupleSeg = ntupleSvc()->book ("MdcSeg/seg", CLID_ColumnWiseTuple, "MdcTrkRecon segment data");
    if ( m_tupleSeg ) {
      sc = m_tupleSeg->addItem ("evtNo",	 m_tsEvtNo);
      sc = m_tupleSeg->addItem ("nSeg",	 	 m_tsNSeg);
      sc = m_tupleSeg->addItem ("nDigi",	 m_tsNDigi, 0, 6796);
      sc = m_tupleSeg->addIndexedItem ("layer",  m_tsNDigi, m_tsLayer);
      sc = m_tupleSeg->addIndexedItem ("wire",   m_tsNDigi, m_tsWire);
      sc = m_tupleSeg->addIndexedItem ("inSeg",  m_tsNDigi, m_tsInSeg);
      sc = m_tupleSeg->addIndexedItem ("mcTkId", m_tsNDigi, m_tsMcTkId);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcSeg/seg" << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of event
  NTuplePtr nt4(ntupleSvc(), "MdcRec/evt");
  if ( nt4 ) { m_tupleEvt = nt4;}
  else {
    m_tupleEvt = ntupleSvc()->book ("MdcRec/evt", CLID_ColumnWiseTuple, "MdcTrkRecon event data");
    if ( m_tupleEvt ) {
      sc = m_tupleEvt->addItem ("evtNo",	 m_t4EvtNo);
      sc = m_tupleEvt->addItem ("nMcTk",	 m_t4nMcTk );
      sc = m_tupleEvt->addItem ("nTdsTk",	 m_t4nRecTk );
      sc = m_tupleEvt->addItem ("t0",		 m_t4t0);
      sc = m_tupleEvt->addItem ("t0Stat",        m_t4t0Stat);
      sc = m_tupleEvt->addItem ("t0Truth",       m_t4t0Truth);
      sc = m_tupleEvt->addItem ("nDigiUn",       m_t4nDigiUnmatch);
      sc = m_tupleEvt->addItem ("nDigi",	 m_t4nDigi, 0, 6796);
      sc = m_tupleEvt->addIndexedItem ("layer",  m_t4nDigi, m_t4Layer);
      sc = m_tupleEvt->addIndexedItem ("wire",   m_t4nDigi, m_t4Wire);
      sc = m_tupleEvt->addIndexedItem ("rt",     m_t4nDigi, m_t4Time);
      sc = m_tupleEvt->addIndexedItem ("rc",     m_t4nDigi, m_t4Charge);
      sc = m_tupleEvt->addIndexedItem ("phiMid", m_t4nDigi, m_t4PhiMid);
      sc = m_tupleEvt->addIndexedItem ("hot",    m_t4nDigi, m_t4Hot);
      //sc = m_tupleEvt->addIndexedItem ("recHit", m_t4nDigi, m_t4recHit);
      //sc = m_tupleEvt->addIndexedItem ("rawHit", m_t4nDigi, m_t4rawHit);
    } else {
      log << MSG::ERROR << "Cannot book N-tuple: MdcRec/evt"   << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of residula for every layer
  NTuplePtr ntCombAx(ntupleSvc(), "MdcCombAx/combAx");
  if ( ntCombAx ) { g_tupleCombAx= ntCombAx;}
  else { 
    g_tupleCombAx = ntupleSvc()->book ("MdcCombAx/combAx", CLID_RowWiseTuple, "MdcTrkRecon cuts in combine ax seg");
    if ( g_tupleCombAx) {
      sc = g_tupleCombAx->addItem ("dPhi0",  	g_combAxdPhi0);
      sc = g_tupleCombAx->addItem ("dCurv",  	g_combAxdCurv);
      sc = g_tupleCombAx->addItem ("sigPhi0",	g_combAxSigPhi0);
      sc = g_tupleCombAx->addItem ("sigCurv",	g_combAxSigCurv);
      sc = g_tupleCombAx->addItem ("slSeed", 	g_combAxSlSeed);
      sc = g_tupleCombAx->addItem ("slTest", 	g_combAxSlTest);
      sc = g_tupleCombAx->addItem ("qSeed",  	g_combAxQualitySeed);
      sc = g_tupleCombAx->addItem ("qTest",  	g_combAxQualityTest);
      sc = g_tupleCombAx->addItem ("pSeed",  	g_combAxPatSeed);
      sc = g_tupleCombAx->addItem ("pTest",  	g_combAxPatTest);
      sc = g_tupleCombAx->addItem ("nHitSeed",	g_combAxNhitSeed);
      sc = g_tupleCombAx->addItem ("nHitTest",	g_combAxNhitTest);
      sc = g_tupleCombAx->addItem ("mc",     	g_combAxMc);
      sc = g_tupleCombAx->addItem ("ptTruth",	g_combAxMcPt);
      sc = g_tupleCombAx->addItem ("thetaTruth",g_combAxMcTheta);
      sc = g_tupleCombAx->addItem ("phiTruth",	g_combAxMcPhi);
      sc = g_tupleCombAx->addItem ("ambigSeed",	g_combAxMcAmbigSeed);
      sc = g_tupleCombAx->addItem ("ambigTest",	g_combAxMcAmbigTest);
    } else {   
      log << MSG::ERROR << "Cannot book N-tuple: FILE101/combAx"   << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of residula for every layer
  NTuplePtr ntFindSeg(ntupleSvc(), "MdcSeg/findSeg");
  if ( ntFindSeg ) { g_tupleFindSeg = ntFindSeg;}
  else { 
    g_tupleFindSeg = ntupleSvc()->book ("MdcSeg/findSeg", CLID_RowWiseTuple, "MdcTrkRecon cuts in segment finder");
    if ( g_tupleFindSeg) {
      sc = g_tupleFindSeg->addItem ("chi2",  	g_findSegChi2);
      sc = g_tupleFindSeg->addItem ("slope",  	g_findSegSlope);
      sc = g_tupleFindSeg->addItem ("intercept",g_findSegIntercept);
      sc = g_tupleFindSeg->addItem ("chi2Refit",g_findSegChi2Refit);
      sc = g_tupleFindSeg->addItem ("chi2Add",  g_findSegChi2Add);
      sc = g_tupleFindSeg->addItem ("pat",  	g_findSegPat);
      sc = g_tupleFindSeg->addItem ("pat34",  	g_findSegPat34);
      sc = g_tupleFindSeg->addItem ("nhit",  	g_findSegNhit);
      sc = g_tupleFindSeg->addItem ("slayer",  	g_findSegSl);
      sc = g_tupleFindSeg->addItem ("mc",  	g_findSegMc);
      sc = g_tupleFindSeg->addItem ("ambig",  	g_findSegAmbig);
    } else {   
      log << MSG::ERROR << "Cannot book N-tuple: FILE101/findSeg"   << endmsg;
      //return StatusCode::FAILURE;
    }
  }

  //book tuple of event
  NTuplePtr ntPick(ntupleSvc(), "MdcTrk/pick");
  if ( ntPick ) { m_tuplePick = ntPick;}
  else {
    m_tuplePick = ntupleSvc()->book ("MdcTrk/pick", CLID_ColumnWiseTuple, "MdcTrkRecon pickHits");
    if ( m_tuplePick ) {
      sc = m_tuplePick->addItem ("layer",  	        m_pickLayer);
      sc = m_tuplePick->addItem ("nCell",	 	m_pickNCell, 0, 288);
      sc = m_tuplePick->addItem ("nCellWithDigi",	m_pickNCellWithDigi, 0, 288);
      sc = m_tuplePick->addIndexedItem ("wire",    m_pickNCellWithDigi, m_pickWire);
      sc = m_tuplePick->addIndexedItem ("predDrift",    m_pickNCellWithDigi, m_pickPredDrift);
      sc = m_tuplePick->addIndexedItem ("drift",        m_pickNCellWithDigi, m_pickDrift);
      sc = m_tuplePick->addIndexedItem ("driftTruth",   m_pickNCellWithDigi, m_pickDriftTruth);
      sc = m_tuplePick->addIndexedItem ("phiZOk",       m_pickNCellWithDigi, m_pickPhizOk);
      sc = m_tuplePick->addIndexedItem ("z",            m_pickNCellWithDigi, m_pickZ);
      sc = m_tuplePick->addIndexedItem ("resid",        m_pickNCellWithDigi, m_pickResid);
      sc = m_tuplePick->addIndexedItem ("sigma",        m_pickNCellWithDigi, m_pickSigma);
      sc = m_tuplePick->addIndexedItem ("phiLowCut",    m_pickNCellWithDigi, m_pickPhiLowCut);
      sc = m_tuplePick->addIndexedItem ("phiHighCut",   m_pickNCellWithDigi, m_pickPhiHighCut);
      sc = m_tuplePick->addIndexedItem ("goodDriftCut", m_pickNCellWithDigi, m_pickDriftCut);
      sc = m_tuplePick->addIndexedItem ("mcTk",         m_pickNCellWithDigi, m_pickMcTk);
      sc = m_tuplePick->addIndexedItem ("is2d",         m_pickNCellWithDigi, m_pickIs2D);
      sc = m_tuplePick->addIndexedItem ("pt",           m_pickNCellWithDigi, m_pickPt);
      sc = m_tuplePick->addIndexedItem ("curv",           m_pickNCellWithDigi, m_pickCurv);
    } else {   
      log << MSG::ERROR << "Cannot book N-tuple: MdcTrk/pick"   << endmsg;
      //return StatusCode::FAILURE;
    }
  }

#ifdef MDCPATREC_TIMETEST
  //book tuple of time test
  NTuplePtr nt5(ntupleSvc(), "MdcTime/ti");
  if ( nt5 ) { m_tupleTime = nt5;}
  else {
    m_tupleTime = ntupleSvc()->book ("MdcTime/ti", CLID_RowWiseTuple, "MdcTrkRecon time");
    if ( m_tupleTime ) {
      sc = m_tupleTime->addItem ("eventtime",  m_eventTime);
      sc = m_tupleTime->addItem ("recTkNum",   m_t5recTkNum);
      sc = m_tupleTime->addItem ("mcTkNum",    m_mcTkNum);
      sc = m_tupleTime->addItem ("evtNo",      m_t5EvtNo);
      sc = m_tupleTime->addItem ("nHit",       m_t5nHit);
      sc = m_tupleTime->addItem ("nDigi",      m_t5nDigi);
    } else {
      log << MSG::ERROR << "Cannot book N-tuple: FILE101/time"   << endmsg;
      //return StatusCode::FAILURE;
    }
  }
  sc = service( "BesTimerSvc", m_timersvc);
  if( sc.isFailure() ) {
    log << MSG::WARNING << " Unable to locate BesTimerSvc" << endreq;
    //return StatusCode::FAILURE;
  } 
  m_timer[1] = m_timersvc->addItem("Execution");
  m_timer[1]->propName("nExecution");
#endif

#ifdef MDCPATREC_RESLAYER
  //book tuple of residula for every layer
  NTuplePtr nt6(ntupleSvc(), "MdcRes/res");
  if ( nt6 ) { g_tupleres = nt6;}
  else {
    g_tupleres= ntupleSvc()->book ("MdcRes/res", CLID_RowWiseTuple, "MdcTrkRecon res");
    if ( g_tupleres ) {
      sc = g_tupleres->addItem ("resid",  g_resLayer);
      sc = g_tupleres->addItem ("layer",  g_t6Layer); 
    } else {   
      log << MSG::ERROR << "Cannot book N-tuple: FILE101/res"   << endmsg;
      return StatusCode::FAILURE;
    }
  }
#endif

  return sc;
}// end of bookNTuple()

void MdcTrkRecon::fillMcTruth(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  t_mcTkNum = 0;
  for(int i=0;i<100;i++){
    isPrimaryOfMcTk[i]=-9999;
    pdgOfMcTk[i]=-9999;
  }
  double ptOfMcTk[100]={0.};
  double thetaOfMcTk[100]={0.};
  double phiOfMcTk[100]={0.};
  double nDigiOfMcTk[100]={0.};
  if (m_mcHist){
    //------------------Retrieve MC truth MdcMcHit------------
    SmartDataPtr<Event::MdcMcHitCol> mcMdcMcHitCol(eventSvc(),"/Event/MC/MdcMcHitCol"); 
    if (!mcMdcMcHitCol) {
      log << MSG::INFO << "Could not find MdcMcHit" << endreq; 
    }else{
      Event::MdcMcHitCol::iterator iter_mchit = mcMdcMcHitCol->begin();
      for (;iter_mchit != mcMdcMcHitCol->end(); iter_mchit++ ) {
	const Identifier id= (*iter_mchit)->identify();
	int layer = MdcID::layer(id);
	int wire = MdcID::wire(id);
	int iMcTk =  (*iter_mchit)->getTrackIndex();
	if(iMcTk<100&&iMcTk>0) nDigiOfMcTk[iMcTk]++;
	mcDrift[layer][wire]= (*iter_mchit)->getDriftDistance()/10.;  //drift in MC.
	//std::cout<<"   "<<__FILE__<<" mcDrift  "<<mcDrift[layer][wire]<<"   "<<std::endl;
	mcX[layer][wire] = (*iter_mchit)->getPositionX()/10.;
	mcY[layer][wire] = (*iter_mchit)->getPositionY()/10.;
	mcZ[layer][wire] = (*iter_mchit)->getPositionZ()/10.;
	mcLR[layer][wire] = (*iter_mchit)->getPositionFlag();
	if (mcLR[layer][wire] == 0) mcLR[layer][wire] = -1;
	t_nHitInTk[iMcTk]++; 
	haveDigiAmbig[layer][wire] =  mcLR[layer][wire];
	haveDigiDrift[layer][wire] = mcDrift[layer][wire];
      }
    }

    //------------------get event start time truth-----------
    t_t0Truth = -10.;
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if(!mcParticleCol){
      log << MSG::INFO << "Could not retrieve McParticelCol" << endreq;
    }else {
      int nMcTk = 0;
      Event::McParticleCol::iterator it = mcParticleCol->begin();
      for (;it != mcParticleCol->end(); it++){ 
	//int pdg_code = (*it)->particleProperty();
	//if ((fabs(pdg_code)!=211) || (fabs(pdg_code)!=13)) continue;
	t_mcTkNum++;
	nMcTk++;
      }
      it = mcParticleCol->begin();
      for (;it != mcParticleCol->end(); it++){
	int tkId   = (*it)->trackIndex();
	if ((*it)->primaryParticle()){
	  t_t0Truth = (*it)->initialPosition().t();
	  isPrimaryOfMcTk[tkId] = 1;
	}else{
	  isPrimaryOfMcTk[tkId] = 0;
	  //continue;
	}
	//fill charged particle
	int pdg_code = (*it)->particleProperty();
	pdgOfMcTk[tkId] = pdg_code;
	//std::cout<<" tkId  "<<tkId<<" pdg_code  "<<pdg_code<<"   "<<std::endl;
	//FIXME skip charged track and track outside MDC
	//if ((fabs(pdg_code)!=211) || (fabs(pdg_code)!=13)) continue;
	const CLHEP::Hep3Vector& true_mom = (*it)->initialFourMomentum().vect();
	const CLHEP::HepLorentzVector& posIni = (*it)->initialPosition();
	const CLHEP::HepLorentzVector& posFin = (*it)->finalPosition();
	if(tkId<100&&tkId>=0) {
	  ptOfMcTk[tkId] = true_mom.perp();
	  thetaOfMcTk[tkId] = (*it)->initialFourMomentum().theta();
	  phiOfMcTk[tkId] = (*it)->initialFourMomentum().phi();
	}

	if ( m_tupleMcHit){
	  m_tMcPx    = true_mom.x();
	  m_tMcPy    = true_mom.y();
	  m_tMcPz    = true_mom.z();
	  m_tMcD0    = posIni.perp()/10.;
	  m_tMcZ0    = posIni.z()/10.;
	  m_tMcTheta = (*it)->initialFourMomentum().theta();
	  m_tMcFiTerm= posFin.phi();
	  m_tMcPid   = pdg_code;
	  m_tMcTkId  = tkId;
	  m_tMcNHit  = t_nHitInTk[tkId];
	  m_tupleMcHit->write();
	}
      }//end of loop mcParticleCol
      if(m_tupleMc) {
	m_tMcNTk= nMcTk;
	m_tMcEvtNo = t_eventNo;
	m_tupleMc->write();
      }
    }
  }//end of m_mcHist

  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  if ((int)mdcDigiVec.size()<m_minMdcDigi){
    std::cout << " Skip this event for MdcDigiVec.size() < "<<m_minMdcDigi << endl;
  }

  if (mdcDigiVec.size()<=0) return;
  //fill raw digi and t0 status
  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  for (;iter!= mdcDigiVec.end(); iter++ ) {
    long l = MdcID::layer((*iter)->identify());
    long w = MdcID::wire((*iter)->identify());
    int tkId = (*iter)->getTrackIndex();
    haveDigiTk[l][w]= tkId;
    //std::cout<<" l  "<<l<<" w  "<<w<<"  tk "<<tkId<<std::endl;
    haveDigiPt[l][w] = ptOfMcTk[(*iter)->getTrackIndex()];
    haveDigiTheta[l][w] = thetaOfMcTk[(*iter)->getTrackIndex()];
    haveDigiPhi[l][w] = phiOfMcTk[(*iter)->getTrackIndex()];
    if(m_tupleWireEff){
      m_we_tkId = tkId;
      if(tkId>=0){
	if(tkId>=1000) tkId-=1000;
	m_we_pdg = pdgOfMcTk[tkId];
	m_we_primary = isPrimaryOfMcTk[tkId];
	//if(pdgOfMcTk[tkId]==-22) cout<<" primaryParticle ? "<< isPrimaryOfMcTk[tkId]<< " tkId "<<tkId <<" layer "<<l<<" wire "<<w<<" pdg="<<pdgOfMcTk[tkId]<<endl;
      }else{
	m_we_pdg = -999;
	m_we_primary = -999;
      }
      m_we_layer = l;
      m_we_wire = w;
      int gwire = w+Constants::nWireBeforeLayer[l];
      m_we_gwire = gwire;
      m_we_poisoned = hitPoisoned[l][w];
      if(hitInSegList[l][w]>0) m_we_seg = 1;
      else m_we_seg = 0;
      if(recHitMap[l][w]>0) m_we_track = 1;
      else m_we_track = 0;
      if(l==35&&tkId>=0&&abs(pdgOfMcTk[tkId])==11&&hitInSegList[l][w]<=0) {
	cout<<"layer "<<l<<" cell "<<w<<" gwire "<<gwire<<" inseg "<<hitInSegList[l][w]<<endl;
      }
      m_we_pt = ptOfMcTk[tkId];
      m_we_theta = thetaOfMcTk[tkId];
      m_we_phi = phiOfMcTk[tkId];
      m_tupleWireEff->write();
    }
  }
}//end of fillMcTruth()

void MdcTrkRecon::fillSegList() {
  if (2 == m_flags.segPar.lPrint) {
    std::cout << "print after Segment finding " << std::endl;
  }
  if (!m_flags.lHist) return;
  // Fill hits of every layer after segment finding
  for (int ii=0;ii<43;ii++){
    for (int jj=0;jj<288;jj++){ hitInSegList[ii][jj] = 0; }
  }
  int nSeg=0;
  for (int i = 0; i < _gm->nSuper(); i++) {
    MdcSeg* aSeg = (MdcSeg *) m_segs->oneList(i)->first();
    while (aSeg != NULL) {
      nSeg++;
      for (int ihit=0 ; ihit< aSeg->nHit() ; ihit++){
	const MdcHit* hit = aSeg->hit(ihit)->mdcHit();
	int layer = hit->layernumber();
	int wire = hit->wirenumber();
	hitInSegList[layer][wire]++;
      }
      aSeg = (MdcSeg *) aSeg->next();
    }
  }//end for slayer
  if (!m_tupleSeg) return;
  m_tsEvtNo =  t_eventNo;
  m_tsNDigi = t_nDigi;
  int iDigi=0;
  for (int ii=0;ii<43;ii++){
    for (int jj=0;jj<288;jj++){ 
      if (haveDigiTk[ii][jj] > -2){
	m_tsLayer[iDigi] = ii;
	m_tsWire[iDigi] = jj;
	m_tsInSeg[iDigi] = hitInSegList[ii][jj];
	m_tsMcTkId[iDigi] = haveDigiTk[ii][jj];
	iDigi++;
      }
    }
  }
  m_tsNSeg=nSeg;
  m_tupleSeg->write();
}//end of fillSegList

void MdcTrkRecon::dumpTdsTrack(RecMdcTrackCol* trackList){
  std::cout<< "tksize = "<<trackList->size() << std::endl;//yzhang debug
  RecMdcTrackCol::iterator it = trackList->begin();
  for (;it!= trackList->end();it++){
    RecMdcTrack *tk = *it;
    std::cout<< endl<<"//====RecMdcTrack "<<tk->trackId()<<"====:" << std::endl;
    cout <<" d0 "<<tk->helix(0)
      <<" phi0 "<<tk->helix(1)
      <<" cpa "<<tk->helix(2)
      <<" z0 "<<tk->helix(3)
      <<" tanl "<<tk->helix(4)
      <<endl;
    std::cout<<" q "<<tk->charge() 
      <<" theta "<<tk->theta()
      <<" phi "<<tk->phi()
      <<" x0 "<<tk->x()
      <<" y0 "<<tk->y()
      <<" z0 "<<tk->z()
      <<" r0 "<<tk->r()
      <<endl;
    std::cout <<" p "<<tk->p()
      <<" pt "<<tk->pxy()
      <<" px "<<tk->px()
      <<" py "<<tk->py()
      <<" pz "<<tk->pz()
      <<endl;
    std::cout<<" tkStat "<<tk->stat()
      <<" chi2 "<<tk->chi2()
      <<" ndof "<<tk->ndof()
      <<" nhit "<<tk->getNhits()
      <<" nst "<<tk->nster()
      <<endl;
    std::cout<< "errmat mat  " << std::endl;
    std::cout<< tk->err()<<std::endl;
    //std::cout<< "errmat array  " << std::endl;
    //for (int i=0; i<15; i++){ std::cout<< " "<<tk->err(i); }
    //std::cout<< "   " << std::endl;

    int nhits = tk->getVecHits().size();
    std::cout<<nhits <<" Hits: " << std::endl;
    for(int ii=0; ii <nhits ; ii++){
      Identifier id(tk->getVecHits()[ii]->getMdcId());
      int layer = MdcID::layer(id);
      int wire = MdcID::wire(id);
      cout<<"("<< layer <<","<<wire<<","<<tk->getVecHits()[ii]->getStat()
	<<",lr:"<<tk->getVecHits()[ii]->getFlagLR()<<") ";
    }//end of hit list
    std::cout << "  "<< std::endl;
  }//end of tk list
  std::cout << "  "<< std::endl;
}//end of dumpTdsTrack

void MdcTrkRecon::fillTrackList(){ 
  if (m_flags.debugFlag()>1) {
    std::cout << "=======Print after Track finding=======" << std::endl;
    m_tracks->plot();
  }

  if (!m_flags.lHist) return; 
  //----------- fill hit -----------
  int nTk = (*m_tracks).nTrack();
  for (int itrack = 0; itrack < nTk; itrack++) {
    MdcTrack *atrack = (*m_tracks)[itrack];
    if (atrack==NULL) continue;
    const TrkFit* fitResult = atrack->track().fitResult();
    if (fitResult == 0) continue;//check the fit worked

    //for fill ntuples
    int nSt=0; //int nSeg=0; 
    int seg[11] = {0}; int segme;
    //-----------hit list-------------
    const TrkHitList* hitlist = atrack->track().hits();
    TrkHitList::hot_iterator hot = hitlist->begin();
    int layerCount[43]={0};
    for(int iii=0;iii<43;iii++){layerCount[iii]=0;}
    int i=0;
    for (;hot!= hitlist->end();hot++){
      const MdcRecoHitOnTrack* recoHot;   		  
      recoHot = dynamic_cast<const MdcRecoHitOnTrack*> (&(*hot));
      int layer = recoHot->mdcHit()->layernumber();
      int wire  = recoHot->mdcHit()->wirenumber();
      layerCount[layer]++;
      recHitMap[layer][wire]++;
      //for n seg
      segme=0;
      if ( layer >0 ) {segme=(layer-1)/4;}
      seg[segme]++;
      if (recoHot->layer()->view()) { ++nSt; }

      if(!m_tuple1) continue;
      m_layer[i]    = layer;
      m_wire[i]	    = wire;
      m_ambig[i]    = recoHot->wireAmbig();// wire ambig
      //fltLen
      double fltLen = recoHot->fltLen(); 
      m_fltLen[i]   = fltLen;
      double tof    = recoHot->getParentRep()->arrivalTime(fltLen);
      //position
      HepPoint3D pos; Hep3Vector dir;    
      recoHot->getParentRep()->traj().getInfo(fltLen,pos,dir);
      m_x[i] = pos.x();	     
      m_y[i] = pos.y();	     
      m_z[i] = pos.z();	     
      m_driftT[i]   = recoHot->mdcHit()->driftTime(tof,pos.z());
      m_tof[i]	    = tof;
      m_driftD[i]   = recoHot->drift();
      m_sigma[i]    = recoHot->hitRms();	    
      m_tdc[i]	    = recoHot->rawTime();
      m_adc[i]	    = recoHot->mdcHit()->charge(); 
      m_doca[i]	    = recoHot->dcaToWire();//sign w.r.t. dirft() FIXME 
      m_entra[i]    = recoHot->entranceAngle();
      m_act[i]	    = recoHot->isActive();
      //diff with truth
      m_dx[i] = m_x[i] - mcX[layer][wire];
      m_dy[i] = m_y[i] - mcY[layer][wire];
      m_dz[i] = m_z[i] - mcZ[layer][wire];
      m_dDriftD[i] = m_driftD[i] - mcDrift[layer][wire];
      m_dlr[i] = m_ambig[i] - mcLR[layer][wire];
      //yzhang for pickHits debug
      m_cellWidth[i] = Constants::twoPi*_gm->Layer(layer)->rMid()/_gm->Layer(layer)->nWires();
      //zhangy
      //resid
      double res=999.,rese=999.;
      if (recoHot->resid(res,rese,false)){
      }else{}
      m_resid[i] = res;
      i++;
    }// end fill hit
    int nSlay=0;
    for(int i=0;i<11;i++){
      if (seg[i]>0) nSlay++;
    }
    if(m_tuple1){
      //------------fill track result-------------
      m_tkId = itrack;
      //track parameters at closest approach to beamline
      double fltLenPoca = 0.0;
      TrkExchangePar helix = fitResult->helix(fltLenPoca);
      m_q    = fitResult->charge();
      m_phi0 = helix.phi0();
      m_tanl = helix.tanDip();
      m_z0 = helix.z0();
      m_d0 = helix.d0();
      m_pt = fitResult->pt();
      m_nSlay = nSlay;
      if (m_pt > 0.00001){
	m_cpa = fitResult->charge()/fitResult->pt();
      }
      //momenta and position
      CLHEP::Hep3Vector p1 = fitResult->momentum(fltLenPoca);
      double px,py,pz,pxy;
      pxy = fitResult->pt();
      px = p1.x();
      py = p1.y();
      pz = p1.z();
      m_p = p1.mag();
      m_pz = pz;
      HepPoint3D poca = fitResult->position(fltLenPoca);
      m_pocax = poca.x();
      m_pocay = poca.y();
      m_pocaz = poca.z(); 
      m_nAct = fitResult->nActive();
      m_nHit = hitlist->nHit();
      m_nDof = fitResult->nDof();
      m_nSt  = nSt;
      m_chi2 = fitResult->chisq();
      //for (int l=0;l<43;l++) m_layerCount[l] = layerCount[l];
      m_t0 = t_t0;
      m_t0Stat = t_t0Stat;
      m_t0Truth = t_t0Truth;
      m_nTdsTk = t_nRecTk;
      m_nMcTk = t_mcTkNum;
      m_evtNo = t_eventNo;
      m_tuple1->write();
    }
  }//end of loop rec trk list
}//end of fillTrackList


void MdcTrkRecon::fillEvent(){
  if (m_tupleEvt!=NULL){

    uint32_t getDigiFlag = 0;
    getDigiFlag += m_maxMdcDigi;
    if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
    if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
    if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
    MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
    if ((int)mdcDigiVec.size()<m_minMdcDigi){
      std::cout << " Skip this event for MdcDigiVec.size() < "<<m_minMdcDigi << endl;
    }

    m_t4nDigi = mdcDigiVec.size();
    int iDigi=0;
    MdcDigiCol::iterator iter = mdcDigiVec.begin();
    for (;iDigi<m_t4nDigi; iter++ ) {
      double tdc = RawDataUtil::MdcTime((*iter)->getTimeChannel());
      double adc = RawDataUtil::MdcCharge((*iter)->getChargeChannel());
      m_t4Time[iDigi] = tdc;
      m_t4Charge[iDigi] = adc;
      long l = MdcID::layer((*iter)->identify());
      long w = MdcID::wire((*iter)->identify());
      m_t4Layer[iDigi] = l;
      m_t4Wire[iDigi] = w;
      m_t4PhiMid[iDigi] = _gm->Layer(l)->phiWire(w);
      m_t4Hot[iDigi] = recHitMap[l][w];
      iDigi++;
    }
    m_t4t0 = t_t0;
    m_t4t0Stat = t_t0Stat;
    m_t4t0Truth = t_t0Truth;
    m_t4EvtNo =  t_eventNo;
    m_t4nRecTk = t_nRecTk;
    SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
    if (mdcDigiCol){
      m_t4nDigiUnmatch = mdcDigiCol->size();
    }
    m_t4nMcTk= t_mcTkNum;
    m_tupleEvt->write();
  }
}//end of fillEvent 

void MdcTrkRecon::dumpDigi(){
  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot)     getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  std::cout<<name()<<" dump MdcDigiVec, nDigi="<<mdcDigiVec.size()<<std::endl;
  for (int iDigi=0;iter!= mdcDigiVec.end(); iter++,iDigi++ ) {
    long l = MdcID::layer((*iter)->identify());
    long w = MdcID::wire((*iter)->identify());
    int tkTruth = (*iter)->getTrackIndex();
    double tdc = RawDataUtil::MdcTime((*iter)->getTimeChannel());
    double adc = RawDataUtil::MdcCharge((*iter)->getChargeChannel());
    std::cout<<"("<<l<<","<<w<<";"<<tkTruth<<",t "<<tdc<<",c "<<adc<<")";
    if(iDigi%4==0) std::cout<<std::endl;
  }
  std::cout<<std::endl;
}
