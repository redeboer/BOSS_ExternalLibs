//====================================================================
//	McCnvSvc.cxx
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//

// Include files
#include <iostream>
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IConversionSvc.h"

#include "McEventSelector/McCnvSvc.h"
#include "McEventSelector/McEventCnv.h"
#include "McEventSelector/McAddress.h"
#include "McEventSelector/McEventSelector.h"

//External definitions

// Instantiation of a static factory class used by clients to create
// instances of this service
//static SvcFactory<McCnvSvc> s_factory;
//const ISvcFactory& McCnvSvcFactory = s_factory;

/// Standard constructor
McCnvSvc::McCnvSvc(const std::string& name, ISvcLocator* svc)
: ConversionSvc( name, svc, storageType())
{
}

/// Standard Destructor
McCnvSvc::~McCnvSvc()   {
}

/// Initialize the service.
StatusCode McCnvSvc::initialize()     {
   MsgStream log(msgSvc(), name());
  StatusCode status = ConversionSvc::initialize();
  return status;
}

StatusCode McCnvSvc::finalize()     {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "RootCnvSvc::finalize" << endreq;

  ConversionSvc::finalize();
 
  return StatusCode::SUCCESS;
}

StatusCode McCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)  {

  if ( IID_IMcCnvSvc == riid )  {
    *ppvInterface = (McCnvSvc*)this;
  }
  else  {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
            }
  addRef();
  return StatusCode::SUCCESS;
}

/// Update state of the service
StatusCode McCnvSvc::updateServiceState(IOpaqueAddress* pAddress)    {
  MsgStream log(messageService(), name());
  static bool first = true;
  //  static int fid   = 0;
  //  static int recid = 0;
  if ( 0 != pAddress )    {
    GenericAddress* pAddr = dynamic_cast<GenericAddress*>(pAddress);
    if ( 0 != pAddr )    {
      if ( first )    {
        first = false;
      }
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode::FAILURE;
}

unsigned char  McCnvSvc::storageType() {
  static unsigned char type=0x10;
  return type;
}
