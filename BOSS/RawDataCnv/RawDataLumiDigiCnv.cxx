// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "LumiDigi/LumiDigi.h"
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataLumiDigiCnv.h"


// REMOVE THIS LINE AFTER "PACKEDRAWEVENT_StorageType" IS ADDED TO
// THE FILE "GaudiKernel/ClassID.h"
extern const CLID& CLID_LumiDigiCol;

// Constructor.
RawDataLumiDigiCnv::RawDataLumiDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
}

RawDataLumiDigiCnv::~RawDataLumiDigiCnv()
{
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataLumiDigiCnv::classID()
{
   return CLID_LumiDigiCol;
}

StatusCode RawDataLumiDigiCnv::initialize()
{

    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataLumiDigiCnv_Name("RawDataLumiDigiCnv");
    
    // for Mutil-thread by tianhl
    //ConversionSvc*  pCnvSvc = 0;
    //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
    SmartIF<IService> pCnvSvc(conversionSvc());  
    if  (isGaudiThreaded(pCnvSvc->name())){
        PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
        RawDataLumiDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
    //}
    
    StatusCode sc = RawDataBaseCnv::initialize();
    if(StatusCode::SUCCESS!=sc)
    {
       return sc;
    }

    MsgStream log(messageService(), RawDataLumiDigiCnv_Name.c_str());

    // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);

   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
    if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  RawDataLumiCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
      return StatusCode::FAILURE ;
   }

   if(m_RawDataAccess)
        log << MSG::INFO << "RawDataCnvSvc retrieved "<< endreq;

   IService* svc ;
   sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
   if(sc != StatusCode::SUCCESS ) {
       log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
       return sc ;
   }
 
   m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
   if(m_inputSvc == 0 ) {
       log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
       return StatusCode::FAILURE ;
   }
 
   return StatusCode::SUCCESS; 
}


StatusCode RawDataLumiDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataLumiDigiCnv");
    //log << MSG::DEBUG << "RawDataLumiDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataLumiDigiCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   DataObject* pLumi;
   dataProvider()->findObject("/Event/Digi/LumiDigiCol", pLumi);

   if ( pLumi == NULL ) {
      LumiDigiCol* lumiDigiCol = new LumiDigiCol;
      dataProvider()->registerObject("/Event/Digi/LumiDigiCol", lumiDigiCol);
      pObj = lumiDigiCol;
   }
   else {
      pObj = pLumi;
   }

   return StatusCode::SUCCESS;
}

StatusCode RawDataLumiDigiCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
  return StatusCode::SUCCESS;
}
