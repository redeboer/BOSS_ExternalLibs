#ifndef IREALIZATIONSVC_H
#define IREALIZATIONSVC_H

#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ObjectVector.h"

/* Declaration of the interface ID */
static const InterfaceID IID_IRealizationSvc("IRealizationSvc",1,0);

class IRealizationSvc: virtual public IInterface{

public:
  static const InterfaceID& interfaceID() { return IID_IRealizationSvc; }

  // Interface
/*
  virtual void readDB(int runNo) = 0;
  virtual void updateRealData() = 0;

  virtual double getLuminosity() = 0;
  virtual double getBunchSizeX() = 0; 
  virtual double getBunchSizeY() = 0; 
  virtual double getBunchSizeZ() = 0; 
  virtual double getBunchSigmaX() = 0;
  virtual double getBunchSigmaY() = 0;
  virtual double getBunchSigmaZ() = 0;
*/
};

#endif 
