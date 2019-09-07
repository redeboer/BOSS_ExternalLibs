#ifndef EventPreSelect_H
#define EventPreSelect_H 
#include <cstring>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EventPreSelect/DimuPreSelect.h"

class EventPreSelect : public Algorithm {

public:
  EventPreSelect(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 

  bool WhetherSector(double ph,double ph1,double ph2);

private:

  int m_BarrelOrEndcap;
  bool m_output;
  double m_ecm;
  static int idmax[43];

  // Tag this event
  bool m_isBarrelBhabha;
  bool m_isEndcapBhabha;
  bool m_isBarrelDimu;
  bool m_isEndcapDimu;
  bool m_isHadron;
  bool m_isBarrelDiphoton;
  bool m_isEndcapDiphoton;

  // Whether select event
  bool m_selectBhabha;
  bool m_selectDimu;
  bool m_selectHadron;
  bool m_selectDiphoton;
  bool m_writeDst;
  bool m_writeRec;

  // Sub-Algorithm for each event
  DimuPreSelect *m_dimuAlg;

  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;
  double m_pt0HighCut;
  double m_pt0LowCut;

  // Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaPhiCut;
  double m_gammaThetaCut;

  // Select creteria for each event
  double m_bhabhaEmcECut;
  double m_bhabhaMaxECut;
  double m_bhabhaSecECut;
  double m_bhabhaDTheCut;
  double m_bhabhaDPhiCut1;
  double m_bhabhaDPhiCut2;
  double m_bhabhaDPhiCut3;
  double m_bhabhaDPhiCut4;
  int    m_bhabhaMdcHitCutB;
  int    m_bhabhaMdcHitCutE;
  
  double m_dimuEHighCut;
  double m_dimuELowCut;
  double m_dimuDTheCut;
  double m_dimuDPhiCut;
  
  double m_hadronChaECut;
  double m_hadronNeuECut;
  
  double m_diphotonEmcECut; 
  double m_diphotonSecECut;
  double m_diphotonDTheCut;
  double m_diphotonDPhiCut1;
  double m_diphotonDPhiCut2;

  /**number of total events */
  long int m_events;

  /**number of events selected */
  long int m_barrelBhabhaNumber;  
  long int m_endcapBhabhaNumber;  
  long int m_barrelDimuNumber; 
  long int m_endcapDimuNumber; 
  long int m_hadronNumber;  
  long int m_barrelDiphotonNumber;
  long int m_endcapDiphotonNumber;

  
  Algorithm* m_subalg1; //select barrel bhabha
  Algorithm* m_subalg2; //select endcap bhabha
  Algorithm* m_subalg3; //select barrel dimu
  Algorithm* m_subalg4; //select endcap dimu
  Algorithm* m_subalg5; //select hadron
  Algorithm* m_subalg6; //select barrel diphoton
  Algorithm* m_subalg7; //select endcap diphoton
  Algorithm* m_subalg8; //write dst
  Algorithm* m_subalg9; //write rec 
    
  // define Ntuples here

  NTuple::Tuple*  m_tuple0;      //Trk2Sh2
  NTuple::Item<double>  m_esum;
  NTuple::Item<double>  m_eemc;
  NTuple::Item<double>  m_etot;
  NTuple::Item<double>  m_nGood;
  NTuple::Item<double>  m_nCharge;
  NTuple::Item<double>  m_nGam;
  NTuple::Item<double>  m_ptot;
  NTuple::Item<double>  m_pp;
  NTuple::Item<double>  m_pm;
  NTuple::Item<long>    m_runnb;
  NTuple::Item<long>    m_evtnb;
  NTuple::Item<double>  m_maxE;
  NTuple::Item<double>  m_secE;
  NTuple::Item<double>  m_dThe;
  NTuple::Item<double>  m_dPhi;
  NTuple::Item<long>    m_mdcHit1; 
  NTuple::Item<long>    m_mdcHit2;

  NTuple::Tuple*  m_tuple1;      // charged track vertex
  NTuple::Item<double>  m_vx0;
  NTuple::Item<double>  m_vy0;
  NTuple::Item<double>  m_vz0;
  NTuple::Item<double>  m_vr0;
  NTuple::Item<double>  m_theta0;
  NTuple::Item<double>  m_p0;
  NTuple::Item<double>  m_pt0;

  NTuple::Tuple*  m_tuple2;      // photon
  NTuple::Item<double>  m_dthe;
  NTuple::Item<double>  m_dphi;
  NTuple::Item<double>  m_dang;
  NTuple::Item<double>  m_eraw;

  NTuple::Tuple*  m_tuple3;      // for dimu selection
};

#endif 
