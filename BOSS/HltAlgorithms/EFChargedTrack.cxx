#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/ISvcLocator.h"
//#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "HltAlgorithms/EFChargedTrack.h"

using namespace Event;

EFChargedTrack::EFChargedTrack(const std::string& name, ISvcLocator* pSvcLocator) :
  IEFAlgorithm(name, pSvcLocator){
  int output = (m_output%100)/10;
  //declareProperty("OutputLevel",m_output = MSG::NIL);
  MsgStream log(msgSvc(), name);
  msgSvc()->setOutputLevel(name,output);
  m_ntrk = new CriteriaItemValue;
  m_acol = new CriteriaItemValue;
  m_mbal = new CriteriaItemValue;
  m_pmax1 = new CriteriaItemValue;
  m_pmax2 = new CriteriaItemValue;
  m_cost1 = new CriteriaItemValue;
  m_cost2 = new CriteriaItemValue;
  m_vr    = new CriteriaItemValue;
  m_vz    = new CriteriaItemValue;
}

EFChargedTrack::~EFChargedTrack() {
  delete m_ntrk;
  delete m_acol;
  delete m_mbal;
  delete m_pmax1;
  delete m_pmax2;
  delete m_cost1;
  delete m_cost2;
  delete m_vr;
  delete m_vz;
}

