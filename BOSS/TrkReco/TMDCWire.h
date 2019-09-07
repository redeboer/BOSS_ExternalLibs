//-----------------------------------------------------------------------------
// $Id: TMDCWire.h,v 1.8 2011/07/31 02:50:49 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCWire.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a wire in MDC.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCWire_FLAG_
#define TMDCWire_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

// itostring
#include "CLHEP/String/Strings.h"

#define HEP_SHORT_NAMES
#include "CLHEP/Geometry/Vector3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "TrkReco/TMDCLayer.h"

#define WireInnerLeft 0
#define WireInnerRight 1
#define WireLeft 2
#define WireRight 3
#define WireOuterLeft 4
#define WireOuterRight 5

//struct geocdc_wire;
class MdcGeoWire;
class TMDC;
class TMDCTsf;
class TMDCWireHit;
class TMDCWireHitMC;

/// A class to represent a wire in MDC.
class TMDCWire {

  public:
    /// Constructor.
    TMDCWire(const MdcGeoWire *, TMDCLayer *);

    /// Destructor
    virtual ~TMDCWire();

  public:// Selectors
    /// returns id.
    unsigned id(void) const;

    /// returns local id in a wire layer.
    unsigned localId(void) const;

    /// returns layer id.
    unsigned layerId(void) const;

    /// returns local layer id in a super layer.
    unsigned localLayerId(void) const;

    /// returns super layer id.
    unsigned superLayerId(void) const;

    /// returns id of axial or stereo id.
    unsigned axialStereoLayerId(void) const;

    /// returns a pointer to GEOMDC_WIR.
    const MdcGeoWire * geocdc(void) const;

    /// returns a pointer to a layer.
    const TMDCLayer * const layer(void) const;

    /// returns a pointer to a TMDCWireHit.
    const TMDCWireHit * const hit(void) const;

    /// returns a pointer to a TMDCTsf.
    const TMDCTsf * const tsf(void) const;

    /// returns state.
    unsigned state(void) const;

    /// returns true if this wire is in an axial layer.
    bool axial(void) const;

    /// returns true if this wire is in a stereo layer.
    bool stereo(void) const;

    /// returns true if this wire is in the inner part.
    bool innerPart(void) const;

    /// returns true if this wire is in the main part.
    bool mainPart(void) const;

    /// returns a pointer to a neighbor wire.
    const TMDCWire * const neighbor(unsigned) const;

    /// returns true if a given wire is consective in a layer.
    bool consective(const TMDCWire &) const;

    /// returns true if a given wire is adjacent.
    bool adjacent(const TMDCWire &) const;

    /// returns localId but if maxLocalId, return -1.
    int localIdForPlus(void) const;

    /// returns localId but if 0, return maxLocalId + 1.
    int localIdForMinus(void) const;

    /// returns position in forward endplate.
    const HepPoint3D & forwardPosition(void) const;

    /// returns position in backward endplate.
    const HepPoint3D & backwardPosition(void) const;
    double * backwardPosition(double p[3]) const;
    const double & tension(void) const;
    /// returns middle position of a wire. z componet is 0.
    const HepPoint3D & xyPosition(void) const;
    double * xyPosition(double p[3]) const;
    const HepPoint3D  xyPosition(double z) const;
    /// returns direction vector of the wire.
    const HepVector3D & direction(void) const;

    /// calculates position and direction vector with sag correction.
    void wirePosition(float zPosition,
		      HepPoint3D & xyPosition,
		      HepPoint3D & backwardPosition,
		      HepVector3D & direction) const;

    /// returns cell size in phi.
    float cellSize(void) const;

    /// returns local id difference.
    int localIdDifference(const TMDCWire &) const;

