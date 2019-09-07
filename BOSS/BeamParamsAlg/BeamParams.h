#ifndef Analysis_PRIMARYVTX_H
#define Analysis_PRIMARYVTX_H

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "TH1D.h"
#include "CLHEP/Matrix/Vector.h"

using CLHEP::HepVector;
class ITHistSvc;
class BeamParams : public Algorithm
{
public:
  BeamParams(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  ITHistSvc* m_thsvc;//zhaohs
  int m_sel_number[15];

  int m_pid;
  int m_vertexFind;

  double m_vz0Cut;
  double m_cosThetaCut;
  double m_minDistance;
  double m_minPointX;
  double m_minPointY;
  double m_meanPointX;
  double m_meanPointY;
  double m_chisqCut;
  int m_trackIteration;
  int m_vertexIteration;
  double m_chi2CutforTrkIter;
  double m_chi2CutforSmooth;
  int m_trackNumberCut;

  //particle ID cut
  bool m_useDedx;
  bool m_useTof1;
  bool m_useTof2;
  bool m_useTofE;
  bool m_useTofQ;
  bool m_useEmc;
  bool m_useMuc;
  double m_PidProbCut;

  double m_pivotX;
  double m_pivotY;
  double m_pivotZ;

  int m_trkNum;
  int m_runNo;
  int m_parVer;
  std::string m_fileNameDst;
  std::string m_fileNameHadron;
  std::string m_figsName;

  TH1D* m_vertex_x;
  TH1D* m_vertex_y;
  TH1D* m_vertex_z;
  TH1D* m_vertex_x_kal;
  TH1D* m_vertex_y_kal;
  TH1D* m_vertex_z_kal;

  HepVector m_Cpuvc;
  double m_cpu[5];

  int m_hadronFile;

  // define NTuples

  NTuple::Tuple* m_tuple1;  // minimal distance and so on
  NTuple::Item<double>  m_xc;
  NTuple::Item<double>  m_yc;
  NTuple::Item<double>  m_zc;
  NTuple::Item<double>  m_mind;

  NTuple::Tuple* m_tuple2; // chi-square of smooth
  NTuple::Item<double>  m_chis;
  NTuple::Item<double>  m_chif;
  NTuple::Item<double>  m_probs;
  NTuple::Item<double>  m_probf;

  NTuple::Tuple* m_tuple3; // Kalman vertex fit results
  NTuple::Item<double>  m_chik;
  NTuple::Item<long>    m_ndofk;
  NTuple::Item<double>  m_probk;
  NTuple::Item<double>  m_kvx;
  NTuple::Item<double>  m_kvy;
  NTuple::Item<double>  m_kvz;
  NTuple::Item<long> m_numTrk;

  NTuple::Tuple* m_tuple4; // Global vertex fit results
  NTuple::Item<double>  m_chig;
  NTuple::Item<long>    m_ndofg;
  NTuple::Item<double>  m_probg;
  NTuple::Item<double>  m_gvx;
  NTuple::Item<double>  m_gvy;
  NTuple::Item<double>  m_gvz;

  NTuple::Tuple* m_tuple5;
  NTuple::Item<double> m_pull_drho;
  NTuple::Item<double> m_pull_phi;
  NTuple::Item<double> m_pull_kapha;
  NTuple::Item<double> m_pull_dz;
  NTuple::Item<double> m_pull_lamb;
  NTuple::Item<double> m_pull_momentum;

  NTuple::Tuple* m_tuple6;
  NTuple::Item<double> m_mdcTrk_x;
  NTuple::Item<double> m_mdcTrk_y;
  NTuple::Item<double> m_mdcTrk_z;
  NTuple::Item<double> m_mdcTrk_r;
  NTuple::Item<double> m_rxy;
  NTuple::Item<double> m_mdcTrk_dr;
  NTuple::Item<double> m_mdcKalTrk_z;

  NTuple::Tuple* m_tuple7;
  NTuple::Item<double> m_gpull_drho;
  NTuple::Item<double> m_gpull_phi;
  NTuple::Item<double> m_gpull_kapha;
  NTuple::Item<double> m_gpull_dz;
  NTuple::Item<double> m_gpull_lamb;
};
#endif
