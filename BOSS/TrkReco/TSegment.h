//-----------------------------------------------------------------------------
// $Id: TSegment.h,v 1.12 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TSegment.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to manage a group of TMLink's.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TSegment_FLAG_
#define TSegment_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Geometry/Vector3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif

#include "TrkReco/TTrackBase.h"
#include "TrkReco/TMDCUtil.h"
#include "TrkReco/TMLink.h"

class TTrack;
class Range;
template <class T> class CAList;

#define TSegmentCrowd 8

/// A class to relate TMDCWireHit and TTrack objects.
class TSegment : public TTrackBase {

  public:
    /// Constructor.
    TSegment();
    TSegment(const AList<TMLink> &);

    /// Destructor
    virtual ~TSegment();

  public:// Selectors
    /// returns type.
    virtual unsigned objectType(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
    	      const std::string & prefix = std::string("")) const;

    /// returns super layer id.
    unsigned superLayerId() const;

    /// returns position.
    const HepPoint3D & position(void) const;
    const HepPoint3D & outerPosition(void) const;

    /// return line of Tsf for pos and dir
    const HepPoint3D & lineTsf(void) const;
    const HepPoint3D & lineTsf(const HepPoint3D &);

    /// returns direction.
    const HepVector3D & direction(void) const;

    /// calculates distance between two clusters. Smaller value indicates closer.
    double distance(const TSegment &) const;
    double distance(const HepPoint3D &, const HepVector3D &) const;

    /// returns Range of x-coordinate of TMLinks.
    Range rangeX(double min, double max) const;

    /// returns TMLinks in inner/outer-most layer.
    const AList<TMLink> & inners(void) const;
    const AList<TMLink> & outers(void) const;

    /// returns a TMLink which is the closest to the center.
    const TMLink & center(void) const;

    /// returns width.
    unsigned width(void) const;

    /// returns inner width.
    unsigned innerWidth(void) const;

    /// returns outer width.
    unsigned outerWidth(void) const;

    /// returns inner most layer.
    unsigned innerMostLayer(void) const;

    /// returns outer most layer.
    unsigned outerMostLayer(void) const;

    /// returns cluster type. 0:empty, 1:short line, 2:long line, 3:V shage(from outside), 4:A shape, 5:X shape, 6:parallel, 7:complicated.
    unsigned clusterType(void) const;

    /// returns a list of sub TSegments in this cluster. If cluster type is 1, 2, or 7, no cluster is returned.
    AList<TSegment> split(void) const;

    //split in Tsf model.
    AList<TSegment> splitTsf(AList<TMLink> &);

    int solveDualHits(void);
    double duality(void) const;

    ///solve LR of hit in TSF.
    void solveLR(void);
    void solveThreeHits(void);

  public:// TTrack relations
    AList<TTrack> & tracks(void);

  public:// Segment links
    AList<TSegment> & innerLinks(void);
    const AList<TSegment> & innerLinks(void) const;
    AList<TSegment> & outerLinks(void);
    const AList<TSegment> & outerLinks(void) const;
    unsigned state(void) const;
    unsigned state(unsigned);

  private:
    AList<TSegment> splitAV(void) const;
    AList<TSegment> splitParallel(void) const;
    AList<TSegment> splitComplicated(void) const;
    AList<TSegment> splitDual(void) const;

  private:
    /// updates cache.
    void update(void) const;

    /// updates type.
    void updateType(void) const;

    /// updates duality.
    void updateDuality(void) const;

  private:
    //for Tsf
    void fitLine(TMLink *seed, TMLink *outer, HepPoint3D line[4]) const;
    AList<TSegment> appendByLine(TMLink *seed, TMLink *outer, AList<TMLink> & seedNeighbors, AList<TMLink> & restHits, const HepPoint3D line[4]);
    double distance2(TMLink *, HepPoint3D) const;
    double maxdDistance(TMLink *) const;

    HepPoint3D leastSquaresFitting1(const AList<TMLink> & links, const HepPoint3D tsfLine) const;
    HepPoint3D leastSquaresFitting(const AList<TMLink> & links, const HepPoint3D tsfLine) const;
    HepPoint3D positionTsf(const AList<TMLink> & links, const HepPoint3D line) const;
    void segSalvage(AList<TMLink> &);
    void expandSeg(const int, AList<TMLink> &);
    HepPoint3D closestPoint(const HepPoint3D & p, const HepPoint3D line) const;

  private:
    // always updated.
    AList<TTrack> _tracks;
    AList<TSegment> _innerLinks;
    AList<TSegment> _outerLinks;
    unsigned _state;

  private:
    mutable HepPoint3D _position;
    mutable HepPoint3D _outerPosition;
    mutable HepVector3D _direction;
    mutable unsigned _innerWidth;
    mutable unsigned _outerWidth;
    mutable unsigned _innerMostLayer;
    mutable unsigned _outerMostLayer;
    mutable AList<TMLink> _inners;
    mutable AList<TMLink> _outers;
    mutable unsigned _nLayer;
    mutable unsigned _clusterType;
    mutable double _duality;
    mutable unsigned _nDual;
    mutable double _angle;

