#include "HltAlgorithms/CheckEFResult.h"
#include "HltDataTypes/EFResult.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "HltDataTypes/CriteriaItemValue.h"
//#include "HltDataTypes/EFBesTimer.h"
#include <fstream>

using namespace Event;

////////////////////////////////////////////////////////////////////////

CheckEFResult::CheckEFResult(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_notpass()
{
  declareProperty("FilterOutput",m_outfile="lv2");
}

CheckEFResult::~CheckEFResult()
{
}

StatusCode CheckEFResult::initialize(){
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  StatusCode sc = service( "HltStoreSvc", m_HltStoreSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() << ": Unable to locate Service HltStoreSvc" << endreq;
    return sc;
  }

  sc = serviceLocator()->service("EventDataSvc", m_evtDataSvc, true);
  if (!sc.isSuccess()) {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataProviderSvc." << endreq;
    return sc;
  }
  
  NTuplePtr nt(ntupleSvc(),"FILE123/EventFilter");
  if ( nt ) m_tuple = nt;
  else {
    m_tuple=ntupleSvc()->book("FILE123/EventFilter",CLID_ColumnWiseTuple,"EventFilter");
    if( m_tuple ) {
      m_tuple->addItem ("alg", m_alg);
      m_tuple->addItem ("nevt",  m_nevt);

      m_tuple->addItem ("ntof", m_ntof);
      m_tuple->addItem ("nemc", m_nemc);
      m_tuple->addItem ("nmdc", m_nmdc);
      m_tuple->addItem ("nshw", m_nshower);
      m_tuple->addItem ("nsec", m_nsec);
      m_tuple->addItem ("mbal", m_mbalance);
      m_tuple->addItem ("type", m_evttype);
      //m_tuple->addItem ("time", m_filtertime);
      //m_tuple->addItem ("CPUt", m_CPUtime);

      m_tuple->addItem ("nmuc", m_nmuc);
      m_tuple->addItem ("etot", m_etot);
      m_tuple->addItem ("emax1", m_emax1);
      m_tuple->addItem ("emax2", m_emax2);
      m_tuple->addItem ("acop", m_acop);
      m_tuple->addItem ("ebal", m_ebalance);
      m_tuple->addItem ("ebar", m_ebarrel);
      m_tuple->addItem ("eend", m_eendcap);
      m_tuple->addItem ("ntrk", m_ntrk);
      m_tuple->addItem ("pmax1", m_pmax1);
      m_tuple->addItem ("pmax2", m_pmax2);
      m_tuple->addItem ("acol", m_acol);
      m_tuple->addItem ("cost1", m_cost1);
      m_tuple->addItem ("cost2", m_cost2);
      m_tuple->addItem ("dtof1", m_dtof1);
      m_tuple->addItem ("dphi1", m_dphi1);
      m_tuple->addItem ("dtof2", m_dtof2);
      m_tuple->addItem ("dphi2", m_dphi2);
    }
    else    {   // did not manage to book the N tuple....
      log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }

  /*
  sc = service( "BesTimerSvc", m_timersvc);
  if( sc.isFailure() ) {
    log << MSG::WARNING << name() << ": Unable to locate BesTimer Service" << endreq;
    return StatusCode::FAILURE;
  }

  m_timer[0] = m_timersvc->addItem("PrueHandler");
  m_timer[0]->propName("nPrueHandler");
  pBesTimer=m_timer[0];
  m_timer[1] = m_timersvc->addItem("WholePro");
  m_timer[1]->propName("nWholePro");
  m_timer[1]->start();
  
  log << MSG::INFO << pBesTimer<<endreq;
  */
  return StatusCode::SUCCESS;
}

StatusCode CheckEFResult::execute() {

  MsgStream log(msgSvc(), name());
  //log << MSG::INFO << "in execute()" << endreq;

  EFResult* efResult;
  StatusCode sc = m_HltStoreSvc->get("EFResult", efResult);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << " Could not find EFResult" << endreq;
    return sc;
  }

  if (efResult->getAnswerIndex() == -1) {
    log << MSG::ERROR << " Unvalid efResult(AnswerIndex==-1)" << endreq;
    return StatusCode::FAILURE;
  }

  std::string answer = efResult->getAnswer();
  // answer should be "Accepted" or "Rejected" (or "Error")
  log << MSG::INFO << "this event is " << answer << endreq;

  if (answer == "Accepted") {
    EFResult* evtType;
    sc = m_HltStoreSvc->get("EventType", evtType);
    if ( sc.isFailure() ) {
      log << MSG::ERROR << " Could not find EventType" << endreq;
      return sc;
    }
    if (evtType->getAnswerIndex() == -1) {
      log << MSG::ERROR << " Unvalid evtType(AnswerIndex==-1)" << endreq;
      return StatusCode::FAILURE;
    }
    std::string typeStr = evtType->getAnswer();
    log << MSG::INFO << "   ==> it's marked as " << typeStr << endreq;

    // Event Type
    if(typeStr=="Junk") m_evttype=0;
    else if(typeStr=="GJunk") m_evttype=1;
    else if(typeStr=="Beamgas") m_evttype=2;
    else if(typeStr=="GBeamgas") m_evttype=3;
    else if(typeStr=="Cosmic") m_evttype=4;
    else if(typeStr=="GCosmic") m_evttype=5;
    else if(typeStr=="EBhabha") m_evttype=6;
    else if(typeStr=="GEBhabha") m_evttype=7;
    else if(typeStr=="BBhabha") m_evttype=8;
    else if(typeStr=="GBBhabha") m_evttype=9;
    else if(typeStr=="Dimuon") m_evttype=10;
    else if(typeStr=="GDimuon") m_evttype=11;
    else if(typeStr=="Diphoton") m_evttype=12;
    else if(typeStr=="GDiphoton") m_evttype=13;
    else if(typeStr=="Hadron") m_evttype=14;
    else if(typeStr=="GHadron") m_evttype=15;
    else if(typeStr=="Twophoton") m_evttype=16;
    else if(typeStr=="GTwophoton") m_evttype=17;
    else if(typeStr=="RandomTrg") m_evttype=31;
    else if(typeStr=="Error") m_evttype=30;
    else if(typeStr=="Other") m_evttype=29;
    else m_evttype=-1;
  }
  log << MSG::DEBUG << "get event type info: " << m_evttype << endreq;
  //Event No.
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return (StatusCode::FAILURE);
  }
  m_nevt= eventHeader->eventNumber();
  log << MSG::INFO << "EventNo: " << m_nevt <<endl;
  if(m_evttype==1||m_evttype==3||m_evttype==5) m_notpass.push_back(eventHeader->eventNumber());
  
  // Run time
  //m_timer[1]->stop();
  //m_filtertime = m_timer[1]->elapsed();

  //if(pBesTimer){
  //  pBesTimer->stop();
  //  m_CPUtime =  pBesTimer->elapsed();
  //}

  // Detector information
  float nmdc,ntof,nemc,nmuc,nshower,ntrk,nsec;
  float etot,ebar,eend,ebal,acop,emax1,emax2,dtof1,dphi1,dtof2,dphi2,acol,mbal,pmax1,pmax2;
  
  CriteriaItemValue* pcri;
  
  sc = m_HltStoreSvc->get("nmdc", pcri);
  if ( sc.isFailure() ) nmdc=-9;
  else pcri->getValue(nmdc);
  sc = m_HltStoreSvc->get("nmuc", pcri);
  if ( sc.isFailure() ) nmuc=-9;
  else pcri->getValue(nmuc);
  sc = m_HltStoreSvc->get("ntof", pcri);
  if ( sc.isFailure() ) ntof=-9;
  else pcri->getValue(ntof);
  sc = m_HltStoreSvc->get("nemc", pcri);
  if ( sc.isFailure() ) nemc=-9;
  else pcri->getValue(nemc);

  sc = m_HltStoreSvc->get("nshw", pcri);
  if ( sc.isFailure() ) nshower=-9;
  else  pcri->getValue(nshower);
  sc = m_HltStoreSvc->get("ntrk", pcri);
  if ( sc.isFailure() ) ntrk=-9;
  else  pcri->getValue(ntrk);
  sc = m_HltStoreSvc->get("etot", pcri);
  if ( sc.isFailure() ) etot=-9;
  else  pcri->getValue(etot);
  sc = m_HltStoreSvc->get("ebar", pcri);
  if ( sc.isFailure() ) ebar=-9;
  else  pcri->getValue(ebar);
  sc = m_HltStoreSvc->get("eend", pcri);
  if ( sc.isFailure() ) eend=-9;
  else  pcri->getValue(eend);
  sc = m_HltStoreSvc->get("ebal", pcri);
  if ( sc.isFailure() ) ebal=-9;
  else  pcri->getValue(ebal);
  sc = m_HltStoreSvc->get("acop", pcri);
  if ( sc.isFailure() ) acop=-9;
  else  pcri->getValue(acop);
  sc = m_HltStoreSvc->get("emax1", pcri);
  if ( sc.isFailure() ) emax1=-9;
  else  pcri->getValue(emax1);
  sc = m_HltStoreSvc->get("emax2", pcri);
  if ( sc.isFailure() ) emax2=-9;
  else  pcri->getValue(emax2);
  sc = m_HltStoreSvc->get("dtof1", pcri);
  if ( sc.isFailure() ) dtof1=-999;
  else  pcri->getValue(dtof1);
  sc = m_HltStoreSvc->get("dphi1", pcri);
  if ( sc.isFailure() ) dphi1=-9;
  else  pcri->getValue(dphi1);
  sc = m_HltStoreSvc->get("dtof2", pcri);
  if ( sc.isFailure() ) dtof2=-999;
  else  pcri->getValue(dtof2);
  sc = m_HltStoreSvc->get("dphi2", pcri);
  if ( sc.isFailure() ) dphi2=-9;
  else  pcri->getValue(dphi2);
  sc = m_HltStoreSvc->get("acol", pcri);
  if ( sc.isFailure() ) acol=-9;
  else  pcri->getValue(acol);
  sc = m_HltStoreSvc->get("pmax1", pcri);
  if ( sc.isFailure() ) pmax1=-9;
  else  pcri->getValue(pmax1);
  sc = m_HltStoreSvc->get("pmax2", pcri);
  if ( sc.isFailure() ) pmax2=-9;
  else  pcri->getValue(pmax2);
  sc = m_HltStoreSvc->get("nsec", pcri);
  if ( sc.isFailure() ) nsec=-9;
  else  pcri->getValue(nsec);
  sc = m_HltStoreSvc->get("mbal", pcri);
  if ( sc.isFailure() ) mbal=-9;
  else  pcri->getValue(mbal);
  m_nmdc=long(nmdc);
  m_ntof=long(ntof);
  m_nemc=long(nemc);
  m_nmuc=long(nmuc);
  m_nshower=long(nshower);
  m_ntrk=long(ntrk);
  m_nsec=long(nsec);
  m_etot=etot;
  m_ebarrel=ebar;
  m_eendcap=eend;
  m_ebalance=ebal;
  m_acop=acop;
  m_emax1=emax1;
  m_emax2=emax2;
  m_dtof1=dtof1;
  m_dphi1=dphi1;
  m_dtof2=dtof2;
  m_dphi2=dphi2;
  m_acol=acol;
  m_mbalance=mbal;
  m_pmax1=pmax1;
  m_pmax2=pmax2;

  std::vector<uint32_t> efVec=efResult->getEFVec();
  m_alg  =efVec[0];
  /*
  m_nmuc =efVec[1];
  m_etot =efVec[4];
  m_emax1=efVec[5];
  m_emax2=efVec[6];
  m_acop =efVec[7];
  m_ebalance=efVec[8];
  m_ebarrel=efVec[9];
  m_eendcap=efVec[10];
  m_ntrk =efVec[11];
  m_pmax1=efVec[12];
  m_pmax2=efVec[13];
  m_acol =efVec[14];
  m_cost1=efVec[15];
  m_cost2=efVec[16];
  m_dtof1 =efVec[19];
  m_dphi1 =efVec[20];
  m_dtof2 =efVec[19];
  m_dphi2 =efVec[20];
  */
  log<< MSG::INFO<<efVec<<endreq;
  log<< MSG::INFO<<acol<<"  "<<etot<<endreq;
  if(m_tuple){
    StatusCode status = m_tuple->write();
    if (!status.isSuccess()) {
      log << MSG::ERROR << "Can't fill ntuple!" << endreq;
    }
  }

  //m_timer[1]->start();

  return StatusCode::SUCCESS;
}

StatusCode CheckEFResult::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  //m_timersvc->print();
  if(m_notpass.size()>0&&m_outfile!="lv2"){
    ofstream outfile;
    outfile.open(m_outfile.c_str(),ios_base::out);
    for(unsigned int i=0;i<m_notpass.size();i++){
      outfile << m_notpass[i] <<endl;
    }
    outfile.close();
  }

  return StatusCode::SUCCESS;
}

