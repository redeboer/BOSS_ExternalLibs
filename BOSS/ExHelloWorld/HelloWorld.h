#include "GaudiKernel/Algorithm.h"


/////////////////////////////////////////////////////////////////////////////

class HelloWorld:public Algorithm {
public:
  HelloWorld (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:

  int m_myInt;
  bool m_myBool;
  double m_myDouble;
  std::vector<std::string> m_myStringVec;

};

