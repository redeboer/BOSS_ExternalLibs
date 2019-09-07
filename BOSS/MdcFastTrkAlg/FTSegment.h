// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTSegment
// 
// Description:  segment class for MdcFastTrkAlg
//

#ifndef FTSegment_FLAG_
#define FTSegment_FLAG_

#include "MdcFastTrkAlg/FTList.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/FTLayer.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"
#include <iostream> 

class FTFinder;
class FTWire;
class FTLayer;
class FTSuperLayer;
class FTTrack;
class FTSegment{
public:

  /// constructors
  FTSegment(FTSuperLayer * super, FTList<FTWire *> & hits);

  /// destructors
  ~FTSegment();

public: // public member functions
  /// examine segment
  int examine(void);

#ifndef OnlineMode
  /// printout of the wireHits info.
  /// added by X.-R. Lu
  void printout(void);
#endif

  /// update information for axial segment
  void update(void);

  /// update information for stereo segment
  int update3D(const FTTrack * track);

  /// link stereo segments by tanLambda
  void linkStereoSegments(void);

  /// connect short segments
  void connect_outer(const FTList<FTWire *> &, const FTList<FTWire *> &);
  void connect_inner(const FTList<FTWire *> &, const FTList<FTWire *> &);

  /// connect single hit
  void connect_outer(const FTWire *);

  /// connect single hit
  void connect_inner(const FTWire *);

public: // Selectors
  /// returns wire-hit list
  FTList<FTWire *>& wireHits(void) const;

  /// returns super-layer
  FTSuperLayer & superLayer(void) const;

  /// returns innerBoundHits
  FTList<FTWire *> & innerBoundHits(void) const;

  /// returns outerBoundHits
  FTList<FTWire *> & outerBoundHits(void)  const;

  /// returns x of outgoing position
  float outgoingX(void) const;

  /// returns y of outgoing position
  float outgoingY(void) const;

  /// returns x of incoming position
  float incomingX(void) const;

  /// returns y of incoming position
  float incomingY(void) const;

  /// returns phi of incoming position
  float incomingPhi(void) const;

  /// returns phi of outgoing position
  float outgoingPhi(void) const;

  /// returns the sin(dphi)/MOD(r_out-r_in)
  float attr_const(void) const;
  
  /// returns track
  FTTrack * track(void) const;

  /// returns kappa(axial)
  float kappa(void) const;

  /// returns r from origin
  float r(void) const;

  /// returns s for matched 2D track
  float s(void) const;

  /// returns z for matched 2D track
  float z(void) const;

public: // Modifiers
  /// set track
  FTTrack * track(FTTrack *);
  static MdcParameter * param;

private: // private data members
  IMagneticFieldSvc* m_pmgnIMF;
  FTList<FTWire *>& _wireHits; 
  FTSuperLayer & _superLayer;
  FTList<FTWire *> & _innerBoundHits;
  FTList<FTWire *> & _outerBoundHits;
  FTList<float> * _sList;
  union{
    float _kappa;
    FTList<float> * _zList;
  };
  union{
    FTTrack * _track;
    FTList<FTTrack *> * _trackList;
  };
  float _r;
  float _outgoing_x;
  float _outgoing_y;
  float _incoming_x;
  float _incoming_y;
};


inline
FTSegment::FTSegment(FTSuperLayer * super, FTList<FTWire *> & hits)
  : _wireHits(hits),
    _superLayer(*super),
    _innerBoundHits(*(new FTList<FTWire *>(3))),
    _outerBoundHits(*(new FTList<FTWire *>(3))),
    _sList(NULL),
    _zList(NULL),
    _track(NULL)
{
  StatusCode scmgn = Gaudi::svcLocator()->service("MagneticFieldSvc",m_pmgnIMF);
  if(scmgn!=StatusCode::SUCCESS) {
    std::cout<< "Unable to open Magnetic field service"<<std::endl;
  }
}

