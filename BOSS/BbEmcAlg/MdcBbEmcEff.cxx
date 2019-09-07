#include "BbEmcAlg/MdcBbEmcEff.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventHeader.h"
#include "RawEvent/RawDataUtil.h"
#include "Identifier/MdcID.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EvTimeEvent/RecEsTime.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "CLHEP/Units/PhysicalConstants.h"


#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatblty wll be enabled ONLY n CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

// MDC reconstructed data
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"

// Ntuple
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

typedef std::vector<HepLorentzVector> Vp4;
using namespace std;
using namespace Event;

MdcBbEmcEff::MdcBbEmcEff(const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator) {
    declareProperty("hist", m_hist = false);
    declareProperty("debug", m_debug= 0);
    //Good Emc shower selection cut
    declareProperty("emcEneCutLow",  m_emcEneCutLow=1.44);
    declareProperty("emcEneCutHigh", m_emcEneCutHigh=1.88);
    declareProperty("emcEneCutTot",  m_emcEneCutTot=3.16);
    declareProperty("emcDangCutLow", m_emcDangCutLow=2.94);
    declareProperty("emcDangCutHigh",m_emcDangCutHigh=3.08);
    //Mdc track cut
    declareProperty("dPhi", m_dPhiCut= 0.2);
    declareProperty("dCosTheta", m_dCosThetaCut= 0.2);
    declareProperty("d0", m_d0Cut= 1.);
    declareProperty("z0", m_z0Cut= 5.);
    //Barrel and Endcap cut
    declareProperty("barrelCut", m_barrelCut = 0.8);
    declareProperty("endcapCut", m_endcapCutLow = 0.83);
    declareProperty("endcapCutHigh", m_endcapCutHigh = 0.93);
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode MdcBbEmcEff::initialize(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  m_evtIndex = 0;

  m_effAllN1 = 0;
  m_effAllN2 = 0;

  for (int i=0;i<3;i++){
    m_effN1[i] = 0;
    m_effN2[i] = 0;
  }

  if(m_hist) {
    if(bookNTuple()<0) sc = StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcBbEmcEff::execute() {
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::SUCCESS;

  setFilterPassed(false);

  // Get eventNo, t0 
  if(getEventInfo()<0) return StatusCode::FAILURE;

  // Select Bhabha by Emc shower
  if(selectBbByEmcShower()<0) return StatusCode::SUCCESS; 

  // Select Good track 
  if(bbEmcMdcTrackingEff()<0) return StatusCode::SUCCESS;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcBbEmcEff::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  if((m_effAllN1+m_effAllN2)>0)std::cout<<" MdcBbEmcEff efficiency = N2/(N1+N2) = "
    << m_effAllN2<<"/("<<m_effAllN1<<"+"<<m_effAllN2<<") = " 
      << (m_effAllN2)/((float)(m_effAllN1+m_effAllN2))<<std::endl;
  for(int i=0;i<3;i++){
    string pos;
    if (0==i) pos="barrel";
    if (1==i) pos="gap";
    if (2==i) pos="endcap";
    if((m_effN1[i]+m_effN2[i])>0){
      std::cout<<" MdcBbEmcEff of "<<pos <<" N2/(N1+N2) = "
	<< m_effN2[i]<<"/("<<m_effN1[i]<<"+"<<m_effN2[i]<<") = " 
	<< (m_effN2[i])/((float)(m_effN1[i]+m_effN2[i]))<<std::endl;
    }
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
int MdcBbEmcEff::bookNTuple(){
  MsgStream log(msgSvc(), name());
  StatusCode sc; 
  NTuplePtr nt1(ntupleSvc(), "MdcBbEmcEff/evt");
  if ( nt1 ) { m_tuple1 = nt1;}
  else {
    m_tuple1 = ntupleSvc()->book ("MdcBbEmcEff/evt", CLID_ColumnWiseTuple, "event");
    if ( m_tuple1 ) {
      //event info
      sc = m_tuple1->addItem ("runNo",		m_runNo);
      sc = m_tuple1->addItem ("evtNo",		m_evtNo);
      sc = m_tuple1->addItem ("t0",		m_t0);
      sc = m_tuple1->addItem ("t0Stat",		m_t0Stat);

      //Emc shower 
      sc = m_tuple1->addItem ("index",        	m_index,0,2);
      sc = m_tuple1->addIndexedItem ("emcTheta",m_index, m_emcTheta);
      sc = m_tuple1->addIndexedItem ("emcPhi",  m_index, m_emcPhi);
      sc = m_tuple1->addIndexedItem ("emcEne",  m_index, m_emcEne);
      sc = m_tuple1->addItem ("emcDang",        m_emcDang);

      //Mdc track
      sc = m_tuple1->addItem ("dCosTheta",	m_dCosTheta);
      sc = m_tuple1->addItem ("dPhi",		m_dPhi);
      sc = m_tuple1->addItem ("nTk",		m_nTk,0,2);
      sc = m_tuple1->addIndexedItem ("phi",     m_nTk,m_phi);
      sc = m_tuple1->addIndexedItem ("cosTheta",m_nTk,m_cosTheta);
      sc = m_tuple1->addIndexedItem ("d0",	m_nTk,m_d0);
      sc = m_tuple1->addIndexedItem ("z0",	m_nTk,m_z0);
      sc = m_tuple1->addIndexedItem ("p",	m_nTk,m_p);
      sc = m_tuple1->addIndexedItem ("pt",	m_nTk,m_pt);
    } else {   
      log << MSG::ERROR << "Cannot book tuple MdcBbEmcEff/evt" << endmsg;
      return -1;
    }
  }
  return 1;
}

int MdcBbEmcEff::getEventInfo(){
  MsgStream log(msgSvc(), name());

  // Get event header
  SmartDataPtr<Event::EventHeader> evtHead(eventSvc(),"/Event/EventHeader");
  if (evtHead) {
    t_runNo = evtHead->runNumber();
    t_evtNo = evtHead->eventNumber();
  }else{
    log << MSG::WARNING<< "Could not retreve event header" << endreq;
  }

  std::cout<<m_evtIndex <<"------------evtNo:"<<t_evtNo << std::endl;//yzhang debug
  m_evtIndex++;

  //Get event start tme
  t_t0 = -1;
  t_t0Stat = -1;
  SmartDataPtr<RecEsTimeCol> aevtmeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if ((!aevtmeCol)||(aevtmeCol->size()==0)) {
    log << MSG::WARNING << "Could not fnd RecEsTimeCol" << endreq;
  }else{
    RecEsTimeCol::iterator iter_evt = aevtmeCol->begin();
    t_t0 = (*iter_evt)->getTest();
    t_t0Stat = (*iter_evt)->getStat();
  } 
  return 1;
}

int MdcBbEmcEff::selectBbByEmcShower(){
  MsgStream log(msgSvc(), name());

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), "/Event/EvtRec/EvtRecEvent");
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
    << evtRecEvent->totalCharged() << " , "
    << evtRecEvent->totalNeutral() << " , "
    << evtRecEvent->totalTracks() <<endreq;
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(), "/Event/EvtRec/EvtRecTrackCol");

  Vp4 vGood;
  HepLorentzVector m_lv_ele;
  for(int i=0; i<evtRecEvent->totalTracks(); i++){
    if(i>=evtRecTrkCol->size()) return -1;

    EvtRecTrackIterator  itTrk =evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) {
      if(m_debug>1)std::cout<<"EmcShower NOT Valid   "<<std::endl;
      continue;
    }
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    if((emcTrk->energy()>m_emcEneCutHigh)||(emcTrk->energy()<m_emcEneCutLow)){
      if(m_debug>1)std::cout<<"Cut by EmcEnergy: "<<emcTrk->energy()<<std::endl;
      continue;
    }
    Hep3Vector emcpos(emcTrk->x(), emcTrk->y(), emcTrk->z()); 
    m_lv_ele.setVect(emcpos);
    m_lv_ele.setE(emcTrk->energy());

    vGood.push_back(m_lv_ele);
  }

  if(m_debug>1)std::cout<<"vGood.size =  "<<vGood.size()<<std::endl;
  if(vGood.size()!=2){
    if (m_debug>0) std::cout<<"Cut by vGood.size: "<<vGood.size()<<std::endl;
    return -2;//num of good showers==2
  }


  //Barrel or endcap 
  //if one shower in endcap , then this event in endcap
  for(int i=0; i<2; i++){
    double cosEmcTheta = cos(vGood[i].vect().theta()); 
    if( fabs(cosEmcTheta) <= m_barrelCut ){
      m_posFlag = BARREL;
    }else if((cosEmcTheta>=m_endcapCutLow)&&(cosEmcTheta<=m_endcapCutHigh)){
      m_posFlag = ENDCAP;
      break;
    }else if((cosEmcTheta>m_barrelCut)&&(cosEmcTheta<m_endcapCutLow)){
      m_posFlag = GAP;
    }else{
      m_posFlag = OUT;
    }
    if(m_debug>1) std::cout<<"Emc cos(theta)="<<cosEmcTheta
      <<"Track in "<<m_posFlag<<std::endl;
  }
  if(m_posFlag == OUT) return -5;

  double dang = vGood[0].vect().angle(vGood[1].vect());
  if(vGood[0].e() > vGood[1].e()) swap(vGood[0],vGood[1]);
  double emcTheta[2],emcEne[2]; //emcPhi[2]
  for(int index=0; index<2; index++){
    emcTheta[index] = vGood[index].vect().theta();
    t_emcPhi[index] = vGood[index].vect().phi();
    emcEne[index] = vGood[index].e();
  }

  if(m_hist){
    m_index = 2;
    for (int index=0;index<2;index++){
      m_emcTheta[index] = emcTheta[index];
      m_emcPhi[index] = t_emcPhi[index];
      m_emcEne[index] = emcEne[index];
    }
    m_emcDang = dang;
  }
  if(m_debug>1) std::cout<<" dang  "<<dang<<std::endl;
  if(m_debug>1) std::cout<<" energy  "<<emcEne[0]<<" "<<emcEne[1]<<std::endl;
  //delta(angle) cut
  if(dang<=m_emcDangCutLow || dang>=m_emcDangCutHigh ) {
    if(m_debug>0) std::cout<<"Cut by emcDang "<<dang<<std::endl;
    return -3;
  }
  //cut by shower energy
  if(emcEne[0]<=m_emcEneCutLow || emcEne[1]>=m_emcEneCutHigh){
    if(m_debug>0) std::cout<<"Cut by emc energy low or high  "
      <<emcEne[0]<<" "<<emcEne[1]<<std::endl;
    return -4;
  }
  if( (emcEne[0] + emcEne[1])<=m_emcEneCutTot){
    if(m_debug>0) std::cout<<"Cut by emc total energy:"
      <<emcEne[0]<<" "<<emcEne[1]<<" tot:"<<emcEne[0]+emcEne[1]<<std::endl;
    return StatusCode::FAILURE;
  }

  return 1;
}

int MdcBbEmcEff::bbEmcMdcTrackingEff(){
  MsgStream log(msgSvc(), name());

  // Get RecMdcTrackCol and RecMdcHitCol
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(), "/Event/Recon/RecMdcTrackCol");
  if (!recMdcTrackCol){
    log << MSG::WARNING<< " Unable to retrieve recMdcTrackCol" << endreq;
    return -1;
  } 

  //1.Take track number ==1 or ==2
  t_nTk = recMdcTrackCol->size();
  if((t_nTk>2) || (0==t_nTk)) {
    if(m_debug>1)std::cout<<name()<<"Cut by nTk   "<<t_nTk<<std::endl;
    return -2;
  }

  // Get RecMdcTrack info
  double d0[2],z0[2],cosTheta[2],phi[2],p[2],pt[2];
  RecMdcTrackCol::iterator it = recMdcTrackCol->begin();
  for(int iTk=0 ; it != recMdcTrackCol->end(); it++,iTk++ ) {   
    RecMdcTrack* tk = (*it);
    d0[iTk] = tk->helix(0);
    z0[iTk] = tk->helix(3);
    if ((fabs(d0[iTk])>m_d0Cut) || (fabs(z0[iTk])>m_z0Cut)){
      if(m_debug>0) std::cout<<"Cut by d0 "
	<<d0[iTk]<<" z0 "<<z0[iTk]<<std::endl;
      return -3;
    }
    cosTheta[iTk] = cos(tk->theta());
    phi[iTk] = tk->phi();
    p[iTk] = tk->p();
    pt[iTk] = tk->pxy();
  }

  double dCosTheta = cosTheta[0]+cosTheta[1];
  double dPhi = phi[0] - phi[1] + CLHEP::pi;
  if(dPhi > CLHEP::pi) dPhi-=CLHEP::twopi;

  //hist Mdc track info
  if (m_hist){
    m_runNo = t_runNo;
    m_evtNo = t_evtNo;
    m_t0    = t_t0;
    m_t0Stat= t_t0Stat;

    for(int i=0;i<2;i++){
      m_cosTheta[i]=cosTheta[i];
      m_phi[i]=phi[i];
      m_d0[i]=d0[i];
      m_z0[i]=z0[i];
      m_p[i]=p[i];
      m_pt[i]=pt[i];
    }
    m_nTk = t_nTk;
    m_dCosTheta= dCosTheta;
    m_dPhi  = dPhi;

    m_nTk = t_nTk;
    m_tuple1->write();
  }

  //4.Angle cut of 2 track event: delta(theta)<0.1 and delta(phi)<0.1
  if(2==t_nTk){
    if((fabs(dCosTheta)>m_dCosThetaCut) || (fabs(dPhi)>m_dPhiCut)){
      if(m_debug>0) {
	if (fabs(dCosTheta)>m_dCosThetaCut){
	  std::cout<<"Cut by dCosTheta "<<dCosTheta<<std::endl;
	}else{
	  std::cout<<"Cut by dPhi "<<dPhi<<std::endl;
	}
      }
      return -4;
    }
    m_effAllN2++;
    m_effN2[m_posFlag]++;
  }else{
    m_effAllN1++;
    m_effN1[m_posFlag]++;
  }


  if ((1==t_nTk)&&(m_posFlag==BARREL)) setFilterPassed(true);
  return 1;
}
