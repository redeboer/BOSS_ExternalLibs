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
#include "BhabhaPreSelect/BhabhaPreSelect.h"
#include "BhabhaPreSelect/BhabhaType.h"
int  selected =0;

#include <vector>

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;
const double PI = 3.1415927;
const double EBeam=1.548; //temporary
int BhabhaPreSelect::idmax[43]={40,44,48,56,64,72,80,80,76,76,
  88,88,100,100,112,112,128,128,140,140,
  160,160,160,160,176,176,176,176,208,208,
  208,208,240,240,240,240,256,256,256,256,
  288,288,288};

/////////////////////////////////////////////////////////////////////////////

BhabhaPreSelect::BhabhaPreSelect(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

    //Declare the properties  

    declareProperty ("BarrelOrEndcap", m_BarrelOrEndcap = 1);
    declareProperty ("Output", m_output = false);
    //cout<<"  BarrelOrEndcap  "<<m_BarrelOrEndcap<<endl;

  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode BhabhaPreSelect::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;
  cout<<"  BarrelOrEndcap  "<<m_BarrelOrEndcap<<endl;

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
        status = m_tuple1->addItem ("etot", m_etot);
        status = m_tuple1->addItem ("mdc_hit1", m_mdc_hit1);
        status = m_tuple1->addItem ("mdc_hit2", m_mdc_hit2);

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
StatusCode BhabhaPreSelect::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  //if(!eventHeader)
  //{
  //  cout<<"  eventHeader  "<<endl;
  //  return StatusCode::FAILURE;
  //}

  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();
  //cout<<"  event  "<<event<<endl;
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

  m_events++;
  setFilterPassed(false);

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

  int total1 = 0;  //mdc hit
  int total2 = 0;  //mdc hit
  if(GoodShowers.size()>=2 && MaxE>1.0 && abs(dthe)<3
      && ((dphi>-25&&dphi<-4)||(dphi>2&&dphi<20)) && etot>2.7) {

    double phi1 = MaxPhi<0 ? MaxPhi+CLHEP::twopi : MaxPhi;
    double phi2 = SecPhi<0 ? SecPhi+CLHEP::twopi : SecPhi;

    //Define sector (phi11,phi12) and (phi21,phi22)
    double phi11=min(phi1,phi2);
    double phi22=max(phi1,phi2);
    double phi12=(phi11+phi22-CLHEP::pi)*0.5;
    double phi21=(phi11+phi22+CLHEP::pi)*0.5;
    if(phi12<0.) phi12 += CLHEP::twopi;
    if(phi21>CLHEP::twopi) phi21 -= CLHEP::twopi;

    SmartDataPtr<MdcDigiCol> mdcDigiCol(evtSvc(), "/Event/Digi/MdcDigiCol");
    if (!mdcDigiCol) {
      log << MSG::FATAL << "Could not find MdcDigiCol!" << endreq;
      return StatusCode::FAILURE;
    }
    int hitnum = mdcDigiCol->size();
    for (int i = 0;i< hitnum;i++ ) {
      MdcDigi* digi= dynamic_cast<MdcDigi*>(mdcDigiCol->containedObject(i));
      double time =  digi->getTimeChannel();
      double charge = digi->getChargeChannel();
      if (time == 0x7FFFFFFF || charge == 0x7FFFFFFF) continue;
      Identifier id(digi->identify());
      unsigned int iphi=MdcID::wire(id);
      unsigned int ilayer=MdcID::layer(id);
      if(ilayer>=43)
        log << MSG::ERROR << "MDC(" << ilayer <<","<<iphi <<")"<<endreq;
      double phi=CLHEP::twopi*iphi/idmax[ilayer];
      if(WhetherSector(phi,phi11,phi12)) total1++;
      else if(WhetherSector(phi,phi21,phi22)) total2++;
      //cout<<"phi="<<phi<<",phi11="<<phi11<<",phi12="<<phi12
        //<<",phi21="<<phi21<<",phi22="<<phi22<<endl;
    }
    //cout<<"total1="<<total1<<"total2="<<total2<<endl;
    if ( (m_BarrelOrEndcap==1&&total1>15 &&total2>15)
        || (m_BarrelOrEndcap!=1&&total1>5 &&total2>5) ) {
      setFilterPassed(true);
      selected++;
    }
  }

  if(m_output) {
    m_etot=etot;
    m_mdc_hit1=total1;
    m_mdc_hit2=total2;
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
StatusCode BhabhaPreSelect::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  cout<<"m_events "<<m_events<<endl;
  cout<<" selected "<<selected<<endl;
  cout<<"m_rejected "<<m_rejected<<endl;
  cout<<"m_oneProngsSelected "<<m_oneProngsSelected<<endl;
  cout<<"m_twoProngsMatchedSelected "<<m_twoProngsMatchedSelected<<endl;
  cout<<"m_twoProngsOneMatchedSelected "<<m_twoProngsOneMatchedSelected<<endl;

  return StatusCode::SUCCESS;
}

bool BhabhaPreSelect::WhetherSector(double ph,double ph1,double ph2){
  double phi1=min(ph1,ph2);
  double phi2=max(ph1,ph2);
  double delta=0.0610865; //3.5*3.1415926/180.
  if((phi2-phi1)<CLHEP::pi){
    phi1 -=delta;
    phi2 +=delta;
    if(phi1<0.) phi1 += CLHEP::twopi;
    if(phi2>CLHEP::twopi) phi2 -= CLHEP::twopi;
    double tmp1=min(phi1,phi2);
    double tmp2=max(phi1,phi2);
    phi1=tmp1;
    phi2=tmp2;
  }
  else{
    phi1 +=delta;
    phi2 -=delta;
  }
  if((phi2-phi1)<CLHEP::pi){
    if(ph<=phi2&&ph>=phi1) return true;
    else   return false;
  }
  else{
    if(ph>=phi2||ph<=phi1) return true;
    else   return false;
  }
}

