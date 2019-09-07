// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTLayer
// 
// Description:  layer class for MdcFastTrkAlg
//
// Usage:
//    <usage>
//

#ifndef FTLayer_FLAG_
#define FTLayer_FLAG_

#include <cmath>

/* for M_PI */
//#include "CLHEP/config/CLHEP.h"

#include "MdcFastTrkAlg/FTSuperLayer.h"

class FTLayer{
public:
  /// constructor
  FTLayer(const float radius, const float stereoAngle, 
	  const float zf, const float zb, const float offset,
	  const int layerID, const int localLayerID, const int NWire, 
	  const FTSuperLayer & super);

  /// destructor
  ~FTLayer(){};

public: // Selectors
  /// returns local-layer ID
  const int localLayerId(void) const;

  /// returns layer ID
  const int layerId(void) const;

  /// returns the number of wire
  const int NWire(void) const;

  /// returns tangent of slant angle
  const float tanSlant(void) const;

  /// returns r form origin
  const float r(void) const;

  /// returns z of forward end-plate
  const float zf(void) const;

  /// returns z of backward end-plate
  const float zb(void) const;

  /// returns offset of numbering(local ID)
  const float offset(void) const;

  /// returns super-layer
  const FTSuperLayer & superLayer(void) const;

  /// returns limit of "d" for stereo layer
  const double limit(void) const;

  /// returns z for "d" in r-phi plane
  double z(const double d) const;

  /// returns cell size
  double csize(void) const;

private: // private data members
  const float _radius;
  const float _tanSlant;
  const float _zf;				// z of forward endplate
  const float _zb;				// z of backward endplate
  const int _layerId;
  const int _localLayerId;
  const float _offset;
  const int _NWire;
  const FTSuperLayer & _superLayer;
};

//----------------------------------------------
#ifdef FTLayer_NO_INLINE
#define inline
#else
#undef inline
#define FTLayer_INLINE_DEFINE_HERE
#endif

#ifdef FTLayer_INLINE_DEFINE_HERE

inline
FTLayer::FTLayer(const float radius, const float stereoAngle, 
		 const float zf, const float zb, const float offset,
		 const int layerID, const int localLayerID, const int NWire,
		 const FTSuperLayer & super)
  : _radius(radius),
    _tanSlant(1./std::tan(stereoAngle)),
    _zf(zf),
    _zb(zb),
    _layerId(layerID),
    _localLayerId(localLayerID),
    _NWire(NWire),
    //_offset((int)(2.0*offset)),
    _offset(offset),
    _superLayer(super)
{
}

inline
const int
FTLayer::layerId(void) const
{
  return _layerId;
}

inline
const int
FTLayer::localLayerId(void) const
{
  return _localLayerId;
}

inline
const int
FTLayer::NWire(void) const
{
  return _NWire;
}

inline
const float
FTLayer::tanSlant(void) const
{
  return _tanSlant;
}


inline
const float
FTLayer::r(void) const
{
  return _radius;
}

inline
const float
FTLayer::zf(void) const
{
  return _zf;
}

inline
const float
FTLayer::zb(void) const
{
  return _zb;
}

inline
const double 
FTLayer::limit(void) const
{
  return (double)(_zf-_zb)/_tanSlant;
}

inline  
double 
FTLayer::z(const double d) const
{
  return (double)_zb+d*_tanSlant;
}

inline
const float 
FTLayer::offset(void) const
{
  return _offset;
}

inline
const FTSuperLayer & 
FTLayer::superLayer(void) const
{
  return _superLayer;
}

inline
double
FTLayer::csize(void) const
{
  return 2*M_PI*_radius/NWire();
}

#endif

#undef inline

#endif /* FTLayer_FLAG_ */

