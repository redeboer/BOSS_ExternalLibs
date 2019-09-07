#ifndef Physics_Analysis_DQAinclLamda_H
#define Physics_Analysis_DQAinclLamda_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TProfile.h"

class ITHistSvc;

class incllambda : public Algorithm {

public:
  incllambda(const std::string& name, ISvcLocator* pSvcLocator);

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
  NTuple::Item<double>  m_np;
  NTuple::Item<double>  m_npi;
  NTuple::Item<double>  m_ctau;
  NTuple::Item<double>  m_lxyz;
  NTuple::Item<double>  m_elxyz;
  NTuple::Item<double>  m_chis;
  NTuple::Item<double>  m_lamRawMass;
  NTuple::Item<double>  m_plam;

  ITHistSvc* m_thsvc;
};

#endif 
