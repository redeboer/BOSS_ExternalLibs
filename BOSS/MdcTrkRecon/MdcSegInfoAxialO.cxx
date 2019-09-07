//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoAxialO.cxx,v 1.2 2005/07/18 02:34:42 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.h"
#include "MdcTrkRecon/MdcSegInfoAxialO.h"
#include <math.h>
#include <assert.h>
#include "MdcTrkRecon/mdcTwoInv.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "MdcGeom/BesAngle.h"
//#include "ErrLogger/ErrLog.h"
using std::endl;

//--------------------------------------------------------------------------
bool 
MdcSegInfoAxialO::parIsAngle(int i) const {
//---------------------------------------------------------------------------
  assert (i >= 0 && i < 2);
  return (0 == i);   //i.e., 0th parameter is an angle
}
//------------------------------------------------------------------
void 
MdcSegInfoAxialO::calcFromOrigin(double phi, double slope, 
				  double radius, const double *inErr) {
//------------------------------------------------------------------
  
  if (slope == 0.) slope = 0.00000001;
  double slinv = 1./slope;
  _par1 = slope / sqrt(1. + radius*radius * slope*slope);  //curve
  double temp = 1. - _par1*_par1 * radius*radius;
  if (temp < 0.) temp = 0.;
  double dphi0ds = -radius * _par1 * sqrt(temp) * slinv;
  BesAngle phi0 = phi - asin(_par1 * radius);    
  _par0 = phi0.posRad();

  // phi0:
  _errmat[0] = inErr[2] * dphi0ds * dphi0ds  + inErr[0]  +  
    inErr[1] * 2. * dphi0ds;
  if (_errmat[0] < 0.) _errmat[0] = 0.;
  // curv:
  _errmat[2] = inErr[2] * _par1*_par1 * temp*temp * slinv*slinv;
  if (_errmat[2] < 0.) _errmat[2] = 0.;
    // phi0|curv:
  _errmat[1] = inErr[2] *_par1 * slinv * temp * dphi0ds + 
               inErr[1] *_par1 * slinv * temp;
  int error = mdcTwoInv(_errmat, _inverr);
  if (error) {
    std::cout << " ErrMsg(warning) " 
      << "Failed to invert matrix -- MdcSegInfo::calcFromOrigin" << endl
      << _errmat[0] << " " << _errmat[1] << " " << _errmat[2]<< endl;
  }
}

//-------------------------------------------------------------------
void 
MdcSegInfoAxialO::calcFromOrigin(const MdcSeg *parentSeg) {
//-------------------------------------------------------------------
  double slope = parentSeg->slope();
  double radius = parentSeg->superlayer()->rad0();
  double phi = parentSeg->phi();
  const double *inErr = parentSeg->errmat();

  calcFromOrigin(phi, slope, radius, inErr);
}

//-------------------------------------------------------------------
double 
MdcSegInfoAxialO::sigPhi0() const {
//-------------------------------------------------------------------
return sqrt(_errmat[0]);}

//-------------------------------------------------------------------
double 
MdcSegInfoAxialO::sigCurv() const {
//-------------------------------------------------------------------
return sqrt(_errmat[2]);}




