//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackList.cxx,v 1.25 2017/02/22 05:53:33 zhangy Exp $
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
// 	Steve Schaffner
// 	Zhang Xueyao(zhangxy@hepg.sdu.edu.cn) Migration for BESIII
// 	Zhang Yao(zhangyao@ihep.ac.cn) 
//
//--------------------------------------------------------------------------
#include <math.h>
#include <iostream>
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include "CLHEP/Alist/AList.h" 
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcRawEvent/MdcDigi.h"
#include "MdcData/MdcHitMap.h"
#include "MdcData/MdcHit.h"  
#include "MdcGeom/MdcDetector.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcTrackList.h"
#include "MdcTrkRecon/mdcWrapWire.h"
#include "MdcTrkRecon/MdcTrackParams.h"
#include "MdcTrkRecon/MdcSegGrouperSt.h"
#include "MdcTrkRecon/MdcSegGrouperAx.h"
#include "MdcTrkRecon/MdcSegInfo.h"  
#include "MdcTrkRecon/MdcSegInfoAxialO.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/GmsList.h"
#include "TrkBase/TrkRecoTrk.h"
#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkHotList.h"
#include "TrkBase/TrkFit.h"
#include "TrkBase/TrkFitStatus.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkErrCode.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkCircleMaker.h"
#include "MdcGeom/Constants.h"
#include "BField/BField.h"

//yzhang hist cut
#include "GaudiKernel/NTuple.h"
#include "AIDA/IHistogram1D.h"
#include "Rtypes.h"
extern int haveDigiTk[43][288];
extern double haveDigiDrift[43][288];
extern AIDA::IHistogram1D*  g_cirTkChi2;
extern AIDA::IHistogram1D*  g_3dTkChi2;
extern AIDA::IHistogram1D*  g_fitNAct;
extern AIDA::IHistogram1D*  g_pickHitWire;
//extern AIDA::IHistogram2D*  g_predDriftVsMcTk;
extern NTuple::Tuple*  m_tuplePick;
extern NTuple::Item<long>               m_pickIs2D;
extern NTuple::Item<long>               m_pickLayer;
extern NTuple::Item<long>               m_pickNCell;
extern NTuple::Item<long>               m_pickNCellWithDigi;
extern NTuple::Array<long>              m_pickWire;
extern NTuple::Array<double>            m_pickPredDrift;
extern NTuple::Array<double>            m_pickDrift;
extern NTuple::Array<double>            m_pickDriftTruth;
extern NTuple::Array<int>               m_pickPhizOk;
extern NTuple::Array<double>            m_pickZ;
extern NTuple::Array<double>            m_pickResid;
extern NTuple::Array<double>            m_pickSigma;
extern NTuple::Array<double>            m_pickPhiLowCut;
extern NTuple::Array<double>            m_pickPhiHighCut;
extern NTuple::Array<double>            m_pickDriftCut;
extern NTuple::Array<long>              m_pickMcTk;
extern NTuple::Array<double>            m_pickPt;
extern NTuple::Array<double>            m_pickCurv;

//zhangy hist cut

#ifdef MDCPATREC_TIMETEST
// tau include
#include <Profile/Profiler.h>
#include <sys/time.h>
#include <pthread.h>
#endif
//zhangy hist
using std::cout;
using std::endl;

// *************************************************************************
MdcTrackList::MdcTrackList(const MdcTrackParams &tkPar ) : 
  MdcTrackListBase(tkPar) { 
    // *************************************************************************
    return;
  }

//------------------------------------------------------------------------
MdcTrackList::~MdcTrackList() {}
//------------------------------------------------------------------------

