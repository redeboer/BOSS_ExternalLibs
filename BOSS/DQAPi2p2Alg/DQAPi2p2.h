#ifndef Physics_Analysis_DQAPi2p2_H
#define Physics_Analysis_DQAPi2p2_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
//#include "VertexFit/ReadBeamParFromDb.h"


class DQAPi2p2 : public Algorithm {

public:
  DQAPi2p2(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

  int Ncut0,Ncut1,Ncut2,Ncut3,Ncut4,Ncut5,Ncut6;

  //ReadBeamParFromDb m_reader;
  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;

  //Declare energy, dphi, dthe cuts for fake gamma's
  double m_energyThreshold;
  double m_gammaPhiCut;
  double m_gammaThetaCut;
  double m_gammaAngleCut;

  // 
  int m_test4C;
  int m_test5C;

  // 
  int m_checkDedx;
  int m_checkTof;
  bool m_saventuple;
  // define Ntuples here

  
  NTuple::Tuple*  m_tuple0;
  NTuple::Item<long>  m_nrun;
  NTuple::Item<long>  m_nrec;
  

  NTuple::Array<double>  m_dedxchi_e;
  NTuple::Array<double>  m_dedxchi_mu;
  NTuple::Array<double>  m_dedxchi_pi;
  NTuple::Array<double>  m_dedxchi_kaon;
  NTuple::Array<double>  m_dedxchi_proton;

  NTuple::Array<double>  m_tofchi_e;
  NTuple::Array<double>  m_tofchi_mu;
  NTuple::Array<double>  m_tofchi_pi;
  NTuple::Array<double>  m_tofchi_kaon;
  NTuple::Array<double>  m_tofchi_proton;

  NTuple::Array<double>  m_trackfitchi;
  NTuple::Array<double>  m_trackfitndof;
  NTuple::Array<double>  m_dedxngoodhit;
  
  NTuple::Array<long>  m_index_pmiss;
  NTuple::Array<long>  m_index_pbmiss;
  NTuple::Array<long>  m_index_pipmiss;
  NTuple::Array<long>  m_index_pimmiss;

  
  NTuple::Item<long>  m_nGam;
  NTuple::Item<long>  m_nGood;
  NTuple::Item<long>  m_nCharge;
  NTuple::Item<long>  m_npi;
  NTuple::Item<long>  m_nkaon;
  NTuple::Item<long>  m_nproton;

  NTuple::Item<long>  m_istat_pmiss;
  NTuple::Item<long>  m_istat_pbmiss;
  NTuple::Item<long>  m_istat_pipmiss;
  NTuple::Item<long>  m_istat_pimmiss;


  NTuple::Array<long>  m_ipidch_pmiss;
  NTuple::Array<long>  m_ipidch_pbmiss;
  NTuple::Array<long>  m_ipidch_pipmiss;
  NTuple::Array<long>  m_ipidch_pimmiss;

  NTuple::Item<double>  m_mpmiss;
  NTuple::Item<double>  m_mpbmiss;
  NTuple::Item<double>  m_mpipmiss;
  NTuple::Item<double>  m_mpimmiss;

  NTuple::Item<double>  m_ppmiss;
  NTuple::Item<double>  m_ppbmiss;
  NTuple::Item<double>  m_ppipmiss;
  NTuple::Item<double>  m_ppimmiss;

  NTuple::Array<double>  m_ptrk_pmiss;
  NTuple::Array<double>  m_ptrk_pbmiss;
  NTuple::Array<double>  m_ptrk_pipmiss;
  NTuple::Array<double>  m_ptrk_pimmiss;

  
  NTuple::Array<long>  m_id_pmiss;
  NTuple::Array<long>  m_id_pbmiss;
  NTuple::Array<long>  m_id_pipmiss;
  NTuple::Array<long>  m_id_pimmiss;


  NTuple::Array<double>  m_eop;
 
};

#endif 
