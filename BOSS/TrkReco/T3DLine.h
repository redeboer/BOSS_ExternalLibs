//-----------------------------------------------------------------------------
// $Id: T3DLine.h,v 1.11 2010/03/31 09:58:59 liucy Exp $
//-----------------------------------------------------------------------------
// Filename : T3DLine.h
// Section  : Tracking
// Owner    : Kenji Inami
// Email    : inami@bmail.kek.jp
//-----------------------------------------------------------------------------
// Description : A class to represent a 3D line in tracking
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif
#ifndef T3DLine_FLAG_
#define T3DLine_FLAG_

#define Line3D	32
// This must be writen in TTrackBase.h

#include <string>

#define HEP_SHORT_NAMES
#ifndef CLHEP_POINT3D_H
#include "CLHEP/Geometry/Point3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef CLHEP_VECTOR3D_H
#include "CLHEP/Geometry/Vector3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif

//#include "helix/Helix.h"
#include "TrackUtil/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrkReco/TTrackBase.h"

class T3DLineFitter;
class TTrack;
class TMLink;

/// A class to represent a track in tracking.
class T3DLine : public TTrackBase {

public:
  /// Constructors
  T3DLine();
  T3DLine(const TTrack&);
  T3DLine(const T3DLine&);

  /// Destructor
  virtual ~T3DLine();

public:// Extractors
  /// Track parameters
  double dr(void) const;
  double phi0(void) const;
  double dz(void) const;
  double tanl(void) const;

  double cosPhi0(void) const;
  double sinPhi0(void) const;

  Vector a(void) const;		//dr,phi0,dz,tanl

  /// returns error matrix
  const SymMatrix& Ea(void) const;

  /// pivot position
  const HepPoint3D & pivot(void) const;

  /// approximated helix class
  Helix helix(void) const;

  /// returns NDF
  unsigned ndf(void) const;

  /// returns chi2.
  double chi2(void) const;

  /// returns reduced-chi2
  double reducedchi2(void) const;

  /// returns object type
  unsigned objectType(void) const;

public:// Executors

public:// Utilities
  /// returns position on 3D line
  HepPoint3D x(double) const;

  /// returns 3D line component    x(t)=x0 + t * k
  HepPoint3D x0(void) const;
//zsl  Vector3 k(void) const; 
  HepVector3D k(void) const;

  /// calculates the closest approach to a wire in real space. Results are stored in TMLink. Return value is negative if error happened.
  int approach(TMLink &, bool sagCorrection = true) const;

  /// caluculate closest points between a line and this track
  int approach_line(const HepPoint3D&,const HepVector3D&,
		    HepPoint3D& onLine,HepPoint3D& onTrack) const;

  /// caluculate closest point between a point and this track
  int approach_point(const HepPoint3D&,HepPoint3D& onTrack) const;

public:// Modifiers
  /// set new pivot
  const HepPoint3D & pivot(const HepPoint3D&);

  /// set track parameters,pivot
  void set(const HepPoint3D&,double t_dr,double t_phi0,double t_dz,double t_tanl);

  /// set track parameters
  Vector a(const Vector&);

  /// set error matrix
  const SymMatrix& Ea(const SymMatrix&);

private:
  HepPoint3D _pivot;
  // Updated when fitted
  double _dr;
  double _phi0;
  double _dz;
  double _tanl;

  SymMatrix _Ea;

  double _chi2;
  unsigned _ndf;

  //Cashe
  double _cos_phi0;
  double _sin_phi0;

  static const T3DLineFitter _fitter;

  friend class T3DLineFitter;
};

inline unsigned T3DLine::objectType(void) const{
  return Line3D;
}

#endif /* T3DLine_FLAG_ */
