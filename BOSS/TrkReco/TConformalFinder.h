//-----------------------------------------------------------------------------
// $Id: TConformalFinder.h,v 1.24 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TConformalFinder.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to find tracks with the conformal method.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TConformalFinder_FLAG_
#define TConformalFinder_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES
#include "CLHEP/Alist/ConstAList.h"
#include "TrkReco/TFinderBase.h"
#include "TrkReco/TMLink.h"
#include "TrkReco/TSegment.h"
#include "TrkReco/TBuilder.h"

#ifdef TRKRECO_DEBUG
#define ConformalInitialization 0
#define ConformalFast2DLevel0 10
#define ConformalFast2DLevel1 11
#define ConformalFast3DLevel0 20
#define ConformalFast3DLevel1 21
#define ConformalSlow2D 40
#define ConformalSlow3D 41
#define ConformalOutside 99
#endif
#ifdef TRKRECO_WINDOW
#include "TrkReco/TWindow.h"
#endif

class TMDC;
class TPoint2D;

/// A class to find tracks with the conformal method.
class TConformalFinder : public TFinderBase {

  public:
    /// Constructor.
    TConformalFinder(unsigned fastFinder,
		     unsigned slowFinder,
		     unsigned perfectSegmentFinding,
		     float maxSigma,
		     float maxSigmaStereo,
		     float salvageLevel,
		     unsigned minNLinksForSegment,
		     unsigned minNCoreLinks,
		     unsigned minNSegments,
		     unsigned salvageLoadWidth,
		     unsigned stereoMode,
		     unsigned stereoLoadWidth,
		     float szSegmentDistance,
		     float szLinkDistance,
		     unsigned fittingFlag);

    /// Destructor
    virtual ~TConformalFinder();

  public:// Selectors
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns T0 reset is done.
    bool T0ResetDone(void) const;

  public:// Modifiers
    /// clear internal information.
    void clear(void);

    /// finds tracks.
    int doit(const AList<TMDCWireHit> & axialHits,
	     const AList<TMDCWireHit> & stereoHits,
	     AList<TTrack> & tracks,
	     AList<TTrack> & tracks2D);

    /// sets a flag to do T0 reset in the fast 2D finding.
    bool doT0Reset(bool);

  private:
    /// selects good hits.
    void selectGoodHits(void);

    /// finds segments from _goodHits.
    void findSegments(void);
    void findSegmentsPerfect(void);
    /// finds segments from all hits.
    void findSegmentsTsf(void);

    /// links segments.
    void linkSegments(unsigned level);
    void linkSegmentsTsf(void);

    /// fast findings. level:0/1:selection of segments to be used
    void fastFinding2D(unsigned level);
    void fastFinding3D(unsigned level);

    /// slow findings. level:0/1:selection of segments to be used
    void slowFinding2D(unsigned level);
    TTrack * expand(AList<TSegment> &) const;
    TTrack * expand(TTrack &) const;
    void targetSuperLayer(unsigned ptn, unsigned & in, unsigned & out) const;
    AList<TSegment> targetSegments(const TTrack &, unsigned sl) const;
    AList<TMLink> targetLinks(const TTrack &, unsigned sl) const;
    bool trackQuality(const TTrack &) const;

    /// finds setereo segments.
    AList<TSegment> stereoSegments(const TTrack & t) const;
    AList<TSegment> stereoSegmentsFromBadHits(const TTrack & t) const;

    /// salvages segments.
    void salvage(TTrack & track,
		 unsigned axialStereoSwitch,
		 const AList<TSegment> & bads) const;

    /// removes bad segments.
    AList<TSegment> removeBadSegments(TTrack &) const;
    AList<TSegment> refineSegments(const TTrack &) const;
    void refineLinks(TTrack &, unsigned minNHits) const;

    /// resolves multi-assignment of segments/hits.
    void resolveSegments(AList<TTrack> & tracks) const;
    void resolveHits(AList<TTrack> & tracks) const;

    /// remove used segments.
    void removeUsedSegments(const AList<TTrack> & tracks);

    /// deletes a track.
    void deleteTrack(TTrack &) const;

    /// re-calculate TMLinks.
    void updateTLinks(AList<TTrack> & tracks);

