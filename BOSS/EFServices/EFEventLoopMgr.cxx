//===================================================================
//
//	EFEventLoopMgr.cpp
//--------------------------------------------------------------------
//
//	Package    : EFEventLoopMgr
//
//  Description: implementation of the Application's batch mode handler
//
//	Author     : TangRui  tangr@mail.ihep.ac.cn
//
//====================================================================
#define  GAUDISVC_EFEVENTLOOPMGR_CPP
#include <stdlib.h>

#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSelectionAgent.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IAlgManager.h"

#include "HltDataTypes/EventIncident.h"
#include "HltDataTypes/EventInfo.h"
#include "HltDataTypes/EventID.h"
#include "HltDataTypes/EventType.h"
#include "HltDataTypes/EFResult.h"
//#include "HltDataTypes/EFBesTimer.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "EFServices/EFEventLoopMgr.h"

#include "eformat/eformat.h"
//#include "eformat/write/eformat.h"

#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "RawDataCnv/RawDataAddress.h" 

extern const CLID& CLID_Event;
//=========================================================================
// Standard Constructor
//=========================================================================
EFEventLoopMgr::EFEventLoopMgr(const std::string& nam, 
				       ISvcLocator* svcLoc)
: MinimalEventLoopMgr(nam, svcLoc)
{
  declareProperty( "EvtSel", m_evtsel );
  declareProperty( "HistogramPersistency", m_histPersName = "");
  declareProperty( "DisableEFResult", m_disableEFResult = true);
  declareProperty( "OkexecuteEvent", m_okexecuteEvent = true);
  declareProperty( "OutputLevel", m_outputLevel = MSG::ALWAYS);
  declareProperty( "TimeTest", m_timeTest = 0);
  m_histoDataMgrSvc   = 0;
  m_histoPersSvc      = 0;
  m_total_nevt        = 0;
  m_currentRun        = 0;
  m_evtDataMgrSvc     = 0;
  m_evtDataSvc        = 0;
  m_evtSelector       = 0;
  m_evtContext        = 0;
  m_firstRun          = true;
  m_incidentSvc       = 0;
  m_trgType           = 0;
  m_firstEvent        = true;
}

//=========================================================================
// Standard Destructor
//=========================================================================
EFEventLoopMgr::~EFEventLoopMgr()   
{
  if( m_histoDataMgrSvc ) m_histoDataMgrSvc->release();
  if( m_histoPersSvc ) m_histoPersSvc->release();
  if( m_incidentSvc ) m_incidentSvc->release();
  if( m_evtDataMgrSvc ) m_evtDataMgrSvc->release();
  if( m_evtDataSvc ) m_evtDataSvc->release();
  if( m_evtSelector ) m_evtSelector->release();
  if( m_evtContext ) delete m_evtContext;
}

