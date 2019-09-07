//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoAxialO.h,v 1.3 2011/05/16 06:52:13 zhangy Exp $
//
// Description:
//   holds my Mdc segment info, calculated assuming the seg is from a track 
//    through the origin

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

#ifndef MDCSEGINFOAXIALO_HH
#define MDCSEGINFOAXIALO_HH
#include "MdcTrkRecon/MdcSegInfo.h"

// Class interface //
class MdcSegInfoAxialO : public MdcSegInfo {

public:
  MdcSegInfoAxialO() { };
  ~MdcSegInfoAxialO() { };

  double phi0() const {return _par0;}
  double curv() const {return _par1;}
  double sigCurv() const;
  double sigPhi0() const;
  bool parIsAngle(int i) const;

  void calcFromOrigin(const MdcSeg *parentSeg);
  void calcFromOrigin(double phi, double slope, double rad, 
		      const double *inErr);

private:	

  // preempt
  MdcSegInfoAxialO&   operator= (const MdcSegInfoAxialO&);
  MdcSegInfoAxialO(const MdcSegInfoAxialO &);
};

// Inline functions //

#endif
