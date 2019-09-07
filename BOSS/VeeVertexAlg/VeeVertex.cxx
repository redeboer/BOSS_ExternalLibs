#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EvtRecEvent/EvtRecVeeVertex.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "VeeVertexAlg/VeeVertex.h"
#include <vector>
//**************************************************************************************
VeeVertex::VeeVertex(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  //Declare the properties
  declareProperty("KShortReconstruction", m_recKShort = true);
  declareProperty("LambdaReconstruction", m_recLambda = true);
  declareProperty("GammaConversionReconstruction", m_recGC = true);
}

// ***********************************************************************************
StatusCode VeeVertex::initialize() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "creating VeeVertex sub Algorithm" << endreq;

  StatusCode sc;
  
  if(m_recKShort) { 
    sc = createSubAlgorithm("KShortReconstruction", "KShortReconstruction", m_KShortReconstruction);
    if(sc.isFailure()) {
      log << MSG::ERROR << "Error while creating KShortReconstruction" << endreq;
      return StatusCode::FAILURE;
    }
  }
  if(m_recLambda) { 
    sc = createSubAlgorithm("LambdaReconstruction", "LambdaReconstruction", m_LambdaReconstruction);
    if(sc.isFailure()) {
      log << MSG::ERROR << "Error while creating LambdaReconstruction" << endreq;
      return StatusCode::FAILURE;
    }
  }
  if(m_recGC) { 
    sc = createSubAlgorithm("GammaConversionReconstruction", "GammaConversionReconstruction", m_gcReconstruction);
    if(sc.isFailure()) {
      log << MSG::ERROR << "Error while creating GammaConversionReconstruction" << endreq;
      return StatusCode::FAILURE;
    }
  }

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

StatusCode VeeVertex::registerParent(MsgStream& log) {
  DataObject *aEvtRecEvent;
  eventSvc()->findObject("/Event/EvtRec",aEvtRecEvent);
  if (aEvtRecEvent == NULL) {
    aEvtRecEvent = new EvtRecEvent();
    StatusCode sc = eventSvc()->registerObject("/Event/EvtRec",aEvtRecEvent);
    if (sc != StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register EvtRecEvent" <<endreq;
      return StatusCode::FAILURE;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode VeeVertex::clearEvtRecVeeVertexCol(MsgStream& log) {
  StatusCode sc = registerParent(log);
  if (sc != StatusCode::SUCCESS) {
    return sc;
  }
  
  DataObject* aEvtRecVeeVertexCol;
  eventSvc()->findObject("/Event/EvtRec/EvtRecVeeVertexCol",aEvtRecVeeVertexCol);
  if (aEvtRecVeeVertexCol != NULL) {
    SmartIF<IDataManagerSvc> dataManSvc(eventSvc());
    dataManSvc->clearSubTree("/Event/EvtRec/EvtRecVeeVertexCol");
    eventSvc()->unregisterObject("/Event/EvtRec/EvtRecVeeVertexCol");
  } 
  return StatusCode::SUCCESS;
}

void VeeVertex::registerEvtRecVeeVertexCol(MsgStream& log) {
  EvtRecVeeVertexCol* aNewEvtRecVeeVertexCol = new EvtRecVeeVertexCol;
  StatusCode sc = eventSvc()->registerObject("/Event/EvtRec/EvtRecVeeVertexCol",
                                             aNewEvtRecVeeVertexCol);
  if (sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EvtRecVeeVertexCol in TDS!" << endreq;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode VeeVertex::execute() {
  MsgStream log(msgSvc(), name());

  StatusCode sc = clearEvtRecVeeVertexCol(log);
  if (sc != StatusCode::SUCCESS) {
    return sc;
  }
  
  std::vector<Algorithm*>::const_iterator it  = subAlgorithms()->begin();
  std::vector<Algorithm*>::const_iterator end = subAlgorithms()->end();
  for(; it != end; it++) {
    sc = (*it)->execute();
    if(sc.isFailure()) {
      log << "Error executing selection "  << (*it)->name() << endreq;
    }
  }

  DataObject* aEvtRecVeeVertexCol;
  eventSvc()->findObject("/Event/EvtRec/EvtRecVeeVertexCol", aEvtRecVeeVertexCol);
  if (aEvtRecVeeVertexCol == NULL) {
    registerEvtRecVeeVertexCol(log);
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode VeeVertex::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

