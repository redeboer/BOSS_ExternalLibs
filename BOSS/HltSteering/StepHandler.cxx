/********************************************************************
NAME:     StepHandler.cxx
********************************************************************/
// INCLUDE GAUDI HEADER FILES:      
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ISvcLocator.h"

#include "HltSteering/StepHandler.h"
//#include "HltSteering/StepSequencer.h"
//#include "HltSteering/StepDecision.h"
#include "HltSteerData/Sequence.h"
#include "HltSteerData/Signature.h"

//#ifdef TIME_MEASUREMENT   //// Timer
//#include "HltDataTypes/EFBesTimer.h"
//#endif

using HltProcessor::Sequence;
using HltProcessor::Signature;
using namespace HltProcessor;
////////////////////////////////////////////////////////////////
//  CONSTRUCTOR:
StepHandler::StepHandler(const std::string& name, ISvcLocator* pSvcLocator): 
  Algorithm(name, pSvcLocator)
{
  std::vector<std::string> resultVec;
  resultVec.push_back("Error");
  resultVec.push_back("Rejected");
  resultVec.push_back("Accepted");
  m_EFResult = new EFResult(resultVec);

  declareProperty("EventTypes",  m_tmp_evtType);
  //evtTypes.push_back("Unknown");
  //evtTypes.push_back("Hadron");
  //evtTypes.push_back("Dimuon");
  //evtTypes.push_back("DiElectron");
  //evtTypes.push_back("DiPhoton");
  //m_evtType = new EFResult(m_tmp_evtType);
}

// DESTRUCTOR:
StepHandler::~StepHandler() {
  if (m_EFResult) delete m_EFResult;
  //if (m_evtType)  delete m_evtType;
}

