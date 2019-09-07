//====================================================================
//      RawDataDigiCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/Converter.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "RawEvent/DigiEvent.h"
#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/PackedRawDataCnvSvc.h"  
#include "RawDataCnv/EventManagement/RawEvent.h"
#include "RawDataCnv/RawDataDigiCnv.h"


extern const CLID& CLID_DigiEvent;  

// Constructor.
RawDataDigiCnv::RawDataDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
    //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataDigiCnv::classID()
{
  return CLID_DigiEvent;
}

StatusCode RawDataDigiCnv::initialize() 
{
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataDigiCnv_Name("RawDataDigiCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    SmartIF<IService> pCnvSvc(conversionSvc());
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
      if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    // for Mutil-thread by tianhl
    StatusCode sc = RawDataBaseCnv::initialize();
    if(StatusCode::SUCCESS!=sc)
    {
       return sc;
    }

    MsgStream log(messageService(), RawDataDigiCnv_Name.c_str());

    IService* svc = 0;
    sc = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), svc);
    if (sc != StatusCode::SUCCESS) {
        log << MSG::ERROR << "Can't get RawDataAccess interface" << endreq;
        return StatusCode::FAILURE;
    }

    m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (svc);
    if (m_RawDataAccess  == 0 ) {
      log << MSG::ERROR << "RawDataCnv: Cant cast to RawDataCnvSvc" << endreq;
      return StatusCode::FAILURE;
    }

    sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
    if(sc != StatusCode::SUCCESS ) {
        log << MSG::WARNING << "Cant get RawDataInputSvc " << endreq;
        return sc ;
    }
 
    m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
    if(m_inputSvc == 0 ) {
        log << MSG::ERROR << "Cant cast to RawDataInputSvc" << endreq;
        return StatusCode::FAILURE ;
    }

    return StatusCode::SUCCESS;
}


StatusCode RawDataDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataDigiCnv");
    //log << MSG::DEBUG << "RawDataDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataDigiCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   MsgStream log(msgSvc(), "RawDataDigiCnv");
   //RawDataAddress *pEFAddr;
   
   // Purpose and Method:  This converter will create an empty EventHeader on
   //   the TDS.
   DigiEvent *digi = new DigiEvent();
   // Set fromMc to false
   digi->initialize(false);
   pObj = digi;
   return StatusCode::SUCCESS;

}

StatusCode RawDataDigiCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)        
{
    // convert Raws in the container into ByteStream
    StatusCode sc = StatusCode::SUCCESS ; 
    //MsgStream log(messageService(), "RawDataDigiCnv");
 
    //WriteRawEvent* re = static_cast<WriteRawEvent*>(m_RawDataAccess->getRawEvent()); 
 
 
    std::string nm = pObj->registry()->name(); 
 
    RawDataAddress* addr = new RawDataAddress(classID(), nm, ""); 
 
    pAddr = addr; 
 
    return StatusCode::SUCCESS; 
 
}


