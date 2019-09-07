//-----------------------------------------------------------------------------
// $Id: TTrack.h,v 1.21 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TTrack.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a track in tracking.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TTrack_FLAG_
#define TTrack_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

//...Definition of finder/manager...
#define TrackOldConformalFinder 1
#define TrackFastFinder         2
#define TrackSlowFinder         4
#define TrackCurlFinder         8
#define TrackTrackManager      16
#define TrackPMCurlFinder      32
#define TrackSVDAssociator     64
#define TrackFinderMask       255

//...Definition of type/kind...
#define TrackTypeUndefined        0
#define TrackTypeNormal           1
#define TrackTypeCurl             2
#define TrackTypeCircle           4
#define TrackTypeCosmic           8
#define TrackTypeIncomingCosmic   8
#define TrackTypeOutgoingCosmic  16
#define TrackTypeKink            32
#define TrackTypeSVDOnly        256

//...Definition of quality...
#define TrackQualityOutsideCurler   1
#define TrackQualityAfterKink       2
#define TrackQualityCosmic          4
#define TrackQuality2D              8
#define TrackQualityShift           8
#define TrackQualityMask          255

//...Definition of fitting status...
#define TrackFitGlobal         1
#define TrackFitCosmic         2
#define TrackFitCdcKalman      4
#define TrackFitSvdCdcKalman   8
#define TrackFitShift         16
#define TrackFitMask         255

//...Definition of relation...
#define TrackHasMother         1
#define TrackHasDaughter       2
#define TrackRelationShift    24
#define TrackRelationMask    255

#include <string>

#define HEP_SHORT_NAMES
#include "CLHEP/Vector/ThreeVector.h"
//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"
#include "TrkReco/TMDCUtil.h"
#include "TrkReco/TTrackBase.h"
#include "TrkReco/TMLink.h"
#include "TrkReco/TBuilder0.h"
#include "TrkReco/TBuilderCosmic.h"
#include "TrkReco/TBuilderCurl.h"
//#include "TrkReco/TMDCCatHit.h"
#include "TrkReco/TPoint2D.h"
//#include "TrkReco/TSvdHit.h"

#include "TrkReco/T3DLine.h"
#include "TrkReco/TRunge.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Geometry/Point3D.h"


using CLHEP::Hep3Vector;

#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif
using HepGeom::Point3D;

//class HepPoint3D;
class TrkReco;
class TTrackManager;
class TCircle;
class TCosmicFitter;
class Refit;
class THelixFitter;
class TBuilder;
//class TPMCurlFinder;
class TCurlFinder;
//struct reccdc_trk;
//struct reccdc_trk_add;
//struct rectrk_localz;
//struct gen_hepevt;
//struct mdst_trk_fit;
class MdcRec_trk;
class MdcRec_trk_add;
class MdcTrk_localz;
class Gen_hepevt;
class Mdst_trk_fit;

/// A class to represent a track in tracking.
class TTrack : public TTrackBase {

  public:
    /// Default constructor
    TTrack();

    /// Constructor
    TTrack(const TCircle &);

    /// Copy constructor
    TTrack(const TTrack &);

    /// Constructor
    TTrack(const T3DLine &);

    /// Constructor
    TTrack(const Helix &);

    TTrack(const TRunge &);
    /// Destructor
    virtual ~TTrack();

  public:// General information
    /// returns/sets name.
    const std::string & name(void) const;
    const std::string & name(const std::string & newName);

    /// sets/returns mother/daughter.
    TTrack * mother(void) const;
    TTrack * mother(TTrack *);
    TTrack * daughter(void) const;
    TTrack * daughter(TTrack *);

    /// returns type.
    unsigned objectType(void) const;

    /// returns type. Definition is depending on an object type.
    unsigned type(void) const;

    /// sets/returns finder.
    unsigned finder(void) const;
    unsigned finder(unsigned finderMask);
    void setFinderType(unsigned);
    unsigned getFinderType(void)const;

    /// sets/returns quality.
    unsigned quality(void) const;
    unsigned quality(unsigned qualityMask);

    /// sets/returns fitting status.
    unsigned fitting(void) const;
    unsigned fitting(unsigned fitMask);

    /// returns/sets internal state.(for bank output)
    unsigned state(void) const;

    /// assigns wire hits to this track.
    void assign(unsigned maskForWireHit);

    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

