#ifndef MDCTRKRECON_H
#define MDCTRKRECON_H

#include "GaudiKernel/Algorithm.h"
#include "MdcTrkRecon/MdcFlagHold.h"
#include "GaudiKernel/NTuple.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include <string>
#include "MagneticField/IMagneticFieldSvc.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "MdcPrintSvc/MdcPrintSvc.h"

class MdcDetector;
class MdcSegData;
class MdcSegList;
class MdcTrackListBase;
class MdcSegFinder;
class BField;
class IHistogram1D;
class McParticle;
#ifdef MDCPATREC_TIMETEST
class IBesTimerSvc;
class BesTimer;
#endif

#ifdef UseCalib
class MdcCalibFunSvc;
#endif


class MdcTrkRecon:public Algorithm {
public:
  MdcTrkRecon(const std::string& name, ISvcLocator* pSvcLocator);
  ~MdcTrkRecon();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();

protected:
  bool ignoringUsedHits() const              {return m_onlyUnusedHits;}
  bool poisoningHits() const                 {return m_poisonHits;}
  void fillSegList();
  void fillTrackList();
  void dumpDigi();
  void dumpTdsTrack(RecMdcTrackCol* trackList);
  void fillMcTruth();
  void fillEvent();
  StatusCode bookNTuple();
private:
  int t_iExecute;
  int m_fittingMethod;
  int m_allHit;//if all hit (active and inactive) include in the number of nHit
  std::string m_configFile;
  std::string m_paramFile;
  std::string m_pdtFile;
  const MdcDetector * _gm;
  MdcFlagHold m_flags; // run-time-settable flags and params  
  std::auto_ptr<MdcSegData> m_hitData;
  std::auto_ptr<MdcSegList> m_segs;
  std::auto_ptr<MdcTrackListBase> m_tracks;
  std::auto_ptr<MdcSegFinder> m_segFinder;
  RawDataProviderSvc* m_rawDataProviderSvc;
  MdcPrintSvc*          m_mdcPrintSvc;

  BField *m_bfield;
  
  std::vector<int>  m_selEvtNo;
  bool m_onlyUnusedHits;
  bool m_poisonHits;
  bool m_doLineFit;    
  bool m_tryBunch; 
  bool m_recForEsTime; 
  std::vector<float> m_helixHitsSigma;

  uint32_t 	    m_getDigiFlag;
  int		    m_maxMdcDigi;
  bool		    m_keepBadTdc;
  bool		    m_dropHot;
  bool		    m_keepUnmatch;
  int		    m_minMdcDigi;

  bool m_fieldCosmic;
  double m_d0Cut;
  double m_z0Cut;
  double m_dropTrkPt;
  bool m_mcHist;
  int  m_hist;
  int  m_debug;
  bool m_doSagFlag;
  bool m_arbitrateHits;
  bool m_combineTracking;
  
  double mcDrift[43][288];//FIXME assume max 43 track/event, 288 hit/track
  double mcLayer[43][288];
  double mcWire[43][288];
  double mcLR[43][288];
  double mcX[43][288];
  double mcY[43][288];
  double mcZ[43][288];
  double hitOnSeg[43];
  double hitOnMcTk[43];
  int hitPoisoned[43][288];
  int hitInSegList[43][288];
  int   t_nHitInTk[100];
  int    t_t0Stat;
  double t_t0;
  double t_t0Truth;
  double t_mcTkNum;
  int	 t_nDigi;
  int    t_eventNo;
  int    t_nRecTk;
  int    t_iExexute;
  int isPrimaryOfMcTk[100];
  int pdgOfMcTk[100];
  bool m_noInner;
#ifdef MDCPATREC_TIMETEST
  IBesTimerSvc* m_timersvc;
  BesTimer* m_timer[2];
  double ti_nHit;
  double ti_p;
  double ti_pt;
#endif
  IMagneticFieldSvc*               m_pIMF;
};

#endif