// *************************************************************************
int 
MdcTrackList::createFromSegs(MdcSegList *segs, const MdcHitMap* map, 
    const MdcDetector* gm, 
    TrkContext& context, double bunchTime) {
  // *************************************************************************
  //Forms tracks out of list of superlayer segments
  int nTracks = 0;

  m_debug = tkParam.lPrint;//yzhang debug

  // Create empty list of stereo segs (to save time)
  MdcSegGrouperSt stereoSegs(gm, tkParam.lPrint);
  stereoSegs.setNoInner(m_noInner);//yzhang add 2016-10-12 

  // *** Create r-phi track

  // Create list of axial segments, treated as on tracks from origin
#ifdef MDCPATREC_TIMETEST
  TAU_PROFILE_TIMER(timer8,"fill ax seg", "int ()", TAU_DEFAULT);
  TAU_PROFILE_START(timer8);
#endif
  MdcSegGrouperAx origSegs(gm, tkParam.lPrint);
  origSegs.fillWithSegs(segs);
  //std::cout << "Plot segs after ax fillWithSegs " << std::endl;//yzhang debug 
  //segs->plot(0);//yzhang debug
#ifdef MDCPATREC_TIMETEST
  TAU_PROFILE_STOP(timer8);
#endif
  MdcSeg *seed;
  bool goodOnly = true;
  bool useBad = (segs->count() < 400);  // if true, use non-good seeds 
  //bool useBad = false;
  segs->resetSeed(gm); 
  MdcTrack *trialTrack = 0;

  while (1) {
    seed = segs->getSeed(0,goodOnly);
    if (seed == 0 && goodOnly && useBad) {
      segs->resetSeed(gm);
      goodOnly = false;
      continue;
    }
    else if (seed == 0 && (!goodOnly || !useBad)) {
      break;
    }

    if (3 == m_debug) dumpSeed(seed, goodOnly);//yzhang debug

    // Pass through first superlayer of MDC required, Field layer No5 = 12.135 cm
    if ( fabs( ((MdcSegInfoAxialO *) seed->info())->curv()) > 0.0417) continue;//curv cut yzhang 
    delete trialTrack;
    trialTrack = 0;

    //---------Combine Ax segs--------
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_TIMER(timer3,"combine ax seg", "int ()", TAU_DEFAULT);
    TAU_PROFILE_START(timer3);
#endif
    int success = origSegs.combineSegs(trialTrack, seed, context, bunchTime, 
	tkParam.maxSegChisqO);
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_STOP(timer3);
#endif


    if (3 <= m_debug){
      cout<<" ***** Ax.combineSegs success?  " << success<<"\n";
      dumpAxCombine(trialTrack);//yzhang debug
    }
    if (!success) continue;     


    //--------Finish circle-------- 
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_TIMER(timer4,"circle fitting", "int ()", TAU_DEFAULT);
    TAU_PROFILE_START(timer4);
#endif
    success = finishCircle(*trialTrack, map, gm);
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_STOP(timer4);
#endif

    if (3 <= m_debug){
      cout<<"finishCircle success?  " << success<<"\n";
      dumpCircle(trialTrack);//yzhang debug
    }

    if (!success) continue;
    //--------Make sure it really did come from origin--------
    const TrkFit* tkFit = trialTrack->track().fitResult();
    assert (tkFit != 0);
    TrkExchangePar par = tkFit->helix(0.0);
    //dumpD0(par);

    //------------------d0 cut-------------------
    // 2010-03-31 , m_d0Cut from 0 to epsilon

    //std::cout<<  __FILE__ <<"  d0 cut------------"<< fabs(par.d0())<<" d0cut "<< m_d0Cut << std::endl;
    if ( (m_d0Cut > Constants::epsilon) && (fabs(par.d0()) > m_d0Cut) ){
      if (tkParam.lPrint>1)cout<<__FILE__<<" Killing track by d0:" <<par.d0()<<">"<<m_d0Cut << endl;
      continue;
    }

    //------------------pt cut-------------------
    if ( (fabs(m_ptCut)>0.) && (fabs(tkFit->pt(0.)) < m_ptCut) ){
      if (tkParam.lPrint>1)cout<<__FILE__<<" Killing track by pt:" <<tkFit->pt(0.)<<"<"<<m_ptCut << endl;
      continue;
    }

    // *** End r-phi track-finding
    if (3 <= m_debug) std::cout << " *** End r-phi track-finding "<<std::endl;

    //--------Add stereo to taste--------
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_TIMER(timer5,"fill st seg", "int ()", TAU_DEFAULT);
    TAU_PROFILE_START(timer5);
#endif
    stereoSegs.fillWithSegs(segs, trialTrack);
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_STOP(timer5);
#endif
    if (3 <= m_debug){
      //dumpStFill();//yzhang debug
      std::cout<<std::endl<<"----------------------------------------"<<  std::endl;
      std::cout<<" Segment list after St.fillWithSegs "<<  std::endl;
      stereoSegs.dumpSegList();
    }

#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_TIMER(timer6,"combine st seg", "int ()", TAU_DEFAULT);
    TAU_PROFILE_START(timer6);
#endif
    success = stereoSegs.combineSegs(trialTrack, 0, context, bunchTime, 
	tkParam.maxSegChisqO, tkParam.combineByFitHits);
#ifdef MDCPATREC_TIMETEST
    TAU_PROFILE_STOP(timer6);
#endif

    if (3 <= m_debug){
      cout<<" ***** St.combineSegs success?  " << success<<"\n";
      dumpStCombine(trialTrack);
    }


    //--------Finish 3-d track--------
    if (success) {
#ifdef MDCPATREC_TIMETEST
      TAU_PROFILE_TIMER(timer7,"helix fitting", "int ()", TAU_DEFAULT);
      TAU_PROFILE_START(timer7);
#endif
      success = finishHelix(*trialTrack, map, gm);
#ifdef MDCPATREC_TIMETEST
      TAU_PROFILE_STOP(timer7);
#endif
    }  // end if (success -- st)

    if (3 == m_debug){
      dumpHelix(trialTrack);
    }
    if (!success) continue;

    //------------------d0 cut after helix fitting-------------------
    //yzhang add 2011-08-01 
    double d0par = trialTrack->track().fitResult()->helix(0.).d0();
    if ( (m_d0Cut > Constants::epsilon) && (fabs(d0par) > m_d0Cut) ){
      if (tkParam.lPrint>1) {cout<<__FILE__
	<<" Killing track by d0 after 3d fit:" <<d0par<<">"<<m_d0Cut << endl;}
      continue;
    }

    double z0par = trialTrack->track().fitResult()->helix(0.).z0();
    if ( (m_z0Cut > Constants::epsilon) && (fabs(z0par) > m_z0Cut) ){
      if (tkParam.lPrint>1) {cout<<__FILE__
	<<" Killing track by z0:" <<z0par<<">"<<m_z0Cut << endl;}
      continue;
    }


    nTracks++;
    append(trialTrack);    // Add to list of Tracks

    trialTrack = 0;

    if (3 == m_debug) std::cout << " ***** End one track-finding *****"<<std::endl;
  }  // end while(1)   

  delete trialTrack;
  return nTracks;

}


