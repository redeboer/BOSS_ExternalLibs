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



#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

 using CLHEP::HepVector; 
 using CLHEP::Hep3Vector;
 using CLHEP::HepMatrix;
 using CLHEP::HepSymMatrix;

using namespace CLHEP;

//class HepPoint3D;


//class Bfield;

class Bfield {
  
public:
  /// returns Bfield object.
  static Bfield * getBfield(int);
  
protected:
  /// Constructor, Destructor
  Bfield(int);
  virtual ~Bfield(void) {};
  
public://selectors
  
  ///returns B field Map ID
  int fieldID(void) const {return _fieldID;}
  
  ///returns B field
  const Hep3Vector & fieldMap(double x, double y, double z) const;
  const Hep3Vector & fieldMap(const HepPoint3D &xyz) const;
  void fieldMap(double *position, double *bfield);
  
  ///returns an element of B field
  double bx(double x, double y, double z) const;
  double by(double x, double y, double z) const;
  double bz(double x, double y, double z) const;
  double bx(const HepPoint3D &xyz) const;
  double by(const HepPoint3D &xyz) const;
  double bz(const HepPoint3D &xyz) const;
  
private:
  static  Bfield *_field[200];
  double _Bz[175][399];
  double _Br[175][399];
  double _Bphi[175][399];
  double _BzQR[101][163];
  double _BrQR[101][163];
  double _BphiQR[101][163];
  double _BzQL[17][51][52];
  double _BrQL[17][51][52];
  double _BphiQL[17][51][52];
  int   _fieldID;
  mutable double m_Bx;
  mutable double m_By;
  mutable double m_Bz;
  mutable double m_x;
  mutable double m_y;
  mutable double m_z;
  mutable Hep3Vector m_Bfld;
  
private:
  void updateCache(double, double, double) const;
};

#endif /* BFIELD_H */
