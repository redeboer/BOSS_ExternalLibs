//-----------------------------------------------------------------------------
// $Id: TFinderBase.h,v 1.4 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TFinderBase.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A virtual class for a track finder in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TFinderBase_FLAG_
#define TFinderBase_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"

class TMDCWireHit;
class TTrack;

/// A virtual class for a track finder in tracking.
class TFinderBase {

  public:
    /// Constructor
    TFinderBase();

    /// Destructor
    virtual ~TFinderBase();

  public:// General interfaces
    /// returns name.
    virtual std::string name(void) const = 0;

    /// returns version.
    virtual std::string version(void) const= 0;

    /// dumps debug information.
    virtual void dump(const std::string & message = std::string(""),
		      const std::string & prefix = std::string("")) const;

    /// returns debug level.
    virtual int debugLevel(void) const;

  public:// Modifiers
    /// clear internal information.
    virtual void clear(void) = 0;

    /// sets debug level.
    virtual int debugLevel(int);

    /// sets flag to reconstruct 3D.
    virtual bool doStereo(bool);

    /// sets flag to salvage hits.
    virtual bool doSalvage(bool);

    /// finds tracks. 'hits' are used to reconstruct. 'tracks' can be used for both inputs and outputs. Return value = (0, +, -) means (success, warning, fatal error).
    virtual int doit(const AList<TMDCWireHit> & axialHits,
		     const AList<TMDCWireHit> & stereoHits,
		     AList<TTrack> & tracks3D,
		     AList<TTrack> & tracks2D) = 0;

  private:
    int _debugLevel;
};

//-----------------------------------------------------------------------------

#ifdef TFinderBase_NO_INLINE
#define inline
#else
#undef inline
#define TFinderBase_INLINE_DEFINE_HERE
#endif

#ifdef TFinderBase_INLINE_DEFINE_HERE

inline
int
TFinderBase::debugLevel(void) const {
    return _debugLevel;
}

inline
int
TFinderBase::debugLevel(int a) {
    return _debugLevel = a;
}

inline
bool
TFinderBase::doStereo(bool a) {
    return a;
}

inline
bool
TFinderBase::doSalvage(bool a) {
    return a;
}

#endif

#undef inline

#endif /* TFinderBase_FLAG_ */
