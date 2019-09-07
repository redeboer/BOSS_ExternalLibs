// $Id: OfflineEventLoopMgr.cxx,v 1.11 2011/02/15 06:57:12 dengzy Exp $
//#define  GAUDISVC_EVENTLOOPMGR_CPP

#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IEvtSelector.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IAlgorithm.h"
#include "ApplicationMgr/HistogramAgent.h"
#include "OfflineEventLoopMgr/OfflineEventLoopMgr.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

// Instantiation of a static factory class used by clients to create instances of this service
//static SvcFactory<OfflineEventLoopMgr> s_EventLoopMgrFactory;
//const IFactory& OfflineEventLoopMgrFactory = s_EventLoopMgrFactory;
extern const CLID& CLID_Event;

//--------------------------------------------------------------------------------------------
// Standard Constructor
//--------------------------------------------------------------------------------------------
OfflineEventLoopMgr::OfflineEventLoopMgr(const std::string& nam, ISvcLocator* svcLoc)
: MinimalEventLoopMgr(nam, svcLoc)
{
  m_histoDataMgrSvc   = 0;
  m_histoPersSvc      = 0;
  m_incidentSvc       = 0;
  m_evtDataMgrSvc     = 0;
  m_evtDataSvc        = 0;
  m_evtSelector       = 0;
  m_evtContext       = 0;
  m_runNo            = -1.1;   
  // Declare properties
  declareProperty("HistogramPersistency", m_histPersName = "");
  declareProperty( "EvtSel", m_evtsel );
}

//--------------------------------------------------------------------------------------------
// Standard Destructor
//--------------------------------------------------------------------------------------------
OfflineEventLoopMgr::~OfflineEventLoopMgr()   {
  if( m_histoDataMgrSvc ) m_histoDataMgrSvc->release();
  if( m_histoPersSvc ) m_histoPersSvc->release();
  if( m_incidentSvc ) m_incidentSvc->release();
  if( m_evtDataMgrSvc ) m_evtDataMgrSvc->release();
  if( m_evtDataSvc ) m_evtDataSvc->release();
  if( m_evtSelector ) m_evtSelector->release();
  if( m_evtContext ) delete m_evtContext;
}

