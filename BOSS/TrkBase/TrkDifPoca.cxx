//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkDifPoca.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
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
#include "TrkBase/TrkDifPoca.h"
#include "TrkBase/TrkDifTraj.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MdcRecoUtil/DifVector.h"
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifPoint.h"
#include "TrkBase/TrkPoca.h"
#include "TrkBase/TrkErrCode.h"

TrkDifPoca::TrkDifPoca(const TrkDifTraj& traj1, double f1, 
                       const Trajectory& traj2, double f2, double prec) 
        : TrkPocaBase(f1,f2,prec), _doca(-9999.,0) 
{
        minimize(traj1,f1,traj2,f2);
        if (status().failure()) return;
        calcDist(traj1,traj2);

}


TrkDifPoca::TrkDifPoca(const TrkDifTraj& traj, double f1, 
                       const HepPoint3D& pt, double prec) 
        : TrkPocaBase(f1,prec), _doca(-9999.,0) 
{ 
        minimize(traj,f1,pt);
        if (status().failure()) return;
        calcDist(traj,pt);
}


void 
TrkDifPoca::calcDist(const TrkDifTraj& traj1, const Trajectory& traj2) 
{
  // Does a final calculation of the distance -- better behaved derivs than 
  //  stepTowardPoca for zero dist.  In case of (nearly) parallel, returns 
  //  distance calculated at whatever point we happen to be at.  
  //  Derivatives are taken (of dist) w/r/t traj2 
  //  parameters, using DifNumbers for the calculation.

  // A bunch of unsightly uninitialized variables:
  static DifVector dir1;
  static DifPoint pos1;
  static Hep3Vector dirTem2;
  static HepPoint3D posTem2;

  // Request DifNumber info from traj1, ordinary info from traj2, and then 
  //   convert ordinary info into DifNumber
  traj2.getInfo(flt2(), posTem2, dirTem2);
  traj1.getDFInfo2(flt1(), pos1, dir1);
  DifVector dir2(dirTem2);
  DifPoint pos2(posTem2);

  DifVector delta = pos2 - pos1;
  if (status().success() != 3) { // Not parallel:
    DifVector between = cross( dir1, dir2 );  // cross-product
    between.normalize();
    _doca = delta * between;
  } else {  // Parallel: Arbitrary sign convention
    _doca = (delta - delta.dot(dir1) * dir1).length();
    if  (dir1.dot(dir2) > 0.) _doca.flipsign();
    if (fabs(_doca.number()) < 0.0001 * precision()) {
      // Parallel and on top of each other (derivatives singular)
      _doca = 0;
      _status.setFailure(3);
    }
  }
}

void 
TrkDifPoca::calcDist(const TrkDifTraj& traj, const HepPoint3D& pt) 
{
  // Does a final calculation of the distance -- and handles singularity 
  // in derivs when d = 0.

  // A bunch of unsightly uninitialized variables:
  static DifVector dir;
  static DifPoint posTraj;

  DifPoint posPoint(pt);
  traj.getDFInfo2(flt1(), posTraj, dir);  // call faster one, if exists
  DifVector delta = posTraj - posPoint;
  // delta -= dir*delta;  // note: dir*delta is zero, but the derivatives are NOT

  DifNumber dist = delta.length();
  if (dist.number() > 0.01 * precision()) { // d != 0
    _doca = dist;
  } else {
    // d = 0.  Fudge like mad: pick a direction (not parallel to traj) and 
    //   move the point slightly.  This should not happen very often.
    Hep3Vector fudgeVec(0., 0.1 * precision(), 0.0);
    if (dir.x.number() == 0.0 && dir.z.number() == 0.0) {
      fudgeVec = Hep3Vector(0.1 * precision(), 0., 0.);
    }
    posPoint += fudgeVec;
    delta = posTraj - posPoint;
    _doca = dist;
    _status.setSuccess(20, "TrkDifPoca: distance zero, calculation fudged.");
  }
}
