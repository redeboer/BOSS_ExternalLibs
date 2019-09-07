// this service is used for TOF QTC correction
// Sun Shengsen 2009-9-17
//
#ifndef ITOFQELECSVC_H_
#define ITOFQELECSVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"

// Declaration of the interface ID 
static const InterfaceID  IID_ITofQElecSvc("ITofQElecSvc",1,0);

class ITofQElecSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_ITofQElecSvc; }
   virtual const unsigned int Board(bool barrel, int id, bool eastEnd) = 0;
   virtual const unsigned int Crate(bool barrel, int id, bool eastEnd) = 0;
   virtual const unsigned int Fee(bool barrel, int id, bool eastEnd) = 0;
   virtual const unsigned int Channel(bool barrel, int id, bool eastEnd) = 0;
   virtual const double BQTC1(int id, double q) = 0;
   virtual const double BQTC2(int id, double q) = 0;
   virtual const double EQTC(int id, double q) = 0;
   virtual const double BQChannel1(int id, double q) = 0;
   virtual const double BQChannel2(int id, double q) = 0;
   virtual const double EQChannel(int id, double q) = 0;
};
 
#endif // ITOFQELECSVC_H