    /// Utility functions
    int crossPoints(const TTrack &, TPoint2D points[12]) const;
    int crossPointsInConformal(const AList<TSegment> &,
			       HepPoint3D points[12]) const;
    AList<TSegment> pickUpSegments(const TPoint2D p[12],
				   float loadWidth,
				   unsigned axialStereo) const;
    AList<TSegment> pickUpSegmentsInConformal(float phi[12],
					      float loadWidth,
					      unsigned axialStereo) const;
    AList<TMLink> pickUpLinks(const TPoint2D p[12],
			     float loadWidth,
			     unsigned axialStereo) const;
    AList<TMLink> pickUpLinksInConformal(float phi[12],
					float loadWidth,
					unsigned axialStereo) const;
    AList<TMLink> trackSide(const TTrack &, const AList<TMLink> &) const;
    AList<TSegment> trackSide(const TTrack &, const AList<TSegment> &) const;
    bool quality2D(TTrack & t, unsigned level) const;
    TSegment * link(const TSegment & seed,
		    const HepPoint3D & p,
		    const HepVector3D & v,
		    const AList<TSegment> & candidates,
		    AList<TSegment> & alternatives,
		    unsigned level) const;
    HepVector3D direction(const TSegment &) const;

  public:// Utility functions
    static const TMDCWire * conformal2Wire(const HepPoint3D & conformalPoint);

  private:// Parameters
    bool _doT0Reset;
    const unsigned _perfectSegmentFinding;
    const unsigned _segmentSeparation;
    const unsigned _minNLinksForSegment;
    const unsigned _minNLinksForSegmentInRefine;
    const unsigned _maxNLinksForSegment;
    const unsigned _maxWidthForSegment;
    const unsigned _minNCoreLinks;
    const unsigned _minNSegments;
    const float _minUsedFractionSlow2D;
    const unsigned _appendLoad;
    const unsigned _salvageLoadWidth;
    const unsigned _stereoMode;
    const unsigned _stereoLoadWidth;
    float _linkMaxDistance[3];
    float _linkMinDirAngle[3];

  private:
    IMagneticFieldSvc* m_pmgnIMF;
    const bool _fastFinder;
    const bool _slowFinder;
    const TBuilder _builder;
    const float _maxSigma2;

    AList<TMLink> _allHits[3];     // 0:axial, 1:stereo, 2:both
    AList<TMLink> _hits[3];        // 0:axial, 1:stereo, 2:both
    AList<TMLink> _unused[3];      // 0:axial, 1:stereo, 2:both

    AList<TSegment> _allSegments[2][6]; // 0:axial, 1:stereo
    AList<TSegment> _allUnused[2][6];   // 0:axial, 1:stereo
    AList<TSegment> _stereoBadSegments;

    AList<TTrack> _2DTracks;
    AList<TTrack> _3DTracks;

    bool _T0ResetDone;

    struct summary {
	unsigned _nEvents;
	unsigned _nTracksFast3D;
	unsigned _nTracksSlow3D;
	unsigned _nTracksFast2D;
	unsigned _nTracksSlow2D;
	unsigned _nTracksFast2DBadQuality;
	unsigned _nTracksSlow2DBadQuality;
	unsigned _dummy;
    };
    struct summary * _s;

#ifdef TRKRECO_DEBUG
  public:
    static unsigned _stage;
#endif

#ifdef TRKRECO_WINDOW
  private:
    mutable TWindow _rphiWindow;

  public:
    void displayStatus(const std::string & message) const;
    void displayAppendSegments(const AList<TSegment> a[2][6],
			       leda_color =leda_black) const;
    void displayTracks(const AList<TTrack> &,
		       const AList<TSegment> seg[2][6],
		       const std::string & text) const;
#endif
};

//-----------------------------------------------------------------------------

#ifdef TConformalFinder_NO_INLINE
#define inline
#else
#undef inline
#define TConformalFinder_INLINE_DEFINE_HERE
#endif

#ifdef TConformalFinder_INLINE_DEFINE_HERE

inline
std::string
TConformalFinder::name(void) const {
    return "Conformal Finder";
}

inline
bool
TConformalFinder::doT0Reset(bool a) {
    return _doT0Reset = a;
}

inline
bool
TConformalFinder::T0ResetDone(void) const {
    return _T0ResetDone;
}

#endif

#undef inline

#endif /* TConformalFinder_FLAG_ */
