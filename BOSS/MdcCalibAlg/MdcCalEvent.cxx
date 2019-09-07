#include "MdcCalibAlg/MdcCalEvent.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "BesTimerSvc/IBesTimerSvc.h"
#include "BesTimerSvc/BesTimerSvc.h"

#include "EventModel/Event.h"
#include "RawEvent/RawDataUtil.h"
#include "EventModel/Event.h"

#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"

#include "EvTimeEvent/RecEsTime.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcKalTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"
#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"
#include "MdcRawEvent/MdcDigi.h"

#include <iostream>
#include <math.h>

using namespace Event;

MdcCalEvent::MdcCalEvent(){
}

MdcCalEvent::~MdcCalEvent(){
}

StatusCode MdcCalEvent::setRecEvent(){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalEvent");
     log << MSG::INFO << "MdcCalEvent::setRecEvent" << endreq;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     // retrieve Mdc digi
     SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc,"/Event/Digi/MdcDigiCol");
     if (!mdcDigiCol) {
          log << MSG::FATAL << "Could not find event" << endreq;
     }

     MdcDigiCol::iterator iter = mdcDigiCol->begin();
     m_nhitTQ = 0;
     for(; iter != mdcDigiCol->end(); iter++) {
          MdcDigi *aDigi = (*iter);
	  unsigned fgOverFlow = (aDigi) -> getOverflow();
	  if ( ((fgOverFlow & 3) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;
	  m_nhitTQ++;
     }

     // get EsTimeCol
     SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
     if( (!aevtimeCol) || (aevtimeCol->size()==0) ){
	  m_tes = -9999.0;
	  m_esFlag = -1;
     }else{
	  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
	  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
	       m_tes = (*iter_evt)->getTest();
	       m_esFlag = (*iter_evt)->getStat();
	  }
     }
     m_esCutFg = false;
     m_nEsCutFg = -1;
     bool flagTes = false;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(m_esFlag == m_param.esFlag[iEs]){
	       flagTes = true;
	       m_nEsCutFg = iEs;
	       break;
	  }
     }
     if( flagTes && (m_tes > m_param.tesMin) && (m_tes < m_param.tesMax) ) m_esCutFg = true;

     SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc, "/Event/Recon/RecMdcTrackCol");
     if(!newtrkCol){
	  log << MSG::ERROR << "Could not find RecMdcTrackCol" << endreq;
	  return ( StatusCode::FAILURE );
     }

     int i = 0;
     MdcCalRecTrk* rectrk;
     RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
     for(; it_trk != newtrkCol->end(); it_trk++){
	  rectrk = new MdcCalRecTrk(m_param.particle);
	  rectrk -> setGeomSvc(m_mdcGeomSvc);
	  rectrk -> setUtilSvc(m_mdcUtilitySvc);
	  rectrk -> setRecTrk(it_trk);
	  m_rectrk.push_back(rectrk);

	  i++;
     }
//      log << MSG::WARNING << "RecTrack ntrk = " << i << endreq;

     return StatusCode::SUCCESS;
}

StatusCode MdcCalEvent::setKalEvent(){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcCalEvent");
     log << MSG::INFO << "MdcCalEvent::setKalEvent" << endreq;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     // retrieve Mdc digi
     SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc,"/Event/Digi/MdcDigiCol");
     if (!mdcDigiCol) {
          log << MSG::FATAL << "Could not find event" << endreq;
     }

     MdcDigiCol::iterator iter = mdcDigiCol->begin();
     m_nhitTQ = 0;
     for(; iter != mdcDigiCol->end(); iter++) {
          MdcDigi *aDigi = (*iter);
	  unsigned fgOverFlow = (aDigi) -> getOverflow();
	  if ( ((fgOverFlow & 3) !=0 ) || ((fgOverFlow & 12) != 0) ||
	       (aDigi->getTimeChannel() == 0x7FFFFFFF) ||
	       (aDigi->getChargeChannel() == 0x7FFFFFFF) ) continue;
	  m_nhitTQ++;
     }

     // get EsTimeCol
     SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
     if( (!aevtimeCol) || (aevtimeCol->size()==0) ){
	  m_tes = -9999.0;
	  m_esFlag = -1;
     }else{
	  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
	  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
	       m_tes = (*iter_evt)->getTest();
	       m_esFlag = (*iter_evt)->getStat();
	  }
     }
     m_esCutFg = false;
     m_nEsCutFg = -1;
     bool flagTes = false;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(m_esFlag == m_param.esFlag[iEs]){
	       flagTes = true;
	       m_nEsCutFg = iEs;
	       break;
	  }
     }
     if( flagTes && (m_tes > m_param.tesMin) && (m_tes < m_param.tesMax) ) m_esCutFg = true;

     SmartDataPtr<RecMdcKalTrackCol> kaltrkCol(eventSvc,"/Event/Recon/RecMdcKalTrackCol");
     if (!kaltrkCol) {
	  log << MSG::FATAL << "Could not find RecMdcKalTrackCol" << endreq;
	  return StatusCode::FAILURE;
     }

     int i = 0;
     MdcCalRecTrk* rectrk;
     RecMdcKalTrackCol::iterator iter_trk = kaltrkCol->begin();
     for(; iter_trk != kaltrkCol->end(); iter_trk++) {
	  rectrk = new MdcCalRecTrk(m_param.particle);
	  rectrk -> setGeomSvc(m_mdcGeomSvc);
	  rectrk -> setUtilSvc(m_mdcUtilitySvc);
	  rectrk -> setKalTrk(iter_trk);
	  m_rectrk.push_back(rectrk);

	  i++;
     }
     log << MSG::INFO << "KalTrack ntrk = " << i << endreq;

     return StatusCode::SUCCESS;
}

void MdcCalEvent::clear(){
     unsigned int i;
     for(i=0; i<m_rectrk.size(); i++){
	  delete m_rectrk[i];
     }
     m_rectrk.clear();
}
