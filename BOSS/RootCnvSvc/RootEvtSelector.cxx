// File and Version Information:
// $Header: /bes/bes/BossCvs/Event/RootCnvSvc/src/RootEvtSelector.cxx,v 1.20 2015/06/17 03:12:16 dengzy Exp $
// 
// Description:
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/GenericAddress.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootInterface.h"
#include "RootCnvSvc/RootEventContext.h"
#include "EventModel/EventModel.h"
#include "TagFilterSvc/ITagFilterSvc.h"
#include "TagFilterSvc/TagFilterSvc.h"

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISvcLocator.h"

#include "TFile.h"
#include "TTree.h"
#include "TFormula.h"
#include <fstream>
#include <vector>

extern const CLID& CLID_Event;
extern const long int ROOT_StorageType;

//static const SvcFactory<RootEvtSelector> s_factory;
//const ISvcFactory& RootEvtSelectorFactory = s_factory;

// Constructor
RootEvtSelector::RootEvtSelector( const std::string& name, ISvcLocator* svcloc ) 
  : Service( name, svcloc),
    p_beginContext(0),
    p_endContext(0) {
    m_recId = new int;
    (*m_recId) = -1;
    declareProperty( "Input", m_jobInput="ROOT");
    getMaxEvent();
}
RootEvtSelector::~RootEvtSelector() {
  delete m_recId;
}
// Instantiation of a static factory class used by clients to create
// instances of this service
StatusCode RootEvtSelector::initialize()     {
  MsgStream log(msgSvc(), name());
  StatusCode sc = Service::initialize();
  log << MSG::DEBUG << "RootEvtSelector::initialize" << endreq;    if( sc.isSuccess() ) {
    setProperties();
  } else {
    log << MSG::ERROR << "Unable to initialize service " << endreq;
  }
    
  if(m_jobInput.value() != " "){
    sc = setCriteria(m_jobInput);
    if(sc.isFailure()){
      log << MSG::ERROR << "Unable to get input value" << endreq;
      return sc;
    }
  }
    
  // Retrieve conversion service handling event iteration
  sc = serviceLocator()->service("EventCnvSvc", m_addrCreator);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << 
      "Unable to localize interface IID_IAddressCreator from service:" 
	<< "RootEventSelector" 
	<< endreq;
    return sc;
  }

  // Get DataSvc
  IDataManagerSvc* eds = 0;
  sc = serviceLocator()->service("EventDataSvc", eds, true);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR 
	<< "Unable to localize interface IID_IDataManagerSvc "
	<< "from RootEventSelector"
	<< endreq;
    return sc;
  }
    
  m_rootCLID = eds->rootCLID();

  // provide access to the Root Interface
  m_rootInterface=RootInterface::Instance(log);
  log << MSG::INFO << "currentFile in RootEvtSelector::initialize "<<m_rootInterface->getCurrentFileName()<<endreq;
  if (!m_rootInterface)      log << MSG::ERROR << "Unable to start Root service within RootCnvSvc" << endreq;

  // create root event context
  p_beginContext = new RootEventContext(this); 

 
  ITagFilterSvc *tmpSvc;

  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode status = svcLocator->service("TagFilterSvc",tmpSvc);
  if (status.isSuccess()) {
    log << MSG::INFO << "get the TagFilterSvc" << endreq;
    m_tagFilterSvc=dynamic_cast<TagFilterSvc *>(tmpSvc);
  }else {
    log << MSG::WARNING << "could not get the TagFilterSvc. Ignore it." << endreq;
  }

  m_rootInterface->setTagInputFile(m_tagFilterSvc->getTagFiles());

  vector<string> dstFiles = m_tagFilterSvc->getDstFiles();
  int size = dstFiles.size();
  std::string treeName="Event";

  for(int i=0;i<size;i++)
  {
    std::cout<<"RootEvtSelector, dstFile: "<<dstFiles[i]<<std::endl;
    m_rootInterface->addInput(treeName, dstFiles[i].c_str());
  }
  return sc;
}

