#ifndef STEPSEQUENCER_H
#define STEPSEQUENCER_H

#include <string> 
#include <math.h>
#include <vector>
#include <map>

#include "GaudiKernel/Algorithm.h"
#include "HltSteerData/Sequence.h"

class HltProcessor::Sequence;

class StepSequencer : public Algorithm
{
 public:

  StepSequencer(const std::string& name, ISvcLocator* pSvcLocator);
  ~StepSequencer();

  virtual bool isEnabled( ) const{return m_isEnabled;};
  StatusCode initialize() { return StatusCode::SUCCESS; }
  StatusCode execute() { return StatusCode::SUCCESS; }
  StatusCode finalize() { return StatusCode::SUCCESS; }

// start of StepSequencer specific interface
  void reset();

  StatusCode initSequencer(HltProcessor::Sequence* );
  bool execSequencer(const std::string& seqID);
  std::vector<Algorithm*> searchAlgForEachTE_o(HltProcessor::Sequence* );
  bool doesExistAlready(std::string );
  Algorithm* existingAlg (std::string );
  //DummyAlg* m_DummyAlg;

 private:
  Algorithm* matchAlg;  
  std::vector<Algorithm* > vMatchAlg;
  //std::string instance;
  std::map<std::string, std::vector<Algorithm* > > m_AlgMap;

  bool m_isEnabled;
};

#endif
