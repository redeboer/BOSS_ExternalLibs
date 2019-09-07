#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "HltMakerAlg/HltEventMaker.h"
#include "HltEvent/HltInf.h"
#include "HltEvent/HltRaw.h"
#include "TofRawEvent/TofDigi.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
#include "Identifier/HltID.h"

using namespace Event;

HltEventMaker::HltEventMaker(const std::string& name, ISvcLocator* pSvcLocator) : Algorithm(name, pSvcLocator)
{
  //Declare the properties
}

StatusCode HltEventMaker::initialize(){
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "HltEventMaker: in initialize()" << endreq;
  m_ntot=0;

  return StatusCode::SUCCESS;
}

StatusCode HltEventMaker::execute(){
  MsgStream log(msgSvc(),name());
  log<<MSG::DEBUG<< "HltEventMaker: in execute()" <<endreq;

  // Get the event header, print out event and run number
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return StatusCode::FAILURE;
  }

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "HltEventMaker: retrieved event: " <<"  Event Number  "<< eventId << "  run: " <<runId  << endreq;

  HltInf* aHlt = new HltInf();
  //Get EventFilter information from TDS
  SmartDataPtr<HltRawCol> hltRawCol(eventSvc(),"/Event/Hlt/HltRawCol");
  if (!hltRawCol) {
    log << MSG::DEBUG << "Could not find HltRawCol!" << endreq;
    //return StatusCode::SUCCESS;
  }
  else{
    uint32_t type=0,alg=0,cri=0,ver=0;
    float etot=0;
    uint32_t nmdc=0xFF,ntof=0xFF,nemc=0xFF,nmuc=0xFF;

    HltRawCol::iterator iter = hltRawCol->begin();
    for (;iter != hltRawCol->end(); iter++ ) {
      Identifier id = (*iter)->identify(); 
      log << MSG::VERBOSE << "EventFilter info id: " << hex << id << ": "
	  << (*iter)->getIntChannel() << ","<< dec << (*iter)->getFloatChannel() <<endreq; 
      if(HltID::is_eventtype(id)) aHlt->setEventType(type=(*iter)->getIntChannel());
      if(HltID::is_algorithm(id)) aHlt->setAlgProcess(alg=(*iter)->getIntChannel());
      if(HltID::is_criteria(id)) aHlt->setCriteriaTable(cri=(*iter)->getIntChannel());
      if(HltID::is_version(id)) aHlt->setVersion(ver=(*iter)->getIntChannel());
      if(HltID::is_energy(id)) aHlt->setTotalEnergy(etot=(*iter)->getFloatChannel());
      
      uint32_t pureid= id&0xFF;
      if(HltID::is_number(id)){
	uint32_t datanum=(*iter)->getIntChannel();
	aHlt->setNumber(datanum);
	nmdc=datanum&0xFF;
	ntof=(datanum&0xFF00)>>8;
	nemc=(datanum&0xFF0000)>>16;
	nmuc=(datanum&0xFF000000)>>24;
	log << MSG::VERBOSE << nmdc << ", " << ntof << ", " << nemc << ", " << nmuc << endreq;
      }
      else if(HltID::is_mdc_inf(id)){
	aHlt->push_mdc((*iter)->getIntChannel());
      }
      else if(HltID::is_tof_inf(id)){
	aHlt->push_tof((*iter)->getIntChannel());      
      }
      else if(HltID::is_emc_inf(id)){
	aHlt->push_emc((*iter)->getIntChannel());
      }
      else if(HltID::is_muc_inf(id)){
	aHlt->push_muc((*iter)->getIntChannel());
      }
      else if(HltID::is_con_inf(id)){
	if(HltID::is_ncon(id)) aHlt->setNCON((*iter)->getIntChannel());
	else                   aHlt->push_con((*iter)->getIntChannel());
      }
    }
    log<<MSG::DEBUG<<"type="<<type<<", alg="<<alg<<", criteria="<<cri
       <<", version="<<ver<<", energy="<<etot<<endreq;
    
    m_ntot++;
  }
  //Register related event filter information to TDS for physics analysis   
  StatusCode sc;
  DataObject *aHltInfEvent;
  eventSvc()->findObject("/Event/Hlt/HltInf", aHltInfEvent);
  if(aHltInfEvent!=NULL) {
    SmartDataPtr<HltInf> hltInf(eventSvc(),"/Event/Hlt/HltInf");
    log<<MSG::DEBUG<<"old type="<<hltInf->getEventType()<<", alg="<<hltInf->getAlgProcess()
       <<", criteria="<<hltInf->getCriteriaTable()<<", version="<<hltInf->getVersion()
       <<", energy="<<hltInf->getTotalEnergy()<<endreq;
    //then unregister HltInf
    sc = eventSvc()->unregisterObject("/Event/Hlt/HltInf");
    delete aHltInfEvent;   //it must be delete to avoid memory leakage
    log << MSG::INFO << "Old HltInf has been unregistered" << endreq;
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister HltInf" << endreq;
      return( StatusCode::FAILURE);
    }
  }
  sc = eventSvc()->registerObject(EventModel::Hlt::HltInf,aHlt);
  if(sc!=StatusCode::SUCCESS) {
    log<<MSG::FATAL<< "Could not register HltInf" <<endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode HltEventMaker::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize HltEventMaker" << endreq;
  log << MSG::ALWAYS << m_ntot << " events are converted." <<endreq;
  return StatusCode::SUCCESS;
}
