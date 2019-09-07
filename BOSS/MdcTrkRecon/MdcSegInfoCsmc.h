//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoCsmc.h,v 1.2 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
//   holds my Mdc segment info, calculated assuming the seg is from a 
//    straight track.

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

#ifndef MDCSEGINFOCSMC_HH
#define MDCSEGINFOCSMC_HH
#include "MdcTrkRecon/MdcSegInfo.h"

// Class interface //
class MdcSegInfoCsmc : public MdcSegInfo {

public:
  MdcSegInfoCsmc() { };
  ~MdcSegInfoCsmc() { };

  double d0() const {return _par0;}
  double phi0() const {return _par1;}
  double sigPhi0() const;
  double sigD0() const;
  bool parIsAngle(int i) const;

  void calcStraight(double phi, double slope, double rad, 
		      const double *inErr);
  void calcStraight(const MdcSeg *parentSeg);

private:	

  // preempt
  MdcSegInfoCsmc&   operator= (const MdcSegInfoCsmc&)   ; 
  MdcSegInfoCsmc(const MdcSegInfoCsmc &)                ; 
};

#endif







