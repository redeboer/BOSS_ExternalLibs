//-----------------------------------------------------------------------------
// $Id: TFastFinder.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TFastFinder.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to find tracks with the conformal method.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TFastFinder_FLAG_
#define TFastFinder_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#include "TrkReco/TFinderBase.h"
#include "TrkReco/TBuilder0.h"
class TMLink;

/// A class to find tracks with the conformal method.
class TFastFinder : public TFinderBase {

  public:
    /// Constructor.
    TFastFinder();

    /// Destructor
    virtual ~TFastFinder();

  public:// Selectors
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers
    /// clear internal information.
    void clear(void);

    /// finds tracks.
    int doit(const AList<TMDCWireHit> & axialHits,
	     const AList<TMDCWireHit> & stereoHits,
	     AList<TTrack> & tracks);

  private:
    /// selects isolated hits.(conf.finder type)
    void selectHits(const AList<TMDCWireHit> & axialHits,
		    const AList<TMDCWireHit> & stereoHits);

    /// selects isolated hits.(calling selectSimpleSegments)
    void selectHits2(const AList<TMDCWireHit> & axialHits,
		     const AList<TMDCWireHit> & stereoHits);

    /// selects simple segments.
    void selectSimpleSegments(const AList<TMDCWireHit> & hits,
			      AList<TMDCWireHit> & output) const;

    /// finds close hits.
    AList<TMLink> findCloseHits(const AList<TMLink> & links,
			       const TTrack & track) const;

  private:
    AList<TMDCWireHit> _axialHits;
    AList<TMDCWireHit> _stereoHits;
    AList<TMLink> _axialLinks;
    AList<TMLink> _stereoLinks;

    TBuilder0 _builder;
    TMSelector _selector;
};

//-----------------------------------------------------------------------------

#ifdef TFastFinder_NO_INLINE
#define inline
#else
#undef inline
#define TFastFinder_INLINE_DEFINE_HERE
#endif

#ifdef TFastFinder_INLINE_DEFINE_HERE

inline
std::string
TFastFinder::name(void) const {
    return "Fast Finder";
}

#endif

#undef inline

#endif /* TFastFinder_FLAG_ */

