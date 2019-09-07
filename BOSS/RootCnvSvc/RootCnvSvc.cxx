#define RootCnvSvc_CXX
#include <iostream>
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/SvcFactory.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/GenericAddress.h"

#include "RawEvent/DigiEvent.h"
#include "DstEvent/DstEvent.h"
#include "McTruth/McEvent.h"
#include "TrigEvent/TrigEvent.h"//caogf
#include "HltEvent/HltEvent.h"//fucd
#include "RootCnvSvc/RootInterface.h"

#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/RootCnvSvc.h"
#include "RootCnvSvc/RootEvtSelector.h"


#include "RootCnvSvc/Dst/MdcDedxCnv.h"
#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Digi/EmcDigiCnv.h"
#include "RootCnvSvc/Mc/EmcMcHitCnv.h"
#include "RootCnvSvc/Dst/EmcTrackCnv.h"
#include "RootCnvSvc/EventCnv.h"
#include "RootCnvSvc/Dst/ExtTrackCnv.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/McParticleCnv.h"
#include "RootCnvSvc/Digi/MdcDigiCnv.h"
#include "RootCnvSvc/Dst/MdcKalTrackCnv.h"
#include "RootCnvSvc/Mc/MdcMcHitCnv.h"
#include "RootCnvSvc/Dst/MdcTrackCnv.h"
#include "RootCnvSvc/Digi/MucDigiCnv.h"
#include "RootCnvSvc/Mc/MucMcHitCnv.h"
#include "RootCnvSvc/Dst/MucTrackCnv.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/TofDigiCnv.h"
#include "RootCnvSvc/Mc/TofMcHitCnv.h"
#include "RootCnvSvc/Dst/TofTrackCnv.h"
#include "RootCnvSvc/Digi/LumiDigiCnv.h"

#include "RootCnvSvc/Rec/RecMucTrackCnv.h"   
#include "RootCnvSvc/Rec/RecMdcTrackCnv.h"   
#include "RootCnvSvc/Rec/RecMdcHitCnv.h"  
#include "RootCnvSvc/Rec/RecMdcKalTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcKalHelixSegCnv.h"
#include "RootCnvSvc/Rec/RecMdcDedxCnv.h"    
#include "RootCnvSvc/Rec/RecMdcDedxHitCnv.h"
#include "RootCnvSvc/Rec/RecTofTrackCnv.h"  
#include "RootCnvSvc/Rec/RecBTofCalHitCnv.h" 
#include "RootCnvSvc/Rec/RecETofCalHitCnv.h" 
#include "RootCnvSvc/Rec/RecMucRecHitCnv.h"  
#include "RootCnvSvc/Rec/RecEmcHitCnv.h"     
#include "RootCnvSvc/Rec/RecEmcClusterCnv.h" 
#include "RootCnvSvc/Rec/RecEmcShowerCnv.h"   
#include "RootCnvSvc/Rec/RecEvTimeCnv.h"      
#include "RootCnvSvc/Rec/RecExtTrackCnv.h"   
#include "RootCnvSvc/Rec/RecTrackCnv.h"   

#include "RootCnvSvc/EvtRec/EvtRecCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEventCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecTrackCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPrimaryVertexCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecVeeVertexCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecPi0Cnv.h"
#include "RootCnvSvc/EvtRec/EvtRecEtaToGGCnv.h"
#include "RootCnvSvc/EvtRec/EvtRecDTagCnv.h"
#include "RootCnvSvc/Rec/RecZddChannelCnv.h"
   

#include "RootCnvSvc/Trig/TrigCnv.h"
#include "RootCnvSvc/Trig/TrigDataCnv.h"

#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/Hlt/HltRawCnv.h"
#include "RootCnvSvc/Hlt/HltInfCnv.h"
#include "RootCnvSvc/Hlt/DstHltInfCnv.h"

#include "RootCnvSvc/EvtHeaderCnv.h"
#include "RootCnvSvc/EvtNavigatorCnv.h"

#include <math.h>

using namespace std;

// RCS Id for identification of object version
static const char* rcsid = "$Id: RootCnvSvc.cxx,v 1.68 2015/06/17 03:12:16 dengzy Exp $";


