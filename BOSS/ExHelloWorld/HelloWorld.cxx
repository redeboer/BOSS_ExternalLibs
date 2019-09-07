#include "ExHelloWorld/HelloWorld.h"
#include "GaudiKernel/MsgStream.h"

/////////////////////////////////////////////////////////////////////////////

HelloWorld::HelloWorld(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator), m_myInt(0), m_myBool(0), m_myDouble(0)
{
  // Part 1: Declare the properties
  declareProperty("MyInt", m_myInt);
  declareProperty("MyBool", m_myBool);
  declareProperty("MyDouble", m_myDouble);
  declareProperty("MyStringVec",m_myStringVec);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloWorld::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " HelloWorld initialize()" << endreq;

  // Part 2: Print out the property values
  log << MSG::INFO << "  MyInt =    " << m_myInt << endreq;
  log << MSG::INFO << "  MyBool =   " << (int)m_myBool << endreq;
  log << MSG::INFO << "  MyDouble = " << m_myDouble << endreq;
  
  for (unsigned int i=0; i<m_myStringVec.size(); i++) {
    log << MSG::INFO << "  MyStringVec[" << i << "] = " << m_myStringVec[i] 
	<< endreq;
  }
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloWorld::execute() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "AtlasHelloWorld execute()" << endreq;

  // Part 2: Print out the different levels of messages
  log << MSG::DEBUG << "A DEBUG message" << endreq;
  log << MSG::INFO << "An INFO message" << endreq;
  log << MSG::WARNING << "A WARNING message" << endreq;
  log << MSG::ERROR << "An ERROR message" << endreq;
  log << MSG::FATAL << "A FATAL error message" << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode HelloWorld::finalize() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "HelloWorld finalize()" << endreq;
  
  return StatusCode::SUCCESS;
}

