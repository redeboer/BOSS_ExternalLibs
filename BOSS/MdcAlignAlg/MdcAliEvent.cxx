#include "MdcAlignAlg/MdcAliEvent.h"

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

#include <iostream>
#include <math.h>

using namespace Event;

StatusCode MdcAliEvent::setRecEvent(){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAliEvent");
     log << MSG::INFO << "MdcAliEvent::setRecEvent" << endreq;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     // get EsTimeCol
     SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
     if( ! aevtimeCol || (aevtimeCol->size()==0) ){
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
     bool flagTes = false;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(m_esFlag == m_param.esFlag[iEs]){
	       flagTes = true;
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
     MdcAliRecTrk* rectrk;
     RecMdcTrackCol::iterator it_trk = newtrkCol->begin();
     for(; it_trk != newtrkCol->end(); it_trk++){
	  rectrk = new MdcAliRecTrk();
	  rectrk -> setRecTrk(it_trk);
	  m_rectrk.push_back(rectrk);

	  i++;
     }
     log << MSG::WARNING << "RecTrack ntrk = " << i << endreq;

     return StatusCode::SUCCESS;
}

StatusCode MdcAliEvent::setKalEvent(){
     IMessageSvc *msgSvc;
     Gaudi::svcLocator()->service("MessageSvc", msgSvc);
     MsgStream log(msgSvc, "MdcAliEvent");
     log << MSG::INFO << "MdcAliEvent::setKalEvent" << endreq;

     IDataProviderSvc* eventSvc = NULL;
     Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

     // get EsTimeCol
     SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc,"/Event/Recon/RecEsTimeCol");
     if( ! aevtimeCol || (aevtimeCol->size()==0) ){
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
     bool flagTes = false;
     for(int iEs=0; iEs<m_param.nEsFlag; iEs++){
	  if(m_esFlag == m_param.esFlag[iEs]){
	       flagTes = true;
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
     MdcAliRecTrk* rectrk;
     RecMdcKalTrackCol::iterator iter_trk = kaltrkCol->begin();
     for(; iter_trk != kaltrkCol->end(); iter_trk++) {
	  rectrk = new MdcAliRecTrk();
	  rectrk -> setKalTrk(iter_trk);
	  m_rectrk.push_back(rectrk);

	  i++;
     }
     log << MSG::INFO << "KalTrack ntrk = " << i << endreq;

     return StatusCode::SUCCESS;
}

void MdcAliEvent::clear(){
     unsigned int i;
     for(i=0; i<m_rectrk.size(); i++){
	  delete m_rectrk[i];
     }
     m_rectrk.clear();
}
