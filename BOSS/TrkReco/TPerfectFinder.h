//-----------------------------------------------------------------------------
// $Id: TPerfectFinder.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TPerfectFinder.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to find tracks using MC info.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TPERFECTFINDER_FLAG_
#define TPERFECTFINDER_FLAG_
#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES
#include "TrkReco/TFinderBase.h"
#include "TrkReco/TMLink.h"
#include "TrkReco/TBuilder.h"
#include "TrkReco/THelixFitter.h"
template <class T> class AList;

/// A class to find tracks using MC info.
class TPerfectFinder : public TFinderBase {

  public:
    /// Constructor.
    TPerfectFinder(int perfectFitting,
		   float maxSigma,
		   float maxSigmaStereo,
		   unsigned fittingFlag);

    /// Destructor
    virtual ~TPerfectFinder();

  public:// Selectors
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// clear internal information.
    void clear(void);

    /// gets perfect momentum from MC info.
    bool perfectFitting(bool);

  public:// Modifiers
    /// finds tracks.
    int doit(const AList<TMDCWireHit> & axialHits,
	     const AList<TMDCWireHit> & stereoHits,
	     AList<TTrack> & tracks,
	     AList<TTrack> & tracks2D);

  private:
    float charge(int pType) const;

  private:
    bool _perfectFitting;
    AList<TMLink> _links;
    float _maxSigma;
    float _maxSigmaStereo;
    unsigned _fittingFlag;
    TBuilder _builder;
    THelixFitter _fitter;
};

//-----------------------------------------------------------------------------

#ifdef TPERFECTFINDER_NO_INLINE
#define inline
#else
#undef inline
#define TPERFECTFINDER_INLINE_DEFINE_HERE
#endif

#ifdef TPERFECTFINDER_INLINE_DEFINE_HERE

inline
std::string
TPerfectFinder::name(void) const {
    return "Perfect Finder";
}

inline
bool
TPerfectFinder::perfectFitting(bool a) {
    return _perfectFitting = a;
}

#endif

#undef inline

#endif /* TPERFECTFINDER_FLAG_ */
