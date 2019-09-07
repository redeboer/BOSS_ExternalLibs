//====================================================================
//	EventSelector.cpp
//--------------------------------------------------------------------
//
//	Package    : McEventSelector
//
//  Description: The EventSelector component is able
//              to produce a list of event references given a set of "selection
//               criteria".
//
//
//
//====================================================================

// Include files
#include <vector>
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/Bootstrap.h"
#include "McEventSelector/McEventSelector.h"
#include "McEventSelector/McAddress.h"
#include "EventModel/EventModel.h"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"
#include "DataInfoSvc/DataInfoSvc.h"
#include "DataInfoSvc/IDataInfoSvc.h"
//#include "McEventSelector/McIterator.h"
//#include "EventInfo/EventInfo.h" 

using namespace EventModel;
extern const CLID& CLID_Event;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static const SvcFactory<McEventSelector> s_factory;
//const ISvcFactory& McEventSelectorFactory = s_factory;


class McContext : public IEvtSelector::Context {
private:
  const McEventSelector*        m_pSelector;
  std::string                   m_criteria;
  std::vector<int> m_runList;
  std::vector<int> m_evtNoList;

  RealizationSvc* m_RealizationSvc;
  IProperty*        m_appMgrProperty;

  int m_initRunNo;
  unsigned int m_initEventNo;
  unsigned int m_eventsP;

  int m_runNo;
  unsigned int m_eventNo;
  bool m_firstEvent;
  int m_lumiBlockNo;
  

public:
  /// Standard constructor
  McContext(const McEventSelector* pSelector);
  McContext(const McEventSelector* pSelector, 
	    const int &initRunNo, const unsigned int &initEventNo, 
	    const unsigned int &evPR);

  /// Standard destructor 
  virtual ~McContext();

  virtual void* identifier() const {
    return (void*)m_pSelector;
  }
  void setCriteria(const std::string& crit) {
    m_criteria = crit;
  }

  void rewind() {

    // Question: should this rewind to before the first event, or to
    // it? ie, will next() be called right after rewind()?
    // if so, then should set m_firstEvent to true;

      m_eventNo = m_initEventNo;
      m_runNo = m_initRunNo;
  }    
  
  void next() {
    if (m_firstEvent) {
      m_firstEvent = false;

      m_eventNo = m_initEventNo;
      m_runNo = m_initRunNo;

      return;
    }
    m_eventNo++;
    if(m_RealizationSvc->UseDBFlag() == false) return;
    if(m_eventNo>=(m_initEventNo + m_evtNoList[m_lumiBlockNo])){
      m_lumiBlockNo++;
      m_runNo = -std::abs(m_runList[m_lumiBlockNo]);
      m_RealizationSvc->setRunId(m_runNo);
      m_RealizationSvc->setRunEvtNum(m_evtNoList[m_lumiBlockNo]);
      m_eventNo= m_initEventNo;
    }
  }

  void previous() {
    m_eventNo--;
  }

  int runNumber() const { return m_runNo; }
  unsigned int eventNumber() const { return m_eventNo; }

  void setRunNumber( int runNo ) { m_runNo = runNo; }
  void setEventNumber( unsigned int eventNo ) { m_eventNo = eventNo ; }

};

