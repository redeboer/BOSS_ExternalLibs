#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "HltAlgorithms/EFProcessCluster.h"

using namespace Event;

EFProcessCluster::EFProcessCluster(const std::string& name,ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator) {
  int output=(m_output%10000)/1000;
  //declareProperty("OutputLevel",m_output = MSG::NIL);
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);
  m_nshower = new CriteriaItemValue;
  m_acop = new CriteriaItemValue;
  m_acole = new CriteriaItemValue;
  m_emax1 = new CriteriaItemValue;
  m_emax2 = new CriteriaItemValue;
  m_emax3 = new CriteriaItemValue;
  m_emax12 = new CriteriaItemValue;
  m_coste1 = new CriteriaItemValue;
  m_coste2 = new CriteriaItemValue;
  m_coste3 = new CriteriaItemValue;
  m_phi1 = new CriteriaItemValue;
  m_phi2 = new CriteriaItemValue;
  m_phi3 = new CriteriaItemValue;
}

EFProcessCluster::~EFProcessCluster() {
  delete m_emax1;
  delete m_emax2;
  delete m_emax3;
  delete m_emax12;
  delete m_coste1;
  delete m_coste2;
  delete m_coste3;
  delete m_phi1;
  delete m_phi2;
  delete m_phi3;
  delete m_acop;
  delete m_acole;
  delete m_nshower;
}

StatusCode EFProcessCluster::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("nshw", m_nshower);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(nshw) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("acop", m_acop);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(acop) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("acole", m_acole);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(acole) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("emax1", m_emax1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(emax1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("emax2", m_emax2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(emax2) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("emax3", m_emax3);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(emax3) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("emax12", m_emax12);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(emax12) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("coste1", m_coste1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(cos1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("coste2", m_coste2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(cos2) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("coste3", m_coste3);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(cos3) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("phi1", m_phi1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(phi1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("phi2", m_phi2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(phi2) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("phi3", m_phi3);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(phi3) wrong" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode EFProcessCluster::execute() {
  
  reset();

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
  SmartDataPtr<RecEmcShowerCol> emcShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!emcShowerCol) {
    log << MSG::FATAL << "Could not find Emc rec!!" << endreq;
    return( StatusCode::FAILURE);
  }
  //Part 3: Calculate acoplanarity of two-max energy showers
  RecEmcShowerCol::iterator iterShower=emcShowerCol->begin();
  double max1=-999,max2=-999,max3=-999;
  double cost1=-999,cost2=-999,cost3=-999,phi1=-999,phi2=-999,phi3=-999;
  
  unsigned int nshower=emcShowerCol->size();
  for(;iterShower!= emcShowerCol->end();iterShower++){
    if((*iterShower)->energy()>=max1){
      max3=max2;
      max2=max1;
      cost3=cost2;
      cost2=cost1;
      phi3=phi2;
      phi2=phi1;
      max1=(*iterShower)->energy();
      cost1=cos((*iterShower)->position().theta());
      phi1=(*iterShower)->position().phi();
    }
    else if((*iterShower)->energy()>max2){
      max3=max2;
      cost3=cost2;
      phi3=phi2;
      max2=(*iterShower)->energy();
      cost2=cos((*iterShower)->position().theta());
      phi2=(*iterShower)->position().phi();
    }
    else if((*iterShower)->energy()>max3){
      max3=(*iterShower)->energy();
      cost3=(*iterShower)->position().theta();
      phi3=(*iterShower)->position().phi();
    }
  }
  double acop=180,acol=180.;
  if(nshower>=2){
    acop=180.-180./3.1415927*acos(cos(phi1)*cos(phi2)+sin(phi1)*sin(phi2));
    acol=180.-180./3.1415927*acos(cos(phi1)*sin(acos(cost1))*cos(phi2)*sin(acos(cost2))
                                  +sin(phi1)*sin(acos(cost1))*sin(phi2)*sin(acos(cost2))
                                  +cost1*cost2);
  }

  log << MSG::INFO << "nshower=" << nshower << "; acop=" << acop 
      <<"; emax1="<< max1  <<"; emax2="<< max2 <<endreq;

  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_nshower->setValue(nshower);
  m_acop->setValue(acop);
  m_acole->setValue(acol);
  m_emax1->setValue(max1/abs(m_beam));
  m_emax2->setValue(max2/abs(m_beam));
  m_emax3->setValue(max3/abs(m_beam));
  m_emax12->setValue((max1+max2)/abs(m_beam));
  m_coste1->setValue(cost1);
  m_coste2->setValue(cost2);
  m_coste3->setValue(cost3);
  m_phi1->setValue(phi1);
  m_phi2->setValue(phi2);
  m_phi3->setValue(phi3);

  m_ef->addToEFVec(nshower, 38);
  m_ef->appToEFVec(max1, 39);
  m_ef->appToEFVec(cost1,40);
  m_ef->appToEFVec(phi1,41);
  m_ef->appToEFVec(max2, 42);
  m_ef->appToEFVec(cost2,43);
  m_ef->appToEFVec(phi2,44);
  m_ef->appToEFVec(acop, 45);
  m_ef->appToEFVec(acol, 46);
  m_ef->appToEFVec(max1+max2, 47);
  m_ef->appToEFVec(max3, 48);
  m_ef->appToEFVec(cost3,49);
  m_ef->appToEFVec(phi3,50);
 
  m_ef->setVecBit(true, 0, 2);
  if(nshower==0)m_ef->addToEFVec(1<<16,1);
  else if(nshower==1)m_ef->addToEFVec(4<<16,1);
  else if(nshower==2)m_ef->addToEFVec(10<<16,1);
  else if(nshower>=3)m_ef->addToEFVec(13<<16,1);
  else m_ef->addToEFVec(0<<16,1);
  
  m_run=1;

  return StatusCode::SUCCESS;
}

StatusCode EFProcessCluster::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFProcessCluster::reset() {

  if(m_run) {
    m_nshower->reset();
    m_acop->reset();
    m_acole->reset();
    m_emax1->reset();
    m_emax2->reset();
    m_emax3->reset();
    m_emax12->reset();
    m_coste1->reset();
    m_coste2->reset();
    m_coste3->reset();
    m_phi1->reset();
    m_phi2->reset();
    m_phi3->reset();
    m_run=0;
  }
  return;
}