/***************************************************************************/
int 
MdcTrackList::pickHits(MdcTrack *trk, const MdcHitMap* map, 
    const MdcDetector* gm, bool pickAm) {

  /***************************************************************************/

  // Selects candidate hits along track; 
  // sorts them into "active" (small residual) and "inactive" (large resid);
  // throws away hits separated from main group by large gaps. //?? FIXME
  // pickAm => pick the ambiguity for hits already on track
  // Return # of active hits found

  bool is2d = trk->track().status()->is2d();
  if(6==tkParam.lPrint){
    cout << "Before pickHits";
    if (is2d) cout<<" 2d:";
    else cout<<" 3d:";
    cout<< endl;
  }

  int nFound = 0;
  int nCand = 0;        // cells tried 
  double rMin, rMax;    // min & max search radii for this track
  double rEnter, rExit;    // radii at which track enters, exits layer
  BesAngle phiEnter, phiExit;//yzhang change
  int wireLow, wireHigh;
  double phiLow, phiHigh;
  int nCell;
  MdcHit *thisHit;
  HepAList<MdcHitOnTrack> localHistory;    //temporary list of added hits
  //until bogus hits are chucked
  double cellWidth;
  double goodDriftCut;  // missing hits with predDrift > goodDriftCut don't count in figuring gaps
  double aresid = 0.;    // = abs(resid)
  int firstInputHit = -1; //first hit in firstInputLayer/lastInputLayer region
  int lCurl = 0;   // reached curl point

  //****************************************************/
  const MdcLayer *firstInputLayer = trk->firstLayer();
  const MdcLayer *lastInputLayer = trk->lastLayer();

  double bunchTime = trk->track().trackT0();
  const TrkFit* tkFit = trk->track().fitResult();
  assert (tkFit != 0);
  const TrkFitStatus* tkStatus = trk->track().status();
  assert (tkStatus != 0);
  TrkHitList* hitList = trk->track().hits();
  assert (hitList != 0);
  TrkExchangePar par = tkFit->helix(0.0);
  double d0 = par.d0();
  double curv = 0.5 * par.omega();   //!!! change to using omega itself
  double sinPhi0 = sin(par.phi0());
  double cosPhi0 = cos(par.phi0());

  // *** Set min and max radius for track
  rMin = gm->firstLayer()->rIn();   
  double absd0 = fabs(d0);
  if (absd0 > rMin) rMin = absd0 + Constants::epsilon;

  bool willCurl = false;
  double rCurl = fabs(d0 + 1./curv);
  rMax = gm->lastLayer()->rOut(); 
  //std::cout<<  __FILE__ <<"  "<< __LINE__<<" rCurl "<< rCurl <<" rMax "<< rMax << std::endl;
  if (rCurl < rMax) {
    willCurl = true;
    rMax = rCurl - Constants::epsilon;
  }
  //std::cout<<" willCurl "<< willCurl  << std::endl;
  bool reachedLastInput = false;
  bool hasCurled = false;  // hit found past curl point

  //yzhang add skip layer with hot, 2011-05-03 
  bool isHotOnLayer[43];
  if(tkParam.pickSkipExistLayer){
    for(int ii=0; ii<43; ii++) isHotOnLayer[ii]=false;
    for (TrkHitList::hot_iterator ihit(hitList->begin()); ihit != hitList->end(); ++ihit){
      isHotOnLayer[ihit->layerNumber()]=true;
    }
  }

  // *** Loop through layers in view, looking for the hits
  // assumes axial inner
  for (const MdcLayer *layer = gm->firstLayer(); layer != 0; 
      layer = ((!lCurl) ? ( (hasCurled) ? gm->prevLayer(layer) : 
	  gm->nextLayer(layer)) : layer) ) {  


    if (lCurl) {
      lCurl = 0;
      hasCurled = true;
    }
    if (tkStatus->is2d() && layer->view() != 0) continue;

    if(tkParam.pickSkipExistLayer &&  isHotOnLayer[layer->layNum()]) continue;//yzhang add 2011-05-03 

    //std::cout<<  __FILE__ <<"  "<< __LINE__  << " lCurl "<< lCurl
    //<<" hasCurled "<< hasCurled <<" layer "<< layer->layNum() << std::endl;
    bool lContinue = true;
    if(tkParam.pickUitlLastLayer) {//yzhang change 2010-09-10 
      if (layer == lastInputLayer) reachedLastInput = true; 
    }

    // *** Find enter and exit points
    if (hasCurled) {
      rEnter = layer->rOut();
      if (rEnter < rMin) {
	//std::cout<<  __FILE__ <<"  "<< __LINE__  
	//<<" rEnter<rMin "<<rEnter<<" "<<rMin<< std::endl;
	break;
      }
      rExit = layer->rIn();
      if (rExit < rMin) rExit = rMin;
      if (rEnter > rMax) rEnter = rMax;

      //std::cout<<  __FILE__ <<" hasCurled: rEnter  "<< rEnter 
      //<<" rExit " << rExit << " rMin "<<rMin<<" rMax "<<rMax<< std::endl;
    } else {
      rEnter = layer->rIn();
      rExit = layer->rOut();
      //std::cout<<  __FILE__ <<" NOT hasCurled: rEnter  "<< rEnter 
      //<<" rExit " << rExit << " rMin "<<rMin<<" rMax "<<rMax<< std::endl;
      if (rExit < rMin) continue;

      if (willCurl) {
	if (rEnter > rMax) {
	  //std::cout<<  __FILE__<<" Reached curl point before entering layer"<< std::endl;
	  // Reached curl point before entering layer
	  hasCurled = 1;
	  continue;
	}
	if (rExit > rMax) {
	  lCurl = 1;
	  rExit = rMax;
	}
      } else {  // not a potential curler
	//std::cout<<  __FILE__ <<"  "<< __LINE__  <<" not a potential curler"<< std::endl;
	if (rEnter > rMax) {
	  //std::cout<<  __FILE__ <<" rEnter> rMax "<< rEnter << std::endl;
	  break;
	}
	if (rExit > rMax) rExit = rMax;
      }
    }  // end if curled already

    nCell = layer->nWires();
    cellWidth = Constants::twoPi * layer->rMid() / nCell;//??
    // don't count outer xmm of cell
    goodDriftCut = 0.5 * cellWidth * M_SQRT2 + tkParam.pickHitMargin;  
    //goodDriftCut = 0.5 * cellWidth - tkParam.pickHitMargin;//yzhang change 2012-08-17 
    double deltaPhiCellWidth = 0.5 * (cellWidth * M_SQRT2)/layer->rMid(); 

    //**** Find entrance and exit phi's
    BesAngle phiTemp(0.0);
    int ierror = trk->projectToR(rEnter, phiTemp, hasCurled);
    phiEnter = phiTemp.posRad();
    if (ierror != 0) {
      if(6==tkParam.lPrint) std::cout<< " ErrMsg(warning) " 
	<< "Error in MdcTrackList::pickHits projection, ierror " <<ierror<< std::endl;
      continue;//yzhang 2011-04-14 
    }
    ierror = trk->projectToR(rExit, phiTemp, hasCurled);
    phiExit = phiTemp.posRad();
    if (ierror != 0) {
      if(6==tkParam.lPrint) std::cout<< " ErrMsg(warning) " 
	<< "Error in MdcTrackList::pickHits projection, ierror "<<ierror << std::endl;
      continue;//yzhang 2011-04-14 
    }


    if(6==tkParam.lPrint){
      std::cout<< endl<<"--pickHit of layer "<<layer->layNum()<<"--"<<std::endl;
      std::cout<< "  track phiEnter "<< phiEnter.deg()<<" phiExit "<<phiExit.deg()<<" degree"<< std::endl;
      std::cout<< "  cell width "<< 360./layer->nWires()<<" dPhiz "<<layer->dPhiz()*Constants::radToDegrees <<" deltaPhiCellWidth "<<0.5 * (cellWidth * M_SQRT2)/layer->rMid() * Constants::radToDegrees<<std::endl;
      std::cout<< "  maxInactiveResid "<< tkParam.maxInactiveResid <<" pickHitPhiFactor "<<tkParam.pickHitPhiFactor<< std::endl;
      std::cout<< "  goodDriftCut "<< goodDriftCut <<"=sqrt(2)*0.5*"<<cellWidth<<"+"<<tkParam.pickHitMargin<< std::endl;
    }

    double Bz = trk->track().bField().bFieldZ();
    //std::cout<<  " facotr "<<tkParam.pickHitPhiFactor<< " dPhiz "<<layer->dPhiz()
    //<< " factor*dPhiz "<<layer->dPhiz()*tkParam.pickHitPhiFactor<< std::endl;
    double t_phiHit = -999.;
    if (curv*Bz <= 0.0) {
      //positive track in minus Bz
      phiLow = phiEnter;
      phiHigh = phiExit;
      // Allow some slop in phi
      phiLow -= tkParam.maxInactiveResid / rEnter;
      phiHigh += tkParam.maxInactiveResid / rExit;
    } else {
      phiLow = phiExit;
      phiHigh = phiEnter;
      phiLow -= tkParam.maxInactiveResid / rExit;
      phiHigh += tkParam.maxInactiveResid / rEnter;
    }   
    //yzhang fix cross x axis bug 2011-04-10 
    if((phiHigh>0 && phiLow<0)){
      phiLow += Constants::twoPi;
    }else if((phiHigh<0 && phiLow>0)){
      phiHigh += Constants::twoPi;
    }

    double lowFloat = nCell /Constants::twoPi * (phiLow - layer->phiOffset()) + 0.5;
    double highFloat = nCell /Constants::twoPi * (phiHigh  - layer->phiOffset()) + 0.5;
    wireLow = (lowFloat >= 0.0) ? int(lowFloat) : int(floor(lowFloat));
    wireHigh = (highFloat >= 0.0) ? int(highFloat) : int(floor(highFloat));

    if(6==tkParam.lPrint){
      std::cout << "  wireLow "<<wireLow << " wireHigh "<<wireHigh <<" phiLow "<<phiLow*180./Constants::pi << " phiHigh "<<phiHigh*180./Constants::pi <<  std::endl;
    }
    // *** Loop through the predicted hit wires
    int tempDiff = 0;
    if(g_pickHitWire) { 
      int tempN = Constants::maxCell[layer->layNum()];
      if(wireLow>tempN/2. && wireHigh<tempN/2.){
	g_pickHitWire->fill(wireHigh+tempN - wireLow); 
	tempDiff = wireHigh+tempN - wireLow +1;
      }else{
	g_pickHitWire->fill(wireHigh - wireLow); 
	tempDiff = wireHigh - wireLow +1;
      }
    }//yzhang hist cut

    if(wireLow>wireHigh) wireLow -= nCell;//yzhang 2011-05-16 
    long t_iHit = 0;
    for (int thisWire = wireLow; thisWire <= wireHigh; thisWire++) {
      int corrWire = mdcWrapWire(thisWire, nCell);
      thisHit = map->hitWire(layer->layNum(), corrWire);

      if(6==tkParam.lPrint){
	if(thisHit != 0) {cout<<endl<<"test hit "; thisHit->print(std::cout);}
	else std::cout << endl<<"test hit ("<<layer->layNum()<<","<<corrWire<<")"<< std::endl;
      }

      double tof = 0.;
      double z = 0.;
      double driftDist = 0.;
      // Calculate expected drift distance
      double delx, dely;
      double resid = 0., predDrift = 0.;
      int ambig = 0;
      const MdcHitOnTrack *alink = 0;
      double mcTkId = -9999; //yzhang for tuple 2011-06-28 
      if (thisHit == 0 ) {
	delx = -d0 * sinPhi0 - layer->xWire(corrWire);
	dely =  d0 * cosPhi0 - layer->yWire(corrWire);
	predDrift = cosPhi0 * dely - sinPhi0 * delx + 
	  curv * (delx * delx + dely * dely);
	if(6==tkParam.lPrint) cout<<"No hit. predDrift="<<predDrift<<endl;
	continue;
      } else {  
	if(m_tuplePick) mcTkId = thisHit->digi()->getTrackIndex();
	// look for existing hit
	if(thisHit->getHitOnTrack(&(trk->track())) ==0){
	  alink = 0;//yzhang temp
	}else{
	  alink = thisHit->getHitOnTrack(&(trk->track()))->mdcHitOnTrack();//yzhang temp
	  if(6==tkParam.lPrint) { cout << " existing hot; " ;}
	}

	if (alink == 0 || pickAm) {
	  if ((!tkStatus->is2d()) && layer->view() != 0){
	    double rc = 9999.;
	    if(fabs(par.omega())>Constants::epsilon) rc = 1./fabs(par.omega());
	    double rw = layer->rMid();
	    double alpha = acos(1 - rw*rw/(2*rc*rc));
	    double fltLen = rw;
	    if(fabs(1 - rw*rw/(2*rc*rc))<1) fltLen = rc * alpha;
	    z = par.z0() + fltLen* par.tanDip();
	    tof = fltLen / Constants::c;
	    double x = layer->getWire(corrWire)->xWireDC(z);
	    double y = layer->getWire(corrWire)->yWireDC(z);
	    delx = -d0 * sinPhi0 - x;
	    dely =  d0 * cosPhi0 - y;
	    if(m_tuplePick) t_phiHit = thisHit->phi(z);
	  }else{
	    delx = -d0 * sinPhi0 - thisHit->x();
	    dely =  d0 * cosPhi0 - thisHit->y();
	    if(m_tuplePick) t_phiHit = thisHit->phi();
	  }
	  predDrift = cosPhi0 * dely - sinPhi0 * delx + 
	    curv * (delx * delx + dely * dely);

	  // predDrift = predDrift * (1. - curv() * predDrift);
	  ambig = (predDrift >= 0) ? 1 : -1;
	  if (hasCurled) ambig = -ambig;
	  double entranceAngle=0.;
	  driftDist = thisHit->driftDist(tof+bunchTime,ambig,entranceAngle,0.,z);
	  if (alink == 0) {
	    // FIXME: is this ambig here VVVVV OK for incoming tracks?
	    resid = ambig * fabs(driftDist) - predDrift;
	    aresid = fabs(resid);
	    //if (aresid > tkParam.maxInactiveResid ) ambig = 0;//yzhang delete 2012-10-09
	  }
	} else {
	  ambig = alink->ambig();
	  if(6==tkParam.lPrint) cout << " pick ambig for hot"<<endl;
	  if(m_tuplePick) t_phiHit = par.phi0()+par.omega()*alink->fltLen();
	}
      }

      //yzhang 2012-08-20 , guess phi of this hit on z
      double zGuess = par.z0() + layer->rMid() * par.tanDip();
      double phiDCz = layer->getWire(corrWire)->phiDC(zGuess);
      BesAngle phiDCzMax(phiDCz + deltaPhiCellWidth);
      BesAngle phiDCzMin(phiDCz - deltaPhiCellWidth);

      if(m_tuplePick && alink==0) {
	double sigma = 999.;
	if (thisHit != 0 &&alink==0) {
	  double entranceAngle = 0.;
	  sigma = thisHit->sigma(driftDist, ambig, entranceAngle, atan(par.tanDip()), z);
	}
	m_pickPredDrift[t_iHit] = predDrift;
	m_pickDrift[t_iHit] = driftDist;
	m_pickDriftTruth[t_iHit] = haveDigiDrift[thisHit->layernumber()][thisHit->wirenumber()];
	if(curv*Bz<=0.){
	  //positive track under minus Bz
	  if((phiDCzMin-phiExit>0) || (phiDCzMax-phiEnter<0)) m_pickPhizOk[t_iHit] = 0;
	  else m_pickPhizOk[t_iHit] = 1;
	}else{
	  if((phiDCzMin-phiEnter>0) || (phiDCzMax-phiExit<0)) m_pickPhizOk[t_iHit] = 0;
	  else m_pickPhizOk[t_iHit] = 1;
	}
	m_pickZ[t_iHit] = z;
	m_pickWire[t_iHit]=thisHit->wirenumber();
	m_pickResid[t_iHit] = aresid;
	if(abs(sigma)>0.000001)  m_pickSigma[t_iHit] = sigma;
	else m_pickSigma[t_iHit] = 999.;
	double t_phiLowCut=-999.;
	double t_phiHighCut= -999.;
	if(t_phiHit > -998.){
	  t_phiLowCut = (phiEnter-t_phiHit)*rEnter;
	  t_phiHighCut = (phiExit-t_phiHit)*rExit;
	}
	m_pickPhiLowCut[t_iHit] = t_phiLowCut;
	m_pickPhiHighCut[t_iHit] = t_phiHighCut;
	m_pickDriftCut[t_iHit] = goodDriftCut;
	m_pickMcTk[t_iHit] = mcTkId;
	m_pickPt[t_iHit] = tkFit->pt();
	m_pickCurv[t_iHit] = curv;
	t_iHit++;
      }

      if(curv*Bz<=0.){
	//positive track
	if((phiDCzMin-phiExit>0) || (phiDCzMax-phiEnter<0)) {
	  if(6==tkParam.lPrint){ std::cout<<" CUT by phiDCz not in phi En Ex range, curv>=0"<<std::endl; }
	  continue;
	}
      }else{
	//negtive track
	if((phiDCzMin-phiEnter>0) || (phiDCzMax-phiExit<0)) {
	  if(6==tkParam.lPrint){ std::cout<<" CUT by phiDCz not in phi En Ex range, curv<0"<<std::endl; }
	  continue;
	}
      }

      // Cases
      // (0) pred drift dist > goodDriftCut, drop Hit
      if (ambig != 0 && fabs(predDrift) > goodDriftCut){//yzhang add 2012-08-17 
	if(6==tkParam.lPrint){cout<<" predDrift "<<predDrift<<">goodDriftCut "<<goodDriftCut<<endl;}
	continue;
      }

      // (1) No good hit, but track near cell-edge => do nothing and continue
      if (ambig == 0 && fabs(predDrift) > goodDriftCut){//yzhang 2009-11-03 add 3factor, 2011-05-30 from 3 to 2,2012-08-17 from 2 to 1
	if(6==tkParam.lPrint){
	  cout<<"   ambig==0 && |predDirft| "<<fabs(predDrift) <<"> goodDriftCut "<< goodDriftCut<<endl;
	  cout<<" No good hit, but track near cell-edge " <<  endl;
	}
	continue;
      }


      // (2) Hit found: 
      if (ambig != 0) {
	//yzhang changed 2009-10-19
	//if resid> maxActiveSimga * sigma do not add hits to track
	double entranceAngle = 0.;
	double sigma = thisHit->sigma(driftDist, ambig, entranceAngle, atan(par.tanDip()), z);
	double factor = tkParam.pickHitFactor;
	//yzhang delete 2012-10-09 
	//if(!is2d fabs(par.tanDip())<2) factor = (2-par.tanDip())/2 * factor;
	double residCut = tkParam.maxActiveSigma * factor * sigma;//yzhang 2012-08-21 
	//if(6==tkParam.lPrint){
	  //std::cout<< "aresid "<<aresid<<" residCut "<<residCut<<" sigma "<<sigma <<" tanDip "<< par.tanDip() <<" factor "<<factor <<" tkParam.maxActiveSigma "<<tkParam.maxActiveSigma<<std::endl;
	//}

	if (alink == 0 && (aresid <= residCut) ) {
	  if(6==tkParam.lPrint){
	    cout << " (2) New hit found " <<  endl;//yzhang debug
	  }
	  //yzhang 2012-08-17 delete
	  int isActive = 1;
	  //if (aresid > residCut) isActive = 0;
	  //if(6==tkParam.lPrint) {
	  //  if (aresid > residCut)
	  //    std::cout << "notACT, resid  "<<aresid<<" >residCut " << residCut<< std::endl;
	  //}
	  MdcRecoHitOnTrack tempHot(*thisHit, ambig, bunchTime);
	  tempHot.setActivity(isActive);
	  // Don't add active hits if they're in use.
	  if (thisHit->usedHit()){
	    tempHot.setUsability(false);
	    if(6==tkParam.lPrint) std::cout<<   " thisHit used, setUsability false "   << std::endl;
	  }
	  // very crude starting point for flight length !!!! improve
	  double flt = layer->rMid();
	  if (hasCurled) flt = Constants::twoPi * rCurl - layer->rMid();
	  flt += 0.000001 * (thisHit->x() + thisHit->y());
	  tempHot.setFltLen(flt);
	  if(6==tkParam.lPrint) {
	    std::cout<< " aresid  "<<aresid<<"<=residCut " <<residCut<<" nSig "<<aresid/sigma<< " nSigCut "<<(tkParam.maxActiveSigma*factor)<<" factor "<<factor<<" maxActiveSigma "<<tkParam.maxActiveSigma<<" tanDip "<<par.tanDip()<<std::endl;
	    std::cout<< " Append Hot  "   << std::endl;
	  }
	  alink = (MdcHitOnTrack*) hitList->appendHot(&tempHot);
	}else{
	  if(6==tkParam.lPrint){
	    if(alink!=0){  
	      std::cout<< "Exist hot found"<<std::endl;
	    }else if(aresid > residCut){
	      thisHit->print(std::cout);
	    std::cout<< " Drop hit. aresid  "<<aresid<<">residCut " <<residCut<<" nSig "<<aresid/sigma<< " nSigCut "<<(tkParam.maxActiveSigma*factor)<<" factor "<<factor<<" maxActiveSigma "<<tkParam.maxActiveSigma<<" tanDip "<<par.tanDip()<<std::endl;
	    }
	  }
	}
	if (!localHistory.member(const_cast<MdcHitOnTrack*>(alink))) {
	  localHistory.append(const_cast<MdcHitOnTrack*>(alink));
	  if (hasCurled) trk->setHasCurled();
	  nFound++;
	  if(6==tkParam.lPrint) std::cout<<" nFound="<<nFound<<" nCand "<<nCand<<std::endl;
	  if (layer == firstInputLayer && firstInputHit < 0) {
	    firstInputHit = nCand;	
	  }
	} else {
	  if(6==tkParam.lPrint) std::cout << "ErrMsg(warning) "<< "would have inserted identical HOT "
	    "twice in history buffer" << std::endl;
	}
      }

      // (3) No hit found; if beyond known good region, should we continue?
      else if (ambig == 0 && reachedLastInput) {
	if(6==tkParam.lPrint) std::cout << "ambig==0 "<<std::endl;
	lContinue = false;
	int nSuccess = 0;
	int last = 8;
	if (nCand < 8) last = nCand;
	for (int i = 0; i < last; i++) {
	  int j = nCand - 1 - i;
	  if (localHistory[j] != 0) {
	    //std::cout<<  __FILE__ <<" localHistory["<<j<<"]!=0 nSuccess++ "<<  std::endl;
	    nSuccess++;
	  }
	  if (i == 2 && nSuccess >= 2) lContinue = true;
	  if (i == 4 && nSuccess >= 3) lContinue = true;
	  if (i == 7 && nSuccess >= 5) lContinue = true;
	  if(6==tkParam.lPrint) cout <<i<< "  (3) No hit found; if beyond known good region " <<  endl;//yzhang debug
	  if (lContinue) {
	    if(6==tkParam.lPrint) std::cout<<" pickHits break by lContinue. i="<<i<<" nSuccess="<<nSuccess<< std::endl;
	    break;
	  }
	}  

	if(6==tkParam.lPrint) cout << "  (3) No hit found; if beyond known good region " <<  endl;//yzhang debug
	// if lContinue == false => there's been a gap, so quit
	if (!lContinue) {
	  //std::cout<<  __FILE__ <<"  "<< __LINE__  <<" break by !lContinue "<< std::endl;
	  break;
	}
	localHistory.append(0);
      } 

      nCand++;	  
      // Update last layer:
      if (ambig != 0 && reachedLastInput) {
	if (trk->hasCurled() == 0) {
	  if (thisHit->layer()->rEnd() > trk->lastLayer()->rEnd()) {
	    trk->setLastLayer(thisHit->layer());
	  }
	}
	else {
	  if (thisHit->layer()->rEnd() < trk->lastLayer()->rEnd()) {
	    trk->setLastLayer(thisHit->layer());
	  }
	}
      }

    }  // end loop over hit wires
    if(t_iHit>0 && m_tuplePick) {
      m_pickNCell = tempDiff;
      m_pickNCellWithDigi = t_iHit;
      m_pickLayer = layer->layNum();
      m_pickIs2D = is2d;
      m_tuplePick->write();
    }
    if (!lContinue && reachedLastInput) {
      //std::cout<<  __FILE__ <<" break by !lContinue && reachedLastInput "<< std::endl;
      break;
    }
  }  // end loop over layers

  // Now look back at hits in early layer and see if there are any to be thrown away there.  
  bool lContinue = true;
  for (int ihit = firstInputHit; ihit >= 0; ihit--) {
    if (localHistory[ihit] != 0) {
      if (lContinue) {
	// Update firstLayer
	const MdcHitOnTrack *mdcHit = localHistory[ihit]->mdcHitOnTrack();
	if (mdcHit != 0) {
	  if (mdcHit->layer()->rEnd() < trk->firstLayer()->rEnd()) {
	    trk->setFirstLayer(mdcHit->layer());	
	  }
	}
	continue;          // no gap yet
      }	else { 
	// gap found; delete hits
	if(6==tkParam.lPrint){
	  std::cout << " gap found; delete hits. ";
	}
	if (!localHistory[ihit]->isUsable()) {
	  if(6==tkParam.lPrint){
	    cout << "about to delete hit for unusable HOT:" << endl;
	    localHistory[ihit]->print(std::cout);
	  }
	  hitList->removeHit(localHistory[ihit]->hit());   
	}
	if(6==tkParam.lPrint){
	  cout << " current contents of localHistory: "
	    <<localHistory.length()<<"hot" << endl;
	  //for (int i=0; i<localHistory.length();++i) {
	  //cout << " hit @ " << localHistory[i]->hit() << "; hot @ " << localHistory[i] << endl;
	  //}
	}
	nFound--;
      }
    }
    else if (localHistory[ihit] == 0) {
      if(6==tkParam.lPrint){ cout << " localHistory= 0 " << endl; }
      int nSuccess = 0;
      lContinue = false;
      int last = 8;
      if (nCand < 8) last = nCand;
      for (int i = 0; i < last; i++) {
	int j = ihit + 1 + i;
	if (localHistory[j] != 0) nSuccess++;
	if (i == 2 && nSuccess >= 2) lContinue = true;
	if (i == 4 && nSuccess >= 3) lContinue = true;
	//	if (i == 7 && nSuccess >= 5) lContinue = true;
	if (lContinue) break;
      }
    }
  }
  if(6==tkParam.lPrint){ 
    std::cout<<  "nFound="<<nFound <<" < "<< tkParam.pickHitFract*nCand<<" pickHitFract? "<< tkParam.pickHitFract<<"*"<<nCand  << std::endl;
  }
  if (nFound < tkParam.pickHitFract * nCand) nFound = 0;//yzhang delete 2010-05-10 use pickHitFract=0.
  if(6==tkParam.lPrint){ cout << " localHistory= 0 " << endl; }

  if(6==tkParam.lPrint || 3==tkParam.lPrint){
    cout << "After pickHits found " << nFound <<" hit(s)"<< endl;
    hitList->hotList().print(std::cout);
    std::cout<< std::endl;
  }

  return nFound;
}

