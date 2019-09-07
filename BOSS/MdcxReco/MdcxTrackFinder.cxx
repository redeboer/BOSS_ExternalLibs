//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxTrackFinder.cxx,v 1.50 2017/08/18 06:51:52 zhangy Exp $
//
// Description:
//	Class MdcxTrackFinder
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Steve Wagner    		Original Author
//	Zhang Yao(zhangyao@ihep.ac.cn)
//
// Copyright Information:
//	Copyright (C) 1997		BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcxReco/MdcxTrackFinder.h"

//-------------
// C Headers --
//-------------
#include <assert.h>
#include <time.h>
#include <math.h>

//---------------
// C++ Headers --
//---------------
#include <iostream>

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "BField/BField.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcGeom/MdcDetector.h"
#include "EventModel/EventHeader.h"

#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/MdcxHits.h"
#include "MdcxReco/MdcxFindSegs.h"
#include "MdcxReco/MdcxSeg.h"
#include "MdcxReco/MdcxHel.h"
#include "MdcxReco/MdcxFittedHel.h"
#include "MdcxReco/MdcxFindTracks.h"
#include "MdcxReco/Mdcxprobab.h"
#include "CLHEP/Alist/AIterator.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkHitList.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkLineMaker.h"
#include "MdcxReco/MdcxAddHits.h"
#include "MdcxReco/MdcxMergeDups.h"
#include "TrkBase/TrkFitStatus.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"  
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"
#include "EvTimeEvent/RecEsTime.h"
#include "McTruth/McParticle.h"
#include "McTruth/MdcMcHit.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcRecoUtil/Pdt.h"
#include "MdcTrkRecon/MdcTrack.h"
//debug
#include "TrkBase/TrkHotList.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "MdcxReco/MdcxHistItem.h" 
#include "MdcGeom/Constants.h"
#include "MdcGeom/MdcTrkReconCut.h"
#include "MdcxReco/MdcxSegPatterns.h"
#include "TrigEvent/TrigData.h"

#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include "BesTimerSvc/BesTimer.h"



using std::cout;
using std::endl;

extern double MdcTrkReconCut_helix_fit[43];
//----------------
// Constructors --
//----------------

MdcxTrackFinder::MdcxTrackFinder(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_mdcCalibFunSvc(0)
{
  //input
  declareProperty("pdtFile",	  m_pdtFile = "pdt.table");
  //debug control
  declareProperty("debug",	  m_debug= 0);
  declareProperty("hist",	  m_hist= 0);
  declareProperty("mcHist",	  m_mcHist = false);
  //cuts and control
  declareProperty("cresol",	  m_cresol = 0.013);

  declareProperty("getDigiFlag",  m_getDigiFlag = 0);
  declareProperty("maxMdcDigi",   m_maxMdcDigi= 0);
  declareProperty("keepBadTdc",   m_keepBadTdc= 0);
  declareProperty("dropHot",      m_dropHot= 0);
  declareProperty("keepUnmatch",  m_keepUnmatch= 0);
  declareProperty("salvageTrk",   m_salvageTrk = false);
  declareProperty("dropMultiHotInLayer",m_dropMultiHotInLayer = false);
  declareProperty("dropTrkPt",    m_dropTrkPt = -999.);
  declareProperty("d0Cut",   	  m_d0Cut = 999.);
  declareProperty("z0Cut",   	  m_z0Cut = 999.);

  declareProperty("minMdcDigi",	  m_minMdcDigi = 0);
  declareProperty("countPropTime",m_countPropTime = true);
  declareProperty("addHitCut",	  m_addHitCut = 5.);
  declareProperty("dropHitsSigma",m_dropHitsSigma);
  declareProperty("helixFitCut",  m_helixFitCut);
  declareProperty("minTrkProb",	  m_minTrkProb = 0.01);
  declareProperty("csmax4",	  m_csmax4 = 50.);
  declareProperty("csmax3",	  m_csmax3 = 1.);
  declareProperty("helixFitSigma",m_helixFitSigma= 5.);
  declareProperty("maxRcsInAddSeg",m_maxRcsInAddSeg= 50.);
  declareProperty("nSigAddHitTrk", m_nSigAddHitTrk= 5.);
  declareProperty("maxProca",      m_maxProca= 0.6);
  declareProperty("doSag",         m_doSag= false);
  declareProperty("lineFit",       m_lineFit = false);
  //declareProperty("cosmicFit",     m_cosmicFit= false);
}

//--------------
// Destructor --
//--------------
MdcxTrackFinder::~MdcxTrackFinder() {
  delete m_bfield;
}

StatusCode MdcxTrackFinder::beginRun(){  
  // Get Mdc Detector Geometry
  m_gm = MdcDetector::instance(m_doSag); 
  if(NULL == m_gm) return StatusCode::FAILURE;
  MdcxHit::setMdcDetector(m_gm);

  return StatusCode::SUCCESS;
}
//--------------
// Operations --
//--------------
StatusCode MdcxTrackFinder::initialize(){  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;	

  t_nTkTot = 0;
  for (int i=0;i<20;i++) t_nTkNum[i]=0;

  //m_flags.readPar(m_paramFile);
#ifdef MDCXTIMEDEBUG
  StatusCode tsc = service( "BesTimerSvc", m_timersvc);
  if( tsc.isFailure() ) {
    log << MSG::WARNING << name() << ": Unable to locate BesTimer Service" << endreq;
    return StatusCode::FAILURE;
  }
  m_timer[0] = m_timersvc->addItem("Execution");
  m_timer[0]->propName("Execution");
  m_timer[1] = m_timersvc->addItem("findSeg");
  m_timer[1]->propName("findSeg");
  m_timer[2] = m_timersvc->addItem("findTrack");
  m_timer[2]->propName("findTrack");
  m_timer[3] = m_timersvc->addItem("fitting");
  m_timer[3]->propName("fitting");
#endif

  if (m_helixFitCut.size() == 43){
    for(int i=0; i<43; i++){
      //MdcTrkReconCut_helix_fit[i] = m_helixFitCut[i]; 
      TrkHelixFitter::nSigmaCut[i] = m_helixFitCut[i];
    }
  }
  MdcxParameters::debug = m_debug;
  MdcxParameters::minTrkProb = m_minTrkProb;
  MdcxParameters::csmax4 = m_csmax4;
  MdcxParameters::csmax3 = m_csmax3;
  MdcxParameters::helixFitSigma = m_helixFitSigma;
  MdcxParameters::maxRcsInAddSeg= m_maxRcsInAddSeg;
  MdcxParameters::nSigAddHitTrk = m_nSigAddHitTrk;
  MdcxParameters::maxProca      = m_maxProca;
  TrkHelixFitter::m_debug       = (m_debug>7);
  Pdt::readMCppTable(m_pdtFile); 
  MdcxFittedHel::debug = m_debug;


  // Get MdcCalibFunSvc
  IMdcCalibFunSvc* imdcCalibSvc; 
  StatusCode sc = service ("MdcCalibFunSvc", imdcCalibSvc);
  m_mdcCalibFunSvc = dynamic_cast<MdcCalibFunSvc*>(imdcCalibSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  MdcxHit::setMdcCalibFunSvc(m_mdcCalibFunSvc);
  MdcxHit::setCountPropTime(m_countPropTime);

  // Get RawDataProviderSvc
  IRawDataProviderSvc* iRawDataProvider; 
  sc = service ("RawDataProviderSvc", iRawDataProvider);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*>(iRawDataProvider);


  //Initailize magnetic filed 
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
  }
  m_bfield = new BField(m_pIMF);
  log << MSG::INFO << "field z = "<<m_bfield->bFieldNominal()<< endreq;
  m_context = new TrkContextEv(m_bfield);

  if (m_hist) {bookNTuple();}
  if (m_dropHitsSigma.size()==43){
    for (int ii=0;ii<43;ii++) {
      MdcxParameters::dropHitsSigma[ii]=m_dropHitsSigma[ii];
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode MdcxTrackFinder::execute() {


  b_saveEvent=false;
  setFilterPassed(b_saveEvent);
#ifdef MDCXTIMEDEBUG
  m_timer[0]->start();
  m_timer[1]->start();
#endif
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  StatusCode sc;

  nTk = 0; t_nTdsTk = 0; t_nDigi=0; t_nSeg=0; //yzhang  for fill 
  //------------------------------------
  // Get event No.
  //------------------------------------
  SmartDataPtr<Event::EventHeader> evtHead(eventSvc(),"/Event/EventHeader");
  if (!evtHead) {
    log << MSG::FATAL<< "Could not retrieve event header" << endreq;
    return StatusCode::FAILURE;
  } 
  m_eventNo = evtHead->eventNumber();
  if(m_debug>0)std::cout << "x evt: "<<evtHead->runNumber()<<" " << m_eventNo<< std::endl;
  long t_evtNo = m_eventNo;
  g_eventNo = m_eventNo;
  //if (t_evtNo % 1000 == 0) std::cout << "x evt: " << t_evtNo << std::endl;
  IDataManagerSvc *dataManSvc;
  DataObject *aTrackCol;
  DataObject *aHitCol;
  if (!m_salvageTrk) { 
    SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
    eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
    if(aTrackCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
      eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
    }
    eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aHitCol);
    if(aHitCol != NULL) {
      dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
      eventSvc()->unregisterObject("/Event/Recon/RecMdcHitCol");
    }
  }

  //------------------------------------
  // Initialize track collection in TDS
  //------------------------------------
  DataObject *aReconEvent;
  eventSvc()->findObject("/Event/Recon",aReconEvent);
  if (aReconEvent==NULL) {
    aReconEvent = new ReconEvent();
    sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
    if(sc != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return StatusCode::FAILURE;
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
  eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aHitCol);
  if (aHitCol) {
    hitList = dynamic_cast<RecMdcHitCol*> (aHitCol);
  }else{
    hitList = new RecMdcHitCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcHitCol, hitList);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcHit collection" <<endreq;
      return StatusCode::FAILURE;
    }
  }

  //------------------------------------
  // Initialize hit collection in TDS
  //------------------------------------
  DataObject *pnode = 0;
  sc = eventSvc()->retrieveObject("/Event/Hit",pnode);
  if(!sc.isSuccess()) {
    pnode = new DataObject;
    sc = eventSvc()->registerObject("/Event/Hit",pnode);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register /Event/Hit branch " <<endreq;
      return StatusCode::FAILURE;
    }
  }
  SmartDataPtr<MdcHitCol> m_hitCol(eventSvc(), "/Event/Hit/MdcHitCol");
  if (!m_hitCol){
    m_hitCol= new MdcHitCol;
    sc = eventSvc()->registerObject("/Event/Hit/MdcHitCol",m_hitCol);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register hit collection" <<endreq;
      return StatusCode::FAILURE;
    }
  } 


  //------------------------------------
  // Get bunch time t0 (ns) and timing
  //------------------------------------
  m_bunchT0 = -999.;
  SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!aevtimeCol || aevtimeCol->size()==0) {
    log << MSG::WARNING<< "evt "<<m_eventNo<<" Could not find RecEsTimeCol"<< endreq;
    return StatusCode::SUCCESS;
  }

  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
    m_bunchT0 =  (*iter_evt)->getTest();
    m_t0Stat = (*iter_evt)->getStat();
    if(m_debug>1) std::cout<<name()<<" "<<t_evtNo<<" t0  "<<m_bunchT0<<" t0Stat  "<<m_t0Stat<<std::endl;
    if ((m_t0Stat==0) || (m_bunchT0 < 0.) || (m_bunchT0 > 9999.0) ){
      log << MSG::WARNING << "Skip evt:"<<m_eventNo<< " by  t0 = "<<m_bunchT0 << endreq;
      //return StatusCode::SUCCESS;
    }
  }
  if(m_debug>1) std::cout<<name()<<" evt "<<t_evtNo<<" t0  "<<m_bunchT0<<" t0Stat  "<<m_t0Stat<<std::endl;
  int trigtiming=-10;
  SmartDataPtr<TrigData> trigData(eventSvc(),"/Event/Trig/TrigData");
  if(trigData){
    log << MSG::INFO <<"Trigger conditions 0--43:"<<endreq;
    for(int i = 0; i < 48; i++) {
      log << MSG::INFO << trigData->getTrigCondName(i)<<" ---- "<<trigData->getTrigCondition(i)<< endreq;
    }
    for(int i = 0; i < 16; i++) log << MSG::INFO << "Trigger channel "<< i << ": " << trigData->getTrigChannel(i) << endreq;
    m_timing=trigData->getTimingType();
    //cout<<"-----------------trigger timing type-----------------------: "<<trigtiming<<endl;
    log << MSG::INFO <<"Tigger Timing type: "<< trigtiming << endreq;
  }

  //------------------------------------
  //  Initialize MdcxHits
  //------------------------------------
  m_mdcxHits.reset();
  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot||m_salvageTrk) getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
  t_nDigi = mdcDigiVec.size();

  // fill Mc truth
  //if(m_hist) fillMcTruth();

  //skip event by hit numbe
  if (t_nDigi < m_minMdcDigi){
    if (0 == t_nDigi ){
      log << MSG::WARNING << " No hits in MdcDigiVec" << endreq;
    }
    log << MSG::WARNING << " Skip this event for MdcDigiVec.size() < "<<m_minMdcDigi << endreq;
    return StatusCode::SUCCESS;
  }
  m_mdcxHits.create(mdcDigiVec, m_bunchT0, m_cresol); 
  const HepAList<MdcxHit>& dchitlist = m_mdcxHits.GetMdcxHitList();

  if(m_debug>2) m_mdcxHits.print(std::cout,6796);

  //--------------------------------------------
  //  Make segments (MdcxSeg's) out of MdcxHit's
  //-------------------------------------------- 
  MdcxFindSegs dcsegs(dchitlist,m_debug);
  const HepAList<MdcxSeg>& seglist = dcsegs.GetMdcxSeglist();
  if(m_debug > 1 ){ dumpMdcxSegs(seglist);}
  t_nSeg = seglist.length();
  //if(m_hist){ fillMdcxSegs(seglist);}

