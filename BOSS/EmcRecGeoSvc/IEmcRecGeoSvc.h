//
//  Bes Emc barrel part and endcap Geometry service
//  Virtual interface class
//  
//  Created by Zhe Wang, May, 29 2004
//

#ifndef I_EMC_REC_GEO_SVC_H
#define I_EMC_REC_GEO_SVC_H

#include "GaudiKernel/IInterface.h" 
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "Identifier/Identifier.h"
#include "EmcRecGeoSvc/EmcRecCrystal.h"
using namespace CLHEP;

static const InterfaceID IID_IEmcRecGeoSvc("IEmcRecGeoSvc",1,0);

class IEmcRecGeoSvc:virtual public IInterface
{
   //access to geometry
 public:
   virtual EmcRecCrystal GetCrystal(const Identifier& id) const =0;
   virtual HepPoint3D    GetCrystalPoint(const Identifier& id, const int i) const =0;
   virtual HepPoint3D    GetCCenter(const Identifier& id) const =0;
   virtual HepPoint3D    GetCFrontCenter(const Identifier& id) const =0;
   
   virtual double GetBarrelR() const = 0;
   virtual double GetBarrelOffset1() const = 0;
   virtual double GetBarrelOffset2() const = 0;
   virtual double GetBarrelh1() const = 0;
   virtual double GetBarrelh2() const = 0;
   virtual double GetBarrelh3() const = 0;
   virtual double GetBarrelL() const = 0;
   virtual int GetBarrelNPhiMax() const = 0;
   virtual int GetBarrelNThetaMax() const = 0;
   
   static const InterfaceID& interfaceID() { return IID_IEmcRecGeoSvc; } 
};
   
#endif //I_EMC_REC_GEO_SVC_H

