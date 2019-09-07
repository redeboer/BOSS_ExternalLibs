#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "TrigEvent/TrigEvent.h"
#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/RawDataTrigCnv.h"


extern const CLID& CLID_TrigEvent;

// Constructor.
RawDataTrigCnv::RawDataTrigCnv(ISvcLocator* svc) :
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc)
{
  init();
}

const CLID& RawDataTrigCnv::classID()
{
  return CLID_TrigEvent;
}

StatusCode RawDataTrigCnv::init() 
{
    StatusCode sc = RawDataBaseCnv::initialize();
    if (sc.isFailure()) {
       return sc;
    }

    MsgStream log(messageService(), "RawDataTrigCnv");

    IService* svc = 0;
    sc = serviceLocator()->service("PackedRawDataCnvSvc", svc);
    if (sc != StatusCode::SUCCESS) {
        log << MSG::ERROR << "Can't get RawDataAccess interface" << endreq;
        return StatusCode::FAILURE;
    }

    m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (svc);
    if (m_RawDataAccess  == 0 ) {
      log << MSG::ERROR << "RawDataCnv: Cant cast to RawDataCnvSvc" << endreq;
      return StatusCode::FAILURE;
    }

    sc = serviceLocator()->getService("RawDataInputSvc", svc);
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

StatusCode RawDataTrigCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
    MsgStream log(msgSvc(), "RawDataTrigCnv");
    log << MSG::DEBUG << "RawDataTrigCnv::updateObj" << endreq;
    return Converter::updateObj(pAddr, pObj);
}

StatusCode RawDataTrigCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  TrigEvent * evt = new TrigEvent();
  pObj = evt;
  return StatusCode::SUCCESS;
}

StatusCode RawDataTrigCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)        
{
    std::string nm = pObj->registry()->name(); 
    RawDataAddress* addr = new RawDataAddress(classID(), nm, ""); 
    pAddr = addr; 
 
  return StatusCode::SUCCESS;
}