  public:// Kinematics
    /// returns charge.
    double charge(void) const;

    /// returns helix parameter.
    const Helix & helix(void) const;

    /// returns momentum.
    Hep3Vector p(void) const;

    /// returns magnitude of momentum.
    double ptot(void) const;

    /// returns Pt.
    double pt(void) const;

    /// returns Pz.
    double pz(void) const;

    /// returns signed impact parameter to the origin.
    double impact(void) const;

    /// returns position of helix center.
    TPoint2D center(void) const;

    /// returns signed radius.
    double radius(void) const;

    /// returns NDF.
    unsigned ndf(void) const;

    /// returns chi2.
    double chi2(void) const;

    /// returns confidence level.
    double confidenceLevel(void) const;

  public:// Utilities for segment operations.
    /// returns AList<TSegment>.
    AList<TSegment> & segments(void);
    const AList<TSegment> & segments(void) const;

  public:// Utilities
    /// calculates the closest approach to a wire in real space. Results are stored in TMLink. Return value is negative if error happened.
    int approach(TMLink &) const;
    int approach(TMLink &, bool sagCorrection) const;
    int approach2D(TMLink &) const;

    /// calculates arc length and z for a stereo hit.
    int szPosition(TMLink & link) const;

    /// calculates arc length and z for a segment. Results are stored in TMLink.
    int szPosition(const TSegment & segment, TMLink & link) const;
    
    /// calculates arc length for a point.
    int szPosition(const HepPoint3D & p, HepPoint3D & szPosition) const;

    /// calculates arc length and z for a stereo hit. uses these functions for curl tracks.
#define OLD_STEREO 1
#if OLD_STEREO
    int stereoHitForCurl(TMLink & link, AList<HepPoint3D> & arcZList) const;
    int stereoHitForCurl(TMLink & link, TMLink & link1) const;
    int stereoHitForCurl(TMLink & link, TMLink & link1, TMLink & link2) const;
    void deleteListForCurl(AList<HepPoint3D> &l1,
			   AList<HepPoint3D> &l2) const;
    void deleteListForCurl(AList<HepPoint3D> &l1,
			   AList<HepPoint3D> &l2,
			   AList<HepPoint3D> &l3) const;
#endif
    /// calculates arc length and z for a stereo hit. uses these functions for curl tracks(included svd version).
    int stereoHitForCurl(AList<TMLink>&) const;
    ////int svdHitForCurl(AList<TSvdHit>&) const;

  public:// Modifiers
    /// sets charge.
    double charge(double);

    /// fits itself. Error was happened if return value is not zero.
    // int fitx(void);

    /// fits itself with r-phi view. Error was happened if return value is not zero.
    int fit2D(unsigned = 0, double = 0.1, double = 0.015);

    /// fits itself with cathode hits.
    //    int fitWithCathode( float windowSize = 0.6 , int SysCorr = 0 ); // mod by matsu ( 1999/07/05 )

    /// remove bad points by chi2. Bad points are returned in a 'list'. fit() should be called before calling this function. (using stereo wire as axial wire(z=0))
    void refine2D(AList<TMLink> & list, float maxSigma);

    /// moves pivot to the inner most hit.
    void movePivot(void);

  public:// temporary added
    /// returns a cathode hit list.
    //    const AList<TMDCCatHit> & catHits(void) const;

    /// calculates an intersection of this track and a cylinder.
    int HelCyl(double rhole,
	       double rcyl,
	       double zb,
	       double zf,
	       double epsl,
	       double & phi,
	       HepPoint3D & xp) const;

    /// finds cathode hits associated to this track.
    //    void findCatHit(unsigned trackid);

   /// relation between cluster and wire
    //   void relationClusterWithWire();

   /// relation between cluster and layer
    //   void relationClusterWithLayer(int SysCorr ); // by matsu ( 1999/07/05 )

   /// appends Svd Hits(TSvdHit)
    //   void svdHits(AList<TSvdHit> &);

   /// returns Svd Hits List
    //   AList<TSvdHit> & svdHits(void);
    //   const AList<TSvdHit> & svdHits(void) const;

   /// sets/returns a list of TMLink which are used for table output.
   const AList<TMLink> & finalHits(void) const;
   const AList<TMLink> & finalHits(const AList<TMLink> & hits);

