//-----------------------------------------------------------------------
// File from KalFit module
//
// Filename : KalFitCylinder.h
//------------------------------------------------------------------------
// Description :
// Cylinder is an Element whose shape is a cylinder.
//------------------------------------------------------------------------
// Modif :
//------------------------------------------------------------------------

#ifndef _DEFINE_CYLINDER_H_
#define _DEFINE_CYLINDER_H_
#include "KalFitAlg/KalFitElement.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D <double > HepPoint3D;
#endif

/// Cylinder is an Element whose shape is a cylinder.
class KalFitCylinder : public KalFitElement {
protected:
  double ro_;
  double ri_;
  double zf_;
  double zb_;

public:
  /// Constructor
  KalFitCylinder(const KalFitMaterial* material,
	       double radius, double thick,
	       double length, double z0) 
    : KalFitElement(material)
  {
    ri_ = radius;
    ro_ = radius + thick;
    zf_ = z0 + length * .5;
    zb_ = z0 - length * .5;
  }

  /// Find intersection with Helix
  virtual double intersect(const KalFitTrack& track,
			   HepPoint3D& x) const;

  virtual double intersect(const KalFitTrack& track,
                           HepPoint3D& x, const HepPoint3D& point) const;
  
  /// Check if the position x is inside the current cylinder
  bool isInside(const HepPoint3D& x) const;
  bool isInside2(const HepPoint3D& x) const;
  
  /// Extract radius of the cylinder
  virtual double radius(void) const { return ro_; }
};
#endif

