//-----------------------------------------------------------------------------
// $Id: TSegment0.h,v 1.7 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TSegment0.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to manage a group of TMLink's.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TSegment0_FLAG_
#define TSegment0_FLAG_

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

/// A class to relate TMDCWireHit and TTrack objects.
class TSegment0 : public TTrackBase {

  public:
    /// Constructor.
    TSegment0();
    TSegment0(const AList<TMLink> &);

    /// Destructor
    virtual ~TSegment0();

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

    /// returns direction.
    const HepVector3D & direction(void) const;

    /// calculates distance between two clusters. Smaller value indicates closer.
    double distance(const TSegment0 &) const;
    double distance(const HepPoint3D &, const HepVector3D &) const;

    /// returns Range of x-coordinate of TMLinks.
    Range rangeX(double min, double max) const;

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
    AList<TSegment0> split(void) const;

    int solveDualHits(void);
    double duality(void) const;

  public:// TTrack relations
    AList<TTrack> & tracks(void);

  public:// Segment links
    AList<TSegment0> & innerLinks(void);

  private:
    AList<TSegment0> splitAV(void) const;
    AList<TSegment0> splitParallel(void) const;
    AList<TSegment0> splitComplicated(void) const;
    AList<TSegment0> splitDual(void) const;

  private:
    /// updates cache.
    void update(void) const;

    /// updates type.
    void updateType(void) const;

    /// updates duality.
    void updateDuality(void) const;

  private:
    // always updated.
    AList<TTrack> _tracks;
    AList<TSegment0> _innerLinks;

  private:
    mutable HepPoint3D _position;
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
};

// Utility functions
/// returns \# of core links in segments.
unsigned NCoreLinks(const CAList<TSegment0> & list);

/// returns AList of TMLink used for a track.
AList<TMLink> Links(const TSegment0 &, const TTrack &);

/*
/// checks property of segments.
void
CheckSegments(const CAList<TSegment0> & segmentList);

/// checks to link segments.
void
CheckSegmentLink(const TSegment0 & base,
		 const TSegment0 & next,
		 float distance,
		 float dirAngle);
*/

//-----------------------------------------------------------------------------

#ifdef TSegment0_NO_INLINE
#define inline
#else
#undef inline
#define TSegment0_INLINE_DEFINE_HERE
#endif

#ifdef TSegment0_INLINE_DEFINE_HERE

inline
const HepPoint3D &
TSegment0::position(void) const {
    if (! _fitted) update();
    return _position;
}

inline
const HepVector3D &
TSegment0::direction(void) const {
    if (! _fitted) update();
    return _direction;
}

inline
unsigned
TSegment0::innerWidth(void) const {
    if (! _fitted) update();
    return _innerWidth;
}

inline
unsigned
TSegment0::outerWidth(void) const {
    if (! _fitted) update();
    return _outerWidth;
}

inline
unsigned
TSegment0::innerMostLayer(void) const {
    if (! _fitted) update();
    return _innerMostLayer;
}

inline
unsigned
TSegment0::outerMostLayer(void) const {
    if (! _fitted) update();
    return _outerMostLayer;
}

inline
unsigned
TSegment0::clusterType(void) const {
    if (! nLinks()) return 0;
    if (_clusterType == 0) updateType();
    return _clusterType;
}

inline
double
TSegment0::duality(void) const {
    return _duality;
}

inline
unsigned
TSegment0::objectType(void) const {
    return Segment;
}

inline
unsigned
TSegment0::superLayerId(void) const {
    unsigned id = (links())[0]->wire()->superLayerId();
#ifdef TRKRECO_DEBUG
    {
	const AList<TMLink> & list = links();
	unsigned n = list.length();
	for (unsigned i = 1; i < n; i++) {
	    if (list[i]->hit()->wire()->superLayerId() != id) {
		std::cout << "TSegment0::superLayerId !!! strange segment found";
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
TSegment0::tracks(void) {
    return _tracks;
}

inline
AList<TSegment0> &
TSegment0::innerLinks(void) {
    return _innerLinks;
}

#endif

#undef inline

#endif /* TSegment0_FLAG_ */