#ifdef MDCXTIMEDEBUG
  m_timer[1]->stop();
  m_timer[2]->start();
#endif
  //--------------------------------------------
  //  Make tracks (MdcxFittedHel's) out of MdcxSeg's
  //--------------------------------------------
  MdcxFindTracks dctrks(seglist,m_debug);
  HepAList<MdcxFittedHel>& firsttrkl = (HepAList<MdcxFittedHel>&)dctrks.GetMdcxTrklist();
  if(m_debug>1) dumpTrackList(firsttrkl);

#ifdef MDCXTIMEDEBUG
  m_timer[2]->stop();
  m_timer[3]->start();
#endif
  //if(m_hist){ fillTrkl(firsttrkl);}

  //if(m_debug>1){
  //  std::cout << "dchitlist after find tracks before MergeDups, nhits=" << dchitlist.length() << std::endl;
  //  for (int ii = 0; ii < dchitlist.length(); ii++) {
  //    dchitlist[ii]->print(std::cout, ii);
  //  }
  //  std::cout<<std::endl;
  //}
  MdcxMergeDups dcmergeem(firsttrkl,m_debug);
  HepAList<MdcxFittedHel>& trkl = (HepAList<MdcxFittedHel>&)dcmergeem.GetMergedTrklist();

  //if (m_debug > 1 ){
  //  cout << "MdcxTrackFinder: after MergeDups, have "
  //  << trkl.length() << " track(s). nhits=" << dchitlist.length() << endl;
  //  for (int ii = 0; ii < dchitlist.length(); ii++) {
  //    dchitlist[ii]->print(std::cout, ii);
  //  }
  //  std::cout<<std::endl;
  //}

  //---------------------------------------------------------
  //  Put my tracks into official fitter and store to TDS 
  //---------------------------------------------------------- 


  sc = FitMdcxTrack(trkl, dchitlist, m_hitCol, trackList, hitList);
  if (!sc.isSuccess()) {return StatusCode::SUCCESS;}
  t_nTdsTk = trackList->size();

  t_nTkTot += trackList->size();
  if(t_nTdsTk<20) t_nTkNum[t_nTdsTk]++;

#ifdef MDCXTIMEDEBUG
  m_timer[0]->stop();
  m_timer[3]->stop();
#endif

  if(m_hist) fillEvent();
  if (m_debug > 0) {
    DataObject* pNode;
    eventSvc()->retrieveObject("/Event/Recon/RecMdcTrackCol",pNode);
    RecMdcTrackCol *tmpTrackCol = dynamic_cast<RecMdcTrackCol*> (pNode);
    eventSvc()->retrieveObject("/Event/Recon/RecMdcHitCol",pNode);
    int nTdsTk = 0;
    if(tmpTrackCol) nTdsTk = tmpTrackCol->size(); 

    //if (t_evtNo % 1000 == 0) {
    std::cout<< "MdcxTrackFinder: evtNo "<< m_eventNo << " t0="<<m_bunchT0
      <<" Found " <<trkl.length() 
      <<" keep "<< t_nTdsTk
      <<" finialy keep "<< nTdsTk;

    int ndelete =0; trkl.length() - trackList->size();
    if( ndelete>0 ) std::cout <<" delete "<< ndelete;
    std::cout <<" track(s)" <<endl;
    //}

    if(m_debug>1)dumpTdsTrack(tmpTrackCol);
    if(m_debug>1)dumpTrack(tmpTrackCol);
    //dumpTdsHits(tmpHitCol);

  }
  if((trackList->size()!=4) ) b_saveEvent = true;
  setFilterPassed(b_saveEvent); 
  return StatusCode::SUCCESS;
}

StatusCode MdcxTrackFinder::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  std::cout<< " --after " << name() << " keep " << t_nTkTot<< " tracks  "<<std::endl;
  for(int i=0; i<20; i++){
    if(t_nTkNum[i]>0)std::cout<< " nTk="<< i << " "<< t_nTkNum[i]<< std::endl;
  }

  //tot evtNo, trkNum
  return StatusCode::SUCCESS;
}


