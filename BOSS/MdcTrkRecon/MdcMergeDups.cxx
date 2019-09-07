//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcMergeDups.cxx,v 1.3 2012/05/13 23:43:41 zhangy Exp $
//
// Description:
//	Class MdcMergeDups is to be merge duplicated track in MDC.
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//	Zhang Yao(zhangyao@ihep.ac.cn)  		Migrate to BESIII
//	Yoshi Iwasaki(yoshihito.iwasaki@kek.jp)		Overlap method
//	Jia Lukui(jialk@ihep.ac.cn)			Optimized overlap
//
// Copyright Information:
//	Copyright (C) 2009	IHEP	
// 
// History:
//	Zhang Yao 2009-10-30 
//		Migration BELLE TTrackManager::merge() for BESIII MDC
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcTrkRecon/MdcMergeDups.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcGeom/Constants.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkHitList.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkBase/TrkFit.h"
#include "MdcData/MdcHitMap.h"
#include "MdcData/MdcHitUse.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcData/MdcHitOnTrack.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkContextEv.h"
#include "EvTimeEvent/RecEsTime.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcTrkRecon/MdcTrack.h"


MdcMergeDups::MdcMergeDups(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  declareProperty("debug",		m_debug = 0);
  //cuts for mergeDups()
  declareProperty("maxDd0InMerge", 	m_maxDd0InMerge = 2.7);
  declareProperty("maxDphi0InMerge", 	m_maxDphi0InMerge = 0.15);
  declareProperty("maxDPdradInMerge", 	m_maxPdradInMerge= 0.22);
  declareProperty("maxRcsInMerge", 	m_maxRcsInMerge = 18.);
  //cuts for mergeCurl()
  declareProperty("mergePt",		m_mergePt = 0.13);
  declareProperty("mergeLoadAx", 	m_mergeLoadAx = 3.);
  declareProperty("mergeLoadSt", 	m_mergeLoadSt = 4.);
  declareProperty("mergeOverlapRatio", 	m_mergeOverlapRatio = 0.7);
}

//--------------
// Destructor --
//--------------
MdcMergeDups::~MdcMergeDups() {
  delete m_bfield;
}

StatusCode MdcMergeDups::beginRun(){  
  //Detector geometry 
  m_gm = MdcDetector::instance();
  if(NULL == m_gm) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}

//--------------
// Operations --
//--------------
StatusCode MdcMergeDups::initialize(){  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;	
  StatusCode sc;


  //Initailize magnetic filed 
  IMagneticFieldSvc* m_pIMF;
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
    return StatusCode::FAILURE;
  }
  m_bfield = new BField(m_pIMF);

  return StatusCode::SUCCESS;   
}


StatusCode MdcMergeDups::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  setFilterPassed(false); 

  m_bunchT0 = -999.;
  SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!aevtimeCol || aevtimeCol->size()==0) {
    log << MSG::WARNING<< " Could not find RecEsTimeCol"<< endreq;
    return StatusCode::SUCCESS;
  }

  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
    m_bunchT0 =  (*iter_evt)->getTest();
  }


  int nMerged = mergeCurl();

  if(m_debug>0) {
    std::cout<<name()<<": Merged "<<nMerged << " track "<<  std::endl;
    dumpRecMdcTrack();
  }

  return StatusCode::SUCCESS;   
}


StatusCode MdcMergeDups::finalize(){  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;	

  return StatusCode::SUCCESS;   
}


