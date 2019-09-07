#include "EventWriter/EventWriter.h"
// for DistBoss
#include "DistBossUtil/DistBossCode.h"
#include "DistBossUtil/GetPropertyValue.h"
#include "RootEventData/TBossFullEvent.h" 

#include "DataInfoSvc/DataInfoSvc.h" 
#include "DataInfoSvc/IDataInfoSvc.h" 
#include "TBufferFile.h"
 
#include "GaudiKernel/SmartIF.h" 
#include "GaudiKernel/IProperty.h" 
#include "GaudiKernel/Bootstrap.h" 
#include "GaudiKernel/IAppMgrUI.h" 
#include <algorithm> // for find 
#include <iostream> 
#include <fstream> 

/////////////////////////////////////////////////////////////////////////////
//int EventWriter::counter = 0;	// static data member

EventWriter::EventWriter(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  m_itemNames.push_back("/Event");
  m_itemNames.push_back("/Event/EventHeader");
  m_itemNames.push_back("/Event/Navigator");

  m_itemNames.push_back("/Event/Digi");
  m_itemNames.push_back("/Event/Digi/MdcDigiCol");
  m_itemNames.push_back("/Event/Digi/TofDigiCol");
  m_itemNames.push_back("/Event/Digi/EmcDigiCol");
  m_itemNames.push_back("/Event/Digi/MucDigiCol");

  m_itemNames.push_back("/Event/Hlt");
  m_itemNames.push_back("/Event/Hlt/HltRawCol");
  m_itemNames.push_back("/Event/Hlt/HltInf");
  //m_itemNames.push_back("/Event/Hlt/DstHltInf");

  m_itemNames.push_back("/Event/Recon");
  //  m_itemNames.push_back("/Event/Recon/RecMdcHitCol");  
  m_itemNames.push_back("/Event/Recon/RecMdcTrackCol"); 
  //   m_itemNames.push_back("/Event/Recon/MdcHOTCol"); 
  m_itemNames.push_back("/Event/Recon/RecMdcDedxCol");
  //    m_itemNames.push_back("/Event/Recon/RecMdcDedxHitCol");
  m_itemNames.push_back("/Event/Recon/RecMdcKalTrackCol");
  //   m_itemNames.push_back("/Event/Recon/RecMdcKalHelixSegCol");
  m_itemNames.push_back("/Event/Recon/RecEsTimeCol");
  m_itemNames.push_back("/Event/Recon/RecExtTrackCol");
  //    m_itemNames.push_back("/Event/Recon/RecBTofHitCol");     
  //    m_itemNames.push_back("/Event/Recon/RecETofHitCol");    
  m_itemNames.push_back("/Event/Recon/RecTofTrackCol");     
  //    m_itemNames.push_back("/Event/Recon/RecBTofCalHitCol");
  //    m_itemNames.push_back("/Event/Recon/RecETofCalHitCol");   
  m_itemNames.push_back("/Event/Recon/RecEmcHitCol");    
  m_itemNames.push_back("/Event/Recon/RecEmcClusterCol");
  m_itemNames.push_back("/Event/Recon/RecEmcShowerCol");
  //    m_itemNames.push_back("/Event/Recon/MucRecHitCol");
  m_itemNames.push_back("/Event/Recon/RecMucTrackCol"); 
  m_itemNames.push_back("/Event/Recon/EvtRecEvent");
  m_itemNames.push_back("/Event/Recon/EvtRecTrackCol");
  //    m_itemNames.push_back("/Event/Recon/EvtRecVertexCol");    

  // Part 1: Declare the properties
  declareProperty("digiRootOutputFile",m_dofileName = "event.rec");
  declareProperty("ItemList",m_itemNames);
  declareProperty("RunMode", m_mode = 2);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
EventWriter::~EventWriter(){
  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " EventWriter ~EventWriter()" << endreq;



}

//**************************************************************************
StatusCode EventWriter::initialize(){

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << " EventWriter initialize()" << endreq;


  //   StatusCode status = Converter::initialize();

  //   if ( status.isSuccess() )   {

  IService* isvc = 0;

  StatusCode status = serviceLocator()->service("RootCnvSvc", isvc, false);

  if ( !status.isSuccess() )   status = serviceLocator()->service("EventCnvSvc", isvc, true);

  if ( status.isSuccess() )   {

    status = isvc->queryInterface(IID_IRootCnvSvc, (void**)&m_cnvSvc);

  }


  status = serviceLocator()->service("EventCnvSvc", m_pConversionSvc, true);
  if ( !status.isSuccess() ) {
    log << MSG::FATAL << "Unable to locate IConversionSvc interface"
      << endreq;
    return status;
  }


  status = serviceLocator()->service("EventDataSvc", m_pDataProvider, true);
  if ( !status.isSuccess() ) {
    log << MSG::FATAL << "Unable to locate IDataProviderSvc interface"
      << endreq;
    return status;
  }

  //  }

  if ( m_mode == 2 ) {  //OfflineMode
     m_single_outputFiles = new TFile(m_dofileName.c_str(), "RECREATE");
     if(m_single_outputFiles->IsZombie()||(!m_single_outputFiles->IsWritable())){
        std::cout<<"EventWriter ERROR::Can't not open file"<<m_dofileName.c_str()<<std::endl;
        exit(1);
     }

     m_single_outputTrees = new TTree("Event","Event");
     m_jobInfoTree = new TTree("JobInfoTree","Job info"); 
     jobInfo = new TJobInfo;
  }
  else if ( m_mode == 3 ) {  //DistBossMode
     status = DistBoss::GetPropertyValue<std::string>("DistBoss", "ServerName", m_svrName);
     m_TFullEvt = new TBossFullEvent;
     m_writer   = new NetDataWriter(m_svrName+'/'+name());
     m_bufsize  = 512*1024;
     m_cbuf     = new char[m_bufsize];
     int code = DistBossCode::ClientReady;
     m_writer->writeEvent((void*)&code, 4);
  }
  else {
     log << MSG::FATAL << "Unvalid RunMode @ initialize(): " << m_mode << endreq;
     exit(1);
  }

  status = getSvc();
  if ( !status.isSuccess() ) {
    log << MSG::FATAL << "can not getSvc"  << endreq;
    return status;
  }
  st=0;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode EventWriter::getSvc(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "getSvc()" << endreq;
  StatusCode status = StatusCode::SUCCESS;
  if(m_itemNames.empty()) {
    log << MSG::ERROR << "no ItemList from the jobOption" << endreq;
    return StatusCode::FAILURE;
  } 
  //   ItemNames	m_itemNames;
  clearItems(m_itemList);
  ItemNames::iterator i;
  for (i = m_itemNames.begin(); i != m_itemNames.end(); i++){
    addItem(m_itemList, *i);
  }
  return status;
}


StatusCode EventWriter::execute() {

  // Part 1: Get the messaging service, print where you are
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "EventWriter execute()" << endreq;
  collectObjects();

  if ( m_mode == 2 ) {  //OfflineMode
     if(st!=1){
	if(m_common.m_rectrackEvt) m_single_outputTrees->Branch("TRecEvent","TRecTrackEvent",&m_common.m_rectrackEvt,3200000,1);
	if(m_common.m_evtRecObject) m_single_outputTrees->Branch("TEvtRecObject","TEvtRecObject",&m_common.m_evtRecObject,3200000,1);
    	if(m_common.m_dstEvt)  m_single_outputTrees->Branch("TDstEvent","TDstEvent",&m_common.m_dstEvt,3200000,1);
	if(m_common.m_recEvt) m_single_outputTrees->Branch("TDigiEvent","TDigiEvent",&m_common.m_recEvt,3200000,1);
	if(m_common.m_EvtHeader) m_single_outputTrees->Branch("TEvtHeader","TEvtHeader",&m_common.m_EvtHeader,3200000,1);
        if(m_common.m_EvtNavigator) m_single_outputTrees->Branch("TEvtNavigator","TEvtNavigator",&m_common.m_EvtNavigator,3200000,1);
	if(m_common.m_hltEvt) m_single_outputTrees->Branch("THltEvent","THltEvent",&m_common.m_hltEvt,3200000,1);
    	if(m_common.m_mcEvt) m_single_outputTrees->Branch("TMcEvent","TMcEvent",&m_common.m_mcEvt,3200000,1);
    	if(m_common.m_trigEvt) m_single_outputTrees->Branch("TTrigEvent","TTrigEvent",&m_common.m_trigEvt,3200000,1);
	m_jobInfoTree->Branch("JobInfo",&jobInfo);
	st=1;
     }

     if(m_single_outputFiles->IsZombie()||(!m_single_outputFiles->IsOpen())){
        std::cout<<"EventWriter ERROR::The ROOT File:"<<m_dofileName.c_str()<<"status is false"<<std::endl;
        exit(1);
     }
     int nb = m_single_outputTrees->Fill();
     if(nb==-1)
     {
	log << MSG::FATAL << "Error in fill tree (EventWriter) "<<m_single_outputTrees->GetName() << " with "<<nb<<" bytes" <<endreq;
	exit(1);
     }

     m_single_outputFiles = m_single_outputTrees->GetCurrentFile();
  }
  else if ( m_mode == 3 ) {  //DistBossMode
     m_TFullEvt->setEvtHeader(m_common.m_EvtHeader);
     m_TFullEvt->setDigiEvent(m_common.m_recEvt);  //TDigiEvent
     m_TFullEvt->setDstEvent(m_common.m_dstEvt);
     m_TFullEvt->setMcEvent(m_common.m_mcEvt);
     m_TFullEvt->setTrigEvent(m_common.m_trigEvt);
     m_TFullEvt->setHltEvent(m_common.m_hltEvt);
     m_TFullEvt->setRecTrackEvent(m_common.m_rectrackEvt);
     m_TFullEvt->setEvtRecObject(m_common.m_evtRecObject);

     // Might it be able to optimize within higher version of ROOT?
     TBufferFile m_TBuffer(TBufferFile::kWrite, m_bufsize-12, m_cbuf+4, false);
     m_TBuffer.WriteObject(m_TFullEvt);
     if ( m_TBuffer.Buffer() != (m_cbuf+4) ) {
	m_bufsize = m_TBuffer.Length() + 12;
	m_cbuf = new char[m_bufsize];
	memcpy(m_cbuf+4, m_TBuffer.Buffer(), m_TBuffer.Length());
	m_TBuffer.SetBit(TBuffer::kIsOwner);
     }
     *((int*)m_cbuf) = m_common.m_EvtHeader->getEventId();
     m_writer->writeEvent( (void*)m_cbuf, m_TBuffer.Length()+4 );

     m_TFullEvt->reset();
  }

  m_common.clear();

  return StatusCode::SUCCESS;
}

void EventWriter::addItem(Items& itms, const std::string& descriptor)   {
  MsgStream log(msgSvc(), name());
  int level = 0;
  size_t sep = descriptor.rfind("#");
  std::string obj_path (descriptor,0,sep);
  std::string slevel   (descriptor,sep+1,descriptor.length());
  if ( slevel == "*" )  {
    level = 9999999;
  }
  else   {
    level = atoi(slevel.c_str());
  }
  size_t idx = obj_path.find("/",1);
  while (idx != std::string::npos)  {
    std::string sub_item = obj_path.substr(0,idx);
    if ( 0 == findItem(sub_item) )   {
      addItem(itms, sub_item+"#1");
    }
    idx = obj_path.find("/",idx+1);
  }
  DataStoreItem* item = new DataStoreItem(obj_path, level);
  log << MSG::DEBUG << "Adding OutputStream item " << item->path()
    << " with " << item->depth()
    << " level(s)." << endreq;
  itms.push_back( item );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void EventWriter::clearItems(Items& itms)     {
  for ( Items::iterator i = itms.begin(); i != itms.end(); i++ )    {
    delete (*i);
  }
  itms.erase(itms.begin(), itms.end());
}

DataStoreItem*
EventWriter::findItem(const std::string& path)  {
  for (Items::const_iterator i=m_itemList.begin(); i != m_itemList.end(); ++i)  {
    if ( (*i)->path() == path )  return (*i);
  }
  return 0;
}



StatusCode EventWriter::collectObjects(){
  MsgStream log(msgSvc(), name());
  StatusCode status = StatusCode::SUCCESS;
  Items::iterator i;

  // Traverse the tree and collect the requested objects
  //    status = m_pConversionSvc->connectOutput(f_rootOutputFile.c_str(), "recreate");

  for ( i = m_itemList.begin(); i != m_itemList.end(); i++ )    {
    DataObject* obj = 0;
    IOpaqueAddress *pAddress = 0;
    m_currentItem = (*i);

    status = m_pDataProvider->retrieveObject(m_currentItem->path(), obj);
    if ( status.isSuccess() )  {
      status = m_pConversionSvc->createRep(obj, pAddress);
      //IRegistry *pReg = obj->registry();
      //pReg->setAddress(pAddress);
      //status = m_pConversionSvc->fillRepRefs(pReg->address(), *i);
    }
    else  {
      log << MSG::ERROR << "Cannot write mandatory object(s) (Not found): "
        << m_currentItem->path() << endreq;
    }
  }


  return status;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

StatusCode EventWriter::finalize() {

   // Part 1: Get the messaging service, print where you are
   MsgStream log(msgSvc(), name());
   int st =1;
   std::cout<<"11111111111111111111111111"<<std::endl;

   if ( m_mode == 2 ) {  //OfflineMode
      m_bossVer = getenv("BES_RELEASE"); 
      log << MSG::INFO << "fill boss version: "<<m_bossVer << endreq; 

      string tmpJobOptions = getJobOptions(); 
      m_jobOptions.push_back( tmpJobOptions ); 

      if(m_decayOptions.size()==0) 
	 m_decayOptions = getDecayOptions(); 

      jobInfo->setBossVer(m_bossVer); 
      jobInfo->setJobOptions(m_jobOptions); 
      jobInfo->setDecayOptions(m_decayOptions); 

      m_jobInfoTree->Fill(); 

      st = m_single_outputFiles->Write();
      if(st==0) 
      {
         log << MSG::FATAL <<"can not write to the file "<<m_dofileName.c_str()<<endreq;
         exit(1);
      }
      m_single_outputFiles->Close();

      delete m_single_outputFiles;
      delete  jobInfo;
   }

   if ( m_mode == 3 ) {  //DistBossMode
      int code = DistBossCode::StatusFinalize;
      m_writer->writeEvent((void*)&code, 4);
      delete m_writer;
      delete m_TFullEvt;
      delete [] m_cbuf;
   }

   log << MSG::INFO << "EventWriter finalize()" << endreq;
   return StatusCode::SUCCESS;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

std::string EventWriter::getJobOptions() 
{ 
   MsgStream log(msgSvc(), name());  
   IInterface* iface = Gaudi::createApplicationMgr(); 
   //SmartIF<IProperty> propMgr ( IID_IProperty, iface ); 
   SmartIF<IProperty> propMgr ( iface );
   std::string path; 
   propMgr->getProperty( "JobOptionsPath", path); 
   log << MSG::INFO << "JobOptions file for current job: " <<path << endreq; 
   ifstream fin(path.c_str()); 
   string jobOptions; 
   string tempString; 
   while(getline(fin,tempString)) 
   { 
      if( tempString.size()>0 && tempString.find("//")>tempString.size() ) 
      { 
	 jobOptions += tempString; 
	 jobOptions += "\n"; 
      } 
   } 
   log << MSG::INFO << "JobOptions: " << endreq 
      << jobOptions << endreq; 
   return jobOptions; 
} 

std::string EventWriter::getDecayOptions() 
{ 
   MsgStream log(msgSvc(), name());
   ISvcLocator* svcLocator = Gaudi::svcLocator(); 
   IDataInfoSvc *tmpInfoSvc; 
   DataInfoSvc* jobInfoSvc; 
   string decayOptions; 
   StatusCode status = svcLocator->service("DataInfoSvc",tmpInfoSvc); 
   if (status.isSuccess()) { 
      log << MSG::INFO << "get the DataInfoSvc" << endreq; 
      jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc); 
      decayOptions = jobInfoSvc->getDecayOptions(); 
      log << MSG::INFO << "get decay options" << endreq 
	 << decayOptions << endreq; 
   }else { 
      log << MSG::WARNING << "could not get the DataInfoSvc. Ignore it." << endreq; 
   } 
   return decayOptions; 
} 


