//====================================================================
//      RawDataHltCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "HltEvent/HltEvent.h"
#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/RawDataInputSvc.h"
//#include "RawDataCnv/EventManagement/RawEvent.h"
#include "RawDataCnv/RawDataHltCnv.h"


extern const CLID& CLID_HltEvent;  

// Constructor.
RawDataHltCnv::RawDataHltCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
    //init();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataHltCnv::classID()
{
  return CLID_HltEvent;
}

StatusCode RawDataHltCnv::initialize() 
{
    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataHltCnv_Name("RawDataHltCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataHltCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    StatusCode sc = RawDataBaseCnv::initialize();
    if(StatusCode::SUCCESS!=sc)
    {
       return sc;
    }

    MsgStream log(messageService(), RawDataHltCnv_Name.c_str());

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


StatusCode RawDataHltCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataHltCnv");
    //log << MSG::DEBUG << "RawDataHltCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataHltCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   //MsgStream log(msgSvc(), "RawDataHltCnv");
   //RawDataAddress *pEFAddr;
   
   // Purpose and Method:  This converter will create an empty EventHeader on
   //   the TDS.
   HltEvent *digi = new HltEvent();
   // Set fromMc to false
   digi->initialize(false);
   pObj = digi;
   return StatusCode::SUCCESS;

}

StatusCode RawDataHltCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)        
{
    // convert Raws in the container into ByteStream
    StatusCode sc = StatusCode::SUCCESS ; 
    //MsgStream log(messageService(), "RawDataHltCnv");
 
    //WriteRawEvent* re = static_cast<WriteRawEvent*>(m_RawDataAccess->getRawEvent()); 
 
 
    std::string nm = pObj->registry()->name(); 
 
    RawDataAddress* addr = new RawDataAddress(classID(), nm, ""); 
 
    pAddr = addr; 
 
    return StatusCode::SUCCESS; 
 
}


