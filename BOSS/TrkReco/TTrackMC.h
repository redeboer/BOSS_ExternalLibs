//-----------------------------------------------------------------------------
// $Id: TTrackMC.h,v 1.5 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : TTrackMC.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to have MC information of TTrack.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TTrackMC_FLAG_
#define TTrackMC_FLAG_

#define TTrackClassified 1

#define TTrackGarbage 1
#define TTrackCharge 2
#define TTrackBad 4
#define TTrackGhost 8
#define TTrackGood 16

#define TTrackHep 65536
#define TTrackWire 131072
#define TTrackPt 524288
#define TTrackPz 1048576
#define TTrackUnique 2097152
#define TTrackMatchingLoose 4194304
#define TTrackMatchingTight 8388608

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
//struct reccdc_mctrk;
class MdcRec_mctrk;
class TrkReco;
class TTrack;
class TTrackHEP;

using CLHEP::Hep3Vector;
using CLHEP::HepLorentzVector;

/// A class to have MC information of TTrack.
class TTrackMC {

  private:
    /// Constructor
    TTrackMC(const TTrack &);

  public:
    /// Destructor
    virtual ~TTrackMC();

  public:
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns HEP ID.
    int hepId(void) const;

    /// returns a pointer to TTrackHEP.
    const TTrackHEP * const hep(void) const;

    /// returns charge matching.
    bool charge(void) const;

    /// returns wire fraction(F1).
    double wireFraction(void) const;

    /// returns wire fraction(F2).
    double wireFractionHEP(void) const;

    /// returns pt fraction.
    double ptFraction(void) const;

    /// returns pz fraction.
    double pzFraction(void) const;

    /// returns residual of momentum.
    const Hep3Vector & residual(void) const;

    /// returns residual of pt.
    double ptResidual(void) const;

    /// returns residual of pz.
    double pzResidual(void) const;

    /// returns pull of pt.
    double ptPull(void) const;

    /// returns pull of pz.
    double pzPull(void) const;

    /// returns state.
    unsigned state(void) const;

    /// returns quality.
    unsigned quality(void) const;

    /// returns quality.
    std::string qualityString(void) const;

  public:
    /// updates information.
    void update(void);

  private:
    /// checks matching of charge.
    void compare(void);

    /// classifies itself.
    void classify(void);

  private:
    unsigned _state;
    unsigned _quality;

    const TTrack & _t;
    TTrackHEP * _hep;
    int _hepID;
    double _wireFraction;
    double _wireFractionHEP;
    bool _charge;
    double _ptFraction;
    double _pzFraction;
    Hep3Vector _residual;
    double _cosOpen;
    double _ptResidual;
    double _pzResidual;
    double _ptPull;
    double _pzPull;

  friend class TrkReco;
};

std::string
TrackMCStatus(const TTrackMC &);

std::string
TrackMCStatus(const MdcRec_mctrk &);

std::string
TrackMCQualityString(unsigned quality);

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TTrackMC_INLINE_DEFINE_HERE
#endif

#ifdef TTrackMC_INLINE_DEFINE_HERE

inline
int
TTrackMC::hepId(void) const {
    return _hepID;
}

inline
const TTrackHEP * const
TTrackMC::hep(void) const {
    return _hep;
}

inline
bool
TTrackMC::charge(void) const {
    return _charge;
}

inline
double
TTrackMC::ptFraction(void) const {
    return _ptFraction;
}

inline
double
TTrackMC::pzFraction(void) const {
    return _pzFraction;
}

inline
double
TTrackMC::wireFraction(void) const {
    return _wireFraction;
}

inline
double
TTrackMC::wireFractionHEP(void) const {
    return _wireFractionHEP;
}

inline
unsigned
TTrackMC::state(void) const {
    return _state;
}

inline 
unsigned 
TTrackMC::quality(void) const { 
    return _quality;
} 

inline
const Hep3Vector &
TTrackMC::residual(void) const {
    return _residual;
}

inline
double
TTrackMC::ptResidual(void) const {
    return _ptResidual;
}

inline
double
TTrackMC::pzResidual(void) const {
    return _pzResidual;
}

inline
double
TTrackMC::ptPull(void) const {
    return _ptPull;
}

inline
double
TTrackMC::pzPull(void) const {
    return _pzPull;
}

#endif

#undef inline

#endif /* TTrackMC_FLAG_ */
