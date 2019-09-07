//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkPocaBase.cxx,v 1.4 2006/03/28 01:02:36 zhangy Exp $
//
// Description:
//     
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
// Modifications:
// - Jan 2003 (WDH) 
//------------------------------------------------------------------------
#include "TrkBase/TrkPocaBase.h"
#include "TrkBase/TrkPoca.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#include "MdcGeom/Trajectory.h"
#include <math.h>
#include <assert.h>
#include <iomanip>
#include <algorithm>

TrkPocaBase::TrkPocaBase(double f1, double f2, double prec)
        :  _precision(prec), _flt1(f1), _flt2(f2),_status(TrkErrCode::fail)
{
  assert(prec > 0.);
}

void 
TrkPocaBase::minimize(const Trajectory& traj1, double f1,
                      const Trajectory& traj2, double f2) 
{
  // Last revision: Jan 2003, WDH
  const int maxnOscillStep = 5 ;
  const int maxnDivergingStep = 5 ;

  // initialize
  _status = TrkErrCode::succeed;
  _flt1 = f1;
  _flt2 = f2;
  
  static HepPoint3D newPos1, newPos2 ;
  double delta(0), prevdelta(0) ;
  int nOscillStep(0) ;
  int nDivergingStep(0) ;
  bool finished = false ;
  int istep(0) ;

  for (istep=0; istep < _maxTry && !finished; ++istep) {
    double prevflt1  = flt1();
    double prevflt2  = flt2() ;
    double prevprevdelta = prevdelta ;
    prevdelta = delta;
    
    stepTowardPoca(traj1, traj2);
    if( status().failure() ) {
      // failure in stepTowardPoca
      finished=true ;
    } else {
      newPos1 = traj1.position(flt1());
      newPos2 = traj2.position(flt2());
      delta = (newPos1 - newPos2).mag();
      double step1 = flt1() - prevflt1;
      double step2 = flt2() - prevflt2;
      int pathDir1 = (step1 > 0.) ? 1 : -1;
      int pathDir2 = (step2 > 0.) ? 1 : -1;
      
      // Can we stop stepping?
      double distToErr1 = traj1.distTo1stError(prevflt1, precision(), pathDir1);
      double distToErr2 = traj2.distTo1stError(prevflt2, precision(), pathDir2);
      
      // converged if very small steps, or if parallel
      finished = 
	(fabs(step1) < distToErr1 && fabs(step2) < distToErr2 ) ||
	(status().success() == 3) ;
 
      // we have to catch some problematic cases
      if( !finished && istep>2 && delta > prevdelta) {
	if( prevdelta > prevprevdelta) {
	  // diverging
	  if(++nDivergingStep>maxnDivergingStep) { 
	    _status.setFailure(2) ; // code for `Failed to converge'
	    finished = true ;
	  }
	} else {
 	  nDivergingStep=0;
	  // oscillating
	  if(++nOscillStep>maxnOscillStep) {
	    // bail out of oscillation. since the previous step was
	    // better, use that one.
	    _flt1 = prevflt1 ;
	    _flt2 = prevflt2 ;
	    _status.setSuccess(21, "Oscillating poca.") ;
	    finished = true ;
	  } else {
	    // we might be oscillating, but we could also just have
	    // stepped over the minimum. choose a solution `in
	    // between'.
	    _flt1 = prevflt1 + 0.5*step1 ;
	    _flt2 = prevflt2 + 0.5*step2 ;	    
	    newPos1 = traj1.position(_flt1) ;
	    newPos2 = traj2.position(_flt2) ;
	    delta = (newPos1 - newPos2).mag() ;
	  }
	}
      } 
    }
  }
  if(!finished) _status.setSuccess(2) ; // code for 'not converged' (yet)
}

TrkPocaBase::TrkPocaBase(double f1, double prec) 
  : _precision(prec), _flt1(f1), _flt2(0), _status(TrkErrCode::fail)
{
}

void
TrkPocaBase::minimize(const Trajectory& traj, double f1, 
                      const HepPoint3D& pt )
{
   _status=TrkErrCode::succeed;
  _flt1 = f1;
  int pathDir = 1;  // which way are we stepping (+/- 1)

  int nTinyStep = 0;  // number of consecutive tiny steps -- oscillation test
  int nOscills = 0;
  double fltLast = 0., fltBeforeLast = 0.; // another check for oscillation
  for (int i = 0; i < _maxTry; i++) {
    fltLast = flt1();
    stepToPointPoca(traj, pt);
    if (status().failure()) return;
    double step = flt1() - fltLast;
    pathDir = (step > 0.) ? 1 : -1;
    // Can we stop stepping?
    double distToErr = traj.distTo1stError(fltLast, precision(), pathDir);
    bool mustStep = (fabs(step) > distToErr && step != 0.);
    // Crude test for oscillation (around cusp point of piecewise traj, I hope)
    if (fabs(step) < 0.5 * precision()) {
      nTinyStep++;
    } else {
      nTinyStep = 0;
      if (i > 1) {
        if (fabs(step) >= fabs(fltBeforeLast-fltLast) &&
            fabs(fltBeforeLast-flt1()) <= fabs(step)) {
          nOscills++;
          double halfway = (fltBeforeLast + fltLast) / 2.;
          if ((traj.position(flt1()) - pt).mag() >
              (traj.position(halfway) - pt).mag()) _flt1 = halfway;
        }
      }
    }
    if (nTinyStep > 3) mustStep = false;
    if (nOscills > 2) {
      mustStep = false;
#ifdef MDCPATREC_WARNING
      std::cout<<" ErrMsg(warning) "<< "Alleged oscillation detected. "
	      << step << "  " << fltLast-fltBeforeLast
	      << "  " << i << "  " << std::endl;
#endif
    }
    if (!mustStep) return;
    fltBeforeLast = fltLast;
  }
  // Ran off the end of the loop
  _status.setFailure(2);
}


