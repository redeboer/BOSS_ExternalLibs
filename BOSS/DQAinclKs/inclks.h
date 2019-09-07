#ifndef Physics_Analysis_DQAinclKs_H
#define Physics_Analysis_DQAinclKs_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TProfile.h"

class ITHistSvc;

class inclks : public Algorithm {

public:
  inclks(const std::string& name, ISvcLocator* pSvcLocator);

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
  NTuple::Item<double>  m_npip;
  NTuple::Item<double>  m_npim;
  NTuple::Item<double>  m_ctau;
  NTuple::Item<double>  m_lxyz;
  NTuple::Item<double>  m_elxyz;
  NTuple::Item<double>  m_chis;
  NTuple::Item<double>  m_ksRawMass;
  NTuple::Item<double>  m_pk0;

   ITHistSvc* m_thsvc;
};

#endif 
