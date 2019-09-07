//====================================================================
//      RawDataMucDigiCnv.cxx
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
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "MucRawEvent/MucDigi.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataMucDigiCnv.h"



extern const CLID& CLID_MucDigiCol;

// Constructor.
RawDataMucDigiCnv::RawDataMucDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
}

RawDataMucDigiCnv::~RawDataMucDigiCnv()
{
   MucConverter::destroy();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataMucDigiCnv::classID()
{
   return CLID_MucDigiCol;
}

StatusCode RawDataMucDigiCnv::initialize()
{
   std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
   std::string RawDataInputSvc_Name("RawDataInputSvc");
   std::string RawDataMucDigiCnv_Name("RawDataMucDigiCnv");

   // for Mutil-thread by tianhl
   //ConversionSvc*  pCnvSvc = 0;
   //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
   SmartIF<IService> pCnvSvc(conversionSvc());   
   if  (isGaudiThreaded(pCnvSvc->name())){
	 PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataMucDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
   //}

   MsgStream log(messageService(), RawDataMucDigiCnv_Name.c_str());

   StatusCode sc = RawDataBaseCnv::initialize();
   if ( sc.isFailure() ) {
      return sc;
   }

   // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);
   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
   if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  MucRawDataCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
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
      log<< MSG::WARNING << " Cant cast to  RawDataInputSvc " <<endreq;
      return StatusCode::FAILURE ;
   }

   m_cnv = MucConverter::instance();

   return StatusCode::SUCCESS; 
}


StatusCode RawDataMucDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataMucDigiCnv");
    //log << MSG::DEBUG << "RawDataMucDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataMucDigiCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //MsgStream log(msgSvc(), "RawDataMucDigiCnv");
  // Purpose and Method:  This converter will create an empty MucDigiCol on
  //   the TDS.
  MucDigiCol *digiCol = new MucDigiCol;
  pObj = digiCol;

  RAWEVENT* evt = m_inputSvc->currentEvent(); 
  if (evt == NULL) {
    cout << "RawDataMucDigiCnv::createObj has event!" << endl; 
    return StatusCode::FAILURE;
  }

  const BufferHolder& mucBuf = evt->getMucBuf();

  return m_cnv->convert(mucBuf, digiCol); 
}

StatusCode RawDataMucDigiCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
  // convert PixelRaw in the container into ByteStream
  //MsgStream log(messageService(), "RawDataMucDigiCnv");

  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  if (re == 0) {
    //log << " get RawEvent failed !" << endreq;
    return StatusCode::FAILURE; 
  }

  SmartDataPtr<MucDigiCol> digiCol(dataProvider(), EventModel::Digi::MucDigiCol);
  if (digiCol == 0) {
    //log << "no MucDigiCol found" << endreq;
    return StatusCode::FAILURE;
  }
  /*
  else {
    log << MSG::INFO << endreq << "Detailed dump of 1st event: " << endreq << endreq;
    int ndigi = 0;
    MucDigiCol::const_iterator pMucDigi = digiCol->begin();
    for (pMucDigi; pMucDigi!= digiCol->end(); pMucDigi++) {
      log <<MSG::INFO << "Digi " << ndigi++ << " ";
      // output the digi proper:  " << digi " no longer works because
      // "digi" now returns an integer sort order
      (**pMucDigi).fillStream(log.stream());
      log << endreq;
    }
  }
  */

  return m_cnv->convert(digiCol, re);
}
