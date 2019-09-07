//-----------------------------------------------------------------------------
// $Id: TRobustLineFitter.h,v 1.4 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TRobustLineFitter.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a line.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TROBUSTLINEFITTER_FLAG_
#define TROBUSTLINEFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "TrkReco/TLineFitter.h"

/// A class to fit a TTrackBase object to a line.
class TRobustLineFitter : public TLineFitter {

  public:
    /// Constructor.
    TRobustLineFitter(const std::string & name);

    /// Destructor
    virtual ~TRobustLineFitter();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;
    double a(void) const;
    double b(void) const;
    double det(void) const;

  public:// Modifiers
    virtual int fit(TTrackBase &) const;

  private:
    double rofunc(const TTrackBase &, double) const;
    double select(unsigned k, unsigned n, double *) const;

  private:
    mutable double _a;
    mutable double _b;
    mutable double _det;
    mutable unsigned _n;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TROBUSTLINEFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TROBUSTLINEFITTER_INLINE_DEFINE_HERE

inline
double
TRobustLineFitter::a(void) const {
    return _a;
}

inline
double
TRobustLineFitter::b(void) const {
    return _b;
}

inline
double
TRobustLineFitter::det(void) const {
    return _det;
}

#endif

#undef inline

#endif /* TROBUSTLINEFITTER_FLAG_ */
