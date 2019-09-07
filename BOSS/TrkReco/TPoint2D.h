//-----------------------------------------------------------------------------
// $Id: TPoint2D.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TPoint2D.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a point in 2D.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TPOINT2D_FLAG_
#define TPOINT2D_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#include <math.h>
#define HEP_SHORT_NAMES
#include "CLHEP/Geometry/Vector3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

//class HepPoint3D;

/// A class to represent a point in 2D.
class TPoint2D {

  public:
    /// Constructors
    TPoint2D();
    TPoint2D(double, double);
    TPoint2D(const HepPoint3D &);
    TPoint2D(const HepVector3D &);

    /// Destructor
    virtual ~TPoint2D();

  public:// Selectors
    double x(void) const;
    double y(void) const;
    double mag(void) const;
    double mag2(void) const;
    double phi(void) const;

  public:// Modifiers
    double x(double);
    double y(double);

  public:// Operators
    double dot(const TPoint2D &) const;
    double cross(const TPoint2D &) const;
    TPoint2D unit(void) const;
    TPoint2D operator + (const TPoint2D &) const;
    TPoint2D operator - (const TPoint2D &) const;
    TPoint2D operator - () const;
    bool operator == (const TPoint2D &) const;

  private:
    double _p[2];
};

std::ostream &
operator << (std::ostream &, const TPoint2D &);

//-----------------------------------------------------------------------------

#ifdef TPOINT2D_NO_INLINE
#define inline
#else
#undef inline
#define TPOINT2D_INLINE_DEFINE_HERE
#endif
#ifdef TPOINT2D_INLINE_DEFINE_HERE

inline
double
TPoint2D::x(void) const {
    return _p[0];
}

inline
double
TPoint2D::y(void) const {
    return _p[1];
}

inline
double
TPoint2D::x(double a) {
    return _p[0] = a;
}

inline
double
TPoint2D::y(double a) {
    return _p[1] = a;
}

inline
double
TPoint2D::mag(void) const {
    return sqrt(_p[0] * _p[0] + _p[1] * _p[1]);
}

inline
double
TPoint2D::mag2(void) const {
    return _p[0] * _p[0] + _p[1] * _p[1];
}

inline
double
TPoint2D::phi(void) const {
    if (_p[0] == 0.0 && _p[1] == 0.0) return 0.;
    double a = atan2(_p[1], _p[0]);
    if (a > 0) return a;
    return a + 2. * M_PI;
}

inline
double
TPoint2D::dot(const TPoint2D & a) const {
    return _p[0] * a.x() + _p[1] * a.y();
}

inline
double
TPoint2D::cross(const TPoint2D & a) const {
    return _p[0] * a.y() - a.x() * _p[1];
}

inline
TPoint2D
TPoint2D::operator + (const TPoint2D & a) const {
    return TPoint2D(_p[0] + a.x(), _p[1] + a.y());
}

inline
TPoint2D
TPoint2D::operator - (const TPoint2D & a) const {
    return TPoint2D(_p[0] - a.x(), _p[1] - a.y());
}

inline
TPoint2D
TPoint2D::operator - () const {
    return TPoint2D(- _p[0], - _p[1]);
}

inline
bool
TPoint2D::operator == (const TPoint2D & a) const {
    if (a.x() == _p[0] && a.y() == _p[1]) return true;
    return false;
}

inline
TPoint2D
TPoint2D::unit(void) const {
    double sum2 = _p[0] * _p[0] + _p[1] * _p[1];
    if (sum2 == 0.) return TPoint2D(0., 0.);
    double sum = sqrt(sum2);
    return TPoint2D(_p[0] / sum, _p[1] / sum);
}

#endif
#undef inline

#endif /* TPOINT2D_FLAG_ */