//************************************************************************
int 
MdcTrackList::finishHelix(MdcTrack& mdcTrk, const MdcHitMap* map, const 
    MdcDetector* gm) {
  //***********************************************************************
  int success = 1;

  TrkRecoTrk& trk = mdcTrk.track();
  TrkErrCode fitResult;
  TrkHelixMaker helMaker;
  const TrkFit* tkFit = trk.fitResult();
  assert (tkFit != 0);
  TrkHitList* hitList = trk.hits();
  assert (hitList != 0);
  TrkExchangePar par = tkFit->helix(0.0);


  helMaker.setFlipAndDrop(trk, true, true);
  fitResult = hitList->fit();

  if (!fitResult.success() && (3 == tkParam.lPrint)) {
    cout << "Helix fit failure: " << endl;
    fitResult.print(cout); 
  }
  helMaker.setFlipAndDrop(trk, false, false);

  if (!fitResult.success()) return 0;

  bool lcurler(fabs(tkFit->helix(0).omega()) > 3.4);//yzhang FIXME, ??
  pickHits(&mdcTrk, map, gm, lcurler);//yzhang add 2010-05-10 

  if(3==tkParam.lPrint) std::cout<< __FILE__ << "   " << __LINE__ << " nHit after pickHit  "<<hitList->nHit() <<std::endl;
  if(3==tkParam.lPrint) hitList->hotList().printAll(std::cout);
  //if(hitList->nHit()<=0) return 0;

  helMaker.setFlipAndDrop(trk, true, true); 
  fitResult = hitList->fit();
  if (fitResult.failure()) {
    if(3==tkParam.lPrint){
      cout << "Second helix fit failed: " << endl;
      fitResult.print(std::cout); 
    }
    return 0;
  }
  if(3==tkParam.lPrint){ cout << "Final fit: " << endl << trk << endl; }
  helMaker.setFlipAndDrop(trk, false, false); 

  double chisqperDOF = 0.;
  int nACT = tkFit->nActive();
  int nDOF = nACT - 5;
  if (nDOF > 5) {
    chisqperDOF = tkFit->chisq() / nDOF;
  } else {
    chisqperDOF = tkFit->chisq();
  }
  if(g_3dTkChi2) { g_3dTkChi2->fill( chisqperDOF ); }//yzhang hist cut
  if(g_fitNAct) { g_fitNAct->fill(nACT ); }//yzhang hist cut

  int goodMatch = 1;
  if (fitResult.failure() || chisqperDOF > tkParam.maxChisq || nACT < tkParam.minHits ) {
    goodMatch = 0;
    if (3 == tkParam.lPrint) {
      std::cout<<" goodMatch=0; chi2/dof "<<chisqperDOF <<" >?maxChisq"<<tkParam.maxChisq 
	<<" nAct:"<<nACT <<"<?minHits"<<tkParam.minHits <<  std::endl;
    }
  }
  //yzhang add
  ////yzhang FIXME
  //if (tkParam.lUseQualCuts) {
  //double tem2 = (float) trk.hits()->nHit() - nACT;
  //if (tem2 >= tkParam.maxNmissTrack) goodMatch = 0;
  //if (tem2 / float(trk.hits()->nHit()) > tkParam.maxNmissNorm) 
  //goodMatch = 0;
  //}
  //zhangy add

  if (goodMatch) {
    if(3 <= m_debug){std::cout<<" ***** finishHelix success!"<<  std::endl;}
    trk.fitResult()->positionErr(0.0);
  } else { // Not a good match
    success = 0;
    if(3 <= m_debug){std::cout<<" ***** finishHelix failure!"<<  std::endl;}
  }  // end if goodmatch

  return success;
}


