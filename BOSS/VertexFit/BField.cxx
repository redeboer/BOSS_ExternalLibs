#include <cassert>

#ifndef BEAN
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#endif

#include "VertexFit/BField.h"
#include "CLHEP/Geometry/Vector3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

using namespace std;

const double VertexFitBField::alpha = -0.00299792458;

VertexFitBField* VertexFitBField::s_bfield = 0;

VertexFitBField::VertexFitBField() {
#ifndef BEAN
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  m_pIMF = NULL;
  StatusCode sc = svcLocator->service("MagneticFieldSvc",m_pIMF);
  assert(m_pIMF != NULL);
  if (sc != StatusCode::SUCCESS) {
    std::cout << "ERROR : Unable to open Magnetic field service" << std::endl; 
//    assert(false);
  }
#else
  m_pIMF = MagneticFieldSvc::instance();
  assert( m_pIMF != 0 );
  if( (m_pIMF->GetPath()).empty() ) {
    cout << " VertexFitBField::ERROR "
        "You MUST set path to directory with magnetic fields tables" << endl;
    exit(1);
  }
  if( !m_pIMF->initialize() ) {
    cout << "ERROR : Can not initialize MagneticField. Stop." << endl;
    exit(1);
  }
#endif
}

double VertexFitBField::getBFieldZ(const HepPoint3D& vtx) {
  HepVector3D vector(0.0, 0.0, 0.0);
  // fixed 2008-8-1
  double radius = sqrt(vtx.x()*vtx.x() + vtx.y()*vtx.y());
  if (radius < 150 && abs(vtx.z()) < 150) {
    m_pIMF->fieldVector(10.0*vtx, vector);
    return 1000 * vector.z();   //unit of m_BFieldZ is Tesla
  } else {
    return 1000 * m_pIMF->getReferField(); 
  }  
}

double VertexFitBField::getBFieldZRef() {
  return 1000 * m_pIMF->getReferField();
}

double VertexFitBField::getCBz(const HepVector& vtx, const HepVector& trackPosition) {
  HepPoint3D Vtx(vtx[0], vtx[1], vtx[2]);
  HepPoint3D TrkPosition(trackPosition[0], trackPosition[1], trackPosition[2]);

  HepVector3D vector_vtx(0.0, 0.0, 0.0);
  HepVector3D vector_trk(0.0, 0.0, 0.0);
  double radius = sqrt(vtx[0]*vtx[0] + vtx[1]*vtx[1]);
  if (radius < 150 && abs(vtx[2]) < 150) {
    m_pIMF->fieldVector(10.0*Vtx, vector_vtx);
    m_pIMF->fieldVector(10.0*TrkPosition, vector_trk);
    return 1000 * alpha * (vector_vtx.z() + vector_trk.z())/2;//unit of m_BFieldZ is Tesla
  } else {
    return 1000 * alpha* m_pIMF->getReferField();
  }
}
