//
// $Id: Helix.h,v 1.4 2011/05/12 10:25:28 wangll Exp $
//
//   Class Helix 
//
//   Author      Date         comments
//   Y.Ohnishi   03/01/1997   original version
//   Y.Ohnishi   06/03/1997   updated
//   Y.Iwasaki   17/02/1998   BFILED removed, func. name changed, func. added
//   J.Tanaka    06/12/1998   add some utilities.
//   Y.Iwasaki   07/07/1998   cache added to speed up
//   Y.Iwasaki   25/04/2001   cache m_ac[5] added to speed up
//
#ifndef Helix_FLAG_
#define Helix_FLAG_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
//#ifndef CLHEP_THREEVECTOR_H
#include "CLHEP/Vector/ThreeVector.h"
//#endif
#include "CLHEP/Vector/LorentzVector.h"
//#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
//#endif

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using namespace CLHEP;

using HepGeom::Point3D;

typedef CLHEP::HepVector Vector;
typedef CLHEP::HepSymMatrix SymMatrix;
typedef CLHEP::Hep3Vector Vector3;
typedef CLHEP::HepLorentzVector VectorL;
typedef CLHEP::HepLorentzVector Vector4;
typedef CLHEP::HepMatrix Matrix;

using CLHEP::HepVector;
using CLHEP::HepSymMatrix;
using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;


/// Helix parameter class
class Helix {

  public:
    /// Constructor with pivot, helix parameter a, and its error matrix.
    Helix(const HepPoint3D & pivot,
	  const HepVector & a,
	  const HepSymMatrix & Ea);

    /// Constructor without error matrix.
    Helix(const HepPoint3D & pivot,
	  const HepVector & a);

    /// Constructor with position, momentum, and charge.
    Helix(const HepPoint3D & position,
	  const Hep3Vector & momentum,
	  double charge);

    /// Destructor
    virtual ~Helix();

  public:// Selectors
    /// returns position of helix center(z = 0.);
    const HepPoint3D & center(void) const;

    /// returns pivot position.
    const HepPoint3D & pivot(void) const;

    /// returns radious of helix.
    double radius(void) const;

    /// returns position after rotating angle dPhi in phi direction.
    HepPoint3D x(double dPhi = 0.) const;
    double * x(double dPhi, double p[3]) const;

    /// returns position and convariance matrix(Ex) after rotation.
    HepPoint3D x(double dPhi, HepSymMatrix & Ex) const;

    /// returns direction vector after rotating angle dPhi in phi direction.
    Hep3Vector direction(double dPhi = 0.) const;

    /// returns momentum vector after rotating angle dPhi in phi direction.
    Hep3Vector momentum(double dPhi = 0.) const;

    /// returns momentum vector after rotating angle dPhi in phi direction.
    Hep3Vector momentum(double dPhi, HepSymMatrix & Em) const;

    /// returns 4momentum vector after rotating angle dPhi in phi direction.
    HepLorentzVector momentum(double dPhi, double mass) const;

    /// returns 4momentum vector after rotating angle dPhi in phi direction.
    HepLorentzVector momentum(double dPhi, double mass, HepSymMatrix & Em) const;

    /// returns 4momentum vector after rotating angle dPhi in phi direction.
    HepLorentzVector momentum(double dPhi, double mass, HepPoint3D & x, HepSymMatrix & Emx) const;  

  public:// Parametrization dependent functions. Prepared for tracking codes. Users should not use them.
    /// returns an element of parameters.
    double dr(void) const;
    double phi0(void) const;
    double kappa(void) const;
    double dz(void) const;
    double tanl(void) const;
    double curv(void) const;
    double sinPhi0(void) const;
    double cosPhi0(void) const;

    /// returns helix parameters.
    const HepVector & a(void) const;

    /// returns error matrix.
    const HepSymMatrix & Ea(void) const;

