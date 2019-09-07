#ifndef NavigationTestAlg_H
#define NavigationTestAlg_H

#include "GaudiKernel/Algorithm.h"

#include <string>
#include <vector>

class RecMdcTrack;
class TH1F;
class TH2F;

class NavigationTestAlg:public Algorithm {
public:
  NavigationTestAlg (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

private:
  std::string m_fout;
  std::vector<TH1F*> m_histo;
  std::vector<TH2F*> m_histo2;
};

#endif
