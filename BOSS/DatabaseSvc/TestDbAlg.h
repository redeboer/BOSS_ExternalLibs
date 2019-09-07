#include "GaudiKernel/Algorithm.h"

class TestDbAlg:public Algorithm {
public:
  TestDbAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
};