/////////////////////////////////////////////////////////////////
// INITIALIZE METHOD:
StatusCode StepHandler::initialize() {
  // MSGStream object to output messages from your algorithm
  MsgStream log( messageService(), name() );

  Algorithm::initialize(); 

  StatusCode sc = service("HltConfigSvc", m_HltConfigSvc);
  if( sc.isFailure()) {
    log << MSG::FATAL << name() << ": Unable to locate HltConfigSvc" << endreq;
    return sc;
  }
  sc = service("HltStoreSvc", m_HltStoreSvc);
  if( sc.isFailure()) {
    log << MSG::FATAL << name() << ": Unable to locate HltStoreSvc" << endreq;
    return sc;
  }
  //CreateSubAlgorithms; StepSequencer and StepDecision
  /*
  Algorithm* p_seqAlg;
  Algorithm* p_decAlg;

  sc = createSubAlgorithm("StepSequencer", "StepSequencer", p_seqAlg); 
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to create StepSequencer SubAlgorithm " << endreq;
    return sc;
  } else {
    sequencerAlg = static_cast<StepSequencer*>(p_seqAlg);
    log << MSG::DEBUG << "created SubAlgorithm StepSequencer/StepSequencer" << endreq;
  }

  sc = createSubAlgorithm("StepDecision", "StepDecision", p_decAlg); 
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to create StepDecision SubAlgorithm " << endreq;
    return sc;
  } else {
    decisionAlg = static_cast<StepDecision*>(p_decAlg);
    log << MSG::DEBUG << "created SubAlgorithm StepDecision/StepDecision" << endreq;
  }

  const std::vector<std::pair<Signature*,Sequence*> >& m_TableVector = m_HltConfigSvc->retrieve();
  std::vector<std::pair<Signature*,Sequence*> >::const_iterator It;

  for (It = m_TableVector.begin(); It != m_TableVector.end(); ++It) {
    sequencerAlg->initSequencer(It->second);
    decisionAlg->initDecision(It->first);
    m_stepIdList.push_back(It->second->getSeqID());
  }
  */
  if (m_EFResult)
    m_HltStoreSvc->put("EFResult", m_EFResult);
  else {
    log << MSG::ERROR << "Unvalid m_EFResult" << endreq;
    return StatusCode::FAILURE;
  }

  m_evtType = new EFResult(m_tmp_evtType);
  if (m_evtType)
    m_HltStoreSvc->put("EventType", m_evtType);
  else {
    log << MSG::ERROR << "Unvalid m_evtType" << endreq;
    return StatusCode::FAILURE;
  }
#ifdef TIME_MEASUREMENT   //// Timer
  sc = service("BesTimerSvc", m_timersvc);
  if (sc.isFailure()) {
    log << MSG::WARNING << name() << ": Unable to locate BesTimer Service" << endreq;
    return StatusCode::FAILURE;
  }
  m_timer[0] = m_timersvc->addItem("Step Handler Time");
  m_timer[1] = m_timersvc->addItem("1stStep exec Time");
  m_timer[2] = m_timersvc->addItem("1stStep deci Time");
  m_timer[3] = m_timersvc->addItem("2ndStep exec Time");
  m_timer[4] = m_timersvc->addItem("2ndStep deci Time");

  NTuplePtr nt(ntupleSvc(),"FILE444/nt");
  if ( nt ) {
    m_tuple = nt;
    m_tuple->addItem("total", g_StepHandlerTime);
    m_tuple->addItem("mdc", g_1stStepExecTime);
    m_tuple->addItem("mdcd", g_1stStepDeciTime);
    m_tuple->addItem("emc", g_2ndStepExecTime);
    m_tuple->addItem("emcd", g_2ndStepDeciTime);
  } else {
    m_tuple = ntupleSvc()->book("FILE444/nt",CLID_ColumnWiseTuple,"StepCnt");
    if (m_tuple) {
      m_tuple->addItem("total", g_StepHandlerTime);
      m_tuple->addItem("mdc", g_1stStepExecTime);
      m_tuple->addItem("mdcd", g_1stStepDeciTime);
      m_tuple->addItem("emc", g_2ndStepExecTime);
      m_tuple->addItem("emcd", g_2ndStepDeciTime);
    } else {
      log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  round = 0;
#endif

  log << MSG::INFO << "Initialization " << name() << " completed successfully" << endreq;
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// FINALIZE METHOD:
  
StatusCode StepHandler::finalize() {
  if (m_evtType)  delete m_evtType;
  return StatusCode::SUCCESS;
}

StatusCode StepHandler::beginRun(){
  MsgStream log( messageService(), name() );

  Algorithm::beginRun();

  Algorithm* p_seqAlg;
  Algorithm* p_decAlg;
 
  StatusCode sc = createSubAlgorithm("StepSequencer", "StepSequencer", p_seqAlg);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to create StepSequencer SubAlgorithm " << endreq;
    return sc;
  } else {
    sequencerAlg = static_cast<StepSequencer*>(p_seqAlg);
    log << MSG::DEBUG << "created SubAlgorithm StepSequencer/StepSequencer" << endreq;
  }
 
  sc = createSubAlgorithm("StepDecision", "StepDecision", p_decAlg);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "Unable to create StepDecision SubAlgorithm " << endreq;
    return sc;
  } else {
    decisionAlg = static_cast<StepDecision*>(p_decAlg);
    log << MSG::DEBUG << "created SubAlgorithm StepDecision/StepDecision" << endreq;
  }
 
  const std::vector<std::pair<Signature*,Sequence*> >& m_TableVector = m_HltConfigSvc->retrieve();
  std::vector<std::pair<Signature*,Sequence*> >::const_iterator It;
 
  for (It = m_TableVector.begin(); It != m_TableVector.end(); ++It) {
    sequencerAlg->initSequencer(It->second);
    decisionAlg->initDecision(It->first);
    m_stepIdList.push_back(It->second->getSeqID());
  }

  log << MSG::DEBUG << "finish initializing sub-algorithms" << endreq;
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////
// EXECUTE METHOD:
StatusCode StepHandler::execute() {
  // MSGStream object to output messages from your algorithm
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "*** Executing StepHandler *** " << FSMState() << endreq;

  //if(EFBesTimer::pBesTimer&&EFBesTimer::pType==2)EFBesTimer::pBesTimer->start();

#ifdef TIME_MEASUREMENT   //// Timer
  m_timer[0]->start();
  m_timer[1]->start();
  m_timer[2]->start();
  m_timer[3]->start();
  m_timer[4]->start();
#endif

  bool StepSeq = true;
  int  StepDec = -999;
  log << MSG::DEBUG << "point: " << m_EFResult << " " << m_evtType << " " << sequencerAlg << endreq;
  m_EFResult->reset();
  m_evtType->reset();
  if(!sequencerAlg) return StatusCode::FAILURE;
  sequencerAlg->reset();

  std::string answer, result;
  std::string::size_type separator;

  std::vector<std::string>::const_iterator id = m_stepIdList.begin();

  while (id != m_stepIdList.end()) {
    log << MSG::DEBUG << "In step " << (*id) << endreq;

    StepSeq = sequencerAlg->execSequencer(*id);
    if (!StepSeq) {
      log << MSG::DEBUG << " Step Sequencer failed" << endreq;
      id++;
      continue;
    } 
 
    log << MSG::DEBUG << "+++++++++++++++++++++++++++++" << endreq;

    StepDec = decisionAlg->execDecision(*id);
    std::string midAnswer = decisionAlg->getContinue();
    if (midAnswer!="") m_evtType->setMidAnswer(midAnswer);
    //cout <<midAnswer<< endl;

    if (StepDec>0)    m_EFResult->push_back(StepDec);
    else {
      //log << MSG::DEBUG << " Execute next step" << endreq;
      m_EFResult->push_back(0);
      id++;
      continue;
    }
    answer = decisionAlg->getBehaviour();
    log << MSG::DEBUG << " Matched Criteria, Behaviour = " << answer << endreq;
    
    separator = answer.find('_');
    if (separator == std::string::npos) {
      log << MSG::ERROR << " Unknown decision result: "<< answer << endreq;
      return StatusCode::FAILURE;
    }
    std::string perform = answer.substr(0, separator);

    //std::string midAnswer = decisionAlg->getContinue();
    //if(midAnswer!="") m_evtType->setMidAnswer(midAnswer);
    //cout <<answer<<"  "<<midAnswer<< endl;
    if (perform == "reject") {
      result = "Rejected";
      break;
    }
    else if (perform == "accept") {
      result = "Accepted";
      break;
    }
    else if (perform == "jump") {
      std::string nextId = answer.substr(separator+1);
      do {
	++id;
      }	while ((id != m_stepIdList.end()) && ((*id) != nextId));
      if (id == m_stepIdList.end()) {
	log << MSG::ERROR << " Unknown jump target: "<< nextId << endreq;
	return StatusCode::FAILURE;
      }
    }
    else {
      log << MSG::ERROR << " Unknown decision result: "<< answer << endreq;
      return StatusCode::FAILURE;
    }
    //std::string evtMark = tmp.substr(tmp.find('_')+1);
    //if(evtMark!="") m_evtType->setMidAnswer(evtMark);
    //cout <<answer<<endl;
  }

  log << MSG::DEBUG << "==============" <<m_evtType->getAnswerIndex()<< endreq;
  
  if (result == "Accepted") {
    std::string evtMark = answer.substr(separator+1);
    log << MSG::DEBUG << " Event is Accepted as " << evtMark << endreq;
    m_evtType->setAnswer(evtMark);
  }
  else if (result == "Rejected") {
    // only beam-gas rejected defaultly
    // other type event not decided in Event Filter
    // by Fu Chengdong
    std::string evtMark = "Beamgas";
    m_evtType->setAnswer(evtMark);
    log << MSG::DEBUG << " Event is Rejected" << endreq;
  }
  else {
    log << MSG::WARNING << " Unknown Behaviour, force to Reject" << endreq;
    result = "Rejected";
  }
  log << MSG::DEBUG << "==============" <<m_evtType->getAnswerIndex()<< endreq;
  m_EFResult->setAnswer(result);

#ifdef TIME_MEASUREMENT   //// Timer
  m_timer[0]->stop();
  g_StepHandlerTime=m_timer[0]->elapsed();
  if(m_tuple){
    StatusCode status = m_tuple->write();
    if (!status.isSuccess()) {
      log << MSG::ERROR << "Can't fill ntuple!" << endreq;
    }
  }
#endif
  return StatusCode::SUCCESS;
}
