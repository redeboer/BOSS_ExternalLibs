//-----------------------------------------------------------------------------
// $Id: TConformalFinder0.h,v 1.8 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TConformalFinder0.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to find tracks with the conformal method.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TConformalFinder0_FLAG_
#define TConformalFinder0_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES

#include "TrkReco/TFinderBase.h"
#include "TrkReco/TBuilder0.h"

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

//class HepPoint3D;
class TMLink;
class THistogram;
class TCircle;
class TSegment0;

/// A class to find tracks with the conformal method.
class TConformalFinder0 : public TFinderBase {

  public:
    /// Constructor.
    TConformalFinder0(float maxSigma,
		      float fraction,
		      float stereoZ3,
		      float stereoZ4,
		      float stereoChisq3,
		      float stereoChisq4,
		      float stereoMaxSigma,
		      unsigned fittingCorrections,
		      float salvageLevel,
		      bool cosmic);

    /// Destructor
    virtual ~TConformalFinder0();

  public:// Selectors
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a list of conformal links.
    const AList<TMLink> & axialConformalLinks(void) const;

    /// returns a list of conformal links.
    const AList<TMLink> & stereoConformalLinks(void) const;

  public:// Modifiers
    /// clear internal information.
    void clear(void);

    /// finds tracks.
    int doit(const AList<TMDCWireHit> & axialHits,
	     const AList<TMDCWireHit> & stereoHits,
	     AList<TTrack> & tracks,
	     AList<TTrack> & tracks3D);

    /// transforms hits into a conformal plane. 'center' is a center of the transformation. Z position of 'center' must be zero. Transformed positions are newly created, and are stored in 'links'. 
    static void conformalTransformation(const HepPoint3D & center,
					const AList<TMDCWireHit> & hits,
					AList<TMLink> & links);

    /// transforms drift circle of hits into a conformal plane. transformed positions( x0, y0, r) are stored in 'links'.
    static void conformalTransformationDriftCircle(const HepPoint3D & center,
                                                   const AList<TMDCWireHit> & hits,
                                                   AList<TMLink> & links);

    /// transforms hits into a conformal plane. 'center' is a center of the transformation. Z position of 'center' must be zero. Transformed positions are newly created, and are stored in 'links'. 
    static void conformalTransformationRphi(const HepPoint3D & center,
					    const AList<TMDCWireHit> & hits,
					    AList<TMLink> & links);
    
    /// finds segments.
    AList< AList<TSegment0> > findSegments(const AList<TMLink> & in) const;
    AList< AList<TSegment0> > findSegments2(const AList<TMLink> & in) const;

    /// finds segments. (obsolete functions)
    AList<TSegment0> findClusters(const THistogram &) const;
    AList<TSegment0> findClusters2(const THistogram &) const;


    /// sets swtich for stereo reconstruction.
    bool doStereo(bool);

    /// sets switch for salvaging.
    bool doSalvage(bool);

  private:
    /// returns a list of links close to a track.
    AList<TMLink> findCloseHits(const AList<TMLink> & in,
			       const TTrack & track) const;

    /// returns a pointer to the best cluster to be linked.
    TSegment0 * findBestLink(const TSegment0 &,
			    const AList<TSegment0> & in) const;

    /// appends the best cluster in 'list' to 'track'.
    TSegment0 * appendCluster(TTrack & track, AList<TSegment0> & in) const;

    /// returns a list of clusters to be a track.
    AList<TSegment0> findClusterLink(TSegment0 &,
				    const AList<TSegment0> * const) const;

    /// makes a track.
    TTrack * makeTrack(const AList<TSegment0> &) const;

    /// appends TMLinks in a list.
    void appendClusters2(TTrack & track, AList<TSegment0> &) const;

    /// returns a list of clusters close to a cluster.
    AList<TSegment0> findCloseClusters(const TTrack &,
				      const AList<TSegment0> &,
				      double maxDistance) const;

    /// main loop with salvaging.
    void standardFinding(AList<TMLink> & seeds,
			 AList<TMLink> & unusedLinks,
			 double fraction);

    /// main loop with salvaging for second trial.
    void specialFinding(AList<TMLink> & seeds,
			AList<TMLink> & unusedLinks,
			double fraction);

  private:
    /// Track builder.
    TBuilder0 * _builder;

    /// Parameters.
    TMSelector _circleSelector;
    TMSelector _trackSelector;
    float _fraction;

    /// List of hit positions in the conformal plane.
    AList<TMLink> _axialConfLinks;
    AList<TMLink> _stereoConfLinks;
    AList<TMLink> _unusedAxialConfLinks;
    AList<TMLink> _unusedStereoConfLinks;
    AList<TMLink> _goodAxialConfLinks;

    /// Internal lists.
    AList<TCircle> _circles;
    AList<TTrack> _tracks;

    /// Switches
    bool _doStereo;
    bool _doSalvage;
};

//-----------------------------------------------------------------------------

#ifdef TConformalFinder0_NO_INLINE
#define inline
#else
#undef inline
#define TConformalFinder0_INLINE_DEFINE_HERE
#endif

#ifdef TConformalFinder0_INLINE_DEFINE_HERE

inline
const AList<TMLink> &
TConformalFinder0::axialConformalLinks(void) const {
    return _axialConfLinks;
}

inline
const AList<TMLink> &
TConformalFinder0::stereoConformalLinks(void) const {
    return _stereoConfLinks;
}

inline
bool
TConformalFinder0::doStereo(bool a) {
    return _doStereo = a;
}

inline
bool
TConformalFinder0::doSalvage(bool a) {
    return _doSalvage = a;
}

inline
std::string
TConformalFinder0::name(void) const {
    return "Conformal Finder 0";
}

#endif

#undef inline

#endif /* TConformalFinder0_FLAG_ */

