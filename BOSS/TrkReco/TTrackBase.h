//-----------------------------------------------------------------------------
// $Id: TTrackBase.h,v 1.14 2011/10/08 01:56:15 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TTrackBase.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A virtual class for a track class in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TTrackBase_FLAG_
#define TTrackBase_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#define TrackAxialLayer 0
#define TrackStereoLayer 1
#define TrackBothLayer 2

//...Definition of objectType...
#define TrackBase            1
#define Circle               2
#define Line                 4
#define Track                8
#define Segment             16

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Alist/AList.h"

#include "TrkReco/TMDCUtil.h"

class TMLink;
class TTrackMC;
class TMFitter;
class TTrackHEP;

/// A virtual class for a track class in tracking.
class TTrackBase {

  public:
    /// Constructor.
    TTrackBase();

    /// Constructor.
    TTrackBase(const AList<TMLink> & links);

    /// Destructor
    virtual ~TTrackBase();

  public:// General information
    /// returns object type.
    virtual unsigned objectType(void) const;

    /// returns type. Definition is depending on an object class.
    virtual unsigned type(void) const;

    /// dumps debug information.
    virtual void dump(const std::string & message = std::string(""),
		      const std::string & prefix = std::string("")) const;

  public:// Hit information
    /// returns a list of masked TMLinks assigned to this track. 'mask' will be applied if mask is not 0.
    const AList<TMLink> & links(unsigned mask = 0) const;

    /// returns \# of masked TMLinks assigned to this track object.
    unsigned nLinks(unsigned mask = 0) const;

    /// returns a list of masked TMLinks for fit. 'mask' will be applied if mask is not 0.
    const AList<TMLink> & cores(unsigned mask = 0) const;

    /// returns \# of masked TMLinks for fit. 'mask' will be applied if mask is not 0.
    unsigned nCores(unsigned mask = 0) const;

  public:// Hit manipulation
    /// update cache.
    void update(void) const;
    
    /// appends a TMLink.
    void append(TMLink &);

    /// appends TMLinks.
    void append(const AList<TMLink> &);

    /// appends TMLinks by approach. 'list' is an input. Unappended TMLinks will be removed from 'list' when returned.
    void appendByApproach(AList<TMLink> & list, double maxSigma);

    /// appends TMLinks by distance. 'list' is an input. Unappended TMLinks will be removed from 'list' when returned.
    void appendByDistance(AList<TMLink> & list, double maxDistance);

    /// removes a TMLink.
    void remove(TMLink & a);

    /// removes TMLinks.
    void remove(const AList<TMLink> &);

    /// removes bad points by pull. The bad points are removed from the track, and are returned in 'list'.
    virtual void refine(AList<TMLink> & list, double maxSigma);

    /// removes bad points by pull. The bad points are masked not to be used in fit.
    virtual void refine(double maxSigma);
    
    virtual int DropWorst();

    virtual void removeLinks(void);

    /// returns distance to a position of TMLink in TMLink space.
    virtual double distance(const TMLink &) const;

    /// calculates the closest approach to a wire in real space. Results are stored in TMLink. Return value is negative if error happened.
    virtual int approach(TMLink &) const;

    /// returns \# of good hits to be appended.
    unsigned testByApproach(const TMLink & list, double sigma) const;
    unsigned testByApproach(const AList<TMLink> & list, double sigma) const;

    /// fits itself by a default fitter. Error was happened if return value is not zero.
    virtual int fit(void);

    /// returns a pointer to a default fitter.
    const TMFitter * const fitter(void) const;

    /// sets a default fitter.
    const TMFitter * const fitter(const TMFitter *);

    /// false Fit
    void falseFit(); // added by matsu ( 1999/05/24 )

  public:// Operators
    TMLink * operator[](unsigned i) const;

  public:// MC information
    /// returns TTrackHEP.
    const TTrackHEP * const hep(void) const;

    /// returns \# of contributed TTrackHEP tracks.
    unsigned nHeps(void) const;

    /// returns a pointer to TTrackMC.
    const TTrackMC * const mc(void) const;

  public:// Obsolete functions. Followings should be removed in TrkReco 2.
    /// returns true if fitted.
    bool fitted(void) const;

    /// returns true if fitted with cathode hits(TEMPORARY).
    bool fittedWithCathode(void) const;

  private:
    /// refine core part.
    AList<TMLink> refineMain(double maxSigma);

  protected:// Always updated
    AList<TMLink> _links;
    mutable bool _fitted;
    mutable bool _fittedWithCathode;
    // unsigned _state;
    TTrackMC * _mc;

  private:// Always updated
    mutable bool _updated;

  private:// Updated when accessed
    mutable AList<TMLink> _cores;

  private:// Always updated when accessed 
    mutable const TTrackHEP * _hep;
    mutable unsigned _nHeps;

  private:
    const TMFitter * _fitter;
    float _time;

    friend class TMFitter;
    friend class THelixFitter;
    friend class TCosmicFitter;

#ifdef TRKRECO_DEBUG
  public:
    bool fitted(bool) const;
#endif
};

//-----------------------------------------------------------------------------

#ifdef TTrackBase_NO_INLINE
#define inline
#else
#undef inline
#define TTrackBase_INLINE_DEFINE_HERE
#endif

#ifdef TTrackBase_INLINE_DEFINE_HERE

inline
void
TTrackBase::remove(TMLink & a) {
    _links.remove(a);
    _updated = false;
    _fitted = false;
    _fittedWithCathode = false; // mod. by matsu ( 1999/05/24 )
}

inline
void
TTrackBase::remove(const AList<TMLink> & a) {
    _links.remove(a);
    _updated = false;
    _fitted = false;
    _fittedWithCathode = false; // mod. by matsu ( 1999/05/24 )
}

inline
bool
TTrackBase::fitted(void) const {
    return _fitted;
}

// added by matsu ( 1999/05/24 )
inline
void
TTrackBase::falseFit(){
    _fitted = false;
    _fittedWithCathode = false;
}
// end of addition

inline
TMLink *
TTrackBase::operator[](unsigned i) const {
    return _links[i];
}

inline
bool
TTrackBase::fittedWithCathode(void) const {
    return _fittedWithCathode;
}

inline
const TTrackMC * const
TTrackBase::mc(void) const {
    return _mc;
}

inline
const TMFitter * const
TTrackBase::fitter(void) const {
    return _fitter;
}

inline
const TMFitter * const
TTrackBase::fitter(const TMFitter * a) {
    _fitted = false;
    return _fitter = a;
}

inline
unsigned
TTrackBase::objectType(void) const {
    return TrackBase;
}

inline
unsigned
TTrackBase::type(void) const {
    return 0;
}

#ifdef TRKRECO_DEBUG
inline
bool
TTrackBase::fitted(bool a) const {
    return _fitted = a;
}
#endif

#endif

#undef inline

#endif /* TTrackBase_FLAG_ */
