//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperSt.h,v 1.6 2011/05/16 06:52:13 zhangy Exp $
//
// Description:
//     
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

#ifndef MDCSEGGROUPERST_HH
#define MDCSEGGROUPERST_HH
#include "MdcTrkRecon/MdcSegGrouper.h"

class MdcSegList;
class MdcTrack;
class TrkContext;

// Class interface //
class MdcSegGrouperSt : public MdcSegGrouper {

public:
  MdcSegGrouperSt(const MdcDetector *gm, int debug);
  ~MdcSegGrouperSt() { };
  void fillWithSegs(const MdcSegList *inSegs, const MdcTrack *axialTrack);
  //void testFillWithSegs(const MdcSegList *inSegs, const MdcTrack *axialTrack);
  void plotStereo() const;
  virtual int incompWithSeg(const MdcSeg *refSeg, const MdcSeg *testSeg);
  virtual int incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
			      int iply);
  void resetComb(const MdcSeg *seed = 0);
  virtual MdcTrack* storePar(MdcTrack* trk, double parms[2], double chisq,
			     TrkContext&, double trackT0); 

  //double calcParByHits(MdcSeg **segGroup, int nToUse, const TrkExchangePar &par, double& qual, int& nSegFit, double param[2]);
  
private:	
  void resetList();

  // Preempt 
  MdcSegGrouperSt&   operator= (const MdcSegGrouperSt&);
};
#endif







