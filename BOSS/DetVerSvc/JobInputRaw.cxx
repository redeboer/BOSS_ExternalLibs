#include "RawDataCnv/IRawDataInputSvc.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "GaudiKernel/ISvcLocator.h"

namespace DetVerSvcPack {

    int funcInputRaw(ISvcLocator* svcLct)
    {
        IRawDataInputSvc*  iSvc = 0;
        StatusCode sc = svcLct->service("RawDataInputSvc", iSvc, false);
        if ( sc.isSuccess() ) {
            RawDataInputSvc* svc = dynamic_cast<RawDataInputSvc*>(iSvc);
            if ( svc != 0 ) {
                return svc->runNo();
            }
        }
        return -1;
    }

    extern int (*pf_helper)(ISvcLocator*);

    //initialize function pointer: pf_helper
    struct InputRawInit {
        InputRawInit() {
            pf_helper = funcInputRaw;
        }
    };

    InputRawInit init;
}
