//====================================================================
//      RawDataEmcDigiCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/MsgStream.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl
 
#include "EventModel/EventModel.h"
#include "EmcRawEvent/EmcDigi.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/PackedRawDataCnvSvc.h" 
#include "RawDataCnv/Util/EmcConverter.h"
#include "RawDataCnv/RawDataEmcDigiCnv.h"


extern const CLID& CLID_EmcDigiCol;

// Constructor.
RawDataEmcDigiCnv::RawDataEmcDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
}
RawDataEmcDigiCnv::~RawDataEmcDigiCnv(){
   EmcConverter::destroy();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataEmcDigiCnv::classID()
{
   return CLID_EmcDigiCol;
}

StatusCode RawDataEmcDigiCnv::initialize()
{
   std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
   std::string RawDataInputSvc_Name("RawDataInputSvc");
   std::string RawDataEmcDigiCnv_Name("RawDataEmcDigiCnv");
   //std::string RawDataProviderSvc_Name("RawDataProviderSvc");

   // for Mutil-thread by tianhl
   //ConversionSvc*  pCnvSvc = 0;
   SmartIF<IService> pCnvSvc(conversionSvc());
   //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
      if  (isGaudiThreaded(pCnvSvc->name())){
	 PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataEmcDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 //RawDataProviderSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
   //}
   // for Mutil-thread by tianhl
   MsgStream log(msgSvc(), RawDataEmcDigiCnv_Name.c_str());

   StatusCode sc = RawDataBaseCnv::initialize();
   if ( sc.isFailure() ) {
      return sc;
   }

   // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);
   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
   if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  RawDataCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
      return StatusCode::FAILURE ;
   }

   IService* svc ;
   sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
   //if(sc != StatusCode::SUCCESS ) {
   //   log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
   //   return sc ;
   //}
   m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
   if ( m_inputSvc == 0 ) {
      log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
      return StatusCode::FAILURE ;
   }

   m_cnv = EmcConverter::instance( m_inputSvc->runMode() );

   return StatusCode::SUCCESS; 
}


StatusCode RawDataEmcDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    MsgStream log(msgSvc(), "RawDataEmcDigiCnv");
    log << MSG::DEBUG << "RawDataEmcDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataEmcDigiCnv::createObj(IOpaqueAddress*, DataObject*& pObj)
{
   MsgStream log(msgSvc(), "RawDataEmcDigiCnv");

   // Purpose and Method:  This converter will create an empty EmcDigiCol on
   //   the TDS.
   EmcDigiCol *digiCol = new EmcDigiCol;
   pObj = digiCol;

   RAWEVENT *evt = m_inputSvc->currentEvent(); 
   if (evt == NULL) {
     cout << "RawDataEmcDigiCnv::createObj can't get event!" << endl; 
     return StatusCode::FAILURE;
   }

   const BufferHolder& emcBuf = evt->getEmcBuf();

   return m_cnv->convert(emcBuf, digiCol); 
}

StatusCode RawDataEmcDigiCnv::createRep(DataObject*, IOpaqueAddress*&)  
{
  // convert PixelRaw in the container into ByteStream
  MsgStream log(msgSvc(), "RawDataEmcDigiCnv");

  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  if (re == 0) {
    log << " get RawEvent failed !" << endreq;
    return StatusCode::FAILURE; 
  }

  SmartDataPtr<EmcDigiCol> digiCol(dataProvider(), EventModel::Digi::EmcDigiCol);
  if (digiCol == 0) {
    log << "no EmcDigiCol found" << endreq;
    return StatusCode::FAILURE;
  }
  /*
  else {
    log << MSG::INFO << endreq << "Detailed dump of 1st event: " << endreq << endreq;
    int ndigi = 0;
    EmcDigiCol::const_iterator pEmcDigi = digiCol->begin();
    for (pEmcDigi; pEmcDigi!= digiCol->end(); pEmcDigi++) {
      log <<MSG::INFO << "Digi " << ndigi++ << " ";
      // output the digi proper:  " << digi " no longer works because
      // "digi" now returns an integer sort order
      (**pEmcDigi).fillStream(log.stream());
      log << endreq;
    }
  }
  */

  return m_cnv->convert(digiCol, re);
}
