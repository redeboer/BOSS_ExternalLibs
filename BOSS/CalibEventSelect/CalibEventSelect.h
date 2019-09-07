#ifndef CalibEventSelect_H
#define CalibEventSelect_H 
#include <cstring>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "EvtRecEvent/EvtRecTrack.h"


#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogramFactory.h"
using AIDA::IHistogram1D;


class CalibEventSelect : public Algorithm {

public:
  CalibEventSelect(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize(); 

  bool WhetherSector(double ph,double ph1,double ph2);
  void readbeamEfromDb(int runNo, double & beamE);
  
private:

  int m_BarrelOrEndcap;
  bool m_output;
  bool m_display;
  
  int m_run;
  double m_ecm;
  static int idmax[43];

  //histograms

  IHistogram1D* h_nGood;
  IHistogram1D* h_nCharge;
  IHistogram1D* h_pmaxobeam;
  IHistogram1D* h_eopmax;
  IHistogram1D* h_eopsec;
  IHistogram1D* h_deltap;
  IHistogram1D* h_esumoecm;
  IHistogram1D* h_egmax;
  IHistogram1D* h_deltaphi;
  IHistogram1D* h_Pt;
 
  // Tag this event

  bool m_isRadBhabha;
  bool m_isGGEE;
  bool m_isGG4Pi;
  bool m_isRadBhabhaT;
  bool m_isRhoPi;
  bool m_isKstarK;
  bool m_isPPPiPi;
  bool m_isRecoJpsi;
  bool m_isBhabha;
  bool m_isDimu;
  bool m_isCosmic;
  bool m_isGenProton;
  bool m_isPsipRhoPi;
  bool m_isPsipKstarK;
  bool m_isPsipPPPiPi;
  bool m_isPsippCand;
  
  
  
  // Whether select event
  bool m_printmonitor;
  bool m_selectRadBhabha;
  bool m_selectGGEE;
  bool m_selectGG4Pi;
  bool m_selectRadBhabhaT;
  bool m_selectRhoPi;
  bool m_selectKstarK;
  bool m_selectPPPiPi;
  bool m_selectRecoJpsi;
  bool m_selectBhabha;
  bool m_selectDimu;
  bool m_selectCosmic;
  bool m_selectGenProton;
  bool m_selectPsipRhoPi;
  bool m_selectPsipKstarK;
  bool m_selectPsipPPPiPi;
  bool m_selectPsippCand;


  bool m_writeDst;
  bool m_writeRec;

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


  /**number of total events */
  long int m_events;
  int m_radbhabha_scale;
  int m_bhabha_scale;
  int m_dimu_scale;
  int m_cosmic_scale;
  int m_proton_scale;

  double m_cosmic_lowp;

  /**number of events selected */

  long int m_radBhabhaNumber;
  long int m_GGEENumber;
  long int m_GG4PiNumber;
  long int m_radBhabhaTNumber;
  long int m_rhoPiNumber;
  long int m_kstarKNumber;
  long int m_ppPiPiNumber;
  long int m_recoJpsiNumber;
  long int m_bhabhaNumber;
  long int m_dimuNumber;
  long int m_cosmicNumber;
  long int m_genProtonNumber;
  long int m_psipRhoPiNumber;
  long int m_psipKstarKNumber;
  long int m_psipPPPiPiNumber;
  long int m_psippCandNumber;

  Algorithm* m_subalg1; //write dst
  Algorithm* m_subalg2; //write rec 
  Algorithm* m_subalg3; //write rad bhabha
  Algorithm* m_subalg4; //write gg to ee events
  Algorithm* m_subalg5; //write gg to 4pi events
  Algorithm* m_subalg6; //write rad bhabha with tight selection critera
  Algorithm* m_subalg7; //write rhopi events
  Algorithm* m_subalg8; //write KK* events
  Algorithm* m_subalg9; //write pppipi events
  Algorithm* m_subalg10; //write recoil jpsi (4,2 track) events
  Algorithm* m_subalg11; //write bhabha events
  Algorithm* m_subalg12; //write dimu events
  Algorithm* m_subalg13; //write cosmic events
  Algorithm* m_subalg14; //write generic proton events
  Algorithm* m_subalg15; //write recoil rhopi events
  Algorithm* m_subalg16; //write recoil kstark events
  Algorithm* m_subalg17; //write recoil pppipi events
  Algorithm* m_subalg18; //write psi'' candidates
  

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
};

#endif 