TrkPocaBase::~TrkPocaBase() 
{}


void 
TrkPocaBase::stepTowardPoca(const Trajectory& traj1, const Trajectory& traj2) 
{ 
  // Last revision: Jan 2003, WDH
  
  // A bunch of unsightly uninitialized variables:
  static Hep3Vector dir1, dir2;
  static Hep3Vector delDir1, delDir2;
  static HepPoint3D pos1, pos2;

  traj1.getInfo(flt1(), pos1, dir1, delDir1);
  traj2.getInfo(flt2(), pos2, dir2, delDir2);
  Hep3Vector delta = ((CLHEP::Hep3Vector)pos1) - ((CLHEP::Hep3Vector)pos2);
  double ua = -delta.dot(dir1);
  double ub =  delta.dot(dir2);
  double caa = dir1.mag2() + delta.dot(delDir1);
  double cbb = dir2.mag2() - delta.dot(delDir2);
  double cab = -dir1.dot(dir2);
  double det = caa * cbb - cab * cab;

  if(det<0) {
    // get rid of second order terms
    caa = dir1.mag2() ;
    cbb = dir2.mag2() ;
    det = caa * cbb - cab * cab;
  }
	  
  if ( det < 1.e-8) {
    // If they are parallel (in quadratic approximation) give up
    _status.setSuccess(3);
    return;
  }

  double df1 = (ua * cbb - ub * cab)/det;
  int pathDir1 = (df1 > 0) ? 1 : -1;
  double df2 = (ub * caa - ua * cab)/det;
  int pathDir2 = (df2 > 0) ? 1 : -1;

  // Don't try going farther than worst parabolic approximation will
  // allow: Since ` _extrapToler' is large, this cut effectively only
  // takes care that we don't make large jumps past the kink in a
  // piecewise trajectory.

  double distToErr1 = traj1.distTo2ndError(flt1(), _extrapToler, pathDir1);
  double distToErr2 = traj2.distTo2ndError(flt2(), _extrapToler, pathDir2);

  // Factor to push just over border of piecewise traj (essential!)
  const double smudge = 1.01 ; 
  if( fabs(df1) > smudge*distToErr1 ) {
    // choose solution for which df1 steps just over border
    df1 = smudge*distToErr1 * pathDir1 ;
    // now recalculate df2, given df1:
    df2 = (ub - df1*cab)/cbb ;
  }

  if( fabs(df2) > smudge*distToErr2 ) {
    // choose solution for which df2 steps just over border
    df2 = smudge*distToErr2 * pathDir2 ;
    // now recalculate df1, given df2:
    df1 = (ua - df2*cab)/cbb ;
    // if still not okay,
    if( fabs(df1) > smudge*distToErr1 ) {
      df1 = smudge*distToErr1 * pathDir1 ;
    }
  }

  _flt1 += df1;
  _flt2 += df2;

  // another check for parallel trajectories
  if (fabs(flt1()) > _maxDist && fabs(flt2()) > _maxDist) 
    _status.setSuccess(3) ;
}

void 
TrkPocaBase::stepToPointPoca(const Trajectory& traj, const HepPoint3D& pt) 
{
  // Unsightly uninitialized variables:
  static Hep3Vector dir, delDir;
  static HepPoint3D trajPos;

  traj.getInfo(flt1(), trajPos, dir, delDir);
  Hep3Vector delta = ((CLHEP::Hep3Vector)trajPos) - ((CLHEP::Hep3Vector)pt);
  double denom = 1. + delta.dot(delDir);
  if (fabs(denom)*_maxDist < 1. ) {
    _status.setFailure(11, "TrkPoca::ambiguous tight looper.");
    return;
  }
  double df = -delta.dot(dir) / fabs(denom);
  int pathDir = (df > 0.) ? 1 : -1;

  // Don't try going farther than worst parabolic approximation will allow:
  double distToErr = traj.distTo2ndError(flt1(), _extrapToler, pathDir);
  if (fabs(df)>distToErr) df = (df>0?distToErr:-distToErr);
  // Make the step slightly longer -- prevents quitting at kinks
  df += 0.001 * pathDir * precision();
  _flt1 += df;
}

double TrkPocaBase::_maxDist = 1.e7;

int TrkPocaBase::_maxTry = 500;

double TrkPocaBase::_extrapToler = 2.;
