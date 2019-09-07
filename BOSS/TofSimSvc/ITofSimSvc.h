#ifndef ITOFSIMSVC_H
#define ITOFSIMSVC_H

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Property.h"

// Declaration of the interface ID 
static const InterfaceID  IID_ITofSimSvc("ITofSimSvc",1,0);

class ITofSimSvc : virtual public IInterface {
 public:
  static const InterfaceID& interfaceID() { return IID_ITofSimSvc; }

  virtual const double BarLowThres()  = 0;
  virtual const double BarHighThres() = 0;
  virtual const double EndLowThres()  = 0;
  virtual const double EndHighThres() = 0;
  virtual const double BarPMTGain() = 0;
  virtual const double EndPMTGain() = 0;
  virtual const double BarConstant() = 0;
  virtual const double EndConstant() = 0;
  virtual const double EndNoiseSwitch() = 0;

  virtual const double BarGain1(unsigned int id) = 0;
  virtual const double BarGain2(unsigned int id) = 0;
  virtual const double EndGain(unsigned int id) = 0;
  virtual const double BarAttenLength(unsigned int id) = 0;
  virtual const double EndAttenLength(unsigned int id) = 0;
  virtual const double EndNoiseSmear(unsigned int id) = 0;

  virtual void Dump() = 0;
};

#endif // ITOFSIMSVC_H
