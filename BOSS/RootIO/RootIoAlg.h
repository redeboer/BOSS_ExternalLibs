#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/Algorithm.h"
#include <string>

class RootIoAlg:public Algorithm {
public:
  RootIoAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
//  int m_fittingMethod;
//  std::string m_configFile;

};
