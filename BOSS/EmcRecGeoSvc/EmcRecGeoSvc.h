//
//  Bes Emc barrel part and endcap Geometry service
//
//  Created by Zhe Wang, May, 29 2004
//

#ifndef EMC_REC_GEO_SVC_H
#define EMC_REC_GEO_SVC_H

#include "GaudiKernel/Service.h"

#include "EmcRecGeoSvc/IEmcRecGeoSvc.h"

#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"

#include "CLHEP/Geometry/Point3D.h" 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "EmcRecGeoSvc/EmcRecCrystal.h"
#include "EmcRecGeoSvc/EmcRecBarrelGeo.h"
#include "EmcRecGeoSvc/EmcRecEndCapGeo.h"
using namespace CLHEP;

class EmcRecROOTGeo;

class EmcRecGeoSvc:public Service,virtual public IEmcRecGeoSvc
{
 public:
   EmcRecGeoSvc(const std::string& name, ISvcLocator* svcloc);
   ~EmcRecGeoSvc();
   
   virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
   virtual StatusCode initialize ( );
   virtual StatusCode finalize ( );
   
   //access for geometry
   virtual EmcRecCrystal GetCrystal(const Identifier& id) const;
   virtual HepPoint3D    GetCrystalPoint(const Identifier& id, const int i) const;
   virtual HepPoint3D    GetCCenter(const Identifier& id) const;   
   virtual HepPoint3D    GetCFrontCenter(const Identifier& id) const;
   
   virtual double GetBarrelR() const;
   virtual double GetBarrelOffset1() const;
   virtual double GetBarrelOffset2() const;
   virtual double GetBarrelh1() const;
   virtual double GetBarrelh2() const;
   virtual double GetBarrelh3() const;
   virtual double GetBarrelL() const;
   virtual int GetBarrelNPhiMax() const;
   virtual int GetBarrelNThetaMax() const;

 private:
   EmcRecBarrelGeo fBarrel;
   EmcRecEndCapGeo fEndCap;
   EmcRecROOTGeo *fROOTGeo;

 private:
   bool fGdml;  //use gdml or code
};
   
#endif //EMC_REC_GEO_SVC_H