McContext::McContext(const McEventSelector* pSelector):
  m_pSelector(pSelector), m_firstEvent(true), m_lumiBlockNo(0), m_initEventNo(0) {
  // Obtain the IProperty of the ApplicationMgr
  //SmartIF<IProperty> prpMgr(IProperty::interfaceID(), Gaudi::svcLocator());
  SmartIF<IProperty> prpMgr(Gaudi::svcLocator());
    if ( ! prpMgr.isValid() )   {
    std::cout << "IProperty interface not found in ApplicationMgr." << std::endl;
  }
  else {
    m_appMgrProperty = prpMgr;
  }
  //get IRealizationSvc
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  IRealizationSvc *tmpReal;
  StatusCode status = svcLocator->service("RealizationSvc", tmpReal);
  if (!status.isSuccess())
  {
     std::cout << " Could not initialize Realization Service" << std::endl;
  } else {
    m_RealizationSvc=dynamic_cast<RealizationSvc*>(tmpReal);
  }
  //get jobSvc
  IDataInfoSvc* tmpInfoSvc;
  DataInfoSvc* jobInfoSvc;
  status = svcLocator->service("DataInfoSvc",tmpInfoSvc);
  if (status.isSuccess()) {
    std::cout << "get the DataInfoSvc" << std::endl;
    jobInfoSvc=dynamic_cast<DataInfoSvc *>(tmpInfoSvc);
  }else {
    std::cout << "could not get the DataInfoSvc." << std::endl;
  }

  //set initial event ID
  m_initEventNo = m_RealizationSvc->getInitEvtID();

  std::string sMax;
  status = m_appMgrProperty->getProperty("EvtMax", sMax);
  int EvtMax = std::atoi(sMax.c_str());
  //for realization
  if(m_RealizationSvc->UseDBFlag() == true) {
    std::vector<int> totEvtNo;
    totEvtNo.clear();
    std::vector<float> lumi;
    std::vector<int> tmp_runList = m_RealizationSvc->getRunList();
    float totLumi = 0;
    for(unsigned int i = 0; i < tmp_runList.size(); i++)
    {
      float lumi_value = m_RealizationSvc->getLuminosity(tmp_runList[i]);
      lumi.push_back(lumi_value); 
      totLumi += lumi_value;      
    }                             
                                  
    m_runList.clear();

    int totSimEvt = 0;
    int evtSubNo = 0;
    if(totLumi != 0) {
      for(unsigned int i = 0; i < lumi.size(); i++) {
        //if(i == 0) m_evtNoList.push_back(0);
        double ratio = lumi[i]/totLumi*EvtMax;
        evtSubNo = int (ratio);
        if((ratio-evtSubNo) >= 0.5) evtSubNo = evtSubNo + 1;
        totSimEvt += evtSubNo;
        if(evtSubNo == 0) {
          std::cout << "The run " <<tmp_runList[i]<<" is not simulated, due to the luminosity is too small!" << std::endl;
        }
        else {
          //m_evtNoList.push_back(totSimEvt);
          m_evtNoList.push_back(evtSubNo);
          m_runList.push_back(tmp_runList[i]);
          totEvtNo.push_back(tmp_runList[i]);
          totEvtNo.push_back(evtSubNo);
        }
        std::cout <<"Total "<< evtSubNo <<" events need to be simulated in run " <<tmp_runList[i]<< std::endl;
      }

      if((EvtMax - totSimEvt) != 0) {
        unsigned int effRunSize = m_evtNoList.size();
        if(effRunSize > 0) {
          m_evtNoList[effRunSize - 1] = m_evtNoList[effRunSize - 1] + (EvtMax - totSimEvt);
          effRunSize = totEvtNo.size();
          totEvtNo[effRunSize - 1] = totEvtNo[effRunSize - 1] + (EvtMax - totSimEvt);
          std::cout <<"Additional "<< EvtMax - totSimEvt  <<" events need to be simulated in run " << m_runList[m_runList.size() - 1]<< std::endl;
        }
        else {
          bool found = false;
          for(unsigned int i = 0; i < lumi.size(); i++) {
            if(lumi[i] > 0) {
              m_evtNoList.push_back(EvtMax - totSimEvt);
              m_runList.push_back(tmp_runList[i]);
              totEvtNo.push_back(tmp_runList[i]);
              totEvtNo.push_back(EvtMax - totSimEvt);
              found = true;
              std::cout <<"The max event number maybe too small, all "<< evtSubNo <<" events need to be simulated in run " <<tmp_runList[i]<< std::endl;
              break;
            }
          }
          if(!found) {
            std::cerr << "ERORR: " << "Total luminosity is ZERO, please check your run list. " << std::endl;
            std::exit(0);
          }
        }
      }
    }
    else {
      std::cerr << "ERORR: " << "Total luminosity is ZERO!!! Please check your run list. " << std::endl;
      std::exit(0);
    }
    m_RealizationSvc->setRunId(-std::abs(m_runList[0]));
    m_RealizationSvc->setRunEvtNum(m_evtNoList[0]);
    m_initRunNo = -std::abs(m_runList[0]);
    jobInfoSvc->setTotEvtNo(totEvtNo);
  }
  else {
    m_initRunNo = -std::abs((m_RealizationSvc->getRunList())[0]);
  }
}

McContext::McContext(const McEventSelector* pSelector, 
		     const int &initRunNo, 
		     const unsigned int &initEventNo, 
		     const unsigned int &evPR):
  m_pSelector(pSelector),
  m_initRunNo(initRunNo), m_initEventNo(initEventNo), m_eventsP(evPR),
  m_firstEvent(true), m_lumiBlockNo(0) {
}
  

McContext::~McContext() {
}


