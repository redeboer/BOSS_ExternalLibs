// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTTrack
// 
// Description:  track class for MdcFastTrkAlg
//

#ifndef FTTrack_FLAG_
#define FTTrack_FLAG_

#include"TrackUtil/Helix.h"
#include"TrackUtil/Lpav.h"
#include"TrackUtil/zav.h"

#include "MdcFastTrkAlg/FTList.h"
#include "MdcFastTrkAlg/FTSegment.h"
#include "MdcFastTrkAlg/MdcParameter.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class FTWire;
class FTTrack{
public:
  /// constructor
  FTTrack(FTList<FTSegment *> & axial_segments, float kappa, float chi2_kappa);

  /// destructor
  ~FTTrack();

public: // public member functions
  /// do r-phi circle fit
  int r_phiFit(void);

  /// do r-phi refit
  int  r_phiReFit(float vx, float xy, int vtx_flag);
  
  // cut the largest residual hit when do r-phi fit 
  int  r_phi2Fit(float vx, float xy, int vtx_flag);
  int  r_phi3Fit(int l, float vx, float xy, int vtx_flag);
  int  r_phi4Fit(float vx, float xy, int vtx_flag);

  /// do s-z linear fit
  int s_zFit(void);

  /// update s and z information for linking
  void updateSZ(void);

  /// link stereo segments by tanLambda
  int linkStereoSegments(void);

public: // Selectors

  /// returns helix parameters
  Helix * helix(void) const;

  /// returns lpav
  const Lpav & lpav(void) const;

  /// returns zav
  const zav & Zav(void) const;

  /// returns axial segments
  FTList<FTSegment *>& axial_segments(void) const;

  /// returns stereo_segments
  FTList<FTSegment *>& stereo_segments(void) const;

  /// returns kappa at linking
  float kappa_tmp(void) const;

  /// returns sigmaKappa at linking
  float chi2_kappa_tmp(void) const;

  /// returns difference between z and estimated z 
  /// at stereo segment linking if its valid
  float d_z(float s, float z) const;

#ifndef OnlineMode
  /// printout _tracks infomation
  /// added by X.-R. Lu
  void printout();
#endif

public: // Modifiers
  /// add z for culculation of tanLambda
  float SigmaZ(float z);

  /// add s for culculation of tanLambda
  float SigmaS(float s);

  /// add s for culculation of dz, tanLambda
  float SigmaSS(float ss);

  /// add s for culculation of dz, tanLambda
  float SigmaSZ(float sz);

  /// append stereo segment to the stereo segment list
  void append_stereo(FTSegment *, float s, float z);

  /// append stereo segment to the cache
  void append_stereo_cache(FTSegment *);

  /// calculate the wire hits number
  int get_nhits(void);

public: // static data members
   float minPt;
   float minDr;
   //const HepPoint3D pivot;
  // int evtTiming;
   float xtCoEff;
   float Testime;

public: 
   void setFTFinder(FTFinder*);

private:
  FTFinder* m_ftFinder;
  IMagneticFieldSvc* m_pmgnIMF;
  
public: // private data members
  Lpav * _la;
  zav * _za;
  Helix * _helix;
  FTList<FTSegment *> & _axial_segments;
  FTList<FTSegment *> * _stereo_segments;
  FTList<FTSegment *> * _stereo_segments_cache;
  FTList<FTList<FTSegment *> *> * _stereo_segments_by_superLayer;
  static MdcParameter * param;

  union{
    float _kappa;
    float _SigmaS;
  };
  union{
    float _chi2_kappa;
    float _SigmaSS;
  };
  float _SigmaZ;
  float _SigmaSZ;
};


//----------------------------------------------
#ifdef FTTrack_NO_INLINE
#define inline
#else
#undef inline
#define FTTrack_INLINE_DEFINE_HERE
#endif

#ifdef FTTrack_INLINE_DEFINE_HERE

