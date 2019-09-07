#ifndef IJobInfoSvc_H
#define IJobInfoSvc_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/Service.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IJobInfoSvc("IJobInfoSvc",1,0);


class IJobInfoSvc: virtual public IInterface{

public:

  static const InterfaceID& interfaceID() { return IID_IJobInfoSvc; }

  virtual int xmlrpc(int evtNum) = 0;
  virtual std::string getJobOutputFile() = 0;

};


#endif

