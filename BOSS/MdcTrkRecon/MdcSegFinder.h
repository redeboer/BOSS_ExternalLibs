//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegFinder.h,v 1.4 2010/07/19 06:41:49 zhangy Exp $
//
// Description:
//     Find straight-line segments in the DCH and put them into MdcSegList
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
// Copyright (C)  1996  The Board of Trustees of  
// 
// History:
//	Migration for BESIII MDC
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------

#ifndef MDCSEGFINDER_H
#define MDCSEGFINDER_H

#include "MdcTrkRecon/MdcSegPatterns.h"

class MdcSegList;
class MdcSegUsage;
template <class K, class V> class MdcMap;
class MdcHit;
class MdcDetector;
class MdcLine;
class MdcSuperLayer;
class MdcSeg;
class MdcHitMap;

// Class interface //
class MdcSegFinder {

public:
  MdcSegFinder(int useAllAmbig);
  ~MdcSegFinder() { };
  int createSegs(const MdcDetector *gm, MdcSegList &segs,       //return nSeg
	      const MdcMap<const MdcHit*, MdcSegUsage*>& usedHits, const MdcHitMap* map,double tbunch);
  MdcSegPatterns* thePattList() {return &patternList;}

private:	
  MdcSegPatterns patternList;  // allowed hit-patterns to form segments
  int tryPatterns(MdcHit *groupHits[8], unsigned groupWord, int nhit, 
       int lPrevHit, int npatt, int *allowedPatt, const MdcSuperLayer *slayer, 
       MdcSegList &segs, const MdcMap<const MdcHit*, MdcSegUsage*>& usedHits, const MdcHitMap*, 
		  double bunchTime);

  bool _addHits;
  // Preempt assignment:
  MdcSegFinder&   operator= (const MdcSegFinder&);
  MdcSegFinder(const MdcSegFinder &);
};
#endif







