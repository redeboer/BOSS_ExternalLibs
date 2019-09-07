#ifndef IDEDXCURSVC_H
#define IDEDXCURSVC_H
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"

// Declaration of the interface ID 
static const InterfaceID  IID_IDedxCurSvc("IDedxCurSvc",1,0);

class IDedxCurSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_IDedxCurSvc; }
   virtual const double  getSigma(int i) = 0;
   virtual const double  getCurve(int i) = 0;
   virtual const int getCurveSize()=0;
   virtual const int getSigmaSize()=0;

};
 
#endif