   /// sets/returns a list of associated TMLink which are used for table output.
   const AList<TMLink> & associateHits(void) const;
   const AList<TMLink> & associateHits(const AList<TMLink> & hits);

  private:
    /// calculates dXda. 'link' and 'dPhi' are inputs. Others are outputs.
    int dxda(const TMLink & link,
	     double dPhi,
	     Vector & dxda,
	     Vector & dyda,
	     Vector & dzda) const;

    int dxda2D(const TMLink & link,
               double dPhi,
               Vector & dxda,
               Vector & dyda,
               Vector & dzda) const;

    int dxda2D(const TMLink & link,
               double dPhi,
               Vector & dxda,
               Vector & dyda) const;

    int dxda2D(double dPhi,
               Vector & dxda,
               Vector & dyda) const;

    unsigned defineType(void) const;

    unsigned _findertype;
  private:// Always updated
    unsigned _state;
    double _charge;
    AList<TSegment> _segments;
    TTrack * _mother;
    TTrack * _daughter;
    //    AList<TSvdHit> _svdHits;
    AList<TMLink> _finalHits;
    AList<TMLink> _associateHits;

  private:// Updated when fitted
    //static const THelixFitter _fitter;//yzhang delete 2012-05-04 
    static const THelixFitter _fitter;//yzhang add
    Helix *  _helix;
    unsigned _ndf;
    double _chi2;
    std::string _name;
    mutable unsigned _type;

  private:// temporary added
    //    AList<TMDCCatHit> _catHits;

    friend class TTrackManager;
    friend class TrkReco;
    friend class THelixFitter;
    friend class TCosmicFitter;
    friend class Refit;
    friend class TBuilder;
    friend class TBuilder0;
    friend class TBuilderCosmic;
    friend class TBuilderCurl;
    friend class TPMCurlFinder;
    friend class TCurlFinder;

  private:
    mutable IMagneticFieldSvc* m_pmgnIMF;


};

/// Utility functions
#if defined(__GNUG__)
int
SortByPt(const TTrack ** a, const TTrack ** b);
#else
extern "C" int
SortByPt(const void* a, const void* b);
#endif

/// to dump a track.
inline std::string
TrackDump(const TTrack &);

/// returns string of track type.
inline std::string
TrackType(const TTrack &);

inline std::string
TrackType(unsigned type);

/// returns string of track status.
std::string
TrackStatus(const TTrack &);

std::string
TrackStatus(const MdcRec_trk &);

std::string
TrackStatus(const MdcRec_trk_add &);

std::string
TrackStatus(unsigned md,
	    unsigned mk,
	    unsigned mq,
	    unsigned ms,
	    unsigned mm,
	    unsigned ma);

inline std::string
TrackKinematics(const TTrack &);

std::string
TrackKinematics(const Helix &);

std::string
TrackInformation(const TTrack &);

std::string
TrackInformation(const MdcRec_trk &);

std::string
TrackInformation(unsigned nA, unsigned nS, unsigned n, float chisq);

std::string
TrackLayerUsage(const TTrack &);

/// returns helix.
Helix
Track2Helix(const MdcRec_trk &);

Helix
Track2Helix(const MdcTrk_localz &);

Helix
Track2Helix(const Gen_hepevt &);

Helix
Track2Helix(const Mdst_trk_fit &);

/// Helix parameter validity
bool
HelixHasNan(const Helix &);

/// Error matrix validity
bool
PositiveDefinite(const Helix &);

//-----------------------------------------------------------------------------

#ifdef TTrack_NO_INLINE
#define inline
#else
#undef inline
#define TTrack_INLINE_DEFINE_HERE
#endif

#ifdef TTrack_INLINE_DEFINE_HERE

inline
const Helix &
TTrack::helix(void) const {
#ifdef TRKRECO_DEBUG
    // if (! _fitted) std::cout << "TTrack::helix !!! helix not updated" << std::endl;
#endif
    return * _helix;
}

inline
unsigned
TTrack::ndf(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TTrack::ndf !!! ndf not updated" << std::endl;
#endif
    return _ndf;
}

inline
double
TTrack::chi2(void) const {
#ifdef TRKRECO_DEBUG
    if (! _fitted) std::cout << "TTrack::chi2 !!! chi2 not updated" << std::endl;
#endif
    return _chi2;
}

inline
double
TTrack::charge(void) const {
    return _charge;
}