void MdcxTrackFinder::MdcxHitsToHots(MdcxHel& mdcxHelix, const HepAList<MdcxHit>& mdcxHits,
    TrkHitList* m_trkHitList, MdcHitCol* mdcHitCol) {

  if ( 0 == mdcxHits.length() ) return;

  int ihits = 0; 
  while(mdcxHits[ihits]) {
    int ambig = 0;//=mdcxHelix.Doca_Samb();//yzhang delete 2011-10-13 
    const MdcHit* newhit = mdcxHits[ihits]->getMdcHit();
    if ( 0 == newhit ) {
      const MdcDigi* theDigi = mdcxHits[ihits]->getDigi();
      int layer =  MdcID::layer(mdcxHits[ihits]->getDigi()->identify());
      int wire  =  MdcID::wire(mdcxHits[ihits]->getDigi()->identify());
      m_digiMap[layer][wire] = mdcxHits[ihits]->getDigi();
      MdcHit *thehit = new MdcHit(theDigi, m_gm); 
      thehit->setCalibSvc(m_mdcCalibFunSvc);
      thehit->setCountPropTime(m_countPropTime);
      //thehit->setCosmicFit(m_cosmicFit);

      mdcHitCol->push_back(thehit);
      newhit = thehit;
    }
    MdcRecoHitOnTrack temp(*newhit, ambig, m_bunchT0);//m_bunchT0 nano second here
    MdcHitOnTrack* newhot = &temp;
    //double fltLen = mdcxHelix.Doca_FLen();
    //std::cout<<" fltLen-- "<<fltLen<<"   "<<std::endl;
    //newhot->setFltLen(fltLen);
    //newhot->print(std::cout);
    //std::cout<< __FILE__ << " ambig  " << ambig << " append  "<<std::endl;

    // Store MdcxHits to TrkHitList
    m_trkHitList->appendHot(newhot);   //yzhang TEMP FIXME
    newhot->setUsability(true);//yzhang add 2011-10-12 TEMP
    newhot->setActivity(true);//yzhang add 2011-10-12 TEMP

    ihits++;
  }
}

StatusCode MdcxTrackFinder::FitMdcxTrack(HepAList<MdcxFittedHel>& trkl, 
    const HepAList<MdcxHit>& dchitlist, MdcHitCol* m_hitCol,
    RecMdcTrackCol* trackList, RecMdcHitCol* hitList){
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  //--Add Hit to MdcxFittedHel
  //if(m_debug>1){
  //  std::cout << "dchitlist before addHits nhits=" << dchitlist.length() << std::endl;
  //  for (int ii = 0; ii < dchitlist.length(); ii++) {
  //    dchitlist[ii]->print(std::cout, ii);
  //  }
  //  std::cout<<std::endl;
  //}
  MdcxAddHits dcaddem(trkl, dchitlist, m_addHitCut);
  //if (m_debug > 1){
  //  cout << "MdcxTrackFinder: after AddHits, have "
  //    << trkl.length() << " track(s). nhits=" << dchitlist.length() << endl;
  //  for (int ii = 0; ii < dchitlist.length(); ii++) {
  //    dchitlist[ii]->print(std::cout, ii);
  //  }
  //  std::cout<<std::endl;
  //}

  TrkLineMaker linefactory; 
  TrkHelixMaker helixfactory;


  int tkLen = trkl.length();//FIXME
  for (int kk=0; kk< tkLen; kk++){
    const HepAList<MdcxHit>& xhits = trkl[kk]->XHitList();
    if(m_debug>2){
      std::cout<<__FILE__<<" FitMdcxTrack "<<kk<<  std::endl;
      for(int i=0; i<xhits.length(); i++){ xhits[i]->print(std::cout); }
      std::cout<<std::endl;
    }

    if(m_debug>2) std::cout<<" before add hits nhits="<<xhits.length()<<  std::endl;
    HepAList<MdcxHit> xass = dcaddem.GetAssociates(kk);
    if(m_debug>2){
      std::cout<<" after,add "<<xass.length()<<" hits"<<std::endl;
    }

    MdcxFittedHel mdcxHelix = *trkl[kk];
    double thechisq = mdcxHelix.Chisq();
    TrkExchangePar tt(-mdcxHelix.D0(),mdcxHelix.Phi0(),mdcxHelix.Omega(),-mdcxHelix.Z0(),-mdcxHelix.Tanl());
    TrkRecoTrk* aTrack;
    int nparm;

    if (m_lineFit) {
      /// m_bunchT0 in "ns" here, but "second" in TrkLineMaker&TrkHelixMaker
      aTrack = linefactory.makeTrack(tt, thechisq, *m_context, m_bunchT0*1.e-9);
      nparm = 4;
      linefactory.setFlipAndDrop(*aTrack, true, true);
    } else {
      aTrack = helixfactory.makeTrack(tt, thechisq, *m_context, m_bunchT0*1.e-9);
      nparm = 5;
      helixfactory.setFlipAndDrop(*aTrack, true, true);
    }

    TrkHitList* m_trkHitList = aTrack->hits();
    if (0 == m_trkHitList) {
      delete aTrack; 
      aTrack = NULL;
      continue;
    }


    MdcxHitsToHots(mdcxHelix, xhits, m_trkHitList, m_hitCol);
    //std::cout<<"xhits---------------------"<<std::endl;//debug
    //m_trkHitList->hotList().printAll(cout);//debug
    MdcxHitsToHots(mdcxHelix, xass, m_trkHitList, m_hitCol);
    //std::cout<<"xass----------------------"<<std::endl;//debug
    //m_trkHitList->hotList().printAll(cout);//debug
    //std::cout<<"size "<<m_trkHitList->hotList().nHit()<<std::endl;
    //int beforDrop = m_trkHitList->hotList().nHit();
    if(m_dropMultiHotInLayer) dropMultiHotInLayer(m_trkHitList);//yzhang debug FIXME
    //int afterDrop = m_trkHitList->hotList().nHit();
    //std::cout<<"drop "<<beforDrop-afterDrop<<" keep:"<<afterDrop<<::endl;
    if(m_debug>1){ std::cout<< "== put to official fitter  "<<std::endl;}
    TrkErrCode err = m_trkHitList->fit();

    if(m_debug>1){
      std::cout<< "== after official fitter  "<<std::endl;
      aTrack->printAll(std::cout);
    }

    const TrkFit* theFit = aTrack->fitResult();
    float rcs = 10000.0;

    if (theFit) {
      int ndof = theFit->nActive()-nparm;
      if (ndof > 0) rcs = theFit->chisq()/float(ndof);
      if (m_debug>1) {
	if (4 == nparm) cout << " TrkLineMaker";  
	else  cout << " TrkHelixMaker"; 
	cout << " trkNo. " << kk << " success " << err.success() << " rcs " << rcs
	  << " chi2 " << theFit->chisq() << " nactive  " << theFit->nActive() << endl;
      }
    }

    if ( (1 == err.success()) && (rcs < 20.0) ) {
      if(m_debug>1) std::cout<<"== fit success "<<std::endl;//yzhang debug
      if (4 == nparm) {
	linefactory.setFlipAndDrop(*aTrack, false, false);
      } else {
	helixfactory.setFlipAndDrop(*aTrack, false, false);
      }
      //-------------Stick the found tracks into the list in RecMdcTrackCol--------
      if (m_debug>1) { cout << "MdcxTrackFinder: accept a track " << endl; }
      // update history
      aTrack->status()->addHistory(err,name().c_str()); //yzhang FIXME 
      store(aTrack, trackList, hitList);//aTrack have been deleted
    } else if ( (2 == err.success()) && (rcs < 150.0) ) {  //////// zoujh
      if(m_debug > 1) std::cout<<"== fit success = 2, refit now"<<std::endl;//yzhang debug
      int nrefit = 0;
      while (nrefit++ < 5) {
	if (m_debug>1) std::cout << "refit time " << nrefit << std::endl;
	err = m_trkHitList->fit();
	if (err.success() == 1) break;
      }
      if (err.success() == 1) {
	if (4 == nparm) {
	  linefactory.setFlipAndDrop(*aTrack, false, false);
	} else {
	  helixfactory.setFlipAndDrop(*aTrack, false, false);
	}
	//-------------Stick the found tracks into the list in RecMdcTrackCol--------
	//if (m_debug>1) { cout << "MdcxTrackFinder: accept a track and store to TDS" << endl; }
	// update history
	aTrack->status()->addHistory(err,name().c_str()); //yzhang FIXME 
	store(aTrack, trackList, hitList);//aTrack have been deleted
      }      //////////////////////////zoujh
    } else {
      if (m_debug >1) {
	std::cout<<"== fit faild "<<std::endl;//yzhang debug
	err.print(cout);
	cout << endl;
      }
      delete aTrack;
      aTrack = NULL;
      //---------------------------------------  
      //  Fit no good; try a better input helix
      //---------------------------------------
      if(m_debug>1) std::cout<<"== Fit no good; try a better input helix"<<std::endl;//yzhang debug
      mdcxHelix.Grow(*trkl[kk],xass);
      mdcxHelix.VaryRes(); 
      mdcxHelix.SetChiDofBail(1500);//yzhang add 2009-11-03 
      int fail = mdcxHelix.ReFit();
      if(m_debug>1)std::cout<<__FILE__<<" refit fail:"<<fail<<  std::endl;
      if (!mdcxHelix.Fail()) {
	const HepAList<MdcxHit>& bxhits = mdcxHelix.XHitList(); 
	thechisq = mdcxHelix.Chisq();
	TrkExchangePar tb(mdcxHelix.D0(),mdcxHelix.Phi0(),mdcxHelix.Omega(),mdcxHelix.Z0(),mdcxHelix.Tanl());
	TrkRecoTrk* bTrack;
	if (4 == nparm){
	  bTrack = linefactory.makeTrack(tb,thechisq,*m_context,m_bunchT0*1.e-9);
	  linefactory.setFlipAndDrop(*bTrack, false, false);
	}else{
	  bTrack = helixfactory.makeTrack(tb,thechisq,*m_context,m_bunchT0*1.e-9); 
	  helixfactory.setFlipAndDrop(*bTrack, false, false);
	}
	TrkHitList* bhits = bTrack->hits();  
	if (0 == bhits){delete bTrack; bTrack = NULL; continue;}

	MdcxHitsToHots(mdcxHelix, bxhits, bhits, m_hitCol);
	TrkErrCode berr = bhits->fit();
	const TrkFit* bFit = bTrack->fitResult();
	rcs=10000.0;
	if (bFit) {
	  int ndof = bFit->nActive() - nparm;
	  if (ndof > 0) rcs = bFit->chisq()/float(ndof);
	  if (m_debug >1) {
	    if (4 == nparm) cout << " TrkLineMaker";
	    else cout << " TrkHelixMaker";
	    cout << " success trkNo. " << kk << " status " << berr.success() << " rcs " << rcs
	      << " chi2 " << bFit->chisq() << " nactive  "<< bFit->nActive() << endl;
	  }
	}
	if ( ( 1 == berr.success() ) && ( rcs < 50.0 ) ) {
	  // update history
	  bTrack->status()->addHistory(berr,name().c_str());//yzhang FIXME
	  if (m_debug>1) { 
	    cout << "MdcxTrackFinder: accept b track and store to TDS" << endl;  
	    bTrack->printAll(cout); 
	  }
	  store(bTrack, trackList, hitList);//bTrack have been deleted
	} else {
	  if (m_debug>1) {
	    cout<< " fit failed "<<endl;
	    berr.print(cout);
	    cout << endl;
	  }
	  if (bTrack!=NULL) { delete bTrack; bTrack = NULL; }
	}
      }else{
	//cout<< " grow and refit failed "<<endl;
      }
    } 
  }
  if(m_debug >1) dumpTdsTrack(trackList);
  return StatusCode::SUCCESS;

}// end of FitMdcxTrack

