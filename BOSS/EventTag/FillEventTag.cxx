#include "EventTag/FillEventTag.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h" 
#include "McTruth/McParticle.h"

FillEventTag::FillEventTag(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  declareProperty("GeneralEventType", initialEventTag=0);
}

StatusCode FillEventTag::initialize()
{
  MsgStream log(msgSvc(), name());
  static const bool CREATEIFNOTTHERE(true);
  StatusCode EventTagSvcStatus = service("EventTagSvc", m_EventTagSvc, CREATEIFNOTTHERE);
  if (!EventTagSvcStatus.isSuccess()) {
    log << MSG::ERROR << " Could not initialize Decay code service" << endreq;
    return EventTagSvcStatus;
  }
  return StatusCode::SUCCESS;
}
StatusCode FillEventTag::execute()
{
  MsgStream log(msgSvc(), name());
  // Get McParticle Collections
  m_EventTag =initialEventTag; 


  if((m_EventTag&0xF)==1)return StatusCode::SUCCESS; //Real data
  
  SmartDataPtr<Event::McParticleCol> mcParticles(eventSvc(),EventModel::MC::McParticleCol);
  if(!mcParticles){
    log<<MSG::ERROR<<"Can not open McParticleCollection" <<endreq;
    return StatusCode::SUCCESS;
  }

  if(m_EventTag==0){//try autodetect general event type: check for ccbar resonance;
    for(Event::McParticleCol::iterator it=mcParticles->begin();it!=mcParticles->end();it++){
      //      if((*it)->itdecayInFlight())!!((*it)->primaryParticle())continue;
      long int pdg=abs((*it)->particleProperty());
      if(((pdg%1000)/10) == 44){
	m_EventTag=((int)(pdg==443))*4+((int)(pdg==100443))*5+((int)(pdg==30443))*6
	  +((int)(pdg==9000443))*7+((int)(pdg==9010443))*8+((int)(pdg==9020443))*9;
	break;
      }
    }
  }

  if(m_EventTag==0){//try autodetect general event type;
    m_EventTag=2; //off-resonance data
    for(Event::McParticleCol::iterator it=mcParticles->begin();it!=mcParticles->end();it++){
      if((*it)->decayInFlight() || (*it)->primaryParticle()) continue;
      long int pdg=abs((*it)->particleProperty());
      if(pdg==11){
	m_EventTag|=0X20;
	break;
      }
      else if(pdg==13){
	m_EventTag|=0X30;
	break;
      }
      else if(pdg==15){
	m_EventTag|=0X40;
	break;
      }
      else if((pdg>0)&&(pdg<9)){
	m_EventTag|=0X50;
	break;
      }
    }
  }


  if(((m_EventTag&0xf)>3)&&((m_EventTag&0xf)<9)){ /// charmonium resonance
    m_EventTag &= 0xf;
    // find initial charmonium resonance;
    for(Event::McParticleCol::iterator it=mcParticles->begin();it!=mcParticles->end();it++){
      long int pdg=(*it)->particleProperty();
      if(((pdg%1000)/10) != 44)continue;
      //      part=(*it);
      m_EventTag |= (m_EventTagSvc->getCharmDecayType(*it))<<4;
      m_EventTag |= (m_EventTagSvc->getChainCode(*it))<<8;
      //      printf("Chain code %20llX \n",(m_EventTagSvc->getChainCode(*it)));
      break;
    }
  }
  else if((m_EventTag&0xf)==0x2){ //off-resonance
    //save topology info:
    int ncha=0;
    int nneu=0;
    for(Event::McParticleCol::iterator it=mcParticles->begin();it!=mcParticles->end();it++){
      long int pdg=abs((*it)->particleProperty());
      bool good=(!(*it)->decayInFlight())&&(!(*it)->primaryParticle());
      ncha+=((int)(good&&((pdg==11)||(pdg==13)||(pdg==211)||(pdg==321)||(pdg==2212)||(pdg==1011))));
      nneu+=((int)(good&&((pdg==111)||(pdg==310)||(pdg==130)||(pdg==2112))));
//       printf("%2i %6i  %2i %2i  Nneu:%3i    Ncha:%3i \n",
// 	     (*it)->trackIndex(),pdg,
// 	     ((pdg==11)||(pdg==13)||(pdg==211)||(pdg==321)||(pdg==2212)||(pdg==1011)),
// 	     ((pdg==111)||(pdg==310)||(pdg==130)||(pdg==2112)),
// 	     nneu,ncha);  
    }
    m_EventTag|=((ncha&0xF)<<8);
    m_EventTag|=((nneu&0xF)<<12);
    if((m_EventTag&0xf0)==0x40){//tau-tau event
      //cout<< "XXXXXXXXX" <<endl;
      for(Event::McParticleCol::iterator it=mcParticles->begin();it!=mcParticles->end();it++){
	long int pdg=(*it)->particleProperty();
	//	cout<<"    YYY: "<<pdg<<endl;
	if(pdg==-15){
	  m_EventTag |=(m_EventTagSvc->getDecayCode(*it))<<16;
	  // cout <<"DEcay cod for a-tau:" <<(m_EventTagSvc->getDecayCode(*it))<<endl;
	}else if(pdg==15){
	  m_EventTag |=(m_EventTagSvc->getDecayCode(*it))<<24;	  
	  //cout <<"DEcay cod for tau:" <<(m_EventTagSvc->getDecayCode(*it))<<endl;
	}
      }
    }
  }
  

  SmartDataPtr<Event::EventHeader> evHead(eventSvc(),EventModel::EventHeader);
  if(!evHead){
    log<<MSG::ERROR<< "  Can not to retreave  EventHeader" << endreq;
    return StatusCode::SUCCESS;
  }
  evHead->setEventTag(m_EventTag);

  return StatusCode::SUCCESS;
}

StatusCode FillEventTag::finalize(){
  MsgStream log(msgSvc(), name());
  return StatusCode::SUCCESS;
}