// Instantiation of a static factory class used by clients to create
// instances of this service
//static const SvcFactory<RootCnvSvc> s_RootCnvSvcFactory;
//const ISvcFactory& RootCnvSvcFactory = s_RootCnvSvcFactory;

RootCnvSvc::RootCnvSvc(const std::string& name, ISvcLocator* svc)
  : ConversionSvc(name, svc, ROOT_StorageType), m_minSplit(1), m_evtsel(0)              
{
  StatusCode sc;
  MsgStream log(msgSvc(), "RootCnvSvc");
  log << MSG::DEBUG << "RootCnvSvc::constructor" << endreq;
  
  IService* isvc = 0;
  sc = serviceLocator()->getService ("RootEvtSelector", isvc, false);
  if (!sc.isSuccess())  sc = serviceLocator()->getService ("EventSelector", isvc, false);
  if (sc.isSuccess()) {
    sc = isvc->queryInterface(IID_IRootEvtSelector, (void**)&m_evtsel);
  }
  if(sc.isFailure()) {
    log << MSG::WARNING << "Unable to start event selector service within RootCnvSvc" << endreq;
  }
  
  m_rootInterface= RootInterface::Instance(log);
  if (!m_rootInterface)      log << MSG::ERROR << "Unable to start Root service within RootCnvSvc" << endreq;
 

  
    //Digi
  declareProperty("digiTreeName", m_dtreeName="Event"); //"Rec"->"Event" 
  
  declareProperty("digiRootInputFile",m_difileName);
  //if (m_difileName.size()==0){
  //  m_difileName.push_back("digi.root");
  //}
  declareProperty("digiRootOutputFile",m_dofileName = "");

  // Split mode for writing the TTree [0,99] 
  declareProperty("digiSplitMode", m_dsplitMode=m_minSplit);
     // Buffer size for writing ROOT data
  declareProperty("digiBufferSize", m_dbufSize=3200000);
    // ROOT default compression
  declareProperty("digiCompressionLevel", m_dcompressionLevel=1);
  declareProperty("selectFromTag", m_selectFromTag=0);
  declareProperty("tagInputFile", m_tagInputFile);
  //declareProperty("tagFileFormat", m_tagFileFormat=1);
  
}

StatusCode RootCnvSvc::initialize()     {
  // Purpose and Method:  Setup GLAST's Event Converter Service.
  //   Associate RootCnvSvc with the EventDataSvc
  //   Associate the list of known converters with this RootCnvSvc
  //   configure map of leaves

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "RootCnvSvc::initialize" << endreq;
  StatusCode status = ConversionSvc::initialize();
  if ( status.isSuccess() )   {
    //        ISvcLocator* svclocator = serviceLocator();  [unused for now]
    IDataProviderSvc *pIDP = 0;
    // Set event data service
    status = service("EventDataSvc", pIDP, true);
    if ( status.isSuccess() )   {
      status = setDataProvider ( pIDP );
    }
    else    {
      return status;
    }

    
    // Add converters to the service 
    status = addConverters();
    if ( !status.isSuccess() )   {
      log << MSG::ERROR << "Unable to add converters to the service" << endreq;
      return status;
    }

      
    // Now we have to configure the map of leaves
    // Which should contain the association of converters with 
    // paths on the TDS
    for (LeafMap::iterator k = m_leaves.begin(); k != m_leaves.end(); k++ )   {
      std::string path = (*k).first;
      for (LeafMap::iterator j = m_leaves.begin(); j != m_leaves.end(); j++ )   {
        std::string path2 = (*j).first;
        std::string pp = (*j).first.substr(0, (*j).first.rfind("/"));
        if ( path == pp && path != (*j).first )   {
          (*k).second->push_back((*j).second);
        }
      }
    }   
  }

  // get properties and tell RootInterface about files
  status=initFiles();

  m_rootInterface->setSelectFromTag(m_selectFromTag);
  m_rootInterface->setTagInputFile(m_tagInputFile);
  //m_rootInterface->setTagFileFormat(m_tagFileFormat);
   
  return status;
}

