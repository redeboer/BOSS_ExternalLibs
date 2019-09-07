//-----------------------------------------------------------------------------
// $Id: TMLink.h,v 1.17 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TMLink.h
// Section  : Tracking
// Owner    : Yoshi Iwasaki
// Email    : yoshihito.iwasaki@kek.jp
//-----------------------------------------------------------------------------
// Description : A class to relate TMDCWireHit and TTrack objects.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TMLink_FLAG_
#define TMLink_FLAG_

#include <iostream>

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#define HEP_SHORT_NAMES
#include <string>
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

#include "CLHEP/Alist/ConstAList.h"
#include "TrkReco/TMDCWire.h"
#include "TrkReco/TMDCWireHit.h"
//#include "TrkReco/TMDCWire.h"
//#include "TrkReco/TMDCClust.h"

class TTrackHEP;

class TTrack;

/// A class to relate TMDCWireHit and TTrack objects.
class TMLink {

  public:
    /// Constructor.
/*    TMLink(TTrack * track = 0,
	  const TMDCWireHit * hit = 0,
	  const HepPoint3D & position = HepPoint3D());*/
    TMLink(TTrack * track = 0,
          const TMDCWireHit * hit = 0,
          const HepPoint3D & position = HepPoint3D(),
          const HepPoint3D & positionD = HepPoint3D(),
          const double = 0.0);

    /// Copy constructor.
    TMLink(const TMLink &);

    /// Destructor
    virtual ~TMLink();

  public:// Selectors
    /// dumps debug information.
    void dump(const std::string & message = std::string(""),
	      const std::string & prefix = std::string("")) const;

    /// returns a pointer to a hit.
    const TMDCWireHit * hit(void) const;

    /// returns a pointer to a wire.
    const TMDCWire * const wire(void) const;

    /// returns a pointer to a track.
    TTrack * track(void) const;

    /// returns pull.
    double pull(void) const;

    /// returns middle position of a wire. z componet is 0.
    const HepPoint3D & xyPosition(void) const;

    /// returns the closest point on wire to a track.
    const HepPoint3D & positionOnWire(void) const;

    /// returns the closest point on track to wire.
    const HepPoint3D & positionOnTrack(void) const;

    /// returns dPhi to the closest point.
    double dPhi(void) const;

    /// returns left-right. 0:left, 1:right, 2:wire
    unsigned leftRight(void) const;

     /// returns stauts of stereo hit 
    int zStatus(void) const;

    /// returns id\#  of the pair, if zStatus == 20 (2 consective hits).  
    int zPair(void) const;

    /// returns position.
    const HepPoint3D & position(void) const;
    const HepPoint3D & positionD(void) const;

  
    /// returns distance between point on wire and on track.
    double distance(void) const;
     
    double distance(double);

    double distancenew(void)const;

    /// returns neighbor TMLink.
    TMLink * neighbor(unsigned int) const;

    /// returns a pointer to a TMLink.
    TMLink * link(void) const;

// S.Suzuki added
    /// returns UseCathode flag
    //    int usecathode(void) const;

    /// returns pointer to the cluster to be fit
    //    TMDCClust * getmclust(void) const;
// end S.Suzuki

    /// returns arc and Z for the curl finder.
    const HepPoint3D & arcZ(const unsigned = 0) const;

  public:// tmp
//    const HepPoint3D & conf(void) const;
    const HepPoint3D & conf(const HepPoint3D & conf);

  public:// Modifiers
    /// sets results of fitting.
    void update(const HepPoint3D & onTrack,
		const HepPoint3D & onWire,
		unsigned leftRight,
		double pull);

    /// sets a pointer to a hit.
    const TMDCWireHit * hit(const TMDCWireHit *);

    /// sets a pointer to a track.
    TTrack * track(TTrack *);

    /// sets and returns the closest point on wire to a track.
    const HepPoint3D & positionOnWire(const HepPoint3D &);
    const HepPoint3D & positionOnWire(double p[3]);

    /// sets and returns the closest point on track to wire.
    const HepPoint3D & positionOnTrack(const HepPoint3D &);
    const HepPoint3D & positionOnTrack(double p[3]);

    /// sets and returns dPhi to the closest point.
    double dPhi(double);

    /// sets and returns position.
    const HepPoint3D & position(const HepPoint3D &);
    const HepPoint3D & positionD(const HepPoint3D &);

