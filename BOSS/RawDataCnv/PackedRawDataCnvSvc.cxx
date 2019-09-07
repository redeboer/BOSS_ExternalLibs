//====================================================================
//	PackedRawDataCnvSvc.cpp
//--------------------------------------------------------------------
//
//	Package    : AsciiFile Conversion Service 
//
//====================================================================

static const char* rcsid = "$Id: PackedRawDataCnvSvc.cxx,v 1.24 2012/12/16 07:23:10 zoujh Exp $";

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
#include "RawDataCnvBase/IRawDataCnvSvc.h"

#include "RawDataCnv/ClassID_temp.h"
#include "RawDataCnv/IRawDataOutputSvc.h"
#include "RawDataCnv/RawDataCnv.h"
#include "RawDataCnv/RawDataEvtHeaderCnv.h"
#include "RawDataCnv/RawDataMcCnv.h"
#include "RawDataCnv/RawDataMcParticleCnv.h"
#include "RawDataCnv/RawDataMdcMcHitCnv.h"
#include "RawDataCnv/RawDataTofMcHitCnv.h"
#include "RawDataCnv/RawDataEmcMcHitCnv.h"
#include "RawDataCnv/RawDataMucMcHitCnv.h"
#include "RawDataCnv/RawDataDigiCnv.h"
#include "RawDataCnv/RawDataMdcDigiCnv.h"
#include "RawDataCnv/RawDataTofDigiCnv.h"
#include "RawDataCnv/RawDataLumiDigiCnv.h"
#include "RawDataCnv/RawDataEmcDigiCnv.h"
#include "RawDataCnv/RawDataMucDigiCnv.h"
#include "RawDataCnv/RawDataTrigCnv.h"
#include "RawDataCnv/RawDataTrigGTDCnv.h"
#include "RawDataCnv/RawDataHltCnv.h"
#include "RawDataCnv/RawDataHltRawCnv.h"
#include "RawDataCnv/RawDataZddEventCnv.h"
#include "RawDataCnv/PackedRawDataCnvSvc.h"

using namespace std;

static const InterfaceID IID_IRawDataBaseCnv("IRawDataBaseCnv", 1 , 0); 

/// Standard constructor
PackedRawDataCnvSvc::PackedRawDataCnvSvc(const std::string& name, ISvcLocator* svc)
: RawDataCnvSvc(name, svc, PACKEDRAWEVENT_StorageType)
{
     declareProperty("RawDataOutputSvc", m_ioSvcName); 
     declareProperty("Status", m_status=0xFFFFFFFF);    //fucd
}

/// Standard Destructor
PackedRawDataCnvSvc::~PackedRawDataCnvSvc()
{}

/// Initialize the service.
StatusCode PackedRawDataCnvSvc::initialize()     {
  MsgStream log(messageService(), name());
  log << MSG::INFO << name() << " PackedRawDataCnvSvc::initialize RawDataOutputSvc:" << m_ioSvcName<< endreq;

  StatusCode sc = RawDataCnvSvc::initialize();
  if( sc.isFailure() ) {
    log << MSG::ERROR << "Unable to initialize service " << endreq;
    return sc;
  }

  IService* svc ;
  sc = service(m_ioSvcName, svc);
  if (sc != StatusCode::SUCCESS ) {
     log << MSG::WARNING << "Cant get RawDataOutputSvc " << endreq;
     //return sc ;
  }
 
  m_ioSvc = dynamic_cast<IRawDataOutputSvc*> (svc);
  if (m_ioSvc == 0 ) {
    log << MSG::WARNING << "Cant cast to  RawDataOutputSvc " << endreq;
    //return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

/// Add converters to the service
StatusCode PackedRawDataCnvSvc::addConverters() {
  MsgStream log(messageService(), name());
  //log << MSG::INFO << name() << " PackedRawDataCnvSvc::addConverters" << endreq;
  declareObject(IRawDataCnvSvc::Leaf(EventModel::EventH, RawDataCnv::classID(), "PASS", 0));
  //log << MSG::INFO << "EventModel::EventH " << RawDataCnv::classID() << endreq;
  declareObject(IRawDataCnvSvc::Leaf(EventModel::EventHeader, RawDataEvtHeaderCnv::classID(), "PASS", 0));
  //log << MSG::INFO << "EventModel::EventHeader " << RawDataEvtHeaderCnv::classID() << endreq;
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::Event, RawDataMcCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::McParticleCol, RawDataMcParticleCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::MdcMcHitCol, RawDataMdcMcHitCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::TofMcHitCol, RawDataTofMcHitCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::EmcMcHitCol, RawDataEmcMcHitCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::MC::MucMcHitCol, RawDataMucMcHitCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::Event, RawDataDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::MdcDigiCol, RawDataMdcDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::TofDigiCol, RawDataTofDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::LumiDigiCol,RawDataLumiDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::EmcDigiCol, RawDataEmcDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Digi::MucDigiCol, RawDataMucDigiCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Trig::Event, RawDataTrigCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Trig::TrigGTDCol, RawDataTrigGTDCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Hlt::Event, RawDataHltCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Hlt::HltRawCol, RawDataHltRawCnv::classID(), "PASS", 0));
  declareObject(IRawDataCnvSvc::Leaf(EventModel::Zdd::ZddEvent, RawDataZddEventCnv::classID(), "PASS", 0));

  return StatusCode::SUCCESS;
}

