#ifndef STEPHANDLER_H
#define STEPHANDLER_H

#include <vector>
#include <string>

#include "GaudiKernel/Algorithm.h"
#include "HltConfig/HltConfigSvc.h"
#include "HltStore/HltStoreSvc.h"
#include "HltDataTypes/EFResult.h"
#include "HltSteering/StepSequencer.h"
#include "HltSteering/StepDecision.h"

#ifdef TIME_MEASUREMENT  ////// Timer
#include "GaudiKernel/NTuple.h"
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include <iomanip>  //setw()
#endif

class StepHandler : public Algorithm
{
 public:

  StepHandler(const std::string& name, ISvcLocator* pSvcLocator);
  ~StepHandler();

  StatusCode initialize();
  StatusCode finalize();
  StatusCode execute();
  StatusCode beginRun();

 private:

  HltStoreSvc*     m_HltStoreSvc;
  HltConfigSvc*    m_HltConfigSvc;
  StepSequencer*   sequencerAlg;
  StepDecision*    decisionAlg;

  EFResult*        m_EFResult;
  EFResult*        m_evtType;
  std::vector<std::string> m_stepIdList;

  std::vector<std::string> m_tmp_evtType;
#ifdef TIME_MEASUREMENT   //// Timer
  IBesTimerSvc* m_timersvc;
  BesTimer* m_timer[5];

  NTuple::Tuple* m_tuple;

  NTuple::Item<float> g_StepHandlerTime;
  NTuple::Item<float> g_1stStepExecTime;
  NTuple::Item<float> g_1stStepDeciTime;
  NTuple::Item<float> g_2ndStepExecTime;
  NTuple::Item<float> g_2ndStepDeciTime;
  unsigned long round;
#endif
};

#endif
