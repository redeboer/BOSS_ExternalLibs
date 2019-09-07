//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixUtils.h,v 1.2 2006/03/28 01:02:36 zhangy Exp $
//
// Description:  package of utility routines for doing things to helices.
//  No data members.  I'll probably want to put this someplace else 
//  eventually.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKHELIXUTILS_H
#define TRKHELIXUTILS_H
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h" 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using CLHEP::HepMatrix;
using CLHEP::HepVector;
using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;

class TrkExchangePar;
class BField;
class BesPointErr;
class BesVectorErr;
class NeutParams;
// Class interface //
class TrkHelixUtils {

public:
  TrkHelixUtils() { };

     // Create a helix-set from a position and a momentum.
     // Uses nominal B field to determine curvature. 
  static TrkExchangePar helixFromMom(const HepPoint3D& vertex, 
		const Hep3Vector& p, double sign, const BField&);

     // Does the same, but gives *real* errors on the parameters
     // (instead of a default error matrix)
     // Uses nominal B field to determine curvature. 
  static TrkExchangePar helixFromMomErr(const BesPointErr& vertex,
                const BesVectorErr& p,const HepMatrix& cxp,  double sign, const BField&);
     // Does the same but for neutrals
  static NeutParams lineFromMomErr(const BesPointErr& vertex,
                const BesVectorErr& p,const HepMatrix& cxp,  double sign, const BField&); 

     // Jacobian for transforming std helix params to new set defined at fltNew
  static HepMatrix jacobianExtrapolate(const TrkExchangePar&, double fltNew);

     // Actually transform the error matrix, as above
  static HepSymMatrix extrapolateCov(TrkExchangePar &, double fltNew);

     // Path length (3-d) to intersection with cylinder at radius rad.
  static double fltToRad(const TrkExchangePar& hel, double rad);

private:	
  // Preempt 
  TrkHelixUtils&   operator= (const TrkHelixUtils&);
  TrkHelixUtils(const TrkHelixUtils &);
};

#endif