/// Update state of the service
StatusCode PackedRawDataCnvSvc::updateServiceState(IOpaqueAddress* pAddress)    {
  MsgStream log(messageService(), name());
  StatusCode sc = RawDataCnvSvc::updateServiceState(pAddress);
  //log << MSG::INFO << "In PackedRawDataCnvSvc::updateServiceState" << endreq;
  return sc;
}

StatusCode PackedRawDataCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)  {
  if ( IID_IRawDataBaseCnv == riid ) {
     *ppvInterface = (IRawDataCnvSvc*)this;
  } else {
      // Interface is not directly availible: try out a base class
     return ConversionSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

StatusCode PackedRawDataCnvSvc::connectOutput(const std::string& /* t */)
{
 
   MsgStream log(messageService(), name() );
 
   log << MSG::DEBUG << " In connectOutput "<< endreq;
 
   // Get DataObj from TDS
   SmartDataPtr<Event::EventHeader> evt(dataProvider(), EventModel::EventHeader);
   if (!evt) {
       log << MSG::ERROR << "Did not retrieve event" << endreq;
       return StatusCode::FAILURE;
   }
   unsigned int eventNo = evt->eventNumber();
   unsigned int runNo = evt->runNumber();
 
   m_rawEvent = new WriteRawEvent(0x790000, // source_id
                                  eventNo,  // global_id
				  runNo,    // run_no
				  eventNo,  // lel1_id
				  0,        // lel1_type
				  0,        // lel2_info
				  ef_info   // ef_info
       );
   //const unsigned int status = 0;  // place holder for status
   if(m_status!=0xFFFFFFFF) m_rawEvent->status(1, &m_status);
   else         m_rawEvent->status(0, NULL);               //fucd
 
   return StatusCode::SUCCESS;
}

StatusCode PackedRawDataCnvSvc::commitOutput(const std::string& /* t*/ , bool /*b*/ )
{
   MsgStream log(messageService(), name() );
 
   log << MSG::DEBUG << "In flushOutput" << endreq;
 
   if (!m_ioSvc) {
      log << MSG::ERROR << "RawDataCnvSvc not configure for output" << endreq;
      return StatusCode::FAILURE;
   }
 
   /*if (!m_rawEvent->is_valid()) {
     log << MSG::ERROR << "commitOutput failed, because FullEventFragment invalid"
         << m_rawEvent << endreq;
     return StatusCode::FAILURE;
   }*/
 
   log << MSG::DEBUG << "commitOutput: Size of Event (words) ="
       << m_rawEvent->size_word() <<  endreq;
 
   if (!m_ioSvc->putEvent(m_rawEvent) ) {
     log << MSG::ERROR << "commitOutput failed to send output" << endreq;
     return StatusCode::FAILURE;
   }

   // delete ...
   const eformat::write::SubDetectorFragment* sd = m_rawEvent->first_child();
   while ( sd != 0) {
     const eformat::write::ROSFragment* ros = sd->first_child();
     while (ros != 0) {
       const eformat::write::ROBFragment* rob = ros->first_child();
       while (rob != 0) {
	 const eformat::write::ROBFragment* rob_tmp = rob;
	 rob = rob->next();
	 delete rob_tmp;
       }
       const eformat::write::ROSFragment* ros_tmp = ros;
       ros = ros->next();
       delete ros_tmp;
     }
     const eformat::write::SubDetectorFragment* sd_tmp = sd;
     sd = sd->next();
     delete sd_tmp;
   }
   delete m_rawEvent;
 
   return StatusCode::SUCCESS;
}

