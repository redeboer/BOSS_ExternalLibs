#include "GaudiKernel/MsgStream.h"
#include "TrigEvent/TrigGTD.h"
#include "RawDataCnv/RawDataAddress.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/RawDataTrigGTDCnv.h"

extern const CLID& CLID_TrigGTDCol;

// Constructor
RawDataTrigGTDCnv::RawDataTrigGTDCnv(ISvcLocator* svc) :
RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc)
{
  init();
}

// Return the identification number of this converter to the persistency service.
const CLID& RawDataTrigGTDCnv::classID()
{
  return CLID_TrigGTDCol;
}

StatusCode RawDataTrigGTDCnv::init()
{
  StatusCode sc = RawDataBaseCnv::initialize();
  if (sc.isFailure()) {
    return sc;
  }

  MsgStream log(messageService(), "RawDataTrigGTDCnv");

  IService* isvc = 0;
  StatusCode status = serviceLocator()->service("PackedRawDataCnvSvc", isvc, true);

  m_RawDataAccess = dynamic_cast<PackedRawDataCnvSvc*> (isvc);
  if (m_RawDataAccess == 0) {
    log << MSG::ERROR << "RawDataTrigGTDCnv: Cant cast to RawDataCnvSvc" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_RawDataAccess) log << MSG::INFO << "RawDataCnvSvc retrieved" << endreq;

  IService* svc;
  sc = serviceLocator()->getService("RawDataInputSvc", svc);
  if (sc.isFailure()) {
    log << MSG::WARNING << "Cant get RawDataInputSvc" << endreq;
    return sc;
  }

  m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
  if (m_inputSvc == 0) {
    log << MSG::WARNING << "Cant cast to RawDataInputSvc" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode RawDataTrigGTDCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj) {
  MsgStream log(msgSvc(), "RawDataTrigGTDCnv");
  log << MSG::DEBUG << "RawDataTrigGTDCnv::updateObj" << endreq;
  return Converter::updateObj(pAddr, pObj);
}

// Create a converted object in the Transient Data Store.
StatusCode RawDataTrigGTDCnv::createObj(IOpaqueAddress* pAddr, DataObject*& pObj)
{
  MsgStream log(msgSvc(), "RawDataTrigGTDCnv");
  RawDataAddress* pEFAddr;

  RAWEVENT* evt;
  TrigGTD*  trigGTD;

  TrigGTDCol* gtdCol = new TrigGTDCol;
  pObj = gtdCol;

  evt = m_inputSvc->currentEvent();
  if (evt == NULL) {
    log << MSG::ERROR << "RawDataTrigGTDCnv::createObj has no event!" << endreq;
    return StatusCode::FAILURE;
  }

  const BufferHolder& gtdBuf = evt->getGTDBuf();
  uint32_t nbuf = gtdBuf.nBuf();

  for (uint32_t i = 0; i < nbuf; i++) {
    uint32_t* buf = gtdBuf(i);
    uint32_t bufSize = gtdBuf.bufSize(i);
    uint32_t index = 0;
    while (bufSize - index > 1) {
      uint32_t blockSize = ( ((*(buf+index))>>14) & 0x3FF);
      uint32_t id = ((*(buf+index))>>24);
      if (blockSize == 0 || (index+blockSize) > bufSize) break;
      if ((id> 0xD1 && id < 0xD8 && id != 0xD5) || id == 0xDA || (id > 0xE1 && id < 0xED)) {
	trigGTD = new TrigGTD(buf+index);
	gtdCol->push_back(trigGTD);
      }
      index += blockSize;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode RawDataTrigGTDCnv::createRep(DataObject* pObj, IOpaqueAddress*& pAddr)
{
  MsgStream log(messageService(), "RawDataTrigGTDCnv");

  return StatusCode::SUCCESS;
}
