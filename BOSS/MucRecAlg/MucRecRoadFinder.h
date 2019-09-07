//$id$
//
//$log$

/*
 *    2003/09/06   Zhengyun You      Peking University
 * 
 *    2005/02/27   Zhengyun You      Peking University
 *                 transplanted to Gaudi framework
 */

#ifndef MUC_REC_ROAD_FINDER_H
#define MUC_REC_ROAD_FINDER_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHitContainer.h"
#include <string>
/**
 * Reconstruction of Muon Chamber tracks by combining two 2DRoads
 * to form a 3DRoad. 
 *
 * @author Zhengyun You \URL{mailto:youzy@hep.pku.cn}
 *
 */

class MucRecRoadFinder : public Algorithm
{
 public:
  MucRecRoadFinder(const std::string &name, ISvcLocator* pSvcLocator);
  ~MucRecRoadFinder(){};

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  void TrackFinding(RecMucTrack *aTrack);

 private:
  int m_NEvent;
  int m_NEventWithHit;
  int m_NEventReco;
  int m_NHitsLostTotal;
  int m_NHitsTotal;
  std::vector<int> m_NHitsLost;
  std::vector<int> m_NHitsLostInGap;

  int m_fittingMethod;
  std::string m_configFile;
  int m_mccosmic;
  int m_NtOutput; //switch of output Muc.root
  int m_onlyseedfit; //only fit those layers being seed
  int m_maxHitsRec; //too many hits, do not deal with;
  int m_united;  // judge if that is united or not
  int m_seedtype; // 0: find seed gaps event by event, 1: fix seed gaps 
  bool m_MsOutput; // for debug cout or not 

  std::string m_filter_filename;
  struct FilterEvent {
    std::string bossver;
    int runid;
    int eventid;
  }; 
  std::vector<FilterEvent> m_filter_event;
  MucRecHitContainer *aMucRecHitContainer;

  NTuple::Tuple*  m_tuple;
  NTuple::Item<double>             m_part;
  NTuple::Item<double>             m_seg;
  NTuple::Item<double>             m_gap;
  NTuple::Item<double>             m_strip;
  NTuple::Item<double>             m_diff;
  NTuple::Item<double>             m_dist;

  NTuple::Item<double>             m_run;
  NTuple::Item<double>             m_event;
  NTuple::Item<double>             m_ngapwithhits;
  NTuple::Item<double>             m_nhit;
  NTuple::Item<double>             m_maxhit;
  NTuple::Item<double>             m_multihit;
  NTuple::Item<double>             m_angle_cosmic;
  NTuple::Item<double>             m_angle_updown;  
  NTuple::Item<double>             m_px;
  NTuple::Item<double>             m_py;
  NTuple::Item<double>             m_pz;
  NTuple::Item<double>             m_theta;
  NTuple::Item<double>             m_phi;
  NTuple::Item<double>             m_theta_pipe;
  NTuple::Item<double>             m_phi_pipe;
  NTuple::Item<double>             m_px_mc;
  NTuple::Item<double>             m_py_mc;
  NTuple::Item<double>             m_pz_mc;
  NTuple::Item<double>             m_theta_mc;
  NTuple::Item<double>             m_phi_mc;
  NTuple::Item<double>             m_theta_mc_pipe;
  NTuple::Item<double>             m_phi_mc_pipe;
  NTuple::Item<double>             m_emcUp;
  NTuple::Item<double>             m_emcDown;
  NTuple::Item<double>             m_mucUp;
  NTuple::Item<double>             m_mucDown;
  NTuple::Item<double>             m_projx;
  NTuple::Item<double>             m_projz;

};

#endif /* MUC_REC_ROAD_FINDER_H */
