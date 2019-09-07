#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/DataSvc.h"
#include <exception>
#include "RawDataProviderSvc/RawDataProviderBase.h"
// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt

RawDataProviderBase::RawDataProviderBase() : 
  m_svcLocator(0),
  m_msgSvc(0),
  m_name()
{}


RawDataProviderBase::RawDataProviderBase( const char* name ) :
  m_svcLocator(0),
  m_msgSvc(0),
  m_name(name)
{}


StatusCode RawDataProviderBase::initialize( ISvcLocator* pSvcLoc, IMessageSvc* pMsg ) {
  if(pSvcLoc!=0){
    // test service point
    // tianhl for mt
    std::string evtDataSvc_name("EventDataSvc");
    if(isGaudiThreaded(m_name)){
      evtDataSvc_name += getGaudiThreadIDfromName(m_name);
      std::cout << "---------------------------------- " <<  evtDataSvc_name << std::endl;
    }
    // tianhl for mt
    IDataProviderSvc* evtSvc;
    StatusCode sc = pSvcLoc->service(evtDataSvc_name.c_str(),evtSvc,true);
    if (!sc.isSuccess()){
      std::cout << "RawDataProviderBase::initialize()  ERROR Could not load EventDataSvc" << std::endl;
      return StatusCode::FAILURE;
    }
    m_svcLocator= pSvcLoc;
  }
  else {
    m_svcLocator= Gaudi::svcLocator();
  }
  try{
    if(pMsg!=0){
      MsgStream log(pMsg, m_name);
      log << MSG::INFO << "RawDataProviderBase::initialize()  Test..." << endreq;
      m_msgSvc=pMsg;
    }
    else{ 
      StatusCode sc = m_svcLocator->service("MessageSvc", m_msgSvc ,true);
      if (!sc.isSuccess()) throw SvcDisable("MessageSvc");
    }
  }
  catch(...){
    std::cout << "RawDataProviderBase::initialize()  ERROR Could not get MessageSvc" << std::endl;
  }

  return StatusCode::SUCCESS;
}
