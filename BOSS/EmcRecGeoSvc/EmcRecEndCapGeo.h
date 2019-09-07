//
//   EmcRecEndCapGeo
//   
//   Dec 18, 2003, Created by Wang.Zhe
//
//   unit: mm, radian
//
#ifndef EMC_REC_ENDCAP_GEO_H
#define EMC_REC_ENDCAP_GEO_H

#include <vector>
#include <iostream>

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Plane3D.h"
#include "CLHEP/Geometry/Normal3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  //  backwards compatibility will be enabled ONLY in CLHEP 1.9
  typedef HepGeom::Plane3D<double> HepPlane3D;
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  //  backwards compatibility will be enabled ONLY in CLHEP 1.9
  typedef HepGeom::Normal3D<double> HepNormal3D;
#endif

#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "EmcRecGeoSvc/EmcRecCrystal.h"

using namespace CLHEP;

class EmcRecGeoPlane;

class EmcRecEndCapGeo
{
 public:
   // Constructors and destructors
   EmcRecEndCapGeo();
   ~EmcRecEndCapGeo();
   
   // private method
 private:
   //====for EndCap EMC
   void ParameterInitialize();
   //====calculate one standard sector
   void CalculateStandardSector1();
   //====calculate the other standard sector along with y-z plane
   void CalculateStandardSector2();
   // Fill crystals' center vector
   void FillCCenterVector();
   
 public:
   // Ac_ess by others (refer to Identifier for ID arrangement)
   EmcRecCrystal GetCrystal(const Identifier& id) const;
   HepPoint3D    GetCCenter(const Identifier& id) const;
   HepPoint3D    GetCFrontCenter(const Identifier& id) const;

 private:
   // Find intersection for three planes
   void FindInt(const EmcRecGeoPlane& p1,
		const EmcRecGeoPlane& p2,
		const EmcRecGeoPlane& p3,
		HepPoint3D &p);
   
   //general
   double flength;
   double fzshift;
   double fz;
   double fr[7];
   
   // for Ring5
   double fphi5[7];
   double fphi3[6];
   double fphi1[5];
   
   // for EndCap EMC (standard sector 1) (1/16 of whole ring)
   // East, theta 5, phi 0-5. ( total 96 )
   EmcRecCrystal fRing5[6];
   // East, theta 4, phi 0-5. ( total 96 )
   EmcRecCrystal fRing4[6];
   // East, theta 3, phi 0-4. ( total 80 )
   EmcRecCrystal fRing3[5];
   // East, theta 2, phi 0-4. ( total 80 )
   EmcRecCrystal fRing2[5];
   // East, theta 1, phi 0-3. ( total 64 )
   EmcRecCrystal fRing1[4];
   // East, theta 0, phi 0-3. ( total 64 )
   EmcRecCrystal fRing0[4];
   // for EndCap EMC (gap, standard sector 2)
   // East, theta 5, phi 24-29
   EmcRecCrystal fRing5p[6];
   // East, theta 4, phi 24-29
   EmcRecCrystal fRing4p[6];
   // East, theta 3, phi 20-24
   EmcRecCrystal fRing3p[5];
   // East, theta 2, phi 20-24
   EmcRecCrystal fRing2p[5];
   // East, theta 1, phi 16-19
   EmcRecCrystal fRing1p[4];
   // East, theta 1, phi 16-19
   EmcRecCrystal fRing0p[4];

   /// put them together for easy reference
   EmcRecCrystal fCrystal[2][6][6];
   /// vector of each crystal's center
   vector <HepPoint3D> fCCenter;   
   vector <HepPoint3D> fCFrontCenter;
};

//ad_ three building methods for easy use.
class EmcRecGeoPlane:public HepPlane3D
{
 public:
   /**
    * Constructor from four numbers.
    * The numbers are the coefficients of the equation of the plane:
    * @code
    *   a*x+b*y+c*z+d=0
    * @endcode
    */
   EmcRecGeoPlane(double a=0, double b=0, double c=0, double d=0) {
      a_=a; b_=b; c_=c; d_=d; return; }
   
   /// Constructor from normal and point.
   EmcRecGeoPlane(const HepPoint3D &n, const HepPoint3D &p) {
      a_=n.x(); b_=n.y(); c_=n.z(); d_=-n*p; return; }
    
   /// Constructor from three points.
   EmcRecGeoPlane(const HepPoint3D &p1, const HepPoint3D &p2, const HepPoint3D &p3)
     {
	HepNormal3D n = (p2-p1).cross(p3-p1);
	a_ = n.x(); b_ = n.y(); c_ = n.z(); d_ = -n*p1;
     }

   /// Assignment   
   EmcRecGeoPlane& operator=(const EmcRecGeoPlane &plane) {
      if(this!=&plane) {
	 a_ = plane.a_; b_ = plane.b_; c_ = plane.c_; d_ = plane.d_;
      } 
      return *this;
   }
   
   //More build functions, easy for use
   void Build(double a=0, double b=0, double c=0, double d=0) {
	a_=a; b_=b; c_=c; d_=d; return; }
   
   void Build(const HepPoint3D &n, const HepPoint3D &p) {
      a_=n.x(); b_=n.y(); c_=n.z(); d_=-n*p; return; }
   
   void Build(const HepPoint3D &p1, const HepPoint3D &p2, const HepPoint3D &p3) {
      HepNormal3D n = (p2-p1).cross(p3-p1);
      a_ = n.x(); b_ = n.y(); c_ = n.z(); d_ = -n*p1;
      return; }
};

#endif /* EMC_REC_ENDCAP_GEO_H */