//=========================================================================
// implementation of IAppMgrUI::initalize
//=========================================================================
StatusCode EFEventLoopMgr::initialize()    
{
  MsgStream log(msgSvc(), name());
  // initialize the base class
  cout << "EFEventLoopMgr::initialize()   there are " << m_topAlgList.size() << " topAlg now" << endl;
  StatusCode sc = MinimalEventLoopMgr::initialize();  
  // leave this after initialization of base class, otherwise msgSvc is not correctly set up
  //            -----
  log << MSG::INFO << "EFEventLoopMgr:initalize(): ---> EventLoopMgr = " << name() << " initializing " << endreq;
  if ( !sc.isSuccess() ) {
      log << MSG::ERROR
	  << "EFEventLoopMgr:initalize():Failed to initialize base class MinimalEventLoopMgr"
	  << endreq;
      return sc;
  }
  // Setup access to event data services
  sc = serviceLocator()->service("EventDataSvc", m_evtDataMgrSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataManagerSvc." << endreq;
    return sc;
  }
  sc = serviceLocator()->service("EventDataSvc", m_evtDataSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving EventDataSvc interface IDataProviderSvc." << endreq;
    return sc;
  }
  //-------------------------------------------------------------------------------
  // Get the references to the services that are needed by the ApplicationMgr itself
  //--------------------------------------------------------------------------------
  sc = serviceLocator()->service("IncidentSvc" , m_incidentSvc, true);
 
  if( !sc.isSuccess() ) {
      log << MSG::FATAL << "EFEventLoopMgr:initalize():Error retrieving IncidentSvc"  << endreq;
      return sc;
  }
  
  //-------------------------------------------------------------------------------
  // Get the references to the services that are needed by the ApplicationMgr itself
  //--------------------------------------------------------------------------------
  sc = serviceLocator()->service("HltStoreSvc" , m_HltStoreSvc, true);
  if( !sc.isSuccess() ) {
      log << MSG::FATAL << "EFEventLoopMgr:initalize():Error retrieving HltStoreSvc"  << endreq;
      return sc;
  }

  //--------------------------------------------------------------------------
  // Access Property Manager interface:
  //--------------------------------------------------------------------------
  SmartIF<IProperty> prpMgr(serviceLocator());
  if ( !prpMgr.isValid() ) {
      log << MSG::FATAL 
	  << "EFEventLoopMgr:initalize():IProperty interface not found in ApplicationMgr." 
	  << endreq;
      return StatusCode::FAILURE;
  }
  
  //-------------------------------------------------------------------------
  // Setup Event Setector
  //-------------------------------------------------------------------------
  // We do not expect a Event Selector necessarily being declared
  setProperty(prpMgr->getProperty("EvtSel"));
  
  if( m_evtsel != "NONE" || m_evtsel.length() == 0) {

    sc = serviceLocator()->service( "EventSelector" , m_evtSelector, true );
    if( sc.isSuccess() ) {  // Setup Event Selector
      sc=m_evtSelector->createContext(m_evtContext);
      if( !sc.isSuccess() )   {
        log << MSG::FATAL << "Can not create the event selector Context." << endreq;
        return sc;
      }
    } else {
      log << MSG::FATAL << "EFEventLoopMgr:initalize():EventSelector  not found." << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    m_evtSelector = 0;
    m_evtContext = 0;
    log << MSG::WARNING 
        << "EFEventLoopMgr:initalize():No service \"EventSelector\" provided : assume \"online\" running "
        << endreq;  
  }
  //--------------------------------------------------------------------
  // Get the RawDataInputSvc
  //--------------------------------------------------------------------
  IService* svc ;
  sc = serviceLocator()->getService("RawDataInputSvc", svc); 
  if(sc != StatusCode::SUCCESS ) {
    log<<MSG::WARNING << " Cant get RawDataInputSvc " <<endreq;
    return sc ;
  }
  
  m_inputSvc = dynamic_cast<IRawDataInputSvc*> (svc); 
  if(m_inputSvc == 0 ) {
    log<<MSG::WARNING << " Cant cast to  RawDataInputSvc " <<endreq; 
    return StatusCode::FAILURE ;
  }

  sc = m_HltStoreSvc->get("EventType", m_evtType);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not find EventType" << endreq;
    return StatusCode::FAILURE ;
  }
  sc = m_HltStoreSvc->get("EFResult", m_efResult);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not find EFResult" << endreq;
    return StatusCode::FAILURE ;
  }
  //cout <<"000000000000000000000000000000 "<<m_timeTest<<endl;
  if(m_timeTest){
     //cout <<"111111111111111111111111111111111111"<<endl;
     //IBesTimerSvc* timerSvc;	  
     //sc = service( "BesTimerSvc", timerSvc);
     //if( sc.isFailure() ) {
     //   log << MSG::WARNING << ": Unable to locate BesTimer Service" << endreq;
     //}
     //else{
	//cout <<"222222222222222222222222222222222222222"<<endl;
     //	pBesTimer=timerSvc->addItem("EventFilterHandler");
     //   if(pBesTimer)pBesTimer->propName("nEventFilterHandler");
     //}
  }
  return StatusCode::SUCCESS;
}

//=========================================================================
// implementation of IAppMgrUI::finalize
//=========================================================================
StatusCode EFEventLoopMgr::finalize()    
{
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " EFEventLoopMgr:finalize():Number of events processed : " << m_total_nevt << endreq;

  if ( m_evtSelector && m_evtContext )   {
     m_evtSelector->releaseContext(m_evtContext);
     m_evtContext = 0;
  }

  m_histoPersSvc    = releaseInterface(m_histoPersSvc);

  m_evtSelector   = releaseInterface(m_evtSelector);
  m_incidentSvc   = releaseInterface(m_incidentSvc);
  m_evtDataSvc      = releaseInterface(m_evtDataSvc);
  m_evtDataMgrSvc   = releaseInterface(m_evtDataMgrSvc);
  StatusCode sc = MinimalEventLoopMgr::finalize();
   
  return sc;
}

