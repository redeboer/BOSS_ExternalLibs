// File and Version Information:
// Description:
//      McCnv is the concrete converter for the Mc branch
//

#ifndef McCnv_CXX
#define McCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "McTruth/McEvent.h"          
#include "EventModel/EventModel.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"

using namespace DataSvcHelpers;

TMcEvent* McCnv::m_objWrite = 0;
 
McCnv::McCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  //declareObject(EventModel::MC::Event, objType(), m_rootTreename, m_rootBranchname);
  
  //m_objWrite=0; 
}

StatusCode McCnv::initialize()   {
  MsgStream log(msgSvc(), "McCnv");
  log << MSG::DEBUG << "McCnv::initialize"<< endreq;
 
  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setMcCnv(this);

  return status;

}


StatusCode McCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;
    
  MsgStream log(msgSvc(), "McCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;

  SmartDataPtr<Event::EventHeader> evt(m_eds, EventModel::EventHeader);
  if (!evt) {
    log << MSG::ERROR << "Failed to retrieve McEvent" << endreq;
    return StatusCode::FAILURE;
  }

  McEvent * tdsev = new McEvent();
  refpObject = tdsev;
 
  return sc;
}

StatusCode McCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to Mc event in root format
  MsgStream log(msgSvc(), "McCnv");
  log << MSG::DEBUG << "McCnv::DataObjectToTObject" << endreq;

  // test if already done for this event
  //zoujh: if (m_objWrite && (m_objWrite->getRunId()!=0 || m_objWrite->getEventId()!=0)) return StatusCode::SUCCESS;
  if (m_common.m_mcEvt != NULL) return StatusCode::SUCCESS;

/*
  SmartDataPtr<Event::EventHeader> evtTds(m_eds, EventModel::EventHeader);
  
  if (!evtTds) {
    log << MSG::ERROR << "Failed to retrieve EventHeader in TDS" << endreq;
    return StatusCode::FAILURE;
  }
  */
  McEvent * devtTds=dynamic_cast<McEvent *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS McEvent" << endreq;
    return StatusCode::FAILURE;
  }

  m_objWrite = new TMcEvent();  //zoujh
  m_common.m_mcEvt = m_objWrite;  //zoujh

  // create branch if not yet done
    if (m_branchNrMc<0) {
      //zoujh: m_objWrite = new TMcEvent();
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TMcEvent",m_objWrite->ClassName(),&m_objWrite,m_branchNrMc);
      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not create branch TMcEvent" << endreq;
	return sc;
      }
    }

//  Int_t evtId = evtTds->eventNumber();
//  Int_t runId = evtTds->runNumber();


#ifdef DEBUG
  std::cout<<"******************************"<<std::endl;
  std::cout<<" eveId:       "<<evtId<<std::endl;
  std::cout<<" runId:       "<<runId<<std::endl;
  std::cout<<"******************************"<<std::endl;
#endif
  m_objWrite->initialize();

  return StatusCode::SUCCESS;
}
#endif