StatusCode EFChargedTrack::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  IEFAlgorithm::initialize();

  StatusCode sc;
  sc = m_HltStoreSvc->put("ntrk", m_ntrk);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(ntrk) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("acol", m_acol);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(acol) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("mbal", m_mbal);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(mbal) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("pmax1", m_pmax1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(pmax1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("pmax2", m_pmax2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(pmax2) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("cost1", m_cost1);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(cost1) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("cost2", m_cost2);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(cost2) wrong" << endreq;
    return sc;
  }  
  sc = m_HltStoreSvc->put("vr", m_vr);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(vr) wrong" << endreq;
    return sc;
  }
  sc = m_HltStoreSvc->put("vz", m_vz);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "m_HltStoreSvc->put(vz) wrong" << endreq;
    return sc;
  }

  log << MSG::DEBUG << "finish initialize()" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode EFChargedTrack::execute() {
  
  reset();

  MsgStream log(msgSvc(), name());
  //DataObject*  pObject;
  
  // Part 1: Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }

  //Part 2: Retrieve Track Collection
  SmartDataPtr<RecMdcTrackCol> mdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
  if (!mdcTrackCol) {
    log << MSG::FATAL << "Could not find Mdc track collection!!" << endreq;
    return( StatusCode::FAILURE);
  }
  //Part 3: 
  RecMdcTrackCol::iterator iterTrk=mdcTrackCol->begin();
  double max1=0.;
  double max2=0.;
  double cost1=-999,cost2=-999,phi1=-999,phi2=-999;
  int hit1=-999,hit2=-999,shit1=-999,shit2=-999,charg1=-999,charg2=-999;
  double mdcbalance=-999;

  unsigned int ntrk=mdcTrackCol->size();
  double kappa=-999,tanl=-999,sint=-999;
  double p=-999,theta=-999,phi=-999;
  double vz1=-999,vz2=-999,vr1=-999,vr2=-999;

  //double theta1=0,theta2=0;
  for(;iterTrk!= mdcTrackCol->end();iterTrk++) {
    //z0=(*iterTrk)->getZ0();
    //x0=(*iterTrk)->getX0();
    //y0=(*iterTrk)->getY0();
    phi=(*iterTrk)->helix(1);
    kappa=(*iterTrk)->helix(2);
    tanl=(*iterTrk)->helix(4);

    theta=0.5*3.1415926-atan(tanl);
    sint=sin(theta);
    //log << MSG::DEBUG << "FastTrk=>("<<x0<<","<<y0<<","<<z0")=>"
    //  <<(*iterTrk)->getId() << ":"<<(*iterTrk)->getPivot()<<":"
    //  <<kappa<<">"<<tanl<<endreq;
    if(abs(kappa)>0.001&&abs(sint)>0.01){
      p=abs(1./kappa)/sint;
    }
    else{
      p=1000.;
      log << MSG::WARNING << "FastTrk=>"<<" kappa=" <<kappa<<"; sint="<<sint<<endreq; 
    }
    if(p>=max1){
      max2=max1;
      cost2=cost1;
      phi2=phi1;
      max1=p;
      cost1=cos(theta);
      phi1=phi;
      vz2=vz1;
      vz1=(*iterTrk)->helix(3);
      vr2=vr1;
      vr1=(*iterTrk)->helix(0);
      hit2=hit1;
      hit1=(*iterTrk)->getNhits();
      shit2=shit1;
      shit1=(*iterTrk)->nster();
      charg2=charg1;
      charg1=(*iterTrk)->charge();
    }
    else if(p>max2){
      max2=p;
      cost2=cos(theta);
      phi2=phi;
      vz2=(*iterTrk)->helix(3);
      vr2=(*iterTrk)->helix(0);
      hit2=(*iterTrk)->getNhits();
      shit2=(*iterTrk)->nster();
      charg2=(*iterTrk)->charge();
    }
    log << MSG::DEBUG << "p=" <<p <<", "<<"theta="<<theta
	<<", phi="<<phi<<", vz="<<(*iterTrk)->helix(3)<<", vr="<<(*iterTrk)->helix(0)<<endreq;
    if(cos(theta)>0) mdcbalance +=1.;
    else if(cos(theta)<0) mdcbalance -=1.;
  }
  if(ntrk>=2) mdcbalance /= ntrk;
  else mdcbalance = 1;

  double acol=180.;
  if(ntrk>=2){
    acol=180.-180./3.1415926*acos(cos(phi1)*sin(acos(cost1))*cos(phi2)*sin(acos(cost2))
				  +sin(phi1)*sin(acos(cost1))*sin(phi2)*sin(acos(cost2))
				  +cost1*cost2);
  }

  log << MSG::INFO << "ntrk=" << ntrk << "; mdc balance=" <<mdcbalance 
      <<"; pmax1="<< max1  <<"; pmax2="<< max2
      <<"; acol="<< acol<<"; cost1="<<cost1<<"; cost2="<<cost2<<endreq;

  //Part 4: Put the criteria item(s) to HltStoreSvc here
  m_ntrk->setValue(ntrk);
  m_acol->setValue(acol);
  m_mbal->setValue(mdcbalance);
  m_pmax1->setValue(max1);
  m_pmax2->setValue(max2);
  m_cost1->setValue(cost1);
  m_cost2->setValue(cost2);
  m_vr->setValue(vr1);
  m_vz->setValue(vz1);

  m_ef->addToEFVec(ntrk, 6);
  m_ef->appToEFVec(max1, 7);
  m_ef->appToEFVec(cost1, 8);
  m_ef->appToEFVec(vz1, 9);
  m_ef->appToEFVec(phi1, 10);
  m_ef->appToEFVec(vr1, 11);
  m_ef->addToEFVec(hit1, 12);
  m_ef->addToEFVec(shit1, 13);
  m_ef->appToEFVec(max2, 14);
  m_ef->appToEFVec(cost2, 15);
  m_ef->appToEFVec(vz2, 16);
  m_ef->appToEFVec(phi2, 17);
  m_ef->appToEFVec(vr2, 18);
  m_ef->addToEFVec(hit2, 19);
  m_ef->addToEFVec(shit2, 20);
  m_ef->appToEFVec(acol, 21);
  m_ef->setVecBit(true, 0, 4);
  if(ntrk==0) m_ef->addToEFVec(1, 1);
  else if(ntrk==1) m_ef->addToEFVec(8, 1);
  else if(ntrk>=2) m_ef->addToEFVec(16,1);
  else m_ef->addToEFVec(0, 1);
 
  m_ef->addToEFVec(charg1, 23);
  m_ef->addToEFVec(charg2, 24);

  m_run=1;

  return StatusCode::SUCCESS;
}

StatusCode EFChargedTrack::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

void EFChargedTrack::reset() {
  //Reset
  if(m_run){
    m_ntrk->reset();
    m_acol->reset();
    m_mbal->reset();
    m_pmax1->reset();
    m_pmax2->reset();
    m_cost1->reset();
    m_cost2->reset();
    m_vr->reset();
    m_vz->reset();
    m_run=0;
  }
  return;
}
