#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
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
#include "EmcRawEvent/EmcDigi.h"
#include "RawEvent/RawDataUtil.h"
#include "MdcRawEvent/MdcDigi.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
using CLHEP::Hep3Vector;
using CLHEP::Hep2Vector;
using CLHEP::HepLorentzVector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "EventPreSelect/EventPreSelect.h"
#include "EventPreSelect/DimuPreSelect.h"

#include <vector>

typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;

const double mpi = 0.13957;

int EventPreSelect::idmax[43]={40,44,48,56,64,72,80,80,76,76,
  88,88,100,100,112,112,128,128,140,140,
  160,160,160,160,176,176,176,176,208,208,
  208,208,240,240,240,240,256,256,256,256,
  288,288,288};
/////////////////////////////////////////////////////////////////////////////

EventPreSelect::EventPreSelect(const std::string& name, ISvcLocator* pSvcLocator) :  
  Algorithm(name, pSvcLocator) {
    //Declare the properties  

    declareProperty("Output",           m_output = false);
    declareProperty("Ecm",              m_ecm=3.686);
    declareProperty("SelectBhabha",     m_selectBhabha=false);
    declareProperty("SelectDimu",       m_selectDimu=false);
    declareProperty("SelectHadron",     m_selectHadron=false);
    declareProperty("SelectDiphoton",   m_selectDiphoton=false);
    declareProperty("WriteDst",         m_writeDst=false);
    declareProperty("WriteRec",         m_writeRec=false);
    declareProperty("Vr0cut",           m_vr0cut=1.0);
    declareProperty("Vz0cut",           m_vz0cut=5.0);
    declareProperty("Pt0HighCut",       m_pt0HighCut=5.0);
    declareProperty("Pt0LowCut",        m_pt0LowCut=0.05);
    declareProperty("EnergyThreshold",  m_energyThreshold=0.05);
    declareProperty("GammaPhiCut",      m_gammaPhiCut=20.0);
    declareProperty("GammaThetaCut",    m_gammaThetaCut=20.0);
    
    declareProperty("BhabhaEmcECut",    m_bhabhaEmcECut=0.7*m_ecm);
    declareProperty("BhabhaMaxECut",    m_bhabhaMaxECut=0.3*m_ecm);
    declareProperty("BhabhaSecECut",    m_bhabhaSecECut=0.1*m_ecm);
    declareProperty("BhabhaDTheCut",    m_bhabhaDTheCut=3.);
    declareProperty("BhabhaDPhiCut1",   m_bhabhaDPhiCut1=-25.);
    declareProperty("BhabhaDPhiCut2",   m_bhabhaDPhiCut2=-4.);
    declareProperty("BhabhaDPhiCut3",   m_bhabhaDPhiCut3=2.);
    declareProperty("BhabhaDPhiCut4",   m_bhabhaDPhiCut4=20.);
    declareProperty("BhabhaMdcHitCutB", m_bhabhaMdcHitCutB=15);
    declareProperty("BhabhaMdcHitCutE", m_bhabhaMdcHitCutE=5);
    
    declareProperty("DimuEHighCut",     m_dimuEHighCut=0.27*m_ecm);
    declareProperty("DimuELowCut",      m_dimuELowCut=0.027*m_ecm);
    declareProperty("DimuDTheCut",      m_dimuDTheCut=3.);
    declareProperty("DimuDPhiCut",      m_dimuDPhiCut=23.);
    
    declareProperty("HadronChaECut",    m_hadronChaECut=0.3*m_ecm);
    declareProperty("HadronNeuECut",    m_hadronNeuECut=0.3*m_ecm);
    
    declareProperty("DiphotonEmcECut",  m_diphotonEmcECut=0.7*m_ecm);
    declareProperty("DiphotonSecECut",  m_diphotonSecECut=0.3*m_ecm);
    declareProperty("DiphotonDTheCut",  m_diphotonDTheCut=3.);
    declareProperty("DiphotonDPhiCut1", m_diphotonDPhiCut1=-4.);
    declareProperty("DiphotonDPhiCut2", m_diphotonDPhiCut2=2.);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EventPreSelect::initialize() {
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  m_bhabhaEmcECut=0.7*m_ecm;
  m_bhabhaMaxECut=0.3*m_ecm;
  m_bhabhaSecECut=0.1*m_ecm;
  m_dimuEHighCut=0.27*m_ecm;
  m_dimuELowCut=0.027*m_ecm;
  m_diphotonEmcECut=0.7*m_ecm;
  m_diphotonSecECut=0.3*m_ecm;
  m_hadronChaECut=0.3*m_ecm;
  m_hadronNeuECut=0.3*m_ecm;


  if (m_ecm==2.4) {
  m_bhabhaEmcECut=0.7*m_ecm;
  m_bhabhaMaxECut=0.15*m_ecm;
  m_bhabhaSecECut=0.05*m_ecm;
  m_diphotonEmcECut=0.7*m_ecm;
  m_diphotonSecECut=0.2*m_ecm;

  }



  StatusCode sc;

  log << MSG::INFO << "creating sub-algorithms...." << endreq;

  if(m_selectBhabha) {
    sc =  createSubAlgorithm( "EventWriter", "SelectBarrelBhabha", m_subalg1);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectBhabhaBarrel" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectBhabhaBarrel" <<endreq;
    }
    sc =  createSubAlgorithm( "EventWriter", "SelectEndcapBhabha", m_subalg2);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectBhabhaEndcap" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectBhabhaEndcap" <<endreq;
    }
  }

  if(m_selectDimu) {
    m_dimuAlg = new DimuPreSelect;
    sc =  createSubAlgorithm( "EventWriter", "SelectBarrelDimu", m_subalg3);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectDimuBarrel" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectDimuBarrel" <<endreq;
    }
    sc =  createSubAlgorithm( "EventWriter", "SelectEndcapDimu", m_subalg4);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectDimuEndcap" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectDimuEndap" <<endreq;
    }
  }

  if(m_selectHadron) {
    sc =  createSubAlgorithm( "EventWriter", "SelectHadron", m_subalg5);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectHadron" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectHadron" <<endreq;
    }
  }

  if(m_selectDiphoton) {
    sc =  createSubAlgorithm( "EventWriter", "SelectBarrelDiphoton", m_subalg6);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectDiphotonBarrel" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectDiphotonBarrel" <<endreq;
    }
    sc =  createSubAlgorithm( "EventWriter", "SelectEndcapDiphoton", m_subalg7);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm SelectDiphotonEndcap" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm SelectDiphotonEndcap" <<endreq;
    }
  }

  if(m_writeDst) {
    sc =  createSubAlgorithm( "EventWriter", "WriteDst", m_subalg8);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm WriteDst" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm WriteDst" <<endreq;
    }
  }

  if(m_writeRec) {
    sc =  createSubAlgorithm( "EventWriter", "WriteRec", m_subalg9);
    if( sc.isFailure() ) {
      log << MSG::ERROR << "Error creating Sub-Algorithm WriteRec" <<endreq;
      return sc;
    } else {
      log << MSG::INFO << "Success creating Sub-Algorithm WriteRec" <<endreq;
    }
  }

  if(m_output) {
    StatusCode status;
    NTuplePtr nt0(ntupleSvc(), "FILE1/hadron");
    if ( nt0 ) m_tuple0 = nt0;
    else {
      m_tuple0 = ntupleSvc()->book ("FILE1/hadron", CLID_ColumnWiseTuple, "N-Tuple example");
      if ( m_tuple0 )    {
        status = m_tuple0->addItem ("esum",     m_esum);
        status = m_tuple0->addItem ("eemc",     m_eemc);
        status = m_tuple0->addItem ("etot",     m_etot);
        status = m_tuple0->addItem ("nGood",    m_nGood);
        status = m_tuple0->addItem ("nCharge",  m_nCharge);
        status = m_tuple0->addItem ("nGam",     m_nGam);
        status = m_tuple0->addItem ("ptot",     m_ptot);
        status = m_tuple0->addItem ("pp",       m_pp);
        status = m_tuple0->addItem ("pm",       m_pm);
        status = m_tuple0->addItem ("run",      m_runnb);
        status = m_tuple0->addItem ("event",    m_evtnb);
        status = m_tuple0->addItem ("maxE",     m_maxE);
        status = m_tuple0->addItem ("secE",     m_secE);
        status = m_tuple0->addItem ("dthe",     m_dThe);
        status = m_tuple0->addItem ("dphi",     m_dPhi);
        status = m_tuple0->addItem ("mdcHit1",  m_mdcHit1);
        status = m_tuple0->addItem ("mdcHit2",  m_mdcHit2);
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple0) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt1(ntupleSvc(), "FILE1/vxyz");
    if ( nt1 ) m_tuple1 = nt1;
    else {
      m_tuple1 = ntupleSvc()->book ("FILE1/vxyz", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple1 )    {
        status = m_tuple1->addItem ("vx0",    m_vx0);
        status = m_tuple1->addItem ("vy0",    m_vy0);
        status = m_tuple1->addItem ("vz0",    m_vz0);
        status = m_tuple1->addItem ("vr0",    m_vr0);
        status = m_tuple1->addItem ("theta0", m_theta0);
        status = m_tuple1->addItem ("p0",     m_p0);
        status = m_tuple1->addItem ("pt0",    m_pt0);
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple1) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    NTuplePtr nt2(ntupleSvc(), "FILE1/photon");
    if ( nt2 ) m_tuple2 = nt2;
    else {
      m_tuple2 = ntupleSvc()->book ("FILE1/photon", CLID_ColumnWiseTuple, "ks N-Tuple example");
      if ( m_tuple2 )    {
        status = m_tuple2->addItem ("dthe",   m_dthe);
        status = m_tuple2->addItem ("dphi",   m_dphi);
        status = m_tuple2->addItem ("dang",   m_dang);
        status = m_tuple2->addItem ("eraw",   m_eraw);
      }
      else    { 
        log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple2) << endmsg;
        return StatusCode::FAILURE;
      }
    }

    if(m_selectDimu) {
      NTuplePtr nt3(ntupleSvc(), "FILE1/dimu");
      if ( nt3 ) m_tuple3 = nt3;
      else {
        m_tuple3 = ntupleSvc()->book ("FILE1/dimu", CLID_ColumnWiseTuple, "ks N-Tuple example");
        if ( m_tuple3 )    {
          m_dimuAlg->BookNtuple(m_tuple3);
        }
        else    { 
          log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple3) << endmsg;
          return StatusCode::FAILURE;
        }
      }
    }
  }
  //
  //--------end of book--------
  //

  m_events=0;
  m_barrelBhabhaNumber=0; 
  m_endcapBhabhaNumber=0; 
  m_barrelDimuNumber=0;
  m_endcapDimuNumber=0;
  m_hadronNumber=0; 
  m_barrelDiphotonNumber=0;
  m_endcapDiphotonNumber=0;

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EventPreSelect::execute() {

  //setFilterPassed(false);

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  if( m_writeDst) m_subalg8->execute();
  if( m_writeRec) m_subalg9->execute();

  m_isBarrelBhabha    = false;
  m_isEndcapBhabha    = false;
  m_isBarrelDimu      = false;
  m_isEndcapDimu      = false;
  m_isHadron          = false;
  m_isBarrelDiphoton  = false;
  m_isEndcapDiphoton  = false;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if(!eventHeader)
  {
    cout<<"  eventHeader  "<<endl;
    return StatusCode::FAILURE;
  }

  int run=eventHeader->runNumber();
  int event=eventHeader->eventNumber();

  if(m_events%1000==0)   cout<< m_events << "  --------  run,event:  "<<run<<","<<event<<endl;
  m_events++;

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

  if(evtRecEvent->totalTracks()!=evtRecTrkCol->size()) return StatusCode::SUCCESS;

  // -------- Good Charged Track Selection
  Vint iGood;
  iGood.clear();
  int nCharge = 0;
  for(int i = 0;i < evtRecEvent->totalCharged(); i++)
  {     
    //if(i>=evtRecTrkCol->size()) break;
    EvtRecTrackIterator  itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isMdcTrackValid()) continue;
    RecMdcTrack *mdcTrk =(*itTrk)->mdcTrack();
    double vx0 = mdcTrk->x();
    double vy0 = mdcTrk->y();
    double vz0 = mdcTrk->z();
    double vr0 = mdcTrk->r();
    double theta0 = mdcTrk->theta();
    double p0 = mdcTrk->p();
    double pt0 = mdcTrk->pxy();

    if(m_output) {
      m_vx0 = vx0;
      m_vy0 = vy0;
      m_vz0 = vz0;
      m_vr0 = vr0;
      m_theta0 = theta0;
      m_p0 = p0;
      m_pt0 = pt0;
      m_tuple1->write();
    }

    if(fabs(vz0) >= m_vz0cut) continue;
    if(vr0 >= m_vr0cut) continue;
    if(pt0 >= m_pt0HighCut) continue;
    if(pt0 <= m_pt0LowCut) continue;

    iGood.push_back((*itTrk)->trackId());
    nCharge += mdcTrk->charge();
  }
  int nGood = iGood.size();

  // -------- Good Photon Selection
  Vint iGam;
  iGam.clear();
  for(int i = evtRecEvent->totalCharged(); i< evtRecEvent->totalTracks(); i++) {
    //if(i>=evtRecTrkCol->size()) break;
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    if(!(*itTrk)->isEmcShowerValid()) continue;
    RecEmcShower *emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    if(m_output) {
      m_eraw = eraw;
      m_tuple2->write();
    }
    if(eraw < m_energyThreshold) continue;
    iGam.push_back((*itTrk)->trackId());
  }
  int nGam = iGam.size();

  // -------- Assign 4-momentum to each charged track
  Vint ipip, ipim;
  ipip.clear();
  ipim.clear();
  Vp4 ppip, ppim;
  ppip.clear();
  ppim.clear();

  //cout<<"charged track:"<<endl;
  double echarge = 0.;  //total energy of charged track
  double ptot = 0.; //total momentum of charged track
  double etot = 0.; //total energy in MDC and EMC
  double eemc = 0.; //total energy in EMC
  double pp = 0.;
  double pm = 0.;

  for(int i = 0; i < nGood; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGood[i]; 

    if((*itTrk)->isMdcTrackValid()) { 

      //RecMdcKalTrack* mdcKalTrk = (*itTrk)->mdcKalTrack();//After ParticleID, use RecMdcKalTrack substitute RecMdcTrack
      //RecMdcKalTrack::setPidType(RecMdcKalTrack::pion);//PID can set to electron, muon, pion, kaon and proton;The default setting is pion
      //If use this algorithm in reconstruction job, mdcKalTrk->p()=0!!! I don't know why.

      RecMdcTrack* mdcTrk = (*itTrk)->mdcTrack();

      ptot += mdcTrk->p();

      HepLorentzVector ptrk;
      ptrk.setPx(mdcTrk->px());
      ptrk.setPy(mdcTrk->py());
      ptrk.setPz(mdcTrk->pz());
      double p3 = ptrk.mag();
      ptrk.setE(sqrt(p3*p3+mpi*mpi));
      ptrk = ptrk.boost(-0.011,0,0);//boost to cms

      echarge += ptrk.e();
      etot += ptrk.e();

      if(mdcTrk->charge() >0) {
        ppip.push_back(ptrk);
        pp = ptrk.rho();
      } else {
        ppim.push_back(ptrk);
        pm = ptrk.rho();
      }

    }

    if((*itTrk)->isEmcShowerValid()) {

      RecEmcShower* emcTrk = (*itTrk)->emcShower();
      double eraw = emcTrk->energy();
      double phi = emcTrk->phi();
      double the = emcTrk->theta();

      HepLorentzVector ptrk;
      ptrk.setPx(eraw*sin(the)*cos(phi));
      ptrk.setPy(eraw*sin(the)*sin(phi));
      ptrk.setPz(eraw*cos(the));
      ptrk.setE(eraw);
      ptrk = ptrk.boost(-0.011,0,0);// boost to cms

      eemc += ptrk.e();
      etot += ptrk.e();

    }

  }


  // -------- Assign 4-momentum to each photon
  //cout<<"neutral:"<<endl;
  Vp4 pGam;
  pGam.clear();
  double eneu=0;  //total energy of neutral track
  for(int i = 0; i < nGam; i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + iGam[i]; 
    RecEmcShower* emcTrk = (*itTrk)->emcShower();
    double eraw = emcTrk->energy();
    double phi = emcTrk->phi();
    double the = emcTrk->theta();
    HepLorentzVector ptrk;
    ptrk.setPx(eraw*sin(the)*cos(phi));
    ptrk.setPy(eraw*sin(the)*sin(phi));
    ptrk.setPz(eraw*cos(the));
    ptrk.setE(eraw);
    ptrk = ptrk.boost(-0.011,0,0);// boost to cms
    pGam.push_back(ptrk);
    eneu += ptrk.e();
    etot += ptrk.e();
    eemc += ptrk.e();
  }

  double esum = echarge + eneu;

  // -------- Use EMC shower information only
  double maxE = 0.;
  double secE = 0.;
  double maxThe = 999.;
  double maxPhi = 999.;
  double secThe = 999.;
  double secPhi = 999.;
  int npart = 999.;
  double dphi = 999.;
  double dthe = 999.;
  int mdcHit1 = 0.;
  int mdcHit2 = 0.;

  SmartDataPtr<RecEmcShowerCol> aShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!aShowerCol) {
    log << MSG::WARNING << "Could not find RecEmcShowerCol" << endreq;
    return( StatusCode::SUCCESS);
  }

  int ishower = 0;
  RecEmcShowerCol::iterator iShowerCol;
  for(iShowerCol=aShowerCol->begin();  
      iShowerCol!=aShowerCol->end(); 
      iShowerCol++) {

    if(ishower == 0) {
      maxE   = (*iShowerCol)->e5x5();
      maxThe = (*iShowerCol)->theta();
      maxPhi = (*iShowerCol)->phi();
      npart  = (*iShowerCol)->module();
    } else if(ishower == 1) {
      secE   = (*iShowerCol)->e5x5();
      secThe = (*iShowerCol)->theta();
      secPhi = (*iShowerCol)->phi();
    } else if(ishower == 2) {
      break;
    }

    ishower++;

  }

  if(aShowerCol->size() >= 2) {

    dphi = (fabs(maxPhi-secPhi)-CLHEP::pi)*180./CLHEP::pi;
    dthe = (fabs(maxThe+secThe)-CLHEP::pi)*180./CLHEP::pi;

    double phi1 = maxPhi<0 ? maxPhi+CLHEP::twopi : maxPhi;
    double phi2 = secPhi<0 ? secPhi+CLHEP::twopi : secPhi;

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
      if(WhetherSector(phi,phi11,phi12)) mdcHit1++;
      else if(WhetherSector(phi,phi21,phi22)) mdcHit2++;
      //cout<<"phi="<<phi<<",phi11="<<phi11<<",phi12="<<phi12
      //<<",phi21="<<phi21<<",phi22="<<phi22<<endl;
    }
  }

  //If it's bhabha, return.
  //if(nGood==2 && pp+pm>3.4) return StatusCode::SUCCESS;

  // -------- Select each event
  // Select bhabha
  if( eemc>m_bhabhaEmcECut && maxE>m_bhabhaMaxECut && secE>m_bhabhaSecECut 
      && abs(dthe)<m_bhabhaDTheCut  && 
      ( (dphi>m_bhabhaDPhiCut1&&dphi<m_bhabhaDPhiCut2) || 
        (dphi>m_bhabhaDPhiCut3&&dphi<m_bhabhaDPhiCut4) ) ) {
    if( npart==1 && mdcHit1>m_bhabhaMdcHitCutB && mdcHit2>m_bhabhaMdcHitCutB ) {
      m_isBarrelBhabha = true;
      m_barrelBhabhaNumber++;
    } else if( ( npart==0 || npart==2 ) 
        && mdcHit1>m_bhabhaMdcHitCutE && mdcHit2>m_bhabhaMdcHitCutE ) {
      m_isEndcapBhabha = true;
      m_endcapBhabhaNumber++;
    }
  }

  // Select dimu
  /*if( maxE<m_dimuEHighCut && maxE>m_dimuELowCut 
      && secE<m_dimuEHighCut && secE>m_dimuELowCut  
      && fabs(dthe)<m_dimuDTheCut  && fabs(dphi)<m_dimuDPhiCut ) {
    if( npart==1 ) {
      m_isBarrelDimu = true;
      m_barrelDimuNumber++;
    } else if( npart==0 || npart==2 ) {
      m_isEndcapDimu = true;
      m_endcapDimuNumber++;
    }
  }*/
  if(m_selectDimu) {
    if( m_dimuAlg->IsDimu() == 1 ) {
      m_isBarrelDimu = true;
      m_barrelDimuNumber++;
    } else if(m_dimuAlg->IsDimu() == 2 ) {
      m_isEndcapDimu = true;
      m_endcapDimuNumber++;
    }
  }

  // Select hadron
  if( (nGood>=1 && esum>m_hadronChaECut) 
      || (nGood==0 && esum>m_hadronNeuECut) ) {
    m_isHadron = true;
    m_hadronNumber++;
  }

  // Select diphoton
  if( nGood==0 && eemc>m_diphotonEmcECut && secE>m_diphotonSecECut 
      && fabs(dthe)<m_diphotonDTheCut 
      && dphi>m_diphotonDPhiCut1 && dphi<m_diphotonDPhiCut2 ) {
    if( npart==1 ) {
      m_isBarrelDiphoton = true;
      m_barrelDiphotonNumber++;
    } else if( npart==0 || npart==2 ) {
      m_isEndcapDiphoton = true;
      m_endcapDiphotonNumber++;
    }
  }

  // -------- Write to root file
  if( m_selectBhabha && m_isBarrelBhabha ) m_subalg1->execute();
  if( m_selectBhabha && m_isEndcapBhabha ) m_subalg2->execute();
  if( m_selectDimu && m_isBarrelDimu ) m_subalg3->execute();
  if( m_selectDimu && m_isEndcapDimu ) m_subalg4->execute();
  if( m_selectHadron && m_isHadron ) m_subalg5->execute();
  if( m_selectDiphoton && m_isBarrelDiphoton ) m_subalg6->execute();
  if( m_selectDiphoton && m_isEndcapDiphoton ) m_subalg7->execute();


  if(m_output) {
    m_runnb = run;
    m_evtnb = event;
    m_esum = esum;
    m_eemc = eemc;
    m_etot = etot;
    m_nCharge = nCharge;
    m_nGood = nGood;
    m_nGam = nGam;
    m_ptot = ptot;
    m_pp = pp;
    m_pm = pm;
    m_maxE = maxE;
    m_secE = secE;
    m_dThe = dthe;
    m_dPhi = dphi;
    m_mdcHit1 = mdcHit1;
    m_mdcHit2 = mdcHit2;
    m_tuple0->write();
  }

  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EventPreSelect::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;

  if(m_selectDimu&&m_output) {
    m_dimuAlg->Print();
  }

  cout<<"Total events: "<<m_events<<endl;

  if(m_selectBhabha) {
    cout << "Selected barrel bhabha: "   << m_barrelBhabhaNumber   << endl;
    cout << "Selected endcap bhabha: "   << m_endcapBhabhaNumber   << endl;
  }

  if(m_selectDimu) {
    delete m_dimuAlg;
    cout << "Selected barrel dimu: "     << m_barrelDimuNumber     << endl;
    cout << "Selected endcap dimu: "     << m_endcapDimuNumber     << endl;
  }

  if(m_selectHadron) {
    cout << "Selected hadron: "   << m_hadronNumber   << endl;
  }

  if(m_selectDiphoton) {
    cout << "Selected barrel diphoton: " << m_barrelDiphotonNumber << endl;
    cout << "Selected endcap diphoton: " << m_endcapDiphotonNumber << endl;
  }

  return StatusCode::SUCCESS;
}

bool EventPreSelect::WhetherSector(double ph,double ph1,double ph2) {
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


