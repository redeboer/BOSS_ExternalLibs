//-----------------------------------------------------------------------------
// $Id: TBuilder0.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TBuilder0.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to build a track.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TBuilder0_FLAG_
#define TBuilder0_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TMSelector.h"
#include "TrkReco/THelixFitter.h"
#include "TrkReco/TCosmicFitter.h"

class TTrack;
class TMLink;
class TSegment;

/// A class to build a track.
class TBuilder0 {

  public:
    /// Constructor.
    TBuilder0(const std::string & name);

    /// Constructor with salvage level.
    TBuilder0(const std::string & name,
	     float salvageLevel);

    /// Constructor with parameters.
    TBuilder0(const std::string & name,
	     float stereoZ3,
	     float stereoZ4,
	     float stereoChisq3,
	     float stereoChisq4,
	     float stereoMaxSigma,
	     unsigned fittingCorrections,
	     float salvageLevel);

    /// Destructor
    virtual ~TBuilder0();

  public:// Selectors
    /// returns name.
    const std::string & name(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a track selector.
    const TMSelector & trackSelector(void) const;

    /// builds a r/phi track from TMLinks or from Segments.
    TTrack * buildRphi(const AList<TMLink> &) const;

    /// appends stereo hits to a track. (old version)
    TTrack * buildStereo0(TTrack & track, const AList<TMLink> &) const;

    /// appends stereo hits to a track.
    virtual TTrack * buildStereo(TTrack & track, const AList<TMLink> &) const;

    /// appends TMLinks in a list.
    void appendClusters(TTrack & track, const AList<TMLink> &) const;

    /// salvages links in a list. Used links will be removed from a list.
    void salvage(TTrack & track, AList<TMLink> & list) const;

    /// fits a track using a private fitter.
    virtual int fit(TTrackBase &) const;

  public:// Modifiers
    /// sets a track selector.
    virtual const TMSelector & trackSelector(const TMSelector &);

  private://
    void selectHits(AList<TMLink> & list) const;
    // -50:bad consective, -20:not consective, -1:not in the same layer, 0:consective and chrg<=0, 1:consective and chrg>0
    int consectiveHits(TMLink &l, TMLink & s, int ichg) const;
    int check2CnHits(TMLink &l, TMLink & s, int ichg) const;
    int checkHits(unsigned i, unsigned j, unsigned k) const;
    void salvageNormal(TTrack & track, AList<TMLink> & list) const;

  private:
    std::string _name;
//    THelixFitter _fitter;
    TCosmicFitter _fitter;
    double _salvageLevel;

  protected:// for buildRphi
    TMSelector _circleSelector;
    TMSelector _trackSelector;

  protected:// for buildStereo
    TMSelector _lineSelector;
    float _stereoZ3;
    float _stereoZ4;
    float _stereoChisq3;
    float _stereoChisq4;
    float _stereoMaxSigma;
};

//-----------------------------------------------------------------------------

#ifdef TBuilder0_NO_INLINE
#define inline
#else
#undef inline
#define TBuilder0_INLINE_DEFINE_HERE
#endif

#ifdef TBuilder0_INLINE_DEFINE_HERE

inline
const TMSelector &
TBuilder0::trackSelector(void) const {
    return _trackSelector;
}

inline
const std::string &
TBuilder0::name(void) const {
    return _name;
}

inline
int
TBuilder0::fit(TTrackBase & a) const {
    return _fitter.fit(a);
}

#endif

#undef inline

#endif /* TBuilder0_FLAG_ */
