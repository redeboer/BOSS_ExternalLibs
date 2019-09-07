//-----------------------------------------------------------------------------
// $Id: TMDC.h,v 1.9 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDC.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent MDC.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDC_FLAG_
#define TMDC_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <cmath>
#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "TrkReco/TUpdater.h"

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class TMDC;
class TMDCWire;
class TMDCLayer;
class TMDCWireHit;
class TMDCWireHitMC;
//class Geocdc_wire;
//struct geocdc_wire;
//struct geocdc_layer;
//struct reccdc_wirhit;
class MdcRecGeo;
class MdcGeoWire;
class MdcGeoLayer;
class MdcRec_wirhit;

//class HepPoint3D;
class TMLink;
class TTrack;

/** A class to represent MDC.

  The instance of TMDC is a singleton. 'TMDC::getTMDC()' gives you a
  pointer to access the TMDC. Geometrical information is initialized
  automatically. Before accessing hit information, user has to call
  'update()' to initialize hit information event by event. */

class TMDC : private TUpdater {

  public:
    /// returns TMDC object.
    static TMDC * getTMDC(const std::string & cdcVersion);
    static TMDC * getTMDC(void);

  private:
    /// Constructor
    TMDC(const std::string & cdcVersion);

  public:// Selectors
    /// returns name.
    std::string name(void) const;

    /// returns version.
    std::string version(void) const;

    /// returns MDC version.
    std::string cdcVersion(void) const;

    /// dumps debug information.
    void dump(const std::string & message) const;

    /// returns debug level.
    int debugLevel(void) const;

    /// returns fudge factor for drift time error.
    float fudgeFactor(void) const;

  public:// to access information
    /// clears all TMDC information.
    void clear(void);

    /// clears TMDC information.
    void fastClear(void);

    /// updates TMDC information. clear() is called in this function.
    void update(bool mcAnalysis = true);

    /// sets debug level.
    int debugLevel(int);

    /// sets/returns fudge factor for drift time error.
    float fudgeFactor(float);

  public:// Geometry
    /// returns a pointer to a wire. 0 will be returned if 'wireId' is invalid.
    const TMDCWire * const wire(unsigned wireId) const;

    /// returns a pointer to a wire. 'localId' can be negative. 0 will be returned if 'layerId' is invalid.
    const TMDCWire * const wire(unsigned layerId, int localId) const;

    /// returns a pointer to a wire.
    const TMDCWire * const wire(const HepPoint3D & point) const;

    /// returns a pointer to a wire.
    const TMDCWire * const wire(float r, float phi) const;

    /// returns a pointer to a layer. 0 will be returned if 'id' is invalid.
    const TMDCLayer * const layer(unsigned id) const;

    /// returns a pointer to a super-layer. 0 will be returned if 'id' is invalid.
    const AList<TMDCLayer> * const superLayer(unsigned id) const;

    /// returns \# of wire layers in a super layer. 0 will be returned if 'superLayerId' is invalid.
    unsigned nLocalLayer(unsigned superLayerId) const;

  public:// Event by event information.
    /// returns a list of TMDCWireHit. 'update()' must be called before calling this function.
    const AList<TMDCWireHit> & hits(unsigned mask = 0) const;

    /// returns a list of axial hits. 'update()' must be called before calling this function.
    const AList<TMDCWireHit> & axialHits(unsigned mask = 0) const;

    /// returns a list of stereo hits. 'update()' must be called before calling this function.
    const AList<TMDCWireHit> & stereoHits(unsigned mask = 0) const;

    /// returns a list of TMDCWireHitMC. 'updateMC()' must be called before calling this function.
    const AList<TMDCWireHitMC> & hitsMC(void) const;

    /// returns bad hits(finding invalid hits).
    const AList<TMDCWireHit> & badHits(void);

  public:// Utility functions
    unsigned nWires(void) const;
    unsigned nSuperLayers(void) const;
    unsigned nLayers(void) const;

  public:// Utility functions for panther tables
    static std::string wireName(unsigned wireId);
    //    static std::string wireName(const geocdc_wire * const);
    //    static std::string wireName(const Geocdc_wire * const);
    //    static std::string wireName(const reccdc_wirhit &);
    static std::string wireName(const MdcGeoWire * const);
    static std::string wireName(const MdcRec_wirhit &);

    static unsigned layerId(unsigned wireId);
    //    static unsigned layerId(const geocdc_wire * const);
    //    static unsigned layerId(const Geocdc_wire * const);
    static unsigned layerId(const MdcGeoWire * const);