    //----tsf----//
    double _times[11];   //times of sigma
    mutable HepPoint3D _lineTsf;

    static TMDC * _cdc;

  private:
    //for Tsf
    AList<TMLink> _usedHitsOfSecond; //for the (23) layers;
};

// Utility functions
/// returns \# of core links in segments.
unsigned NCoreLinks(const CAList<TSegment> & list);

/// returns AList of TMLink used for a track.
AList<TMLink> Links(const TSegment &, const TTrack &);

/// returns AList of TMLink.
AList<TMLink> Links(const AList<TSegment> & list);

/// checks property of segments.
//void
//CheckSegments(const CAList<TSegment> & segmentList);
//
/// checks to link segments.
//void
//CheckSegmentLink(const TSegment & base,
//		 const TSegment & next,
//		 float distance,
//		 float dirAngle);

/// returns \# of unique segment links.
unsigned
NUniqueLinks(const TSegment & a);

/// returns a list of unique segments in links.
AList<TSegment>
UniqueLinks(const TSegment & a);

/// returns a segment to the outer-most unique segment.
TSegment *
OuterMostUniqueLink(const TSegment & a);

/// returns \# of links in the major link.
unsigned
NMajorLinks(const TSegment & a);

/// returns a list of segments in major links.
AList<TSegment>
MajorLinks(const TSegment & a);

/// returns \# of link branches in the major link.
unsigned
NLinkBranches(const TSegment & a);

/// returns isolated and crowded list.
void
SeparateCrowded(const AList<TSegment> & input,
		AList<TSegment> & isolated,
		AList<TSegment> & crowded);

/// returns super layer pattern.
unsigned
SuperLayer(const AList<TSegment> & list);

//-----------------------------------------------------------------------------

#ifdef TSegment_NO_INLINE
#define inline
#else
#undef inline
#define TSegment_INLINE_DEFINE_HERE
#endif

#ifdef TSegment_INLINE_DEFINE_HERE

inline
const AList<TMLink> &
TSegment::inners(void) const {
    if (! _fitted) update();
    return _inners;
}

inline
const AList<TMLink> &
TSegment::outers(void) const {
    if (! _fitted) update();
    return _outers;
}

inline
const HepPoint3D &
TSegment::position(void) const {
    if (! _fitted) update();
    return _position;
}

inline
const HepVector3D &
TSegment::direction(void) const {
    if (! _fitted) update();
    return _direction;
}

inline
unsigned
TSegment::innerWidth(void) const {
    if (! _fitted) update();
    return _innerWidth;
}

inline
unsigned
TSegment::outerWidth(void) const {
    if (! _fitted) update();
    return _outerWidth;
}

inline
unsigned
TSegment::innerMostLayer(void) const {
    if (! _fitted) update();
    return _innerMostLayer;
}

inline
unsigned
TSegment::outerMostLayer(void) const {
    if (! _fitted) update();
    return _outerMostLayer;
}

inline
unsigned
TSegment::clusterType(void) const {
    if (! nLinks()) return 0;
    if (_clusterType == 0) updateType();
    return _clusterType;
}

inline
double
TSegment::duality(void) const {
    return _duality;
}

inline
unsigned
TSegment::objectType(void) const {
    return Segment;
}

inline
unsigned
TSegment::superLayerId(void) const {
    unsigned id = (links())[0]->wire()->superLayerId();
#ifdef TRKRECO_DEBUG
    {
	const AList<TMLink> & list = links();
	unsigned n = list.length();
	for (unsigned i = 1; i < n; i++) {
	    if (list[i]->hit()->wire()->superLayerId() != id) {
		std::cout << "TSegment::superLayerId !!! strange segment found";
		std::cout << std::endl;
		dump();
		break;
	    }
	}
    }
#endif
    return id;
}

inline
AList<TTrack> &
TSegment::tracks(void) {
    return _tracks;
}

inline
AList<TSegment> &
TSegment::innerLinks(void) {
    return _innerLinks;
}

inline
const AList<TSegment> &
TSegment::innerLinks(void) const {
    return _innerLinks;
}

inline
AList<TSegment> &
TSegment::outerLinks(void) {
    return _outerLinks;
}

inline
const AList<TSegment> &
TSegment::outerLinks(void) const {
    return _outerLinks;
}

inline
unsigned
TSegment::state(void) const {
    return _state;
}

inline
unsigned
TSegment::state(unsigned a) {
    return _state = a;
}

//-----tsf----//
inline
const HepPoint3D &
TSegment::lineTsf(void) const {
    return _lineTsf;
}

inline
const HepPoint3D &
TSegment::lineTsf(const HepPoint3D & a){
    return _lineTsf = a;
}
#endif

#undef inline

#endif /* TSegment_FLAG_ */