void MdcxTrackFinder::store(TrkRecoTrk* aTrack, RecMdcTrackCol *trackList,
    RecMdcHitCol *hitList) {
  assert (aTrack != NULL);
  nTk++;
  int trackId = trackList->size();
  TrkExchangePar helix = aTrack->fitResult()->helix(0.);
  if(m_dropTrkPt>0. && (aTrack->fitResult()->pt()<m_dropTrkPt)) {
    if(m_debug>1) std::cout<<"MdcxTrackFinder delete track by pt "
      <<aTrack->fitResult()->pt()<<"<ptCut "<<m_dropTrkPt << std::endl;
    return;
  }

  if( ( (fabs(helix.d0())>m_d0Cut) ||( fabs(helix.z0())>m_z0Cut) ) ){
    if(m_debug>1) std::cout<< name()<<" delete track by d0 "<<helix.d0()<<">d0Cut "<<m_d0Cut
      <<" or z0 "<<helix.z0()<<" >z0Cut "<<m_z0Cut << std::endl;
    return;
  }

  if(m_hist) fillTrack(aTrack);
  MdcTrack mdcTrack(aTrack);//aTrack have been deleted in ~MdcTrack() 
  //tkStat:  0,PatRec 1,MdcxReco 2,Tsf 3,CurlFinder -1,Combined cosmic
  int tkStat = 1;
  int nHitbefore = hitList->size();

  mdcTrack.storeTrack(trackId, trackList, hitList, tkStat);
  int nHitAfter = hitList->size();
  if (nHitAfter - nHitbefore <10 ) setFilterPassed(true);
}

void MdcxTrackFinder::dumpTrack(RecMdcTrackCol* trackList){
  RecMdcTrackCol::iterator i_tk = trackList->begin();
  for (;i_tk != trackList->end(); i_tk++) {
    printTrack(*(i_tk));
  }
}// dumpTrack 

void MdcxTrackFinder::printTrack(RecMdcTrack* tk){
  //yzhang debug
  std::cout<< " MdcTrack Id:"<<tk->trackId() <<" q:"<< tk->charge()<< std::endl;
  std::cout<< "dr   Fi0   Cpa   Dz   Tanl   Chi2   Ndf   nSt   FiTerm  poca" << std::endl;
  std::cout<<"(" <<setw(5) << tk->helix(0)<<","<< setw(5) << tk->helix(1)<<"," << setw(5) << tk->helix(2) <<","
    << setw(5) << tk->helix(3) << ","<< setw(5) << tk->helix(4) <<")"
    << setw(5) << tk->chi2() << setw(4) << tk->ndof() 
    << setw(4) << tk->getNhits() << setw(4) << tk->nster() 
    << setw(5) << tk->getFiTerm() <<tk->poca()<<std::endl;
  std::cout<< " ErrMat  "<<tk->err() << std::endl;

  std::cout<< "hitId  tkId  (l,w) fltLen  lr  dt  ddl  tdc "//<<chi2Add
    <<"doca  entr  z  tprop stat  " << std::endl;

  HitRefVec hl = tk->getVecHits();
  HitRefVec::iterator it = hl.begin();   
  for (;it!=hl.end();++it){
    RecMdcHit* h = *it;
    int layer = MdcID::layer(h->getMdcId());
    double _vprop = (layer<8) ? Constants::vpropInner : Constants::vpropOuter;
    const MdcLayer* _layerPtr = m_gm->Layer(layer);
    double _zlen     = _layerPtr->zLength();
    double z = h->getZhit();
    double tprop;
    if (0 == layer%2){
      tprop = (0.5*_zlen + z)/_vprop; //odd
    }else{
      tprop = (0.5*_zlen - z)/_vprop; //even
    }
    // build the sense wires
    //const MdcSWire* wire = m_gm->Wire(MdcID::layer(h->getMdcId()),MdcID::wire(h->getMdcId()));
    //double z = wire->zForw();
    //while(z<wire->zRear()){
    //HepPoint3D pos;
    //Hep3Vector dir; 
    //if(!(wire->getTraj()==NULL)){
    //wire->getTraj()->getInfo(z,pos,dir);
    //}
    //std::cout<<"("<< wire->layer()->layNum()<<","<<wire->cell()<<" "<<wire->Id()<<")";
    //std::cout<<" z, sag:"<<z
    //<<", "<<wire->getTraj()->deltaY(z-wire->zForw())<<std::endl;
    ////<<" pos:"<<pos <<" dir:"<<dir<<std::endl;
    //z+=1.;
    //}
    std::cout<< setw(3) << h->getId() << setw(2) << h->getTrkId() <<
      "(" << MdcID::layer(h->getMdcId()) <<"," << MdcID::wire(h->getMdcId()) <<")"<<
      setw(10) << h->getFltLen() <<
      setw(2) << h->getFlagLR() <<setw(10) << h->getDriftT() <<
      setw(12) << h->getDriftDistLeft() <<//setw(8) << h->getErrDriftDistRight() <<
      setw(8) << h->getTdc() <<//setw(10) << h->getChisqAdd() <<
      setw(10) << h->getDoca() <<setw(10) << h->getEntra() <<
      setw(10) << h->getZhit() << setw(10) << tprop<<
      setw(2)<< h->getStat() << std::endl;
  }
}//print track

