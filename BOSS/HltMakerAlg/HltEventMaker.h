#include "GaudiKernel/Algorithm.h"
#include <string>

class IDataProviderSvc;

class HltEventMaker:public Algorithm {
public:
  HltEventMaker(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  uint32_t m_ntot;
};
