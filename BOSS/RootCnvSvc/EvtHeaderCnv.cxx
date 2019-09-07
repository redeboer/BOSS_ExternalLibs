// File and Version Information:
// Description:
//      EvtHeaderCnv is the concrete converter for the Rec branch
//

#ifndef EvtHeaderCnv_CXX
#define EvtHeaderCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "RootEventData/TEvtHeader.h"
#include "RootCnvSvc/EvtHeaderCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"

using namespace DataSvcHelpers;

TEvtHeader* EvtHeaderCnv::m_objWrite = 0;

EvtHeaderCnv::EvtHeaderCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  MsgStream log(msgSvc(), "EvtHeaderCnv");
  log << MSG::DEBUG << " constructor of EvtHeaderCnv "<<endreq;
  //m_rootTreename ="Event";
  m_rootBranchname ="m_runId:m_eventId:m_time:m_eventTag:m_flag1:m_flag2"; 
  //declareObject(EventModel::Dst::Event, objType(), m_rootTreename, m_rootBranchname);
  m_adresses.push_back(&m_runId);
  m_adresses.push_back(&m_eventId);
  m_adresses.push_back(&m_time);
  m_adresses.push_back(&m_eventTag);
  m_adresses.push_back(&m_flag1);
  m_adresses.push_back(&m_flag2);
}

StatusCode EvtHeaderCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;
    
  MsgStream log(msgSvc(), "EvtHeaderCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;

  
  
  Event::EventHeader* evt = new Event::EventHeader();
  evt->setEventNumber(m_eventId);
  evt->setRunNumber(m_runId);
  evt->setTime(m_time);
  evt->setEventTag(m_eventTag);
  evt->setFlag1( m_flag1 );
  evt->setFlag2( m_flag2 );

  refpObject = evt;
  
  log << MSG::DEBUG << "TObjectToDataObject eventid = "<<m_eventId<<"  "<<m_runId 
      << "  "<<std::hex<<"0x"<<m_eventTag<<std::dec<<endreq;
  //  cout << MSG::DEBUG << "TObjectToDataObject eventid = "<<m_eventId<<"  "<<m_runId 
  //    << "  "<<std::hex<<"0x"<<m_eventTag<<std::dec<<endl;

  //char dummy;
  //cin>>dummy;
  
  return sc;
}

StatusCode EvtHeaderCnv::initialize()   {
  MsgStream log(msgSvc(), "EvtHeaderCnv");
  log << MSG::DEBUG << "EvtHeaderCnv::initialize"<< endreq;
 
  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setEvtHeaderCnv(this);

  return status;

}

StatusCode EvtHeaderCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to Dst event in root format
  MsgStream log(msgSvc(), "EvtHeaderCnv");
  log << MSG::DEBUG << "EvtHeaderCnv::DataObjectToTObject" << endreq;


  Event::EventHeader * devtTds=dynamic_cast<Event::EventHeader *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS EventHeader" << endreq;
    return StatusCode::FAILURE;
  }

  Int_t evtId = devtTds->eventNumber();
  Int_t runId = devtTds->runNumber();
  UInt_t time = devtTds->time();
  UInt_t evtTag=devtTds->eventTag();
  UInt_t flag1 = devtTds->flag1();
  UInt_t flag2 = devtTds->flag2();

  if(evtId == -1){
    //wrong evh
    return StatusCode::SUCCESS;
  }

  
  m_objWrite = new TEvtHeader();  //zoujh
  m_common.m_EvtHeader = m_objWrite;  //zoujh

  log << MSG::DEBUG << "EvtHeaderCnv::DataObjectToTObject tree name: " <<rootaddr->getTreename()<<endreq;
  // create branch if not yet done
  if (m_branchNrEvtHeader<0) {
    StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TEvtHeader",m_objWrite->ClassName(),&m_objWrite,m_branchNrEvtHeader);
    //StatusCode sc = m_rootInterface->createBranch("Event","TEvtHeader",m_objWrite->ClassName(),&m_objWrite,m_branchNrEvtHeader);
    if (sc.isFailure()) {
      delete m_objWrite;
      log << MSG::ERROR << "Could not create branch TEvtHeader" << endreq;
      return StatusCode::SUCCESS;
      //return sc;
    }
  }
  
  m_eventId = evtId;
  m_runId   = runId;
  m_time    = time;
  m_eventTag= evtTag;
  m_flag1   = flag1;
  m_flag2   = flag2;
  log << MSG::DEBUG << "EvtHeaderCnv::DataObjectToTObject  eventid = "<<evtId<<"  "<<runId <<" "<<std::hex<<evtTag<<std::dec<< endreq;

  m_objWrite->initialize(evtId, runId, evtTag);
  m_objWrite->setTime(time);
  m_objWrite->setFlag1( flag1 );
  m_objWrite->setFlag2( flag2 );
  
  return StatusCode::SUCCESS;
}
#endif