//************************************************************************
int 
MdcTrackList::finishCircle(MdcTrack& mdcTrk, const MdcHitMap* map, const 
    MdcDetector* gm) {
  //************************************************************************
  TrkRecoTrk& trk = mdcTrk.track();
  if(9==tkParam.lPrint){
    std::cout << " finishCircle "<< std::endl;
    trk.print(std::cout);
  }

  const TrkFit* tkFit = trk.fitResult();
  if(9==tkParam.lPrint){  cout << "Before circle fit, nactive " << tkFit->nActive() << endl;}
  assert (tkFit != 0);
  TrkHitList* hitList = trk.hits();
  assert (hitList != 0);
  TrkCircleMaker circMaker;
  circMaker.setFlipAndDrop(trk, false, false);    // reset as a precaution
  //circMaker.setFactor(trk, 1.);//nSigma cut factor//yzhang FIXME 2010-09-13 

  TrkErrCode fitResult = hitList->fit();
  if (fitResult.failure()) {
    trk.status()->addHistory(TrkErrCode(TrkErrCode::fail,15,"finishCircle"),"MdcTrkRecon");
    if (tkParam.lPrint > 1) {
      cout << "First circle fit failed: " << endl;
      fitResult.print(std::cout); 
    }
    return 0;
  }
  trk.status()->addHistory(TrkErrCode(TrkErrCode::succeed,15,"finishCircle"),"MdcTrkRecon");

  if(9==tkParam.lPrint){ cout << "After circle fit, nactive " << tkFit->nActive() << endl;}
  double chisqperDOF;
  int nDOF = tkFit->nActive() - 3;
  if (nDOF > 3){
    chisqperDOF = tkFit->chisq() / nDOF;
  }else{
    chisqperDOF = tkFit->chisq();
  }

  if(g_cirTkChi2) { g_cirTkChi2->fill( chisqperDOF ); }//yzhang hist cut
  int success = (chisqperDOF <= tkParam.maxChisq && tkFit->nActive() >= 3);

  if(9==tkParam.lPrint){
    std::cout<<__FILE__<<" "<<__LINE__
      << " success "<<success
      << " chisqperDOF "<< chisqperDOF<<"<? maxChisq "<< tkParam.maxChisq
      << " nAct "<<tkFit->nActive()<<">=3 "
      << std::endl;
    mdcTrk.track().hots()->printAll(std::cout);
  }
  bool lcurler(fabs(tkFit->helix(0).omega()) > 3.4);//yzhang FIXME, ??
  pickHits(&mdcTrk, map, gm, lcurler);

  if(9==tkParam.lPrint){
    std::cout<< __FILE__ << "   " << __LINE__ << " nHit after pickHit  "<<hitList->nHit() <<std::endl;
  }
  //if(hitList->nHit()<=0) return 0;

  circMaker.setFlipAndDrop(trk, true, true); 
  fitResult = hitList->fit();
  if (fitResult.failure()) {
    if(9==tkParam.lPrint){
      cout << "Second circle fit failed: " << endl;
      fitResult.print(std::cout); 
    }
    return 0;
  }
  if(9==tkParam.lPrint){
    cout << "Final fit: " << endl << trk << endl;
  }
  circMaker.setFlipAndDrop(trk, false, false); 

  nDOF = tkFit->nActive() - 3;
  if (nDOF > 3) {
    chisqperDOF = tkFit->chisq() / nDOF;
  }
  else {
    chisqperDOF = tkFit->chisq();
  }
  if(g_cirTkChi2) { g_cirTkChi2->fill( chisqperDOF ); }//yzhang hist cut
  success = (chisqperDOF <= tkParam.maxChisq)  && (tkFit->nActive() >= 3);

  if(9==tkParam.lPrint){
    cout << "chisqperDOF "<<chisqperDOF<<"=?" << tkParam.maxChisq<< endl;
    cout << "nActive "<<tkFit->nActive()<<">= 3"<< endl;
  }

  if(9==tkParam.lPrint){
    trk.printAll(cout);
  }

  return success;
}

