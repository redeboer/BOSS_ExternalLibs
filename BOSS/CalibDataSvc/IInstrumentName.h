#ifndef CALIBDATASVC_IINSTRUMENTNAME_H
#define CALIBDATASVC_IINSTRUMENTNAME_H

#include "GaudiKernel/IInterface.h"
#include <string>

/**  @class IInstrumentName
     @brief Simple interface to keep track of which instrument (LAT,
     EM, etc.) the process is concerned with.  Modeled after
     IDetDataSvc handling of event time.

     Intention is to implement in CalibDataSvc.
*/
static const InterfaceID IID_IInstrumentName("IInstrumentName", 1, 0);

class IInstrumentName : virtual public IInterface {
public:

  static const InterfaceID& interfaceID() { return IID_IInstrumentName; }

  virtual const bool validInstrumentName() const = 0;
  virtual const std::string& getInstrumentName() const = 0;
  virtual void setInstrumentName(const std::string& name) = 0;
  //get the CalibType from  CalibDataSvc, either  MYSQL_StorageType or CALIBROOT_StorageType;
  virtual int  getsvctype() = 0;

};

#endif
