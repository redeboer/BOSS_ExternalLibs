//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperCsmc.cxx,v 1.7 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
//
// Copyright (C)  1996  The Board of Trustees of  
// 
// History:
//	Migration for BESIII MDC
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------
#include <assert.h>
#include "MdcGeom/BesAngle.h"
#include "CLHEP/Alist/AList.h" 
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcGeom/MdcDetector.h"
#include "MdcTrkRecon/MdcSegGrouperCsmc.h"
#include "MdcTrkRecon/MdcSegList.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcTrkRecon/MdcSegInfoCsmc.h"
#include "MdcTrkRecon/mdcWrapAng.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "MdcTrkRecon/GmsList.h"
#include "TrkBase/TrkExchangePar.h"

//Constructor
//------------------------------------------------------------------------
MdcSegGrouperCsmc::MdcSegGrouperCsmc(const MdcDetector *gm, int debug) : 
  MdcSegGrouper(gm, gm->nAxialSuper()-1, debug) {
//------------------------------------------------------------------------
		    
  lTestGroup = false;
  lTestSingle = false;

  isValid = new bool * [nPly()];
  for (int j = 0; j < nPly(); j++) {
    isValid[j] = 0;
  } 
}

//------------------------------------------------------------------------
void 
MdcSegGrouperCsmc::fillWithSegs( const MdcSegList *inSegs) {
//------------------------------------------------------------------------
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
      MdcSegInfoCsmc *info = new MdcSegInfoCsmc;
      inSeg->setInfo(info);
      info->calcStraight(inSeg);  // calc. origin-dependent info

      // Loop over the segs already stored, looking for the right place 
      //   to stick the new one
      int isInserted = 0;
      for (int iseg = 0; iseg < (int) segList[isuper].length(); iseg++) {
	MdcSeg *aSeg = segList[isuper][iseg];
	if ( ((MdcSegInfoCsmc *)aSeg->info())->phi0() < info->phi0()) 
	  continue;  
	segList[isuper].insert(inSeg, iseg);
	isInserted = 1;
	break;
      }  // end of loop over existing segs
      if (isInserted == 0) segList[isuper].append(inSeg);

      inSeg = (MdcSeg *) inSeg->next();
    }  // end loop over new segs
  }  //  end loop over superlayers

}

//-------------------------------------------------------------------------
int 
MdcSegGrouperCsmc::incompWithSeg(const MdcSeg *refSeg, 
				   const MdcSeg *testSeg) {
//-------------------------------------------------------------------------

  return 0;
  // Returns 0 if valid, -1 if invalid, +1 if invalid and no more valid 
  //   ones possible in this slayer (assumes they're ordered)
  if (testSeg == 0) return 0;
 
  // Test phi0 match
  MdcSegInfoCsmc *refInfo = (MdcSegInfoCsmc *) refSeg->info();
  MdcSegInfoCsmc *testInfo = (MdcSegInfoCsmc *) testSeg->info();
  double sigPhi0 = (refInfo->sigPhi0() > testInfo->sigPhi0() ? 
		    refInfo->sigPhi0() : testInfo->sigPhi0());
  double refPhi0 = refInfo->phi0();
  double testPhi0 =  testInfo->phi0();
  double corrPhi0 = mdcWrapAng(refPhi0, testPhi0);
  if (refPhi0 - corrPhi0 > 6. * sigPhi0) return -1;
  if (corrPhi0 - refPhi0 > 6. * sigPhi0) { 
    if (testPhi0 > refPhi0) return  1;  
    else return -1; // => testPhi0>2pi & refPhi0<2pi
  }

  // Test d0 match
  // use larger error of the two
  double sigD0 = (refInfo->sigD0() > testInfo->sigD0() ? 
		    refInfo->sigD0() : testInfo->sigD0());
  double refD0 = refInfo->d0();
  double testD0 = testInfo->d0();
  if (fabs(refD0 - testD0) > 6. * sigD0) return -2;

  return 0;
}
//-------------------------------------------------------------------------
int 
MdcSegGrouperCsmc::incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
			    int iply) {
//-------------------------------------------------------------------------

  return 0;
}

//---------------------------------------------------------------------
void 
MdcSegGrouperCsmc::resetComb(const class MdcSeg *seed) {
//---------------------------------------------------------------------
  
  // Delete existing list of valid/invalid segs
  if (isValid != 0) {
    int i;
    for (i = 0; i < nDeep; i++) {
      delete [] isValid[i];
      isValid[i] = 0;
    }
  }

  _seed = seed;
  //Grab the seglist for each slayer
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
    if (thisSlay->whichView() != 0) continue;
    firstGood[iply] = 0;
    // Loop over the segs, marking start & end of valid region for this seed
    firstBad[iply] = 0;
    if (segList[islay-1].length() != 0) 
      isValid[iply] = new bool[segList[islay-1].length()];
    for (int i = 0; i < (int) segList[islay-1].length(); i++) {
      MdcSeg *aSeg = segList[islay-1][i];
      int invalid = incompWithSeg(seed, aSeg);
      isValid[iply][i] = true;
      if (invalid < 0) {
	firstBad[iply] = i;
	isValid[iply][i] = false;
	if (noGoodYet) firstGood[iply] = i+1;
      }
      else if (invalid > 0) {
	// No more valid segs in this slayer
	firstBad[iply] = i;
	for (int j = i; j < (int) segList[islay-1].length(); j++) 
	  isValid[iply][j] = false;
	break;
      }
      else {
	firstBad[iply] = i+1;
	noGoodYet = false;
      }
    }
    //    if (thisSlay->whichView() != 0) continue;
    //    firstGood[iply] = 0;
    //    // Loop over the segs, marking start & end of valid region
    //    firstBad[iply] = 0;
    //    firstBad[iply] = segList[islay-1].length();


    if (firstGood[iply] > firstBad[iply]) firstGood[iply] = firstBad[iply];
    if (firstGood[iply] == firstBad[iply]) {
      // If there are no valid segs for this ply, skip it
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
MdcTrack* 
MdcSegGrouperCsmc::storePar(MdcTrack* trk, double parms[2], double chi2, 
			    TrkContext& context, double t0){
//---------------------------------------------------------------------
  assert(trk == 0);
  BesAngle foundPhi0(parms[1]);
  TrkExchangePar par(parms[0], foundPhi0.Rad(), 0., 0., 0.);
  return new MdcTrack(_gm->nSuper(), par, chi2, context, t0);
}
