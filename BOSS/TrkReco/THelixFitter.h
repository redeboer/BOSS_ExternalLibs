//-----------------------------------------------------------------------------
// $Id: THelixFitter.h,v 1.14 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : THelixFitter.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a helix.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef THELIXFITTER_FLAG_
#define THELIXFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES
#include "CLHEP/Matrix/Vector.h"
//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"
#include "TrkReco/TMFitter.h"
#include "TrkReco/TTrackBase.h"

#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"


class TMLink;

using CLHEP::HepVector;

//...Drift time correction method...
#define TrackFitNoCorrection          0
#define TrackFitCorrectOnce           1
#define TrackFitCorrectEveryIteration 2

/// A class to fit a TTrackBase object to a helix.
class THelixFitter : public TMFitter {

  public:
    /// Constructor.
    THelixFitter(const std::string & name);

    /// Destructor
    virtual ~THelixFitter();

  public:// Informations
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Options
    /// sets/returns 2D flag.
    bool fit2D(void) const;
    bool fit2D(bool);

    /// sets/returns free T0 flag.
    bool freeT0(void) const;
    bool freeT0(bool);

    /// sets/returns correctin flag.
    unsigned corrections(void) const;
    unsigned corrections(unsigned);

  public:// Options (obsolete)
    /// sets/returns sag correction flag.
    bool sag(void) const;
    bool sag(bool);

    /// sets/returns propagation-delay correction flag.
    bool propagation(void) const;
    bool propagation(bool);

    /// sets/returns propagation-delay correction flag.
    bool tof(void) const;
    bool tof(bool);

    /// sets/returns tanLambda correction flag.
    bool tanl(void) const;
    bool tanl(bool);

    /// returns sum of chi2 before fit.
    double preChi2(void) const;

    /// returns sum of chi2 aftter fit.
    double chi2(void) const;

    IMagneticFieldSvc* getMagneticFieldPointer(void) const{return m_pmgnIMF;}
  public:// Fitting
    int fit(TTrackBase &) const;
    int fit(TTrackBase &, double *pre_chi2, double *fitted_chi2) const;
    int fit(TTrackBase &, float t0Offset, double *pre_chi2=NULL, double *fitted_chi2=NULL) const;
    int fit(TTrackBase &, float & tev, float & tev_err, double *pre_chi2=NULL, double *fitted_chi2=NULL) const;

  private:
    /// main routine for fixed T0.
    int main(TTrackBase &, float t0Offset, double *pre_chi2=NULL, double *fitted_chi2=NULL) const;

    /// main routine for free T0.
    int main(TTrackBase &, float & tev, float & tev_err, double *pre_chi2=NULL, double *fitted_chi2=NULL) const;

    /// calculates drift distance and its error.
    void drift(const TTrack &,
	       TMLink &,
	       float t0Offset,
	       double & distance,
	       double & itsError) const;
//be cautious here
    /// calculates drift distance and its error for free T0 case.
    void drift(const TTrack &,
	       TMLink &,
	       float t0Offset,
	       double & distance,
	       double & itsError,
	       double & ddda) const;
//be cautious here
    /// calculates dXda. 'link' and 'dPhi' are inputs. Others are outputs.
    // virtual
    int dxda(const TMLink & link,
	     const Helix & helix,
	     double dPhi,
	     HepVector & dxda,
	     HepVector & dyda,
	     HepVector & dzda) const;

  private:
    bool _fit2D;
    bool _freeT0;
    unsigned _corrections;

    bool _sag;
    int _propagation;
    bool _tof;
    bool _tanl;
    
    //jialk
    double _driftTime;
    mutable double _pre_chi2;
    mutable double _fitted_chi2;
  private:
    mutable IMagneticFieldSvc* m_pmgnIMF;


};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define THELIXFITTER_INLINE_DEFINE_HERE
#endif

#ifdef THELIXFITTER_INLINE_DEFINE_HERE

inline
bool
THelixFitter::fit2D(void) const {
    return _fit2D;
}

inline
bool
THelixFitter::fit2D(bool a) {
    return _fit2D = a;
}

inline
bool
THelixFitter::sag(void) const {
    return _sag;
}

inline
bool
THelixFitter::sag(bool a) {
    return _sag = a;
}

inline
bool
THelixFitter::propagation(void) const {
    return (bool) _propagation;
}

inline
bool
THelixFitter::propagation(bool a) {
    if (a) _propagation = 1;
    else   _propagation = 0;
    return propagation();
}

inline
bool
THelixFitter::tof(void) const {
    return _tof;
}

inline
bool
THelixFitter::tof(bool a) {
    return _tof = a;
}

inline
bool
THelixFitter::tanl(void) const {
    return _tanl;
}

inline
bool
THelixFitter::tanl(bool a) {
    return _tanl = a;
}

inline
int
THelixFitter::fit(TTrackBase & a) const {
    if (! _freeT0) return main(a, 0.);
    else {
	float tev = 0.;
	float tevError;
	return main(a, tev, tevError);
    }
}

inline
int
THelixFitter::fit(TTrackBase & a,
		  double *pre_chi2, double *fitted_chi2) const {
    if (! _freeT0) return main(a, 0., pre_chi2, fitted_chi2);
    else {
	float tev = 0.;
	float tevError;
	return main(a, tev, tevError, pre_chi2, fitted_chi2);
    }
}

inline
int
THelixFitter::fit(TTrackBase & a, float t0Offset,
		  double *pre_chi2, double *fitted_chi2) const {
    a._fitted = false;
    if (! _freeT0) return main(a, t0Offset, pre_chi2, fitted_chi2);
    else {
	float tev = t0Offset;
	float tevError;
	return main(a, tev, tevError, pre_chi2, fitted_chi2);
    }
}

inline
int
THelixFitter::fit(TTrackBase & a, float & tev, float & tev_err,
		  double *pre_chi2, double *fitted_chi2) const {
    a._fitted = false;
    return main(a, tev, tev_err, pre_chi2, fitted_chi2);
}

inline
bool
THelixFitter::freeT0(void) const {
    return _freeT0;
}

inline
bool
THelixFitter::freeT0(bool a) {
    return _freeT0 = a;
}

inline
unsigned
THelixFitter::corrections(void) const {
    return _corrections;
}

inline
unsigned
THelixFitter::corrections(unsigned a) {
    return _corrections = a;
}

inline
double
THelixFitter::preChi2(void) const {
    return _pre_chi2;
}

inline
double
THelixFitter::chi2(void) const {
    return _fitted_chi2;
}

#endif

#undef inline

#endif /* THELIXFITTER_FLAG_ */
