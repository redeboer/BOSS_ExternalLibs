//*******************************
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/Service.h"
//#include "GaudiKernel/DataSvc.h"
#include "Identifier/Identifier.h"
#include "Identifier/EmcID.h"
#include "RawEvent/RawDataUtil.h"
#include "EmcRawEvent/EmcDigi.h"
//#include <exception>
// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt

#include "RawDataProviderSvc/EmcRawDataProvider.h"

EmcRawDataProvider::EmcRawDataProvider():
  RawDataProviderBase(),
  m_adcLowThreshold(0), m_tLowThreshold(0), m_tHighThreshold(60),
  m_hotCrystals(0), m_deadCrystals(0),
  m_emcDigiVec(){
  m_emcCalibConstSvc=0;
}

EmcRawDataProvider::EmcRawDataProvider(const char* name, uint32_t elow, uint32_t tlow, uint32_t thigh):
  RawDataProviderBase(name),
  m_adcLowThreshold(elow), m_tLowThreshold(tlow), m_tHighThreshold(thigh),
  m_hotCrystals(0), m_deadCrystals(0),
  m_emcDigiVec(){
  m_emcCalibConstSvc=0;
}

EmcRawDataProvider::EmcRawDataProvider(const char* name,std::vector<uint32_t>& hot, std::vector<uint32_t>& dead,
				       uint32_t elow, uint32_t tlow, uint32_t thigh):
  RawDataProviderBase(name),
  m_adcLowThreshold(elow), m_tLowThreshold(tlow), m_tHighThreshold(thigh),
  m_hotCrystals(hot), m_deadCrystals(dead),
  m_emcDigiVec(){
  m_emcCalibConstSvc=0;
}

void EmcRawDataProvider::handle(const Incident& inc){
  MsgStream log(m_msgSvc, m_name);
  log << MSG::DEBUG << "inc.type=" << inc.type() << endreq;
  if ( inc.type() == "BeginEvent" ){
    EmcDigiCol empty;
    m_emcDigiVec=empty;
  }
  return;
}

StatusCode EmcRawDataProvider::initialize(bool mode, ISvcLocator* pSvcLoc, IMessageSvc* pMsg){

  RawDataProviderBase::initialize(pSvcLoc,pMsg);
  // test cut value
  MsgStream log(m_msgSvc, m_name);
  if(log.level()<=MSG::INFO){
    std::cout << "EmcRawDataProvider::    " << "elow=" << m_adcLowThreshold << ", tlow=" << m_tLowThreshold
	      << ", thigh=" << m_tHighThreshold << std::endl;
    std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
    std::cout << "     hot crystals       ";
    for(;it!=m_hotCrystals.end();it++){
      std::cout << *it << "    ";  
    }
    std::cout << std::endl << "    dead crystals       ";
    for(it=m_deadCrystals.begin();it!=m_deadCrystals.end();it++){
      std::cout << *it << "    ";
    }
    std::cout << std::endl;
  }

  // get point of EMC calibration service
  StatusCode sc = m_svcLocator->service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Can't get EmcCalibConstSvc." << endreq;
    m_emcCalibConstSvc=0;
  }
  else if(mode)   m_emcCalibConstSvc->getDigiCalibConst(0);

  return StatusCode::SUCCESS;
}

bool EmcRawDataProvider::isGoodEmcDigi(uint32_t control, uint32_t id, uint32_t measure,
				       uint32_t adc, uint32_t tdc){
  if(control&DropLow) {
    if(measure==0&&adc<m_adcLowThreshold) return false;
  }
  if(control&DropFull) {
    if(measure==3) return false;
  }
  if(control&CutTime) {
    if(tdc<m_tLowThreshold||tdc>m_tHighThreshold) return false;
  }
  if(control&DropHot) {
    std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
    for(;it!=m_hotCrystals.end();it++){
      if((*it)==id) return false;
    }
  }
  if(control&DropDead) {
    std::vector<uint32_t>::iterator it=m_deadCrystals.begin();
    for(;it!=m_deadCrystals.end();it++){
      if((*it)==id) return false;
    }
  }
  return true;
}

bool EmcRawDataProvider::isGoodEmcDigi(uint32_t control, EmcDigi* emcDigi){
  uint32_t adc=emcDigi->getChargeChannel();
  uint32_t measure=emcDigi->getMeasure();
  uint32_t tdc=emcDigi->getTimeChannel();
  Identifier id=emcDigi->identify();
  return isGoodEmcDigi(control,id.get_value(),measure,adc,tdc);
}

