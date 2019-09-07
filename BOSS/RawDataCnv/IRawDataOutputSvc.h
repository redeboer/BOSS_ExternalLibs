#ifndef IRawDataOutputSvc_H
#define IRawDataOutputSvc_H 

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Service.h" 

/** @class IRawDataOutputSvc
* @brief The event conversion service needs an additional interface used 
* by the converters to declare their object path.
*
*/

#include "RawDataCnv/EventManagement/RawEvent.h"

class IRawDataOutputSvc : public Service  
{

public:
  IRawDataOutputSvc(const std::string& name, ISvcLocator* svcloc);
  virtual ~IRawDataOutputSvc(void);
  virtual StatusCode initialize();  
  virtual bool putEvent(WriteRawEvent* re) = 0;  
};


#endif // IRawDataOutputSvc_H


