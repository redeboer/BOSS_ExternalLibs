#include "GaudiKernel/Algorithm.h"

class StoreGateSvc;

class DumpMC:public Algorithm {
public:
        DumpMC(const std::string& name, ISvcLocator* pSvcLocator);
        StatusCode initialize();
        StatusCode execute();
        StatusCode finalize();

private:
  StoreGateSvc* m_sgSvc;
};