void MdcxTrackFinder::bookNTuple(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  if(!m_hist) return;

  g_poison = histoSvc()->book( "poison", "poison",43,0,42,288,0,288 );
  g_csmax4 = histoSvc()->book( "csmax4", "csmax4",100,0,100 );
  g_csmax3 = histoSvc()->book( "csmax3", "csmax3",50000,0,500000 );
  g_omegag = histoSvc()->book( "omegag", "omegag",1000 ,0.,1.);
  g_dPhiAU = histoSvc()->book( "dPhiAU", "dPhiAU",1000 ,0.,3.5);
  g_dPhiAU_0 = histoSvc()->book( "dPhiAU_0", "dPhiAU_0",1000 ,0.,3.5);
  g_dPhiAU_1 = histoSvc()->book( "dPhiAU_1", "dPhiAU_1",1000 ,0.,3.5);
  g_dPhiAU_5 = histoSvc()->book( "dPhiAU_5", "dPhiAU_5",1000 ,0.,3.5);
  g_dPhiAU_7 = histoSvc()->book( "dPhiAU_7", "dPhiAU_7",1000 ,0.,3.5);
  g_dPhiAV = histoSvc()->book( "dPhiAV", "dPhiAV",1000 ,0.,3.5);
  g_addSegPhi = histoSvc()->book( "addSegPhi", "addSegPhi",1000 ,0.,3.5);
  g_dPhiAV_0 = histoSvc()->book( "dPhiAV_0", "dPhiAV_0",1000 ,0.,3.5);
  g_dPhiAV_1 = histoSvc()->book( "dPhiAV_1", "dPhiAV_1",1000 ,0.,3.5);
  g_dPhiAV_6 = histoSvc()->book( "dPhiAV_6", "dPhiAV_6",1000 ,0.,3.5);
  g_dPhiAV_8 = histoSvc()->book( "dPhiAV_8", "dPhiAV_8",1000 ,0.,3.5);
  g_trkllmk = histoSvc()->book( "trkllmk", "trkllmk",43,0.,42);
  g_trklgood = histoSvc()->book( "trklgood", "trklgood",43,0.,42);
  g_trklcircle = histoSvc()->book( "trklcircle", "trklcircle",43,0.,42);
  g_trklhelix= histoSvc()->book( "trklhelix", "trklhelix",43,0.,42);
  g_trkldrop1= histoSvc()->book( "trkldrop1", "trkldrop1",43,0.,42);
  g_trkldrop2= histoSvc()->book( "trkldrop2", "trkldrop2",43,0.,42);
  g_trklappend1= histoSvc()->book( "trklappend1", "trklappend1",43,0.,42);
  g_trklappend2= histoSvc()->book( "trklappend2", "trklappend2",43,0.,42);
  g_trklappend3= histoSvc()->book( "trklappend3", "trklappend3",43,0.,42);
  //g_fitOmega= histoSvc()->book( "fitOmega", "fitOmega",200,0.,100.);
  g_trklfirstProb= histoSvc()->book( "trklfirstProb", "trklfirstProb",200,0.,2.);
  g_trkltemp= histoSvc()->book( "trkltemp", "trkltemp",200,-3.5,3.5);

  g_trklproca= histoSvc()->book( "trklproca", "trklproca",100,0.,5.);
  g_trkle= histoSvc()->book( "trkle", "trkle",200,0.,0.025);
  g_trkld= histoSvc()->book( "trkld", "trkld",200,-1.2,1.2);
  g_trklel= histoSvc()->book( "trklel", "trklel",200,0.,0.025,43,0,43);
  g_trkldl= histoSvc()->book( "trkldl", "trkldl",200,-1.2,1.2,43,0,43);
  g_trkldoca= histoSvc()->book( "trkldoca", "trkldoca",200,-1.2,1.2);
  g_trkllayer= histoSvc()->book( "trkllayer", "trkllayer",43,0,43);
  g_dropHitsSigma= histoSvc()->book( "dropHitsSigma", "dropHitsSigma",43,0,43,100,0,11);
  g_addHitCut= histoSvc()->book( "addHitCut", "addHitCut",100,0,10);
  g_addHitCut2d= histoSvc()->book( "addHitCut2d", "addHitCut2d",43,0,43,100,0,10);
  //g_addSegPhiDiff = histoSvc()->book( "addSegPhiDiff", "addSegPhiDiff",100,-6.28,6.28);

  NTuplePtr nt1(ntupleSvc(), "MdcxReco/rec");
  if ( nt1 ) { m_xtuple1 = nt1;}
  else {
    m_xtuple1 = ntupleSvc()->book ("MdcxReco/rec", CLID_ColumnWiseTuple, "MdcxReco reconsturction results");
    if ( m_xtuple1 ) {
      sc = m_xtuple1->addItem ("t0",      m_xt0);
      sc = m_xtuple1->addItem ("timing",  m_xtiming);
      sc = m_xtuple1->addItem ("t0Stat",  m_xt0Stat);
      sc = m_xtuple1->addItem ("t0Truth", m_xt0Truth);

      sc = m_xtuple1->addItem ("nSlay",   m_xnSlay);
      sc = m_xtuple1->addItem ("p",       m_xp);
      sc = m_xtuple1->addItem ("pt",      m_xpt);
      sc = m_xtuple1->addItem ("pz",      m_xpz);
      sc = m_xtuple1->addItem ("d0",      m_xd0);
      sc = m_xtuple1->addItem ("phi0",    m_xphi0);
      sc = m_xtuple1->addItem ("cpa",     m_xcpa);
      sc = m_xtuple1->addItem ("z0",      m_xz0);
      sc = m_xtuple1->addItem ("tanl",    m_xtanl);
      sc = m_xtuple1->addItem ("q",       m_xq);
      sc = m_xtuple1->addItem ("pocax",   m_xpocax);
      sc = m_xtuple1->addItem ("pocay",   m_xpocay);
      sc = m_xtuple1->addItem ("pocaz",   m_xpocaz);

      sc = m_xtuple1->addItem ("evtNo",   m_xevtNo);
      sc = m_xtuple1->addItem ("nSt",     m_xnSt);
      sc = m_xtuple1->addItem ("nAct",    m_xnAct);
      sc = m_xtuple1->addItem ("nDof",    m_xnDof);
      sc = m_xtuple1->addItem ("chi2",    m_xchi2);
      sc = m_xtuple1->addItem ("tkId",    m_xtkId);
      sc = m_xtuple1->addItem ("nHit",    m_xnHit, 0, 7000);//# of hit/rec track
      sc = m_xtuple1->addIndexedItem ("resid",    m_xnHit, m_xresid);
      sc = m_xtuple1->addIndexedItem ("sigma",    m_xnHit, m_xsigma);
      sc = m_xtuple1->addIndexedItem ("driftD",   m_xnHit, m_xdriftD);
      sc = m_xtuple1->addIndexedItem ("driftT",   m_xnHit, m_xdriftT);
      sc = m_xtuple1->addIndexedItem ("doca",     m_xnHit, m_xdoca);
      sc = m_xtuple1->addIndexedItem ("entra",    m_xnHit, m_xentra);
      sc = m_xtuple1->addIndexedItem ("ambig",    m_xnHit, m_xambig);
      sc = m_xtuple1->addIndexedItem ("fltLen",   m_xnHit, m_xfltLen);
      sc = m_xtuple1->addIndexedItem ("tof",      m_xnHit, m_xtof);
      sc = m_xtuple1->addIndexedItem ("act",      m_xnHit, m_xact);
      sc = m_xtuple1->addIndexedItem ("tdc",      m_xnHit, m_xtdc);
      sc = m_xtuple1->addIndexedItem ("adc",      m_xnHit, m_xadc);
      sc = m_xtuple1->addIndexedItem ("layer",    m_xnHit, m_xlayer);
      sc = m_xtuple1->addIndexedItem ("wire",     m_xnHit, m_xwire);
      sc = m_xtuple1->addIndexedItem ("x",        m_xnHit, m_xx);
      sc = m_xtuple1->addIndexedItem ("y",        m_xnHit, m_xy);
      sc = m_xtuple1->addIndexedItem ("z",        m_xnHit, m_xz);
    } else {   // did not manage to book the N tuple....
      log << MSG::ERROR << "    Cannot book N-tuple: MdcxReco/rec" << endmsg;
      //return;
    }
  }//end book of nt1
  NTuplePtr nt4(ntupleSvc(), "MdcxReco/evt");
  if ( nt4 ) { m_xtupleEvt = nt4;}
  else {
    m_xtupleEvt = ntupleSvc()->book ("MdcxReco/evt", CLID_ColumnWiseTuple, "MdcxReco event data");
    if ( m_xtupleEvt ) {
      sc = m_xtupleEvt->addItem ("evtNo",	 m_xt4EvtNo );
      sc = m_xtupleEvt->addItem ("nRecTk",	 m_xt4nRecTk );
      sc = m_xtupleEvt->addItem ("nTdsTk",	 m_xt4nTdsTk );
      sc = m_xtupleEvt->addItem ("t0",		 m_xt4t0);
      sc = m_xtupleEvt->addItem ("t0Stat",       m_xt4t0Stat);
      sc = m_xtupleEvt->addItem ("t0Truth",      m_xt4t0Truth);
      sc = m_xtupleEvt->addItem ("time",         m_xt4time);
      sc = m_xtupleEvt->addItem ("timeSeg",      m_xt4timeSeg);
      sc = m_xtupleEvt->addItem ("timeTrack",    m_xt4timeTrack);
      sc = m_xtupleEvt->addItem ("timeFit",      m_xt4timeFit);
      sc = m_xtupleEvt->addItem ("nSeg",         m_xt4nSeg);
      sc = m_xtupleEvt->addItem ("nDigi",	 m_xt4nDigi, 0, 7000);//# of hit/mc track 
      sc = m_xtupleEvt->addIndexedItem ("layer",  m_xt4nDigi, m_xt4Layer);
      sc = m_xtupleEvt->addIndexedItem ("rt",     m_xt4nDigi, m_xt4Time);
      sc = m_xtupleEvt->addIndexedItem ("rc",     m_xt4nDigi, m_xt4Charge);
      //sc = m_xtupleEvt->addIndexedItem ("rawHit", m_xt4nDigi, m_xt4rawHit);
      //sc = m_xtupleEvt->addIndexedItem ("recHit", m_xt4nDigi, m_xt4recHit);
    } else {   // did not manage to book the N tuple....
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/evt"   << endmsg;
      //return;
    }
  }// end of book nt4

  //book tuple of segment
  NTuplePtr ntSeg(ntupleSvc(), "MdcxReco/seg");
  if ( ntSeg ) { m_xtupleSeg = ntSeg;}
  else {
    m_xtupleSeg = ntupleSvc()->book ("MdcxReco/seg", CLID_ColumnWiseTuple, "MdcxTrackFinder segment data");
    if ( m_xtupleSeg ) {
      sc = m_xtupleSeg->addItem ("sl",	          m_xtsSl);
      sc = m_xtupleSeg->addItem ("d0",	          m_xtsD0);
      sc = m_xtupleSeg->addItem ("omega",         m_xtsOmega);
      sc = m_xtupleSeg->addItem ("phi0",          m_xtsPhi0);
      sc = m_xtupleSeg->addItem ("d0sl",          m_xtsD0_sl_approx);
      sc = m_xtupleSeg->addItem ("phi0sl",        m_xtsPhi0_sl_approx);
      sc = m_xtupleSeg->addItem ("xbbrrf",        m_xtsXline_bbrrf);
      sc = m_xtupleSeg->addItem ("ybbrrf",        m_xtsYline_bbrrf);
      sc = m_xtupleSeg->addItem ("xslope",        m_xtsXline_slope);
      sc = m_xtupleSeg->addItem ("yslope",        m_xtsYline_slope);
      sc = m_xtupleSeg->addItem ("pat",           m_xtsPat);
      sc = m_xtupleSeg->addItem ("chisq",         m_xtsChisq);
      sc = m_xtupleSeg->addItem ("nDigi",         m_xtsNDigi, 0, 20);
      sc = m_xtupleSeg->addIndexedItem ("layer",  m_xtsNDigi, m_xtsLayer);
      sc = m_xtupleSeg->addIndexedItem ("wire",   m_xtsNDigi, m_xtsWire);
      sc = m_xtupleSeg->addIndexedItem ("inSeg",  m_xtsNDigi, m_xtsInSeg);
    }else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/evt"   << endmsg;
      //return;
    }
  }
  NTuplePtr nt5(ntupleSvc(), "MdcxReco/trkl");
  if ( nt5 ) { m_xtupleTrkl= nt5;}
  else {
    m_xtupleTrkl= ntupleSvc()->book ("MdcxReco/trkl", CLID_RowWiseTuple, "MdcxReco track info");
    if ( m_xtupleTrkl) {
      sc = m_xtupleTrkl->addItem ("layer",	 m_xt5Layer);
      sc = m_xtupleTrkl->addItem ("wire",	 m_xt5Wire);
    }else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/trkl"   << endmsg;
      //return;
    }
  }

  NTuplePtr ntCsmcSew(ntupleSvc(), "MdcxReco/csmc");
  if ( ntCsmcSew ) { m_xtupleCsmcSew = ntCsmcSew;}
  else {
    m_xtupleCsmcSew = ntupleSvc()->book ("MdcxReco/csmc", CLID_ColumnWiseTuple, "MdcxReco reconsturction results");
    if ( m_xtupleCsmcSew ) {
      sc = m_xtupleCsmcSew->addItem ("dD0",      m_csmcD0);
      sc = m_xtupleCsmcSew->addItem ("dPhi0",    m_csmcPhi0);
      sc = m_xtupleCsmcSew->addItem ("dZ0",      m_csmcZ0);
      sc = m_xtupleCsmcSew->addItem ("dOmega",   m_csmcOmega);
      sc = m_xtupleCsmcSew->addItem ("dPt",      m_csmcPt);
      sc = m_xtupleCsmcSew->addItem ("dTanl",    m_csmcTanl);
    }else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/csmc"   << endmsg;
      //return;
    }
  }
  NTuplePtr ntSegAdd1(ntupleSvc(), "MdcxReco/addSeg1");
  if ( ntSegAdd1 ) { m_xtupleAddSeg1 = ntSegAdd1;}
  else {
    m_xtupleAddSeg1 = ntupleSvc()->book ("MdcxReco/addSeg1", CLID_ColumnWiseTuple, "MdcxReco event data");
    if ( m_xtupleAddSeg1 ) {
      sc = m_xtupleAddSeg1->addItem ("same",	 m_addSegSame);
      sc = m_xtupleAddSeg1->addItem ("seedSl",	 m_addSegSeedSl );
      sc = m_xtupleAddSeg1->addItem ("seedPhi",	 m_addSegSeedPhi );
      sc = m_xtupleAddSeg1->addItem ("seedPhiLay",m_addSegSeedPhiLay );
      sc = m_xtupleAddSeg1->addItem ("seedLen",	 m_addSegSeedLen );
      sc = m_xtupleAddSeg1->addItem ("seedD0",	 m_addSegSeedD0 );
      sc = m_xtupleAddSeg1->addItem ("seedPhi0",	 m_addSegSeedPhi0 );
      sc = m_xtupleAddSeg1->addItem ("addSl",	 m_addSegAddSl );
      sc = m_xtupleAddSeg1->addItem ("addPhi",	 m_addSegAddPhi );
      sc = m_xtupleAddSeg1->addItem ("addPhiLay", m_addSegAddPhiLay );
      sc = m_xtupleAddSeg1->addItem ("addLen",	 m_addSegAddLen );
      sc = m_xtupleAddSeg1->addItem ("addD0",	 m_addSegAddD0 );
      sc = m_xtupleAddSeg1->addItem ("addPhi0",	 m_addSegAddPhi0 );
    }
    else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/addSeg1"   << endmsg;
      //return;
    }
  }

  NTuplePtr ntSegComb(ntupleSvc(), "MdcxReco/segComb");
  if ( ntSegComb ) { m_xtupleSegComb = ntSegComb;}
  else {
    m_xtupleSegComb = ntupleSvc()->book ("MdcxReco/segComb", CLID_ColumnWiseTuple, "MdcxReco event data");
    if ( m_xtupleSegComb ) {
      sc = m_xtupleSegComb->addItem ("evtNo",	m_segCombEvtNo);
      sc = m_xtupleSegComb->addItem ("omega",	m_segCombOmega);
      sc = m_xtupleSegComb->addItem ("sameAU",	m_segCombSameAU);
      sc = m_xtupleSegComb->addItem ("sameUV",	m_segCombSameUV);
      sc = m_xtupleSegComb->addItem ("dlenAU",	m_segCombDLenAU);
      sc = m_xtupleSegComb->addItem ("dlenUV",	m_segCombDLenUV);
      sc = m_xtupleSegComb->addItem ("slA",	m_segCombSlA);
      sc = m_xtupleSegComb->addItem ("slU",	m_segCombSlU);
      sc = m_xtupleSegComb->addItem ("slV",	m_segCombSlV);
      sc = m_xtupleSegComb->addItem ("phiA",	m_segCombPhiA);
      sc = m_xtupleSegComb->addItem ("phiU",	m_segCombPhiU);
      sc = m_xtupleSegComb->addItem ("phiV",	m_segCombPhiV);
    }
    else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/segComb"   << endmsg;
      //return;
    }
  }

  NTuplePtr ntDropHits(ntupleSvc(), "MdcxReco/dropHits");
  if ( ntDropHits ) { m_xtupleDropHits = ntDropHits;}
  else {
    m_xtupleDropHits = ntupleSvc()->book ("MdcxReco/dropHits", CLID_ColumnWiseTuple, "MdcxReco event data");
    if ( m_xtupleDropHits ) {
      sc = m_xtupleDropHits->addItem ("evtNo",	m_segDropHitsEvtNo);
      sc = m_xtupleDropHits->addItem ("layer",	m_segDropHitsLayer);
      sc = m_xtupleDropHits->addItem ("wire",	m_segDropHitsWire);
      sc = m_xtupleDropHits->addItem ("pull",	m_segDropHitsPull);
      sc = m_xtupleDropHits->addItem ("doca",	m_segDropHitsDoca);
      sc = m_xtupleDropHits->addItem ("sigma",	m_segDropHitsSigma);
      sc = m_xtupleDropHits->addItem ("drift",	m_segDropHitsDrift);
      sc = m_xtupleDropHits->addItem ("mcTkId",	m_segDropHitsMcTkId);
    } else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/dropHits"   << endmsg;
      //return;
    }
  }
  NTuplePtr ntAddSeg2(ntupleSvc(), "MdcxReco/addSeg2");
  if ( ntAddSeg2 ) { m_xtupleAddSeg2 = ntAddSeg2;}
  else {
    m_xtupleAddSeg2 = ntupleSvc()->book ("MdcxReco/addSeg2", CLID_ColumnWiseTuple, "MdcxReco event data");
    if ( m_xtupleAddSeg2 ) {
      sc = m_xtupleAddSeg2->addItem ("evtNo",	m_addSegEvtNo);
      sc = m_xtupleAddSeg2->addItem ("slayer",	m_addSegSlayer);
      sc = m_xtupleAddSeg2->addItem ("poca",	m_addSegPoca);
      sc = m_xtupleAddSeg2->addItem ("len",	m_addSegLen);
    } else{
      log << MSG::ERROR << "Cannot book N-tuple: MdcxReco/addSeg2"   << endmsg;
      //return;
    }
  }
  //--------------end of book ntuple------------------
}