// IInterface::queryInterface
StatusCode McEventSelector::queryInterface(const InterfaceID& riid, 
					   void** ppvIf) {
  if ( riid == IEvtSelector::interfaceID() )  {
    *ppvIf = (IEvtSelector*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface( riid, ppvIf );
}



McEventSelector::McEventSelector( const std::string& name, ISvcLocator* svcloc ) :
  Service( name, svcloc)
{

  //declareProperty( "RunNumber",           m_runNo = 5040 );
  //declareProperty( "EventsPerRun",        m_eventsPerRun = 100000000 );
  //declareProperty( "FirstEvent",          m_firstEventNo = 0 );
  m_runNo.verifier().setLower( 0 );
  m_eventsPerRun.verifier().setLower( 0 );
  m_firstEventNo.verifier().setLower( 0 );   
 
}

McEventSelector::~McEventSelector() {
  // FIXME: who deletes the Context?
}

StatusCode
McEventSelector::createContext(Context*& refpCtxt) const 
{
  McContext* ctx =  new McContext(this);//,
				  //(unsigned int) m_runNo.value( ),
				  //(unsigned int) m_firstEventNo.value( ),
				  //(unsigned int) m_eventsPerRun.value( )
				  //);
  refpCtxt = ctx;

  return StatusCode::SUCCESS;
}

StatusCode McEventSelector::initialize()     {
  MsgStream log(messageService(), name());
  log << MSG::INFO << " Enter McEventSelector Initialization " << endreq;
  StatusCode sc = Service::initialize();
  if( sc.isSuccess() ) {
  } else {
    log << MSG::ERROR << "Unable to initialize service " << endreq;
    return sc;
  }

  log << MSG::INFO  << " McEventSelector Initialized Properly ... " << endreq;
  return sc;
}

StatusCode McEventSelector::finalize()     {
  MsgStream log(messageService(), name());
  log << MSG::INFO << "finalize" << endreq;

  return StatusCode::SUCCESS;
}

// IEvtSelector::next
StatusCode
McEventSelector::next(Context& ctxt) const {
  MsgStream log(messageService(), name());
  log << MSG::DEBUG << "............. Next Event ............." << endreq;

  McContext* ct = dynamic_cast<McContext*>(&ctxt);

  if (ct != 0 ) {
    ct->next();
    return StatusCode::SUCCESS;
  } else {
    MsgStream log(messageService(), name());
    log << "Could not dcast to McContext" << endreq;
    return StatusCode::FAILURE;
  }
}
StatusCode 
McEventSelector::next(Context& ctxt,int jump) const 
{
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


// IEvtSelector::previous
StatusCode
McEventSelector::previous(Context& ctxt) const {
  McContext* ct = dynamic_cast<McContext*>(&ctxt);

  if (ct != 0 ) {
    ct->previous();
    return StatusCode::SUCCESS;
  } else {
    MsgStream log(messageService(), name());
    log << "Could not dcast to McContext" << endreq;
    return StatusCode::FAILURE;
  }

}

StatusCode 
McEventSelector::previous(Context& ctxt, int jump) const 
{
  if ( jump > 0 ) {
    for ( int i = 0; i < jump; ++i ) {
      StatusCode status = previous(ctxt);
      if ( !status.isSuccess() ) {
        return status;
      }
    }
    return StatusCode::SUCCESS;
  }
  return StatusCode::FAILURE;
}

StatusCode
McEventSelector::last(Context& /*ctxt*/) const {
  MsgStream log(messageService(), name());
  log << MSG::ERROR 
      << "............. Last Event Not Implemented ............." 
      << endreq;
  return StatusCode::FAILURE;
}


StatusCode 
McEventSelector::rewind(Context& ctxt) const 
{

  McContext* ct = dynamic_cast<McContext*>(&ctxt);

  if (ct != 0 ) {
    ct->rewind();
    return StatusCode::SUCCESS;
  } else {
    MsgStream log(messageService(), name());
    log << "Could not dcast to McContext" << endreq;
    return StatusCode::FAILURE;
  }

}


StatusCode
McEventSelector::createAddress(const Context& refCtxt, 
			       IOpaqueAddress*& addr) const {

  const McContext* ctx = dynamic_cast<const McContext*>( &refCtxt );

  if (ctx != 0) {
    McAddress* mcAddr = new McAddress(CLID_Event, "Event", "");
    mcAddr-> setRunEvt(ctx->runNumber(), ctx->eventNumber());
    addr = mcAddr;
  } else {
    MsgStream log(messageService(), name());
    log << MSG::ERROR << "casting to a McContext" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

StatusCode
McEventSelector::releaseContext(Context*& /*refCtxt*/) const {

  MsgStream log(messageService(), name());
  log << MSG::ERROR 
      << "............. releaseContext Not Implemented ............." 
      << endreq;

  return StatusCode::FAILURE;

}

StatusCode
McEventSelector::resetCriteria(const std::string&, Context& ) const {

  MsgStream log(messageService(), name());
  log << MSG::ERROR 
      << "............. resetCriteria Not Implemented ............." 
      << endreq;

  return StatusCode::FAILURE;
  
}

