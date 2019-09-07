#ifndef Physics_Analysis_DQAinclPhi_H
#define Physics_Analysis_DQAinclPhi_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TProfile.h"

class ITHistSvc;

class inclphi : public Algorithm {

public:
  inclphi(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:
  int m_pass[10];

  // Declare r0, z0 cut for charged tracks
  double m_vr0cut;
  double m_vz0cut;

  //
  int m_checkDedx;
  int m_checkTof;

  // define Ntuples here

  NTuple::Tuple*  m_tuple1;
  NTuple::Item<double>  m_mphiall;
  NTuple::Item<double>  m_pphiall;

  NTuple::Tuple*  m_tuple2;
  NTuple::Item<double>  m_nkp;
  NTuple::Item<double>  m_nkm;
  NTuple::Item<double>  m_ncharge;
  NTuple::Item<double>  m_difchi;
  NTuple::Item<double>  m_mphi;
  NTuple::Item<double>  m_pphi;
  NTuple::Item<double>  m_pk1;

   ITHistSvc* m_thsvc;
};

#endif 
