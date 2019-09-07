#ifndef EventFillEventTag_H
#define EventFillEventTag_H

#include "EventTag/EventTagSvc.h"
#include "GaudiKernel/Algorithm.h"

class FillEventTag:public Algorithm {
public:
  FillEventTag (const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
private:
  IEventTagSvc* m_EventTagSvc;
  unsigned int  m_EventTag;
  unsigned int  initialEventTag;
};
#endif