//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::initalize
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::initialize()    {
  MsgStream log(msgSvc(), name());

  // initilaize the base class
  StatusCode sc = MinimalEventLoopMgr::initialize();
  if( !sc.isSuccess() ) {
    log << MSG::DEBUG << "Error Initializing base class MinimalEventLoopMgr." << endreq;
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

  // Get the references to the services that are needed by the ApplicationMgr itself
  sc = serviceLocator()->service("IncidentSvc", m_incidentSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving IncidentSvc." << endreq;
    return sc;
  }

  // Obtain the IProperty of the ApplicationMgr
 // SmartIF<IProperty> prpMgr(IID_IProperty, serviceLocator());
 SmartIF<IProperty> prpMgr(serviceLocator());
  if ( ! prpMgr.isValid() )   {
    log << MSG::FATAL << "IProperty interface not found in ApplicationMgr." << endreq;
    return StatusCode::FAILURE;
  }
  else {
    m_appMgrProperty = prpMgr;
  }

  // We do not expect a Event Selector necessarily being declared
  setProperty(m_appMgrProperty->getProperty("EvtSel"));

  if( m_evtsel != "NONE" || m_evtsel.length() == 0) {
    sc = serviceLocator()->service( "EventSelector", m_evtSelector, true );
    if( sc.isSuccess() )     {
      // Setup Event Selector
      sc=m_evtSelector->createContext(m_evtContext);
      if( !sc.isSuccess() )   {
        log << MSG::FATAL << "Can not create the event selector Context." << endreq;
        return sc;
      }
    }
    else {
      log << MSG::FATAL << "EventSelector not found." << endreq;
      return sc;
    }
  }
  else {
    m_evtSelector = 0;
    m_evtContext = 0;
    log << MSG::WARNING << "Unable to locate service \"EventSelector\" " << endreq;    
    log << MSG::WARNING << "No events will be processed from external input." << endreq;    
  }

  // Setup access to histogramming services
  sc = serviceLocator()->service("HistogramDataSvc", m_histoDataMgrSvc, true);
  if( !sc.isSuccess() )  {
    log << MSG::FATAL << "Error retrieving HistogramDataSvc." << endreq;
    return sc;
  }
  // Setup histogram persistency
  sc = serviceLocator()->service("HistogramPersistencySvc", m_histoPersSvc, true );
  if( !sc.isSuccess() ) {
    log << MSG::WARNING << "Histograms cannot not be saved - though required." << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}
//--------------------------------------------------------------------------------------------
// implementation of IService::reinitialize
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::reinitialize() {
  MsgStream log(msgSvc(), name());

  // initilaize the base class
  StatusCode sc = MinimalEventLoopMgr::reinitialize();
  if( !sc.isSuccess() ) {
    log << MSG::DEBUG << "Error Initializing base class MinimalEventLoopMgr." << endreq;
    return sc;
  }

  // Check to see whether a new Event Selector has been specified
  setProperty(m_appMgrProperty->getProperty("EvtSel"));
  if( m_evtsel != "NONE" || m_evtsel.length() == 0) {
    IEvtSelector* theEvtSel;
    IService*     theSvc;
    sc = serviceLocator()->service( "EventSelector", theEvtSel );
    sc = serviceLocator()->service( "EventSelector", theSvc );
    if( sc.isSuccess() && ( theEvtSel != m_evtSelector ) ) {
      // Setup Event Selector
      m_evtSelector = theEvtSel;
 //     if (theSvc->state() == IService::INITIALIZED) {
   if (theSvc->FSMState() == Gaudi::StateMachine::INITIALIZED) {
        sc = theSvc->reinitialize();
        if( !sc.isSuccess() ) {
          log << MSG::ERROR << "Failure Reinitializing EventSelector "
              << theSvc->name( ) << endreq;
          return sc;
        }
      } 
      else {
        sc = theSvc->initialize();
        if( !sc.isSuccess() ) {
          log << MSG::ERROR << "Failure Initializing EventSelector "
              << theSvc->name( ) << endreq;
          return sc;
        }
      }
      sc = m_evtSelector->createContext(m_evtContext);
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Can not create Context "
            << theSvc->name( ) << endreq;
        return sc;
      }
      log << MSG::INFO << "EventSelector service changed to "
          << theSvc->name( ) << endreq;
    }
    else if ( m_evtSelector && m_evtContext )   {
      m_evtSelector->releaseContext(m_evtContext);
      m_evtContext = 0;
      sc = m_evtSelector->createContext(m_evtContext);
      if( !sc.isSuccess() ) {
        log << MSG::ERROR << "Can not create Context "
            << theSvc->name( ) << endreq;
        return sc;
      }
    }
  }
  else if ( m_evtSelector && m_evtContext )   {
    m_evtSelector->releaseContext(m_evtContext);
    m_evtSelector = 0;
    m_evtContext = 0;
  }
  return StatusCode::SUCCESS;
}

//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::finalize
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::finalize()    {
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  // Finalize base class
  MinimalEventLoopMgr::finalize();

  // Save Histograms Now
  if ( 0 != m_histoPersSvc )    {
    HistogramAgent agent;
    sc = m_histoDataMgrSvc->traverseTree( &agent );
    if( sc.isSuccess() )   {
      IDataSelector* objects = agent.selectedObjects();
      // skip /stat entry!
      if ( objects->size() > 0 )    {
        IDataSelector::iterator i;
        for ( i = objects->begin(); i != objects->end(); i++ )    {
          IOpaqueAddress* pAddr = 0;
          StatusCode iret = m_histoPersSvc->createRep(*i, pAddr);
          if ( iret.isSuccess() )     {
            (*i)->registry()->setAddress(pAddr);
          }
          else  {
            sc = iret;
          }
        }
        for ( i = objects->begin(); i != objects->end(); i++ )    {
          IRegistry* reg = (*i)->registry();
          StatusCode iret = m_histoPersSvc->fillRepRefs(reg->address(), *i);
          if ( !iret.isSuccess() )    {
            sc = iret;
          }
        }
      }
      if ( sc.isSuccess() )    {
        log << MSG::INFO << "Histograms converted successfully according to request." << endreq;
      }
      else  {
        log << MSG::ERROR << "Error while saving Histograms." << endreq;
      }
    }
    else {
      log << MSG::ERROR << "Error while traversing Histogram data store" << endreq;
    }
  }

  // Release evemt selector context
  if ( m_evtSelector && m_evtContext )   {
    m_evtSelector->releaseContext(m_evtContext);
    m_evtContext = 0;
  }

  // Release all interfaces...
  m_histoDataMgrSvc = releaseInterface(m_histoDataMgrSvc);
  m_histoPersSvc    = releaseInterface(m_histoPersSvc);

  m_evtSelector     = releaseInterface(m_evtSelector);
  m_incidentSvc     = releaseInterface(m_incidentSvc);
  m_evtDataSvc      = releaseInterface(m_evtDataSvc);
  m_evtDataMgrSvc   = releaseInterface(m_evtDataMgrSvc);

  return StatusCode::SUCCESS;
}


//--------------------------------------------------------------------------------------------
// executeEvent(void* par)
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::executeEvent(void* par)    {
   MsgStream log(msgSvc(), name());
  // Fire BeginEvent "Incident"
  m_incidentSvc->fireIncident(Incident(name(),IncidentType::BeginEvent));
  int runNo;
   SmartDataPtr<Event::EventHeader> evt(m_evtDataSvc,"/Event/EventHeader");
  if( evt ){
    runNo = evt -> runNumber();
    log << MSG::INFO <<"The runNumber of current event is  "<<runNo<<endreq;
  }

    else{
      log << MSG::ERROR << "Error accessing Event" <<endreq;
       return StatusCode::FAILURE;
     }
   if(runNo!=m_runNo) {
     log << MSG::INFO <<"fireIncident of begin run"<<endreq;
     m_incidentSvc->fireIncident(Incident(name(),"NewRun"));
     if(m_runNo==-1.1){
      ListAlg::iterator it_topAlg =  m_topAlgList.begin();
      for (;it_topAlg!=m_topAlgList.end();it_topAlg++){
       (*it_topAlg)->beginRun();}
      }
    m_runNo = runNo;    
   }

  // Execute Algorithms
  StatusCode sc = MinimalEventLoopMgr::executeEvent(par);
  // Fire EndEvent "Incident"
  m_incidentSvc->fireIncident(Incident(name(),IncidentType::EndEvent));

  // Check if there was an error processing current event
  if( !sc.isSuccess() ){
    MsgStream log( msgSvc(), name() );
    log << MSG::ERROR << "Terminating event processing loop due to errors" << endreq;
  }

  return sc;
}

//--------------------------------------------------------------------------------------------
// IEventProcessing::executeRun
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::executeRun( int maxevt )    {
  StatusCode  sc;
  // initialize the base class
  sc = MinimalEventLoopMgr::executeRun(maxevt);
  return sc;
}

//--------------------------------------------------------------------------------------------
// implementation of IAppMgrUI::nextEvent
//--------------------------------------------------------------------------------------------
StatusCode OfflineEventLoopMgr::nextEvent(int maxevt)   {
  static int        total_nevt = 0;
  DataObject*       pObject = 0;
  StatusCode        sc;
  MsgStream         log( msgSvc(), name() );

  //yzhang add
  //begin run
/*  ListAlg::iterator it_topAlg =  m_topAlgList.begin();
  for (;it_topAlg!=m_topAlgList.end();it_topAlg++){
    (*it_topAlg)->beginRun();
  }*/
  //zhangy add
  // loop over events if the maxevt (received as input) if different from -1. 
  // if evtmax is -1 it means infinite loop
  for( int nevt = 0; (maxevt == -1 ? true : nevt < maxevt);  nevt++, total_nevt++) {
    // Check if there is a scheduled stop issued by some algorithm/sevice
    if ( m_scheduledStop ) {
      m_scheduledStop = false;
      log << MSG::ALWAYS << "Terminating event processing loop due to scheduled stop" << endreq;
      break;
    }
    // Clear the event store, if used in the event loop
    if( 0 != total_nevt ) {
      sc = m_evtDataMgrSvc->clearStore();
      if( !sc.isSuccess() )  {
        log << MSG::DEBUG << "Clear of Event data store failed" << endreq;
      }
    }

    // Setup event in the event store
    if( m_evtContext ) {
      IOpaqueAddress* addr = 0;
      // Only if there is a EventSelector
      sc = getEventRoot(addr);
      if( !sc.isSuccess() )  {
        log << MSG::INFO << "No more events in event selection " << endreq;
        break;
      }
      // Set root clears the event data store first
      sc = m_evtDataMgrSvc->setRoot ("/Event", addr);
      if( !sc.isSuccess() )  {
        log << MSG::WARNING << "Error declaring event root address." << endreq;
        continue;
      }
      // register event header for mc
      SmartDataPtr<Event::EventHeader> evt(m_evtDataSvc,"/Event/EventHeader");
      if(!evt) {
        sc = m_evtDataMgrSvc->registerAddress ("/Event/EventHeader", addr);
        if( !sc.isSuccess() )  {
          log << MSG::WARNING << "Error register EventHeader address." << endreq;
        }
      }
      sc = m_evtDataSvc->retrieveObject("/Event", pObject);
      if( !sc.isSuccess() ) {
        log << MSG::WARNING << "Unable to retrieve Event root object" << endreq;
        break;
      }
    }
    else {
      sc = m_evtDataMgrSvc->setRoot ("/Event", new DataObject());
      if( !sc.isSuccess() )  {
        log << MSG::WARNING << "Error declaring event root DataObject" << endreq;
      } 
    }

    

    // Execute event for all required algorithms
    sc = executeEvent(NULL);
    if( !sc.isSuccess() ){
      log << MSG::ERROR << "Terminating event processing loop due to errors" << endreq;
      break;
    }
  }
  //yzhang add
  //end run
  ListAlg::iterator it_topAlg =  m_topAlgList.begin();
  for (;it_topAlg!=m_topAlgList.end();it_topAlg++){
    (*it_topAlg)->endRun();
  }
  //zhangy add
  return StatusCode::SUCCESS;
}

/// Create event address using event selector
StatusCode OfflineEventLoopMgr::getEventRoot(IOpaqueAddress*& refpAddr)  {
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
