#include "GaudiKernel/Algorithm.h"
#include <string>

class DstEventMaker:public Algorithm {
public:
  DstEventMaker(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
 // int m_fittingMethod;
  //std::string m_configFile;

};
