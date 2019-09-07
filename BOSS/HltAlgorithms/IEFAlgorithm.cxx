#include "HltAlgorithms/IEFAlgorithm.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IMessageSvc.h"

IEFAlgorithm::IEFAlgorithm(const std::string& name, ISvcLocator* pSvcLocator):
  Algorithm(name, pSvcLocator){
  m_propMgr.declareProperty("OutputLevel",m_output = 7777777);
  m_propMgr.declareProperty("BeamEnergy",m_beam = 1.89);
  //cout << "OutputLevel="<<m_output<<";  name="<<name<<endl;
  // Get a pointer to the Job Options Service
  IJobOptionsSvc* jobSvc;
  Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);  
  jobSvc->setMyProperties("EFAlgorithm", &m_propMgr);

  //cout << "OutputLevel="<<m_output<<";  name="<<name<<endl;
  //MsgStream log(msgSvc(), name);
  //msgSvc()->setOutputLevel(name,m_output);
}

StatusCode IEFAlgorithm::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
 
  m_run=0;

  StatusCode sc = service( "HltStoreSvc", m_HltStoreSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() << ": Unable to locate Service HltStoreSvc" << endreq;
    return sc;
  }

  sc = m_HltStoreSvc->get("EFResult", m_ef);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->get(EFResult) wrong" << endreq;
    return sc;
  }  

  sc = service("RawDataProviderSvc", m_rawDigiSvc);
  if (sc ==  StatusCode::SUCCESS) {
    log << MSG::INFO << "Raw Data Service Sucessfully at " << m_rawDigiSvc << " for " << name() << endreq;
  }
  else {
    log << MSG::ERROR << "Raw Data Service Failed !! " << endreq;
    return StatusCode::FAILURE;
  }
  
  log << MSG::DEBUG << "finish initialize()" << endreq;
  return StatusCode::SUCCESS;
}
