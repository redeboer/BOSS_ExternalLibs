#ifndef BesTMTrig_h
#define BesTMTrig_h 1

#include <vector>

class IBesGlobalTrigSvc;
class BesGlobalTrigSvc;

class BesTMTrig {
public:
  BesTMTrig();
  ~BesTMTrig();
  void startTMTrig();
  void dump();

private:
  IBesGlobalTrigSvc* m_tmpSvc;
  BesGlobalTrigSvc* m_pIBGT;

  std::vector<std::vector<int> > m_tm_barrel;
  std::vector<std::vector<int> > m_tm_ecap;
  std::vector<std::vector<int> > m_tm_wcap;
};
#endif