    double pt(void) const{ return m_pt; }
    double cosTheta(void) const{ return m_a[4]/sqrt(1.+ m_a[4]*m_a[4]); }
  public:// Modifiers
    /// sets helix parameters.
    const HepVector & a(const HepVector & newA); 

    /// sets helix paramters and error matrix.
    const HepSymMatrix & Ea(const HepSymMatrix & newdA);

    /// sets pivot position.
    const HepPoint3D & pivot(const HepPoint3D & newPivot);

    /// sets helix pivot position, parameters, and error matrix.
    void set(const HepPoint3D & pivot,
	     const HepVector & a,
	     const HepSymMatrix & Ea);

    /// unsets error matrix. Error calculations will be ignored after this function call until an error matrix be set again. 0 matrix will be return as a return value for error matrix when you call functions which returns an error matrix.
    void ignoreErrorMatrix(void);

    /// sets/returns z componet of the magnetic field.
    double bFieldZ(double);
    double bFieldZ(void) const;

  public:// Operators
    /// Copy operator
    Helix & operator = (const Helix &);

  public:// Mathmatical functions
    HepMatrix delApDelA(const HepVector & ap) const;
    HepMatrix delXDelA(double phi) const;
    HepMatrix delMDelA(double phi) const;
    HepMatrix del4MDelA(double phi, double mass) const;
    HepMatrix del4MXDelA(double phi, double mass) const;

  //private:
  protected:
    IMagneticFieldSvc* m_pmgnIMF;
    double m_bField;
    double m_alpha;

  private:
    void updateCache(void);

  public:
    /// Constant alpha for uniform field.
    static const double ConstantAlpha;

  private:
    //double m_bField;
    //double m_alpha;
    HepPoint3D m_pivot;
    HepVector m_a;
    HepSymMatrix m_Ea;
    bool m_matrixValid;

  private: // caches
    HepPoint3D m_center;
    double m_cp;
    double m_sp;
    double m_pt;
    double m_r;
    double m_ac[5];
};

//-----------------------------------------------------------------------------

#ifdef Helix_NO_INLINE
#define inline
#else
#undef inline
#define Helix_INLINE_DEFINE_HERE
#endif

#ifdef Helix_INLINE_DEFINE_HERE

inline
const HepPoint3D &
Helix::center(void) const {
    return m_center;
}

inline
const HepPoint3D &
Helix::pivot(void) const {
    return m_pivot;
}

inline
double
Helix::radius(void) const {
    return m_r;
}

inline
Hep3Vector
Helix::direction(double phi) const {
    return momentum(phi).unit();
}

inline
double
Helix::dr(void) const {
    return m_ac[0];
}

inline
double
Helix::phi0(void) const {
    return m_ac[1];
}

inline
double
Helix::kappa(void) const {
    return m_ac[2];
}

inline
double
Helix::dz(void) const {
    return m_ac[3];
}

inline
double
Helix::tanl(void) const {
    return m_ac[4];
}

inline
double
Helix::curv(void) const {
    return m_r;
}

inline
const HepVector &
Helix::a(void) const {
    return m_a;
}

inline
const HepSymMatrix &
Helix::Ea(void) const {
    return m_Ea;
}

inline
const HepVector &
Helix::a(const HepVector & i) {
    m_a = i;
    updateCache();
    return m_a;
}

inline
const HepSymMatrix &
Helix::Ea(const HepSymMatrix & i) {
    return m_Ea = i;
}

inline
double
Helix::bFieldZ(double a) {
    m_bField = a;
    m_alpha = 10000. / 2.99792458 / m_bField;
    updateCache();
    return m_bField;
}

inline
double
Helix::bFieldZ(void) const {
    return m_bField;
}

inline
double
Helix::sinPhi0(void) const {
    return m_sp;
}

inline
double
Helix::cosPhi0(void) const {
    return m_cp;
}

#endif

#undef inline

#endif /* Helix_FLAG_ */
