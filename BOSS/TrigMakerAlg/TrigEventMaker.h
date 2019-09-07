#include "GaudiKernel/Algorithm.h"
#include <string>

class IDataProviderSvc;

class TrigEventMaker:public Algorithm {
public:
  TrigEventMaker(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:

};