    /// sets neighbor TMLink.
    void neighbor(unsigned int, TMLink *);

    /// sets a pointer to a TMLink.
    TMLink * link(TMLink *);

    /// sets left-right. 0:left, 1:right, 2:wire
    unsigned leftRight(unsigned);

    /// sets pull.
    double pull(double);

    /// sets stereo Hit status
    int zStatus(int);

    /// sets id\# of the pair
    int zPair(int);

// S.Suzuki added

    /// sets usecathode flag 
    //    void setusecathode(int);

    /// sets pointer to the cluster to be fit
    //    void setmclust(TMDCClust *);

    /// sets z(phi) before cathode fit
    //    void setZphiBeforeCathode(float);

// end S.Suzuki

    /// sets and returns arc and Z for the curl finder.
    const HepPoint3D & arcZ(const HepPoint3D &, const unsigned = 0);
    const unsigned fit2D(const unsigned &);
    unsigned fit2D(void);

  public:// Drift distance
    /// returns/sets drift distance.
    float drift(void) const;
    float drift(unsigned) const;
    float drift(float, unsigned);
    double cDrift(void) const;
    double cDrift(unsigned) const;
    double cDrift(double, unsigned);

    /// returns/sets drift distance error.
    float dDrift(void) const;
    float dDrift(unsigned) const;
    float dDrift(float, unsigned);

    /// return tsfTag of links
    unsigned tsfTag(void) const;
    unsigned tsfTag(unsigned);

    /// return time of flight
    double tof(void) const;
    double tof(double);

    /// add by jialk returns timeDrift after prop correction
    void setDriftTime(double);
    double getDriftTime(void);

    /// returs drift time
   double DriftTime(double,double) const;
  private:
    TTrack * _track;
    const TMDCWireHit * _hit;
    HepPoint3D _onTrack;
    HepPoint3D _onWire;
    HepPoint3D _position;
    HepPoint3D _positionD;
    double _dPhi;
    unsigned _leftRight;
    float _drift[2];
    float _dDrift[2];

    double _cDrift[2];

    int _zStatus;
    int _zPair;
    double _pull;
    double _distance;
    TMLink * _neighbor[6];
    TMLink * _link;

    //...tmp...
    HepPoint3D _conf;

    double _tof;
    double _drifttime;

   //-- S. Suzuki added -------
    //    int _usecathode;
    //    float _ZphiBeforeCathode;
    //    TMDCClust * _mclust;
    //-- S. Suzuki added end ---

    HepPoint3D _arcZ[4];
    unsigned _fit2D;
    unsigned _tsfTag;
};

// Utility functions

/// returns TTrackHEP
const TTrackHEP &
Links2HEP(const AList<TMLink> & links);

/// returns \# of layers.
unsigned
NSuperLayers(const AList<TMLink> & links);

unsigned
NSuperLayers(const AList<TMLink> & links, unsigned minNHits);

unsigned
NMissingAxialSuperLayers(const AList<TMLink> & links);

/// returns \# of layers.
unsigned
NLayers(const AList<TMLink> & links);

/// returns \# of hits per layer.
void
NHits(const AList<TMLink> & links, unsigned nHits[50]);

/// returns \# of hits per super layer.
void
NHitsSuperLayer(const AList<TMLink> & links, unsigned nHits[11]);

/// returns axial hits.
AList<TMLink>
AxialHits(const AList<TMLink> & links);

/// returns stereo hits.
AList<TMLink>
StereoHits(const AList<TMLink> & links);

/// returns \# of axial hits.
unsigned
NAxialHits(const AList<TMLink> & links);

/// returns \# of stereo hits.
unsigned
NStereoHits(const AList<TMLink> & links);

/// returns width(wire cell unit) of given AList<TMLink>. This function assumes that all TMLink's are in the same super layer.
unsigned
Width(const AList<TMLink> &);

/// returns links which are edges. This function assumes that all TMLink's are in the same super layer.
AList<TMLink>
Edges(const AList<TMLink> &);

/// returns links which are in the same layer as 'a' or 'id'.
AList<TMLink>
SameLayer(const AList<TMLink> & list, const TMLink & a);
AList<TMLink>
SameLayer(const AList<TMLink> & list, unsigned id);

/// returns links which are in the same super layer as 'a' or 'id'.
AList<TMLink>
SameSuperLayer(const AList<TMLink> & list, const TMLink & a);
AList<TMLink> 
SameSuperLayer(const AList<TMLink> & list, unsigned id);

