//********************************************************
//
//  BESIII Tof Simulation Service 
//  Class: TofSimSvc
//  Sun Shengsen <sunss@ihep.ac.cn>
//  
//  Description:
//     This service is used to get Tof Simulation Constants 
//     from DataBase and do the Tof simulation.
//
//*********************************************************

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Service.h"
#include "CalibData/Tof/TofSimData.h"
#include "TofSimSvc/TofSimSvc.h"

using namespace std;

TofSimSvc::TofSimSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
  declareProperty("Run",m_run=1);
}

StatusCode TofSimSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_ITofSimSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<ITofSimSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TofSimSvc::initialize ( ) {
  m_hasbeeninitialized=false;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;
  
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ){
    incsvc -> addListener(this, "NewRun", priority);
  }

  StatusCode scc;
  
  log<<MSG::INFO << "setProperties()" << endreq;
  scc = service("CalibDataSvc", m_pSimDataSvc, true);
  if ( !scc.isSuccess() ) {
    log<<MSG::ERROR<<"Could not get IDataProviderSvc interface of SimXmlCnvSvc"<<endreq;
    return scc;
  } else {
    log << MSG::DEBUG<< "Retrieved IDataProviderSvc interface of SimXmlCnvSvc"<<endreq;
  }
  // Get properties from the JobOptionsSvc
  scc = setProperties();
  
  return sc;
}  
  
StatusCode TofSimSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}

TofSimSvc::~TofSimSvc(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": Destructor function of TofSimSvc" << endreq;

  for(vector<TofConst*>::iterator it1 = fTofConst.begin(); it1 != fTofConst.end(); it1++) { delete (*it1); }
  fTofConst.clear();
  for(vector<BTofSim*>::iterator it2 = fBTofSim.begin(); it2 != fBTofSim.end(); it2++) { delete (*it2); }
  fBTofSim.clear();
  for(vector<ETofSim*>::iterator it3 = fETofSim.begin(); it3 != fETofSim.end(); it3++) { delete (*it3); }
  fETofSim.clear();

  return;
}

StatusCode TofSimSvc::FillfromDatabase(){

  MsgStream log(msgSvc(), name());    
  std::string fullPath = "/Calib/TofSim";
  log << MSG::INFO<<" Tof simulation fullPath = "<<fullPath<< endreq;
  SmartDataPtr<CalibData::TofSimData> test(m_pSimDataSvc, fullPath);
  if(!test){
    log << MSG::FATAL << "TofSimSvc could not find TofSimData in TDS!!" << endreq;
    return StatusCode::FAILURE;
  }       

  for(vector<TofConst*>::iterator it1 = fTofConst.begin(); it1 != fTofConst.end(); it1++) { delete (*it1); }
  fTofConst.clear();
  for(vector<BTofSim*>::iterator it2 = fBTofSim.begin(); it2 != fBTofSim.end(); it2++) { delete (*it2); }
  fBTofSim.clear();
  for(vector<ETofSim*>::iterator it3 = fETofSim.begin(); it3 != fETofSim.end(); it3++) { delete (*it3); }
  fETofSim.clear();

  TofConst* tofConst = new TofConst;
  tofConst->setBarLowThres( test->getBarLowThres() );
  tofConst->setBarHighThres( test->getBarHighThres() );
  tofConst->setEndLowThres( test->getEndLowThres() );
  tofConst->setEndHighThres( test->getEndHighThres() );
  tofConst->setBarPMTGain( test->getBarPMTGain() );
  tofConst->setEndPMTGain( test->getEndPMTGain() );
  tofConst->setBarConstant( test->getBarConstant() );
  tofConst->setEndConstant( test->getEndConstant() );
  tofConst->setEndNoiseSwitch( test->getEndNoiseSwitch() );
  fTofConst.push_back(tofConst);

  for( unsigned int id=0; id<176; id++ ) {
//    std::cout<<test->getBarAttenLength(id)<<std::endl;
    BTofSim* btof = new BTofSim;
    btof->setGain( test->getBarGain(id) );
    btof->setRatio( test->getBarRatio(id) );
    btof->setAttenLength( test->getBarAttenLength(id) );
    fBTofSim.push_back(btof);
  }

  for( unsigned int id=0; id<96; id++ ) {
    ETofSim* etof = new ETofSim;
    etof->setGain( test->getEndGain(id) );
    etof->setAttenLength( test->getEndAttenLength(id) );
    etof->setNoiseSmear( test->getEndNoiseSmear(id) );
    fETofSim.push_back(etof);
  }

  return StatusCode::SUCCESS;
}

void TofSimSvc::Dump(){
  std::cout<<"Now We can get the TOF Simulation Service"<<std::endl;
  return;
}

void TofSimSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "New Run" << endreq;
    StatusCode sc= FillfromDatabase();
    if(sc.isSuccess()){
      m_hasbeeninitialized=true;
    }
  }
  return;
}

const double TofSimSvc::BarLowThres() {
  return fTofConst[0]->getBarLowThres();
}

const double TofSimSvc::BarHighThres() {
  return fTofConst[0]->getBarHighThres();
}

const double TofSimSvc::EndLowThres() {
  return fTofConst[0]->getEndLowThres();
}

const double TofSimSvc::EndHighThres() {
  return fTofConst[0]->getEndHighThres();
}

const double TofSimSvc::BarPMTGain() {
  return fTofConst[0]->getBarPMTGain();
}

const double TofSimSvc::EndPMTGain() {
  return fTofConst[0]->getEndPMTGain();
}

const double TofSimSvc::BarConstant() {
  return fTofConst[0]->getBarConstant();
}

const double TofSimSvc::EndConstant() {
  return fTofConst[0]->getEndConstant();
}

const double TofSimSvc::EndNoiseSwitch() {
  return fTofConst[0]->getEndNoiseSwitch();
}

const double TofSimSvc::BarGain1(unsigned int id) {
  if( id < 176 ) {
    return fBTofSim[id]->getGain1();
  }
  else {
    std::cout<<"TofSimSvc::BarGain1: bad id="<<id<<std::endl;
    return 0.0;
  }
}

const double TofSimSvc::BarGain2(unsigned int id) {
  if( id < 176 ) {
    return fBTofSim[id]->getGain2();
  }
  else {
    std::cout<<"TofSimSvc::BarGain2: bad id="<<id<<std::endl;
    return 0.0;
  }
}

const double TofSimSvc::EndGain(unsigned int id) {
  if( id < 96 ) {
    return fETofSim[id]->getGain();
  }
  else {
    std::cout<<"TofSimSvc::EndGain: bad id="<<id<<std::endl;
    return 0.0;
  }
}

const double TofSimSvc::BarAttenLength(unsigned int id) {
  if( id < 176 ) {
    return fBTofSim[id]->getAttenLength();
  }
  else {
    std::cout<<"TofSimSvc::BarAttenLength: bad id="<<id<<std::endl;
    return 0.0;
  }
}

const double TofSimSvc::EndAttenLength(unsigned int id) {
  if( id < 96 ) {
    return fETofSim[id]->getAttenLength();
  }
  else {
    std::cout<<"TofSimSvc::EndAttenLength: bad id="<<id<<std::endl;
    return 0.0;
  }
}

const double TofSimSvc::EndNoiseSmear(unsigned int id) {
  if( id < 96 ) {
    return fETofSim[id]->getNoiseSmear();
  }
  else {
    std::cout<<"TofSimSvc::EndNoiseSmear: bad id="<<id<<std::endl;
    return 0.0;
  }
}