void MdcTrackList::dumpAxFill(const MdcTrack * trialTrack ) {
  std::cout << "ax fill: "<<std::endl; 
  if(!trialTrack){
    trialTrack->track().printAll(cout);//yzhang debug
  }
}

void MdcTrackList::dumpSeed(const MdcSeg * seed,bool goodOnly ) {
  std::cout << std::endl<<"Dump seed segment goodOnly="<<goodOnly<<" ";
  seed->plotSegAll();
  std::cout<< std::endl;
}

void MdcTrackList::dumpAxCombine(const MdcTrack * trialTrack) {
  if (NULL == trialTrack) return;
  std::cout<<std::endl<< "-------------------------------------"<<std::endl;
  std::cout<<"Track and hitList after AxCombine "<<std::endl;
  trialTrack->track().printAll(cout);//yzhang debug
  TrkHotList::hot_iterator hotIter= trialTrack->track().hits()->hotList().begin();
  while (hotIter!=trialTrack->track().hits()->hotList().end()) {
    cout <<"(" <<((MdcHit*)(hotIter->hit()))->layernumber()
      <<","<<((MdcHit*)(hotIter->hit()))->wirenumber() 
      <<":"<<hotIter->isActive()<<")  ";
    hotIter++;
  }
  std::cout << std::endl;
  std::cout<< "-------------------------------------"<<std::endl;
}