inline
double
TTrack::charge(double a) {
    return _charge = a;
}

inline
const std::string &
TTrack::name(void) const {
    return _name;
}

inline
const std::string &
TTrack::name(const std::string & a) {
    return _name = a;
}

inline
double
TTrack::pt(void) const {
    return 1. / fabs(_helix->a()[2]);
}

inline
double
TTrack::pz(void) const {
    return (1. / fabs(_helix->a()[2])) * _helix->a()[4];
}

inline
double
TTrack::ptot(void) const {
    return (1. / fabs(_helix->a()[2])) * 
	sqrt(1. + _helix->a()[4] * _helix->a()[4]);
}

inline
double
TTrack::confidenceLevel(void) const {
    return chisq2confLevel((int) _ndf, _chi2);
}

inline
Hep3Vector
TTrack::p(void) const {
    return _helix->momentum(0.);
}

inline
unsigned
TTrack::objectType(void) const {
    return Track;
}

inline
unsigned
TTrack::type(void) const {
    return defineType();
}

inline
double
TTrack::impact(void) const {
    return fabs(_helix->radius()) - _helix->center().mag();
}

inline
double
TTrack::radius(void) const {
    return _helix->radius();
}

inline
AList<TSegment> &
TTrack::segments(void) {
    return _segments;
}

inline
const AList<TSegment> &
TTrack::segments(void) const {
    return _segments;
}

inline
TPoint2D
TTrack::center(void) const {
    return TPoint2D(_helix->center());
}

inline
unsigned
TTrack::finder(void) const {
    return _state & TrackFinderMask;
}

inline
unsigned
TTrack::finder(unsigned a) {
    _state |= (a & TrackFinderMask);
    return finder();
}

inline
unsigned
TTrack::quality(void) const {
    return (_state >> TrackQualityShift) & TrackQualityMask;
}

inline
unsigned
TTrack::quality(unsigned a) {
//      _state = ((a << TrackQualityShift) & TrackQualityMask) |
//  	(_state & (~ (TrackQualityMask << TrackQualityShift)));
    _state = ((a & TrackQualityMask) << TrackQualityShift) |
	(_state & (~ (TrackQualityMask << TrackQualityShift)));
    return quality();
}

inline
unsigned
TTrack::fitting(void) const {
    return (_state >> TrackFitShift) & TrackFitMask;
}

inline
unsigned
TTrack::fitting(unsigned a) {
    _state |= ((a << TrackFitShift) & TrackFitMask);
    return fitting();
}

inline
TTrack *
TTrack::mother(void) const {
    return _mother;
}

inline
TTrack *
TTrack::mother(TTrack * a) {
    if (a) _state |= (TrackHasMother << TrackRelationShift);
    else   _state &= (~(TrackHasMother << TrackRelationShift));
    return _mother = a;
}

inline
TTrack *
TTrack::daughter(void) const {
    return _daughter;
}

inline
TTrack *
TTrack::daughter(TTrack * a) {
    if (a) _state |= (TrackHasDaughter << TrackRelationShift);
    else   _state &= (~(TrackHasDaughter << TrackRelationShift));
    return _daughter = a;
}

inline
unsigned
TTrack::state(void) const {
    return _state;
}

inline
void TTrack::setFinderType(unsigned type){
        _findertype=type;
}
inline
unsigned TTrack::getFinderType(void)const{
        return _findertype;
}
inline
std::string
TrackType(const TTrack & t) {
    return TrackType(t.type());
}

inline
std::string
TrackKinematics(const TTrack & t) {
    return TrackKinematics(t.helix());
}

inline
std::string
TrackDump(const TTrack & t) {
    return t.name() + " " + TrackStatus(t) + " " + TrackKinematics(t) + " " +
	TrackInformation(t);
}

inline
const AList<TMLink> &
TTrack::finalHits(const AList<TMLink> & list) {
    _finalHits = list;
    return _finalHits;
}

inline
const AList<TMLink> &
TTrack::finalHits(void) const {
    return _finalHits;
}

inline
const AList<TMLink> &
TTrack::associateHits(const AList<TMLink> & list) {
    _associateHits = list;
    return _associateHits;
}

inline
const AList<TMLink> &
TTrack::associateHits(void) const {
    return _associateHits;
}


#endif

#undef inline

#endif /* TTrack_FLAG_ */