//-------------------------------------------------------------------
int MdcMergeDups::mergeCurl(){
  //-------------------------------------------------------------------

  SmartDataPtr<RecMdcTrackCol> trackList(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackList) return -1;

  int needMerge = 0;

  //...Merging. Search a track to be merged...
  RecMdcTrackCol::iterator iterRefTk = trackList->begin();
  for (; iterRefTk != trackList->end(); iterRefTk++) {
    RecMdcTrack* refTk = *iterRefTk;
    if (refTk->stat()<0) continue;
    std::vector<RecMdcTrack*> mergeTkList;
    mergeTkList.push_back(refTk);


    bool curl = false;
    int sameParm = 0;
    RecMdcTrackCol::iterator iterTestTk = trackList->begin();
    for (; iterTestTk != trackList->end(); iterTestTk++) {
      RecMdcTrack* testTk = *iterTestTk;
      if (iterRefTk == iterTestTk || (testTk->stat()<0)) continue;

      //-- overlapRatio cut 0.7 by jialk, original is 0.8
      if (testByOverlapHit(refTk,testTk)){
	if(m_debug>0)std::cout<<__FILE__<<" overlape tk:" <<refTk->trackId()<<" with "<<testTk->trackId()<<  std::endl;
	mergeTkList.push_back(testTk);
	curl = true;
      }
      sameParm = testByParam(refTk,testTk);
      if(sameParm >0) {
	if(m_debug>0) std::cout<<__FILE__<<" same param tk:" <<refTk->trackId()<<" with "<<testTk->trackId()<<  std::endl;
	mergeTkList.push_back(testTk);
      }
    } 
    if (mergeTkList.size()>1 && curl) needMerge = doMergeCurl(mergeTkList); 
    if ((needMerge < 999) && mergeTkList.size()>1 ) needMerge = doMergeLong(mergeTkList); 
    //if ((needMerge <999) && mergeTkList.size()==2 && (sameParm==2) ) needMerge = doMergeOdd(mergeTkList); //FIXME
  }

  //return 0 if No track need merged
  if( needMerge <=0 ) return 0;

  // reset track Id
  iterRefTk = trackList->begin();
  int iTk=0;
  int nDeleted = 0;
  for (; iterRefTk != trackList->end(); ) {
    if ( (*iterRefTk)->stat() >= 0 ){
      (*iterRefTk)->setTrackId(iTk);
      iterRefTk++;
      iTk++;
    }else {
      int id = (*iterRefTk)->trackId();
      bool erased = eraseTdsTrack(iterRefTk);
      if ( erased ){ 
	nDeleted++;
	if(m_debug>0)std::cout<<__FILE__<<" erase track No."<<id<<  std::endl;
      }else {
	if(m_debug>0)std::cout<<__FILE__<<" erase failed !"<<  std::endl;
      }
    }

  }
  if(m_debug>0) std::cout<<__FILE__<<" After merge save "<<iTk<<" tracks"<<  std::endl;

  return nDeleted;
}

//-------------------------------------------------------------------
// Test by track parameters of 2 track, if overlaped return true
// return 0: not duplicates, 1: same charge, 2: odd charge
int MdcMergeDups::testByParam(RecMdcTrack* refTk, RecMdcTrack* testTk){
  //-------------------------------------------------------------------
  int overlaped = 0;


  //Convert to Babar track convension
  double Bz = m_bfield->bFieldZ();
  double omega1 = (Constants::c * Bz*refTk->helix(2))/10000.;
  double omega2 = (Constants::c * Bz*testTk->helix(2))/10000.;
  //phi0_babar = phi0_belle + pi/2   [0,2pi)
  double phi01  = refTk->helix(1)+Constants::pi/2.;
  double phi02  = testTk->helix(1)+Constants::pi/2.;
  while(phi01>Constants::twoPi) phi01 -= Constants::twoPi;
  while(phi02>Constants::twoPi) phi02 -= Constants::twoPi;
  double d01    = -refTk->helix(0);
  double d02    = -testTk->helix(0);
  double dphi0  = fabs(phi01 - phi02);
  double dd0    = fabs(d01 - d02);
  double prodo  = omega1*omega2;
  double r1=100000.;
  double r2=100000.;
  if (fabs(omega1)>0.00001) r1 = 1.0/fabs(omega1);
  if (fabs(omega2)>0.00001) r2 = 1.0/fabs(omega2); //FIXME
  double pdrad = fabs((r1-r2)/(r1+r2)) ;

  if (2==m_debug){
    std::cout << "  fabs(d01 - d02)  " << fabs(d01 - d02) << std::endl;
    std::cout << "  fabs(phi01-phi02)  " << fabs(phi01-phi02) << std::endl;
  }
  //  Try to merge pair that looks like duplicates (same charge)
  if ( (prodo > 0.) && (dd0 < m_maxDd0InMerge) && (dphi0 < m_maxDphi0InMerge) &&
      (pdrad < m_maxPdradInMerge)) {
    overlaped = 1;
  }

  //  Try to merge pair that looks like albedo (opp charge, large d0)
  if ( (prodo < 0.) && (fabs(d01+d02) < 4.0) && (dd0 > 47.0) &&
      (fabs( dphi0 - Constants::pi) < m_maxDphi0InMerge) 
      && (pdrad < m_maxPdradInMerge)) {
    overlaped = 2;
  }

  return overlaped;
}

