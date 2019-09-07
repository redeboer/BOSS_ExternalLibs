//-----------------------------------------------------------------------------
// $Id: TRungeFitter.h,v 1.16 2012/05/28 05:16:29 maoh Exp $
//-----------------------------------------------------------------------------
// Filename : TRungeFitter.h
// Section  : Tracking
// Owner    : Kenji Inami
// Email    : inami@bmail.kek.jp
//-----------------------------------------------------------------------------
// Description : A class to fit a TTrackBase object to a Runge Kutta track
//               See http://bsunsrv1.kek.jp/~yiwasaki/tracking/
//-----------------------------------------------------------------------------

#ifndef TRUNGEFITTER_FLAG_
#define TRUNGEFITTER_FLAG_

//#ifdef TRKRECO_DEBUG_DETAIL
//#define TRKRECO_DEBUG
//#endif

#include <string>

#define HEP_SHORT_NAMES
//#include "helix/Helix.h"
//#include "TrkReco/Helix.h"
#include "TrackUtil/Helix.h"

#include "TrkReco/TMFitter.h"
#include "TrkReco/RkFitCylinder.h"
#include "TrkReco/RkFitMaterial.h"
class TMLink;
class TRunge;
typedef HepGeom::Transform3D  HepTransform3D;
/// A class to fit a TTrackBase object to a 3D line.
class TRungeFitter : public TMFitter {

public:
  /// Constructor.
  TRungeFitter(const std::string & name);
  TRungeFitter(const std::string & name,bool m_sag,int m_prop,bool m_tof);
 
  /// Destructor
  virtual ~TRungeFitter();
  
public:// Selectors
  /// dumps debug information.
  void dump(const std::string & message = std::string(""),
	    const std::string & prefix = std::string("")) const;
  
public:// Modifiers
  virtual int fit(TTrackBase &) const;
  virtual int fit(TTrackBase &, float t0Offset,int layer) const;
  void innerwall(TRunge& trk, int l_mass,double y[6])const;
  void sag(bool);
  const RkFitMaterial getMaterial(int i)const;
  void propagation(int);
  void tof(bool);
  std::vector<RkFitCylinder> _BesBeamPipeWalls;
  std::vector<RkFitMaterial> _BesBeamPipeMaterials;  
  void setBesFromGdml(void);
private:
  IMagneticFieldSvc* m_pmgnIMF;
  bool _sag;
  int _propagation;
  bool _tof;
  
};

//-----------------------------------------------------------------------------
#ifdef TRKRECO_NO_INLINE
#define inline
#else
#undef inline
#define TRUNGEFITTER_INLINE_DEFINE_HERE
#endif

#ifdef TRUNGEFITTER_INLINE_DEFINE_HERE

#endif

#undef inline

#endif /* TRUNGEFITTER_FLAG_ */
