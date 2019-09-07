//-----------------------------------------------------------------------------
// $Id: TLine0.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TLine0.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a line in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TLine0_FLAG_
#define TLine0_FLAG_

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

/// A class to represent a track in tracking.
class TLine0 : public TTrackBase {

  public:
    /// Constructor.
    TLine0();

    /// Constructor.
    TLine0(const AList<TMLink> &);

    /// Destructor
    virtual ~TLine0();

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

//-----------------------------------------------------------------------------

#ifdef TLine0_NO_INLINE
#define inline
#else
#undef inline
#define TLine0_INLINE_DEFINE_HERE
#endif

#ifdef TLine0_INLINE_DEFINE_HERE

inline
double
TLine0::a(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TLine0::a !!! fit not performed" << std::endl;
#endif
    return _a;
}

inline
double
TLine0::b(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TLine0::b !!! fit not performed" << std::endl;
#endif
    return _b;
}

inline
double
TLine0::distance(const TMLink & l) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TLine0::distance !!! fit not performed" << std::endl;
#endif
    double dy = fabs(_a * l.position().x() + _b - l.position().y());
    double invCos = sqrt(1. + _a * _a);
    return dy / invCos;
}

inline
void
TLine0::property(double a, double b, double det) {
    _a = a;
    _b = b;
    _det = det;
}

inline
unsigned
TLine0::objectType(void) const {
    return Line;
}

#endif

#undef inline

#endif /* TLine0_FLAG_ */
