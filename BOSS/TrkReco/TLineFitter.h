//-----------------------------------------------------------------------------
// $Id: TLineFitter.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TLineFitter.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a line.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TLINEFITTER_FLAG_
#define TLINEFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "TrkReco/TMFitter.h"

/// A class to fit a TTrackBase object to a line.
class TLineFitter : public TMFitter {

  public:
    /// Constructor.
    TLineFitter(const std::string & name);

    /// Destructor
    virtual ~TLineFitter();

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
    mutable double _a;
    mutable double _b;
    mutable double _det;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TLINEFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TLINEFITTER_INLINE_DEFINE_HERE

inline
double
TLineFitter::a(void) const {
    return _a;
}

inline
double
TLineFitter::b(void) const {
    return _b;
}

inline
double
TLineFitter::det(void) const {
    return _det;
}

#endif

#undef inline

#endif /* TLINEFITTER_FLAG_ */