inline
FTSegment::~FTSegment()
{
  delete &_wireHits;
  delete &_innerBoundHits;
  delete &_outerBoundHits;
  if (_sList){
    delete _sList;
    delete _zList;
    delete _trackList;
  }
}

inline
void
FTSegment::connect_outer(const FTList<FTWire *> & outerHits, 
			 const FTList<FTWire *> & outerBound)
{
  _wireHits.append(outerHits);
  _outerBoundHits.clear();
  _outerBoundHits.append(outerBound);
}

inline
void
FTSegment::connect_inner(const FTList<FTWire *> & innerHits, 
                         const FTList<FTWire *> & innerBound)
{
  _wireHits.append(innerHits);
  _innerBoundHits.clear();
  _innerBoundHits.append(innerBound);
}

inline
void
FTSegment::connect_outer(const FTWire * h)
{
  _wireHits.append((FTWire *)h);
  _outerBoundHits.clear();
  _outerBoundHits.append((FTWire *)h);
}

inline
void
FTSegment::connect_inner(const FTWire * h)
{
  _wireHits.append((FTWire *)h);
  _innerBoundHits.clear();
  _innerBoundHits.append((FTWire *)h);
}

inline
FTSuperLayer & 
FTSegment::superLayer(void) const
{
  return _superLayer;
}

inline
FTList<FTWire *>& 
FTSegment::wireHits(void) const
{
  return _wireHits;
}

inline
FTList<FTWire *>& 
FTSegment::innerBoundHits(void) const
{
  return _innerBoundHits;
}

inline
FTList<FTWire *>& 
FTSegment::outerBoundHits(void) const
{
  return _outerBoundHits;
}

inline
float
FTSegment::outgoingX(void) const
{
  return _outgoing_x;
}

inline
float
FTSegment::outgoingY(void) const
{
  return _outgoing_y;
}

inline
float
FTSegment::incomingX(void) const
{
  return _incoming_x;
}

inline
float
FTSegment::incomingY(void) const
{
  return _incoming_y;
}

inline
float
FTSegment::incomingPhi(void) const
{
  if (_innerBoundHits.length() ^ 1) {
    float phi_1st = _innerBoundHits[0]->phi();
    float phi_2nd = _innerBoundHits[1]->phi();
    float phi = 0.5 * (phi_1st + phi_2nd);
    if (fabs(phi_1st - phi_2nd) > M_PI) {
      return (phi>M_PI) ? (phi-M_PI) : (phi+M_PI);
    }
    return phi;
  }
  return _innerBoundHits[0]->phi();
}

inline
float
FTSegment::outgoingPhi(void) const
{
  if (_outerBoundHits.length() ^ 1) {
    float phi_1st = _outerBoundHits[0]->phi();
    float phi_2nd = _outerBoundHits[1]->phi();
    float phi = 0.5 * (phi_1st + phi_2nd);
    if (fabs(phi_1st - phi_2nd) > M_PI) {
      return (phi>M_PI) ? (phi-M_PI) : (phi+M_PI);
    }
    return phi;
  }
  return _outerBoundHits[0]->phi();
}

inline
float
FTSegment::attr_const(void) const
{
  float x_len = _outgoing_x - _incoming_x;
  float y_len = _outgoing_y - _incoming_y;
  float phi_l = outgoingPhi() - incomingPhi();
  return sin(phi_l) / sqrt(x_len*x_len + y_len*y_len);
}

inline
float
FTSegment::r(void) const
{
  return _r;
}

inline
float
FTSegment::kappa(void) const
{
  return _kappa;
}

inline
float
FTSegment::s(void) const
{
  return _sList->first();
}

inline
float
FTSegment::z(void) const
{
  return _zList->first();
}

inline
FTTrack *
FTSegment::track(void) const
{
  return _track;
}

inline
FTTrack *
FTSegment::track(FTTrack * src)
{
  return _track = src;
}


#endif /* FTSegment_FLAG_ */