/// returns super layer pattern.
unsigned
SuperLayer(const AList<TMLink> & list);
unsigned
SuperLayer(const AList<TMLink> & list, unsigned minNHits);

/// returns the inner(outer)-most link.
TMLink *
InnerMost(const AList<TMLink> & links);
TMLink *
OuterMost(const AList<TMLink> & links);

/// returns links which are in the inner most and outer most layer. This function assumes that all TMLink's are in the same super layer.
AList<TMLink>
InOut(const AList<TMLink> &);

template <class T> class CAList;

/// separate cores and non-cores.
void
SeparateCores(const AList<TMLink> & input,
	      AList<TMLink> & cores,
	      AList<TMLink> & nonCores);

AList<TMLink>
Cores(const AList<TMLink> & input);

/// dumps TMLinks.
void
Dump(const CAList<TMLink> & links,
     const std::string & message = std::string(""),
     const std::string & prefix = std::string(""));
void
Dump(const TMLink & link,
     const std::string & message = std::string(""),
     const std::string & prefix = std::string(""));

/// Sorter
#if defined(__GNUG__)
int
SortByWireId(const TMLink ** a, const TMLink ** b);
int
SortByX(const TMLink ** a, const TMLink ** b);
#else
extern "C" int
SortByWireId(const void* a, const void* b);
extern "C" int
SortByX(const void* a, const void* b);
#endif

//-----------------------------------------------------------------------------

#ifdef TMLink_NO_INLINE
#define inline
#else
#undef inline
#define TMLink_INLINE_DEFINE_HERE
#endif

#ifdef TMLink_INLINE_DEFINE_HERE

inline
const TMDCWireHit *
TMLink::hit(void) const {
    return _hit;
}

inline
TTrack *
TMLink::track(void) const {
    return _track;
}

inline
const TMDCWireHit *
TMLink::hit(const TMDCWireHit * a) {
    return _hit = a;
}

inline
TTrack *
TMLink::track(TTrack * a) {
    return _track = a;
}

inline
void
TMLink::update(const HepPoint3D & onTrack,
	      const HepPoint3D & onWire,
	      unsigned leftRight,
	      double pull) {
    _onTrack = onTrack;
    _onWire = onWire;
    _leftRight = leftRight;
    _pull = pull;
}

inline
double
TMLink::pull(void) const {
    return _pull;
}

inline
double
TMLink::pull(double a) {
    return _pull = a;
}

inline
const HepPoint3D &
TMLink::positionOnWire(void) const {
    return _onWire;
}

inline
const HepPoint3D &
TMLink::positionOnTrack(void) const {
    return _onTrack;
}

inline
const HepPoint3D &
TMLink::positionOnWire(const HepPoint3D & a) {
    return _onWire = a;
}

inline
const HepPoint3D &
TMLink::positionOnWire(double p[3]) {
    _onWire.setX(p[0]);
    _onWire.setY(p[1]);
    _onWire.setZ(p[2]);
    return _onWire;
}

inline
const HepPoint3D &
TMLink::positionOnTrack(const HepPoint3D & a) {
    return _onTrack = a;
}

inline
const HepPoint3D &
TMLink::positionOnTrack(double p[3]) {
    _onTrack.setX(p[0]);
    _onTrack.setY(p[1]);
    _onTrack.setZ(p[2]);
    return _onTrack;
}

inline
unsigned
TMLink::leftRight(void) const {
    return _leftRight;
}

inline
unsigned
TMLink::leftRight(unsigned a) {
    return _leftRight = a;
}

inline
double
TMLink::dPhi(void) const {
    return _dPhi;
}

inline
double
TMLink::dPhi(double a) {
    return _dPhi = a;
}

inline
const HepPoint3D &
TMLink::position(void) const {
    return _position;
}

inline
const HepPoint3D &
TMLink::position(const HepPoint3D & a) {
    return _position = a;
}

inline
const HepPoint3D &
TMLink::positionD(void) const {
    return _positionD;
}
                                                                                                                             
inline
const HepPoint3D &
TMLink::positionD(const HepPoint3D & a) {
    return _positionD = a;
}

// inline
// const HepPoint3D &
// TMLink::conf(void) const {
//     return _conf;
// }

inline
const HepPoint3D &
TMLink::conf(const HepPoint3D & a) {
    return _conf = a;
}

