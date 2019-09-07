//-----------------------------------------------------------------------------
// $Id: T3DLineFitter.h,v 1.15 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : T3DLineFitter.h
// Section  : Tracking
// Owner    : Kenji Inami
// Email    : inami@bmail.kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a 3D line.
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef T3DLINEFITTER_FLAG_
#define T3DLINEFITTER_FLAG_

#ifdef TRKRECO_DEBUG_DETAIL
#ifndef TRKRECO_DEBUG
#define TRKRECO_DEBUG
#endif
#endif

#include <string>

#define HEP_SHORT_NAMES
#ifndef CLHEP_VECTOR3D_H
#include "CLHEP/Geometry/Vector3D.h"
#endif
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Vector3D<double> HepVector3D;
#endif

//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"
#include "TrkReco/TMFitter.h"

class TMLink;
class T3DLine;

/// A class to fit a TTrackBase object to a 3D line.
class T3DLineFitter : public TMFitter {

public:
  /// Constructor.
  T3DLineFitter(const std::string & name);
  T3DLineFitter(const std::string & name,bool m_sag,int m_prop,bool m_tof);
 
  /// Destructor
  virtual ~T3DLineFitter();
  
public:// Selectors
  /// dumps debug information.
  void dump(const std::string & message = std::string(""),
	    const std::string & prefix = std::string("")) const;
  
public:// Modifiers
  virtual int fit(TTrackBase &) const;
  virtual int fit(TTrackBase &, float t0Offset) const;

  void sag(bool);
  void propagation(int);
  void tof(bool);
  
private:
  /// calculates dXda. 'TMLink' and 'T3DLine' are inputs. Others are outputs.
  int dxda(const TMLink&,
	   const T3DLine&,
	   Vector & dxda,
	   Vector & dyda,
	   Vector & dzda,
	   HepVector3D & wireDirection) const;
  
  /// calculates drift distance and its error.
  void drift(const T3DLine &,
	     const TMLink &,
	     float t0Offset,
	     double & distance,
	     double & err) const;
  
private:
  bool _sag;
  int _propagation;
  bool _tof;
  
};

//-----------------------------------------------------------------------------

#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define T3DLINEFITTER_INLINE_DEFINE_HERE
#endif

#ifdef T3DLINEFITTER_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* T3DLINEFITTER_FLAG_ */
