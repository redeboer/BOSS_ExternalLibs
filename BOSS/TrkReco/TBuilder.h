//-----------------------------------------------------------------------------
// $Id: TBuilder.h,v 1.10 2011/05/19 16:33:07 jialk Exp $
//-----------------------------------------------------------------------------
// Filename : TBuilder.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to build a track.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TBuilder_FLAG_
#define TBuilder_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/THelixFitter.h"

class TTrack;
class TMLink;
class TMLine;
class TSegment;

/// A class to build a track.
class TBuilder {

  public:
    /// Constructor with salvage level.
    TBuilder(const std::string & name,
	     float maxSigma,
	     float maxSigmaStereo,
	     float salvageLevel,
	     float szSegmentDistance,
	     float szLinkDistance,
	     unsigned fittingFlag);

    /// Destructor
    virtual ~TBuilder();

  public:// Selectors
    /// returns name.
    const std::string & name(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// builds a r/phi track from TMLinks or from Segments.
    TTrack * buildRphi(const AList<TMLink> &) const;
    TTrack * buildRphi(const AList<TSegment> &) const;
    TTrack * buildStereoNew(const TTrack & t,
			    AList<TSegment> & goodSegments,
			    AList<TSegment> & badSegments) const;
    TTrack * buildStereo(const TTrack & t, AList<TSegment> &) const;
    TTrack * buildStereo(TTrack & t,
			 TMLine & l,
			 const AList<TMLink> & links) const;
    TTrack * buildStereoOld(TTrack & t,
			    const AList<TMLink> & allLinks,
			    const AList<TMLink> & isolatedLinks) const;
    TTrack * build(TTrack & t, const TMLine & l) const;

    /// salvages hits.
    void salvage(TTrack & t, AList<TMLink> & hits) const;
    void salvage(TTrack & t, AList<TSegment> & segments) const;

    /// makes a line.
    TMLine * initialLine(const TTrack &, AList<TSegment> &) const;
    TMLine * initialLineOld(const TTrack &, AList<TSegment> &) const;
    TMLine * initialLine1(const TTrack &,
			 const AList<TSegment> &,
			 const AList<TMLink> &) const;
    TMLine * initialLine2(const TTrack &, const AList<TMLink> &) const;

    AList<TMLine> searchInitialLines(unsigned nSuperLayers) const;
    AList<TMLine> searchLines6(void) const;
    AList<TMLine> searchLines5(void) const;
    AList<TMLine> searchLines4(void) const;
    AList<TMLine> searchLines3(void) const;
    AList<TMLine> searchLines2(void) const;
    AList<TMLine> searchLines1(void) const;
    TMLine searchLine(const TMLine & initialLine) const;

    AList<TSegment> selectStereoSegment(const TMLine & line,
					const AList<TSegment> & list,
					const AList<TMLink> & szList) const;
    void removeFarSegment(const TMLine &,
			  AList<TSegment> &,
			  AList<TMLink> &) const;

  private:
    /// select good stereo segments.
    //void selectStereoSegments(const TTrack &, AList<TSegment> & list) const;

    /// initializes internal variables for stereo reconstruction. This should be called with new stereo codes.
    bool initializeForStereo(const TTrack &,
			     const AList<TSegment> &,
			     const AList<TSegment> &) const;

    /// checks stereo quality of a track.
    unsigned stereoQuality(const AList<TMLink> & links) const;

  private:
    std::string _name;
    THelixFitter _fitter;
    const float _maxSigma;
    const float _maxSigmaStereo;
    const float _salvageLevel;
    const float _szSegmentDistance;
    const float _szLinkDistance;

  private:// Parameters
    unsigned _minNLinksForCircle;
    unsigned _minNCores;

  private:
    mutable AList<TMLink> _allLinks;
    mutable AList<TMLink> _links[6]; //stereo 
    mutable AList<TMLink> _forLine;
    mutable unsigned _nSuperLayers;
    mutable unsigned _nHits[5];  //axial
};

//-----------------------------------------------------------------------------

#ifdef TBuilder_NO_INLINE
#define inline
#else
#undef inline
#define TBuilder_INLINE_DEFINE_HERE
#endif

#ifdef TBuilder_INLINE_DEFINE_HERE

inline
const std::string &
TBuilder::name(void) const {
    return _name;
}

#endif

#undef inline

#endif /* TBuilder_FLAG_ */
