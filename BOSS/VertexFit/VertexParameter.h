#ifndef Vertex_Parameter_H
#define Vertex_Parameter_H

//
//  define the vertex parameter 
//
//  Author: K.L. He   date 10/31/2005, created
//
//
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
   typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
using CLHEP::HepSymMatrix;
using CLHEP::HepVector;
class VertexParameter {

 public:

  VertexParameter(){ m_Evx = HepSymMatrix(3,0); m_vx=HepVector(3, 0);}
  ~VertexParameter(){;}

  inline VertexParameter(const VertexParameter &);

  inline void setVx(const HepPoint3D &vx);
  void setVx(const HepVector &vx) { m_vx = vx;}
  void setEvx(const HepSymMatrix &eVx) {m_Evx = eVx;}
  HepPoint3D vx() const{return HepPoint3D(m_vx[0], m_vx[1], m_vx[2]);}
  HepVector  Vx() const{return m_vx;}
  HepVector x() const {return m_vx;}
  HepSymMatrix Evx() const {return m_Evx;}
  HepSymMatrix Ex() const {return m_Evx;}
  inline VertexParameter & operator = (const VertexParameter &);

 private:

  HepVector m_vx;   // vertex point (x, y, z)
  HepSymMatrix m_Evx; // error matrix 
};

inline VertexParameter::VertexParameter(const VertexParameter & vpar) {
  m_vx = vpar.m_vx;
  m_Evx = vpar.m_Evx;
}

inline VertexParameter & VertexParameter::operator = (const VertexParameter & vpar) {
  m_vx = vpar.m_vx;
  m_Evx = vpar.m_Evx;
  return (*this);
}

inline void VertexParameter::setVx(const HepPoint3D &vx) {
  m_vx[0] = vx.x();
  m_vx[1] = vx.y();
  m_vx[2] = vx.z();
}
#endif

