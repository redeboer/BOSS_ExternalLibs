#ifndef IEFEventLoopMgr_H
#define IEFEventLoopMgr_H
#include "GaudiKernel/IInterface.h"
#include <vector>
#include <string>
#include <stdint.h>

// Declaration of the interface ID ( interface id, major version, minor version)
static const InterfaceID IID_IEFEventLoopMgr("IEFEventLoopMgr", 1 , 0);

class IEFEventLoopMgr: virtual public IInterface  {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IEFEventLoopMgr; }

  /// prepare for run step
  virtual StatusCode prepareForRun(int) = 0;

  ///
  virtual bool getTrigChn(uint32_t*, uint32_t) = 0;
  /// return data from EF decision
  virtual std::string EF_Decision() = 0;
  /// return version number
  virtual uint32_t EF_Version() =0;
  /// return algorithm tag
  virtual uint32_t EF_Algorithm_Tag()=0;
  /// return total energy
  virtual uint32_t EF_Total_Energy()=0;
  ///Return a pointer to the local memory containing the EF subdetector fragment
  virtual uint32_t* EF_Fragment_Address()=0;
};

#endif
