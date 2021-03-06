// File and Version Information:
// Description:
//      HltCnv is the concrete converter for the Hlt branch
//

#ifndef HltCnv_CXX
#define HltCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "HltEvent/HltEvent.h"
#include "EventModel/EventModel.h"
#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"

using namespace DataSvcHelpers;
THltEvent* HltCnv::m_objWrite = 0;

HltCnv::HltCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  //o m_rootBranchname ="m_fromMc";
  //declareObject(EventModel::Digi::Event, objType(), m_rootTreename, m_rootBranchname);
  //o m_adresses.push_back(&m_fromMc);

  //m_objWrite=0; 

}

StatusCode HltCnv::initialize()   {
  MsgStream log(msgSvc(), "HltCnv");
  log << MSG::DEBUG << "HltCnv::initialize"<< endreq;

  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setHltCnv(this);

  return status;

}

StatusCode HltCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the root object
  StatusCode sc=StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "HltCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;
/*
  SmartDataPtr<Event::EventHeader> evt(m_eds, EventModel::EventHeader);
  if (!evt) {
    log << MSG::ERROR << "Failed to retrieve RecEvent" << endreq;
    return StatusCode::FAILURE;
  }
*/
  HltEvent * tdsev = new HltEvent();
  refpObject = tdsev;

  m_fromMc = false; // There is no "THltEvent" branch in MC-dst-files.
  tdsev->initialize(m_fromMc);

  return sc;
}

StatusCode HltCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to digi event in root format
  MsgStream log(msgSvc(), "HltCnv");
  log << MSG::DEBUG << "HltCnv::DataObjectToTObject" << endreq;

  // test if already done for this event
  //zoujh: if (m_objWrite && (m_objWrite->getRunId()!=0 || m_objWrite->getEventId()!=0)) return StatusCode::SUCCESS;
  if (m_common.m_hltEvt != NULL) return StatusCode::SUCCESS;

/*
  SmartDataPtr<Event::EventHeader> evtTds(m_eds, EventModel::EventHeader);
  
  if (!evtTds) {
    log << MSG::ERROR << "Failed to retrieve EventHeader in TDS" << endreq;
    return StatusCode::FAILURE;
  }
  */
  HltEvent * devtTds=dynamic_cast<HltEvent *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS RecEvent" << endreq;
    return StatusCode::FAILURE;
  }
  m_objWrite = new THltEvent();  //zoujh
  m_common.m_hltEvt = m_objWrite;  //zoujh

  // create branch if not yet done
    if (m_branchNr<0) {
      //zoujh: m_objWrite = new THltEvent();
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"THltEvent",m_objWrite->ClassName(),&m_objWrite,m_branchNr);
      if (sc.isFailure()) {
        log << MSG::ERROR << "Could not create branch THltEvent" << endreq;
        return sc;
      }
    }

  //Int_t evtId = evtTds->eventNumber();
  //Int_t runId = evtTds->runNumber();

  //o Bool_t fromMc = devtTds->fromMc();

#ifdef DEBUG
  std::cout<<"******************************"<<std::endl;
  std::cout<<" eveId:       "<<evtId<<std::endl;
  std::cout<<" runId:       "<<runId<<std::endl;
  std::cout<<"******************************"<<std::endl;
#endif

  //o m_objWrite->initialize(fromMc);
  m_objWrite->initialize();

  return StatusCode::SUCCESS;
}
#endif
