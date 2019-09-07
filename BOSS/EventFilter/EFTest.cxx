#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"                                                                            
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/HltID.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/DstHltInf.h"

#include "EventFilter/EventFilter.h"

#include "EventFilter/EFTest.h"

EFTest::EFTest(const std::string& name, ISvcLocator* pSvcLocator) :
	       Algorithm(name, pSvcLocator) {
  // Declare the properties  
  // declareProperty("Cut", m_acolCut = 10);
  declareProperty("OutputLevel", m_outputLevel = MSG::NIL);
}

EFTest::~EFTest(){
}

StatusCode EFTest::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  // user code

  StatusCode status=StatusCode::SUCCESS;
  NTuplePtr nt1(ntupleSvc(), "FILE1/EventFilter");
  if ( nt1 ) m_tuple = nt1;
  else {
    m_tuple = ntupleSvc()->book ("FILE1/EventFilter", CLID_ColumnWiseTuple, "EventFilter N-Tuple");
    if ( m_tuple ) {
      status = m_tuple->addItem ("type",   m_type);
      status = m_tuple->addItem ("alg",   m_alg);
      status = m_tuple->addItem ("endEE",   m_endEE);
      status = m_tuple->addItem ("version",   m_version);
      status = m_tuple->addItem ("etot",   m_etot);
      status = m_tuple->addItem ("nMDCdata",   m_nmdc0);
      status = m_tuple->addItem ("nTOFdata",   m_ntof0);
      status = m_tuple->addItem ("nEMCdata",   m_nemc0);
      status = m_tuple->addItem ("nMUCdata",   m_nmuc0);
      status = m_tuple->addItem ("nCONdata",   m_ncon0);
      status = m_tuple->addItem ("nMDCfill",   m_nmdc, 0, (int)HltID::getID_MDC_MAX() );
      status = m_tuple->addItem ("nTOFfill",   m_ntof, 0, (int)HltID::getID_TOF_MAX() );
      status = m_tuple->addItem ("nEMCfill",   m_nemc, 0, (int)HltID::getID_EMC_MAX() );
      status = m_tuple->addItem ("nMUCfill",   m_nmuc, 0, (int)HltID::getID_MUC_MAX() );
      status = m_tuple->addItem ("nCONfill",   m_ncon, 0, 100 );
      status = m_tuple->addIndexedItem ("MDCdata", m_nmdc, m_mdcData);
      status = m_tuple->addIndexedItem ("TOFdata", m_ntof, m_tofData);
      status = m_tuple->addIndexedItem ("EMCdata", m_nemc, m_emcData);
      status = m_tuple->addIndexedItem ("MUCdata", m_nmuc, m_mucData);
      status = m_tuple->addIndexedItem ("CONdata", m_ncon, m_conData);
      if (!status) {
	log << MSG::ERROR << "    Cannot add item into N-tuple" << endreq; 
	return status;
      }
    }
    else { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple) << endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode EFTest::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  if(!EventFilter::getResult()) {
    // this event refused by event filter
    log << MSG::DEBUG << "this event refused by event filter: "
	<< EventFilter::getName() <<endreq;
    // user code

  }
  else {
    // this event accepted by event filter  
    log << MSG::DEBUG << "this event accepteded by event filter: "
	<< EventFilter::getName() <<endreq;
    // user code

  }
  SmartDataPtr<HltInf> hlt(eventSvc(),"/Event/Hlt/HltInf");
  bool dstFlag=false;
  if(!hlt) {
    log << MSG::WARNING << "Could not find Event HltInf (try DstHltInf now)" << endreq;
    SmartDataPtr<DstHltInf> hltDst(eventSvc(),"/Event/Hlt/DstHltInf");
    if(!hltDst){
      log << MSG::FATAL << "Could not find Event DstHltInf too ( please re-generate data)" << endreq;
      return( StatusCode::FAILURE);
    }
    else{
      hlt=hltDst;
      dstFlag=true;
    }
  }

  uint32_t type = hlt->getEventType();
  for(int i=0;i<32;i++) {
    if((1<<i)&type) m_type=i;
  }
  m_alg     = hlt->getAlgProcess();
  m_endEE   = hlt->getCriteriaTable();
  m_version = hlt->getVersion();
  m_etot    = hlt->getTotalEnergy();
  m_nmdc0    = hlt->getNMDC();
  m_ntof0    = hlt->getNTOF();
  m_nemc0    = hlt->getNEMC();
  m_nmuc0    = hlt->getNMUC();
  m_ncon0    = hlt->getNCON();
  m_nmdc    = 0;
  m_ntof    = 0;
  m_nemc    = 0;
  m_nmuc    = 0;
  m_ncon    = 0;
  if(!dstFlag) {
    vector<uint32_t>::iterator it;

    vector<uint32_t> mdcData= hlt->getDataVec(HltID::MDC);
    it = mdcData.begin();
    for(;it!=mdcData.end();it++,m_nmdc++){
      if(m_nmdc>1)   m_mdcData[m_nmdc]= *((float*) &(*it));
      else           m_mdcData[m_nmdc]= *it;  //*((int*) &(*it));
    }

    vector<uint32_t> tofData= hlt->getDataVec(HltID::TOF);
    it = tofData.begin();
    for(;it!=tofData.end();it++,m_ntof++){
      if(m_ntof>0)   m_tofData[m_ntof]= *((float*) &(*it));
      else           m_tofData[m_ntof]= *it;  //*((int*) &(*it));
    }

    vector<uint32_t> emcData= hlt->getDataVec(HltID::EMC);
    it = emcData.begin();
    for(;it!=emcData.end();it++,m_nemc++){
      if(!(m_nemc==0||m_nemc==7||m_nemc==8))   m_emcData[m_nemc]= *((float*) &(*it));
      else                                  m_emcData[m_nemc]= *it;  //*((int*) &(*it));
    }

    vector<uint32_t> mucData= hlt->getDataVec(HltID::MUC);
    it = mucData.begin();
    for(;it!=mucData.end();it++,m_nmuc++){
      if(m_nmuc>1)   m_mucData[m_nmuc]= *((float*) &(*it));
      else           m_mucData[m_nmuc]= *it;  //*((int*) &(*it));
    }

    vector<uint32_t> conData= hlt->getDataVec(HltID::CON);
    it = conData.begin();
    for(;it!=conData.end();it++,m_ncon++){
      m_conData[m_ncon]= *it;  //*((int*) &(*it));
    }
  }

  m_tuple->write();

  return StatusCode::SUCCESS ;
}

StatusCode EFTest::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  // event number
  long unsigned ntot=EventFilter::getNEvt();
  long unsigned npass=EventFilter::getNPass();
  log << MSG::ALWAYS << "=========================================" <<endreq
      << "total "<< ntot <<" events with " << npass << " effective events " <<endreq
      <<              "========================================="<< endreq;
  // user code

  return StatusCode::SUCCESS;
}
