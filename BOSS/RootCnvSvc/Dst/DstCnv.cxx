// File and Version Information:
// Description:
//      DstCnv is the concrete converter for the Rec branch
//

#ifndef DstCnv_CXX
#define DstCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "DstEvent/DstEvent.h"
#include "EventModel/EventModel.h"
#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"
#include "McTruth/DecayMode.h"

using namespace DataSvcHelpers;

TDstEvent* DstCnv::m_objWrite = 0;

DstCnv::DstCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  m_rootBranchname =""; 
  //declareObject(EventModel::Dst::Event, objType(), m_rootTreename, m_rootBranchname);
}

StatusCode DstCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;
    
  MsgStream log(msgSvc(), "DstCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;
  /*
  SmartDataPtr<Event::EventHeader> evt(m_eds, EventModel::EventHeader);
  if (!evt) {
    log << MSG::ERROR << "Failed to retrieve RecEvent" << endreq;
    return StatusCode::FAILURE;
  }

  */
  
 
  DstEvent * tdsev = new DstEvent();
  refpObject = tdsev;


  return sc;
}

StatusCode DstCnv::initialize()   {
  MsgStream log(msgSvc(), "DstCnv");
  log << MSG::DEBUG << "DstCnv::initialize"<< endreq;
 
  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setDstCnv(this);

  return status;

}

StatusCode DstCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to Dst event in root format
  MsgStream log(msgSvc(), "DstCnv");
  log << MSG::DEBUG << "DstCnv::DataObjectToTObject" << endreq;

  // test if already done for this event
  //zoujh: if (m_objWrite && (m_objWrite->getRunId()!=0 || m_objWrite->getEventId()!=0)) return StatusCode::SUCCESS;
  if (m_common.m_dstEvt != NULL) return StatusCode::SUCCESS;

/*
  SmartDataPtr<Event::EventHeader> evtTds(m_eds, EventModel::EventHeader);
  
  if (!evtTds) {
    log << MSG::ERROR << "Failed to retrieve EventHeader in TDS" << endreq;
    return StatusCode::FAILURE;
  }
  */
  DstEvent * devtTds=dynamic_cast<DstEvent *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS DstEvent" << endreq;
    return StatusCode::FAILURE;
  }

  m_objWrite = new TDstEvent();  //zoujh
  m_common.m_dstEvt = m_objWrite;  //zoujh

  // create branch if not yet done
    if (m_branchNrDst<0) {
      //zoujh: m_objWrite = new TDstEvent();
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TDstEvent",m_objWrite->ClassName(),&m_objWrite,m_branchNrDst);
      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not create branch TDstEvent" << endreq;
	return sc;
      }
    }

//  Int_t evtId = evtTds->eventNumber();
//  Int_t runId = evtTds->runNumber();


  m_objWrite->initialize();

  return StatusCode::SUCCESS;
}
#endif