inline
void 
TMLink::neighbor(unsigned n, TMLink * neighbor){
  if(n <= 6)
    _neighbor[n] = neighbor;
}

inline
TMLink *
TMLink::neighbor(unsigned n) const {
  if(n <= 6)
    return _neighbor[n];
  return NULL;
}

inline
TMLink *
TMLink::link(void) const {
    return _link;
}

inline
TMLink *
TMLink::link(TMLink * a) {
    return _link = a;
}

inline
double
TMLink::distance(void) const {

    return (_onTrack-_onWire).mag() ;
}
inline
double
TMLink::distancenew(void) const {


        return _distance;

}

inline
double
TMLink::distance(double a)  {

        _distance=a ;
}
//inline
//const TMDCWire * const
//TMLink::wire(void) const {
//    if (_hit) return _hit->wire();
//    return NULL;
//}

//zangsl 040518 move the following two fun. to TMLink.cxx file
/*
inline
const TMDCWire * const
TMLink::wire(void) const {
    if (_hit) return _hit->wire();
    return NULL;
}

inline
const HepPoint3D &
TMLink::xyPosition(void) const {
    return _hit->wire()->xyPosition();
}
*/

inline
int
TMLink::zStatus(void) const {
    return _zStatus;
}

inline
int
TMLink::zStatus(int a) {
    return _zStatus = a;
}

inline
int
TMLink::zPair(void) const {
    return _zPair;
}

inline
int
TMLink::zPair(int a) {
    return _zPair = a;
}

/*
//-- S. Suzuki added -------
inline
int 
TMLink::usecathode(void) const {
    return _usecathode;
}

inline
void
TMLink::setusecathode(int a) {
    _usecathode = a ;
}

inline 
TMDCClust *
TMLink::getmclust(void) const {
    return _mclust;
}

inline
void
TMLink::setmclust(TMDCClust * a) {
    _mclust = a ;
}

//-- S. Suzuki added end ----
*/

inline
const HepPoint3D &
TMLink::arcZ(const unsigned i) const {
  if(i < 4)return _arcZ[i];
  std::cerr << "Error!! Please stop!!.....arcZ of TMLink!!! in TrkReco." << std::endl;
  return _arcZ[0];
}

inline
const HepPoint3D & 
TMLink::arcZ(const HepPoint3D &az, const unsigned i){
  if(i < 4)return _arcZ[i] = az;
  std::cerr << "Error!! Please stop!!.....arcZ of TMLink!!! in TrkReco." << std::endl;
  return _arcZ[0];
}

//inline
//void
//TMLink::setZphiBeforeCathode(float a) {
//	_ZphiBeforeCathode = a;
//}

inline
const unsigned 
TMLink::fit2D(const unsigned &f) {
  return _fit2D = f;
}

inline
unsigned 
TMLink::fit2D(void) {
  return _fit2D;
}

inline
float
TMLink::drift(unsigned a) const {
    return _drift[a];
}

inline
float
TMLink::drift(float b, unsigned a) {
    return _drift[a] = b;
}

inline
double
TMLink::cDrift(unsigned a) const {
    return _cDrift[a];
}
                                                                                                                                               
inline
double
TMLink::cDrift(double b, unsigned a) {
    return _cDrift[a] = b;
}

inline
float
TMLink::dDrift(unsigned a) const {
    return _dDrift[a];
}

inline
float
TMLink::dDrift(float b, unsigned a) {
    return _dDrift[a] = b;
}

inline
float
TMLink::drift(void) const {
    return (_drift[0] + _drift[1]) / 2.;
}
                                                                                                                                               
inline
double
TMLink::cDrift(void) const {
    return (_cDrift[0] + _cDrift[1]) / 2.;
}

inline
float
TMLink::dDrift(void) const {
    return (_dDrift[0] + _dDrift[1]) / 2.;
}

inline
unsigned
TMLink::tsfTag(void) const {
	return _tsfTag;
}

inline
unsigned
TMLink::tsfTag(unsigned a) {
        return _tsfTag = a;
}

inline
double
TMLink::tof(void) const {
  return _tof;
}

inline
double
TMLink::tof(double a) {
  return _tof = a;
}

inline
double
TMLink::getDriftTime(void) {
  return _drifttime;
}

inline
void
TMLink::setDriftTime(double time){
_drifttime = time;

 }
#endif

#undef inline

#endif /* TMLink_FLAG_ */
