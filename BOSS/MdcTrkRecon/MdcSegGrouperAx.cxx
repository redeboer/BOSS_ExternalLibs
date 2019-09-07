//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperAx.cxx,v 1.13 2011/09/26 01:06:37 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
//	 Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
// Copyright (C)  1996  The Board of Trustees of  
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------
#include "MdcGeom/BesAngle.h"
#include "CLHEP/Alist/AList.h" 
#include "MdcData/MdcHit.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcTrkRecon/MdcSegGrouperAx.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcSegInfoAxialO.h"
#include "MdcTrkRecon/MdcSegUsage.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/mdcWrapAng.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/GmsList.h"
#include "TrkBase/TrkExchangePar.h"
extern double MdcTrkReconCut_combAxPhi0;
extern double MdcTrkReconCut_combAxCurv;
extern double MdcTrkReconCut_combAxPhi0Cut;
extern double MdcTrkReconCut_combAxCurvCut;

//tuple item of combine ax segs
#include "GaudiKernel/NTuple.h"
extern NTuple::Tuple*  g_tupleCombAx;
extern NTuple::Item<double>               g_combAxdPhi0;
extern NTuple::Item<double>               g_combAxdCurv;
extern NTuple::Item<double>               g_combAxSigPhi0;
extern NTuple::Item<double>               g_combAxSigCurv;
extern NTuple::Item<double>               g_combAxSlSeed;
extern NTuple::Item<double>               g_combAxSlTest;
extern NTuple::Item<double>               g_combAxQualitySeed;
extern NTuple::Item<double>               g_combAxQualityTest;
extern NTuple::Item<double>               g_combAxPatSeed;
extern NTuple::Item<double>               g_combAxPatTest;
extern NTuple::Item<double>               g_combAxNhitSeed;
extern NTuple::Item<double>               g_combAxNhitTest;
extern NTuple::Item<double>               g_combAxMc;
extern NTuple::Item<double>               g_combAxMcPt;
extern NTuple::Item<double>               g_combAxMcTheta;
extern NTuple::Item<double>               g_combAxMcPhi;
extern NTuple::Item<double>               g_combAxMcAmbigSeed;
extern NTuple::Item<double>               g_combAxMcAmbigTest;

//Constructor
//------------------------------------------------------------------------
MdcSegGrouperAx::MdcSegGrouperAx(const MdcDetector *gm, int debug) : 
  MdcSegGrouper(gm, gm->nAxialSuper() - 1, debug) {
//------------------------------------------------------------------------
  lTestGroup = false;
  lTestSingle = true;

  isValid = new bool * [nPly()];
  for (int j = 0; j < nPly(); j++) {
    isValid[j] = 0;
  } 
}

//------------------------------------------------------------------------
void MdcSegGrouperAx::fillWithSegs( const MdcSegList *inSegs) {
//------------------------------------------------------------------------
  if(3==_debug) std::cout<<std::endl<< "=====MdcSegGrouperAx::fillWithSegs====="<<std::endl;
  // Prepare for axial finding
  // Store the segments (pointers, actually), sorting by phi0 
  for (int isuper = 0; isuper < _gm->nSuper(); isuper++) {
    const GmsList *inList = inSegs->oneList(isuper);
    if (inList->count() == 0) continue;
    MdcSeg *inSeg = (MdcSeg *) inList->first();
    // Only load axial segments
    if (inSeg->superlayer()->whichView() != 0) continue;
   
    while (inSeg != 0) {
      // Create an info object within the seg to store info
      MdcSegInfoAxialO *info = new MdcSegInfoAxialO;
      inSeg->setInfo(info);
      info->calcFromOrigin(inSeg);  // calc. origin-dependent info

      // Loop over the segs already stored, looking for the right place 
      //   to stick the new one
      int isInserted = 0;
      for (int iseg = 0; iseg < (int) segList[isuper].length(); iseg++) {
	MdcSeg *aSeg = segList[isuper][iseg];
	if ( ((MdcSegInfoAxialO *)aSeg->info())->phi0() < info->phi0()) {
	  continue;  }
	segList[isuper].insert(inSeg, iseg);
	isInserted = 1;
	break;
      }  // end of loop over existing segs
      if (isInserted == 0) segList[isuper].append(inSeg);
      inSeg = (MdcSeg *) inSeg->next();
    }  // end loop over new segs
//   cout<<"segList["<<isuper<<"].length"<< segList[isuper].length()<<endl;//yzhang debug
  }  //  end loop over superlayers

/////////////////////
/*  for(int isuper = 0; isuper < _gm->nSuper(); isuper++) {
    std::cout<<"-------super layer "<<isuper<<std::endl;
      for (int iseg = 0; iseg < (int) segList[isuper].length(); iseg++) {
        MdcSeg *aSeg = segList[isuper][iseg];
        std::cout << "    seg phi "<<iseg<< " "<<((MdcSegInfoAxialO*)aSeg->info())->phi0()<<std::endl;
      }  // end of loop over existing segs
  }

*/

}

