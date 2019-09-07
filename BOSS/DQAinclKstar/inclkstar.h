#ifndef Physics_Analysis_DQAinclKstar_H
#define Physics_Analysis_DQAinclKsstar_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "TROOT.h"
#include "TObjArray.h"
#include "TH1F.h"
#include "TProfile.h"

class ITHistSvc;

class inclkstar : public Algorithm {

public:
  inclkstar(const std::string& name, ISvcLocator* pSvcLocator);

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

  NTuple::Tuple*  m_tuple2;      
  NTuple::Item<double>  m_nkm;
  NTuple::Item<double>  m_nkp;
  NTuple::Item<double>  m_npip;
  NTuple::Item<double>  m_npim;
  NTuple::Item<double>  m_ncharge;
  NTuple::Item<double>  m_difchikp;
  NTuple::Item<double>  m_difchikm;
  NTuple::Item<double>  m_kstarkp;
  NTuple::Item<double>  m_kstarkm;
  NTuple::Item<double>  m_mkstar;
  NTuple::Item<double>  m_pkstar;

  ITHistSvc* m_thsvc;
};

#endif 