void MdcxTrackFinder::fillEvent(){
  //-----------get raw digi-----------------------
  if (m_xtupleEvt == NULL ) return;
#ifdef MDCXTIMEDEBUG
  m_xt4time = m_timer[0]->elapsed();
  m_xt4timeSeg = m_timer[1]->elapsed();
  m_xt4timeTrack = m_timer[2]->elapsed();
  m_xt4timeFit = m_timer[3]->elapsed();
#endif
  m_xt4EvtNo = m_eventNo;
  m_xt4t0 = m_bunchT0;
  m_xt4t0Stat = m_t0Stat;
  m_xt4t0Truth = m_t0Truth;
  m_xt4nRecTk = nTk;
  m_xt4nTdsTk = t_nTdsTk;
  m_xt4nDigi = t_nDigi;
  m_xt4nSeg = t_nSeg;
  int iDigi=0;
  MdcDigiCol::iterator iter = mdcDigiVec.begin();
  for (;iDigi<t_nDigi; iter++ ) {
    int l = MdcID::layer((*iter)->identify());
    m_xt4Layer[iDigi] = l;
    //int w = MdcID::wire((*iter)->identify());
    m_xt4Time[iDigi] = RawDataUtil::MdcTime((*iter)->getTimeChannel());
    m_xt4Charge[iDigi] = RawDataUtil::MdcCharge((*iter)->getChargeChannel());
    //m_xt4rawHit[l]++;
    iDigi++;
  }//end for iter
  m_xtupleEvt->write();
}

