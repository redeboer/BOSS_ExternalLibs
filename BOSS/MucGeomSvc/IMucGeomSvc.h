/*
 *    2004/09/10   Zhengyun You     Peking University
 *                 
 */

#ifndef IMUC_GEOM_SVC_H_
#define IMUC_GEOM_SVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
  
// Declaration of the interface ID 
static const InterfaceID  IID_IMucGeomSvc = 3401;

class IMucGeomSvc : virtual public IInterface  {  
public: 
   static const InterfaceID& interfaceID() { return IID_IMucGeomSvc; } 
   virtual void Dump() = 0;
};
 
#endif // IMUC_GEOM_SVC_H