//=========================================================================
// executeEvent(void* par)  ---> for "online" running 
//=========================================================================
// the parameter par is an EventFormat::RawMemory::FullEventFragment
//and it will be feed into Psc::process method
StatusCode EFEventLoopMgr::executeEvent(void* par)    
{
  
  StatusCode        sc;
  DataObject*       pObject = 0;
  MsgStream log( msgSvc(), name() );
  //return StatusCode::SUCCESS;
  //log << MSG::DEBUG << "EFEventLoopMgr:executeEvent(void* par):Declaring  Fragment" << endreq;
  //if(pBesTimer&&m_timeTest==1){
  //    pBesTimer->start();
  //    pStart=true;
      //cout<<"time test: start!!!!!!!!!!!!!!!!!!!"<<endl;
  //}
  if(m_firstEvent){
    m_re = new RAWEVENT;
    m_firstEvent=false;
  }
  else{
    m_re->reset();
  }

  eformat::FullEventFragment<uint32_t*> pre((uint32_t*)par);
  try {
    pre.check_tree();
  }
  catch (eformat::Issue& ex) {
    std::cerr << std::endl
      << "Uncaught eformat issue: " << ex.what() << std::endl;
    return StatusCode::FAILURE ;
  }
  catch (ers::Issue& ex) {
    std::cerr << std::endl
      << "Uncaught ERS issue: " << ex.what() << std::endl;
    return StatusCode::FAILURE ;
  }
  catch (std::exception& ex) {
    std::cerr << std::endl
      << "Uncaught std exception: " << ex.what() << std::endl;
    return StatusCode::FAILURE ;
  }
  catch (...) {
    std::cerr << std::endl << "Uncaught unknown exception" << std::endl;
    return StatusCode::FAILURE ;
  }
  
  log << MSG::DEBUG << "[Event No. #" << pre.global_id()
            << "] " << pre.fragment_size_word() << " words in "
            << pre.nchildren() << " subdetectors "
            << endreq;

  m_re->setRunNo(pre.run_no());
  m_re->setEventNo(pre.global_id());
  m_re->setTime(pre.time());

  uint32_t *robs[100];
  int nrobs = eformat::get_robs((uint32_t*)par, (const uint32_t **)robs, 100);
  if(nrobs>100) log<<MSG::ERROR<< "ROBFragments' number exceeds than 100!"<<endreq;

  for (int robi = 0; robi < nrobs; robi++) {
    eformat::ROBFragment<uint32_t*> rob(robs[robi]);
    uint32_t status= rob.rod_detev_type();
    uint32_t sourceid = rob.rod_source_id();
    if((status&0x2)&&(sourceid!=0xa50000)) {
      std::string answer="Error";
      m_efResult->reset();
      m_evtType->reset();
      m_evtType->setAnswer(answer);
      log << MSG::DEBUG << "detector status is "<<status<<endreq;
      //delete m_re;
      return StatusCode::SUCCESS;
    }
  }

  bool trg=false;
  for (int robi = 0; robi < nrobs; robi++) {
    eformat::ROBFragment<uint32_t*> rob(robs[robi]);
    uint32_t* dataptr = NULL;
    rob.rod_data(dataptr);
    vector<uint32_t> DetElements(dataptr, (dataptr+rob.rod_ndata()));

    uint32_t source_id_number = rob.rod_source_id();
    //std::cout<<"#####source_id_number#####"<<source_id_number<<std::endl;
    source_id_number <<= 8;
    source_id_number >>= 24;
    //std::cout<<"#####(source_id_number<<24)>>29#####"<<source_id_number<<std::endl;
    //be careful here!!!
    log<<MSG::DEBUG<< source_id_number << " Digi size="<<DetElements.size()<<endreq;
    switch(source_id_number)   
    {
      case 161:
        m_re->addReMdcDigi(dataptr,DetElements.size());
        //if(DetElements.size()>0)std::cout<<hex<<DetElements[0]<<std::endl;
	break;
      case 163:
	m_re->addReEmcDigi(dataptr,DetElements.size());
        break;
      case 162:
	//std::cout<<"EFEventLoopMgr::TOFDigi size="<<DetElements.size()<<std::endl;
        m_re->addReTofDigi(dataptr,DetElements.size());
        break;
      case 164:
	//std::cout<<"EFEventLoopMgr::MUCDigi size="<<DetElements.size()<<std::endl;
        m_re->addReMucDigi(dataptr,DetElements.size());
        break;
      case 165:
	trg=getTrigChn(dataptr,rob.rod_ndata());
	break;
      default:
        //std::cerr << "no such subdetector type!" << std::endl;
        break;
    }
    if(trg){
      std::string answer="RandomTrg";
      m_efResult->reset();
      m_evtType->reset();
      m_evtType->setAnswer(answer);
      log << MSG::DEBUG << "a random trigger event is taken" << endreq;
      m_re->reset();
      return StatusCode::SUCCESS;
    }
  }
  //if(pBesTimer&&m_timeTest==2){
  //    pBesTimer->start();
  //    pStart=true;
      //cout<<"time test: start!!!!!!!!!!!!!!!!!!!"<<endl;
  //}
  if(!(m_inputSvc->setCurrentEvent(m_re))) {
    log << MSG::FATAL << "m_inputSvc->setCurrentEvent(m_re) Failed!" << endreq;
    exit(1);
  }

  if( 0 != m_total_nevt ) {
     sc = m_evtDataMgrSvc->clearStore();
     if( !sc.isSuccess() )  {
	log << MSG::DEBUG << "Clear of Event data store failed" << endreq;
     }
  }
  /*
  if( m_evtContext ) {
    IOpaqueAddress* addr = 0;
    sc = getEventRoot(addr);
    if( !sc.isSuccess() ) {
      log << MSG::INFO << "No more events in event selection " << endreq;
      return StatusCode::FAILURE;
    }
    sc = m_evtDataMgrSvc->setRoot ("/Event", addr);
    if( !sc.isSuccess() ) {
      log << MSG::WARNING << "Error declaring event root address." << endreq;
      return StatusCode::FAILURE;
    }
    SmartDataPtr<Event::EventHeader> evt(m_evtDataSvc,"/Event/EventHeader");
    if(!evt) {
      sc = m_evtDataMgrSvc->registerAddress ("/Event/EventHeader", addr);
      if( !sc.isSuccess() ) {
	log << MSG::WARNING << "Error register EventHeader address." << endreq;
      }
    }
    sc = m_evtDataSvc->retrieveObject("/Event", pObject);
    if( !sc.isSuccess() ) {
      log << MSG::WARNING << "Unable to retrieve Event root object" << endreq;
      return StatusCode::FAILURE;
    }
  }
  else {
    sc = m_evtDataMgrSvc->setRoot ("/Event", new DataObject());
    if( !sc.isSuccess() ) {
      log << MSG::WARNING << "Error declaring event root DataObject" << endreq;
    }
  }
  */

  // Create event address in the event store
  //IOpaqueAddress* addr = new RawDataAddress(CLID_Event, "EventHeader", "EventHeader");
  IOpaqueAddress* addr = 0;
  sc = m_evtSelector->createAddress(*m_evtContext,addr);
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "Error creating IOpaqueAddress." << endreq;
    return sc;
  }
  // Set root clears the event data store first
  sc = m_evtDataMgrSvc->setRoot ("/Event", addr);
  if( !sc.isSuccess() )  {
    log << MSG::WARNING << "Error declaring event root address." << endreq;
    return sc;
  }
  sc = m_evtDataSvc->retrieveObject("/Event", pObject);
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "Unable to retrieve Event root object" << endreq;
    return sc;
  }
  
  m_total_nevt++;
  // Fire BeginEvent "Incident"
  m_incidentSvc->fireIncident(Incident(name(), "BeginEvent"));
  // Execute Algorithms
  sc = MinimalEventLoopMgr::executeEvent(NULL);
  // Fire EndEvent "Incident"
  m_incidentSvc->fireIncident(Incident(name(), "EndEvent"));
	
  //if(pBesTimer){
  //   pBesTimer->stop();
  //   double time=pBesTimer->elapsed();
  //   std::cout<<"Run time="<<time<<std::endl;
  //}
  //delete addr;
  //delete m_re;
  return sc;
}

