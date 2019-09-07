#include "RealizationSvc/IRealizationSvc.h"
#include "RealizationSvc/RealizationSvc.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/ISvcLocator.h"
#include <cstdlib>

namespace DetVerSvcPack {

    int funcSimulation(ISvcLocator* svcLct)
    {
        IRealizationSvc* iSvc = 0;
        StatusCode sc = svcLct->service("RealizationSvc", iSvc, false);
        if ( sc.isSuccess() ) {
            RealizationSvc* svc = dynamic_cast<RealizationSvc*>(iSvc);
            if ( svc != 0 ) {
                return abs(svc->getRunId());
            }
        }
        return -1;
    }

    extern int (*pf_helper)(ISvcLocator*);

    //initialize function pointer: pf_helper
    struct SimulationInit {
        SimulationInit() {
            pf_helper = funcSimulation;
        }
    };

    SimulationInit init;
}
