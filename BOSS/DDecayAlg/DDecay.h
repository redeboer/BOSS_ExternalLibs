#ifndef Physics_Analysis_DDecay_H
#define Physics_Analysis_DDecay_H 

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

#include "VertexFit/IVertexDbSvc.h"

class DDecay : public Algorithm {

public:
  DDecay(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

 
  // define Ntuples here

  NTuple::Tuple*  m_tuple1;    // charged track vertex
  NTuple::Item<double>  m_vx0;
  NTuple::Item<double>  m_vy0;
  NTuple::Item<double>  m_vz0;
  NTuple::Item<double>  m_vr0;

  
  NTuple::Tuple*  m_tuple2;    // check Ks
  NTuple::Item<double>  m_ksmass;
  NTuple::Item<double>  m_ksd;
  NTuple::Item<double>  m_ksmode;

  NTuple::Tuple*  m_tuple3;      // check pi0
  NTuple::Item<double>  m_pi0mass;
  NTuple::Item<double>  m_pi0mode;
  
  NTuple::Tuple*  m_tuple4;     // DTag info
  NTuple::Item<double>  m_mode;
  NTuple::Item<double>  m_type;
  NTuple::Item<double>  m_charge;
  NTuple::Item<double>  m_charm;
  NTuple::Item<double>  m_numofchildren;
  NTuple::Item<double>  m_mass;
  NTuple::Item<double>  m_mBC;
  NTuple::Item<double>  m_deltae;
  NTuple::Item<double>  m_e;
  NTuple::Item<double>  m_ntrk;
  
};

#endif 
