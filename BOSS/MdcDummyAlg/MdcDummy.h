#include "GaudiKernel/Algorithm.h"
#include <string>

class MdcDummy:public Algorithm {
public:
  MdcDummy(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  int m_fittingMethod;
  std::string m_configFile;

};
