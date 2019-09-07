#include <cmath>
#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
//#include "TofRawEvent/TofDigi.h"
//#include "Identifier/TofID.h"
//#include "RawEvent/RawDataUtil.h"
#include "RawDataProviderSvc/TofData.h"

#include "HltAlgorithms/EFFlightTime.h"

//using namespace Event;
typedef std::vector<EFTofHitsCol> VTOF;

EFFlightTime::EFFlightTime(const std::string& name, ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator) {
  int output = (m_output%10)/1;
  //declareProperty("OutputLevel",m_output = MSG::NIL);
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);
  m_dtof1 = new CriteriaItemValue;
  m_dphi1 = new CriteriaItemValue;
  m_dtof2 = new CriteriaItemValue;
  m_dphi2 = new CriteriaItemValue;
}

EFFlightTime::~EFFlightTime() {
  delete m_dtof1;
  delete m_dphi1;
  delete m_dtof2;
  delete m_dphi2;
}

StatusCode EFFlightTime::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("dtof1", m_dtof1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(dtof1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("dphi1", m_dphi1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(dphi1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("dtof2", m_dtof2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(dtof2) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("dphi2", m_dphi2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(dphi2) wrong" << endreq;
    return sc;
  }

  log << MSG::DEBUG << "finish initialize()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode EFFlightTime::execute() {

  reset();

  MsgStream log(msgSvc(), name());
  //DataObject*  pObject;

  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  
  //Part 2: Retrieve Hits Collection 
  //SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
  //if (!tofDigiCol) {
  //  log << MSG::FATAL << "EmcRec could not find Tof digi!!" << endreq;
  //  return( StatusCode::FAILURE);
  //}
  //Part 3: Calculate time difference and back-to-back
  //TofDigiCol::iterator iterTOF=tofDigiCol->begin();
  VTOF topTOF;
  VTOF bottomTOF;
  topTOF.clear();
  bottomTOF.clear();
  Identifier id;
  unsigned int idBarrel_Endcap,iphi;
  double tdc;

  TofDataVector tofDataVec=m_rawDigiSvc->tofDataVectorOnlineMode();
  //TofDataVec tofDataVec=m_rawDigiSvc->tofDataVector(false,false,false,true);
  if(tofDataVec.size()>1){
    TofDataVector::iterator iterTOF=tofDataVec.begin();
    for(;iterTOF!= tofDataVec.end();iterTOF++) {
      if((*iterTOF)->barrel()) {
	idBarrel_Endcap=1;
	int id=(*iterTOF)->tofId();
	iphi = id%88;
	int ilayer = id/88;
	double t1=(*iterTOF)->tdc1();
	double t2=(*iterTOF)->tdc2();
	double tof=-999;
	if(t1<1500&&t2<1500&&t1>0&&t2>0)  tof=0.5*(t1+t2);
	else if(t1<1500&&t1>0)   tof=t1;
	else if(t2<1500&&t2>0)   tof=t2;
	else            continue;
	log <<MSG::DEBUG<< "hit: " <<"("<<idBarrel_Endcap<<","<<ilayer<<","<<iphi<<")-->"<< tof <<" ns"<<endreq;
	//if(idBarrel_Endcap!=1&&idBarrel_Endcap!=0&&idBarrel_Endcap!=2)
	//log << MSG::WARNING <<"TOF(" <<idBarrel_Endcap <<","<< ilayer << "," << iphi << ")=>"
	//    << tdc <<endreq;
	if(iphi<41&&iphi>2){
	  topTOF.push_back(EFTofHitsCol(idBarrel_Endcap,ilayer,iphi,tof));
	}
	else if(iphi>46&&iphi<85){
	  bottomTOF.push_back(EFTofHitsCol(idBarrel_Endcap,ilayer,iphi,tof));
	}
      }
      else{
	int iphi=(*iterTOF)->tofId();
	double tof=(*iterTOF)->tdc();
	if(iphi>=48) {
	  idBarrel_Endcap=2;
	  iphi %=48;
	}
	else  idBarrel_Endcap=0;
	if(iphi>1&&iphi<22&&tof>0&&tof<1500){
	  topTOF.push_back(EFTofHitsCol(idBarrel_Endcap,0,iphi,tof));
	}
	else if(iphi>25&&iphi<46&&tof>0&&tof<1500){
	  bottomTOF.push_back(EFTofHitsCol(idBarrel_Endcap,0,iphi,tof));
	}
      }    
    }
  }
  double dtof1=99;
  double dphi1=180;
  double dtof2=99;
  double dphi2=180;
  if(topTOF.size()>=1&&bottomTOF.size()>=1){
    for(unsigned int i=0;i<topTOF.size();i++){
      double topPhi=0.;
      if(topTOF[i].ib_e()==1){
	topPhi=topTOF[i].iphi()*360./88.;
      }
      else if(topTOF[i].ib_e()==0||topTOF[i].ib_e()==2){
	topPhi=topTOF[i].iphi()*360./48.;
      }
      else{
	log << MSG::ERROR << "TOF Barrel_Encap ID not right!" << endreq;
      }
      for(unsigned int j=0;j<bottomTOF.size();j++){
	double bottomPhi=0.;
	if(bottomTOF[j].ib_e()==1){
	  bottomPhi=bottomTOF[j].iphi()*360./88.;
	}
	else if(bottomTOF[j].ib_e()==0||bottomTOF[j].ib_e()==2){
	  bottomPhi=bottomTOF[j].iphi()*360./48.;
	}
	else{
	  log << MSG::ERROR << "TOF Barrel_Encap ID not right!" << endreq;
	}       
	double tmp1=fabs(topTOF[i].GetT()-bottomTOF[j].GetT());
	double tmp2=fabs(topTOF[i].GetT()-bottomTOF[j].GetT()+7.);
	if(tmp2<fabs(dtof2+7.)){
	  dtof2=topTOF[i].GetT()-bottomTOF[j].GetT();
	  dphi2=abs(fabs(topPhi-bottomPhi)-180.);
	}
        if(tmp1<fabs(dtof1)){
	  dphi1=abs(fabs(topPhi-bottomPhi)-180.);
	  dtof1=topTOF[i].GetT()-bottomTOF[j].GetT();
	}
      }
    }
  }

  log << MSG::INFO << "dtof1=" << dtof1 << ", dphi1=" << dphi1 
      << "dtof2=" << dtof2 << ", dphi2=" << dphi2 <<endreq;

  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_dtof1->setValue(dtof1);
  m_dphi1->setValue(dphi1);
  m_dtof2->setValue(dtof2);
  m_dphi2->setValue(dphi2);
  m_ef->appToEFVec(dtof1, 26);
  m_ef->appToEFVec(dphi1, 27);
  m_ef->appToEFVec(dtof2, 28);
  m_ef->appToEFVec(dphi2, 29);
  m_ef->setVecBit(true, 0, 5);
  m_ef->addToEFVec(4<<8, 1);

  m_run=1;
  return StatusCode::SUCCESS;
}

StatusCode EFFlightTime::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFFlightTime::reset() {

  if(m_run){
    m_dtof1->reset();
    m_dphi1->reset();
    m_dtof2->reset();
    m_dphi2->reset();
    m_run=0;
  }
  return;
}

VTOF::iterator EFFlightTime::FindHit(VTOF& tof,
				     const unsigned int ilayer,const unsigned int iphi,const unsigned int ibe){
  VTOF::iterator it=tof.begin();
  for(;it!=tof.end();it++){
    if(ibe!=it->ib_e())continue;
    if(ilayer==it->ilayer()){
      if(iphi==it->iphi()) return it;
      if(iphi+1==it->iphi()) return it;
      if(iphi-1==it->iphi()) return it;
    }
    else{
      if(iphi==it->iphi()) return it;
      if(iphi+(it->ilayer()-ilayer)==it->iphi()) return it;
    }
  }
  return it;
}
