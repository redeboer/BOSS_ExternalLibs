//-----------------------------------------------------------------------------
// $Id: TMDCWireHit.h,v 1.7 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCWireHit.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a wire hit in MDC.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCWireHit_FLAG_
#define TMDCWireHit_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define WireHitLeft                0
#define WireHitRight               1

//...Masks for state()...

#define WireHitTimeValid      1048576
#define WireHitChargeValid    2097152
#define WireHitFindingValid   4194304
#define WireHitFittingValid   0x40000000
#define WireHitAxial                8
#define WireHitStereo              48

#define WireHitPatternLeft        256
#define WireHitPatternRight       512
#define WireHitIsolated          1024
#define WireHitContinuous        2048
#define WireHitNeighborHit         12
#define WireHitNeighborPattern 258048
#define WireHitNeighborHit0      4096
#define WireHitNeighborHit1      8192
#define WireHitNeighborHit2     16384
#define WireHitNeighborHit3     32768
#define WireHitNeighborHit4     65536
#define WireHitNeighborHit5    131072

#define WireHitLocked               128
#define WireHitUsed                  64
#define WireHitLeftMask               1
#define WireHitRightMask              2
#define WireHitMultiTrack       8388608
#define WireHitConformalFinder 16777216
#define WireHitCurlFinder      33554432
#define WireHitClustFinder     67108864
#define WireHitTrackManager   134217728
#define WireHitInvalidForFit  268435456
#define WireHitFastFinder     536870912

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

//zangsl 040518 change the class declaration to included files
//struct reccdc_wirhit;
//class MdcRec_wirhit
//class TMDCWire;
class TTrack;
//class TMDCWireHitMC;
#include "MdcTables/MdcTables.h"
#include "TrkReco/TMDCWire.h"
//#include "TrkReco/TTrack.h"
#include "TrkReco/TMDCWireHitMC.h"
  
// A class to represent a wire hit in MDC.
class TMDCWireHit {

  public:
    /// Constructor.
    TMDCWireHit(TMDCWire *, MdcRec_wirhit *, float fudgeFactor);

    /// Destructor
    virtual ~TMDCWireHit();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a pointer to a TMDCWire.
    const TMDCWire * const wire(void) const;

    /// returns a pointer to RECMDC_WIRHIT.
    struct MdcRec_wirhit * reccdc(void) const;

    /// returns state.
    unsigned state(void) const;

    /// returns drift distance.
    float drift(unsigned) const;

    /// returns drift distance error.
    float dDrift(unsigned) const;

    /// returns drift distance.
    float drift(void) const;

    /// returns drift distance error.
    float dDrift(void) const;


   
    /// returns drift time
//    double DriftTime(double,double) const;



    /// returns position in the middle of wire. z is always zero, however.
    const HepPoint3D & xyPosition(void) const;

    /// returns left position. z is always zero.
    HepPoint3D position(unsigned) const;

    /// assigns a pointer to a TTrack.
    const TTrack * const track(void) const;

    /// returns sequential Length in one segment : this parameter is used in TCurlFinder now.
    unsigned sequence(void) const;

    /// returns a pointer to TMDCWireHitMC.
    const TMDCWireHitMC * const mc(void) const;

  public:// Modifiers
    /// sets state. Meaning of bits are written below.
    unsigned state(unsigned newState);

    /// sets state. Meaning of bits are written below. (tmp)
    unsigned state(unsigned newState) const;

    /// assigns a pointer to a TTrack.
    const TTrack * const track(const TTrack *);

    /// assigns a pointer to a TTrack. (tmp)
    const TTrack * const track(const TTrack *) const;

    /// sets a pointer to TMDCWireHitMC.
    const TMDCWireHitMC * const mc(TMDCWireHitMC *);

    /// sets sequential length in one segment : this parameter is used in TCurlFinder now.
    unsigned sequence(unsigned) const;

  private:
    mutable unsigned _state;
    float _drift[2];         // 0:left, 1:right
    float _driftError[2];
    const TMDCWire * _wire;
    MdcRec_wirhit * const _r;
    const HepPoint3D & _xyPosition;
    mutable const TTrack * _track;
    const TMDCWireHitMC * _mc;
    mutable unsigned _sequentialLength;

   // _state bit definition

   // Pre-detemined
   //   20 : drift time valid
   //   21 : charge(dE/dx) valid
   //   22 : valid for finding
   //   30 : valid for fit
   //    3 : axial hit
   // 4, 5 : stereo hit

   // Hit pattern
   //    8 : hit pattern left
   //    9 : hit pattern right
   //   10 : isolated hit pattern
   //   11 : continuous hit pattern
   //   12 : neighbor hit 0
   //   13 : neighbor hit 1
   //   14 : neighbor hit 2
   //   15 : neighbor hit 3
   //   16 : neighbor hit 4
   //   17 : neighbor hit 5

   // Tracking results
   //    7 : locked
   //    6 : used for tracking
   //    0 : left hit
   //    1 : right hit
   //   23 : shared by multi-track
   //   24 : found by conforaml finder
   //   25 : found by curl finder
   //   26 : found by clust finder
   //   27 : found by track manager
   //   28 : NOT valid for fitting

   // Fitting results
};

//-----------------------------------------------------------------------------

#ifdef TMDCWireHit_NO_INLINE
#define inline
#else
#undef inline
#define TMDCWireHit_INLINE_DEFINE_HERE
#endif

#ifdef TMDCWireHit_INLINE_DEFINE_HERE

inline
const TMDCWire * const
TMDCWireHit::wire(void) const {
    return _wire;
}

inline
struct MdcRec_wirhit *
TMDCWireHit::reccdc(void) const {
    return _r;
}

inline
unsigned
TMDCWireHit::state(void) const {
    return _state;
}

inline
float
TMDCWireHit::drift(unsigned i) const {
    if (i) return _drift[1];
    return _drift[0];
}

inline
float
TMDCWireHit::dDrift(unsigned i) const {
    if (i) return _driftError[1];
    return _driftError[0];
}

inline
float
TMDCWireHit::drift(void) const {
    return (_drift[0] + _drift[1]) / 2.;
}

inline
float
TMDCWireHit::dDrift(void) const {
    return (_driftError[0] + _driftError[1]) / 2.;
}

inline
const HepPoint3D &
TMDCWireHit::xyPosition(void) const {
    return _xyPosition;
}

inline
unsigned
TMDCWireHit::state(unsigned i) {
    return _state = i;
}

inline
unsigned
TMDCWireHit::state(unsigned i) const {
    return _state = i;
}

inline
const TTrack * const
TMDCWireHit::track(void) const {
    return _track;
}

inline
const TTrack * const
TMDCWireHit::track(const TTrack * a) {
    return _track = a;
}

inline
const TTrack * const
TMDCWireHit::track(const TTrack * a) const {
    return _track = a;
}

inline
const TMDCWireHitMC * const
TMDCWireHit::mc(void) const {
    return _mc;
}

inline
const TMDCWireHitMC * const
TMDCWireHit::mc(TMDCWireHitMC * a) {
    return _mc = a;
}

inline
unsigned
TMDCWireHit::sequence(void) const {
    return _sequentialLength;
}

inline
unsigned
TMDCWireHit::sequence(unsigned a) const {
    return _sequentialLength = a;
}

#endif

#undef inline

#endif /* TMDCWireHit_FLAG_ */
