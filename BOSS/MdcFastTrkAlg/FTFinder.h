// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTFinder
// 
// Description:  fast track finder class for MdcFastTrkAlg
//
//

#ifndef FTFinder_FLAG_
#define FTFinder_FLAG_


#include <iostream>

#include "MdcFastTrkAlg/FTList.h"
#include "MdcFastTrkAlg/FTTrack.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/FTLayer.h"
#include "MdcFastTrkAlg/FTSuperLayer.h"
#include "MdcFastTrkAlg/MdcParameter.h"
#include "MdcFastTrkAlg/MdcFastTrkAlg.h"
#include "HepPDT/ParticleDataTable.hh"
#include "CLHEP/Vector/ThreeVector.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "MagneticField/MagneticFieldSvc.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using namespace CLHEP;

class CLHEP::Hep3Vector;
//class MdcParameter;
class FTSegment;
class BesEvent;
class StatusCode;

class FTFinder{

public:
  /// returns FTFinder pointer 
  //static FTFinder * GetPointer(void);

  void setAlgorithmPointer(Algorithm*);
public:
  /// Constructors and destructor
  FTFinder();

  /// initializer(creates geometry)
  void init();

  /// terminator
  void term();

  /// begin run function(reads constants)
  void begin_run();

  /// track finder core
  void event();

public: // Selectors

  /// returns superlayer
  FTSuperLayer * superLayer(int id) const;

  /// returns track list
  FTList<FTTrack *> & tracks(void) const;

  /// returns event primary vertex
  CLHEP::Hep3Vector vertex(void) const;

  /// returns wire ID for given FTWire object
  int getWireId(FTWire *) const;

  /// convert t to x
  float t2x(const FTLayer& l, const float t) const;

  /// convert x to t
  float x2t(const FTLayer& l, const float x) const;

  /// set bunchtime for MC events
	void  setBunchtime(double bunchtime){_bunchtime=bunchtime;};
  /// set crude mdc time calibconst (~200 ns) for t0 calculation
	void  setT0cal(double t0cal) {_t0cal=t0cal;};

private: // private member functions
Algorithm * m_algorithm;

  /// clear object
  void clear(void);

  /// unpack RAWCDC and create wire-hit
  int updateMdc(void);

  ///get event start time from segment linear fit
  int  getTestime(void);

  /// create track list
  void mkTrackList(void);

  /// create 3D track list
  void mkTrack3D(void);

  /// finds event primary vertex
  int VertexFit(int z_flag);

  /// finds event primary vertex from 2D tracks
  int VertexFit2D();

  /// find vertex closest to IP
  int findBestVertex(void);

  /// corrects event timing after 2nd r-phi fit and returns event timing
  int CorrectEvtTiming(void);

  /// link axial segments to make track
  FTTrack * linkAxialSegments(FTSegment ** inner, FTSegment ** outer);

  // link 2,3,4 axial segments
  void  linkAxialSuperLayer234(FTList<FTSegment *> & inner_segments);

  // link 9,10 axial segments
  void  linkAxialSuperLayer910(FTList<FTSegment *> & outer_segments);

  void  linkAxialSegments_step(FTList<FTSegment *>& InnerSegments,
                                 FTList<FTSegment *>& OuterSegments,
				 FTList<FTSegment *>& Connected,
				 float maxDphi, float maxChi2);

#ifndef OnlineMode
  /// make Mdst_charged/trk/trk_fit table from reconstruted tracks
  void makeMdst(void);
#endif

  /// output tracking results into TDS
  //   by wangdy
  StatusCode makeTds(void);

  //register T0 into TDS by max
  StatusCode registT0(void);

  public:  // public data menbers
  /* int findEventVertex;
  int evtTimeCorr;
  double minPt;
  double minDr;
  float tOffSet;
  float xtCoEff;
  int doIt;
#ifndef OnlineMode
  bool mkMdst;
#endif
  bool mkTds;
  */ 
  int   eventNo;
  int   runNo;
  int   expflag;	
  float t0Estime;
  float tOffSet;
  float evtTiming;
  const HepPoint3D pivot;
  float Testime;
  int   i_rPhiFit; 
  int   tEstFlag;
  FTList<float> tEstime[10];

private: // private data members
  //static FTFinder * _tFinder;
  FTWire * _wire;
  FTLayer * _layer;
  FTSuperLayer * _superLayer;
  FTList<FTTrack *> & _tracks;
  FTList<FTSegment *> * _linkedSegments;
  FTList<FTSegment *> _axialSegCollect;
  double _vx;
  double _vy;
  double _vz;
  int _ExpNo;
  int _RunNo;
	double  _bunchtime;
	double  _t0cal;
  int m_total_trk;
  HepPDT::ParticleDataTable* m_particleTable;
  IRawDataProviderSvc* m_rawDataProviderSvc;
  IMagneticFieldSvc* m_pmgnIMF;

  static MdcParameter* param;
  int _widbase[43];
};

inline
FTSuperLayer *
FTFinder::superLayer(int id) const{
  return _superLayer + id;
}

inline
FTList<FTTrack *> & 
FTFinder::tracks(void) const{
  return _tracks;
}

inline
int
FTFinder::getWireId(FTWire * w) const {
  return ((long)w - (long)_wire)/sizeof(FTWire);
}

inline
float
FTFinder::x2t(const FTLayer & l, const float x) const
{
  return (x*x) / (param->_xtCoEff * param->_xtCoEff *l.csize());
}

inline
float
FTFinder::t2x(const FTLayer & l, const float t) const
{
  float x = (t>0.0f) ?((param->_xtCoEff))*sqrt(t*l.csize()) : 0.0f;
  if ( x > 0.47f*l.csize() ){
    x = 0.0004f*t + 0.47f*l.csize()*(1.0f-0.0004f*0.47f/((param->_xtCoEff)*(param->_xtCoEff)));
  }

  return x;
}

inline
void 
FTFinder::setAlgorithmPointer(Algorithm* alg){
  m_algorithm = alg;
}
#endif /* FTFinder_FLAG_ */
