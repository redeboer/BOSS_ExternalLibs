#ifndef I_DET_VER_SVC_H
#define I_DET_VER_SVC_H

#include "GaudiKernel/IService.h"

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID  IID_IDetVerSvc("IDetVerSvc",1,0);

class IDetVerSvc : virtual public IInterface
{
    public :

        static const InterfaceID& interfaceID() { return IID_IDetVerSvc; }

        virtual int phase() = 0;
};

#endif
