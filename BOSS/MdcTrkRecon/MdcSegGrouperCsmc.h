//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperCsmc.h,v 1.5 2009/12/16 09:02:47 zhangy Exp $
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

#ifndef MDCSEGGROUPERCSMC_HH
#define MDCSEGGROUPERCSMC_HH
#include "MdcTrkRecon/MdcSegGrouper.h"

class MdcSegList;
class TrkContext;

// Class interface //
class MdcSegGrouperCsmc : public MdcSegGrouper {

public:
  MdcSegGrouperCsmc(const MdcDetector *gm, int debug);
  ~MdcSegGrouperCsmc() { };

  void fillWithSegs( const MdcSegList *inSegs);
  virtual int incompWithSeg(const MdcSeg *refSeg, const MdcSeg *testSeg);
  virtual int incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
			      int iply);
  void resetComb(const class MdcSeg *);  
  virtual MdcTrack* storePar(MdcTrack* trk, double parms[2], double chisq, 
			     TrkContext&, double trackT0);

private:	
  const MdcSeg *_seed; 

  // Preempt 
  MdcSegGrouperCsmc&   operator= (const MdcSegGrouperCsmc&)  ; 
};
#endif







