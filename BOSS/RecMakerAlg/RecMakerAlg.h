#include "GaudiKernel/Algorithm.h"
#include <string>

class RecMakerAlg:public Algorithm {
public:
  RecMakerAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

};