//-------------------------------------------------------------------
//return best tk id
//if failed return 999
int MdcMergeDups::doMergeLong(std::vector<RecMdcTrack*> mergeTkList){
  //-------------------------------------------------------------------
  //merge hitlist
  double minRcs=999.;
  int bestTkId=999;
  RecMdcTrack* bestTk=NULL;
  std::vector<RecMdcTrack*>::iterator itTk = mergeTkList.begin();
  for (int iTk=0; itTk != mergeTkList.end(); itTk++,iTk++){
    RecMdcTrack* tk = (*itTk);
    double chi2 = tk->chi2();
    double ndf  = tk->ndof();
    if(chi2/ndf < minRcs) {
      bestTkId = tk->trackId(); 
      bestTk = tk;
    }
  }
  if (minRcs < m_maxRcsInMerge) return bestTkId;
  bestTk->setStat(-1);

  return 999;
  //FIXME
  /*
  //fit with track parameter respectively
  MdcxFittedHel fit1(dcxhlist, *iptr);
  MdcxFittedHel fit2(dcxhlist, *trkl[j]); 
  int uf = 0; 
  //get a best fit 
  if ( !fit1.Fail() && (fit1.Rcs()<m_maxRcsInMerge) ) uf = 1; 
  if ( !fit2.Fail() && (fit2.Rcs()<fit1.Rcs()) ) uf = 2;

  if (uf) {//two fit all ok
  //delete bad track
  MdcxHel fitme = (uf == 1) ? fit1 : fit2;
  }
  */
}


//-------------------------------------------------------------------
//int MdcMergeDups::doMergeOdd(std::vector<RecMdcTrack*> mergeTkList){
  //-------------------------------------------------------------------
  /*
  //odd charged track 
  if (2==m_debug)
  cout << "MdcxMD i j sum " << i << " " << j << " " << d01+d02 << " "
  << deltap << " " << r1 << " " << r2 << " " << pdrad << endl;
  //merge list
  //fit
  MdcxFittedHel fit1(dcxhlist, temp1); // fit1.FitPrint(); fit1.print();
  MdcxFittedHel fit2(dcxhlist, temp2); // fit2.FitPrint(); fit2.print();
  int uf = 0; 
  if ( !fit1.Fail() && (fit1.Rcs()<m_maxRcsInMerge) ) uf = 1; 
  if ( !fit2.Fail() && (fit2.Rcs()<fit1.Rcs()) ) uf = 2;
  if (uf) {
  MdcxHel fitme = (1 == uf) ? fit1 : fit2;
  MdcxFittedHel* finehel = new MdcxFittedHel(dcxhlist, fitme);
  if (!finehel->Fail()) {
  if (already_merged) {
  NewTrklist.replace(iptr, finehel);
  delete iptr;
  iptr = finehel;
  trkl[j]->SetUsedOnHel(already_merged);
  } else {
  NewTrklist.append(finehel);
  already_merged = NewTrklist.length();
  iptr->SetUsedOnHel(already_merged);
  iptr = finehel; 
  trkl[j]->SetUsedOnHel(already_merged);
  }
  } else {
  delete finehel;
  }
  } 
  */
//}

//-------------------------------------------------------------------
// Test by overlap hit rate of 2 track, if overlaped return 1, else return 0
int MdcMergeDups::testByOverlapHit(RecMdcTrack* refTk, RecMdcTrack* testTk){
  //-------------------------------------------------------------------
  int overlaped = 0;
  if ((testTk->pxy() >= m_mergePt) || (refTk->pxy() >= m_mergePt)) return overlaped;

  HitRefVec testHits = testTk->getVecHits();
  int nHit = testHits.size();
  int nOverlap = 0;

  HitRefVec::iterator iterHit = testHits.begin();
  for (; iterHit != testHits.end(); iterHit++) {
    RecMdcHit* hit = *iterHit;

    //-- load for Axial and Stereo layer are 3,4 by jialk, original is 2,3
    double load = m_mergeLoadAx; 
    bool isStLayer = (m_gm->Layer(MdcID::layer(hit->getMdcId()))->view() != 0);
    if(isStLayer) load = m_mergeLoadSt;

    //helix parameters
    double vx0  = refTk->getVX0();
    double vy0  = refTk->getVY0();
    double vz0  = refTk->getVZ0();
    double dr   = refTk->helix(0);
    double phi0 = refTk->helix(1);
    double Bz = m_bfield->bFieldZ();
    double r    = 10000./ (Constants::c * Bz*refTk->helix(2));
    double dz   = refTk->helix(3);
    double tanl = refTk->helix(4);

    //center of circle
    double xc = vx0 + (dr + r) * cos(phi0);
    double yc = vy0 + (dr + r) * sin(phi0);

    //position of hit
    double zHit = hit->getZhit();
    double phi  = (vz0 + dz - zHit) / (r * tanl);
    double xHit = vx0 + dr*cos(phi0) + r*(cos(phi0) - cos(phi0+phi));
    double yHit = vy0 + dr*sin(phi0) + r*(sin(phi0) - sin(phi0+phi));

    //distance from center of circle to hit
    double dx = xc - xHit;
    double dy = yc - yHit;
    double dHit2Center = sqrt(dx * dx + dy * dy);
    double rTk = fabs(r);

    //is this hit overlaped ? 
    if ( (dHit2Center>(rTk - load)) && (dHit2Center<(rTk + load))) nOverlap++;
  }

  if ( nOverlap<=0 ) return overlaped;

  double overlapRatio = double(nOverlap) / double(nHit);

  if (overlapRatio > m_mergeOverlapRatio) overlaped = 1;

  return overlaped;
}

