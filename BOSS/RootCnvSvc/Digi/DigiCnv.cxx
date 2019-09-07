// File and Version Information:
// Description:
//      DigiCnv is the concrete converter for the Rec branch
//

#ifndef DigiCnv_CXX
#define DigiCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "RawEvent/DigiEvent.h"
#include "EventModel/EventModel.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"

using namespace DataSvcHelpers;
TDigiEvent* DigiCnv::m_objWrite = 0;

DigiCnv::DigiCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  m_rootBranchname ="m_fromMc"; 
  //declareObject(EventModel::Digi::Event, objType(), m_rootTreename, m_rootBranchname);
  m_adresses.push_back(&m_fromMc);

  //m_objWrite=0; 

}

StatusCode DigiCnv::initialize()   {
  MsgStream log(msgSvc(), "DigiCnv");
  log << MSG::DEBUG << "DigiCnv::initialize"<< endreq;
 
  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setDigiCnv(this);

  return status;

}

StatusCode DigiCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;
    
  MsgStream log(msgSvc(), "DigiCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;
/*
  SmartDataPtr<Event::EventHeader> evt(m_eds, EventModel::EventHeader);
  if (!evt) {
    log << MSG::ERROR << "Failed to retrieve RecEvent" << endreq;
    return StatusCode::FAILURE;
  }
*/
  DigiEvent * tdsev = new DigiEvent();
  refpObject = tdsev;
 
  tdsev->initialize(m_fromMc);

  return sc;
}

StatusCode DigiCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to digi event in root format
  MsgStream log(msgSvc(), "DigiCnv");
  log << MSG::DEBUG << "DigiCnv::DataObjectToTObject" << endreq;

  // test if already done for this event
  //zoujh: if (m_objWrite && (m_objWrite->getRunId()!=0 || m_objWrite->getEventId()!=0)) return StatusCode::SUCCESS;
  if (m_common.m_recEvt != NULL) return StatusCode::SUCCESS;

/*
  SmartDataPtr<Event::EventHeader> evtTds(m_eds, EventModel::EventHeader);
  
  if (!evtTds) {
    log << MSG::ERROR << "Failed to retrieve EventHeader in TDS" << endreq;
    return StatusCode::FAILURE;
  }
  */
  DigiEvent * devtTds=dynamic_cast<DigiEvent *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS RecEvent" << endreq;
    return StatusCode::FAILURE;
  }

  m_objWrite = new TDigiEvent();  //zoujh
  m_common.m_recEvt = m_objWrite;  //zoujh

  // create branch if not yet done
    if (m_branchNr<0) {
      //zoujh: m_objWrite = new TDigiEvent();
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TDigiEvent",m_objWrite->ClassName(),&m_objWrite,m_branchNr);
      if (sc.isFailure()) {
	log << MSG::ERROR << "Could not create branch TDigiEvent" << endreq;
	return sc;
      }
    }

  //Int_t evtId = evtTds->eventNumber();
  //Int_t runId = evtTds->runNumber();

  Bool_t fromMc = devtTds->fromMc();
 
 // const bool m_fromMc = fromMc;

#ifdef DEBUG
  std::cout<<"******************************"<<std::endl;
  std::cout<<" eveId:       "<<evtId<<std::endl;
  std::cout<<" runId:       "<<runId<<std::endl;
  std::cout<<" fromMc:      "<<fromMc<<std::endl;
  std::cout<<"******************************"<<std::endl;
#endif
  m_objWrite->initialize(fromMc);

  return StatusCode::SUCCESS;
}
#endif