/// Create event address using event selector
StatusCode EFEventLoopMgr::getEventRoot(IOpaqueAddress*& refpAddr)  {
  refpAddr = 0;
  StatusCode sc = m_evtSelector->next(*m_evtContext);
  if ( !sc.isSuccess() )  {
    return sc;
  }
  // Create root address and assign address to data service
  sc = m_evtSelector->createAddress(*m_evtContext,refpAddr);
  if( !sc.isSuccess() )  {
    sc = m_evtSelector->next(*m_evtContext);
    if ( sc.isSuccess() )  {
      sc = m_evtSelector->createAddress(*m_evtContext,refpAddr);
      if ( !sc.isSuccess() )  {
        MsgStream log( msgSvc(), name() );
        log << MSG::WARNING << "Error creating IOpaqueAddress." << endreq;
      }
    }
  }
  return sc;
}

//----------------------------------------------------------------------------------
// implementation of IInterface: queryInterface
//----------------------------------------------------------------------------------
StatusCode EFEventLoopMgr::queryInterface(const InterfaceID& riid, void** ppvInterface)   {
  if ( IEFEventLoopMgr::interfaceID().versionMatch(riid) )  {
    *ppvInterface = (IEFEventLoopMgr*)this;
  }
  else  {
    return MinimalEventLoopMgr::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}

//=========================================================================
// EF: prepare for run step  ---> for "online" running
//=========================================================================
StatusCode EFEventLoopMgr::prepareForRun(int ef_run_number)
{
  MsgStream log( msgSvc(), name() );
  StatusCode sc;
  int run_num = ef_run_number;
  log << MSG::DEBUG << " ---> EFprepareForRun::Received run number from PT = " 
      << run_num << endreq;
  
  log << MSG::DEBUG 
      << " ---> EFprepareForRun::Setup IOVSvc for run number = " 
      << run_num << endreq;
  const EventInfo* pEvent = new EventInfo(new EventID(run_num,0), new EventType());

  if(m_currentRun!=run_num){
    m_incidentSvc-> fireIncident(Incident(name(),"NewRun"));
    m_currentRun = run_num;
  }
  m_incidentSvc-> fireIncident(Incident(name(),"BeginRun"));
  ListAlg::iterator ita;
  bool failed = false;
  for (ita = m_topAlgList.begin(); ita != m_topAlgList.end(); ita++ ) {
    sc = (*ita)->sysBeginRun();
    if( !sc.isSuccess() ) {
      log << MSG::WARNING << "beginRun() of algorithm " << (*ita)->name() << " failed" << endmsg;
      failed = true; 
    }
  }
  //hxt sc = m_StoreGateSvc->record(pEvent,"");
  if(failed) return StatusCode::FAILURE;
  else       return StatusCode::SUCCESS;
}

//=========================================================================
// EFResult()  ---> for "online" running
//=========================================================================
//3 possible EFDecisions exist: "Accepted", "Rejected", "Error"
std::string EFEventLoopMgr::EF_Decision()
{
  MsgStream log( msgSvc(), name() );
  if (m_disableEFResult) {
    std::string defaultEFDecision = "Other" ;
    log << MSG::WARNING 
	<< "EFEventLoopMgr:EF_Decision():---> EFResult handling is disabled. Default EFDecision = "
	<< defaultEFDecision 
	<< endreq;
    if (m_okexecuteEvent!=1){
      std::string defaultEFDecision = "Error";
      log << MSG::WARNING
	  << "EFEventLoopMgr:EF_Decision():---> Error in executeEvent, setting EFDecision to: "<< defaultEFDecision << endreq;
      return defaultEFDecision;
    }
    return defaultEFDecision;
  } 
  else {
    //log << MSG::DEBUG << "Try to get the EFDecision" << endreq;

    //StatusCode sc = m_HltStoreSvc->get("EFResult", m_efResult);
    //enrich the result returned to online
    //by Fu Chengdong
    if (m_evtType->getAnswerIndex() == -1) {
      log << MSG::DEBUG << "EF_Decision(): Unvalid EFResult(AnswerIndex=-1)" << endreq;
      return "Error";
    }
    std::string answer = m_evtType->getAnswer();
    log << MSG::INFO << "The answer is " << answer << endreq;
    return answer;
  }
}

uint32_t EFEventLoopMgr::EF_Version()
{
  MsgStream log( msgSvc(), name() );
  uint32_t x=1;
  uint32_t y=0;
  uint32_t z=0;
  uint32_t version=(x<<16)+(y<<8)+z;
  log << MSG::INFO << "The version is " <<version << endreq;
  return version;
}
uint32_t EFEventLoopMgr::EF_Algorithm_Tag()
{
  MsgStream log( msgSvc(), name() );
  uint32_t alg=m_efResult->getEFVec(0);
  //log << MSG::INFO << "The algorithm tag is " << alg<<endreq;
  int itype=m_evtType->getMidAnswerIndex();
  //cout <<itype<<endl;
  if(itype != -1) alg += (1<<itype);
  uint32_t alg16=alg>>16;
  log << MSG::DEBUG << "The algorithm tag is " << alg <<":"
      << (alg&1)<<((alg>>1)&1)<<((alg>>2)&1)<<((alg>>3)&1)
      <<((alg>>4)&1)<<((alg>>5)&1)<<((alg>>6)&1)<<((alg>>7)&1)
      << ":%# "<< alg16<<endreq;
  return alg;
}

uint32_t EFEventLoopMgr::EF_Total_Energy()
{
  MsgStream log( msgSvc(), name() );
  uint32_t etot=m_efResult->getEFVec(31);
  log << MSG::INFO << "The total energy is " <<*((float*)&etot) <<"GeV"<<endreq;
  return etot;
}
//This method should return a pointer to the local memory containing the EF SubDetectorFragment 
uint32_t* EFEventLoopMgr::EF_Fragment_Address()
{
  //std::cout << "jak:"<<__FILE__<<std::endl;
  MsgStream log( msgSvc(), name() );

  if (m_disableEFResult) {
    log << MSG::WARNING << "EFEventLoopMgr:EF_Fragment_Address():---> EFResult handling is disabled!!!! No pointer to EF Fragment will be given." << endreq;
    return NULL;

  } 
  else {
    
    //log << MSG::DEBUG << "EFEventLoopMgr:EF_Fragment_Address():---> Going to retrieve EFResult, for the Fragment Address"<<endreq;
    uint32_t alg=m_efResult->getEFVec(0);
    log << MSG::DEBUG << "alg="<<alg<<endreq;
    uint32_t num=m_efResult->getEFVec(1);
    int nmdc=num&0xFF;
    int ntof=(num>>8)&0xFF;
    int nemc=(num>>16)&0xFF;
    int nmuc=(num>>24)&0xFF;
    int ndata=1;
    uint32_t ndatap = nmdc+ntof+nemc+nmuc;
    ndata += ndatap;
    int ndec=m_efResult->getDecNum();
    if(ndec>0) ndata +=(ndec+1);
    if((alg&4)&&(!(alg&2))) log << MSG::WARNING<<"sub-algorithms error!"<<endreq;
    if((ONLINE_DATA_HEAD+ONLINE_DATA_TAIL)!=40){
      log << MSG::FATAL << "data length error!" <<endreq;
      return NULL;
    }
    log << MSG::INFO<< "sent data lenth=" << ndata+40<<endreq;//byjak
    //uint32_t* efAddress=malloc((ndata+ONLINE_DATA_HEAD+ONLINE_DATA_TAIL)*sizeof(uint32_t));
    uint32_t* efAddress=new uint32_t[ndata+ONLINE_DATA_HEAD+ONLINE_DATA_TAIL];
    //std::cout << "fucd-->Address" << efAddress << "; ndata="<<ndata<<std::endl;
    if(efAddress==NULL) return NULL;
    efAddress[0]=0xbb1234bb;
    efAddress[1]=ndata+ONLINE_DATA_HEAD+ONLINE_DATA_TAIL;
    efAddress[2]=8;
    efAddress[3]=0x3000000;
    efAddress[4]=0x7c0000;
    efAddress[5]=1;
    efAddress[6]=0;
    efAddress[7]=0;
    efAddress[8]=0xcc1234cc;
    efAddress[9]=efAddress[1]-efAddress[2];
    efAddress[10]=0xb;
    efAddress[11]=0x3000000;
    efAddress[12]=0x7c0000;
    efAddress[13]=1;
    efAddress[14]=0;
    efAddress[15]=3;
    efAddress[16]=0;
    efAddress[17]=0;
    efAddress[18]=0;
    efAddress[19]=0xdd1234dd;
    efAddress[20]=efAddress[9]-efAddress[10];
    efAddress[21]=8;
    efAddress[22]=0x3000000;
    efAddress[23]=0x7c0000;
    efAddress[24]=1;
    efAddress[25]=0;
    efAddress[26]=0;
    efAddress[27]=0xee1234ee;
    efAddress[28]=9;
    efAddress[29]=0x3000000;
    efAddress[30]=0x7c0000;
    efAddress[31]=0;
    efAddress[32]=0;
    efAddress[33]=0;
    efAddress[34]=0;
    efAddress[35]=0;
    //efAddress[36]=0;
    efAddress[36+ndata]=0;
    efAddress[37+ndata]=1;
    efAddress[38+ndata]=ndata;
    efAddress[39+ndata]=1;
    efAddress[ONLINE_DATA_HEAD]=m_efResult->getEFVec(1);
    if(ndata==1){
      return efAddress;
    }
    else{
      unsigned int i=1;
      for(int j=5;j<5+nmdc;j++){
	efAddress[ONLINE_DATA_HEAD+i]=m_efResult->getEFVec(j);
	++i;
      }
      for(int j=25;j<25+ntof;j++){
        efAddress[ONLINE_DATA_HEAD+i]=m_efResult->getEFVec(j);
        ++i;
      }
      for(int j=30;j<30+nemc;j++){
        efAddress[ONLINE_DATA_HEAD+i]=m_efResult->getEFVec(j);
        ++i;
      }
      for(int j=52;j<52+nmuc;j++){
        efAddress[ONLINE_DATA_HEAD+i]=m_efResult->getEFVec(j);
        ++i;
      }
      if(ndec>0) {
	efAddress[ONLINE_DATA_HEAD+i]=ndec;
	++i;
	for(int j=54;j<54+ndec;++j){
	  efAddress[ONLINE_DATA_HEAD+i]=m_efResult->getEFVec(j);
	  ++i;
	}
      }
      log << MSG::DEBUG <<"0th eff data is "
	  <<((efAddress[ONLINE_DATA_HEAD]>>24)&0xFF)
	  <<((efAddress[ONLINE_DATA_HEAD]>>16)&0xFF)
	  <<((efAddress[ONLINE_DATA_HEAD]>>8)&0xFF)
	  <<((efAddress[ONLINE_DATA_HEAD]>>0)&0xFF)<<endreq;
      for(int i=1;i<ndatap+1;i++){
	log << MSG::DEBUG <<i<<"th eff data is " <<*((float*)&(efAddress[ONLINE_DATA_HEAD+i]))<<endreq;
      }
      for(int i=ndatap+1;i<ndata;i++){
	log << MSG::DEBUG <<i<<"th eff data is " << efAddress[ONLINE_DATA_HEAD+i]<<endreq;
      }
      return efAddress;
    }

    return NULL;
    
    //FullEventAssembler<EFSrcIdMap> fea ; 

      /** EF ROD */ 
      //FullEventAssembler<EFSrcIdMap>::RODDATA*  EFROD ; 
      //EventFormat::SourceID helpEFID(EventFormat::TDAQ_EVENT_FILTER,EventFormat::ROD_TYPE,0);
      //uint32_t rodIdEF = helpEFID.source_id(); 
  
     /* Create the ROD data container to be filled later */
     //EFROD  = fea.getRodData( rodIdEF ); 
  
     //std::vector<uint32_t> efVec(m_efResult->getEFVec());
  
     //for (std::vector<uint32_t>::iterator i=efVec.begin();
      //   i!=efVec.end();++i)
      //{
      // EFROD->push_back(*i);
      // }
  
  
     /** Create an emply RawEvent and fill the RODs into the RawEvent (FullEventFragment) re */ 

     //Get ByteStreamCnvSvc
     
      //     StatusCode sc;
     
     
     //Get ByteStreamCnvSvc
//hxt     if(StatusCode::SUCCESS != service("ByteStreamCnvSvc",m_ByteStreamEventAccess)){
//hxt       log << MSG::ERROR << " EFEventLoopMgr:EF_Fragment_Address():Can't get ByteStreamEventAccess interface " << endreq;
//hxt       return NULL;
//hxt     }

    //Obtain the RawEvent
    /*
    log <<MSG::INFO<< "EFEventLoopMgr:EF_Fragment_Address(): Abans del get RawEvent" << endreq;

    RawEvent* re = m_ByteStreamEventAccess->getRawEvent(); 
     
    log << MSG::INFO <<"EFEventLoopMgr:EF_Fragment_Address(): Despres del get RawEvent" << endreq;
    */
     
    //fea.fill(re,log); 
     
     
     //To serialize RawEvent and obtain a pointer to the memory address
     //where the EF SubdetectorFragment is
     
     
     //Now get SubdetectorFragment from Raw Event
     /*
     const VDETF& vdetf=re->compounds();
     
     log << MSG::DEBUG<< " EFEventLoopMgr:EF_Fragment_Address(): Full Event:" << hex << "Full source ID: " << re->header().source_id() <<endreq;
     log << MSG::DEBUG<< " EFEventLoopMgr:EF_Fragment_Address(): Fragment size in words:" << re->header().fragment_size_word()<<endreq;
     log << MSG::DEBUG<< " EFEventLoopMgr:EF_Fragment_Address(): # of subdetectors:" <<vdetf.size()<<endreq;
     
     
     VDETF::const_iterator it_detf = vdetf.begin(); 
     VDETF::const_iterator it_detf_end = vdetf.end();
     

     //Loop over all Subdetectors
     
     for(; it_detf!=it_detf_end; ++it_detf){  
       
       
       
       if(!((*it_detf)->is_valid())){
	 
	 log<< MSG::ERROR << "EFEventLoopMgr:EF_Fragment_Address():Got invalid subdetector fragment!. A null pointer will be returned" << endreq;
	 return NULL;
       }
       
       // Get the size of the SubDet fragment in WORDS
       int fragSize = (*it_detf)->size_word();
       
       //Now like in ptdummy::Steering
       
       
       // Create a NEW memory storage (it mallocs a new region in "local" memory)
       EventFormat::RawMemoryStorage store(fragSize*4);
       
       // Serialize the subDetector fragment on the store (on the "local" buffer)
       (*it_detf)->serialise(store);
       
       log << MSG::DEBUG<< " EFEventLoopMgr:EF_Fragment_Address():SubDetectorFragment serialized on 'local memory.'" <<endreq;
       
       
       
       // Return to pt::Worker the address (p) of the "local" memory region
       // containing the serialized EF fragment
       
       m_serializedEFAddress=(char*)store.address();
       
       // Release the ownership of the 'local' memory region containing the
       // serialized Fragment (i.e.: exiting from process() the storage distructor
       // will not delete this memory region)
       // The memory region MUST be deleted by pt::worker !!!
       
       store.release_memory();
     }
     return m_serializedEFAddress; 
     */     
  }

  return NULL;
}

bool EFEventLoopMgr::getTrigChn(uint32_t* data, uint32_t ndata){
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << ndata<<":"<<(*data)<<","<<endreq;
  uint32_t index = 0;

  while (ndata - index > 1) {
    uint32_t header= *(data+index);
    uint32_t blockSize = ( (header>>14) & 0x3FF);
    uint32_t id = (header>>24);
    //std::cout << "TriggerChannel:" << hex<<header << "==>" << blockSize<<","<<id
    //	      << ","<<((header >> 8)  & 0x3F) << dec<<std::endl;
    if (blockSize == 0 || (index+blockSize) > ndata) break;
    if (id==0xD3) {
      uint32_t window = ((header >> 8)  & 0x3F); //Time window, bit8 to bit13, total: 0--31
      uint32_t size= blockSize-1;
      if(window==0){
	log << MSG::FATAL << "window=0" << endreq;
	//return false;
      }
      else if(size%window != 0) {
	log << MSG::FATAL << "GTL data is NOT completed" << endreq;
	//return false;
      }
      //vector<uint32_t> trigData(data+index+1, data+index+size);
      for(uint32_t j = 0; j < size; j++) {
	uint32_t trgdata = *(data+index+1+j);
	uint32_t dataId = ((trgdata >> 24) & 0x7);
	//std::cout <<hex<< trgdata << "==>"<< dataId <<","  << (trgdata&0xFFFFFF)<<dec<<std::endl;
	if(dataId!=5) continue;
	//std::cout <<hex<< trgdata <<dec<<std::endl;
	m_trgType= m_trgType|(trgdata&0xFFFF);
	if(trgdata&(1<<9)) return true;
	//if(trgdata&(1<<8)) return true;
	//if(trgdata&(1<<10)) return true;
      }
    }
    index += blockSize;
  }
  //cout<<m_trgType<<endl;

  return false;
}
