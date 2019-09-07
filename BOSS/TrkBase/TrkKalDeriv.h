//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkKalDeriv.h,v 1.2 2006/03/28 01:02:36 zhangy Exp $
//
// Description:  Pure abstract class adding derrivative functions needed
//  for kalman tracking.  This shouldn't be inherited directly, but is
//  instead brought in either through TrkSimpTraj or TrkDifPieceTraj.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Dave Brown 6/2/97
//
//------------------------------------------------------------------------

#ifndef TRKKALDERIV_H
#define TRKKALDERIV_H
#include "CLHEP/Matrix/Matrix.h"

using CLHEP::HepMatrix;

//  Enum defining deflection directions: theta1 is deflection in the
//  rho-z plane, theta2 is deflection in the phi direction.
enum deflectDirection{ theta1=0,theta2};

class TrkKalDeriv {
public:
// FIXME: this is a baseclass, therefore it should have a virtual d'tor...
// virtual ~TrkKalDeriv() {} ;
//  Change in param WRT change in direction 
  virtual HepMatrix derivDeflect(double fltlen,deflectDirection idir) const = 0;
// change in param WRT change in transverse position.  This is another
// effect of multiple scattering, only substantial for thick materials
  virtual HepMatrix derivDisplace(double fltlen,deflectDirection idir) const = 0;
//  Similair function for the parameter change WRT change in fractional momentum
  virtual HepMatrix derivPFract(double fltlen) const = 0;

};

#endif
