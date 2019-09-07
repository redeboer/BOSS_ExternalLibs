#ifndef IDATAINFOSVC_H_
#define IDATAINFOSVC_H_

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include <string>
using namespace std;
/* Declaration of the interface ID */
static const InterfaceID IID_IDataInfoSvc("IDataInfoSvc",1,0);

class IDataInfoSvc: virtual public IService{
public:
     static const InterfaceID& interfaceID() { return IID_IDataInfoSvc; }
     //virtual void setDecayCard(string card) = 0;

};

#endif /* IDATAINFOSVC_H_ */
