// File and Version Information:
// Description:
//      DstCnv is the concrete converter for the Rec branch
//

#ifndef RecTrackCnv_CXX
#define RecTrackCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventHeader.h"
#include "ReconEvent/ReconEvent.h"
#include "EventModel/EventModel.h"
#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"
#include "McTruth/DecayMode.h"

using namespace DataSvcHelpers;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecTrackCnv> s_factory;
//const ICnvFactory& RecTrackCnvFactory = s_factory;

TRecTrackEvent* RecTrackCnv::m_objWrite = 0;

RecTrackCnv::RecTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

  // Here we associate this converter with the path on the TDS.
  m_rootBranchname =""; 
  //declareObject(EventModel::Recon::Event, objType(), m_rootTreename, m_rootBranchname);   //Dst-->Recon liangyt -->Rec
  //m_objWrite=0; 
}

StatusCode RecTrackCnv::initialize()   {
  MsgStream log(msgSvc(), "RecTrackCnv");
  log << MSG::DEBUG << "RecTrackCnv::initialize"<< endreq;
 
  StatusCode status = RootEventBaseCnv::initialize();

  //pass dst converter to the conversion service
  m_cnvSvc->setRecTrackCnv(this);

  return status;

}

StatusCode RecTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;

  MsgStream log(msgSvc(), "RecTrackCnv");
  log << MSG::DEBUG << "TObjectToDataObject" << endreq;
  /*
  SmartDataPtr<Event::EventHeader> evt(m_eds, EventModel::EventHeader);
  if (!evt) {
    log << MSG::ERROR << "Failed to retrieve RecEvent" << endreq;
    return StatusCode::FAILURE;
  }

  */
  
  ReconEvent * tdsev = new ReconEvent();
  refpObject = tdsev;
  //sc =m_eds ->registerObject(EventModel::EventHeader, evt);


  return sc;
}

StatusCode RecTrackCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to Dst event in root format
  MsgStream log(msgSvc(), "RecTrackCnv");
  log << MSG::DEBUG << "RecTrackCnv::DataObjectToTObject" << endreq;

  // test if already done for this event
  //zoujh: if (m_objWrite && (m_objWrite->getRunId()!=0 || m_objWrite->getEventId()!=0)) return StatusCode::SUCCESS;

  if (m_common.m_rectrackEvt != NULL) return StatusCode::SUCCESS;
/*
  SmartDataPtr<Event::EventHeader> evtTds(m_eds, EventModel::EventHeader);
  
  if (!evtTds) {
    log << MSG::ERROR << "Failed to retrieve EventHeader in TDS" << endreq;
    return StatusCode::FAILURE;
  }
*/
  ReconEvent * devtTds=dynamic_cast<ReconEvent *> (obj);
  if (!devtTds) {
    log << MSG::ERROR << "Could not downcast to TDS Event" << endreq;
    return StatusCode::FAILURE;
  }

  m_objWrite = new TRecTrackEvent();  //zoujh
  m_common.m_rectrackEvt = m_objWrite;  //zoujh

  // create branch if not yet done
  if (m_branchNrRecon<0) {   ///////////liangyt
      //zoujh: m_objWrite = new TDstEvent();
      StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"TRecEvent",m_objWrite->ClassName(),&m_objWrite,m_branchNrRecon);


      if (sc.isFailure()) {
        cout<<"Could not create branch TRecEvent"<<endl;
	log << MSG::ERROR << "Could not create branch TRecEvent" << endreq;
	return sc;
      }
    }

  //Int_t evtId = evtTds->eventNumber();
  //Int_t runId = evtTds->runNumber();

 
  m_objWrite->initialize();

  return StatusCode::SUCCESS;
}
#endif










