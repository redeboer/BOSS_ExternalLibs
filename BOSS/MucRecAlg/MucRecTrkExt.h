//$id$
//
//$log$

/*
 *    2004/03/08   Zhengyun You      Peking University
 * 
 *    2004/09/12   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_TRK_EXT_H
#define MUC_REC_TRK_EXT_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "HepPDT/ParticleDataTable.hh"
#include <string>
#include <vector>

#include "MucRecEvent/MucRecHitContainer.h"
#include "MucRecEvent/RecMucTrack.h"

class MucRecTrkExt:public Algorithm {
public:
  MucRecTrkExt(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void TrackFinding(RecMucTrack *aTrack);
  float getWindowSize(Hep3Vector mom, int part, int seg, int gap);
  
private:
  int m_ExtTrackSeedMode;
  int m_CompareWithMcHit;
  int m_fittingMethod;
  int m_EmcShowerSeedMode;
  int m_MucHitSeedMode;
  int  m_NtOutput; //switch of output Muc.root
  bool m_Blind;
  bool m_MsOutput;
  long m_totalEvent; 
  
  int m_NDigisTotal;
  int m_NHitsTotal;
  int m_NHitsFoundTotal;
  int m_NHitsLostTotal;
  int m_NHitsMisFoundTotal;
  int m_NHitsLostByMdcTotal;
  int m_NHitsLostByExtTotal;

  int m_NTracksTotal;
  int m_NTracksRecoTotal;
  int m_NTracksLostByMdcTotal;
  int m_NTracksLostByExtTotal;
  int m_NTracksMdcGhostTotal;

  int m_emcrec;

  std::vector<int> m_NHitsLost;
  std::vector<int> m_NHitsLostInGap;
  std::vector<int> m_TrackLostHit;
  std::vector<int> m_TrackLostHitNumber;
  std::vector<int> m_TrackLostByMdc;
  std::vector<int> m_TrackLostByExt;

  std::string      m_configFile;
  HepPDT::ParticleDataTable* m_particleTable;
  MucRecHitContainer *m_MucRecHitContainer;

  std::string m_filter_filename;
  struct FilterEvent {
    std::string bossver;
    int runid;
    int eventid;
  };
  std::vector<FilterEvent> m_filter_event;
  MucRecHitContainer *aMucRecHitContainer;

  NTuple::Tuple*  m_tuple;
  NTuple::Item<double>             m_posx;
  NTuple::Item<double>             m_posy;
  NTuple::Item<double>             m_posz;
  NTuple::Item<double>             m_posx_ext;
  NTuple::Item<double>             m_posy_ext;
  NTuple::Item<double>             m_posz_ext;
  NTuple::Item<double>             m_posx_sigma;
  NTuple::Item<double>             m_posy_sigma;
  NTuple::Item<double>             m_posz_sigma;
  NTuple::Item<double>             m_depth;
  NTuple::Item<double>             m_Distance;
  NTuple::Item<double>             m_MaxHits;
  NTuple::Item<double>             m_Chi2;
  NTuple::Item<double>             m_Dist_x;
  NTuple::Item<double>             m_Dist_y;
  NTuple::Item<double>             m_Dist_z;
  NTuple::Item<double>             m_px_mc;
  NTuple::Item<double>             m_py_mc;
  NTuple::Item<double>             m_pz_mc;
  NTuple::Item<double>             m_emctrack;
  NTuple::Item<double>             m_muc_digi;

  NTuple::Item<double>             m_part;
  NTuple::Item<double>             m_seg;
  NTuple::Item<double>             m_gap;
  NTuple::Item<double>             m_strip;
  NTuple::Item<double>             m_diff;
  NTuple::Item<double>             m_distance;
  
  NTuple::Item<double>             m_run;
  NTuple::Item<double>             m_event;

};

#endif /* MUC_REC_TRK_EXT_H */