void MdcTrackList::dumpCircle(const MdcTrack* trialTrack ){
  if(NULL == trialTrack) return;
  if (!trialTrack->track().fitResult()) return;
  /*
     double omega,r,pt;
     omega =trialTrack->track().fitResult()->helix(0.0).omega();
     if (omega == 0) pt = 0;
     else pt = (-1) * 1/(omega * 333.576 );
     std::cout<<"in MdcTrackList Circle Pt = "<< pt <<std::endl;//yzhang deubg

     if (omega == 0) r=0;
     else r = 1/ omega;
     std::cout<<"in MdcTrackList Circle R = "<< r <<std::endl;//yzhang deubg 
     */
  std::cout<<std::endl<< "-------------------------------------"<<std::endl;
  std::cout << "Track and hitList after finishCircle" << std::endl;
  trialTrack->track().printAll(cout);
  TrkHotList::hot_iterator hotIter= trialTrack->track().hits()->hotList().begin();
  while (hotIter!=trialTrack->track().hits()->hotList().end()) {
    cout <<"(" <<((MdcHit*)(hotIter->hit()))->layernumber()
      <<","<<((MdcHit*)(hotIter->hit()))->wirenumber() 
      <<":"<<hotIter->isActive()<<")  ";
    hotIter++;
  }
  cout <<endl;
  std::cout<< "-------------------------------------"<<std::endl;
}

