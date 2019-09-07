#include <cmath>
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
#include "EmcRawEvent/EmcDigi.h"
#include "Identifier/EmcID.h"
#include "RawEvent/RawDataUtil.h"
#include "HltAlgorithms/EFGlobalEnergy.h"

using namespace Event;

EFGlobalEnergy::EFGlobalEnergy(const std::string& name, ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator) {
  //declareProperty("OutputLevel",m_output = MSG::NIL);
  int output=(m_output%100000)/10000;
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);
  m_etot = new CriteriaItemValue;
  m_ebar = new CriteriaItemValue;
  m_eend = new CriteriaItemValue;
  m_ebal = new CriteriaItemValue;
}

EFGlobalEnergy::~EFGlobalEnergy() {
  delete m_etot;
  delete m_ebar;
  delete m_eend;
  delete m_ebal;
}

StatusCode EFGlobalEnergy::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("etot", m_etot);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(etot) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("ebar", m_ebar);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(ebar) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("eend", m_eend);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(eend) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("ebal", m_ebal);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(ebal) wrong" << endreq;
    return sc;
  }

  sc = service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "Can't get EmcCalibConstSvc." << endreq;
    m_emcCalibConstSvc=0;
  }

  if(m_rawDigiSvc){
    if(m_rawDigiSvc->isOnlineMode()){
      char cbeam[20];
      sprintf(cbeam,"%f", m_beam);
      setenv("BEPCII_INFO.BER_PRB",cbeam,0);
      setenv("BEPCII_INFO.BPR_PRB",cbeam,0);
    }
  }
  return sc;
}