void EmcRawDataProvider::doCalib(Identifier& id, uint32_t& measure, uint32_t& adc){
  
  uint32_t part = EmcID::barrel_ec(id);
  uint32_t itheta = EmcID::theta_module(id);
  uint32_t iphi   = EmcID::phi_module(id);
  int index = m_emcCalibConstSvc->getIndex(part,itheta,iphi);
  double adc2e = m_emcCalibConstSvc->getDigiCalibConst(index);
  double e = RawDataUtil::EmcCharge(measure, adc)*adc2e;

  measure =RawDataUtil::EmcChargeMeasure(e);
  adc     =RawDataUtil::EmcChargeChannel(e);
}

void EmcRawDataProvider::doCalib(EmcDigi* emcDigi){
  Identifier id=emcDigi->identify();
  uint32_t adc=emcDigi->getChargeChannel();
  uint32_t measure=emcDigi->getMeasure();
  doCalib(id,measure,adc);
  emcDigi->setChargeChannel(adc);
  emcDigi->setMeasure(measure);
}

EmcDigiCol& EmcRawDataProvider::getEmcDigiVec(uint32_t control){
  MsgStream log(m_msgSvc, m_name);
  log << MSG::INFO << "EmcRawDataProvider::getEmcDigiVec..." << endreq;
  log << MSG::DEBUG << "vector size=" << m_emcDigiVec.size() << endreq;

  if(m_emcDigiVec.size()>0){
    if(control&Redo) m_emcDigiVec=EmcDigiCol();
    else             return m_emcDigiVec;
  }
  // Retrieve Hits Collection 
    // tianhl for mt
    std::string evtDataSvc_name("EventDataSvc");
    if(isGaudiThreaded(m_name)){
      evtDataSvc_name += getGaudiThreadIDfromName(m_name);
    }
    // tianhl for mt
  IDataProviderSvc* evtSvc;
  StatusCode sc = m_svcLocator->service(evtDataSvc_name.c_str(),evtSvc,true);
  if (!sc.isSuccess()){
    log << MSG::FATAL << "EmcRawDataProvider: ERROR Could not load EventDataSvc" << endreq;
  }

  SmartDataPtr<EmcDigiCol> emcDigiCol(evtSvc,"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
    log << MSG::FATAL << "Could not find Emc digi!!" << endreq;
    return m_emcDigiVec;
  }
  log << MSG::DEBUG << "start dealing with EmcRawCol " << emcDigiCol->size() << endreq; 
  EmcDigiCol::iterator iterEMC=emcDigiCol->begin();
  for(;iterEMC!= emcDigiCol->end();iterEMC++) {
    log << MSG::NIL << "vector size=" << m_emcDigiVec.size() << endreq;
    if(isGoodEmcDigi(control,*iterEMC)) m_emcDigiVec.insert(m_emcDigiVec.end(),*iterEMC);
    if(control&DoCalib) {
      if(m_emcCalibConstSvc!=0) doCalib(*iterEMC);
      else {
	log << MSG::WARNING << "require EMC calibration, but none service, ignore." << endreq; 
      }
    }
    if(control&DoOther) { }
  }
  log << MSG::VERBOSE << "ready for return" << endreq;
  return m_emcDigiVec;
}

bool EmcRawDataProvider::removeHotCrystal(uint32_t teid){
  std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
  for(;it!=m_hotCrystals.end();it++){
    if((*it)==teid) {
      m_hotCrystals.erase(it);
      break;
    }
  }
  if(it==m_hotCrystals.end()) return false;

  return true;
}

bool EmcRawDataProvider::removeDeadCrystal(uint32_t teid){
  std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
  for(;it!=m_deadCrystals.end();it++){
    if((*it)==teid) {
      m_deadCrystals.erase(it);
      break;
    }
  }
  if(it==m_deadCrystals.end()) return false;
  
  return true;
}

void EmcRawDataProvider::addHotCrystal(uint32_t teid){
  std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
  for(;it!=m_hotCrystals.end();it++){
    if((*it)==teid) break;
  }
  if(it==m_hotCrystals.end()) m_hotCrystals.push_back(teid);

  return;
}
 
void EmcRawDataProvider::addDeadCrystal(uint32_t teid){
  std::vector<uint32_t>::iterator it=m_hotCrystals.begin();
  for(;it!=m_deadCrystals.end();it++){
    if((*it)==teid) break;
  }
  if(it==m_deadCrystals.end()) m_deadCrystals.push_back(teid);
  
  return;
}