StatusCode RootEvtSelector::setCriteria( const std::string& criteria ) {
  // Purpose and Method:  set the input criteria
  MsgStream log(msgSvc(), name());
  m_criteria = criteria;
  
  if( criteria == "ROOT" ) {
    log << MSG::INFO << "RootEvtSelector input has been set to ROOT" << endreq;
    m_criteriaType = ROOT;
  }else if( criteria == "NONE"){
    m_criteriaType = NONE;
    log << MSG::INFO << "RootEvtSelector input has been set to NONE" << endreq;
  }
  else {
    log << MSG::ERROR << "Invalid Event Selection Criteria: " << criteria << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode
RootEvtSelector::createContext(IEvtSelector::Context*& it) const
{
  it = p_beginContext;
  return(StatusCode::SUCCESS);
}

// Implementation of IEvtSelector::next().
StatusCode RootEvtSelector::next(IEvtSelector::Context& it) const 
{
  return this->next(); 
}

//__________________________________________________________________________
StatusCode RootEvtSelector::next(IEvtSelector::Context& ctxt, int jump) const {
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = next(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode RootEvtSelector::previous(IEvtSelector::Context& /*it*/) const {
  MsgStream log(messageService(), name());
  log << MSG::ERROR << "RootEvtSelector::previous() not implemented" << endreq;
  return(StatusCode::FAILURE);
}

StatusCode RootEvtSelector::previous(IEvtSelector::Context& it,int /*jump*/) const {
  return(previous(it));
}

StatusCode RootEvtSelector::last(IEvtSelector::Context& it)const {
  if (it.identifier() == p_endContext->identifier()) {
    MsgStream log(messageService(), name());
    log << MSG::DEBUG << "last(): Last event in InputStream." <<endreq;
    return(StatusCode::SUCCESS);
  }
  return (StatusCode::FAILURE);
}

StatusCode RootEvtSelector::resetCriteria(const std::string& /*criteria*/, IEvtSelector::Context& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}


StatusCode RootEvtSelector::rewind(IEvtSelector::Context& /*it*/) const {
   MsgStream log(messageService(), name());
   log << MSG::ERROR << "RootEvtSelector::rewind() not implemented" << endreq;
   return(StatusCode::FAILURE);
}

StatusCode
RootEvtSelector::releaseContext(IEvtSelector::Context*& /*it*/)const {
   return(StatusCode::SUCCESS);
}

StatusCode RootEvtSelector::createAddress(const IEvtSelector::Context& /*it*/,
					  IOpaqueAddress*& iop) const {
  iop = new GenericAddress(ROOT_StorageType, CLID_Event, " ", " ", 0);
  return(StatusCode::SUCCESS);
}

void RootEvtSelector::getEntryFromTag() const {
  bool flag=true;;
  MsgStream log(messageService(), name());
  static std::string tempFile = "";
  log << MSG::DEBUG <<"current file: "<<m_rootInterface->getCurrentFileName()<<endreq;
  std::ifstream fin;
  static std::string tagFile;
  static int i=-1;
  static std::vector<int> ventry;
  int num1 = m_rootInterface->getCurrentFileNum();
  int num2 = m_rootInterface->getTotalFileNum();

  
  if(tempFile != m_rootInterface->getCurrentFileName())
  {
    tempFile = m_rootInterface->getCurrentFileName();
    log << MSG::DEBUG<<"temp file: "<<tempFile<<endreq;
    i=-1;
    ventry.clear();
    log << MSG::DEBUG<<"num1: "<<num1<<" num2: "<<num2<<endreq;
    tagFile = (m_rootInterface->getTagInputFile())[num1];
    log << MSG::DEBUG <<"tag file: "<<tagFile<<endreq;
    m_tagFilterSvc->getVEntry(tagFile, ventry);
  }  //end of read a tag file

  i++;
  log << MSG::DEBUG<<"ventry i: "<<i<<endreq;
  log << MSG::DEBUG<<"ventry size: "<<ventry.size()<<endreq;
  if(i<ventry.size())
  {
    (*m_recId)=ventry[i];
    log << MSG::DEBUG<<"ventry[i]: "<<ventry[i]<<endreq;
  }
  else
    (*m_recId)= -1;

  log << MSG::DEBUG<<"(*m_recId): "<<(*m_recId)<<endreq;

  int vs = ventry.size();

  if( (i>(vs-1)) && (num1<(num2-1)))
  {  
    bool endAll = m_rootInterface->checkEndOfTree();
    getEntryFromTag();
  }
  if( (i>(vs-1)) && (num1==(num2-1)))
    (*m_recId)= -1;
}


StatusCode RootEvtSelector::next()  const {  
    MsgStream log(msgSvc(), name());
    if(m_criteriaType == ROOT)
    {
      log<<MSG::INFO<<"m_criteriaType == ROOT"<<endreq; 
      if(m_rootInterface->getSelectFromTag()!=0)
      {
        getEntryFromTag();
        if((*m_recId)<0)
           return StatusCode::FAILURE;
      }

      else
      {
        (*m_recId)++;        

        //if((*m_recId)>m_evtMax){ // if get the max event number?
        //  return StatusCode::FAILURE;
        //}else{  //didn't get the max event number
        
        static int entry = 0;

        bool endOfFile = m_rootInterface->getEOF();
        entry++;
        bool maxEntry = (entry >m_rootInterface->getEntries() &&  m_rootInterface->getEntries()>=0); 
        if(maxEntry) entry =1;
        log<<MSG::INFO<<"entry = "<<entry<<"    "<<maxEntry<<"   "<<endOfFile<<endreq;
        if(endOfFile || maxEntry){   //the using file is get max event or get the end of file
          bool endAll = m_rootInterface->checkEndOfTree();

          if(endAll){ //if all is finished ?
            return StatusCode::FAILURE;
          }
        }
      }
      //}  // to make it run if not set event number from jobOption
    }// end if(m_criteriaType == ROOT) 
    else if( m_criteriaType == NONE)
    {     
      log << MSG::DEBUG << "Reading Event " << (*m_recId) << endreq;
      //If we go over the count set equal to the end
      if((*m_recId) > m_evtMax) {
        return StatusCode::FAILURE;
      }
    }else {
      //We are going to blow up if this situation happens
      log << MSG::FATAL << "Cannot iterate a dead iterator" << endreq;
      return StatusCode::FAILURE;
    } 

    log << MSG::DEBUG << "Reading Event " <<  (*m_recId) << endreq;
    log << MSG::DEBUG << "Max Event " <<  m_evtMax << endreq; 
   return StatusCode::SUCCESS;
}

StatusCode RootEvtSelector::queryInterface(const InterfaceID& riid, void** ppvInterface) {
  if ( riid == IID_IRootEvtSelector )  {
    *ppvInterface = (RootEvtSelector*)this;
  }
  else if ( riid == IEvtSelector::interfaceID() )  {
    *ppvInterface = (IEvtSelector*)this;
  }
  else if ( riid == IProperty::interfaceID() )  {
    *ppvInterface = (IProperty*)this;
  }
  else   {
    return Service::queryInterface( riid, ppvInterface );
  }
  addRef();
  return SUCCESS;
}

StatusCode RootEvtSelector::getMaxEvent() {
  IProperty* appPropMgr=0;
  StatusCode status = 
    serviceLocator()->getService("ApplicationMgr", IProperty::interfaceID(),
				 reinterpret_cast<IInterface*&>( appPropMgr ));
  if( status.isFailure() ) return status;
    
  IntegerProperty evtMax("EvtMax",0);
  status = appPropMgr->getProperty( &evtMax );
  if (status.isFailure()) return status;
  
  m_evtMax = evtMax.value();
  return status;
}