    static unsigned localId(unsigned wireId);
    //    static unsigned localId(const geocdc_wire * const);
    //    static unsigned localId(const Geocdc_wire * const);
    static unsigned localId(const MdcGeoWire * const);

    static unsigned superLayerId(unsigned wireId);
    //    static unsigned superLayerId(const geocdc_wire * const);
    //    static unsigned superLayerId(const Geocdc_wire * const);
    //    static unsigned superLayerId(const geocdc_layer * const);
    static unsigned superLayerId(const MdcGeoWire * const);
    static unsigned superLayerId(const MdcGeoLayer * const);

    static unsigned localLayerId(unsigned wireId);
    //    static unsigned localLayerId(const geocdc_wire * const);
    //    static unsigned localLayerId(const Geocdc_wire * const);
    //    static unsigned localLayerId(const geocdc_layer * const);
    static unsigned localLayerId(const MdcGeoWire * const);
    static unsigned localLayerId(const MdcGeoLayer * const);


    //    static unsigned axialStereoLayerId(const geocdc_layer * const);
    static unsigned axialStereoLayerId(const MdcGeoLayer * const);
    static unsigned layerId(unsigned axialStereo, unsigned axialStereoLayerId);

    static float phi(float phi);

    /// calculates corrected drift time. correctionFlag(bit 0:tof, 1:T0 offset, 2:propagation delay, 3:tan(lambda) correction)
    static void driftDistance(TMLink & link,
			      const TTrack & track,
			      unsigned correctionFlag = 0,
			      float T0Offset = 0.);

//  private:
    /// classify hits.
    void classification(void);

    /// updates TMDC information for MC.
    void updateMC(void);

  private:
    static TMDC * _cdc;
//    MdcRecGeo* _geo;
	    
    int _debugLevel;

    const std::string _cdcVersion;
    const bool _newCdc;
    const unsigned _nWires;
    const unsigned _nSuperLayers;
    const unsigned _nLayers;

    float _fudgeFactor;
    AList< AList<TMDCLayer> > _superLayers;
    AList<TMDCLayer> _layers;
    AList<TMDCWire> _wires;
    AList<TMDCWire> _hitWires;
    AList<TMDCWireHit> _hits;
    AList<TMDCWireHit> _axialHits;
    AList<TMDCWireHit> _stereoHits;
    AList<TMDCWireHit> _badHits;
    AList<TMDCWireHitMC> _hitsMC;
};

//-----------------------------------------------------------------------------

#ifdef TMDC_NO_INLINE
#define inline
#else
#undef inline
#define TMDC_INLINE_DEFINE_HERE
#endif

#ifdef TMDC_INLINE_DEFINE_HERE

inline
int
TMDC::debugLevel(void) const {
    return _debugLevel;
}

inline
int
TMDC::debugLevel(int a) {
    return _debugLevel = a;
}

inline
unsigned
TMDC::nLocalLayer(unsigned superLayerId) const {
    AList<TMDCLayer> * superLayer = _superLayers[superLayerId];
    if (! superLayer) return 0;
    return superLayer->length();
}

inline
const TMDCLayer * const
TMDC::layer(unsigned id) const {
    return _layers[id];
}

inline
const AList<TMDCWireHitMC> &
TMDC::hitsMC(void) const {
    return _hitsMC;
}

inline
const AList<TMDCLayer> * const
TMDC::superLayer(unsigned id) const {
    return _superLayers[id];
}

inline
const TMDCWire * const
TMDC::wire(unsigned id) const {
    return _wires[id];
}

inline
float
TMDC::phi(float a) {
    static const float pi2 = 2. * M_PI;
    a = std::fmod(a, pi2);
    if (a < 0.) return a + pi2;
    return a;
}

inline
float
TMDC::fudgeFactor(void) const {
    return _fudgeFactor;
}

inline
float
TMDC::fudgeFactor(float a) {
    return _fudgeFactor = a;
}

inline
std::string
TMDC::cdcVersion(void) const {
    return std::string(_cdcVersion);
}

inline
unsigned
TMDC::nWires(void) const {
    return _nWires;
}

inline
unsigned
TMDC::nSuperLayers(void) const {
    return _nSuperLayers;
}

inline
unsigned
TMDC::nLayers(void) const {
    return _nLayers;
}

#endif

#undef inline

#endif /* TMDC_FLAG_ */
