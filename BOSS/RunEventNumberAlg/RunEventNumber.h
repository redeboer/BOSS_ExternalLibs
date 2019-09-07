#ifndef RunEventNumber_H
#define RunEventNumber_H 

//#include <cstring>
//#include <string>

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"


class RunEventNumber : public Algorithm {

public:
  RunEventNumber(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();  

private:

  int m_period;
  int numEvents;
  int numRuns;
  int prevRun;

};

#endif 
