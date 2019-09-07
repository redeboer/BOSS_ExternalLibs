//
//   EmcRecROOTGeo
//
//   May 15, 2007, Created by Miao He
//
//   Construct ROOT geometry from gdml
//
#ifndef EMC_REC_ROOT_GEO_H
#define EMC_REC_ROOT_GEO_H

#include <map>
#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using namespace CLHEP;
class EmcRecCrystal;

class EmcRecROOTGeo
{
  public:
    // Constructors and destructors
    EmcRecROOTGeo();
    ~EmcRecROOTGeo();

  public:
    void InitFromXML();
    void FillCrystalMap(EmcRecCrystal&, const int, const int, const int);
    EmcRecCrystal GetCrystal(const Identifier& id) const;
    HepPoint3D    GetCCenter(const Identifier& id) const;
    HepPoint3D    GetCFrontCenter(const Identifier& id) const;
    void ComputeThetaPhi(const int part, const int theta, const int phi,
        int& newTheta, int& newPhi);

  private:
    std::map<Identifier, EmcRecCrystal> m_crystalMap;

};

#endif
