//-----------------------------------------------------------------------------
// $Id: TMFitter.h,v 1.3 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMFitter.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMFITTER_FLAG_
#define TMFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES

#include <string>

class TTrackBase;
class TTrack;

#define TFitAlreadyFitted 1;
#define TFitErrorFewHits -1;
#define TFitFailed       -2;
#define TFitUnavailable  -3;

/// A class to fit a TTrackBase object.
class TMFitter {

  public:
    /// Constructor.
    TMFitter(const std::string & name);

    /// Destructor
    virtual ~TMFitter();

  public:// Selectors
    /// returns name.
    const std::string & name(void) const;
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Fit functions.
    virtual int fit(TTrackBase &) const = 0;

  protected:
    /// sets the fitted flag. (Bad implementation)
    void fitDone(TTrackBase &) const;

  private:
    std::string _name;
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TMFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TMFITTER_INLINE_DEFINE_HERE

inline
const std::string &
TMFitter::name(void) const {
    return _name;
}

#endif

#undef inline

#endif /* TMFITTER_FLAG_ */