void MdcxTrackFinder::fillTrack(TrkRecoTrk* atrack){

  if(!m_xtuple1) return;
  //-----------get MC truth data-------------------
  m_xevtNo = m_eventNo;
  int recHitMap[43][288]={0};
  //int nTk = (*m_tracks).nTrack();
  //for (int itrack = 0; itrack < nTk; itrack++) {
  if (atrack==NULL) return;

  const TrkFit* fitResult = atrack->fitResult();
  if (fitResult == 0) return;//check the fit worked

  //for fill ntuples
  int nSt=0; //int nSeg=0; 
  int seg[11] = {0}; int segme;
  //-----------hit list-------------
  HitRefVec  hitRefVec;	
  const TrkHitList* hitlist = atrack->hits();

  TrkHitList::hot_iterator hot = hitlist->begin();
  int layerCount[43]={0};
  int i=0;
  for (;hot!= hitlist->end();hot++){

    const MdcRecoHitOnTrack* recoHot;   		  
    recoHot = dynamic_cast<const MdcRecoHitOnTrack*> (&(*hot));
    int layer = recoHot->mdcHit()->layernumber();
    int wire  = recoHot->mdcHit()->wirenumber();
    m_xlayer[i]    = layer;
    //m_xt4recHit[layer]++;//fill rec hit for hit eff
    m_xwire[i]	    = wire;
    layerCount[layer]++;
    recHitMap[layer][wire]++;
    m_xambig[i]    = recoHot->wireAmbig();// wire ambig
    //fltLen
    double fltLen = recoHot->fltLen(); 
    m_xfltLen[i]   = fltLen;
    double tof    = recoHot->getParentRep()->arrivalTime(fltLen);
    //position
    HepPoint3D pos; Hep3Vector dir;    
    recoHot->getParentRep()->traj().getInfo(fltLen,pos,dir);
    m_xx[i] = pos.x();	     
    m_xy[i] = pos.y();	     
    m_xz[i] = pos.z();	     
    m_xdriftT[i]   = recoHot->mdcHit()->driftTime(tof,pos.z());
    m_xtof[i]	    = tof;
    m_xdriftD[i]   = recoHot->drift();
    m_xsigma[i]    = recoHot->hitRms();	    
    m_xtdc[i]	    = recoHot->rawTime();
    m_xadc[i]	    = recoHot->mdcHit()->charge(); 
    m_xdoca[i]	    = recoHot->dcaToWire();//sign w.r.t. dirft() FIXME 
    m_xentra[i]    = recoHot->entranceAngle();
    //m_xentraHit[i]    = recoHot->entranceAngleHit();
    m_xact[i] = recoHot->isActive();
    //resid
    double res=999.,rese=999.;
    if (recoHot->resid(res,rese,false)){
    }else{}
    m_xresid[i] = res;
    //for n seg
    segme=0;
    if ( layer >0 ) {segme=(layer-1)/4;}
    seg[segme]++;
    if (recoHot->layer()->view()) { ++nSt; }
    i++;
  }// end fill hit

  int nSlay=0;
  for(int i=0;i<11;i++){
    if (seg[i]>0) nSlay++;
  }
  m_xnSlay = nSlay;

  //------------fill track result-------------
  //m_xtkId = itrack;
  //track parameters at closest approach to beamline
  double fltLenPoca = 0.0;
  TrkExchangePar helix = fitResult->helix(fltLenPoca);
  m_xphi0 = helix.phi0();
  m_xtanl = helix.tanDip();
  m_xz0 = helix.z0();
  m_xd0 = helix.d0();
  if(fabs(m_xz0)>20||fabs(m_xd0)>2) {
    //b_saveEvent = true;
    if(m_debug>1) std::cout<<"evt:"<<m_eventNo<<" BigVtx:"
      <<" d0 "<<helix.d0()<<" z0 "<<helix.z0()<<std::endl;
  }
  m_xpt = fitResult->pt();
  if (m_xpt > 0.00001){
    m_xcpa = fitResult->charge()/fitResult->pt();
  }
  //momenta and position
  CLHEP::Hep3Vector p1 = fitResult->momentum(fltLenPoca);
  double px,py,pz,pxy;
  pxy = fitResult->pt();
  px = p1.x();
  py = p1.y();
  pz = p1.z();
  m_xp = p1.mag();
  m_xpz = pz;
  HepPoint3D poca = fitResult->position(fltLenPoca);
  m_xpocax = poca.x();
  m_xpocay = poca.y();
  m_xpocaz = poca.z(); 

  m_xq    = fitResult->charge();
  m_xnAct = fitResult->nActive();
  m_xnHit = hitlist->nHit();
  m_xnDof = fitResult->nDof();
  m_xnSt  = nSt;
  m_xchi2 = fitResult->chisq();
  //for (int l=0;l<43;l++) m_xlayerCount[l] = layerCount[l];
  m_xt0 = m_bunchT0;
  m_xtiming = m_timing;
  m_xt0Stat = m_t0Stat;
  m_xt0Truth= m_t0Truth;
  m_xtuple1->write();
  //}//end of loop rec trk list

}//end of 

void MdcxTrackFinder::dumpMdcxSegs(const HepAList<MdcxSeg>& segList) const {
  cout << name()<<" found " <<segList.length() << " segs :" << endl;
  for (int i =0; i< segList.length(); i++){
    std::cout<<i<<" ";
    segList[i]->printSeg();
  }
}

