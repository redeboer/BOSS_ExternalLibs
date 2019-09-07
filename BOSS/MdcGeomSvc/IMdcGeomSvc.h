#ifndef IMDCGEOMSVC_H_
#define IMDCGEOMSVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
  
// Declaration of the interface ID 
//static const InterfaceID  IID_IMdcGeomSvc = 1059;
 static const InterfaceID IID_IMdcGeomSvc("IMdcGeomSvc",1,0);

class MdcGeoWire;
class MdcGeoLayer;
class MdcGeoSuper;
class MdcGeoGeneral;
  
class IMdcGeomSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_IMdcGeomSvc; } 
   virtual const MdcGeoWire * const Wire(unsigned id) = 0;
   virtual const MdcGeoWire * const Wire(unsigned lyrid, unsigned wirid) = 0;         
   virtual const MdcGeoLayer * const Layer(unsigned id) = 0;
   virtual const MdcGeoSuper * const SuperLayer(unsigned id) = 0;
   virtual const MdcGeoGeneral * const GeneralLayer(unsigned id) = 0;
   
   virtual const int getWireSize() = 0;
   virtual const int getLayerSize() = 0;
   virtual const int getSuperLayerSize() = 0;
   virtual const int getGeneralLayerSize() = 0; 
   virtual void Dump() = 0;
};
 
#endif // IMDCGEOMSVC_H
