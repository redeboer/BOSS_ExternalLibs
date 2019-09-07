#ifndef FarmMonitorAlg_H
#define FarmMonitorAlg_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

/// For histograms
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogramFactory.h"
using AIDA::IHistogram1D;
using AIDA::IHistogram2D;

class FarmMonitorAlg : public Algorithm {

public:
  FarmMonitorAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

  // Declare external property variables
  bool m_debug;
  int m_RunEventFreq;

  double m_ecm;
  double m_vr0cut;
  double m_vz0cut;

  //histograms

  /// Total energies
  IHistogram1D* h_eVisibleDivEcm;
  IHistogram1D* h_eEMCDivEcm;
  IHistogram1D* h_eNeutralDivEcm;
  IHistogram1D* h_eChargedDivEcm;

  /// Net momenta
  IHistogram1D* h_netMomentumDivEcm_AllChargedTracks;
  IHistogram1D* h_netTransMomentumDivEcm_AllChargedTracks;
  IHistogram1D* h_cosTheta_AllChargedTracks;

  IHistogram1D* h_netMomentumDivEcm_AllNeutralTracks;
  IHistogram1D* h_netTransMomentumDivEcm_AllNeutralTracks;
  IHistogram1D* h_cosTheta_AllNeutralTracks;

  IHistogram1D* h_netMomentumDivEcm_AllTracks;
  IHistogram1D* h_netTransMomentumDivEcm_AllTracks;
  IHistogram1D* h_cosTheta_AllTracks;

  /// Charged Tracks
  IHistogram1D* h_trackR0;
  IHistogram1D* h_trackZ0;
  IHistogram1D* h_nChargedTracks;
  IHistogram1D* h_nChargedTracksIP;
  IHistogram1D* h_netCharge;
  IHistogram1D* h_netChargeIP;

  /// 2 highest momentum charged tracks
  IHistogram1D* h_pIPTrack1DivEb;
  IHistogram1D* h_pIPTrack2DivEb;
  IHistogram1D* h_eEMCIPTrack1DivEb;
  IHistogram1D* h_eEMCIPTrack2DivEb;
  IHistogram1D* h_acoplanarity_2HighestPIPTracks;

  /// Neutral Tracks
  IHistogram1D* h_nNeutralTracks;
  IHistogram1D* h_nNeutralTracksGT30MeV;
  IHistogram1D* h_eEMCShower1DivEb;
  IHistogram1D* h_eEMCShower2DivEb;
  IHistogram1D* h_eEMCShower3DivEb;

  /// MUC information
  IHistogram1D* h_mucDepth;
  IHistogram2D* h_mucDepthVsCosTheta;
  IHistogram2D* h_mucDepthVsPhi;

  /// PID (dE/dx) information
  IHistogram1D* h_dedxTotalHitsIP;
  IHistogram1D* h_dedxGoodHitsIP;
  IHistogram1D* h_dedxElecChiIP;
  IHistogram1D* h_dedxMuonChiIP;
  IHistogram1D* h_dedxPionChiIP;
  IHistogram1D* h_dedxKaonChiIP;
  IHistogram1D* h_dedxProtonChiIP;
  IHistogram1D* h_dedxProbPHIP;
  IHistogram2D* h_dedxProbPHVsMomentumIP; 

  /// PID (TOF) information
  IHistogram1D* h_tofPHIP_BarrelLayer1;
  IHistogram1D* h_tofPHIP_BarrelLayer2;
  IHistogram1D* h_tofPHIP_EastEndcap;
  IHistogram1D* h_tofPHIP_WestEndcap;
  IHistogram1D* h_tofIP_BarrelLayer1;
  IHistogram1D* h_tofIP_BarrelLayer2;
  IHistogram1D* h_tofIP_EastEndcap;
  IHistogram1D* h_tofIP_WestEndcap;
  IHistogram1D* h_tofElecIP_Barrel;
  IHistogram1D* h_tofMuonIP_Barrel;
  IHistogram1D* h_tofPionIP_Barrel;
  IHistogram1D* h_tofKaonIP_Barrel;
  IHistogram1D* h_tofProtonIP_Barrel;
  IHistogram1D* h_tofElecIP_Endcap;
  IHistogram1D* h_tofMuonIP_Endcap;
  IHistogram1D* h_tofPionIP_Endcap;
  IHistogram1D* h_tofKaonIP_Endcap;
  IHistogram1D* h_tofProtonIP_Endcap;
  IHistogram2D* h_tofVsMomentumIP; 

  /// VeeVertex information
  IHistogram1D* h_nKs;
  IHistogram1D* h_ksMass;

  IHistogram1D* h_nLambda;
  IHistogram1D* h_lambdaMass;

  // Add Pi0 histograms when official pi0 list is ready

};

#endif 