inline
FTTrack::FTTrack(FTList<FTSegment *> & axial_segments,
		 float kappa, float chi2_kappa)
  : _la(NULL),
    _za(NULL),
    _helix(NULL),
    _axial_segments(axial_segments),
    _stereo_segments(NULL),
    _stereo_segments_cache(NULL),
    _stereo_segments_by_superLayer(NULL),
    //_wireHits(*(new FTList<FTWire *>(50))),
    _kappa(kappa),
    _chi2_kappa(chi2_kappa)
{
  StatusCode scmgn = Gaudi::svcLocator()->service("MagneticFieldSvc",m_pmgnIMF);
  if(scmgn!=StatusCode::SUCCESS) {
    std::cout<< "Unable to open Magnetic field service"<<std::endl;
  }
}

inline
FTTrack::~FTTrack()
{
  delete &_axial_segments;
  delete _la;
  delete _za;
  delete _stereo_segments;
  delete _helix;
}

inline
FTList<FTSegment *>& 
FTTrack::axial_segments(void) const
{
  return _axial_segments;
}

inline
FTList<FTSegment *>& 
FTTrack::stereo_segments(void) const
{
  return *_stereo_segments;
}

inline
void
FTTrack::append_stereo_cache(FTSegment * src)
{
  _stereo_segments_cache->append(src);
}

inline
void
FTTrack::append_stereo(FTSegment * src, float s = 0, float z = 0)
{
  _stereo_segments->append(src);
  _SigmaS += s;
  _SigmaZ += z;
  _SigmaSZ += s*z;
  _SigmaSS += s*s;
}

inline
float
FTTrack::kappa_tmp(void) const
{
  return _kappa;
}

inline
float
FTTrack::chi2_kappa_tmp(void) const
{
  return _chi2_kappa;
}

inline
const Lpav &
FTTrack::lpav(void) const
{
  return *_la;
}

inline
const zav &
FTTrack::Zav(void) const
{
  return *_za;
}

inline
Helix *
FTTrack::helix(void) const
{
  return _helix;
}

inline 
float 
FTTrack::SigmaZ(float z)
{
  return _SigmaZ += z;
}

inline 
float
FTTrack::SigmaS(float s)
{
  return _SigmaS += s;
}

inline
float
FTTrack::SigmaSZ(float sz)
{
  return _SigmaSZ += sz;
}

inline 
float
FTTrack::SigmaSS(float ss)
{
  return _SigmaSS += ss;
}

inline 
float
FTTrack::d_z(float s, float z) const
{
  int n = _stereo_segments->length();
  if (!n) return 0;
  float Dz = (n==1)
    ? z - s*_SigmaZ/_SigmaS
    : z - (s*(_SigmaSZ-_SigmaS*_SigmaZ)+_SigmaSS*_SigmaZ-_SigmaS*_SigmaSZ)
      / (_SigmaSS - _SigmaS*_SigmaS);
  return (Dz < 10*(7-n)) ? Dz : 9999.;
}


inline
void
FTTrack::updateSZ(void)
{
  int n = _stereo_segments_cache->length();
  if (n==1){
    FTSegment * s = _stereo_segments_cache->first();
    float s_tmp = s->s();
    float z_tmp = s->z();
    if (d_z(s_tmp,z_tmp) < 9998.){
      _SigmaS += s_tmp;
      _SigmaZ += z_tmp;
      _SigmaSS += s_tmp*s_tmp;
      _SigmaSZ += s_tmp*z_tmp;
      _stereo_segments->append(s);
    }
    _stereo_segments_cache->clear();
  }else if(n){
    _stereo_segments_by_superLayer->append(_stereo_segments_cache);
    _stereo_segments_cache = new FTList<FTSegment *>(3);
  }
}

inline
void
FTTrack::setFTFinder(FTFinder * finder)
{
  m_ftFinder = finder;
}

#endif

#undef inline

#endif /* FTTrack_FLAG_ */

