//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoCsmc.cxx,v 1.3 2009/12/16 09:02:47 zhangy Exp $
//
// Description:
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
//#include "BaBar/BaBar.h"
#include "MdcTrkRecon/MdcSegInfoCsmc.h"
#include <math.h>
#include <assert.h>
#include "MdcTrkRecon/mdcTwoInv.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcGeom/BesAngle.h"
//#include "ErrLogger/ErrLog.h"
using std::endl;

//--------------------------------------------------------------------------
bool 
MdcSegInfoCsmc::parIsAngle(int i) const {
//---------------------------------------------------------------------------
  assert (i >= 0 && i < 2);
  return (0 != i);   //i.e., parameter 1 is an angle
}
//------------------------------------------------------------------
void 
MdcSegInfoCsmc::calcStraight(double phi, double slope, 
				  double rad, const double *inErr) {
//------------------------------------------------------------------
  // Load the segment info obj with phi0 and d0 values, calc. assuming 
  // the segments lie on straight tracks.
  //     
  // d0 defined to be positive for tracks that intercept x axis on positive 
  //   phi (neg x) side of origin.  At least that's what I claim. */
  
  // error matrix conversion: 
  //   Vij(f) = SUM(n,m) ( d(fi)/d(yn) * d(fj)/d(ym) * Vnm(y) )
  //   The derivatives are calculated from:
  //       d0 = r**2 * slope / sqrt( 1 + r**2 * slope**2 )   &
  //       phi0 = phiseg - asin(d0 / r) 
  
  //    d0ovSlope = r**2/sqrt(1 + slope**2 * r**2) = d0 / slope
  //    d0ovrad = d0/r
  //    d0ovradsq = (d0/r)**2
  //    dphi0dphi = d(phi0)/d(seg-phi)
  //    dphi0ds   = d(phi0)/d(seg-slope)
  //    dd0ds   = d(d0)/d(seg-slope)
  //    dd0dphi = 0
  
  double d0ovSlope = -rad * rad / sqrt( 1. + slope*slope * rad*rad );
  double d0 = slope * d0ovSlope;
  _par0 = d0;

  double d0ovRad =  d0 / rad ;
  // Don't blow away arcsine:
  d0ovRad = ( d0ovRad > 1.0 || d0ovRad < -1.0) ? 
    (d0ovRad+0.00001)/fabs(d0ovRad) : d0ovRad; 
  BesAngle phi0 = phi - asin( d0ovRad );
  _par1 = phi0.posRad();

  // Error matrix (anal calculation).
  //  double dphi0dphi = 1.;
  double dphi0ds = -d0ovSlope * sqrt( 1. - d0ovRad*d0ovRad ) / rad;
  double dd0ds = d0ovSlope * ( 1. - d0ovRad*d0ovRad );

  // d0:
  _errmat[0] = inErr[2] * dd0ds * dd0ds;
  if (_errmat[0] < 0.) _errmat[0] = 0.;

  // phi0:
  _errmat[2] = inErr[2] * dphi0ds * dphi0ds  + inErr[0]  +  
    inErr[1] * 2. * dphi0ds;
  if (_errmat[2] < 0.) _errmat[2] = 0.;
  // phi0|d0:
  _errmat[1] = inErr[2] * dd0ds * dphi0ds + 
               inErr[1] * dd0ds;

  int error = mdcTwoInv(_errmat, _inverr);
  if (error) {
    std::cout << " ErrMsg(warning) " 
      << "Failed to invert matrix -- MdcSegInfo::calcStraight" << endl 
      << _errmat[0] << " " << _errmat[1] << " " << _errmat[2]<< std::endl;
  }
}

//------------------------------------------------------------------
void 
MdcSegInfoCsmc::calcStraight(const MdcSeg *parentSeg) {
//------------------------------------------------------------------
  double slope = parentSeg->slope();
  double radius = parentSeg->superlayer()->rad0();
  double phi = parentSeg->phi();
  const double *inErr = parentSeg->errmat();

  calcStraight(phi, slope, radius, inErr);
}

//------------------------------------------------------------------
double 
MdcSegInfoCsmc::sigPhi0() const {
//------------------------------------------------------------------
  return sqrt(_errmat[2]);
}
//------------------------------------------------------------------
double 
MdcSegInfoCsmc::sigD0() const {
//------------------------------------------------------------------
  return sqrt(_errmat[0]);
}
