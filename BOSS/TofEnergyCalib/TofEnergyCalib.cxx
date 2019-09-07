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
using CLHEP::Hep3Vector;
#include "CLHEP/Vector/LorentzVector.h"
using CLHEP::HepLorentzVector;
#include "CLHEP/Vector/TwoVector.h"
using CLHEP::Hep2Vector;
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "TofEnergyCalib/TofEnergyCalib.h"
#include "VertexFit/KinematicFit.h"
#include "VertexFit/VertexFit.h"
//#include "AnalEvent/BParticle.h"
//#include "AnalEvent/BTrack.h"
#include "TofRawEvent/TofDigi.h"
#include "Identifier/TofID.h"
#include "RawEvent/RawDataUtil.h"
#include "RawDataProviderSvc/TofData.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "TofQCorrSvc/ITofQCorrSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include <vector>
typedef std::vector<int> Vint;


/////////////////////////////////////////////////////////////////////////////

TofEnergyCalib::TofEnergyCalib(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) {

    m_tuple = 0;
    declareProperty("Event",          m_event = 0);
    declareProperty("IsData",           m_isData = true);
  }

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TofEnergyCalib::initialize(){
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "in initialize()" << endmsg;

  StatusCode status;
  NTuplePtr nt(ntupleSvc(), "FILE1/dimu");
  if ( nt ) m_tuple = nt;
  else {
    m_tuple = ntupleSvc()->book ("FILE1/dimu", CLID_ColumnWiseTuple, "ks N-Tuple example");
    if ( m_tuple )    {
      status = m_tuple->addItem ("npart",  m_npart);
      status = m_tuple->addItem ("number", m_number);
      status = m_tuple->addItem ("adc1",   m_adc1);
      status = m_tuple->addItem ("adc2",   m_adc2);
      status = m_tuple->addItem ("tdc1",   m_tdc1);
      status = m_tuple->addItem ("tdc2",   m_tdc2);
      status = m_tuple->addItem ("zpos",   m_zpos);
      status = m_tuple->addItem ("length", m_length);
      status = m_tuple->addItem ("energy", m_energy);
      status = m_tuple->addItem ("lengthext", m_length_ext);
      status = m_tuple->addItem ("energyext", m_energy_ext);
      status = m_tuple->addItem ("ztdc",   m_ztdc);
      status = m_tuple->addItem ("q",     m_q);
    }
    else    { 
      log << MSG::ERROR << "    Cannot book N-tuple:" << long(m_tuple) << endmsg;
      return StatusCode::FAILURE;
    }
  }
  //
  //--------end of book--------
  //

  log << MSG::INFO << "successfully return from initialize()" <<endmsg;
  return StatusCode::SUCCESS;

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TofEnergyCalib::execute() {

  //if(m_event%1000==0) {
  //  std::cout << "TofEnergyCalib :: " << m_event <<" events executed" << std::endl;
  //}

  //StatusCode sc = StatusCode::SUCCESS;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  int run = eventHeader->runNumber();
  int event = eventHeader->eventNumber();
  if(m_event%1000==0) {
    std::cout << m_event << "-------------TofEnergyCalib run,event: " << run << "," << event << std::endl;
  }
  m_event++;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);
  //  log << MSG::INFO << "get event tag OK" << endreq;
  log << MSG::DEBUG <<"ncharg, nneu, tottks = "
    << evtRecEvent->totalCharged() << " , "
    << evtRecEvent->totalNeutral() << " , "
    << evtRecEvent->totalTracks() <<endreq;

  if(evtRecEvent->totalCharged() != 2) {
    return StatusCode::SUCCESS;
  }

  //Get TOF Raw Data Provider Service
  IRawDataProviderSvc* tofDigiSvc;
  StatusCode sc = service("RawDataProviderSvc", tofDigiSvc);
  if (sc !=  StatusCode::SUCCESS) { 
    log << MSG::ERROR << "TofRec Get Tof Raw Data Service Failed !! " << endreq;
    return StatusCode::SUCCESS;
  }

  //Get TOF Calibtration Service
  ITofCaliSvc* tofCaliSvc;
  StatusCode scc = service("TofCaliSvc", tofCaliSvc);       
  if (scc !=  StatusCode::SUCCESS) {
    log << MSG::ERROR << "TofRec Get Calibration Service Failed !! " << endreq;
    return StatusCode::SUCCESS;
  }
  //tofCaliSvc->Dump();

  ITofQCorrSvc* tofQCorrSvc;
  sc = service("TofQCorrSvc", tofQCorrSvc);       
  if (sc !=  StatusCode::SUCCESS) {
    log << MSG::ERROR << "TofRec Get QCorr Service Failed !! " << endreq;
    return StatusCode::SUCCESS;
  }

  ITofQElecSvc* tofQElecSvc;
  sc = service("TofQElecSvc", tofQElecSvc);       
  if (sc !=  StatusCode::SUCCESS) {
    log << MSG::ERROR << "TofRec Get QElec Service Failed !! " << endreq;
    return StatusCode::SUCCESS;
  }

  // Retrieve Tof Digi Data
  std::vector<TofData*> tofDataVec;
  tofDataVec = tofDigiSvc->tofDataVectorTof();
  const unsigned int ADC_MASK = 0x00001FFF;

  for(int i = 0; i < evtRecEvent->totalCharged(); i++) {
    EvtRecTrackIterator itTrk = evtRecTrkCol->begin() + i;

    if(!(*itTrk)->isExtTrackValid()) continue;
    RecExtTrack *extTrk = (*itTrk)->extTrack();   //get extTrk

    if(!(*itTrk)->isTofTrackValid()) continue;
    //RecTofTrack *tofTrk = (*itTrk)->TofTrk();   //get tofTrk

    int npart, tof1, tof2;           //tof1, tof2 volume number
    Hep3Vector pos1, pos2;    //tof1, tof2 position
    Hep3Vector p1, p2;        //tof1, tof2 momentum;

    tof1 = extTrk->tof1VolumeNumber();
    tof2 = extTrk->tof2VolumeNumber();

    //cout<<"tof1="<<tof1<<"\ttof2="<<tof2<<endl;

    if(tof1>=0 && tof1<88 && tof2>=88 && tof2<176) {   //barrel
      npart = 1;
    } else if(tof1>=176 && tof1<224) {   //west endcap
      tof1 -= 176;
      npart = 2;
    } else if(tof1>=224 && tof1<272) {   //east endcap
      tof1 -= 224;
      npart = 0;
    } else {
      continue;
    }

    pos1 = extTrk->tof1Position();
    pos2 = extTrk->tof2Position();

    p1 = extTrk->tof1Momentum();
    p2 = extTrk->tof2Momentum();

    double zpos1=0, zpos2=0, l1=0, l2=0, lext=0;
    double z1, xy1, z2, xy2;

    if(npart==1) {    //barrel
      //calculate tof direction
      const double tofDPhi = CLHEP::twopi / 88.;
      double tofPhi1 = tof1*tofDPhi + tofDPhi/2;
      double tofPhi2 = (tof2-88)*tofDPhi;

      //retrive ext direction
      double extTheta1, extTheta2, extPhi1, extPhi2;
      extTheta1 = pos1.theta();
      extTheta2 = pos2.theta();
      extPhi1 = pos1.phi();
      extPhi2 = pos2.phi();

      //calculate track length
      //double z1, xy1, z2, xy2;
      z1 = 5/tan(extTheta1);
      xy1 = 5/cos(extPhi1-tofPhi1);
      l1 = sqrt(z1*z1+xy1*xy1);
      z2 = 5/tan(extTheta2);
      xy2 = 5/cos(extPhi2-tofPhi2);
      l2 = sqrt(z2*z2+xy2*xy2);
      zpos1 = (pos1.z()+z1/2)/100;
      zpos2 = (pos2.z()+z2/2)/100;

      //track length in tof from extrapolation
      lext = extTrk->tof2Path()-extTrk->tof1Path();
    }

    else {    //endcap
      //retrive ext direction
      double extTheta1 = pos1.theta();
      //cout<<"extTheta1: "<<extTheta1<<"\t"<<cos(extTheta1)<<endl;

      //calculate track length
      l1 = 5./fabs(cos(extTheta1));
      zpos1 = (sqrt(pos1.x()*pos1.x()+pos1.y()*pos1.y())+5.*tan(extTheta1)/2)/100;
    }

    vector<TofData*>::iterator it;

    //if neighbors of extrapolated scintillator have hits, this event is abandoned
    for(it=tofDataVec.begin();
        it!=tofDataVec.end();
        it++) {

      Identifier idd((*it)->identify());
      int    layer        = TofID::layer(idd);
      int    im           = TofID::phi_module(idd);
      if(im+layer*88==tof1-1 || im+layer*88==tof1+1 ||
          im+layer*88==tof1-2 || im+layer*88==tof1+2) {
        //cout<<"return"<<endl;
        return StatusCode::SUCCESS;
      }
    }

    for(it=tofDataVec.begin();
        it!=tofDataVec.end();
        it++) {

      Identifier idd((*it)->identify());
      int    layer        = TofID::layer(idd);
      int    im           = TofID::phi_module(idd);

      double adc1,adc2,tdc1,tdc2,ztdc,tofq;
      if((*it)->barrel()) {
        TofData* bTofData = *it;
        tdc1 = bTofData->tdc1();
        tdc2 = bTofData->tdc2();
        adc1 = bTofData->adc1();
        adc2 = bTofData->adc2();

        ztdc = tofCaliSvc->ZTDC( tdc1, tdc2, bTofData->tofId() );
        tofq = tofCaliSvc->BPh( adc1, adc2, ztdc, bTofData->tofId());
        if(tofq<100||tofq>10000) continue;

        npart = 1;
        //cout<<"number="<<im+layer*88<<"\tq="<<m_q<<endl;
      } else {
        TofData* eTofData = *it;
        //m_adc0 = (int)(eTofData->adc())&ADC_MASK;
        adc1 = eTofData->adc();
        tdc1 = eTofData->tdc();
        npart = 2;
      }

      //if(!((*it)->barrel()) || ((*it)->barrel() && im+layer*88==tof1) ) 
      if(im+layer*88==tof1) {
        m_adc1 = adc1;
        m_adc2 = adc2;
        m_tdc1 = tdc1;
        m_tdc2 = tdc2;
        m_ztdc = ztdc;
        m_q = tofq;
        m_npart = npart;
        m_number = tof1;
        m_zpos = zpos1;
        m_length = l1;
        m_length_ext = lext;
        //m_energy = l1*8.9/50.;
        m_energy = l1*10.25/5.;
        m_energy_ext = lext*10.25/5.;
        //cout<<"zpos1="<<zpos1<<endl;
        m_tuple->write();
      } else if((*it)->barrel() && im+layer*88 == tof2) {
        m_adc1 = adc1;
        m_adc2 = adc2;
        m_tdc1 = tdc1;
        m_tdc2 = tdc2;
        m_ztdc = ztdc;
        m_q = tofq;
        m_npart = npart;
        m_number = tof2;
        m_zpos = zpos2;
        m_length = l2;
        m_length_ext = lext;
        //m_energy = l2*8.9/50.;
        m_energy = l2*10.25/5.;
        m_energy_ext = lext*10.25/5.;
        //cout<<"zpos2="<<zpos2<<endl;
        m_tuple->write();
      }
    }

  }

  return sc;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode TofEnergyCalib::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endmsg;
  return StatusCode::SUCCESS;
}

