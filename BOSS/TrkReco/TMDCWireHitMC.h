//-----------------------------------------------------------------------------
// $Id: TMDCWireHitMC.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TMDCWireHitMC.h
// Section  : Tracking MDC
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a MC wire hit in MDC.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMDCWireHitMC_FLAG_
#define TMDCWireHitMC_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "CLHEP/Vector/ThreeVector.h"
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
using namespace CLHEP;
class TMDC;
class TMDCWire;
class TMDCWireHit;
class TTrackHEP;
//struct datcdc_mcwirhit;
class MdcDat_mcwirhit;

/// A class to represent a MC wire hit in MDC.
class TMDCWireHitMC {

  public:
    /// Constructor.
    TMDCWireHitMC(const TMDCWire *, const TMDCWireHit *, MdcDat_mcwirhit *);

    /// Destructor
    virtual ~TMDCWireHitMC();

  public:// Selectors
    /// returns drift distance.
    float distance(void) const;

    /// returns hit position.
    const HepPoint3D & hitPosition(void) const;

    /// returns an entrance point.
    const HepPoint3D & entrance(void) const;

    /// returns vector from entrance to exit point.
    const HepVector3D & direction(void) const;

    /// returns momentum vector at the entrance.
    const Hep3Vector & momentum(void) const;

    /// returns left or right.
    int leftRight(void) const;

    /// returns a pointer to DATMDC_MCWIRHIT.
    MdcDat_mcwirhit * datcdc(void) const;

    /// returns a pointer to a TMDCWire.
    const TMDCWire * const wire(void) const;

    /// returns a pointer to a GEN_HEPEVT.
    const TTrackHEP * const hep(void) const;

    /// returns a pointer to a TMDCWireHit.
    const TMDCWireHit * const hit(void) const;

  public:// Modifiers

  private:
    const TMDCWire * _wire;
    const TMDCWireHit * _hit;
    HepPoint3D _position;
    HepPoint3D _entrance;
    HepVector3D _direction;
    Hep3Vector _momentum;
    float _distance;
    float _energy;
    int _leftRight;
    MdcDat_mcwirhit * _datcdc;
    const TTrackHEP * _hep;

  friend class TMDC;
};

//-----------------------------------------------------------------------------

#ifdef TMDCWireHitMC_NO_INLINE
#define inline
#else
#undef inline
#define TMDCWireHitMC_INLINE_DEFINE_HERE
#endif

#ifdef TMDCWireHitMC_INLINE_DEFINE_HERE

inline
float
TMDCWireHitMC::distance(void) const {
    return _distance;
}

inline
const HepPoint3D &
TMDCWireHitMC::hitPosition(void) const {
    return _position;
}

inline
const HepPoint3D &
TMDCWireHitMC::entrance(void) const {
    return _entrance;
}

inline
const HepVector3D &
TMDCWireHitMC::direction(void) const {
    return _direction;
}

inline
int
TMDCWireHitMC::leftRight(void) const {
    return _leftRight;
}

inline
MdcDat_mcwirhit *
TMDCWireHitMC::datcdc(void) const {
    return _datcdc;
}

inline
const TMDCWire * const
TMDCWireHitMC::wire(void) const {
    return _wire;
}

inline
const TTrackHEP * const
TMDCWireHitMC::hep(void) const {
    return _hep;
}

inline
const TMDCWireHit * const 
TMDCWireHitMC::hit(void) const {
    return _hit;
}

inline
const Hep3Vector &
TMDCWireHitMC::momentum(void) const {
    return _momentum;
}

#endif

#undef inline

#endif /* TMDCWireHitMC_FLAG_ */
