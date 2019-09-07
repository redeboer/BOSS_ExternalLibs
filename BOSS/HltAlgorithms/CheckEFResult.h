#ifndef CHECKEFRESULT_H
#define CHECKEFRESULT_H

#include <string>

#include "GaudiKernel/Algorithm.h"
//#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
//#include "RawDataCnv/EventManagement/MucBuilder.h"
#include "HltStore/HltStoreSvc.h"

#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/NTuple.h"
#include <vector>
//#include "BesTimerSvc/IBesTimerSvc.h"
//#include "BesTimerSvc/BesTimerSvc.h"

class CheckEFResult : public Algorithm {
 public:
  CheckEFResult(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~CheckEFResult();

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
 private:
  //inline clock_t GetCycleCount(void);

 private:
  HltStoreSvc* m_HltStoreSvc;
  IDataProviderSvc* m_evtDataSvc;
  
  NTuple::Tuple*  m_tuple;
  NTuple::Item<long>  m_nevt;
  NTuple::Item<long>  m_alg;
  NTuple::Item<long>  m_nmdc;
  NTuple::Item<long>  m_ntof;
  NTuple::Item<long>  m_nemc;
  NTuple::Item<long>  m_nmuc;
  NTuple::Item<long>  m_nshower;
  NTuple::Item<long>  m_ntrk;
  NTuple::Item<long>  m_nsec;
  NTuple::Item<float>  m_etot;
  NTuple::Item<float>  m_ebarrel;
  NTuple::Item<float>  m_eendcap;
  NTuple::Item<float>  m_acop;
  NTuple::Item<float>  m_ebalance;
  NTuple::Item<float>  m_emax1;
  NTuple::Item<float>  m_emax2;
  NTuple::Item<float>  m_dtof1;
  NTuple::Item<float>  m_dphi1;
  NTuple::Item<float>  m_dtof2;
  NTuple::Item<float>  m_dphi2;
  NTuple::Item<float>  m_acol;
  NTuple::Item<float>  m_pmax1;
  NTuple::Item<float>  m_pmax2;
  NTuple::Item<float>  m_cost1;
  NTuple::Item<float>  m_cost2;
  NTuple::Item<float>  m_mbalance;
  NTuple::Item<long>   m_evttype;

  //  IBesTimerSvc* m_timersvc;
  //#define NTIMERS 2
  //BesTimer* m_timer[NTIMERS];
  //NTuple::Item<float>  m_filtertime;
  //NTuple::Item<float>  m_CPUtime;
  //clock_t m_clock;
  //clock_t m_CPUspeed;
  //cycles_t m_cycles;
  std::vector<int> m_notpass;
  std::string       m_outfile;
};

#endif //CHECKEFRESULT_H
