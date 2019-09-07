// this is -*- c++ -*-
// $Id: BesTimerSvc.h,v 1.4 2011/02/18 04:23:58 maqm Exp $
// $Name: BesTimerSvc-00-00-12 $
#ifndef BesTIMERSVC_H_
#define BesTIMERSVC_H_
#include <vector>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IInterface.h"
#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimer.h"

//extern const IID & IID_IBesTimerSvc;

class BesTimerSvc : public Service, virtual public IBesTimerSvc {
public:

  BesTimerSvc( const std::string& name, ISvcLocator* svcloc );
  ~BesTimerSvc();

  virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown);
  virtual StatusCode initialize ( );
  virtual StatusCode finalize ( );
  virtual BesTimer* addItem(const std::string &name);
  virtual std::vector < BesTimer* >::iterator begin( void );
  virtual std::vector < BesTimer* >::iterator end( void );
  virtual void print( );

  //  inline BesTimer* BesTimerSvc::findItem(const std::string& name);
  
private:
  std::vector< BesTimer* > m_itemList;

};


#endif // BesTIMER_H
