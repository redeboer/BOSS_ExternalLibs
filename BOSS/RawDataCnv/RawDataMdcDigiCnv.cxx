//====================================================================
//      RawDataMdcDigiCnv.cxx
//====================================================================
//
// Description: A converter class to unpack Event Filter packed raw   
//              event persistent data and place it in the Transient    
//              Data Store of Athena.
//
//--------------------------------------------------------------------

// Include files.
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h" 
#include "GaudiKernel/IJobOptionsSvc.h" 
#include "GaudiKernel/SmartDataPtr.h" 
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "EventModel/EventModel.h"
#include "MdcRawEvent/MdcDigi.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "RawDataCnv/EventManagement/RawEvent.h" 
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/RawDataMdcDigiCnv.h"

// REMOVE THIS LINE AFTER "PACKEDRAWEVENT_StorageType" IS ADDED TO
// THE FILE "GaudiKernel/ClassID.h"
extern const CLID& CLID_MdcDigiCol;

// Constructor.
RawDataMdcDigiCnv::RawDataMdcDigiCnv(ISvcLocator* svc) : 
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc) 
{
   PropertyMgr m_propMgr;
   m_propMgr.declareProperty("ExWireFromRun", m_runFrom = 0 );  
   m_propMgr.declareProperty("ExWireToRun",   m_runTo = 999999);

   IJobOptionsSvc* jobSvc; 
   serviceLocator()->service("JobOptionsSvc", jobSvc);
   jobSvc->setMyProperties("RawDataMdcDigiCnv", &m_propMgr);
}

RawDataMdcDigiCnv::~RawDataMdcDigiCnv()
{
   MdcConverter::destroy();
}

// Return the identification number of this converter to the 
// persistency service.
const CLID& RawDataMdcDigiCnv::classID()
{
   return CLID_MdcDigiCol;
}

StatusCode RawDataMdcDigiCnv::initialize()
{
   std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
   std::string RawDataInputSvc_Name("RawDataInputSvc");
   std::string RawDataMdcDigiCnv_Name("RawDataMdcDigiCnv");

   // for Mutil-thread by tianhl
   //ConversionSvc*  pCnvSvc = 0;
   //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
   SmartIF<IService> pCnvSvc(conversionSvc());   
   if  (isGaudiThreaded(pCnvSvc->name())){
	 PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	 RawDataMdcDigiCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      }
   //}


   StatusCode sc = RawDataBaseCnv::initialize();
   if ( sc.isFailure() ) {
      return sc;
   }

   MsgStream log(messageService(), RawDataMdcDigiCnv_Name.c_str());

   // Check RawDataCnvSvc
   IService* isvc = 0;
   StatusCode status = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), isvc, true);
   m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc); 
   if(m_RawDataAccess  == 0 ) {
      log<<MSG::ERROR<< "  RawDataMdcCnv: Cant cast to  RawDataCnvSvc " <<endreq; 
      return StatusCode::FAILURE ;
   }

   IService* svc ;
   sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
   //if ( sc.isFailure() ) {
   //   log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
   //   return sc ;
   //}
   m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
   if(m_inputSvc == 0 ) {
      log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
      return StatusCode::FAILURE ;
   }

   m_cnv = MdcConverter::instance( m_inputSvc->runMode() );

   m_cnv->init(m_runFrom, m_runTo);

   return StatusCode::SUCCESS; 
}


StatusCode RawDataMdcDigiCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    // Purpose and Method:  This method does nothing other than announce it has
    //   been called.

    //MsgStream log(msgSvc(), "RawDataMdcDigiCnv");
    //log << MSG::DEBUG << "RawDataMdcDigiCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataMdcDigiCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  //MsgStream log(msgSvc(), "RawDataMdcDigiCnv");

  // Purpose and Method:  This converter will create an empty MdcDigiCol on
  //   the TDS.
  MdcDigiCol *digiCol = new MdcDigiCol;
  pObj = digiCol;

  RAWEVENT* evt = m_inputSvc->currentEvent(); 

  if (evt == NULL) {
    //log << MSG::ERROR << "RawDataMdcDigiCnv::createObj has event!" << endreq; 
    return StatusCode::FAILURE;
  }

  int runId = evt->eventHeader().run_number();
  m_cnv->setRunId(runId);

  const BufferHolder& mdcBuf = evt->getMdcBuf();

  return m_cnv->convert(mdcBuf, digiCol);
}

StatusCode RawDataMdcDigiCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)  
{
  // convert PixelRaw in the container into ByteStream
  MsgStream log(messageService(), "RawDataMdcDigiCnv");

  WriteRawEvent*& re = m_RawDataAccess->getRawEvent(); 
  if (re == 0) {
    log << " get RawEvent failed !" << endreq;
    return StatusCode::FAILURE; 
  }

  SmartDataPtr<MdcDigiCol> digiCol(dataProvider(), EventModel::Digi::MdcDigiCol);
  if (digiCol == 0) {
    log << "no MdcDigiCol found" << endreq;
    return StatusCode::FAILURE;
  }
  /*
  else {
    log << MSG::DEBUG << endreq << "Detailed dump of 1st event: " << endreq << endreq;
    int ndigi = 0;
    MdcDigiCol::const_iterator pMdcDigi = digiCol->begin();
    for (pMdcDigi; pMdcDigi!= digiCol->end(); pMdcDigi++) {
      log <<MSG::INFO << "Digi " << ndigi++ << " ";
      // output the digi proper:  " << digi " no longer works because
      // "digi" now returns an integer sort order
      (**pMdcDigi).fillStream(log.stream());
      log << endreq;
    }
  }
  */

  return m_cnv->convert(digiCol, re);
}
