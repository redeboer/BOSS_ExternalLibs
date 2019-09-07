#ifndef ITOFQCORRSVC_H_
#define ITOFQCORRSVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"

// Declaration of the interface ID 
static const InterfaceID  IID_ITofQCorrSvc("ITofQCorrSvc",1,0);

class ITofQCorrSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_ITofQCorrSvc; }
   virtual const double  getBarEast(int layer,int no) = 0;
   virtual const double  getBarWest(int layer,int no) = 0;
   virtual const double  getEndcap(int layer,int no) = 0;

   virtual const double  BQRaw1(int id, double q) = 0;
   virtual const double  BQRaw2(int id, double q) = 0;
   virtual const double  EQRaw(int id, double q) = 0;
};
 
#endif
