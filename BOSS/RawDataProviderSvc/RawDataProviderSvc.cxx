#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "RawEvent/RawDataUtil.h"

// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt
#include <stdint.h>
#include <vector>

using namespace std;

RawDataProviderSvc::RawDataProviderSvc( const string& name, ISvcLocator* svcloc) :
    Service (name, svcloc) {
  declareProperty("OutputLevel", m_outputLevel=MSG::WARNING);
  declareProperty("skipLayers",  m_skipLayers);
  declareProperty("MdcWireEffThreshold", m_effThreshold=0.9);
  declareProperty("EmcELow", m_elow=0);  
  declareProperty("EmcTLow", m_tlow=0);
  declareProperty("EmcTHigh", m_thigh=60);
  declareProperty("EmcHotCrystals", m_hotVec);
  declareProperty("EmcDeadCrystals", m_deadVec);
  declareProperty("EmcDeadCrystals", m_deadVec);
  declareProperty("OnlineMode",  m_onlineMode=0);
  // tianhl for mt
  m_mdcProvider = new MdcRawDataProvider(name.c_str()); 
  m_tofProvider = new TofRawDataProvider(name.c_str()); 
  m_emcProvider = new EmcRawDataProvider(name.c_str()); 
  // tianhl for mt
}


RawDataProviderSvc::~RawDataProviderSvc() {
  if(m_mdcProvider) delete m_mdcProvider;
  if(m_tofProvider) delete m_tofProvider;
  if(m_emcProvider) delete m_emcProvider;
}


StatusCode RawDataProviderSvc::initialize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "RawDataProviderSvc::initialize()" << endreq;

     StatusCode sc = Service::initialize();
     if( sc.isFailure() ) return sc;

  // tianhl for mt
  std::string incidentSvc_name("IncidentSvc");
  //if(isGaudiThreaded(name())){
  //   incidentSvc_name += getGaudiThreadIDfromName(name());
  //}
  // tianhl for mt
     IIncidentSvc* incsvc;
     sc = service(incidentSvc_name.c_str(), incsvc);
     int priority = 100;
     if( sc.isSuccess() ){
       incsvc -> addListener(this, "BeginEvent", priority);
     }

     sc = m_mdcProvider->initialize();
     if( sc.isFailure() ) return sc;
     
     
     sc = m_tofProvider->initialize(m_onlineMode);
     if( sc.isFailure() ) return sc;

     if( m_skipLayers.size()>0 ) m_mdcProvider->setSkipLayers( m_skipLayers ); 
     m_mdcProvider->setEffThreshold( m_effThreshold ); 

     if(m_elow>0) m_emcProvider->setELowThreshold(RawDataUtil::EmcChargeChannel(m_elow));
     if(m_tlow>0) m_emcProvider->setTLowThreshold(m_tlow);
     if(m_thigh>0&&m_thigh<60) m_emcProvider->setTHighThreshold(m_thigh);
     if(m_hotVec.size()>0)     m_emcProvider->setHotCrystals(m_hotVec);
     if(m_deadVec.size()>0)     m_emcProvider->setHotCrystals(m_deadVec);
     sc = m_emcProvider->initialize(m_onlineMode,serviceLocator());
     if( sc.isFailure() ) return sc;

     //log << MSG::INFO << "finish initializing..." << endreq;
     return StatusCode::SUCCESS;
}

StatusCode RawDataProviderSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "RawDataProviderSvc::finalize()" << endreq;

     return StatusCode::SUCCESS;
}

StatusCode RawDataProviderSvc::queryInterface(const InterfaceID& riid, void** ppvInterface) 
{
  if ( IRawDataProviderSvc::interfaceID().versionMatch(riid) )    {
    *ppvInterface = (IRawDataProviderSvc*)this;
  }else{
    return Service::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void RawDataProviderSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() == "BeginEvent" ){
    log << MSG::DEBUG << "Begin Event" << endreq;
    m_tofProvider->handle( inc );
    m_emcProvider->handle( inc );
    m_mdcProvider->handle( inc );
  } 
}
