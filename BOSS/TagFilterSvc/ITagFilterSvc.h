#ifndef ITAGFILTERSVC_H_
#define ITAGFILTERSVC_H_

#include "GaudiKernel/IService.h"
#include "GaudiKernel/Kernel.h"
#include <string>
using namespace std;
/* Declaration of the interface ID */
static const InterfaceID IID_ITagFilterSvc("ITagFilterSvc",1,0);

class ITagFilterSvc: virtual public IService{
public:
     static const InterfaceID& interfaceID() { return IID_ITagFilterSvc; }
};

#endif /* ITAGFILTERSVC_H_ */
