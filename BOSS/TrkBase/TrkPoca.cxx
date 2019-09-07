//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkPoca.cxx,v 1.4 2010/09/26 00:31:59 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner, largely taken from Art Snyder
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.h"
#include "MdcGeom/Trajectory.h"
#include "TrkBase/TrkPoca.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "TrkBase/TrkErrCode.h"

TrkPoca::TrkPoca(const Trajectory& traj1, double f1, 
    const Trajectory& traj2, double f2, double prec) 
: TrkPocaBase(f1,f2,prec), _doca(-9999.) 
{ 
  minimize(traj1,f1,traj2,f2);
  if (status().failure()) return;
  calcDist(traj1,traj2);
}

TrkPoca::TrkPoca(const Trajectory& traj, double flt, 
    const HepPoint3D& pt, double prec) 
: TrkPocaBase(flt,prec) 
{
  minimize(traj,flt,pt);
  if (status().failure()) return;
  _doca = (traj.position(flt1()) - pt).mag();
}

void 
TrkPoca::calcDist(const Trajectory& traj1, const Trajectory& traj2) 
{
  // Does a final calculation of the distance -- getting the sign right.
  //  In case of (nearly) parallel, returns error and distance.

  // A bunch of unsightly uninitialized variables:
  static Hep3Vector dir1,dir2;
  static HepPoint3D pos1,pos2;

  traj1.getInfo(flt1(), pos1, dir1);
  traj2.getInfo(flt2(), pos2, dir2);
  Hep3Vector delta = ((CLHEP::Hep3Vector)pos2) - ((CLHEP::Hep3Vector)pos1);
  Hep3Vector between = dir1.cross( dir2 );  // cross-product

  if (status().success() != 3) { // Not parallel:
    between = between.unit();
    _doca = delta.dot(between);
  } else {  // Parallel:
    // Arbitrary sign convention
    double sign = (dir1.dot(dir2) > 0.) ? -1. : 1.;
    _doca = (delta - delta.dot(dir1) * dir1).mag();
    _doca *= sign;
  }
}
