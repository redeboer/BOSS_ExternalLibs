//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkBmSpotOnTrk.cxx,v 1.3 2006/03/28 01:03:35 zhangy Exp $
//
// Description:
//     Implementation of class to add beam spot to track fit.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//------------------------------------------------------------------------
#include <math.h>
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "TrkFitter/TrkBmSpotOnTrk.h"
#include "TrkBase/TrkDifTraj.h"
#include "TrkBase/TrkPoca.h"
#include "TrkBase/TrkDifPoca.h"
#include "TrkBase/TrkRep.h"
using CLHEP::Hep3Vector;

static HepPoint3D dum1(0,0,0),dum2(0,0,1);

TrkBmSpotOnTrk::TrkBmSpotOnTrk(const HepPoint3D& ip, const HepSymMatrix& size)
  : TrkHitOnTrk(0,0.5e-4), 
    _beamTraj(FindBeamTrajectory(ip,size)),
    _ip(ip), 
    _size(size)
{}

//  Effectively a copy constructor
TrkBmSpotOnTrk::TrkBmSpotOnTrk(const TrkBmSpotOnTrk &hot, TrkRep *newRep, const TrkDifTraj *trkTraj)
  : TrkHitOnTrk(hot,newRep,trkTraj), 
    _beamTraj(hot._beamTraj),
    _ip(hot.ip()),
    _size(hot._size)
{}


TrkBmSpotOnTrk::~TrkBmSpotOnTrk()
{ }

TrkBmSpotOnTrk*
TrkBmSpotOnTrk::clone(TrkRep *rep, const TrkDifTraj *trkTraj) const
{
  return  new TrkBmSpotOnTrk(*this,rep,trkTraj);
}


TrkErrCode
TrkBmSpotOnTrk::updateMeasurement(const TrkDifTraj* traj,bool x)
{
  TrkErrCode status=updatePoca(traj,x);
  if (status.success()) {
     assert(_poca!=0);
     setHitResid(_poca->doca());
     setHitRms(GetRms());
  } else {
#ifdef MDCPATREC_WARNING
    std::cout<<"ErrMsg(warning) TrkBmSpotOnTrk::updateMeasurement failed" << std::endl;
#endif
     setHitResid(9999.9);
     setHitRms(9999.9);
  }
  return status;
}

TrkEnums::TrkViewInfo
TrkBmSpotOnTrk::whatView() const
{
  return TrkEnums::xyView;
}

const Trajectory*
TrkBmSpotOnTrk::hitTraj() const
{
  return &_beamTraj;
}

const HepPoint3D&
TrkBmSpotOnTrk::ip() const
{
  return _ip;
}


//
// GetRms
//
// Calculate RMS (hit width or resolution) based on local track
// trajectory.
//
// We have to deal with the error ellipse carefully. To do
// this, find the point along the linear trajectory in x and y
// that minimizes the chi-squared, and then use doca/sqrt(chi2)
// as the RMS.
//
double TrkBmSpotOnTrk::GetRms()
{
	//
	// Get direction
	//
	const TrkDifTraj& trkTraj = getParentRep()->traj();
	Hep3Vector trkDir = trkTraj.direction(fltLen());
	
	//
	// Get errors (assume no correlation)
	//
	double Mxx = 1.0/_size.fast(1,1);
	double Myy = 1.0/_size.fast(2,2);
	
	//
	// Normalized track directions in x/y
	//
	double vx = trkDir[0];
	double vy = trkDir[1];
	double normxy = (vx*vx + vy*vy);
	if (normxy <= 0) return 999.9;
	normxy = sqrt(normxy);
	
	vx /= normxy;
	vy /= normxy;
	
	//
	// Solve for point of least chi2
	//
	double s = vx*vy*(Mxx-Myy)/(vx*vx*Mxx + vy*vy*Myy);
	
	double dx = (-vy + s*vx);
	double dy = (+vx + s*vy);
	
	double chi2 = dx*dx*Mxx + dy*dy*Myy;
	
	return chi2 <= 0 ? 0.0 : (1.0/sqrt(chi2));
}


//
// FindBeamTrajectory
//
// Calculate a linear trajectory that corresponds to the
// beam spot and error matrix
//
// The simplest way I have to understand this calculation
// is to consider which (x,y) point is required to minimize
// the chi2 at +/- one unit in z.
//
// The following simple calculation assumes x and y are uncorrelated.
// This is to save CPU. It is straight forward to extend the
// calculation to finite x/y correlation.
//
const TrkLineTraj TrkBmSpotOnTrk::FindBeamTrajectory( const HepPoint3D &point,
						      const HepSymMatrix &error )
{
	int ifail;
	HepSymMatrix cover(error.inverse(ifail));
	
	if (ifail) {
#ifdef MDCPATREC_FATAL
          std::cout<<"ErrMsg(fatal) TrkLineTraj: "
		  <<"Error inverting beamspot error matrix" << std::endl;
#endif
	}
	double dx = -cover.fast(3,1)/cover.fast(1,1);
	double dy = -cover.fast(3,2)/cover.fast(2,2);
	
	HepPoint3D p1 = point + Hep3Vector(-dx,-dy,-1);
	HepPoint3D p2 = point + Hep3Vector(+dx,+dy,+1);

	return TrkLineTraj( p1, p2 );
}
