#ifndef ITOFGEOMSVC_H_
#define ITOFGEOMSVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include "TofGeomSvc/BTofGeo.h"
#include "TofGeomSvc/ETofGeo.h"

// Declaration of the interface ID 
static const InterfaceID  IID_ITofGeomSvc = 1058;

class BTofGeo;
class ETofGeo;

class ITofGeomSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_ITofGeomSvc; }
   virtual BTofGeo* BTof(unsigned id) const= 0;
   virtual ETofGeo* ETof(unsigned id) const= 0;
   virtual const double getBPhiMax(unsigned id)=0; 
   virtual const double getBPhiMin(unsigned id)=0; 
   virtual const double getEPhiMax(unsigned id)=0; 
   virtual const double getEPhiMin(unsigned id)=0; 
   virtual void Dump() = 0;
};
 
#endif // ITOFGEOMSVC_H
