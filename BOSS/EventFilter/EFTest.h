#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"

class EFTest:public Algorithm {
 public:
  EFTest(const std::string& name, ISvcLocator* pSvcLocator);
  ~EFTest();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
 private:
  // float m_acolCut;
  int m_outputLevel;

  NTuple::Tuple* m_tuple;
  NTuple::Item<long>  m_type;
  NTuple::Item<long>  m_alg;
  NTuple::Item<long>  m_endEE;
  NTuple::Item<long>  m_version;
  NTuple::Item<float> m_etot;
  NTuple::Item<long>  m_nmdc0;
  NTuple::Item<long>  m_ntof0;
  NTuple::Item<long>  m_nemc0;
  NTuple::Item<long>  m_nmuc0;
  NTuple::Item<long>  m_ncon0;
  NTuple::Item<long>  m_nmdc;
  NTuple::Item<long>  m_ntof;
  NTuple::Item<long>  m_nemc;
  NTuple::Item<long>  m_nmuc;
  NTuple::Item<long>  m_ncon;
  NTuple::Array<float> m_mdcData;
  NTuple::Array<float> m_tofData;
  NTuple::Array<float> m_emcData;
  NTuple::Array<float> m_mucData;
  NTuple::Array<float> m_conData;

};
