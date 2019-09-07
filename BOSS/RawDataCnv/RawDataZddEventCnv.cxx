#include "RawDataCnv/RawDataZddEventCnv.h"
#include "RawDataCnv/RawDataInputSvc.h" 
#include "RawDataCnv/Util/ZddConverter.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h" 
#include "ZddEvent/ZddEvent.h"
#include "GaudiKernel/ThreadGaudi.h"

using Event::ZddEvent;

extern const CLID& CLID_ZddEvent;

RawDataZddEventCnv::RawDataZddEventCnv(ISvcLocator*svc)
    : RawDataBaseCnv(PACKEDRAWEVENT_StorageType, classID(), svc)
{
}

RawDataZddEventCnv::~RawDataZddEventCnv()
{
    ZddConverter::destroy();
}

StatusCode RawDataZddEventCnv::initialize()
{
    StatusCode sc = RawDataBaseCnv::initialize();
    if ( sc.isFailure() ) return sc;

    std::string PackedRawDataCnvSvc_Name("PackedRawDataCnvSvc");
    std::string RawDataInputSvc_Name("RawDataInputSvc");
    std::string RawDataZddEventCnv_Name("RawDataZddEventCnv");

    SmartIF<IService> pCnvSvc(conversionSvc());   
    if  (isGaudiThreaded(pCnvSvc->name())){
	PackedRawDataCnvSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	RawDataInputSvc_Name += getGaudiThreadIDfromName(pCnvSvc->name());
	RawDataZddEventCnv_Name += getGaudiThreadIDfromName(pCnvSvc->name());
    }

    MsgStream log(messageService(), RawDataZddEventCnv_Name.c_str());

    IService* svc ;
    sc = serviceLocator()->getService(RawDataInputSvc_Name.c_str(), svc);
    m_inputSvc = dynamic_cast<RawDataInputSvc*> (svc);
    if ( m_inputSvc == 0 ) {
	log<<MSG::WARNING<< " Cant cast to  RawDataInputSvc " <<endreq;
	return StatusCode::FAILURE ;
    }

    m_cnv = ZddConverter::instance( m_inputSvc->runMode() );

    return StatusCode::SUCCESS; 
}

const CLID& RawDataZddEventCnv::classID()
{
    return CLID_ZddEvent;
}

StatusCode RawDataZddEventCnv::createObj(IOpaqueAddress* /*pAddr*/, DataObject*& pObj)
{
    ZddEvent*  zddEvt = new ZddEvent;
    pObj = zddEvt;

    RAWEVENT* evt = m_inputSvc->currentEvent(); 
    if (evt == NULL) {
	//log << MSG::ERROR << "RawDataZddEventCnv::createObj has no event!" << endreq; 
	return StatusCode::FAILURE;
    }

    const BufferHolder& zddBuf = evt->getZddBuf();

    // Data decoding
    uint32_t nbuf = zddBuf.nBuf();
    for ( uint32_t i = 0; i < nbuf; ++i ) {  //loop the buffers
	uint32_t* buf = zddBuf(i);
	m_cnv->convert(buf, zddBuf.bufSize(i), zddEvt);
    }

    return StatusCode::SUCCESS;
}

StatusCode RawDataZddEventCnv::updateObj(IOpaqueAddress* pAddr, DataObject* pObj)
{
    return Converter::updateObj(pAddr, pObj);
}

StatusCode RawDataZddEventCnv::createRep(DataObject* , IOpaqueAddress*& )
{
    // to be filled
    return StatusCode::SUCCESS;
}
