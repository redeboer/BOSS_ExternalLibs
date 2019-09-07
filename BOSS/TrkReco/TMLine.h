//-----------------------------------------------------------------------------
// $Id: TMLine.h,v 1.3 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMLine.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a line in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMLine_FLAG_
#define TMLine_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES

#include "TrkReco/TTrackBase.h"
#include "TrkReco/TMLink.h"
#include "TrkReco/TLineFitter.h"

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

//class HepPoint3D;

/// A class to represent a track in tracking.
class TMLine : public TTrackBase {

  public:
    /// Constructor.
    TMLine();

    /// Constructor.
    TMLine(const AList<TMLink> &);

    /// Destructor
    virtual ~TMLine();

  public:// Selectors
    /// returns type.
    virtual unsigned objectType(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns coefficient a.
    double a(void) const;

    /// returns coefficient b.
    double b(void) const;

    /// returns chi2.
    double chi2(void) const;

    /// returns reduced-chi2.
    double reducedChi2(void) const;

  public:// Utilities
    /// returns distance to a position of TMLink itself. (not to a wire)
    double distance(const TMLink &) const;
    double distance(const HepPoint3D &) const;

  public:// Modifiers
    /// fits itself. Error was happened if return value is not zero.
    // int fitx(void);

    /// fits itself using isolated hits. Error was happened if return value is not zero.
    int fit2();

    /// fits itself using single hits in a wire-layer. Error was happened if return value is not zero.
    int fit2s();

    /// fits itself using isolated hits. Error was happened if return value is not zero.
    int fit2p();

    /// fits itself using single hits in a wire-layer. Error was happened if return value is not zero.
    int fit2sp();

    /// remove extremly bad points.
    void removeChits();

    /// remove bad points by chi2. Bad points are returned in a 'list'. fit() should be called before calling this function.
    void refine(AList<TMLink> & list, float maxSigma);

    /// 
    void removeSLY(AList<TMLink> & list);

    /// 
    void appendSLY(AList<TMLink> & list);

    /// 
    void appendByszdistance(AList<TMLink> & list, unsigned isl, float maxSigma);

    /// sets circle properties.
    void property(double a, double b, double det);

  private:// Always updated
    mutable bool _fittedUpdated;

  private:// Updated when fitted
    double _a;
    double _b;
    double _det;
    static const TLineFitter _fitter;

  private:// Updated when fitted and accessed
    mutable double _chi2;
    mutable double _reducedChi2;
};

/// Sorter
#if defined(__GNUG__)
int
SortByB(const TMLine ** a, const TMLine ** b);
#else
extern "C" int
SortByB(const void* a, const void* b);
#endif

//-----------------------------------------------------------------------------

#ifdef TMLine_NO_INLINE
#define inline
#else
#undef inline
#define TMLine_INLINE_DEFINE_HERE
#endif

#ifdef TMLine_INLINE_DEFINE_HERE

inline
double
TMLine::a(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TMLine::a !!! fit not performed" << std::endl;
#endif
    return _a;
}

inline
double
TMLine::b(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TMLine::b !!! fit not performed" << std::endl;
#endif
    return _b;
}

inline
double
TMLine::distance(const TMLink & l) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TMLine::distance !!! fit not performed" << std::endl;
#endif
    double dy = fabs(_a * l.position().x() + _b - l.position().y());
    double invCos = sqrt(1. + _a * _a);
    return dy / invCos;
}

inline
double
TMLine::distance(const HepPoint3D & p) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TMLine::distance !!! fit not performed" << std::endl;
#endif
    double dy = fabs(_a * p.x() + _b - p.y());
    double invCos = sqrt(1. + _a * _a);
    return dy / invCos;
}

inline
void
TMLine::property(double a, double b, double det) {
    _a = a;
    _b = b;
    _det = det;
}

inline
unsigned
TMLine::objectType(void) const {
    return Line;
}

#endif

#undef inline

#endif /* TMLine_FLAG_ */
