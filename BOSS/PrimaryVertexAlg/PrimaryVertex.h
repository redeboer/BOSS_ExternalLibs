#ifndef Analysis_PRIMARYVTX_H
#define Analysis_PRIMARYVTX_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TH1D.h"
#include "CLHEP/Matrix/Vector.h"
#include "EvtRecEvent/EvtRecPrimaryVertex.h"
#include "EvtRecEvent/EvtRecTrack.h"

using CLHEP::HepVector;

class EvtRecEvent;

class PrimaryVertex : public Algorithm {
 public:
  PrimaryVertex(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

 private:
  StatusCode RegisterEvtRecPrimaryVertex(
      EvtRecPrimaryVertex* aNewEvtRecPrimaryVertex,
      MsgStream& log);

  void SelectGoodChargedTracks(
      SmartDataPtr<EvtRecEvent>& recEvent,
      SmartDataPtr<EvtRecTrackCol>& recTrackCol,
      std::vector<int>& icp, std::vector<int>& icm, std::vector<int>& iGood);

 private:
  int m_sel_number[15];

  int m_trackNumberCut;
  double m_vz0Cut;
  double m_cosThetaCut; 
  int m_fitMethod;
  double m_globalChisqCut; 
  double m_chisqCut;
  int m_trackIteration;
  int m_vertexIteration;
  double m_chi2CutforTrkIter;
  int m_freedomCut;
  double m_chi2CutforSmooth;
  /*
  //particle ID cut
  bool m_useDedx;
  bool m_useTof1;
  bool m_useTof2;
  bool m_useTofE;
  bool m_useTofQ;
  bool m_useEmc;
  bool m_useMuc;
  double m_PidProbCut;*/

  int m_output; //switch of output PrimaryVertex.root

  // define NTuples
  NTuple::Tuple* m_tuple1; // Global vertex fit results
  NTuple::Item<double>  m_chig;
  NTuple::Item<long>    m_ndofg;
  NTuple::Item<double>  m_probg;
  NTuple::Item<double>  m_gvx;
  NTuple::Item<double>  m_gvy;
  NTuple::Item<double>  m_gvz;

  NTuple::Tuple* m_tuple2; // chi-square of smooth
  NTuple::Item<double>  m_chis;
  NTuple::Item<double>  m_chif;
  NTuple::Item<double>  m_probs;
  NTuple::Item<double>  m_probf;
  
  NTuple::Tuple* m_tuple3;
  NTuple::Item<double> m_pull_drho;
  NTuple::Item<double> m_pull_phi;
  NTuple::Item<double> m_pull_kapha;
  NTuple::Item<double> m_pull_dz;
  NTuple::Item<double> m_pull_lamb;
  NTuple::Item<double> m_pull_momentum;

  NTuple::Tuple* m_tuple4; // Kalman vertex fit results
  NTuple::Item<double>  m_chik;
  NTuple::Item<long>    m_ndofk;
  NTuple::Item<double>  m_probk;
  NTuple::Item<double>  m_kvx;
  NTuple::Item<double>  m_kvy;
  NTuple::Item<double>  m_kvz;
};
#endif
