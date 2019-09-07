//
//   EmcRecBarrelGeo
//   
//   Dec 18, 2003, Created by Wang.Zhe
//
//   unit: mm, radian
//
#ifndef EMC_REC_BARREL_GEO_H
#define EMC_REC_BARREL_GEO_H

#include <vector>
#include <iostream>

#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "EmcRecGeoSvc/EmcRecCrystal.h"

using namespace CLHEP;

class EmcRecBarrelGeo
{
 public:
   // Constructors and destructors
   EmcRecBarrelGeo();
   ~EmcRecBarrelGeo();
   
   // private method
 private:
   //====for barrel EMC
   void ParameterInitialize();
   // Evaluate one volumn along y-axis, then transform them to others by rotation.
   void CalculateStandardCrystal();
   // Figure out the volumn with minimum phi
   void Transform2Column1();
   // Fill crystals' center vector
   void FillCCenterVector();

 public:
   // Access by others (refer to Identifier for ID arrangement)
   //
   // Index range:   theta   phi
   //    barrel==>   0-43    0-119
   //              (value of angel from min to max)
   //
   EmcRecCrystal GetCrystal(const Identifier& id) const;
   HepPoint3D    GetCCenter(const Identifier& id) const;
   HepPoint3D    GetCFrontCenter(const Identifier& id) const;
   
   inline double GetBarrelR() const { return fBarrelR; }
   inline double GetBarrelOffset1() const { return fBarrelOffset1; }
   inline double GetBarrelOffset2() const { return fBarrelOffset2; }
   inline double GetBarrelh1() const { return fBarrelh1; }
   inline double GetBarrelh2() const { return fBarrelh2; }
   inline double GetBarrelh3() const { return fBarrelh3; }
   inline double GetBarrelL() const { return fBarrelL; }
   inline int GetBarrelNPhiMax() const { return fBarrelNPhiMax; }
   inline int GetBarrelNThetaMax() const { return fBarrelNThetaMax; }
   
   // private data members
 private:
   // for Barrel EMC
   double fBarrelR;
   double fBarrelOffset1;
   double fBarrelOffset2;
   double fBarrelh1;
   double fBarrelh2;
   double fBarrelh3;
   double fBarrelL;
   double fBarrelL2;
   double fBarrelAlpha;
   int fBarrelNPhiMax;
   int fBarrelNThetaMax;
   
   // standard crystals
   vector <EmcRecCrystal> fStandard;
   // vector of each crystal's center
   vector <HepPoint3D> fCCenter;
   vector <HepPoint3D> fCFrontCenter;
};
#endif /* EMC_REC_BARREL_GEO_H */
