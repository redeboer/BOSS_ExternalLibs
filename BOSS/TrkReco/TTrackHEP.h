//-----------------------------------------------------------------------------
// $Id: TTrackHEP.h,v 1.6 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TTrackHEP.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a GEN_HEPEVT particle in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TTrackHEP_FLAG_
#define TTrackHEP_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

// itostring
#include "CLHEP/String/Strings.h"

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
//#include "tables/hepevt.h"
#include "MdcTables/HepevtTables.h"
#include "TrkReco/TMDC.h"

using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;

/// A class to represent a GEN_HEPEVT particle in tracking.
class TTrackHEP {

  public:
    /// returns a list of TTrackHEP's.
    static const AList<TTrackHEP> & list(void);

    /// returns a pointer to TTrackHEP of given Gen_hepevt.
    static TTrackHEP * hep(const Gen_hepevt *);

  private:
    /// updates information.
    static void update(void);

  private:
    /// Constructor
    TTrackHEP(const Gen_hepevt *);

  public:
    /// Destructor
    virtual ~TTrackHEP();

  public:
    /// returns name.
    std::string name(void) const;

    /// returns an id started from 0.
    unsigned id(void) const;

    /// returns status.
    int status(void) const;

    /// returns particle type.
    int pType(void) const;

    /// returns a pointer to a mother.
    TTrackHEP * mother(void) const;

    /// returns a pointer to i'th child.
    TTrackHEP * child(unsigned i) const;

    /// returns momentum vector.
//    const VectorL & p(void) const;
    const HepLorentzVector & p(void) const;
    
    /// returns position vector.
    const Hep3Vector & v(void) const;

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a list of TMDCWireHitMC.
    const AList<TMDCWireHitMC> & hits(void) const;

    /// returns a pointer to Gen_hepevt.
    const Gen_hepevt * gen(void) const;

  private:
    const Gen_hepevt * _hep;
    TTrackHEP * _mother;
    AList<TTrackHEP> _children;
//    const VectorL _p;
    const HepLorentzVector _p;
    const Hep3Vector _v;
    AList<TMDCWireHitMC> _hits;

  private:// static members
    static AList<TTrackHEP> _list;

  friend void TMDC::updateMC(void);
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TTrackHEP_INLINE_DEFINE_HERE
#endif

#ifdef TTrackHEP_INLINE_DEFINE_HERE

inline
unsigned
TTrackHEP::id(void) const {
    return _hep->id - 1;
}

inline
int
TTrackHEP::status(void) const {
    return _hep->isthep;
}

inline
int
TTrackHEP::pType(void) const {
    return _hep->idhep;
}

inline
TTrackHEP *
TTrackHEP::mother(void) const {
    return _mother;
}

inline
TTrackHEP *
TTrackHEP::child(unsigned i) const {
    return _children[i];
}

inline
const HepLorentzVector &
TTrackHEP::p(void) const {
    return _p;
}

inline
const Hep3Vector &
TTrackHEP::v(void) const {
    return _v;
}

inline
TTrackHEP *
TTrackHEP::hep(const Gen_hepevt * h) {
    if (! h) return 0;
    unsigned id = h->id - 1;
    return _list[id];
}

inline
const AList<TMDCWireHitMC> &
TTrackHEP::hits(void) const {
    return _hits;
}

inline
std::string
TTrackHEP::name(void) const {
    return "mc" + itostring(id());
}

inline
const Gen_hepevt *
TTrackHEP::gen(void) const {
    return _hep;
}

#endif

#undef inline

#endif /* TTrackHEP_FLAG_ */