    /// returns name.
    std::string name(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Modifiers
    /// sets a pointer to TMDCWireHit.
    const TMDCWireHit * const hit(const TMDCWireHit * const);

    /// appends a pointer to TMDCWireHitMC.
    const TMDCWireHitMC * const hit(TMDCWireHitMC * const);

    /// sets a pointer of TMDCTsf.
    TMDCTsf * tsf(TMDCTsf *);

    /// sets state.
    unsigned state(unsigned newState);

    /// clears information.
    void clear(void);

  private:
    unsigned _state;
    const TMDCWireHit * _hit;
    AList<TMDCWireHitMC> _mcHits;

    unsigned _id;
    unsigned _localId;
    unsigned _layerId;
    const TMDCLayer * _layer;
    TMDCTsf * _tsf;
    HepPoint3D _xyPosition;
    HepPoint3D _forwardPosition;
    HepPoint3D _backwardPosition;
    double _tension;
    HepVector3D _direction;
    const MdcGeoWire * _geo;

    static TMDC * _cdc;
};

//-----------------------------------------------------------------------------

#ifdef TMDCWire_NO_INLINE
#define inline
#else
#undef inline
#define TMDCWire_INLINE_DEFINE_HERE
#endif

#ifdef TMDCWire_INLINE_DEFINE_HERE

inline
unsigned
TMDCWire::id(void) const {
    return _id;
}

inline
unsigned
TMDCWire::localId(void) const {
    return _localId;
}

inline
unsigned
TMDCWire::layerId(void) const {
    return _layerId;
}

inline
unsigned
TMDCWire::superLayerId(void) const {
    return _layer->superLayerId();
}

inline
unsigned
TMDCWire::localLayerId(void) const {
    return _layer->localLayerId();
}

inline
const MdcGeoWire *
TMDCWire::geocdc(void) const {
    return _geo;
}

inline
const TMDCLayer * const
TMDCWire::layer(void) const {
    return _layer;
}

inline
const TMDCTsf * const
TMDCWire::tsf(void) const {
    return _tsf;
}

inline
TMDCTsf *
TMDCWire::tsf(TMDCTsf * t) {
    return _tsf = t;
}

inline
unsigned
TMDCWire::state(void) const {
    return _state;
}

inline
unsigned
TMDCWire::state(unsigned a) {
    return _state = a;
}

inline
void
TMDCWire::clear(void) {
    _state = 0;
    _hit = 0;
    _mcHits.removeAll();
}

inline
const TMDCWireHit * const
TMDCWire::hit(const TMDCWireHit * const h) {
    return _hit = h;
}

inline
const TMDCWireHit * const
TMDCWire::hit(void) const {
    return _hit;
}

inline
const TMDCWireHitMC * const
TMDCWire::hit(TMDCWireHitMC * const a) {
    _mcHits.append(a);
    return a;
}

inline
const HepPoint3D &
TMDCWire::forwardPosition(void) const {
    return _forwardPosition;
}

inline
const HepPoint3D &
TMDCWire::backwardPosition(void) const {
    return _backwardPosition;
}

inline
const double &
TMDCWire::tension(void) const {
        return _tension;
}

inline
double *
TMDCWire::backwardPosition(double p[3]) const {
    p[0] = _backwardPosition.x();
    p[1] = _backwardPosition.y();
    p[2] = _backwardPosition.z();
    return p;
}

inline
const HepPoint3D &
TMDCWire::xyPosition(void) const {
    return _xyPosition;
}

inline
double *
TMDCWire::xyPosition(double a[3]) const {
    a[0] = _xyPosition.x();
    a[1] = _xyPosition.y();
    a[2] = 0.;
    return a;
}

inline
const HepVector3D &
TMDCWire::direction(void) const {
    return _direction;
}

inline
bool
TMDCWire::axial(void) const {
    return _layer->axial();
}

inline
bool
TMDCWire::stereo(void) const {
    return _layer->stereo();
}

inline
unsigned
TMDCWire::axialStereoLayerId(void) const {
    return _layer->axialStereoLayerId();
}

inline
bool
TMDCWire::innerPart(void) const {
    if (_layerId < 14) return true;
    return false;
}

inline
bool
TMDCWire::mainPart(void) const {
    if (_layerId > 13) return true;
    return false;
}

inline
float
TMDCWire::cellSize(void) const {
    if (_layer) {
	return _layer->cellSize();
    }
    else {
#ifdef TRKRECO_DEBUG
	std::cout << "TMDCWire::cellSize !!! can't return cell size";
	std::cout << " because no pointer to a layer" << std::endl;
#endif
	return 0.;
    }
}

inline
bool
TMDCWire::consective(const TMDCWire & w) const {
    if (neighbor(2) == & w) return true;
    else if (neighbor(3) == & w) return true;
    return false;
}

inline
bool
TMDCWire::adjacent(const TMDCWire & w) const {
    for (unsigned i = 0; i < 6; i++)
	if (neighbor(i) == & w)
	    return true;
    return false;
}

inline
std::string
TMDCWire::name(void) const {
    if (axial())
	return itostring(_layerId) + std::string("-") + itostring(_localId);
    return itostring(_layerId) + std::string("=") + itostring(_localId);
}

#endif

#undef inline

#endif /* TMDCWire_FLAG_ */

