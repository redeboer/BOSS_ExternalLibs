//-----------------------------------------------------------------------
// File from RkFit module
//
// Filename : RkFitCylinder.h
//------------------------------------------------------------------------
// Description :
// Cylinder is an Element whose shape is a cylinder.
//------------------------------------------------------------------------
// Modif :
//------------------------------------------------------------------------

#ifndef _DEFINE_CYLINDER_H_
#define _DEFINE_CYLINDER_H_
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D <double > HepPoint3D;
#endif
#include "TrkReco/RkFitMaterial.h"
#include "TrkReco/TRunge.h"
/// Cylinder is an Element whose shape is a cylinder.
class RkFitCylinder  {
protected:
  double ro_;
  double ri_;
  double zf_;
  double zb_;
public:
const   RkFitMaterial* material_;
  /// Constructor
  RkFitCylinder(const RkFitMaterial* material,
	       double radius, double thick,
	       double length, double z0) 
  {
    material_=material;  
    ri_ = radius;
    ro_ = radius + thick;
    zf_ = z0 + length * .5;
    zb_ = z0 - length * .5;
  }

  /// Find intersection with Helix
  double intersect( TRunge& track,
			   HepPoint3D& x) const;

  double intersect( TRunge& track,
                           HepPoint3D& x, const HepPoint3D& point) const;
  void updateTrack(TRunge& track,double y[6])const;

  /// Check if the position x is inside the current cylinder
  bool isInside(const HepPoint3D& x) const;
  bool isInside2(const HepPoint3D& x) const;
  
  /// Extract radius of the cylinder
  virtual double radius(void) const { return ro_; }
};
#endif