StatusCode EFGlobalEnergy::execute() {

  //cout<<"m_run="<<m_run<<endl;
  //reset();

  MsgStream log(msgSvc(), name());
  //DataObject*  pObject;
  float e1=0,e2=0;
  char* electron=getenv("BEPCII_INFO.BER_PRB");
  if(electron){
    e1=atof(electron);
  }
  else{
    log << MSG::ERROR << "Cannot get beam energy (e-)! Please call online people" << endreq;
  }
  char* positron=getenv("BEPCII_INFO.BPR_PRB");
  if(positron){
    e2=atof(positron);
  }
  else{
    log << MSG::ERROR << "Cannot get beam energy (e+)! Please call online people" << endreq;
  }
  if(e1>0.1&&e2>0.1&&m_beam>0) {
    m_beam = 0.5*(e1+e2);
  }
  else{
 
  }
  log << MSG::INFO << "beam energy = " << m_beam << endreq;
  
  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }

  //Part 2: Retrieve Hits Collection 
  //vector<EmcDigi*>* emcDigiCol=0;
  bool calFlag=false;
  EmcDigiCol* emcDigiCol=0;
  if(m_rawDigiSvc){
    if(m_rawDigiSvc->isOnlineMode()){
      emcDigiCol=&(m_rawDigiSvc->getEmcDigiVec(EmcRawDataProvider::DropLow|EmcRawDataProvider::CutTime|EmcRawDataProvider::DropHot|EmcRawDataProvider::DropDead|EmcRawDataProvider::Redo));
      calFlag = true;
    }
  }
  if(!emcDigiCol){
    SmartDataPtr<EmcDigiCol> emcDigi(eventSvc(),"/Event/Digi/EmcDigiCol");
    if (!emcDigi) {
      log << MSG::FATAL << "Could not find Emc digi!!" << endreq;
      return( StatusCode::FAILURE);
    }
    //emcDigiCol=dynamic_cast<vector<EmcDigi*>*> (&emcDigi);
    emcDigiCol=emcDigi;
  }
  //Part 3:Calculate total deposited energy and energy balance 
  Identifier id;
  double adc,etot=0.,ebarrel=0.,eendcap=0.; 
  double energyx=0.,energyy=0.,energyz=0.;
  double ebalance;
  unsigned int idBarrel_Endcap,itheta,iphi;
  double ewest=0.,eeast=0.;
  //double tdc;

  EmcDigiCol::iterator iterEMC=emcDigiCol->begin();
  while(iterEMC!= emcDigiCol->end()) {
    id=(*iterEMC)->identify();
    idBarrel_Endcap=EmcID::barrel_ec(id);
    itheta=EmcID::theta_module(id);
    iphi=EmcID::phi_module(id);
    adc=RawDataUtil::EmcCharge((*iterEMC)->getMeasure(),(*iterEMC)->getChargeChannel());
    adc /= 1000. ; //Mev->GeV
    if(!calFlag&&m_emcCalibConstSvc) {
      int index = m_emcCalibConstSvc->getIndex(idBarrel_Endcap,itheta,iphi);
      double adc2e = m_emcCalibConstSvc->getDigiCalibConst(index);
      log << MSG::DEBUG << "adc= " << adc << " and calibration constant: " << adc2e << " at " << idBarrel_Endcap << " " << itheta << " " << iphi << endreq;
      adc *= adc2e;
    }
    //tdc=(*iterEMC)->getTimeChannel();            !not used
    etot += adc;
    double theta=0,phi=0;
    //Estimate hits position(theta,phi)
    if(idBarrel_Endcap==1){
      theta=(34.+112.*(itheta+0.5)/44.)/180.*3.1415926;
      phi=(iphi+0.5)/120.*6.2831852;
      ebarrel +=adc;
      if(itheta<22)  eeast+=adc;
      else           ewest+=adc;
    }
    else{
      theta=(90.+(90.-(itheta+0.5)/6.*(34.-21.56)+21.56)*(idBarrel_Endcap-1))
	/180.*3.1415926;
      if(itheta==0||itheta==1)phi=(iphi+0.5)/64.*6.2831852;
      if(itheta==2||itheta==3)phi=(iphi+0.5)/80.*6.2831852;
      if(itheta==4||itheta==5)phi=(iphi+0.5)/96.*6.2831852;
      eendcap +=adc;
      if(idBarrel_Endcap==0) {
        eeast+=adc;
      }
      else {
        ewest+=adc;
      }
    }
    energyx +=adc*sin(theta)*cos(phi);
    energyy +=adc*sin(theta)*sin(phi);
    energyz +=adc*cos(theta);
    //log << MSG::DEBUG << "Energy:" << idBarrel_Endcap <<","<<itheta << "," << iphi 
    //	<< ":" << adc <<endreq;
    iterEMC++;
  }
  if(etot>0)
    ebalance=sqrt(energyx*energyx+energyy*energyy+energyz*energyz)/etot;
  else
    ebalance=0;

  log << MSG::INFO << "etot=" << etot << "(" << etot/2./abs(m_beam) << "); ebarrel=" <<ebarrel 
      <<"; eendcap="<<eendcap  <<"; ebalanece="<<ebalance<<endreq;

  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_etot->setValue(etot/2./abs(m_beam));
  m_ebar->setValue(ebarrel/2./abs(m_beam));
  m_eend->setValue(eendcap/2./abs(m_beam));
  m_ebal->setValue(ebalance);

  m_ef->appToEFVec(etot, 31);
  m_ef->appToEFVec(ebalance,34);
  m_ef->appToEFVec(ebarrel,32);
  m_ef->appToEFVec(eendcap,33);
  m_ef->appToEFVec(eeast,35);
  m_ef->appToEFVec(ewest,36);
  m_ef->setVecBit(true, 0, 1);
  m_ef->addToEFVec(7<<16,1);
  m_run=1;

  return StatusCode::SUCCESS;
}

StatusCode EFGlobalEnergy::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFGlobalEnergy::reset() {
  
  if(m_run) {
    m_etot->reset();
    m_ebar->reset();
    m_eend->reset();
    m_ebal->reset();
    m_run=0;
  }
  return;
}
