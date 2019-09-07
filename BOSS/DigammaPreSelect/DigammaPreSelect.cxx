#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"

#include "TMath.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/TwoVector.h"
#include "MdcRawEvent/MdcDigi.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "DigammaPreSelect/DigammaPreSelect.h"
int  selected =0;

#include <vector>

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
const double PI = 3.1415927;
const double EBeam=1.548; //temporary
/////////////////////////////////////////////////////////////////////////////

DigammaPreSelect::DigammaPreSelect(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

    //Declare the properties  

    declareProperty ("Vr0cut", m_vr0cut=1.0);   // suggest cut: |z0|<5cm && r0<1cm
    declareProperty ("Vz0cut", m_vz0cut=5.0);

    declareProperty ("lowEnergyShowerCut",  m_lowEnergyShowerCut=0.1);
    declareProperty ("highEnergyShowerCut",  m_highEnergyShowerCut=0.5);
    declareProperty ("matchThetaCut",  m_matchThetaCut = 0.2);
    declareProperty ("matchPhiCut", m_matchPhiCut = 0.2);

    declareProperty ("highMomentumCut", m_highMomentumCut = 0.5);
    declareProperty ("EoPMaxCut", m_EoPMaxCut =1.3);
    declareProperty ("EoPMinCut", m_EoPMinCut = 0.6);
    declareProperty ("minAngShEnergyCut", m_minAngShEnergyCut = 0.2);
    declareProperty ("minAngCut", m_minAngCut = 0.3);
    declareProperty ("acolliCut", m_acolliCut = 0.03);
    declareProperty ("eNormCut", m_eNormCut = 0.5);
    declareProperty ("pNormCut", m_pNormCut = 0.5);
    declareProperty ("BarrelOrEndcap", m_BarrelOrEndcap = 1);
    declareProperty ("Output", m_output = false);
    declareProperty ("oneProngMomentumCut", m_oneProngMomentumCut =1.2);

  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DigammaPreSelect::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  if(m_output) {
    StatusCode status;
    NTuplePtr nt1(ntupleSvc(), "FILE1/bhabha");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILE1/bhabha", CLID_ColumnWiseTuple, "N-Tuple example");
      if ( m_tuple1 )    {
        status = m_tuple1->addItem ("sh1_ene",m_sh1_ene);
        status = m_tuple1->addItem ("sh1_theta", m_sh1_theta);
        status = m_tuple1->addItem ("sh1_phi", m_sh1_phi);
        status = m_tuple1->addItem ("sh2_ene",m_sh2_ene);
        status = m_tuple1->addItem ("sh2_theta", m_sh2_theta);
        status = m_tuple1->addItem ("sh2_phi", m_sh2_phi);
        status = m_tuple1->addItem ("di_phi", m_di_phi);
        status = m_tuple1->addItem ("di_the", m_di_the);
        status = m_tuple1->addItem ("acolli", m_acolli);
        status = m_tuple1->addItem ("mdc_hit", m_mdc_hit);
        status = m_tuple1->addItem ("etot", m_etot);

      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
        return StatusCode::FAILURE;
      }
    }


    NTuplePtr nt2(ntupleSvc(), "FILE1/bha1");
    if ( nt2 ) m_tuple2 = nt2;
    else {
      m_tuple2 = ntupleSvc()->book ("FILE1/bha1", CLID_ColumnWiseTuple, "N-Tuple example");
      if ( m_tuple2 )    {
        status = m_tuple2->addItem ("sh_ene",m_sh_ene);
        status = m_tuple2->addItem ("sh_theta", m_sh_theta);
        status = m_tuple2->addItem ("sh_phi", m_sh_phi);
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
        return StatusCode::FAILURE;
      }
    }
  }


  //
  //--------end of book--------
  //

  m_rejected=0;
  m_events=0;
  m_oneProngsSelected=0;
  m_twoProngsMatchedSelected=0;
  m_twoProngsOneMatchedSelected=0;
  m_selectedTrkID1=999;
  m_selectedTrkID2=999;

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DigammaPreSelect::execute() {

  setFilterPassed(false);

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  m_events++;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if(!eventHeader)
  {
    cout<<"  eventHeader  "<<endl;
    return StatusCode::FAILURE;
  }

  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();
  if(event%1000==0)   cout<<"  run,event:  "<<run<<","<<event<<endl;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  if(!evtRecEvent ) {
    cout<<"  evtRecEvent  "<<endl;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG <<"ncharg, nneu, tottks = " 
    << evtRecEvent->totalCharged() << " , "
    << evtRecEvent->totalNeutral() << " , "
    << evtRecEvent->totalTracks() <<endreq;
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  if(!evtRecTrkCol){
    cout<<"  evtRecTrkCol  "<<endl;
    return StatusCode::FAILURE;
  }

  Vint iGood;
  iGood.clear();

  double ene5x5,theta,phi,eseed;
  double showerX,showerY,showerZ;
  long int thetaIndex,phiIndex;

  RecEmcID showerId;
  unsigned int npart;

  Vint ipip, ipim;
  ipip.clear();
  ipim.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  vector<RecEmcShower* >  GoodShowers;
  GoodShowers.clear();
  double etot=0;
  for(int i = 0; i< evtRecEvent->totalTracks(); i++) {
    if(i>=evtRecTrkCol->size()) break;
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if((*itTrk)->isEmcShowerValid()) {
      RecEmcShower *theShower = (*itTrk)->emcShower();
      etot+=theShower->e5x5();
      showerId = theShower->getShowerId();
      npart = EmcID::barrel_ec(showerId);
      ene5x5=theShower->e5x5(); 
      thetaIndex=EmcID::theta_module(showerId);
      phiIndex=EmcID::phi_module(showerId);  
      if (ene5x5>0.4&&ene5x5<4&&npart==1&&(m_BarrelOrEndcap==1)){
        GoodShowers.push_back(theShower); 
        iGood.push_back((*itTrk)->trackId());
      }
      else if (ene5x5>0.4&&ene5x5<4&&(npart==2||npart==0)&&(m_BarrelOrEndcap==2)){
        GoodShowers.push_back(theShower); 
        iGood.push_back((*itTrk)->trackId());
      }
      else if (ene5x5>0.4&&ene5x5<4&&(m_BarrelOrEndcap==0)){
        GoodShowers.push_back(theShower); 
        iGood.push_back((*itTrk)->trackId());
      }
    }
    // good photon cut will be set here
  }
  // Finish Good Photon Selection
  //

  double  MaxE(0), MaxPhi, MaxThe;
  int MaxId;
  double  SecE(0), SecPhi, SecThe;
  for(int i=0; i<GoodShowers.size(); i++) {
    RecEmcShower *theShower = GoodShowers[i];
    double eraw = theShower->energy();
    if(eraw> MaxE) {
      MaxId =i;
      MaxE =eraw;
      MaxPhi = theShower->phi();
      MaxThe = theShower->theta();
    }
  }
  for(int i=0; i<GoodShowers.size(); i++) {
    RecEmcShower *theShower = GoodShowers[i];
    double eraw = theShower->energy();
    if(i!=MaxId&&eraw>SecE) {
      SecE =eraw;
      SecPhi = theShower->phi();
      SecThe = theShower->theta();
    }
  }

  double dphi = (fabs(MaxPhi-SecPhi)-PI)*180./PI;
  double dthe = (fabs(MaxThe+SecThe)-PI)*180./PI;
  if(GoodShowers.size()>=2&&SecE>1.0&&dphi>-4&&dphi<2&&abs(dthe)<3&&etot>2.7) {
    setFilterPassed(true);
    selected++;
  }

  if(m_output) {
    m_etot=etot;
    m_sh1_ene = MaxE;
    m_sh1_theta = MaxThe;
    m_sh1_phi = MaxPhi;
    m_sh2_ene = SecE;
    m_sh2_theta = SecThe;
    m_sh2_phi = SecPhi;
    m_di_phi = dphi;
    m_di_the = dthe;
    m_tuple1->write();
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DigammaPreSelect::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  cout<<"total events: "<<m_events<<endl;
  cout<<"selected digamma: "<<selected<<endl;

  return StatusCode::SUCCESS;
}

