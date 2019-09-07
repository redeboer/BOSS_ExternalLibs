//====================================================================
//      RawDataTofDigiCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "LumiDigi/LumiDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataTofDigiCnv.h"


extern const CLID& CLID_TofDigiCol;

// Constructor.
RawDataTofDigiCnv::RawDataTofDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
}

RawDataTofDigiCnv::~RawDataTofDigiCnv()
{
   TofConverter::destroy();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataTofDigiCnv::classID()
{
   return CLID_TofDigiCol;
}

StatusCode RawDataTofDigiCnv::initialize()
{
   std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
   std::string RawDataInputSvc_Name("RawDataInputSvc");
   std::string RawDataTofDigiCnv_Name("RawDataTofDigiCnv");

   // for Mutil-thread by tianhl
   //ConversionSvc*  pCnvSvc = 0;
   //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
   SmartIF<IService> pCnvSvc(conversionSvc());
      if  (isGaudiThreaded(pCnvSvc->name())){
	 PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataTofDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
      //}

   StatusCode sc = RawDataBaseCnv::initialize();
   if ( sc.isFailure() ) {
      return sc;
   }

   MsgStream log(messageService(), RawDataTofDigiCnv_Name.c_str());

   // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);
   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
   if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  RawDataTofCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
      return StatusCode::FAILURE ;
   }

   IService* svc ;
   sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
   //if ( sc.isFailure() ) {
   //   log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
   //   return sc ;
   //}
   m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
   if ( m_inputSvc == 0 ) {
      log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
      return StatusCode::FAILURE ;
   }

   m_cnv = TofConverter::instance();

   return StatusCode::SUCCESS; 
}


StatusCode RawDataTofDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataTofDigiCnv");
    //log << MSG::DEBUG << "RawDataTofDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataTofDigiCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   MsgStream log(msgSvc(), "RawDataTofDigiCnv");

   uint32_t TEId, TEData, TEOverflow, TETorQ;
   TofDigi* tofDigi;
   // Purpose and Method:  This converter will create an empty TofDigiCol on
   //   the TDS.
   TofDigiCol *digiCol = new TofDigiCol;
   pObj = digiCol;

   //zoujh: for luminosity begin ...................
   LumiDigiCol *lumiDigiCol = 0;
   DataObject* pLumi;
   dataProvider()->findObject("/Event/Digi/LumiDigiCol", pLumi);
   if ( pLumi == NULL ) {
      lumiDigiCol = new LumiDigiCol;
      StatusCode sc = dataProvider()->registerObject("/Event/Digi/LumiDigiCol", lumiDigiCol);
      if ( sc.isFailure() ) {
	 log << MSG::ERROR << "Could not register LumiDigiCol in TDS!" << endreq;
      }
   }
   else {
      lumiDigiCol = dynamic_cast<LumiDigiCol*>(pLumi);
   }
   //zoujh: for luminosity end ^^^^^^^^^^^^^^^^^^^^^
    
   RAWEVENT* evt = m_inputSvc->currentEvent(); 
   if (evt == NULL) {
     cout << "RawDataTofDigiCnv::createObj has no event!" << endl; 
     return StatusCode::FAILURE;
   }

   const BufferHolder& tofBuf = evt->getTofBuf();
   const BufferHolder& etfBuf = evt->getEtfBuf();

   return m_cnv->convert(tofBuf, etfBuf, digiCol, lumiDigiCol);
}

StatusCode RawDataTofDigiCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
  // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataTofDigiCnv");

  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  if (re == 0) {
    //log << " get RawEvent failed !" << endreq;
    return StatusCode::FAILURE; 
  }

  SmartDataPtr<TofDigiCol> digiCol(dataProvider(), EventModel::Digi::TofDigiCol);
  if (digiCol == 0) {
    //log << "no TofDigiCol found" << endreq;
    return StatusCode::FAILURE;
  }
  /*
  else {
    log << MSG::INFO << endreq << "Detailed dump of 1st event: " << endreq << endreq;
    int ndigi = 0;
    TofDigiCol::const_iterator pTofDigi = digiCol->begin();
    for (pTofDigi; pTofDigi!= digiCol->end(); pTofDigi++) {
      log <<MSG::INFO << "Digi " << ndigi++ << " ";
      // output the digi proper:  " << digi " no longer works because
      // "digi" now returns an integer sort order
      (**pTofDigi).fillStream(log.stream());
      log << endreq;
    }
  }
  */

  return m_cnv->convert(digiCol, re);
}