//-------------------------------------------------------------------------
int MdcSegGrouperAx::incompWithSeg(const MdcSeg *refSeg, 
				   const MdcSeg *testSeg) {
//-------------------------------------------------------------------------

  // Returns 0 if valid, -1 if invalid, +1 if invalid and no more valid 
  //   ones possible in this slayer (assumes they're ordered)
  if (testSeg == 0) return 0;
  if(3 == _debug) {
    //std::cout<< "test seg:"; 
    testSeg->plotSegAll(); 
  }
  // Test phi0 match
  MdcSegInfoAxialO *refInfo = (MdcSegInfoAxialO *) refSeg->info();
  MdcSegInfoAxialO *testInfo = (MdcSegInfoAxialO *) testSeg->info();


  double sigPhi0 = (refInfo->sigPhi0() > testInfo->sigPhi0() ? 
      refInfo->sigPhi0() : testInfo->sigPhi0());
  double refPhi0 = refInfo->phi0();
  double testPhi0 =  testInfo->phi0();
  double corrPhi0 = mdcWrapAng(refPhi0, testPhi0);

  double sigCurv = (refInfo->sigCurv() > testInfo->sigCurv() ? 
      refInfo->sigCurv() : testInfo->sigCurv());
  double refCurv = refInfo->curv();
  double testCurv = testInfo->curv();
  //double nSigmaPhi0 = MdcTrkReconCut_combAxPhi0;//4. for default
  //double nSigmaCurv = MdcTrkReconCut_combAxCurv;//4. for default
  double phi0Cut = MdcTrkReconCut_combAxPhi0Cut;
  double curvCut = MdcTrkReconCut_combAxCurvCut;
  //std::cout << "test phi0 "<<corrPhi0<<" ref "<<refPhi0<<" sig "<< nSigmaPhi0 * sigPhi0 << std::endl;
  //std::cout << "test Curv "<<testCurv<<" ref "<<refCurv<<" sig "<< nSigmaCurv * sigCurv << std::endl;

  if (g_tupleCombAx) {
    g_combAxdPhi0 = refPhi0 - corrPhi0; 
    g_combAxdCurv = refCurv - testCurv; 
    g_combAxQualitySeed = refSeg->quality()+refSeg->nHit()*10+refSeg->superlayer()->slayNum()*1000;
    g_combAxQualityTest = testSeg->quality()+testSeg->nHit()*10+testSeg->superlayer()->slayNum()*1000;
    g_combAxMcAmbigSeed = refSeg->testCombSegAmbig();
    g_combAxMcAmbigTest = testSeg->testCombSegAmbig();
    g_combAxMc = refSeg->testCombSeg(testSeg);
    g_combAxMcPt = refSeg->testCombSegPt();
    g_combAxMcTheta = refSeg->testCombSegTheta();
    //g_combAxSigPhi0 = sigPhi0;
    //g_combAxSigCurv = sigCurv;
    //g_combAxSlSeed = refSeg->superlayer()->slayNum();
    //g_combAxSlTest = testSeg->superlayer()->slayNum();
    //g_combAxPatSeed = refSeg->segPattern();
    //g_combAxPatTest = testSeg->segPattern();
    //g_combAxNhitSeed = refSeg->nHit();
    //g_combAxNhitTest = testSeg->nHit();
    //test if the combined segments in the same track
    // return -1:seed false
    // return value = n hits on the seed track/ n hits of test seg
    //g_combAxMcPhi = refSeg->testCombSegPhi();
    //std::cout<< "mc seg   "<< refSeg->testCombSeg(testSeg) << std::endl;//yzhang debug
    g_tupleCombAx->write();  
  }

  //yzhang add 2009-10-16
  //if (refPhi0 - corrPhi0 > nSigmaPhi0 * sigPhi0)
  //if(3 == _debug){
  //  std::cout << " phi0 ref"<<refPhi0
  //    <<" corr "<<corrPhi0
  //    << " diff "<<fabs(corrPhi0-refPhi0)
  //    <<" >? "<<phi0Cut<<std::endl;
  //  std::cout <<  " curv ref"<<refCurv
  //    <<" test "<<testCurv<< " diff "<<refCurv-testCurv
  //    <<" >? "<<curvCut<< std::endl;
  //}
  if(refSeg->testCombSegPt()>0.4 && fabs(corrPhi0 - refPhi0)>0.4 &&(refSeg->testCombSeg(testSeg)>0.5)){
    if(3==_debug){
      std::cout<< endl<<" test " << std::endl;
      std::cout<<"seed seg: "; refSeg->plotSegAll(); std::cout<< std::endl;
      std::cout<<"test seg: "; testSeg->plotSegAll(); std::cout<< std::endl;
      std::cout<< " dPhi0 abnormal "<<corrPhi0 - refPhi0<<std::endl;
    }
  }else{
    if(3== _debug){
      std::cout<< endl<<" test " << std::endl;
      std::cout<<"seed seg: "; refSeg->plotSegAll(); std::cout<< std::endl;
      std::cout<<"test seg: "; testSeg->plotSegAll(); std::cout<< std::endl;
      std::cout<< " dPhi0 ok " <<setprecision(3)<< corrPhi0 - refPhi0<<std::endl; 
    }
  }

  //std::cout<< __FILE__ << "   " << __LINE__ << " dphi0=  "<<fabs(corrPhi0 - refPhi0)<<" mc "<<refSeg->testCombSeg(testSeg)<<std::endl;

  cout<<setiosflags(ios::fixed);
  //FIXME 2011-05-31 yzhang cut vs pt
  if (fabs(corrPhi0 - refPhi0) > phi0Cut) { 
    if(3 == _debug) std::cout <<  " SKIP by dPhi0 "
      <<fabs(corrPhi0-refPhi0)<<">"<<phi0Cut<<std::endl; 
    //yzhang delete 
    //if (testPhi0 > refPhi0) return  1;  
    //else 
    return -1; // => testPhi0>2pi & refPhi0<2pi
  }else{
    if(3 == _debug)std::cout<< " dphi " <<setprecision(3)<< fabs(corrPhi0 - refPhi0); 
  }

  // Test curvature match
  // use larger error of the two
  //if (fabs(refCurv - testCurv) > nSigmaCurv * sigCurv) 
  if (fabs(refCurv - testCurv) > curvCut){
    if(3 == _debug) std::cout <<  " SKIP by dCurv"
      <<fabs(refCurv-testCurv)<<">"<<curvCut<<std::endl; 
    return -2;
  }else{
    if(3 == _debug)std::cout<< " dCurv " <<setprecision(3)<< fabs(refCurv - testCurv);
  }
  if(3 == _debug) std::cout <<  " KEEP "<<std::endl; 

  cout<<setprecision(6);
  cout<<setiosflags(ios::scientific);
  //std::cout<< "ok!   " << std::endl;//yzhang debug
  return 0;
}

