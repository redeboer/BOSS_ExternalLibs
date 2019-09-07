//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegGrouperAx.h,v 1.6 2011/05/16 06:52:13 zhangy Exp $
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

#ifndef MDCSEGGROUPERAX_HH
#define MDCSEGGROUPERAX_HH
#include "MdcTrkRecon/MdcSegGrouper.h"

class MdcSegList;
class MdcDetector;
class TrkContext;

// Class interface //
class MdcSegGrouperAx : public MdcSegGrouper {

public:
  MdcSegGrouperAx(const MdcDetector *gm, int debug);
  ~MdcSegGrouperAx() { };

  void fillWithSegs( const MdcSegList *inSegs);
  virtual int incompWithSeg(const MdcSeg *refSeg, const MdcSeg *testSeg);
  virtual int incompWithGroup(MdcSeg **segGroup, const MdcSeg *testSeg, 
			      int iply);
  void resetComb(const MdcSeg *seed);  
  virtual MdcTrack* storePar(MdcTrack*, double parms[2], double chisq, 
			     TrkContext&, double trackT0);

  //double calcParByHits(MdcSeg **segGroup, int nToUse, const TrkExchangePar &par, double& qual, int& nSegFit, double param[2]);
private:	

  const MdcSeg *_seed; 

  // Preempt 
  MdcSegGrouperAx&   operator= (const MdcSegGrouperAx&);
};
#endif







