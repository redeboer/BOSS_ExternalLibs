//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackListBase.h,v 1.16 2017/04/10 07:48:39 zhangy Exp $
//
// Description:
//     Base class for track lists, which are where tracks found in MdcTrkRecon 
//     are temporarily stored, and where much of the finding code is.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): 
// 	Steve Schaffner
// 	Zhang Yao(zhangyao@ihep.ac.cn)  Migrate to BESIII
//
//------------------------------------------------------------------------
#ifndef MDCTRACKLISTBASE_H
#define MDCTRACKLISTBASE_H
#include "MdcTrkRecon/MdcTrackParams.h"
#include "CLHEP/Alist/AList.h" 
#include "TrkBase/TrkRecoTrk.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

class MdcSegList;
class MdcTrack;
class MdcDetector;
class MdcSegGrouper;
class MdcHitMap;
class TrkContext;

// Class definition//
class MdcTrackListBase : public HepAList<MdcTrack>  {

public:
  MdcTrackListBase(const MdcTrackParams &tkPar);
  virtual ~MdcTrackListBase();
  
  int nTrack() const {return length();}
  void setPlot(int plotFlag) { tkParam.lPlot = plotFlag;};
  void newParams(const MdcTrackParams &tkPar);
  void plot() const;
  void store(RecMdcTrackCol* ,RecMdcHitCol*);

  virtual int createFromSegs(MdcSegList *, const MdcHitMap*,
                             const MdcDetector*, TrkContext&,
			     double bunchTime ) = 0;

  int arbitrateHits();   // return # of tracks deleted
  void dropMultiHotInLayer(const MdcTrack* tk);

  //Remove and delete:
  void remove( MdcTrack *atrack );
#ifdef MDCPATREC_RESLAYER
  void setResLayer(int layer){m_resLayer=layer;}
#endif 
  void setD0Cut(double d0Cut){m_d0Cut = d0Cut;}//yzhang add 
  void setZ0Cut(double z0Cut){m_z0Cut = z0Cut;}//yzhang add 2010-05-21 
  void setPtCut(double ptCut){m_ptCut = ptCut;}//yzhang add 2009-10-27
  static double m_d0Cut;//yzhang add
  static double m_z0Cut;//yzhang add 2010-05-21 
  static double m_ptCut;//yzhang add 2009-10-27
  void setNoInner(bool noInnerFlag){m_noInner= noInnerFlag;}

private:
  MdcTrackListBase&   operator= (const MdcTrackListBase&);
  MdcTrackListBase(const MdcTrackListBase &);
  void transferTrack();
#ifdef MDCPATREC_RESLAYER
  int m_resLayer;
#endif 
protected:
  MdcTrackParams tkParam;
  bool m_noInner;
};


#endif







