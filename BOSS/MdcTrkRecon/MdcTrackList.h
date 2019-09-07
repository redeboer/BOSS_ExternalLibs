//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackList.h,v 1.11 2010/07/19 06:41:49 zhangy Exp $
//
// Description:
//     defines list of MdcTracks, and holds most MdcTrkRecon track-finding fcns
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s):
// 	Steve Schaffner
// 	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//
//------------------------------------------------------------------------
#ifndef MDCTRACKLIST_H
#define MDCTRACKLIST_H
#include "MdcTrkRecon/MdcTrackListBase.h"
//#include "GaudiKernel/NTuple.h"//yzhang hist

class MdcSegList;
class MdcTrack;
class MdcDetector;
class MdcSegData;
class MdcSegGrouper;
class MdcHitMap;
class TrkContext;
//class IHistogram1D;//yzhang hist

// Class definition//
class MdcTrackList : public MdcTrackListBase  {

public:
  MdcTrackList(const MdcTrackParams &tkPar);
  ~MdcTrackList();
  
  //return success:
  int createFromSegs(MdcSegList *segs, const MdcHitMap*, const MdcDetector*, 
                      TrkContext&, double bunchTime);
  int finishCircle(MdcTrack& track, const MdcHitMap*, const MdcDetector*);
  //int addHits(MdcTrack& track, const MdcHitMap*, const MdcDetector*);
  int finishHelix(MdcTrack& track, const MdcHitMap*, const MdcDetector*);
  int pickHits(MdcTrack*, const MdcHitMap*, const MdcDetector*, 
	       bool pickAmb = true);

  void dumpSeed(const MdcSeg * seed, bool goodOnly);
  void dumpCircle(const MdcTrack*); 
  void dumpAxFill(const MdcTrack*);
  void dumpAxCombine(const MdcTrack*);
  void dumpD0(const TrkExchangePar&);
  void dumpStFill();
  void dumpStCombine(const MdcTrack*);
  void dumpHelix(const MdcTrack*); 
  void dropMultiHotInLayer(const MdcTrack* tk);

  //double Xh(double d0, double phi0, double omega, double xc, double x0, double l)const;
  //double Yh(double d0, double phi0, double omega, double yc, double y0, double l)const;
private:
  MdcTrackList&   operator= (const MdcTrackList&);
  MdcTrackList(const MdcTrackList &);
  int m_debug;
//yzhang hist
/*  NTuple::Tuple*  m_tuple1;
  NTuple::Item<double>             m_d0;
  NTuple::Item<long>               m_nHitCir;
  NTuple::Item<double>             m_delPtCir;
  NTuple::Item<double>             m_lwPhi; 
*/
//zhangy hist

};
#endif
