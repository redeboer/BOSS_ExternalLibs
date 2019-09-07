#include <string>
#include <map>
#include "GaudiKernel/Algorithm.h"

class DumpDecayTreeAlg : public Algorithm {

public:
  DumpDecayTreeAlg(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  


private:

  std::map<int,std::string> m_map;

  int m_eventCounter;
  int m_freq;

  bool m_BesEvtGenOnly; 
  bool m_PrintParticles; 

  bool m_FindRunEvent;
  int m_FindRun;
  int m_FindEvent;
};
