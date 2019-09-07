#include "GaudiKernel/Algorithm.h"
#include <string>

class DstHltMaker:public Algorithm {
public:
  DstHltMaker(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
 // int m_fittingMethod;
  //std::string m_configFile;
  uint32_t     m_ntot;
};
