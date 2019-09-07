//-----------------------------------------------------------------------------
// $Id: TCircle.h,v 1.13 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TCircle.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a circle in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TCircle_FLAG_
#define TCircle_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using HepGeom::Point3D;

#include "TrkReco/TMDCUtil.h"
#include "TrkReco/TTrackBase.h"
#include "TrkReco/TCircleFitter.h"
//#include "lpav/Lpav.h"
//#include "TrkReco/Lpav.h"
#include "TrackUtil/Lpav.h"

/// A class to represent a circle in tracking.
class TCircle : public TTrackBase {

  public:
    /// Constructor.
    TCircle(const AList<TMLink> & links);

    /// Destructor
    virtual ~TCircle();

  public:// Selectors
    /// returns type.
    virtual unsigned objectType(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns position of center.
    const HepPoint3D & center(void) const;

    /// returns radius.
    double radius(void) const;

    /// returns Pt.
    double pt(void) const;

    /// returns impact parameter to the origin.
    double impact(void) const;

    /// returns charge.
    double charge(void) const;

    /// returns weight of TMLink in order to fit and make a circle.
    double weight(const TMLink & l) const;

  public:// Modifiers
    /// fits itself. Error was happened if return value is not zero.
    // int fitx(void);

    /// fits itself only for curl finder. Error was happened if return value is not zero.
    int fitForCurl(int ipConst = 0);

    /// sets circle properties.
    void property(double charge, double radius, HepPoint3D center);

  private:// Updated when fitted
    Lpav _circle;
    double _charge;
    double _radius;
    HepPoint3D _center;
    static const TCircleFitter _fitter;
};

//-----------------------------------------------------------------------------

#ifdef TCircle_NO_INLINE
#define inline
#else
#undef inline
#define TCircle_INLINE_DEFINE_HERE
#endif

#ifdef TCircle_INLINE_DEFINE_HERE

inline
const HepPoint3D &
TCircle::center(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TCircle::center !!! fit not performed" << std::endl;
#endif
    return _center;
}

inline
double
TCircle::radius(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TCircle::radius !!! fit not performed" << std::endl;
#endif
    return _radius;
}

inline
double
TCircle::pt(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TCircle::pt !!! fit not performed" << std::endl;
#endif
   // return fabs(_radius) / Helix::ConstantAlpha;
    return fabs(_radius) / 333.564095;
}

inline
double
TCircle::impact(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TCircle::impact !!! fit not performed" << std::endl;
#endif
    return fabs(sqrt(_center.x() * _center.x() +
		     _center.y() * _center.y())
		- fabs(_radius));
}

inline
double
TCircle::charge(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TCircle::charge !!! fit not performed" << std::endl;
#endif
    return _charge;

}

inline
void
TCircle::property(double c, double r, HepPoint3D e) {
    _charge = c;
    _radius = r;
    _center = e;
}

inline
unsigned
TCircle::objectType(void) const {
    return Circle;
}

#endif

#undef inline

#endif /* TCircle_FLAG_ */
