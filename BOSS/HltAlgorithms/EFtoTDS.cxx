#include "HltAlgorithms/EFtoTDS.h"
#include "HltDataTypes/EFResult.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/Bootstrap.h"
//#include "GaudiKernel/IJobOptionsSvc.h"
//#include "GaudiKernel/ISvcLocator.h"

#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "HltEvent/HltEvent.h"
#include "HltEvent/HltRaw.h"
#include "Identifier/Identifier.h"
#include "Identifier/HltID.h"

using namespace std;
using namespace Event;

////////////////////////////////////////////////////////////////////////

EFtoTDS::EFtoTDS(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {
  declareProperty("OutputLevel", m_outputLevel);
}

EFtoTDS::~EFtoTDS()
{
}

StatusCode EFtoTDS::initialize(){
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
  
  StatusCode sc = service( "HltStoreSvc", m_HltStoreSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << name() << ": Unable to locate Service HltStoreSvc" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

StatusCode EFtoTDS::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return StatusCode::FAILURE;
  }

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "retrieved event: " <<"Event Number "<<eventId<<" run: "<<runId<<endreq;
  
  HltRawCol* aEF=new HltRawCol;

  EFResult* efResult;
  string answer="";
  StatusCode sc = m_HltStoreSvc->get("EventType", efResult);
  if ( sc.isFailure() ) {
    log << MSG::ERROR << "Could not find EFResult (EventType)" << endreq;
    answer="Error";
  }
  else {
    if (efResult->getAnswerIndex() == -1) {
      log << MSG::DEBUG << "Unvalid EFResult(AnswerIndex=-1)" << endreq;
      answer="Error";
    }
    answer = efResult->getAnswer();
  }
  log << MSG::INFO << "The answer is " << answer << endreq;

  uint32_t type=0;
  if(answer=="Junk") type=1<<0;
  else if(answer=="GJunk") type=(1<<1);
  else if(answer=="Beamgas") type=1<<2;
  else if(answer=="GBeamgas") type=(1<<3);
  else if(answer=="Cosmic") type=1<<4;
  else if(answer=="GCosmic") type=(1<<5);
  else if(answer=="EBhabha") type=1<<6;
  else if(answer=="GEBhabha") type=(1<<7);
  else if(answer=="BBhabha") type=1<<8;
  else if(answer=="GBBhabha") type=(1<<9);
  else if(answer=="Dimuon") type=1<<10;
  else if(answer=="GDimuon") type=(1<<11);
  else if(answer=="Diphoton") type=1<<12;
  else if(answer=="GDiphoton") type=(1<<13);
  else if(answer=="Hadron") type=1<<14;
  else if(answer=="GHadron") type=(1<<15);
  else if(answer=="Twophoton") type=1<<16;
  else if(answer=="GTwophoton") type=(1<<17);
  else if(answer=="RandomTrg") type=1<<31;
  else if(answer=="Error")     type=1<<30;
  else if(answer=="Other")     type=1<<29;
  else      type=1<<28;

  uint32_t id=0x50000000;
  HltRaw* efType=new HltRaw(id);
  efType->setIntChannel(type);
  aEF->push_back(efType);

  sc  = m_HltStoreSvc->get("EFResult", efResult) ;
  if ( sc.isFailure() ) {
    log << MSG::FATAL << "Could not find EFResult (EFResult)" << endreq;
    return StatusCode::FAILURE;
  }
  else {
    uint32_t alg= efResult->getEFVec(0);
    id++;
    HltRaw* efAlg=new HltRaw(id);
    uint32_t alg8=alg&0xFF;
    efAlg->setIntChannel(alg8);
    aEF->push_back(efAlg);
    id++;
    uint32_t cri=alg>>8;
    HltRaw* efCri=new HltRaw(id);
    efCri->setIntChannel(cri);
    aEF->push_back(efCri);
    id++;
    HltRaw* efVer=new HltRaw(id);
    efVer->setIntChannel(0);
    aEF->push_back(efVer);
    uint32_t etot= efResult->getEFVec(31);
    id++;
    HltRaw* efEtot=new HltRaw(id);
    efEtot->setFloatChannel(*((float*)&etot));
    aEF->push_back(efEtot);

    uint32_t num=efResult->getEFVec(1);
    int nmdc=num&0xFF;
    int ntof=(num>>8)&0xFF;
    int nemc=(num>>16)&0xFF;
    int nmuc=(num>>24)&0xFF;
    int ndata=1;

    uint32_t ndatap = nmdc+ntof+nemc+nmuc;
    ndata += ndatap;
    int ndec=efResult->getDecNum();
    if(ndec>0) ndata +=(ndec+1);
 
    log << MSG::DEBUG << "ndata=" << ndata << "; nmdc=" << nmdc << ", ntof=" << ntof
	<< ", nemc=" << nemc << ", nmuc=" << nmuc << ", ndec=" << ndec << endreq;

    if((alg&4)&&(!(alg&2))) log << MSG::WARNING<<"sub-algorithms error!"<<endreq;
    
    uint32_t* var=new uint32_t[ndata];
    if(ndata==1){
      var[0]=0;
    }
    else{
      var[0]=num;
      int i=1;
      for(int j=5;j<5+nmdc;j++){
        var[i]=efResult->getEFVec(j);
        ++i;
      }
      for(int j=25;j<25+ntof;j++){
        var[i]=efResult->getEFVec(j);
        ++i;
      }
      for(int j=30;j<30+nemc;j++){
        var[i]=efResult->getEFVec(j);
        ++i;
      }
      for(int j=52;j<52+nmuc;j++){
        var[i]=efResult->getEFVec(j);
        ++i;
      }
      if(ndec>0) {
        var[i]=ndec;
        ++i;
        for(int j=54;j<54+ndec;++j){
          var[i]=efResult->getEFVec(j);
          ++i;
        }
      }
      if(i!=ndata) log << MSG::FATAL << "number of datat error!" << endreq;
    }

    id++;
    HltRaw* efTag=new HltRaw(id);
    efTag->setIntChannel(var[0]);
    aEF->push_back(efTag);
    
    for(int i=1;i<ndata;i++){
      id++;
      HltRaw* efVar=new HltRaw(HltID::convert(id,nmdc,ntof,nemc,nmuc));
      efVar->setFloatChannel(*((float*)&var[i]));
      efVar->setIntChannel(var[i]);
      aEF->push_back(efVar);
    }
    delete var;
  }

  uint32_t atype=0,alg=0,cri=0,ver=0;
  float etot=0.;

  DataObject *aHltEvent;
  eventSvc()->findObject("/Event/Hlt", aHltEvent);
  if(aHltEvent==NULL){
    log<<MSG::INFO<< "HltEvent has not existed, so register it now!" <<endreq;
    HltEvent* aHlt=new HltEvent();
    sc = eventSvc()->registerObject(EventModel::Hlt::Event,aHlt);
    if(sc!=StatusCode::SUCCESS) {
      log<<MSG::FATAL<< "Could not register Hlt!!!" <<endreq;
      return( StatusCode::FAILURE);
    }
  }
  else{
    log<<MSG::WARNING<< "HltEvent has existed!" <<endreq;
  }

  DataObject *aHltRawEvent;
  eventSvc()->findObject("/Event/Hlt/HltRawCol", aHltRawEvent);
  if(aHltRawEvent!=NULL) {
    SmartDataPtr<HltRawCol> hltRawCol(eventSvc(),"/Event/Hlt/HltRawCol");
    HltRawCol::iterator iter = hltRawCol->begin();
    for (;iter != hltRawCol->end(); iter++ ) {
      Identifier id = (*iter)->identify();
      if(HltID::is_eventtype(id)) atype=(*iter)->getIntChannel();
      if(HltID::is_algorithm(id)) alg=(*iter)->getIntChannel();
      if(HltID::is_criteria(id)) cri=(*iter)->getIntChannel();
      if(HltID::is_version(id)) ver=(*iter)->getIntChannel();
      if(HltID::is_energy(id)) etot=(*iter)->getFloatChannel();
    }
    log<<MSG::DEBUG<<"old type="<<atype<<", alg="<<alg<<", criteria="<<cri
       <<", version="<<ver<<", energy="<<etot<<endreq;
    //then unregister HltRawCol
    sc = eventSvc()->unregisterObject("/Event/Hlt/HltRawCol");
    delete aHltRawEvent;   //it must be delete to avoid memory leakage
    log << MSG::INFO << "Old raw collection has been unregistered" << endreq;
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister HLT raw collection" << endreq;
      return( StatusCode::FAILURE);
    }
  }

  // register
  sc = eventSvc()->registerObject(EventModel::Hlt::HltRawCol,aEF);
  if(sc!=StatusCode::SUCCESS) {
    log<<MSG::FATAL<< "Still could not register HltRawCol" <<endreq;
    return StatusCode::FAILURE;
  }
  
  SmartDataPtr<HltRawCol> hltRawCol(eventSvc(),"/Event/Hlt/HltRawCol");
  if (!hltRawCol) {
    log << MSG::FATAL << "Could not find HltRawCol!" << endreq;
    return StatusCode::FAILURE;
  }

  HltRawCol::iterator iter = hltRawCol->begin();
  for (;iter != hltRawCol->end(); iter++ ) {
    Identifier id = (*iter)->identify();
    if(HltID::is_eventtype(id)) atype=(*iter)->getIntChannel();
    if(HltID::is_algorithm(id)) alg=(*iter)->getIntChannel();
    if(HltID::is_criteria(id)) cri=(*iter)->getIntChannel();
    if(HltID::is_version(id)) ver=(*iter)->getIntChannel();
    if(HltID::is_energy(id)) etot=(*iter)->getFloatChannel();
  }
  log<<MSG::DEBUG<<"    type="<<atype<<", alg="<<alg<<", criteria="<<cri
     <<", version="<<ver<<", energy="<<etot<<endreq;
  
  return StatusCode::SUCCESS;
}

StatusCode EFtoTDS::finalize() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

