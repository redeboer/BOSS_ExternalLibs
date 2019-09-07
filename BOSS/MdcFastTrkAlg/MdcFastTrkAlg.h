#ifndef MdcFastTrkAlg_FLAG_
#define MdcFastTrkAlg_FLAG_

#include "GaudiKernel/Algorithm.h"
//#include "MdcFastTrkAlg/FTFinder.h"

#ifndef OnlineMode
#include "GaudiKernel/NTuple.h"
#endif

#ifndef OnlineMode
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"
#include <string>
#endif

class FTFinder;
class MdcFastTrkAlg:public Algorithm {
public:
  MdcFastTrkAlg(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode beginRun();
  
private:
  FTFinder* m_ftFinder;
	double m_bunchtime_MC;
	double m_mdc_tcal;

#ifndef OnlineMode
  NTuple::Tuple*  m_tuple;

  IBesTimerSvc* m_timersvc;
#define NTIMERS 2
  BesTimer* m_timer[NTIMERS];
#endif

};

#endif /* MdcFastTrkAlg_FLAG_ */
