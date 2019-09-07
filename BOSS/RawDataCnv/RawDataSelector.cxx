//====================================================================
//	EventSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : RawDataSelector
//
//  Description: The EventSelector component is able
//               to produce a list of event references given 
//               a set of "selection criteria".
//
//
//====================================================================

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
// for Mutil-thread by tianhl
#include "GaudiKernel/ThreadGaudi.h"
// for Mutil-thread by tianhl

#include "RawDataCnv/RawDataContext.h"
#include "RawDataCnv/RawDataInputSvc.h"
#include "RawDataCnv/EventManagement/RAWEVENT.h"
#include "RawDataCnv/RawDataAddress.h"  
#include "RawDataCnv/RawDataSelector.h"



extern const CLID& CLID_Event;

RawDataSelector::RawDataSelector( const std::string& name, ISvcLocator* svcloc ) :
  Service( name, svcloc),
  m_beginContext(0),
  m_endContext(0)
{
   // Get a pointer to the Job Options Service
   PropertyMgr m_propMgr;
   m_propMgr.declareProperty("InputSvc", m_eventSourceName);
   IJobOptionsSvc* jobSvc;
   Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc);
   jobSvc->setMyProperties("RawDataSelector", &m_propMgr);
	     
}

RawDataSelector::~RawDataSelector() {
  //if (m_eventSource)
    //delete m_eventSource;
  if (m_beginContext)
    delete m_beginContext;
  if (m_endContext)
    delete m_endContext;
}

StatusCode RawDataSelector::initialize()     {

   // Create a message stream.
   MsgStream log(messageService(), name());

   // Initialize the Service base class.
   StatusCode sc = Service::initialize();
   if( sc.isSuccess() ) 
   {
        setProperties();
   } 
   else 
   {
      log << MSG::ERROR << "RawDataSelector::initialize: Unable to " <<
      "initialize Service base class." << endreq;
      return sc;
    }

    IService* svc ;
    
    // for Mutil-thread by tianhl
    if  (isGaudiThreaded(name()))m_eventSourceName += getGaudiThreadIDfromName(name());
    log << MSG::INFO << name() << " get RawDataInputSvc by name: " << m_eventSourceName << endreq;
    // for Mutil-thread by tianhl

    sc = serviceLocator()->getService(m_eventSourceName, svc); 
    if(sc != StatusCode::SUCCESS ) {
       log<<MSG::ERROR << " Cant get RawDataInputSvc " <<endreq;
       return sc ;
    }
			     
    m_eventSource = dynamic_cast<RawDataInputSvc*> (svc); 
    if(m_eventSource == 0 ) {
        log<<MSG::ERROR<< "  Cant cast to  RawDataInputSvc " <<endreq; 
        return StatusCode::FAILURE ;
    }

    m_beginContext = new RawDataContext(this); 

    //m_endContext   = new RawDataContext(this, m_eventSource->end());

    return sc;
}

StatusCode
RawDataSelector::createContext(IEvtSelector::Context*& it) const
{
    m_eventSource->initialize();
    it = m_beginContext;
    return(StatusCode::SUCCESS);
}

// Implementation of IEvtSelector::next().
StatusCode RawDataSelector::next(IEvtSelector::Context& it)const 
{
  RAWEVENT* pre = m_eventSource->nextEvent();
  //((RawDataContext &)it).next();
  return StatusCode::SUCCESS; 
}

//__________________________________________________________________________
StatusCode RawDataSelector::next(IEvtSelector::Context& ctxt, int jump) const {
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

StatusCode RawDataSelector::previous(IEvtSelector::Context& /*it*/) const {
   MsgStream log(messageService(), name());
   log << MSG::ERROR << "RawDataSelector::previous() not implemented" << endreq;
   return(StatusCode::FAILURE);
}

StatusCode RawDataSelector::previous(IEvtSelector::Context& it,int /*jump*/) const {
   return(previous(it));
}

StatusCode RawDataSelector::last(IEvtSelector::Context& it)const {
   return (StatusCode::FAILURE);
}

StatusCode RawDataSelector::resetCriteria(const std::string& /*criteria*/, IEvtSelector::Context& /*ctxt*/) const {
   return(StatusCode::SUCCESS);
}


StatusCode RawDataSelector::rewind(IEvtSelector::Context& /*it*/) const {
   MsgStream log(messageService(), name());
   log << MSG::ERROR << "RawDataSelector::rewind() not implemented" << endreq;
   return(StatusCode::FAILURE);
}

StatusCode RawDataSelector::createAddress(const IEvtSelector::Context& /*it*/,
                IOpaqueAddress*& iop) const {
     iop = new RawDataAddress(CLID_Event, "EventHeader", "EventHeader");

   return(StatusCode::SUCCESS);
}

StatusCode
RawDataSelector::releaseContext(IEvtSelector::Context*& /*it*/)const {
   return(StatusCode::SUCCESS);
}

StatusCode 
RawDataSelector::queryInterface(const InterfaceID& riid, void** ppvInterface)
{
  if ( riid == IEvtSelector::interfaceID() )  {
    *ppvInterface = (IEvtSelector*)this;
  }
  else if ( riid == IProperty::interfaceID() )  {
    *ppvInterface = (IProperty*)this;
  }
  else   {
    return Service::queryInterface( riid, ppvInterface );
  }
  addRef();
  return StatusCode::SUCCESS; 
}




