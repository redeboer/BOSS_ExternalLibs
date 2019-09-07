//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoSterO.h,v 1.9 2011/05/16 06:52:13 zhangy Exp $
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

#ifndef MDCSEGINFOSTERO_HH
#define MDCSEGINFOSTERO_HH
#include "MdcTrkRecon/MdcSegInfo.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class TrkRecoTrk;
class TrkExchangePar;
class MdcLine;
class MdcHitUse;
// Class interface //
class MdcSegInfoSterO : public MdcSegInfo {

public:
  MdcSegInfoSterO() { };
  ~MdcSegInfoSterO() { };

  double z0() const {return _par0;}
  double ct() const {return _par1;}
  bool parIsAngle(int i) const;

  //int calcStereo(const MdcSeg *parentSeg, const TrkRecoTrk &track, 
  int calcStereo(MdcSeg *parentSeg, const TrkRecoTrk &track,
		  MdcSegWorks &segStuff);
  int calcStereo(MdcSeg *parentSeg, const TrkExchangePar &par,
		  MdcSegWorks &segStuff, double Bz);
  
  int zPosition(MdcHitUse & hitUse,const TrkRecoTrk &track,MdcLine* span,int hitFit, double t0) const;
  int zPosition(MdcHitUse & hitUse, const TrkExchangePar &par, MdcLine* span,int hitFit, double t0, double Bz) const;
private:	
  MdcSegInfoSterO(const MdcSegInfoSterO &);
  MdcSegInfoSterO&   operator= (const MdcSegInfoSterO&);
  int _debug;//yzhang debug 
};

// Inline functions //

#endif







