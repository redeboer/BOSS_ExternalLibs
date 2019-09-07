// -*- C++ -*-
//
// Bfield class
//
// 27-Mar-1999 : KUNIYA Toshio
//   Enabled QCS compornent, phi compornent
//
// 21-Feb-1999 : KUNIYA Toshio
//   Keeping comatibility, Bfield class is modified.
//   No longer fortran common block is used for bfield map.
//   Access functions are prepared for fortran call.
//
// 22-Feb-1999 : KUNIYA Toshio
//   constructor, destructor are go to protected entries.
// 

#if !defined(BFIELD_H)
#define BFIELD_H

#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using HepGeom::Point3D;

//class HepPoint3D;

//class Bfield;

class Bfield {
  
public:
  /// returns Bfield object.
  static Bfield * getBfield(int);
  
protected:
  /// Constructor, Destructor
  Bfield(int);
//zsl  virtual ~Bfield(void) {};
  
public://selectors
  
  ///returns B field Map ID
  int fieldID(void) const {return _fieldID;}
  
  ///returns B field
  const Hep3Vector & fieldMap(float x, float y, float z) const;
  const Hep3Vector & fieldMap(const HepPoint3D &xyz) const;
  void fieldMap(float *position, float *bfield);
  
  ///returns an element of B field
  float bx(float x, float y, float z) const;
  float by(float x, float y, float z) const;
  float bz(float x, float y, float z) const;
  float bx(const HepPoint3D &xyz) const;
  float by(const HepPoint3D &xyz) const;
  float bz(const HepPoint3D &xyz) const;
  
private:
  static  Bfield *_field[200];
  float _Bz[175][399];
  float _Br[175][399];
  float _Bphi[175][399];
  float _BzQR[101][163];
  float _BrQR[101][163];
  float _BphiQR[101][163];
  float _BzQL[17][51][52];
  float _BrQL[17][51][52];
  float _BphiQL[17][51][52];
  int   _fieldID;
  mutable float m_Bx;
  mutable float m_By;
  mutable float m_Bz;
  mutable float m_x;
  mutable float m_y;
  mutable float m_z;
  mutable Hep3Vector m_Bfld;
  
private:
  void updateCache(float, float, float) const;
};

#endif /* BFIELD_H */
