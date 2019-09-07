#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
#include "MdcRawEvent/MdcDigi.h"
#include "Identifier/MdcID.h"
#include "HltAlgorithms/EFSectorHits.h"
#include "HltDataTypes/CriteriaItemValue.h"

using namespace Event;

int EFSectorHits::idmax[43]={40,44,48,56,64,72,80,80,76,76,
			     88,88,100,100,112,112,128,128,140,140,
			     160,160,160,160,176,176,176,176,208,208,
			     208,208,240,240,240,240,256,256,256,256,
			     288,288,288};

EFSectorHits::EFSectorHits(const std::string& name, ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator) {
  int output= (m_output%1000)/100;
  //declareProperty("OutputLevel",m_output = MSG::NIL);
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);
  m_nsec = new CriteriaItemValue;
}

EFSectorHits::~EFSectorHits() {
  delete m_nsec;
}

StatusCode EFSectorHits::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("nsec", m_nsec);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(nsec) wrong" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode EFSectorHits::execute() {
  
  //reset();
  MsgStream log(msgSvc(), name());
  int32_t nshower=m_ef->getEFVec(38);
  if(nshower<2)  return StatusCode::SUCCESS;

  //DataObject*  pObject;
 
  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  //Part 2: Retrieve Hits Collection 
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
  if (!mdcDigiCol) {
    log << MSG::FATAL << "EmcRec could not find Mdc digi!!" << endreq;
    return( StatusCode::FAILURE);
  }
  //Part 3: Get information of hits
  unsigned int nsec=0;
  Identifier id;
  MdcDigiCol::iterator iter=mdcDigiCol->begin();

  CriteriaItemValue* p1;
  CriteriaItemValue* p2;
  float              phi1,phi2;
  if((!m_HltStoreSvc->get("phi1", p1))||(!m_HltStoreSvc->get("phi2", p2)))return false;
  if((!(p1->getValue(phi1)))||(!(p2->getValue(phi2)))) return false;
  //phi(-pi,pi)==>(0,2pi)
  if(phi1<0) phi1+=6.2831852;
  if(phi2<0) phi2+=6.2831852;
  //Decide sector (phi11,phi12) and (phi21,phi22)
  float phi11=min(phi1,phi2);
  float phi22=max(phi1,phi2);
  float phi12=(phi11+phi22-3.1415926)*0.5;
  float phi21=(phi11+phi22+3.1415926)*0.5;
  if(phi12<0.) phi12 += 6.2831852;
  if(phi21>6.2831852) phi21 -= 6.2831852;
  for(;iter!= mdcDigiCol->end();iter++) {
    id=(*iter)->identify();
    unsigned int iphi=MdcID::wire(id);
    unsigned int ilayer=MdcID::layer(id);
    if(ilayer>=43)
      log << MSG::ERROR << "MDC(" << ilayer <<","<<iphi <<endreq;
    float phi=6.2831853*iphi/idmax[ilayer];
    if(WhetherSector(phi,phi11,phi12)||WhetherSector(phi,phi21,phi22)) nsec++;
  }
  log << MSG::INFO << " Number of hits in sector of MDC is " << nsec <<endreq;
  
  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_nsec->setValue(nsec);
  m_ef->setVecBit(true, 0, 3);
  m_ef->addToEFVec(9,37);
  if(m_ef->getEFVec(37)!=0) log << MSG::WARNING << "nsec(0) !=0"<< endreq;
  m_ef->addToEFVec(nsec,37);

  m_run=1;

  return StatusCode::SUCCESS;
}

StatusCode EFSectorHits::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFSectorHits::reset() {

  if(m_run){
    m_nsec->reset();
    m_run=0;
  }
  return;
}

bool EFSectorHits::WhetherSector(float ph,float ph1,float ph2){
  float phi1=min(ph1,ph2);
  float phi2=max(ph1,ph2);
  float delta=0.0610865; //3.5*3.1415926/180.
  if((phi2-phi1)<3.1415926){
    phi1 -=delta;
    phi2 +=delta;
    if(phi1<0.) phi1 += 6.2831852;
    if(phi2>6.2831852) phi2 -= 6.2831852;
    float tmp1=min(phi1,phi2);
    float tmp2=max(phi1,phi2);
    phi1=tmp1;
    phi2=tmp2;
  }
  else{
    phi1 +=delta;
    phi2 -=delta;
  }
  if((phi2-phi1)<3.1415926){
    if(ph<=phi2&&ph>=phi1) return true;
    else   return false;
  }
  else{
    if(ph>=phi2||ph<=phi1) return true;
    else   return false;
  }
}
