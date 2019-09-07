#ifndef BesHepMCInterface_H
#define BesHepMCInterface_H

#include <string>
#include "GaudiKernel/IDataProviderSvc.h"
#include "HepMC/GenEvent.h"
#include "G4Svc/G4HepMCInterface.h"

class BesHepMCInterface :public G4HepMCInterface {
public:
	BesHepMCInterface();
	~BesHepMCInterface();
  
  HepMC::GenEvent* GenerateHepMCEvent();
  
private:
        IDataProviderSvc* p_evtSvc;
};

#endif