void MdcxTrackFinder::fillMdcxSegs(const HepAList<MdcxSeg>& segList) const {
  if(!m_xtupleSeg) return;

  int cellMax[43] ={
    40,44,48,56,     64,72,80,80,     76,76,88,88,
    100,100,112,112, 128,128,140,140, 160,160,160,160,
    176,176,176,176, 208,208,208,208, 240,240,240,240,
    256,256,256,256, 288,288,288 };
  // Fill hits of every layer after segment finding
  int hitInSegList[43][288];
  for (int ii=0;ii<43;ii++){
    for (int jj=0;jj<cellMax[ii];jj++){ hitInSegList[ii][jj] = 0; }
  }
  for (int i = 0; i < segList.length(); i++) {
    MdcxSeg* aSeg = segList[i];
    for (int ihit=0 ; ihit< aSeg->Nhits() ; ihit++){
      const MdcxHit* hit = aSeg->XHitList()[ihit];
      int layer = hit->Layer();
      int wire = hit->WireNo();
      hitInSegList[layer][wire]++;
    }
  }
  for (int i = 0; i < segList.length(); i++) {
    MdcxSeg* aSeg = segList[i];
    if(aSeg==NULL)continue;
    m_xtsSl = aSeg->SuperLayer();  
    m_xtsD0 = aSeg->D0();  
    m_xtsOmega = aSeg->Omega();  
    m_xtsPhi0 = aSeg->Phi0();  
    m_xtsD0_sl_approx = aSeg->D0_sl_approx();
    m_xtsPhi0_sl_approx = aSeg->Phi0_sl_approx();
    m_xtsXline_bbrrf = aSeg->Xline_bbrrf();
    m_xtsYline_bbrrf = aSeg->Yline_bbrrf();
    m_xtsXline_slope = aSeg->Xline_slope();
    m_xtsYline_slope = aSeg->Yline_slope();
    int patIndex = -1;
    for (int ii = 0;ii<14;ii++){
      if (aSeg->Pat() == (int) MdcxSegPatterns::patt4[ii]){
	patIndex = ii;
	break;
      }
    }
    for (int ii = 0;ii<20;ii++){
      if (aSeg->Pat() == (int) MdcxSegPatterns::patt3[ii]){
	patIndex = ii +15;
	break;
      }
    }
    m_xtsPat = patIndex;
    m_xtsChisq = aSeg->Chisq();
    m_xtsNDigi = aSeg->Nhits();
    for (int ihit=0 ; ihit< aSeg->Nhits() ; ihit++){
      const MdcxHit* hit = aSeg->XHitList()[ihit];
      int layer = hit->Layer();
      int wire = hit->WireNo();
      m_xtsLayer[ihit] = layer;
      m_xtsWire[ihit] = wire;
      m_xtsInSeg[ihit] = hitInSegList[layer][wire];
      //std::cout<< "hitInSegList   "<<hitInSegList[layer][wire] << std::endl;//yzhang debug
    }
    m_xtupleSeg->write();
  }

}//end of fillMdcxSegs

void MdcxTrackFinder::dumpTrackList(const HepAList<MdcxFittedHel>& trackList) const {
  std::cout<< "dump track list   " <<std::endl;
  for (int i=0;i<trackList.length();i++){
    std::cout<< "track  "<<i<<std::endl;
    for (int ihit=0 ; ihit< trackList[i]->Nhits() ; ihit++){
      const MdcxHit* hit = trackList[i]->XHitList()[ihit];
      int layer = hit->Layer();
      int wire = hit->WireNo();
      std::cout<< " ("<<layer << ","<< wire<<") ";
    }
    std::cout<<std::endl;
  }
}// end of dumpTrackList

void MdcxTrackFinder::fillTrkl(const HepAList<MdcxFittedHel>& firsttrkl) const {
  if(!m_xtupleTrkl) return;
  int nDigi = 0;
  int iDigi = 0;
  for (int i =0; i< firsttrkl.length(); i++){
    nDigi+= firsttrkl[i]->Nhits();
  }
  for (int i=0;i<firsttrkl.length();i++){
    for (int ihit=0 ; ihit< firsttrkl[i]->Nhits() ; ihit++){
      const MdcxHit* hit = firsttrkl[i]->XHitList()[ihit];
      int layer = hit->Layer();
      int wire = hit->WireNo();
      m_xt5Layer = layer;
      m_xt5Wire = wire;
      m_xtupleTrkl->write();
      iDigi++;
    }
  }
}//end of fillTrkl

void MdcxTrackFinder::fillMcTruth(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  //Initialize
  for (int ii=0;ii<43;ii++){
    for (int jj=0;jj<288;jj++){
      haveDigi[ii][jj] = -2;
    }
  }
  int nDigi = mdcDigiVec.size();
  for (int iDigi =0 ;iDigi<nDigi; iDigi++ ) {
    int l = MdcID::layer((mdcDigiVec[iDigi])->identify());
    int w = MdcID::wire((mdcDigiVec[iDigi])->identify());
    //haveDigi[l][w]=(mdcDigiVec[iDigi])->getTrackIndex();
    haveDigi[l][w]=1;
  }

  if( m_mcHist ){
    //------------------get event start time truth-----------
    m_t0Truth = -10.;
    SmartDataPtr<Event::McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
    if(!mcParticleCol){
      log << MSG::INFO << "Could not retrieve McParticelCol" << endreq;
    }else {
      Event::McParticleCol::iterator iter_mc = mcParticleCol->begin();
      for (;iter_mc != mcParticleCol->end(); iter_mc++){
	if ((*iter_mc)->primaryParticle()){
	  m_t0Truth = (*iter_mc)->initialPosition().t();
	  //px = (*iter_mc)->initialFourMomentum().x()/1000.;//GeV
	  //py = (*iter_mc)->initialFourMomentum().y()/1000.;//GeV
	  //pz = (*iter_mc)->initialFourMomentum().z()/1000.;//GeV
	}
      }
    }
  }
  //------------------Retrieve MC truth MdcMcHit------------
  /*
     SmartDataPtr<Event::MdcMcHitCol> mcMdcMcHitCol(eventSvc(),"/Event/MC/MdcMcHitCol"); 
     if (!mcMdcMcHitCol) {
     log << MSG::WARNING << "Could not find MdcMcHit" << endreq; 
     }else{
     Event::MdcMcHitCol::iterator iter_mchit = mcMdcMcHitCol->begin();
     for (;iter_mchit != mcMdcMcHitCol->end(); iter_mchit++ ) {
     const Identifier id= (*iter_mchit)->identify();
     int layer = MdcID::layer(id);
     int wire = MdcID::wire(id);
     mcDrift[layer][wire] = (*iter_mchit)->getDriftDistance();  //drift in MC.
     mcLR[layer][wire] = (*iter_mchit)->getPositionFlag();
     mcX[layer][wire] = (*iter_mchit)->getPositionX();
     mcY[layer][wire] = (*iter_mchit)->getPositionY();
     mcZ[layer][wire] = (*iter_mchit)->getPositionZ();
     if (mcLR[layer][wire] == 0) mcLR[layer][wire] = -1;
     }
     }
     */
}

void MdcxTrackFinder::dropMultiHotInLayer(TrkHitList* trkHitList){
  double tdr[43];
  double tdr_wire[43];
  for(int i=0; i<43; i++){tdr[i]=9999.;}

  // make flag
  TrkHotList::hot_iterator hotIter = trkHitList->hotList().begin();
  while (hotIter!=trkHitList->hotList().end()) {
    MdcHitOnTrack* hot = const_cast<MdcHitOnTrack*> (&(*hotIter->mdcHitOnTrack()));

    //driftTime(tof,z)
    double dt = hot->mdcHit()->driftTime(0.,0.);
    int layer = hot->mdcHit()->layernumber();
    int wire = hot->mdcHit()->wirenumber();

    //std::cout<<__FILE__<<" "<<dt<<  std::endl;
    if (dt < tdr[layer]) {
      tdr[layer] = dt;  
      tdr_wire[layer] = wire;  
    }
    hotIter++;
  }

  //std::cout<<" tdr wire ";
  //for(int i=0;i<43;i++){
  //std::cout<<i<<","<<tdr_wire[i]<<" tdr="<<tdr[i]<<"\n";
  //}
  // inactive multi hit 
  hotIter = trkHitList->hotList().begin();
  while (hotIter!=trkHitList->hotList().end()) {
    int layer = hotIter->mdcHitOnTrack()->mdcHit()->layernumber();
    int wire = hotIter->mdcHitOnTrack()->mdcHit()->wirenumber();
    //double dt = hotIter->mdcHitOnTrack()->mdcHit()->driftTime(0.,0.)-m_bunchT0;

    if ((tdr[layer] <9998.) && (tdr_wire[layer]!=wire)){
      MdcHitOnTrack* hot = const_cast<MdcHitOnTrack*> (&(*hotIter->mdcHitOnTrack()));
      hot->setActivity(false);
      trkHitList->removeHit( hotIter->mdcHitOnTrack()->mdcHit() );
      //std::cout<<__FILE__<<" inactive "<< layer<<" "<<wire<<" dt "<<dt <<  std::endl;
    }else{
      hotIter++;
    }
  }
}
void MdcxTrackFinder::dumpTdsTrack(RecMdcTrackCol* trackList){
  std::cout<< "tksize = "<<trackList->size() << std::endl;//yzhang debug
  RecMdcTrackCol::iterator it = trackList->begin();
  for (;it!= trackList->end();it++){
    RecMdcTrack *tk = *it;
    std::cout<< "//====RecMdcTrack "<<tk->trackId()<<"====:" << std::endl;
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

void MdcxTrackFinder::dumpTdsHits(RecMdcHitCol* hitList){
  std::cout<<__FILE__<<" "<<__LINE__<<" All hits in TDS, nhit="<<hitList->size()<<  std::endl;
  RecMdcHitCol::iterator it = hitList->begin();
  for(;it!= hitList->end(); it++){
    RecMdcHit* h = (*it);
    Identifier id(h->getMdcId());
    int layer = MdcID::layer(id);
    int wire = MdcID::wire(id);
    cout<<"("<< layer <<","<<wire<<") lr:"<<h->getFlagLR()<<" stat:"<<h->getStat()<<" tk:"<<h->getTrkId() <<" doca:"<<setw(10)<<h->getDoca()<<std::endl;
  }//end of hit list
}