//-------------------------------------------------------------------
int MdcMergeDups::doMergeCurl(std::vector<RecMdcTrack*> mergeTkList){
  //-------------------------------------------------------------------
  int innerMostTkId = 999;
  RecMdcTrack* innerMostTk = NULL;
  unsigned innerMostLayerOfTk = 999;
  std::vector<RecMdcTrack*>::iterator itTk = mergeTkList.begin();
  for (int iTk=0; itTk != mergeTkList.end(); itTk++,iTk++){
    RecMdcTrack* tk = (*itTk);
    unsigned innerMostLayer = 999;
    for (unsigned iHit = 0; iHit < tk->getVecHits().size(); iHit++) {
      unsigned layer = MdcID::layer(tk->getVecHits()[iHit]->getMdcId());
      if (layer < innerMostLayer) innerMostLayer=layer; 
    }

    if(m_debug>0)std::cout<<__FILE__<<" to be merged track id="<<tk->trackId()<<  std::endl;
    // test inner most layer id; if same, test dz
    if(innerMostLayer < innerMostLayerOfTk){
      innerMostTkId = iTk;
      innerMostTk = tk;
    }else if (innerMostLayer == innerMostLayerOfTk) {
      // test by dz
      if (tk->helix(3) < innerMostTk->helix(3)){
	innerMostTkId = iTk;
	innerMostTk = tk;
      }
    }
  }//end of for mergeTkList
  innerMostTk->setStat(-1);

  return innerMostTkId;
}

void MdcMergeDups::store(TrkRecoTrk* aTrack){
  SmartDataPtr<RecMdcTrackCol> trackList(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackList) return;
  SmartDataPtr<RecMdcHitCol> hitList(eventSvc(),EventModel::Recon::RecMdcHitCol);
  if (!hitList) return;

  assert (aTrack != NULL);
  TrkExchangePar helix = aTrack->fitResult()->helix(0.);

  if(m_debug>1)std::cout<<__FILE__<<" STORED"<<  std::endl;
  MdcTrack mdcTrack(aTrack);//aTrack have been deleted in ~MdcTrack() 
  //tkStat: 0,Tsf 1,CurlFinder 2,PatRec 3,MdcxReco 4,MergeCurl
  int tkStat = 4;
  mdcTrack.storeTrack(-1, trackList, hitList, tkStat);
} 

bool MdcMergeDups::eraseTdsTrack(RecMdcTrackCol::iterator tk){
  SmartDataPtr<RecMdcTrackCol> trackList(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackList) return false;
  SmartDataPtr<RecMdcHitCol> hitList(eventSvc(),EventModel::Recon::RecMdcHitCol);
  if (!hitList) return false;
  HitRefVec hits = (*tk)->getVecHits();
  HitRefVec::iterator iterHit = hits.begin();
  for (; iterHit != hits.end(); iterHit++) {
    //hitList->erase(iterHit);
  }
  trackList->erase(tk);
  return true;
}


void MdcMergeDups::dumpRecMdcTrack(){
  SmartDataPtr<RecMdcTrackCol> trackList(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackList) return;
  if (trackList->size() != 4 ) setFilterPassed(true);
  std::cout<<"N track after Merged = "<<trackList->size() << std::endl;
  if (m_debug <=1) return;
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
    //std::cout<< "errmat   " << std::endl;
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
}