void MdcTrackList::dumpD0(const TrkExchangePar & par) {
  //yzhang hist
  //m_hd0->fill(par.d0());
  //m_d0 = par.d0();
  //  m_tuple1->write();//yzhang hist
  std::cout <<std::endl<< " Dump d0()  " << par.d0()<<"\n";//yzhang debug

  //zhangy hist
}
void MdcTrackList::dumpStFill() {
  std::cout << "Plot segs after st fillWithSegs " << std::endl;
  cout <<endl;

}

void MdcTrackList::dumpStCombine(const MdcTrack * trialTrack) {
  std::cout<<std::endl<< "-------------------------------------"<<std::endl;
  std::cout<<"Track and hitList after StCombine "<<std::endl;
  if(trialTrack)trialTrack->track().printAll(cout);
  TrkHotList::hot_iterator hotIter= trialTrack->track().hits()->hotList().begin();
  int tmplay = -1;
  while (hotIter!=trialTrack->track().hits()->hotList().end()) {
    int layer = ((MdcHit*)(hotIter->hit()))->layernumber();
    if( (layer%4) ==0 ) { if( tmplay != layer ) cout<<endl; }
    cout <<"(" <<layer <<","<<((MdcHit*)(hotIter->hit()))->wirenumber()
      <<" act:"<<hotIter->isActive() <<" lr:"<<hotIter->ambig() <<")  ";
    hotIter++;
    tmplay=layer;
  }
  cout <<endl;
  std::cout<< "-------------------------------------"<<std::endl;
}
void MdcTrackList::dumpHelix(const MdcTrack * trialTrack){ 
  std::cout<< std::endl<<"-------------------------------------"<<std::endl;
  std::cout<< "Track and hitList after finishHelix " << std::endl;
  trialTrack->track().printAll(cout);
  TrkHotList::hot_iterator hotIter= trialTrack->track().hits()->hotList().begin();
  int tmplay = -1;
  while (hotIter!=trialTrack->track().hits()->hotList().end()) {
    int layer = ((MdcHit*)(hotIter->hit()))->layernumber();
    if( (layer%4) ==0 ) { if( tmplay != layer ) cout<<endl; }
    cout <<"(" <<layer <<","<<((MdcHit*)(hotIter->hit()))->wirenumber()
      <<":"<<hotIter->isActive() <<")  ";
    hotIter++;
    tmplay = layer;
  }
  cout <<endl;
  std::cout<< "-------------------------------------"<<std::endl;
}

void MdcTrackList::dropMultiHotInLayer(const MdcTrack* tk){
  double tdr[43];
  double tdr_wire[43];
  for(int i=0; i<43; i++){tdr[i]=9999.;}

  // make flag
  TrkHotList::hot_iterator hotIter= tk->track().hits()->hotList().begin();
  while (hotIter!=tk->track().hits()->hotList().end()) {
    MdcHitOnTrack* hot = const_cast<MdcHitOnTrack*> (&(*hotIter->mdcHitOnTrack()));

    //driftTime(tof,z)
    double dt = hot->mdcHit()->driftTime(0.,0.);
    int layer = hot->mdcHit()->layernumber();
    int wire = hot->mdcHit()->wirenumber();
    if (dt < tdr[layer]) {
      tdr[layer] = dt;  
      tdr_wire[layer] = wire;  
    }
    hotIter++;
  }

  std::cout<<" tdr wire ";
  for(int i=0;i<43;i++){
    std::cout<<i<<" "<<tdr[i]<<" "<<tdr_wire<<" ";
  }
  std::cout<<" "<<  std::endl;
  // inactive multi hit 
  hotIter= tk->track().hits()->hotList().begin();
  while (hotIter!=tk->track().hits()->hotList().end()) {
    int layer = hotIter->mdcHitOnTrack()->mdcHit()->layernumber();
    int wire = hotIter->mdcHitOnTrack()->mdcHit()->wirenumber();
    double dt = hotIter->mdcHitOnTrack()->mdcHit()->driftTime(0.,0.);

    if ((tdr[layer] <9998.) && (tdr_wire[layer]!=wire)){
      MdcHitOnTrack* hot = const_cast<MdcHitOnTrack*> (&(*hotIter->mdcHitOnTrack()));
      hot->setActivity(false);

      std::cout<<__FILE__<<" inactive "<< layer<<" "<<wire<<" dt "<<dt <<  std::endl;
    }
    hotIter++;
  }
}