StatusCode RootCnvSvc::finalize()     {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "RootCnvSvc::finalize" << endreq;

  ConversionSvc::finalize();
  // log << MSG::INFO<<"this is the end of RootCnvsvc finalize"<<endreq;
  for (LeafMap::iterator k = m_leaves.begin(); k != m_leaves.end(); k++ )   {
    delete (*k).second;
  }
  m_leaves.erase(m_leaves.begin(), m_leaves.end());
 
  return m_rootInterface->finalize();
}

StatusCode RootCnvSvc::initFiles () {
    MsgStream log(msgSvc(), name());
    log << MSG::DEBUG << "RootCnvSvc::initFiles" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

  // Use the Job options service to set the Algorithm's parameters
  // This will retrieve parameters set in the job options file
    setProperties();
    
    //Read inpout files files from job options
    int nSize = m_difileName.size();
    log<<MSG::INFO<<"Root Input files "<<nSize<<endreq;
    if(nSize == 0){
      log<<MSG::INFO<<"Unable to find input file"<<endreq;
      //return StatusCode::FAILURE;
    }
    
    for(int i = 0;i<nSize;i++){
      facilities::Util::expandEnvVar(&m_difileName[i]);
      std::string s=m_difileName[i];
      std::vector<int> wildcardPos;
      for(int pos=0;pos<s.size();pos++) {
	pos=s.find_first_of('?',pos);
	if(pos!=-1) {
	  wildcardPos.push_back(pos);
	}
	else{
	  break;
	}
      }
      if(wildcardPos.size()!=0) {
	int nEnd=facilities::Util::catchOptionVal(&s)+1;
	int maxNo=pow(10.,(int)wildcardPos.size());
	if(nEnd!=0&&nEnd<maxNo) maxNo=nEnd;
	int nStart=facilities::Util::catchOptionVal(&s,0,"&(",")");
	int i=1;
	if(nStart!=-1) i=nStart;
	if(nStart>nEnd) log<<MSG::WARNING<<"file's StartNo>EndNo"<<endreq;  
	for(;i<maxNo;i++){
	  std::vector<int>::iterator it=wildcardPos.end();
	  int id=i;
	  while(it!=wildcardPos.begin()){
	    it--;
	    char c=id%10+48;
	    id /=10;
	    s.replace((*it),1,1,c);
	  }
	  if(access(s.c_str(),0)!=-1) {
	    sc = m_rootInterface->addInput(m_dtreeName,s);
	    if (sc.isFailure()) return sc;
	  }
	}
      }
      else {
	sc = m_rootInterface->addInput(m_dtreeName,m_difileName[i]);
	if (sc.isFailure()) return sc;
      }
    }
    
    //facilities::Util::expandEnvVar(&m_difileName[0]);
    //sc = m_rootInterface->addInput(m_dtreeName,m_difileName[0]);
    //if (sc.isFailure()) return sc;
    
    
    //Digi
    //facilities::Util::expandEnvVar(&m_difileName);
    facilities::Util::expandEnvVar(&m_dofileName);
   // sc = m_rootInterface->addInput(m_dtreeName,m_difileName);
   // if (sc.isFailure()) return sc;

    if (m_dsplitMode<m_minSplit) {
      log << MSG::WARNING << "RootCnvSvc::initFiles Recon splitlevel cant be lower than "<<m_minSplit<<", reset" << endreq;
      m_dsplitMode=m_minSplit;
    }

    if( "" != m_dofileName ){
      sc=m_rootInterface->addOutput(m_dtreeName,m_dofileName,m_dsplitMode,m_dbufSize,m_dcompressionLevel);
    }else{
      log << MSG::WARNING << "No specified digiRootOutputFile!" << endreq;
    }

    return sc;
}


StatusCode RootCnvSvc::declareObject(const Leaf& leaf)   {
  // Purpose and Method:  Callback from each of the individual converters that allows
  //  association of TDS path and converter.
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "RootCnvSvc::declareObject " << leaf.path << " classid: " << leaf.clid << endreq;
  Leaf* ll = new Leaf(leaf);
  std::pair<LeafMap::iterator, bool> p = m_leaves.insert(LeafMap::value_type( leaf.path, ll) );
  if( p.second )    {
    return StatusCode::SUCCESS;
  }
  delete ll;
  return StatusCode::FAILURE;
}

