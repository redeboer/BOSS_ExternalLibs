#ifndef EventFilter_H
#define EventFilter_H

#include "GaudiKernel/Algorithm.h"

class HltInf;

class EventFilter: public Algorithm{
 public:
  EventFilter(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~EventFilter();
  
  StatusCode initialize();
  StatusCode execute();
  
  static const std::string& getName() { return m_type;};
  static long unsigned getNEvt() { return m_nTotalEvent;};
  static long unsigned getNPass() { return m_npass;};
  static bool getResult() {return m_efResult;}; 

  void print(HltInf*);
  
 private:
  static std::string m_type;
  static long unsigned m_nTotalEvent;
  static long unsigned m_npass;
  static bool          m_efResult;

  bool                     m_trgFlag;
  int                      m_outputLevel;
  std::vector<std::string> m_chn;
  bool                     m_example;
};
#endif //EventFilter_H
