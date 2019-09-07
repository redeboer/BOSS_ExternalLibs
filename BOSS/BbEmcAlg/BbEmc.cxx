#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/EventHeader.h"
#include "EventModel/Event.h"
#include "TrigEvent/TrigEvent.h"
#include "TrigEvent/TrigData.h"
#include "McTruth/McParticle.h"
#include "EvTimeEvent/RecEsTime.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "DstEvent/TofHitStatus.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;
#include "CLHEP/Vector/LorentzVector.h"
using CLHEP::HepLorentzVector;
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep2Vector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
#include "VertexFit/SecondVertexFit.h" 
#include "ParticleID/ParticleID.h"
#include "HltEvent/DstHltInf.h"
#include "HltEvent/HltInf.h"

#include "BbEmcAlg/BbEmc.h"
#include <vector>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
const double velc = 299.792458;
const double xmass[5] = {0.000511, 0.105658, 0.139570, 0.493677, 0.938272};
const double pai=3.1415926;

static long m_cout_all(0), m_cout_tracks(0), m_cout_good(0), m_cout_dang(0);
////////////////////////////////////////////////////////////////////
BbEmc::BbEmc(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {  
    //Declare the properties 
    //control flag
    declareProperty("hist",    m_hist=false);
    declareProperty("Trigger", m_trigger_flag=true);
    declareProperty("Hlt",     m_hlt_flag=true);
    declareProperty("Estime",  m_est_flag=true);
    //RecMdcKalTrack or RecMdcTrack
    declareProperty("KalTrk",  m_kalTrk_flag=true);
    //Good shower selection cut
    declareProperty("EneCut",  m_energy_cut=1.2);
    declareProperty("MomCut",  m_mom_cut=0.04);
    declareProperty("DangCut", m_dangCut= 2.5);
    //Mdc Track vertex cut
    declareProperty("Vr0cut",  m_vr0cut=1.0);
    declareProperty("Vz0cut",  m_vz0cut=5.0);
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode BbEmc::initialize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endmsg;

  StatusCode status;

  if(m_hist){
    NTuplePtr nt1(ntupleSvc(), "FILEBbEmc/bbEmc");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILEBbEmc/bbEmc", CLID_ColumnWiseTuple, "BbEmc N-Tuple example");
      if ( m_tuple1 ){
	status = m_tuple1->addItem ("run",            m_run);
	status = m_tuple1->addItem ("event",          m_event);
	status = m_tuple1->addItem ("numCtrk",        m_num_Ctrk);
	status = m_tuple1->addItem ("numNtrk",        m_num_Ntrk);
	status = m_tuple1->addItem ("numGtrk",        m_num_Gtrk);
	//trigger and hlt
	status = m_tuple1->addItem ("trigindex",      m_trig_index, 0, 48);
	status = m_tuple1->addIndexedItem("trigcond", m_trig_index, m_trig_cond);
	status = m_tuple1->addIndexedItem("trigchan", m_trig_index, m_trig_chan);
	status = m_tuple1->addItem ("timewindow",     m_trig_timewindow);
	status = m_tuple1->addItem ("timetype",       m_trig_timetype);
	status = m_tuple1->addItem ("hlttype",        m_hlt_type );
	//estime
	status = m_tuple1->addItem ("estime",         m_est_start);
	status = m_tuple1->addItem ("status",         m_est_status);
	status = m_tuple1->addItem ("quality",        m_est_quality);
	//Emc shower 
	status = m_tuple1->addItem ("dang",           m_dang);
	status = m_tuple1->addItem ("index",          m_index,0,2);
	status = m_tuple1->addIndexedItem ("theta",   m_index, m_theta);
	status = m_tuple1->addIndexedItem ("phi",     m_index, m_phi);
	status = m_tuple1->addIndexedItem ("ene",     m_index, m_ene);
      } else { 
	log << MSG::ERROR << "Cannot book N-tuple theone"<<endmsg;
	return StatusCode::FAILURE;
      }
    }

    log << MSG::INFO << "end initialize()" << endmsg;
    // finish book
  }//end of m_hist

  return StatusCode::SUCCESS;

}

