//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxTrackFinder.h,v 1.28 2017/08/18 06:51:52 zhangy Exp $
//
// Description:
//	Class MdcxTrackFinder. This is a simple example of a user module. It
//	just prints out each entrypoint (operation) as it is accessed.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	David R. Quarrie		Original Author
//	Zhang Yao(zhangyao@ihep.ac.cn)
//
// Copyright Information:
//	Copyright (C) 1994		Lawrence Berkeley Laboratory
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef MDCXTRACKFINDER_H
#define MDCXTRACKFINDER_H

//----------------------
// Base Class Headers --
//----------------------
#include "GaudiKernel/Algorithm.h"
#include "CLHEP/Alist/AList.h"
#include "GaudiKernel/NTuple.h"
#include "MdcData/MdcHit.h"
#include "MdcxReco/MdcxHits.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "MdcTrkRecon/MdcFlagHold.h"
#include "MagneticField/IMagneticFieldSvc.h"
#include "TrkFitter/TrkContextEv.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#ifdef MDCXTIMEDEBUG
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#endif

class MdcxHel;
class TrkHitList;
class BField;
class TrkRecoTrk;
class MdcxSeg;
class MdcxFittedHel;

//---------------------
//-- Class Interface --
//---------------------
 
class MdcxTrackFinder : public Algorithm {

//--------------------
// Instance Members --
//--------------------

public: 
  // Constructors
  MdcxTrackFinder(const std::string& name, ISvcLocator* pSvcLocator);

  // Destructor
  virtual ~MdcxTrackFinder();

  // Operations
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 
  StatusCode beginRun(); 

private:
  // temp var
  int		    	haveDigi[43][288];
  int	            	nTk;
  bool 			b_saveEvent;
  int	            	t_nTdsTk;
  int	            	t_nDigi;
  int	            	t_nSeg;
  int	            	t_nTkTot;
  int   	        t_nTkNum[20];

  double	    	m_bunchT0;
  double	    	m_timing;
  int		    	m_t0Stat;
  double            	m_t0Truth;
  int 		    	m_eventNo;   
  
  // debug and hist flag
  int		    	m_debug;   
  int		    	m_hist;   
  bool		    	m_mcHist;

  //for raw data input
  bool		    	m_dropUnmatch;
  bool		    	m_keepBadTdc;
  bool		    	m_dropHot;
  bool		    	m_keepUnmatch;
  uint32_t          	m_getDigiFlag;
  int		    	m_maxMdcDigi;
  int		    	m_minMdcDigi;
  bool		    	m_dropMultiHotInLayer;

  // fit type control
  bool	            	m_salvageTrk;
  bool		    	m_lineFit;
  //bool		    	m_cosmicFit;

  // fix of Hit
  bool		    	m_countPropTime;
  bool	            	m_doSag;

  // fit cuts
  double	    	m_cresol;
  std::vector<float> 	m_dropHitsSigma;
  std::vector<float> 	m_helixFitCut;   
  double 	    	m_dropTrkPt;
  double	    	m_addHitCut;
  double	    	m_minTrkProb;
  double	    	m_csmax4;
  double	    	m_csmax3;
  double	    	m_helixFitSigma;
  double	    	m_maxRcsInAddSeg;
  double	    	m_nSigAddHitTrk;
  double	    	m_maxProca;
  double	    	m_d0Cut;
  double	    	m_z0Cut;


  BField*	    	m_bfield;
  IMagneticFieldSvc*   	m_pIMF;
  const MdcCalibFunSvc* m_mdcCalibFunSvc; 
  RawDataProviderSvc* 	m_rawDataProviderSvc;
  std::string	    	m_pdtFile;
  HepAList<MdcHit>* 	m_junkHitList;
  MdcxHits          	m_mdcxHits;

  TrkContextEv* 	m_context;
  const MdcDetector* 	m_gm;
  MdcDigiVec mdcDigiVec;

  StatusCode FitMdcxTrack(HepAList<MdcxFittedHel>& trkList, 
      const HepAList<MdcxHit>& dchitlist, MdcHitCol* hitCol, 
      RecMdcTrackCol* trackList, RecMdcHitCol* hitList);
  void MdcxHitsToHots(MdcxHel& mdcxHelix, const HepAList<MdcxHit>& mdcxHits, 
      TrkHitList* m_trkHitList, MdcHitCol* hitCol);
  void MdcxHitsToHots(TrkHitList* m_trkHitList, HitRefVec& recMdcHits, HitRefVec& skipedHits); 

  void store(TrkRecoTrk* aTrack, RecMdcTrackCol*, RecMdcHitCol* ); 
  void printTrack(RecMdcTrack* tk);
  void dumpTrackList(const HepAList<MdcxFittedHel>& trackList) const;
  void dumpTrack(RecMdcTrackCol* trackList);
  void dumpMdcxSegs(const HepAList<MdcxSeg>& segList)const;
  void bookNTuple();
  void fillMdcxSegs(const HepAList<MdcxSeg>& segList)const;
  void fillTrkl(const HepAList<MdcxFittedHel>& firsttrkl) const; 
  void fillMcTruth();
  void fillTrack(TrkRecoTrk* aTrack);
  void fillEvent();
  void dropMultiHotInLayer(TrkHitList* list);
  void dumpTdsTrack(RecMdcTrackCol* trackList);
  void dumpTdsHits(RecMdcHitCol* hitList);
  const MdcDigi* 	m_digiMap[43][288];


#ifdef MDCXTIMEDEBUG
  IBesTimerSvc* m_timersvc;
  static const int NTIMERS = 5;
  BesTimer* m_timer[NTIMERS];
#endif

};

#endif
