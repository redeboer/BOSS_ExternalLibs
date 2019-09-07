// Include files.
#include <string>
#include "RawDataCnv/IRawDataOutputSvc.h"
 
// Constructor.
IRawDataOutputSvc::IRawDataOutputSvc(const std::string& name, ISvcLocator* svcloc)
   : Service(name,svcloc )
{
}

// Destructor.
IRawDataOutputSvc::~IRawDataOutputSvc()
{}
 
StatusCode IRawDataOutputSvc::initialize()
{
  StatusCode sc=  Service::initialize();
  return sc; 
}

