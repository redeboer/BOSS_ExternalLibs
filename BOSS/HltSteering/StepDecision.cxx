/********************************************************************
NAME:     StepDecision.cxx
********************************************************************/
// INCLUDE HEADER FILES:
#include "HltSteering/StepDecision.h"
// INCLUDE GAUDI HEADER FILES:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/StatusCode.h"
                                  
#include <string> 
#include <vector> 
#include <algorithm> 
//  END OF HEADER FILES INCLUDE
using HltProcessor::Signature; 
 
////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:
StepDecision::StepDecision(const std::string& name, ISvcLocator* pSvcLocator): 
  Algorithm(name, pSvcLocator) {
  m_isEnabled = false;
}
                                  
// DESTRUCTOR:
StepDecision::~StepDecision() { }
                                  
/////////////////////////////////////////////////////////////////
// INITIALIZE METHOD:
StatusCode StepDecision::initDecision(Signature* signature) {

  MsgStream log( messageService(), name() );

  StatusCode sc = service( "HltStoreSvc", m_HltStoreSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() << ": Unable to locate Service HltStoreSvc" << endreq;
    return sc;
  }

  std::string sigID = signature->label();
  m_SigMap[sigID] = signature;
  log << MSG::INFO << name() << " Find Signature with ID: " << sigID << endreq;

  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// MY EXECUTE METHOD:                                
int StepDecision::execDecision(const std::string& sigID) {

  MsgStream log( messageService(), name() );  
  //log << MSG::DEBUG << "Starting Execution StepDecision" << endreq;

  Signature* sig = m_SigMap[sigID];
  int decision = sig->isCriteriaMatched();

  m_continue =sig->midresult();
  m_behaviour = sig->result();
  //cout << "----"<< m_behaviour<< "  "<<m_continue<<endl;
  return decision;
}
