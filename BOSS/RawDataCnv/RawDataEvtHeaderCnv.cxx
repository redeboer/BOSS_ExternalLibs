#include "GaudiKernel/MsgStream.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/PackedRawDataCnvSvc.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "EventModel/EventHeader.h"
#include "RawDataCnv/RawDataEvtHeaderCnv.h"

extern const CLID& CLID_EventHeader;

RawDataEvtHeaderCnv::RawDataEvtHeaderCnv(ISvcLocator* svc) :
  RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc)
{
  // for Mutil-thread by tianhl
  //init();
  // for Mutil-thread by tianhl
}

StatusCode RawDataEvtHeaderCnv::initialize()
{
  StatusCode sc = RawDataBaseCnv::initialize();
  if ( sc.isFailure() ) return sc;

  std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
  std::string RawDataInputSvc_Name("RawDataInputSvc");
  std::string RawDataEvtHeaderCnv_Name("RawDataEvtHeaderCnv");
  
  // for Mutil-thread by tianhl
  //ConversionSvc*  pCnvSvc = 0;
  //if (pCnvSvc = dynamic_cast<ConversionSvc*>(conversionSvc())){
  SmartIF<IService> pCnvSvc(conversionSvc());  
  if  (isGaudiThreaded(pCnvSvc->name())){
      PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
      RawDataEvtHeaderCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
    }
  //}
  // for Mutil-thread by tianhl
  MsgStream log(messageService(), RawDataEvtHeaderCnv_Name.c_str());

  IService* svc = 0;
  sc = serviceLocator()->service(PackedRawDataCnvSvc_Name.c_str(), svc);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Can't get RawDataAccess interface" << endreq;
    return StatusCode::FAILURE;
  }

  m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (svc);
  if (m_RawDataAccess  == 0 ) {
    log << MSG::ERROR << "RawDataEvtHeaderCnv: Cant cast to RawDataCnvSvc" << endreq;
    return StatusCode::FAILURE;
  }

  sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
  if ( sc.isFailure() ) {
    log << MSG::WARNING << "Cant get RawDataInputSvc " << endreq;
    return sc ;
  }

  m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
  if ( m_inputSvc == 0 ) {
    log << MSG::ERROR << "Cant cast to RawDataInputSvc" << endreq;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS;
}

const CLID& RawDataEvtHeaderCnv::classID()
{
  return CLID_EventHeader;
}

StatusCode RawDataEvtHeaderCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  //MsgStream log(msgSvc(), "RawDataEvtHeaderCnv");
  //log << MSG::DEBUG << "RawDataEvtHeaderCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataEvtHeaderCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
   StatusCode sc;
   RawDataAddress *pEFAddr;
   const RAWEVENT *evt;

   Event::EventHeader *header = new Event::EventHeader();
   pObj = header;

   //if ( (pEFAddr = dynamic_cast<RawDataAddress*>(pAddr)) ) {
     evt = m_inputSvc->currentEvent();
     
     if (evt == NULL) return StatusCode::FAILURE;

     header->setEventNumber(evt->eventHeader().event_number());
     header->setRunNumber(evt->eventHeader().run_number());
     header->setTime(evt->eventHeader().time());
     header->setFlag1( evt->eventHeader().flag1() );
     //header->setFlag2( evt->eventHeader().flag2() );

     
   return StatusCode::SUCCESS;
   //}
   //else {
   //  return StatusCode::FAILURE;
   //}

}
