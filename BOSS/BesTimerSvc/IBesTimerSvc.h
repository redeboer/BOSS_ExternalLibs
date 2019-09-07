// this is -*- c++ -*-
// $Id: IBesTimerSvc.h,v 1.4 2011/02/18 04:23:58 maqm Exp $
// $Name: BesTimerSvc-00-00-12 $
#ifndef IBesTIMERSVC_H_
#define IBesTIMERSVC_H_
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/Kernel.h"
#include <iostream>
#include <string>

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID  IID_IBesTimerSvc("IBesTimerSvc",1,3);

class BesTimer;

class IBesTimerSvc : virtual public IInterface  {  

public:

  static const InterfaceID& interfaceID() { return IID_IBesTimerSvc; }

  virtual BesTimer* addItem(const std::string& name)=0;
  virtual std::vector < BesTimer* >::iterator begin( void )=0;
  virtual std::vector < BesTimer* >::iterator end( void )=0;
  virtual void print( void )=0;
};


#endif // IBesTIMERSVC_H
