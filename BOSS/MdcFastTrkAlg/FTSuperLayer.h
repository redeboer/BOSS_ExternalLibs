// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTSuperLayer
// 
// Description: super-layer class for MdcFastTrkAlg
//
// Usage:
//    <usage>
//
// Author:      KAKUNO Hidekazu
// Created:     Wed Dec  1 20:20:53 JST 1999
// $Id: FTSuperLayer.h,v 1.6 2011/02/18 04:44:03 maqm Exp $
//
// Revision history
//
// $Log: FTSuperLayer.h,v $
// Revision 1.6  2011/02/18 04:44:03  maqm
// slc5-x64-gcc43
//
// Revision 1.5  2008/12/09 04:44:33  lvxr
// big update, see ChangeLog
//
// Revision 1.4  2006/09/26 12:23:17  max
// see the ChangeLog
//
// Revision 1.3  2006/08/16 01:22:09  zoujh
// See ChangeLog
//
// Revision 1.2  2006/06/23 08:38:53  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2004/11/17 14:35:35  zhangxm
// first version
//
// Revision 1.7  2002/02/26 21:52:20  kakuno
// removal of unused variables and warning suppression
//
// Revision 1.6  2002/01/19 02:06:36  katayama
// gcc3 compatibility
//
// Revision 1.5  2000/02/25 12:09:33  kakuno
// MdcFastTrkAlg 1.5 release: modified to get better quality and to reduce CPU time
//
// Revision 1.4  2000/02/21 10:58:08  katayama
// compatibility with gcc 2.95.2
//
// Revision 1.3  1999/12/06 08:36:33  kakuno
// minor bug fixed
//
// Revision 1.2  1999/12/06 08:15:57  kakuno
// MdcFastTrkAlg 1.1 release: dz resolution improved, primary vertex finder added
//

#ifndef FTSuperLayer_FLAG_
#define FTSuperLayer_FLAG_

#include <iostream> 
#include "MdcFastTrkAlg/FTList.h"
#include "MdcFastTrkAlg/MdcParameter.h"

class FTWire;
class FTSegment;
class FTFinder;
class FTSuperLayer{
public:
  /// Constructors and destructor
  FTSuperLayer(const int firstWireID,const int NWire,
			     const int firstLayerID,const int NLayer,
			     const int superLayerID);
  ~FTSuperLayer();

public:
  /// clear object
  void clear(void);

  /// create segment lists
  void mkSegmentList(void);

  /// append wireHit to the list of hits
  void appendHit(FTWire *);

  /// append segments which are not used for tracks to the list for salvage
  void reAppendSalvage(void);

  /// calculate t0 and Chi2 from segment-fit in every superlayer 
  void reduce_noise(FTList<float> (&Estime)[10]);

public: // Selectors
  /// returns wirehit list
  FTList<FTWire *> & wireHits(void) const;

  /// returns segement list
  FTList<FTSegment *> & segments(void) const;

  /// returns number of wires
  const int nWire(void) const;

  /// returns number of layers
  const int nLayer(void) const;

  /// returns layer max ID
  const int layerMaxId(void) const;

  /// returns local max ID
  const int localMaxId(void) const;

  /// returns super-layer ID
  const int superLayerId(void) const;

  /// returns complecated segments
  FTList<FTSegment *> & complecated_segments(void);

  static MdcParameter * param;

  static const float _maxDphi[11];

private:
  /// clustering of wire-hits
  void clustering(void);

  /// conect short clusters to make segments
  void connect_short_segments(FTList<FTSegment *> & inner_short,
			      FTList<FTSegment *> & outer_short);

  /// connect single wireHit to make segments
  void connect_singleHit(FTList<FTSegment *> &);


private: //static data members
  //static const unsigned int _neighborsMask[6];

private: // private data members
  const int _superLayerId;
  const int _firstLayerId;
  const int _firstWireId;
  const int _Nlayer;
  const int _Nwire;
  FTList<FTSegment *> * const _complecated_segments;
  FTList<FTWire *>& _wireHits; 
  FTList<FTWire *>& _singleHits; 
  FTList<FTSegment *>& _segments; 
};


//----------------------------------------------
#ifdef FTSuperLayer_NO_INLINE
#define inline
#else
#undef inline
#define FTSuperLayer_INLINE_DEFINE_HERE
#endif

#ifdef FTSuperLayer_INLINE_DEFINE_HERE

inline
FTSuperLayer::FTSuperLayer(const int firstWireID,const int NWire,
			   const int firstLayerID,const int NLayer,
			   const int superLayerID)
  : _superLayerId(superLayerID),
    _firstLayerId(firstLayerID),
    _firstWireId(firstWireID),
    _Nlayer(NLayer),
    _Nwire(NWire),
    //_complecated_segments((superLayerID) ? NULL: new FTList<FTSegment *>(20)),
    _complecated_segments(new FTList<FTSegment *>(20)),
    _wireHits(*(new FTList<FTWire *>(500))),
    _singleHits(*(new FTList<FTWire *>(100))),
    _segments(*(new FTList<FTSegment *>(20)))
{
}

inline
FTSuperLayer::~FTSuperLayer()
{
  clear();
  delete &_wireHits;
  delete &_singleHits;
  delete &_segments;
  if (_complecated_segments) delete _complecated_segments;
}

inline
FTList<FTWire *> &
FTSuperLayer::wireHits(void) const{
  return _wireHits;
}

inline
const int
FTSuperLayer::nWire(void) const{
  return _Nwire;
}

inline
const int
FTSuperLayer::nLayer(void) const{
  return _Nlayer;
}

inline
const int
FTSuperLayer::localMaxId(void) const{
  return (_Nwire - 1);
}

inline
const int
FTSuperLayer::layerMaxId(void) const{
  return (_Nlayer - 1);
}

inline
FTList<FTSegment *> & 
FTSuperLayer::segments(void) const{
  return _segments;
}

inline
const int
FTSuperLayer::superLayerId(void) const{
  return _superLayerId;
}

inline
FTList<FTSegment *> &
FTSuperLayer::complecated_segments(void){
  return *_complecated_segments;
}

inline
void
FTSuperLayer::appendHit(FTWire * h){
  _wireHits.append(h);
}

#endif

#undef inline

#endif /* FTSuperLayer_FLAG_ */

