#ifndef __GENMOD_PURGEUNSTABLEPARTICLES__
#define __GENMOD_PURGEUNSTABLEPARTICLES__

#include "GaudiKernel/Algorithm.h"
#include <string>

class StoreGateSvc;

class PurgeUnstableParticles:public Algorithm {
public:
  PurgeUnstableParticles(const std::string& name, ISvcLocator* pSvcLocator);
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  
private:
  std::string m_mcEventColl;
  StoreGateSvc* m_sgSvc;
	
};

#endif
