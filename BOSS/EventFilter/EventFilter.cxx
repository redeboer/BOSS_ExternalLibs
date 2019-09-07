#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h" 
#include "Identifier/HltID.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/DstHltInf.h"
#include "TrigEvent/TrigData.h"
#include "EventFilter/EventFilter.h"

long unsigned EventFilter::m_nTotalEvent =0;
long unsigned EventFilter::m_npass =0;
bool          EventFilter::m_efResult = false;
std::string   EventFilter::m_type ;

EventFilter::EventFilter(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

  declareProperty("useTrigger",     m_trgFlag =true);
  declareProperty("OutputLevel",    m_outputLevel = MSG::NIL);
  declareProperty("OpenedChannels", m_chn );
  declareProperty("Example",        m_example = false);

}

EventFilter::~EventFilter(){
}

StatusCode EventFilter::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  m_nTotalEvent =0;
  m_npass =0;
  m_efResult = false;
  m_type  = "RefuseByEF";

  return StatusCode::SUCCESS;
}


StatusCode EventFilter::execute() {
  
  MsgStream log(msgSvc(), name());

  uint32_t run=0,event=0;
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  run=eventHeader->runNumber();
  event=eventHeader->eventNumber();
  log << MSG::INFO << "execute()  RunNo=" << run << ",  EventNo=" << event << endreq;

  m_nTotalEvent++;

  m_efResult = false;
  setFilterPassed(false);
  m_type  = "RefuseByEF";

  if(m_trgFlag){
    SmartDataPtr<TrigData> trg(eventSvc(),"/Event/Trig/TrigData");
    if(!trg) {
      log << MSG::WARNING << "Could not find Event TrigData" << endreq;
    }
    else {
      bool pass=false;
      for(int i=0;i<16;i++){
	if(trg->getTrigChannel(i))  pass=true;
      }
      if(!pass) {
	m_efResult = false;
	m_type = "RefuseByTrg";
	return( StatusCode::SUCCESS);
      }
    }
  }

  DstHltInf* aHlt;
  SmartDataPtr<HltInf> hlt(eventSvc(),"/Event/Hlt/HltInf");
  if(!hlt) {
    log << MSG::WARNING << "Could not find Event HltInf (try DstHltInf now)" << endreq;
    SmartDataPtr<DstHltInf> hltDst(eventSvc(),"/Event/Hlt/DstHltInf");
    if(!hltDst){
      log << MSG::FATAL << "Could not find Event DstHltInf too ( please re-generate data)" << endreq;
      return( StatusCode::FAILURE);
      //return( StatusCode::SUCCESS);
    }
    else{
      aHlt=hltDst;
    }
  }
  else aHlt=hlt;

  std::vector<std::string>::iterator it=m_chn.begin();
  m_type = aHlt->getEventName();
  if(aHlt->getEventType()==0) {
    log << MSG::WARNING << m_type << endreq;
  }
  if(log.level()<=MSG::VERBOSE) aHlt->print();

  for(;it!=m_chn.end();it++){
    if(aHlt->isType(*it)) {
      m_efResult=true;
      setFilterPassed(true);
      m_npass++;
      log << MSG::INFO << "this event is passed" << endreq;
      break;
    }
  }  
  log << MSG::INFO << "this event is a " << m_type <<endreq;

  if(m_example&&!hlt) {
    cout<< "****** Example for getting event filter informatiom ******" << endl;
    cout<< "****** Please refer to the code in EventFilter.cxx  ******" << endl;
    print(hlt);
  }

  return ( StatusCode::SUCCESS);
}

void EventFilter::print(HltInf* ahlt){
  int nmdc,ntof,nemc,nmuc,ntrk,nshower;
  float etot,e1,e2,cose1,cose2,p1,p2,acop,acole,acol;
  if(ahlt->getData(Identifier(HltID::NMDC),(uint32_t*)&nmdc)) cout<< "nmdc=" << nmdc <<endl;
  if(ahlt->getData(Identifier(HltID::NTOF),(uint32_t*)&ntof)) cout<< "ntof=" << ntof <<endl;
  if(ahlt->getData(Identifier(HltID::NEMC),(uint32_t*)&nemc)) cout<< "nemc=" << nemc <<endl;
  if(ahlt->getData(Identifier(HltID::NMUC),(uint32_t*)&nmuc)) cout<< "nmuc=" << nmuc <<endl;
  if(ahlt->getData(Identifier(HltID::NTRK),(uint32_t*)&ntrk)) cout<< "ntrk=" << ntrk <<endl;
  if(ahlt->getData(Identifier(HltID::NSHW),(uint32_t*)&nshower)) cout<< "nshower=" << nshower <<endl;
  if(ahlt->getData(Identifier(HltID::ETOT),(uint32_t*)&etot)) cout<< "etot=" << etot <<endl;
  if(ahlt->getData(Identifier(HltID::EMAX1),(uint32_t*)&e1)) cout<< "e1=" << e1 <<endl;
  if(ahlt->getData(Identifier(HltID::EMAX2),(uint32_t*)&e2)) cout<< "e2=" << e2 <<endl;
  if(ahlt->getData(Identifier(HltID::ECOST1),(uint32_t*)&cose1)) cout<< "cose1=" << cose1 <<endl;
  if(ahlt->getData(Identifier(HltID::ECOST2),(uint32_t*)&cose2)) cout<< "cose2=" << cose2 <<endl;
  if(ahlt->getData(Identifier(HltID::PMAX1),(uint32_t*)&p1)) cout<< "p1=" << p1 <<endl;
  if(ahlt->getData(Identifier(HltID::PMAX2),(uint32_t*)&p2)) cout<< "p2=" << p2 <<endl;
  if(ahlt->getData(Identifier(HltID::EACOP),(uint32_t*)&acop)) cout<< "acop=" << acop <<endl;
  if(ahlt->getData(Identifier(HltID::EACOL),(uint32_t*)&acole)) cout<< "acole=" << acole <<endl;
  if(ahlt->getData(Identifier(HltID::ACOL),(uint32_t*)&acol)) cout<< "acol=" << acol <<endl;
  return;
}