//-------------------------------------------------------------------------
int MdcSegGrouperAx::incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
    int iply) {
  //-------------------------------------------------------------------------

  return 0;
}

//-------------------------------------------------------------------------
void MdcSegGrouperAx::resetComb(const MdcSeg *seed) {
  //-------------------------------------------------------------------------

  // Delete existing list of valid/invalid segs
  if (isValid != 0) {
    int i;
    for (i = 0; i < nDeep; i++) {
      delete [] isValid[i];
      isValid[i] = 0;
    }
  }

  _seed = seed;
  //Grab the seglist for each non-seed slayer
  int islay = 0;
  int iply = 0;
  nPlyFilled = 0;
  nNull = 0;
  const MdcSuperLayer *seedSlay = 0;
  if (seed != 0) seedSlay = seed->superlayer();


  // Set up all sorts of stuff for fast grouping of segs in nextGroup()
  for (const MdcSuperLayer *thisSlay = _gm->firstSlay(); thisSlay != 0;
      thisSlay = thisSlay->next()) {
    bool noGoodYet = true;
    islay++;

    if (thisSlay == seedSlay) continue;
    if (thisSlay->whichView() != 0) continue;//Axial slayer
    firstGood[iply] = 0;

    // Loop over the segs, marking start & end of valid region for this seed
    firstBad[iply] = 0;
    if (segList[islay-1].length() != 0) {
      isValid[iply] = new bool[segList[islay-1].length()];
    }

    if(3 == _debug && segList[islay-1].length()>0) {
      std::cout<<std::endl<< "--match axial seg by phi in slayer "
	<<thisSlay->slayNum()<<"--"<<std::endl;
      //std::cout<< "reference seg: "; seed->plotSeg();
      //std::cout<< std::endl;
    }

    for (int i = 0; i < (int) segList[islay-1].length(); i++) {
      MdcSeg *aSeg = segList[islay-1][i];
      int invalid = incompWithSeg(seed, aSeg);
      isValid[iply][i] = true;
      if (invalid < 0) {
	firstBad[iply] = i;
	isValid[iply][i] = false;
	if (noGoodYet) firstGood[iply] = i+1;
      } else if (invalid > 0) {
	// No more valid segs in this slayer
	firstBad[iply] = i;
	for (int j = i; j < (int) segList[islay-1].length(); j++) 
	  isValid[iply][j] = false;
	break;
      } else {
	firstBad[iply] = i+1;
	noGoodYet = false;
      }
    }

    //if(3 == _debug) std::cout<<iply<<" islay "<<islay<<" firstGood "<<firstGood[iply]<<" "<<firstBad[iply]<< std::endl;
    if (firstGood[iply] > firstBad[iply]) firstGood[iply] = firstBad[iply];
    if (firstGood[iply] == firstBad[iply]) {
      // If there are no valid segs for this ply, drop it
      delete [] isValid[iply];
      isValid[iply] = 0;
      continue;
    }
    // Associate correct seglist with this ply
    combList[iply] = &segList[islay-1];
    leaveGap[iply] = false;
    iply++;
  }
  nPlyFilled = iply;
  resetSegCounters();
  maxNull = nPlyFilled - 2;
  maxNull++;
}
//---------------------------------------------------------------------
MdcTrack* MdcSegGrouperAx::storePar(MdcTrack* trk, double parms[2], 
    double chisq, TrkContext& context, 
    double t0) {
  //---------------------------------------------------------------------
  // cout << "storePar in MdcSegGrouperAx" <<endl;//yzhang debug
  assert(trk == 0);
  BesAngle foundPhi0(parms[0]);
  // factor of two to convert to BaBar def of curvature (omega)
  //std::cout<< "stored par   " << parms[0]<< "   "<<parms[1]
    //<<" store: "<<foundPhi0.rad()<<" "<<2.*parms[1]<<std::endl;
  TrkExchangePar par(0.0, foundPhi0.rad(), 2.*parms[1], 0.0, 0.0);
  return new MdcTrack(_gm->nSuper(), par, chisq, context, t0);
}

/*
   double MdcSegGrouperAx::calcParByHits(MdcSeg **segGroup, int nToUse, const TrkExchangePar &par,  double param[2]) {
   return 0.;
   }
   */