//*************************************************
StatusCode BbEmc::execute(){

  MsgStream log(msgSvc(),name());
  log<<MSG::INFO<<"in execute()"<<endreq;
  m_cout_all++;

  // save the events passed selection to a new file
  setFilterPassed(false);

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int runNum= eventHeader->runNumber();
  int eventNum= eventHeader->eventNumber();
  if(m_cout_all %1000 ==0) {
    std::cout<<name()<<"::"<< m_cout_all <<" events executed"<<std::endl;
  }

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
    << evtRecEvent->totalCharged() << " , "
    << evtRecEvent->totalNeutral() << " , "
    << evtRecEvent->totalTracks() <<endreq;

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),"/Event/EvtRec/EvtRecTrackCol");
  if((evtRecEvent->totalTracks()<2) || (evtRecEvent->totalTracks()>30)){
    return StatusCode::SUCCESS;
  }
  m_cout_tracks ++;

  // High level trigger
  int hlt_type=0;
  if(m_hlt_flag){
    SmartDataPtr<HltInf> hlt(eventSvc(),"/Event/Hlt/HltInf");
    DstHltInf *aHlt;
    if(!hlt) {
      log << MSG::WARNING << "Could not find Event HltInf, try DstHltInf now" << endreq;
      SmartDataPtr<DstHltInf> hltDst(eventSvc(),"/Event/Hlt/DstHltInf");
      if(!hltDst){
	log << MSG::ERROR << "Could not find Event DstHltInf too, please re-generate data" << endreq;
	return StatusCode::FAILURE;
      } else {
	aHlt=hltDst;
      }
    } else {
      aHlt=hlt;
    }
    uint32_t temp_type(aHlt->getEventType());
    int mask(1);
    for(int i=0; i<32; i++){
      if(temp_type & mask){
	hlt_type = i;
	break;
      }
      temp_type >>= 1;
    }
  }
  
  //Trigger 
  SmartDataPtr<TrigData> trigData(eventSvc(),"/Event/Trig/TrigData");
  if(m_trigger_flag & runNum>0){
    if (!trigData) {
      log << MSG::FATAL << "Could not find TrigData in TDS" << endreq;
      return StatusCode::FAILURE;
    }
    // Print trigger information once:
    log << MSG::DEBUG << "Trigger conditions: " << endreq;
    for(int trig_id=0; trig_id < 48; trig_id++){
      log << MSG::DEBUG << "i:" << trig_id 
	<< "  name:" << trigData->getTrigCondName(trig_id)
	<< "  cond:" << trigData->getTrigCondition(trig_id) << endreq;
    }
  }

  // Event start time
  SmartDataPtr<RecEsTimeCol> recEstimeCol(eventSvc(), "/Event/Recon/RecEsTimeCol");
  if(m_est_flag & runNum>0){
    if(!recEstimeCol){
      log << MSG::WARNING << "Can not get RecEsTimeCol" << endreq;
      return StatusCode::SUCCESS;
    }
    log << MSG::DEBUG <<"size of EsTime: " << recEstimeCol->size() << endreq;
  }

  // good shower selection
  Vp4 vGood;
  HepLorentzVector m_lv_ele;
  for(int i=0; i<evtRecEvent->totalTracks(); i++){
    if(i>=evtRecTrkCol->size()) break;

    EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    if(emcTrk->energy()<m_energy_cut) continue;

    Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z()); 
    m_lv_ele.setVect(emcpos);
    m_lv_ele.setE(emcTrk->energy());
    vGood.push_back(m_lv_ele);
  }

  // num of good showers = 2
  if(vGood.size() - 2) return SUCCESS; 
  m_cout_good ++;

  // angle between two showers 
  double dang = vGood[0].vect().angle(vGood[1].vect());
  if(dang < m_dangCut) return SUCCESS; 
  m_cout_dang ++;

  if(vGood[0].e() > vGood[1].e()) swap(vGood[0],vGood[1]);

  // check x0, y0, z0, r0
  // suggest cut: |z0|<5 && r0<1 (cm)
  double d0,z0,cosTheta,phi,mom;
  Vint iGood; iGood.clear();
  for(int i = 0; i < evtRecEvent->totalCharged(); i++){
    double m_vz,m_vr;
    EvtRecTrackIterator itTrk;
    if (m_kalTrk_flag){
      itTrk=evtRecTrkCol->begin() + i;
      if(!(*itTrk)->isMdcKalTrackValid()) continue;
      RecMdcKalTrack *mdcTrk = (*itTrk)->mdcKalTrack();
      m_vz = mdcTrk->z();
      m_vr = mdcTrk->r();
    }else{
      if(!(*itTrk)->isMdcTrackValid()) continue;
      RecMdcTrack *mdcTrk = (*itTrk)->mdcTrack();
      m_vz = mdcTrk->z();
      m_vr = mdcTrk->r();
    }
    if(fabs(m_vz) >= m_vz0cut) continue;
    if(m_vr >= m_vr0cut) continue;
    iGood.push_back(i);
  }

  if (m_hist){
    m_run = runNum;
    m_event = eventNum;

    m_num_Ctrk = evtRecEvent->totalCharged();
    m_num_Ntrk = evtRecEvent->totalNeutral();
    m_num_Gtrk = iGood.size();
    if (trigData){
      for(int trig_id=0; trig_id < 48; trig_id++){
	m_trig_index = trig_id;
	if(m_trig_index<16){
	  m_trig_chan[m_trig_index] = trigData->getTrigChannel(m_trig_index);
	}
	m_trig_cond[m_trig_index] = trigData->getTrigCondition(m_trig_index);
      }
      m_trig_timewindow = trigData->getTimeWindow();
      m_trig_timetype = trigData->getTimingType();
    }

    m_hlt_type = hlt_type;

    if (recEstimeCol){
      m_est_start = (*(recEstimeCol->begin()))->getTest();
      m_est_status = (*(recEstimeCol->begin()))->getStat();
      m_est_quality = (*(recEstimeCol->begin()))->getQuality();
    }

    m_dang = dang;
    for(int i=0; i<2; i++){
      m_index = i;
      m_theta[m_index] = vGood[m_index].vect().theta();
      m_phi[m_index] = vGood[m_index].vect().phi();
      m_ene[m_index] = vGood[m_index].e();
    }

    m_tuple1->write();
  }

  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

StatusCode BbEmc::finalize() {
  MsgStream log(msgSvc(),name());
  log<<MSG::INFO<<"in finalize()"<<endreq;

  std::cout <<name()<<" total event: "<< m_cout_all << std::endl;
  std::cout <<name()<<" total tracks >= 2, <= 30: "<<m_cout_tracks << std::endl;
  std::cout <<name()<<" good showers = 2: "<< m_cout_good << std::endl;
  std::cout <<name()<<" angle between two showers: "<< m_cout_dang << std::endl;

  return StatusCode::SUCCESS;
}