/// Update state of the service
StatusCode RootCnvSvc::updateServiceState(IOpaqueAddress* pAddress)    {

  // not sure about the use of recid or bank...
  MsgStream log(msgSvc(), name());

  log << MSG::DEBUG << "RootCnvSvc::updateServiceState" << endreq;

  StatusCode status = INVALID_ADDRESS;
  IRegistry* ent = pAddress->registry();
  if ( 0 != ent )   {
    SmartIF<IDataManagerSvc> iaddrReg(dataProvider());
    //       if ( 0 != iaddrReg )   {
    status = StatusCode::SUCCESS;
    std::string path = ent->identifier();
    LeafMap::iterator itm = m_leaves.find(path);
    if ( itm != m_leaves.end() )    {
      Leaf* leaf = (*itm).second;
      if ( 0 != leaf )    {
	for ( Leaf::iterator il = leaf->begin(); il != leaf->end(); il++ )   {
	  IOpaqueAddress* newAddr = 0;
	  unsigned long ipars[2] = {0, 0}; //
	  if (m_evtsel)  ipars[0]=m_evtsel->getRecId();

	  std::string spars[3]={(*il)->path,(*il)->treename,(*il)->branchname}; 

	  StatusCode ir =createAddress(ROOT_StorageType, 
	      (*il)->clid, 
	      spars, 
	      ipars,
	      newAddr);
	  log << MSG::DEBUG << "RootCnvSvc::updateService " << " *****  " << 
	    (*il)->clid << " *****  " << (*il)->path <<endreq;

	  if ( ir.isSuccess() )   {
	    ir = iaddrReg->registerAddress((*il)->path, newAddr);
	    if ( !ir.isSuccess() )    {
	      newAddr->release();
	      status = ir;
	    }
	  }    
	}
      }
    }
  }
  return StatusCode::SUCCESS;
  }

  StatusCode RootCnvSvc::queryInterface(const InterfaceID& riid, void** ppvInterface)  {

    if ( IID_IRootCnvSvc == riid )  {
      *ppvInterface = (RootCnvSvc*)this;
    }
    else  {
      // Interface is not directly availible: try out a base class
      return ConversionSvc::queryInterface(riid, ppvInterface);
    }
    addRef();
    return StatusCode::SUCCESS;
  }

  StatusCode RootCnvSvc::createAddress(long int svc_type,
      const CLID& clid,
      const std::string* spars, 
      const unsigned long* ipars,
      IOpaqueAddress*& refpAddress) 
  {
    MsgStream log( msgSvc(), name() );

    if (svc_type != repSvcType() ) {
      log << MSG::ERROR << "RootCnvSvc::bad storage type" << svc_type << endreq;
      return StatusCode::FAILURE;
    }

    std::string path = spars[0];
    std::string treename = spars[1];
    std::string branchname = spars[2];
    int entry=ipars[0];
    if (path.size()==0) path="/Event";  
    refpAddress = new RootAddress(ROOT_StorageType,
	clid,
	path,treename,branchname,entry);
    return StatusCode::SUCCESS;
  }


  StatusCode RootCnvSvc::commitOutput(const std::string& output, bool do_commit)
  {



    MsgStream log( msgSvc(), name() );
    log << MSG::INFO << "RootCnvSvc::commitOutput starts." << endreq;

    StatusCode sc=m_rootInterface->fillTrees();
    if (sc.isFailure()) 
      log << MSG::ERROR << "No Root tree was filled" << endreq;

    // objects must be cleared after fill
    else{
      TObject*   oEvtHeader = m_evtheaderCnv->getWriteObject();
      if (oEvtHeader) oEvtHeader->Clear();
      TObject*   oEvtNavigator = m_evtnavigatorCnv->getWriteObject();
      if (oEvtNavigator) oEvtNavigator->Clear();
      TObject*   o=m_dCnv->getWriteObject();
      if (o)    o->Clear();
      TObject*   oDst=m_dstCnv->getWriteObject();
      if (oDst)    oDst->Clear(); 
      TObject*   oMc=m_mcCnv->getWriteObject();
      if (oMc)    oMc->Clear();
      TObject*   oRecTrack=m_rectrackCnv->getWriteObject();
      if (oRecTrack) oRecTrack->Clear();  
      TObject*   oEvtRecEvent = m_evtRecCnv->getWriteObject();
      if (oEvtRecEvent) oEvtRecEvent->Clear();
      TObject*   oTrig = m_trigCnv->getWriteObject();
      if(oTrig) oTrig->Clear();
      TObject*   oHlt = m_hltCnv->getWriteObject();
      if(oHlt) oHlt->Clear();
    }
    m_common.clear();

    return sc;
  }

  StatusCode RootCnvSvc::createAddress(DataObject *obj,IOpaqueAddress*& newAddr)    {

    //    create address for this object
    MsgStream log(msgSvc(), name());


    log << MSG::DEBUG << "RootCnvSvc::createAddress"<<endreq;

    StatusCode status = INVALID_ADDRESS;
    IRegistry* ent = obj->registry();

    if ( 0 != ent )   {
      SmartIF<IDataManagerSvc> iaddrReg(dataProvider());
      std::string path = ent->identifier();
      LeafMap::iterator itm = m_leaves.find(path);


      if ( itm != m_leaves.end() )    {
	Leaf* leaf = (*itm).second;
	if ( 0 != leaf )    {
	  std::string spars[3] ;
	  spars[0]=path;
	  spars[1]=leaf->treename;
	  spars[2]=leaf->branchname;


	  unsigned long ipars[2] ={0,0};  
	  if (m_evtsel)  ipars[0]=m_evtsel->getRecId();

	  status=addressCreator()->createAddress(ROOT_StorageType,leaf->clid,spars,ipars,newAddr);
	  if ( status.isSuccess() )   {
	    status = iaddrReg->registerAddress((IRegistry*)0,path, newAddr);
	    if ( !status.isSuccess() )    {
	      newAddr->release();
	    }
	  }
	  return StatusCode::SUCCESS;
	}
      }
    }
    return status;
  }

  StatusCode RootCnvSvc::createAddress(std::string path,IOpaqueAddress*& newAddr)    {

    //    create address for this object
    MsgStream log(msgSvc(), name());

    StatusCode status;
    LeafMap::iterator itm = m_leaves.find(path);
    SmartIF<IDataManagerSvc> iaddrReg(dataProvider());
    if ( itm != m_leaves.end() )    {
      Leaf* leaf = (*itm).second;
      if ( 0 != leaf )    {
	std::string spars[3] ;
	spars[0]=path;
	spars[1]=leaf->treename;
	spars[2]=leaf->branchname;

	unsigned long ipars[2] = {0,0};
	if (m_evtsel) ipars[0]=m_evtsel->getRecId();  
	status=addressCreator()->createAddress(ROOT_StorageType,leaf->clid,spars,ipars,newAddr);
	if ( status.isSuccess() )   {
	  status = iaddrReg->registerAddress((IRegistry*)0,path, newAddr);
	  if ( !status.isSuccess() )    {
	    newAddr->release();
	  }
	}
	return StatusCode::SUCCESS;
      }
    }


    return StatusCode::FAILURE;
  }


  /// Add converters to the service  
  StatusCode RootCnvSvc::addConverters () {
    MsgStream log(msgSvc(), name());
    declareObject(Leaf(EventModel::Dst::DstMdcDedxCol, MdcDedxCnv::classID(), "Event", "m_mdcDedxCol"));
    declareObject(Leaf(EventModel::Dst::Event, DstCnv::classID(), "Event", ""));
    declareObject(Leaf(EventModel::Digi::EmcDigiCol, EmcDigiCnv::classID(), "Event", "m_emcDigiCol"));
    declareObject(Leaf(EventModel::MC::EmcMcHitCol, EmcMcHitCnv::classID(), "Event", "m_emcMcHitCol"));
    declareObject(Leaf(EventModel::Dst::DstEmcShowerCol, EmcTrackCnv::classID(), "Event", "m_emcTrackCol"));

    declareObject(Leaf("/Event", EventCnv::classID(), "", ""));
    declareObject(Leaf(EventModel::EventHeader, EvtHeaderCnv::classID(), "Event", "m_runId:m_eventId:m_time:m_eventTag:m_flag1:m_flag2"));
    declareObject(Leaf(EventModel::Navigator, EvtNavigatorCnv::classID(), "Event", "m_mcMdcMcHits:m_mcMdcTracks:m_mcEmcMcHits:m_mcEmcRecShowers"));
    declareObject(Leaf(EventModel::Dst::DstExtTrackCol, ExtTrackCnv::classID(), "Event", "m_extTrackCol"));
    declareObject(Leaf(EventModel::MC::Event, McCnv::classID(), "Event", ""));
    declareObject(Leaf(EventModel::MC::McParticleCol, McParticleCnv::classID(), "Event", "m_mcParticleCol"));
    declareObject(Leaf(EventModel::Digi::MdcDigiCol, MdcDigiCnv::classID(), "Event", "m_mdcDigiCol"));
    declareObject(Leaf(EventModel::Dst::DstMdcKalTrackCol, MdcKalTrackCnv::classID(), "Event", "m_mdcKalTrackCol"));
    declareObject(Leaf(EventModel::MC::MdcMcHitCol, MdcMcHitCnv::classID(), "Event", "m_mdcMcHitCol"));
    declareObject(Leaf(EventModel::Dst::DstMdcTrackCol, MdcTrackCnv::classID(), "Event", "m_mdcTrackCol"));
    declareObject(Leaf(EventModel::Digi::MucDigiCol, MucDigiCnv::classID(), "Event", "m_mucDigiCol"));
    declareObject(Leaf(EventModel::MC::MucMcHitCol, MucMcHitCnv::classID(), "Event", "m_mucMcHitCol"));
    declareObject(Leaf(EventModel::Dst::DstMucTrackCol, MucTrackCnv::classID(), "Event", "m_mucTrackCol"));
    declareObject(Leaf(EventModel::Digi::Event, DigiCnv::classID(), "Event", "m_fromMc"));
    declareObject(Leaf(EventModel::Digi::TofDigiCol, TofDigiCnv::classID(), "Event", "m_tofDigiCol"));
    declareObject(Leaf(EventModel::MC::TofMcHitCol, TofMcHitCnv::classID(), "Event", "m_tofMcHitCol"));
    declareObject(Leaf(EventModel::Dst::DstTofTrackCol, TofTrackCnv::classID(), "Event", "m_tofTrackCol"));
    //declareObject(Leaf("/Event/Digi/LumiDigiCol", LumiDigiCnv::classID(), "Event", "m_lumiDigiCol"));
    declareObject(Leaf(EventModel::Digi::LumiDigiCol, LumiDigiCnv::classID(), "Event", "m_lumiDigiCol"));

    declareObject(Leaf(EventModel::Trig::Event, TrigCnv::classID(), "Event", ""));
    declareObject(Leaf(EventModel::Trig::TrigData, TrigDataCnv::classID(), "Event", "m_trigData"));

    declareObject(Leaf(EventModel::Hlt::Event, HltCnv::classID(), "Event", ""));
    declareObject(Leaf(EventModel::Hlt::HltRawCol, HltRawCnv::classID(), "Event", "m_hltRawCol"));
    declareObject(Leaf(EventModel::Hlt::HltInf, HltInfCnv::classID(), "Event", "m_hltInf"));
    declareObject(Leaf(EventModel::Hlt::DstHltInf, DstHltInfCnv::classID(), "Event", "m_dstHltInf"));

    declareObject(Leaf(EventModel::Recon::Event, RecTrackCnv::classID(), "Event", ""));

    declareObject(Leaf(EventModel::Recon::RecMdcTrackCol, RecMdcTrackCnv::classID(), "Event", "m_recMdcTrackCol"));
    declareObject(Leaf(EventModel::Recon::RecMdcHitCol, RecMdcHitCnv::classID(), "Event", "m_recMdcHitCol"));
    declareObject(Leaf(EventModel::Recon::RecMdcKalTrackCol, RecMdcKalTrackCnv::classID(),"Event", "m_recMdcKalTrackCol"));
    declareObject(Leaf(EventModel::Recon::RecMdcKalHelixSegCol, RecMdcKalHelixSegCnv::classID(),"Event", "m_recMdcKalHelixSegCol"));
    declareObject(Leaf(EventModel::Recon::RecMdcDedxCol, RecMdcDedxCnv::classID(),"Event", "m_recMdcDedxCol"));
    declareObject(Leaf(EventModel::Recon::RecMdcDedxHitCol, RecMdcDedxHitCnv::classID(),"Event", "m_recMdcDedxHitCol"));

    declareObject(Leaf(EventModel::Recon::RecEsTimeCol, RecEvTimeCnv::classID(),"Event", "m_recEvTimeCol"));
    declareObject(Leaf(EventModel::Recon::RecTofTrackCol, RecTofTrackCnv::classID(), "Event", "m_recTofTrackCol"));
    declareObject(Leaf(EventModel::Recon::RecBTofCalHitCol, RecBTofCalHitCnv::classID(), "Event", "m_recBTofCalHitCol"));
    declareObject(Leaf(EventModel::Recon::RecETofCalHitCol, RecETofCalHitCnv::classID(), "Event", "m_recETofCalHitCol"));
    declareObject(Leaf(EventModel::Recon::RecEmcHitCol, RecEmcHitCnv::classID(), "Event", "m_recEmcHitCol"));
    declareObject(Leaf(EventModel::Recon::RecEmcClusterCol, RecEmcClusterCnv::classID(), "Event", "m_recEmcClusterCol"));
    declareObject(Leaf(EventModel::Recon::RecEmcShowerCol, RecEmcShowerCnv::classID(), "Event", "m_recEmcShowerCol"));
    declareObject(Leaf(EventModel::Recon::RecMucTrackCol, RecMucTrackCnv::classID(), "Event", "m_recMucTrackCol"));
    declareObject(Leaf(EventModel::Recon::MucRecHitCol, RecMucRecHitCnv::classID(), "Event", "m_recMucHitCol"));
    declareObject(Leaf(EventModel::Recon::RecExtTrackCol, RecExtTrackCnv::classID(),"Event", "m_recExtTrackCol"));
    declareObject(Leaf(EventModel::Recon::RecZddChannelCol, RecZddChannelCnv::classID(), "Event", "m_recZddChannelCol"));   
    declareObject(Leaf(EventModel::EvtRec::Event, EvtRecCnv::classID(), "Event", ""));
    declareObject(Leaf(EventModel::EvtRec::EvtRecEvent, EvtRecEventCnv::classID(), "Event", "m_evtRecEvent"));
    declareObject(Leaf(EventModel::EvtRec::EvtRecTrackCol, EvtRecTrackCnv::classID(), "Event", "m_evtRecTrackCol"));
    declareObject(Leaf(EventModel::EvtRec::EvtRecPrimaryVertex, EvtRecPrimaryVertexCnv::classID(), "Event", "m_evtRecPrimaryVertex"));
    declareObject(Leaf(EventModel::EvtRec::EvtRecVeeVertexCol, EvtRecVeeVertexCnv::classID(), "Event", "m_evtRecVeeVertexCol"));   
    declareObject(Leaf(EventModel::EvtRec::EvtRecPi0Col, EvtRecPi0Cnv::classID(), "Event", "m_evtRecPi0Col"));   
    declareObject(Leaf(EventModel::EvtRec::EvtRecEtaToGGCol, EvtRecEtaToGGCnv::classID(), "Event", "m_evtRecEtaToGGCol"));   
    declareObject(Leaf(EventModel::EvtRec::EvtRecDTagCol, EvtRecDTagCnv::classID(), "Event", "m_evtRecDTagCol"));   

    return StatusCode::SUCCESS;

  }
