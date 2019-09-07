#ifndef VertexFit_BField_H
#define VertexFit_BField_H

//
// Read field from MagneticField Service in Gaudi framework 
// Author: Min Xu  data: 4/16/2008, created 

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Vector.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#ifndef BEAN
#include "MagneticField/IMagneticFieldSvc.h"
#else
#include "MagneticField/MagneticFieldSvc.h"
#endif

using CLHEP::HepVector;

class VertexFitBField {
 public :
  static VertexFitBField* instance() {
    if (s_bfield == 0) {
      s_bfield = new VertexFitBField();
    }
    return s_bfield;
  }
  // constructor & deconstructor
  ~VertexFitBField() {}

  double getBFieldZ(const HepPoint3D& vtx);
  double getBFieldZRef();
  // get B*c = B*-0.00299792458
  double getCBz(const HepVector& vtx, const HepVector& trackPosition); 
    
 private :
  VertexFitBField();

#ifndef BEAN
  IMagneticFieldSvc* m_pIMF;
#else
  MagneticFieldSvc* m_pIMF;
#endif

  static VertexFitBField* s_bfield;
  static const double alpha;
};
#endif
