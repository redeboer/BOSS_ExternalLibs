#ifndef STEPDECISION_H
#define STEPDECISION_H

#include "GaudiKernel/Algorithm.h"

#include "HltSteerData/Signature.h"
#include "HltStore/HltStoreSvc.h"

#include <vector>
#include <string> 

class StepDecision : public Algorithm 
{
 public:

  StepDecision(const std::string& name, ISvcLocator* pSvcLocator);
  ~StepDecision();

  virtual bool isEnabled( ) const{return m_isEnabled;};
  StatusCode initialize() {return StatusCode::SUCCESS;};
  StatusCode execute() {return StatusCode::SUCCESS;};
  StatusCode finalize() {return StatusCode::SUCCESS;};

  StatusCode initDecision(HltProcessor::Signature* );
  int execDecision(const std::string& sigID);  
  const std::string& getBehaviour() const { return m_behaviour; }
  const std::string& getContinue() const { return m_continue; }
 private:
  HltStoreSvc* m_HltStoreSvc;

  std::string m_behaviour;
  std::string m_continue;
  std::map<std::string, HltProcessor::Signature*> m_SigMap;

  bool m_isEnabled;
};
#endif
