//
//  Bes Emc Rec basic date type
//
//  Created by Wang.Zhe  2003, 10, 1
//
#ifndef REC_EMC_DATA_TYPE_H
#define REC_EMC_DATA_TYPE_H

#include <vector>
#include <map>
#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include <CLHEP/Geometry/Point3D.h>

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace std;
using namespace CLHEP;

typedef Identifier RecEmcID; 
typedef double RecEmcADC;
typedef double RecEmcTDC;
typedef double RecEmcEnergy;
typedef double RecEmcTime;
typedef double RecEmcDistance;
typedef double RecEmcAngle;
typedef double RecEmcFrac;

typedef vector<RecEmcID> RecEmcIDVector;
typedef RecEmcIDVector::const_iterator ci_RecEmcIDVector;
typedef RecEmcIDVector::iterator i_RecEmcIDVector;

#endif  /* REC_EMC_DATA_TYPE_H */
