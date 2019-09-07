//====================================================================
//      RawDataCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/MsgStream.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/Event.h"
#include "RawDataCnv/RawDataCnv.h"
#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 

extern const CLID& CLID_Event;  

// Constructor.
RawDataCnv::RawDataCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
    //init();
}

StatusCode RawDataCnv::initialize()
{
    StatusCode sc = RawDataBaseCnv::initialize();
    if( sc.isFailure() ) return sc;
    
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataCnv_Name("RawDataCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //SmartIF<IService> pCnvSvc(conversionSvc());
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    // for Mutil-thread by tianhl
 
    MsgStream log(messageService(), RawDataCnv_Name.c_str());
    /*if (StatusCode::SUCCESS != service("PackedRawDataCnvSvc",
        m_RawDataAccess) || !m_RawDataAccess )
    {
        log << MSG::ERROR << " Can't get RawDataAccess interface "
            << endreq;
        return StatusCode::FAILURE;
    }
    log << MSG::INFO << " RawDataCnvSvc retrieved "<< endreq; */
 
    IService* svc ;
    sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc); 
    if(sc != StatusCode::SUCCESS ) {
        log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
        return sc ;
    }
 
    m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc); 
    if(m_inputSvc == 0 ) {
        log<<MSG::WARNING << " Cant cast to  RawDataInputSvc " <<endreq; 
        return StatusCode::FAILURE ;
    } 

    return StatusCode::SUCCESS;
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataCnv::classID()
{
    return CLID_Event;
}

StatusCode RawDataCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataCnv");
    //log << MSG::DEBUG << "RawDataCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  RawDataAddress *pEFAddr;
  const RAWEVENT *evt;

  Event::EventH * evh = new Event::EventH();
  pObj = evh;

  if ( (pEFAddr = dynamic_cast<RawDataAddress*>(pAddr)) ) {
    evt = m_inputSvc->currentEvent();
    if (evt == NULL) return StatusCode::FAILURE;
    return StatusCode::SUCCESS;
  }
  else {
    return StatusCode::FAILURE;
  }
}

StatusCode RawDataCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  Event::EventH * evh=dynamic_cast<Event::EventH *> (pObj);
  if (!evh) {
    std::string RawDataCnv_Name("RawDataCnv");
    //ConversionSvc*  pCnvSvc = 0;
    //SmartIF<IService> pCnvSvc(conversionSvc());
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        RawDataCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    MsgStream log(msgSvc(), RawDataCnv_Name.c_str());
    log << MSG::DEBUG << "RawDataCnv::createRep()" << endreq;

    log << MSG::ERROR << "Could not downcast Event" << endreq;
    return StatusCode::FAILURE;
  }
  // nothing to do, will be done in lower converters
  return StatusCode::SUCCESS;
}
