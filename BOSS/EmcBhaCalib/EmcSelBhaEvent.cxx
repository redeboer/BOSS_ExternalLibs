//--------------------------------------------------------------------------
// File and Version Information:
//
// Description:
//	Class EmcSelBhaEvent - Select Bhabha events (MCdata) for Emc-digi 
//      Calibration; read rec-data, read MDC & Emc information from tracklist 
//      and select Bhabha event
//
// Environment:
//	Software developed for the BESIII Detector at the BEPCII.
//
// Author List:
//      Chunxiu Liu
//
// Copyright Information:
//	Copyright (C) 2008               IHEP
//
//------------------------------------------------------------------------


//-----------------------


//-------------
// C Headers --
//-------------
extern "C" {

}
#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <cstdlib>
//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"

#include "EventModel/EventHeader.h"

#include "EvtRecEvent/EvtRecEvent.h"
#include "EvtRecEvent/EvtRecTrack.h"
#include "EmcRawEvent/EmcDigi.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "EmcBhaCalib/EmcSelBhaEvent.h"

#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Random/RandGauss.h"

using namespace std;
using namespace Event;
using CLHEP::Hep3Vector;
using CLHEP::RandGauss;

#include <vector>
#include <list>
typedef std::vector<int> Vint;
typedef std::vector<HepLorentzVector> Vp4;


//----------------
// Constructors --
//----------------
EmcSelBhaEvent::EmcSelBhaEvent(const std::string& name, ISvcLocator* pSvcLocator)
  :Algorithm(name, pSvcLocator),  
   m_vr0cut(1.0),  
   m_vz0cut(5.0),
   m_lowEnergyShowerCut(0.1),
   m_highEnergyShowerCut(0.5),
   m_matchThetaCut(0.2),
   m_matchPhiCut(0.2),
   m_highMomentumCut(0.5),
   m_EoPMaxCut(1.3),
   m_EoPMinCut(0.6),
   m_minAngShEnergyCut(0.2),
   m_minAngCut(0.3),
   m_acolliCut(0.03),
   m_eNormCut(0.5),
   m_pNormCut(0.5),
   m_oneProngMomentumCut(1.2),

   m_digiRangeCut(6),
   m_ShEneThreshCut(0.02),
   m_ShEneLeptonCut(1.),
   m_minNrXtalsShowerCut(10),
   m_maxNrXtalsShowerCut(70),
   m_phiDiffMinCut(0.05),
   m_phiDiffMaxCut(0.2), 
   m_nrShThreshCut(20),
   m_thetaDiffCut(0.04),
   m_LATCut(0.8),

   m_showersAccepted(0),
   //--------------------
   m_writeMVToFile(true),
   m_fileExt(""),
   m_inputFileDir("../InputData/"),
   m_fileDir("/ihepbatch/besdata/public/liucx/Calib/"),
   m_selMethod("Ixtal"),
   m_nXtals(6240),
   m_sigmaCut(1.),
   m_beamEnergy(1.843),
   m_MsgFlag(0)

{
  
  // Declare the properties  


  declareProperty ("Vr0cut", m_vr0cut);   // suggest cut: |z0|<5cm && r0<1cm
  declareProperty ("Vz0cut", m_vz0cut);

  declareProperty ("lowEnergyShowerCut",  m_lowEnergyShowerCut);
  declareProperty ("highEnergyShowerCut",  m_highEnergyShowerCut);
  declareProperty ("matchThetaCut",  m_matchThetaCut);
  declareProperty ("matchPhiCut", m_matchPhiCut);
 
  declareProperty ("highMomentumCut", m_highMomentumCut);
  declareProperty ("EoPMaxCut", m_EoPMaxCut);
  declareProperty ("EoPMinCut", m_EoPMinCut);
  declareProperty ("minAngShEnergyCut", m_minAngShEnergyCut);
  declareProperty ("minAngCut", m_minAngCut);
  declareProperty ("acolliCut", m_acolliCut);
  declareProperty ("eNormCut", m_eNormCut);
  declareProperty ("pNormCut", m_pNormCut);
  declareProperty ("oneProngMomentumCut", m_oneProngMomentumCut);

  // *
  
  declareProperty("digiRangeCut", m_digiRangeCut);

  declareProperty("ShEneThreshCut",  m_ShEneThreshCut);
  declareProperty("ShEneLeptonCut",  m_ShEneLeptonCut);

  declareProperty("minNrXtalsShowerCut",  m_minNrXtalsShowerCut);
  declareProperty("maxNrXtalsShowerCut",  m_maxNrXtalsShowerCut);
  declareProperty("phiDiffMinCut",  m_phiDiffMinCut);
  declareProperty("phiDiffMaxCut",  m_phiDiffMaxCut);  
  declareProperty("nrShThreshCut",  m_nrShThreshCut);
  declareProperty("thetaDiffCut", m_thetaDiffCut);
  declareProperty("LATCut",  m_LATCut);

  //--------------------
  declareProperty("writeMVToFile", m_writeMVToFile);
  declareProperty("fileExt", m_fileExt);
  declareProperty("fileDir", m_fileDir);
  declareProperty("inputFileDir", m_inputFileDir);
  declareProperty("selMethod",m_selMethod);
  declareProperty("sigmaCut", m_sigmaCut);
  declareProperty("ReadBeamEFromDB", m_ReadBeamEFromDB = false );

  declareProperty("beamEnergy", m_beamEnergy);
  declareProperty("elecSaturation", m_elecSaturation = false );

  declareProperty("MsgFlag", m_MsgFlag);

  
  int j = 0;
  m_index = new int*[56];
  for (j=0;j<56;j++ ) {
    m_index[j] = new int[120];
    for ( int k=0; k<120; k++) {
      m_index[j][k]=-1;
    }
  }
   
  m_iGeoSvc=0;

  for (int i=0; i<6240;i++)
    {
      m_inputConst[i] = 1.0;
    }
  
  m_irun=0;
}

//--------------
// Destructor --
//--------------
EmcSelBhaEvent::~EmcSelBhaEvent() {
  
  if ( m_index != 0 ) {
    for (int i =0; i<56; i++ )
      delete[] m_index[i];
    delete[] m_index;
    m_index = 0;
  }
  
  ///////////
  for (int j=0;j<6240;j++ ) {
    m_measure[j]=-1;
  } 
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcSelBhaEvent::initialize(){
  
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  //---------------------------------------<<<<<<<<<<
  m_showersAccepted=0;
  m_events=0;
  m_Nothing=0;
  m_OneProng=0;
  //number of events with   TwoProngMatched
  m_TwoProngMatched=0;
  //number of events with   TwoProngOneMatched
  m_TwoProngOneMatched=0;   

  //--------------------------------------->>>>>>>>>>>
  //initialize Emc geometry to convert between index <=> theta,phi
  initGeom();
 
  //create the object that holds the calibration data
  if ( m_writeMVToFile )
    myCalibData = new EmcBhaCalibData(m_nXtals,m_MsgFlag);
  else
    myCalibData = 0;
  
  //get EmcRecGeoSvc
  
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",m_iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }  

  /* 
  // use EmcCalibConstSvc
  StatusCode scCalib; 
  scCalib = Gaudi::svcLocator() -> service("EmcCalibConstSvc", m_emcCalibConstSvc);
  if( scCalib != StatusCode::SUCCESS){
    log << MSG::ERROR << "can not use EmcCalibConstSvc" << endreq;
  } 
  else {
    std::cout << "Test EmcCalibConstSvc   DigiCalibConst(0)=  " 
  	      << m_emcCalibConstSvc -> getDigiCalibConst(0) << std::endl;
  }
  */

 StatusCode scBeamEnergy; 
  scBeamEnergy = Gaudi::svcLocator() -> service("BeamEnergySvc", m_BeamEnergySvc);

  if( scBeamEnergy != StatusCode::SUCCESS){
    log << MSG::ERROR << "can not use BeamEnergySvc" << endreq;
  } 



  // read correction function from the file of 'cor.conf'
  //from MC Bhabha data,
  // expected depostion energy for bhabha calibration at cms. system
  //it is as a function of thetaID(0-55) 
  readCorFun();
 // read Esigma(Itheta) 
  //from MC Bhabha data,
  //it is as a function of thetaID(0-55)  from the file of 'sigma.conf'
  readEsigma();

  //read peak of bhabha rawdata before bhabha calibration, 
  //it is as a function of thetaID(0-55) from the file of "peakI.conf"
  readDepEneFun();

  //read sigma of bhabha rawdata before bhabha calibration, 
  //it is as a function of thetaID(0-55) from the file of "sigmaI.conf"
  readSigmaExp();
  readRawPeakIxtal();

  /*
  ofstream out;
  out.open("expectedE.conf");
  for(int i=0; i<6240;i++){
    out<<i<<"\t"<<expectedEnergy(i)<<endl;
  }
  out.close();
  */
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcSelBhaEvent::execute() {
  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;   
  
  //create the object that store the needed data of the Bhabha event  
 
  int event, run;
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");

  run=eventHeader->runNumber();
  event=eventHeader->eventNumber();
  //cout<<"---------"<<event<<".........."<<run<<endl;
  m_events++;
  m_run = run;


 //////////////////
  // get beam energy
  //////////////////
  if(m_ReadBeamEFromDB&&m_irun!=run){ 
    m_irun=run;
    m_BeamEnergySvc ->getBeamEnergyInfo();
   if(m_BeamEnergySvc->isRunValid())   
       m_beamEnergy=m_BeamEnergySvc->getbeamE();
   // if(m_readDb.isRunValid(m_irun))   
   //  m_beamEnergy=m_readDb.getbeamE(m_irun);
   //if(m_BeamEnergySvc->isRunValid())   
     //  m_beamEnergy=m_BeamEnergySvc->getbeamE();
    	cout<< "beamEnergy="<< m_beamEnergy<<endl;
    double the=0.011;
    double phi=0;
    HepLorentzVector ptrk;
    ptrk.setPx(m_beamEnergy*sin(the)*cos(phi));
    ptrk.setPy(m_beamEnergy*sin(the)*sin(phi));
    ptrk.setPz(m_beamEnergy*cos(the));
    ptrk.setE(m_beamEnergy);

    ptrk=ptrk.boost(-0.011,0,0);
    
    cout<< "beamEnergy="<< m_beamEnergy<<"  cms "<<ptrk.e()<<"  ratio="<< (m_beamEnergy-ptrk.e())/ptrk.e()<<endl;
    m_beamEnergy=ptrk.e();
  }

  ////////////
  // int mmea=0;

  for (int j=0;j<6240;j++ ) {
    m_measure[j]=-1;
  } 

  if (m_elecSaturation==true)
    {

       ///////////////////////////find Measure/////////////
      SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
      if (!emcDigiCol) {
	log << MSG::FATAL << "Could not find EMC digi" << endreq;
	return( StatusCode::FAILURE);
      }
      
      EmcDigiCol::iterator iter3;
      for (iter3=emcDigiCol->begin();iter3!= emcDigiCol->end();iter3++) {
	RecEmcID id((*(iter3))->identify());
	//cout<<id<<endl;
	
	unsigned int npart,ntheta,nphi;
	npart = EmcID::barrel_ec(id);
	ntheta = EmcID::theta_module(id);
	nphi = EmcID::phi_module(id);
	
	unsigned int  newthetaInd;
	if (npart==0) newthetaInd = ntheta;
	if (npart==1) newthetaInd = ntheta + 6; 
	if (npart==2) newthetaInd = 55 - ntheta;
	
	int ixtal= index(newthetaInd,nphi);
	m_measure[ixtal]=(*iter3)->getMeasure();
	 //if ((*iter3)->getMeasure()==3) mmea=9; 
	
      }
    }

  ////////////
  
  myBhaEvt = new EmcBhabhaEvent();

  //Select Bhabha
  SelectBhabha();
  if(m_selectedType == BhabhaType::OneProng) m_OneProng++;
  //number of events with   TwoProngMatched
  if(m_selectedType == BhabhaType::TwoProngMatched) m_TwoProngMatched++;
  //number of events with   TwoProngOneMatched
  if(m_selectedType == BhabhaType::TwoProngOneMatched) m_TwoProngOneMatched++;
  
  if(m_selectedType == BhabhaType::Nothing){
    m_Nothing++;
  }

  //retreive shower list of event

  if (m_selectedType == BhabhaType::TwoProngMatched) {
     FillBhabha();
  
     //collect bhabha event for digi-calibration of EMC 
     //and fill matrix and vector of system of linear equations
     CollectBhabha();
  }

  delete myBhaEvt;
  myBhaEvt=0;
  
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcSelBhaEvent::finalize() {
  
  MsgStream log(msgSvc(), name());
  
  log << MSG::INFO << "in finalize()" << endreq;

  //output the data of Matrix and vector to files
  OutputMV();
  /* 
  for (int i=1000; i < 1500; i++) {
	int xtalIndex=myCalibData->xtalInd(i);

	int nhitxtal=myCalibData->xtalHitsDir(xtalIndex);
	cout<<"ixtal, Nhitdir="<< xtalIndex<<"   "<<nhitxtal<<endl;
  
  }
  */
  if ( m_writeMVToFile ) {
    delete myCalibData;
    myCalibData = 0;
  }

  cout<<"Event="<<m_events<<endl;

  cout<<"m_Nothing ="<<m_Nothing <<endl;
  cout<<"m_OneProng="<<m_OneProng<<endl;
  
  cout<<"m_TwoProngMatched="<<m_TwoProngMatched<<endl;
  
  cout<<"m_TwoProngOneMatched="<<m_TwoProngOneMatched<<endl;

  cout<<"m_showersAccepted="<<m_showersAccepted<<endl;

  return StatusCode::SUCCESS;
}



// * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

double
EmcSelBhaEvent::expectedEnergy( long int ixtal ) {
  
 EmcStructure* theEmcStruc=new EmcStructure() ;
  theEmcStruc->setEmcStruc();

  unsigned int module, ntheta, nphi,ThetaIndex;
     
  module=theEmcStruc->getPartId(ixtal);
  ntheta=theEmcStruc->getTheta(ixtal);
  nphi=theEmcStruc->getPhi(ixtal);

  if (module==0) ThetaIndex = ntheta;
  if (module==1) ThetaIndex = ntheta + 6;
  if (module==2) ThetaIndex = 55 - ntheta;

  RecEmcID shId= EmcID::crystal_id(module,ntheta,nphi);
  HepPoint3D SeedPos(0,0,0);
  SeedPos=m_iGeoSvc->GetCFrontCenter(shId);
  
  
  double theta=SeedPos.theta();
  double phi=SeedPos.phi();
  HepLorentzVector ptrk;
  ptrk.setPx(m_beamEnergy*sin(theta)*cos(phi));
  ptrk.setPy(m_beamEnergy*sin(theta)*sin(phi));
  ptrk.setPz(m_beamEnergy*cos(theta));
  ptrk.setE(m_beamEnergy);
  
  ptrk=ptrk.boost(0.011,0,0);
  
  double depoEne_lab  = m_corFun[ThetaIndex]*ptrk.e();
  
  return depoEne_lab;
}


// * * * * * * * * * * * * 

StatusCode EmcSelBhaEvent::SelectBhabha(){
  
  MsgStream log(msgSvc(), name());
  
  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);

  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

  m_selectedType = BhabhaType::Nothing;
  m_events++;

  /*
  Vint iGood;
  iGood.clear();
  int nCharge = 0;
  int numberOfTrack = 0;

  //the accumulated event information
  double totalEnergy = 0.; 
  int numberOfShowers = 0; 
  int numberOfShowersWithTrack = 0;
  bool secondUnMTrackFound=false;
  float eNorm = 0.;
  float pNorm = 0.;
  float acolli_min = 999.;
  double minAngFirstSh = 999., minAngSecondSh = 999.;
  float theFirstTracksTheta = 0;
  float theFirstTracksMomentum = 0;

  //the selection candidates
  RecMdcTrack *theFirstTrack = 0;
  RecMdcTrack *theSecondTrack = 0;
  RecMdcTrack *theKeptTrack = 0;
  RecEmcShower *theFirstShower = 0; 

  RecEmcShower *theSecondShower = 0;
  RecEmcShower *theKeptShower = 0;
  double keptTheta = 0.;
  int theFirstShID = 9999;
  int theSecondShID = 9999;
  int theKeptShID = 9999;
  int theTrkID = 9999;
  */

   Vint iGam;
  iGam.clear(); 

  //double ene5x5,theta,phi,eseed;
  //double showerX,showerY,showerZ;
  //long int thetaIndex,phiIndex;
  //HepPoint3D showerPosition(0,0,0);
  // RecEmcID showerId;
  //unsigned int npart;

  for(int i = 0; i < evtRecEvent->totalTracks(); i++){
 
    if(i>=evtRecTrkCol->size()) break;
    
    EvtRecTrackIterator itTrk=evtRecTrkCol->begin() + i;
    
    if((*itTrk)->isEmcShowerValid()) {  
      

      RecEmcShower *theShower = (*itTrk)->emcShower();
      int TrkID = (*itTrk)->trackId();
      double Shower5x5=theShower->e5x5();
      //cout<<"Shower5x5="<<Shower5x5<<endl;
      /*
      ene5x5=theShower->e5x5();   //uncorrected 5x5 energy unit GeV
      eseed=theShower->eSeed(); //unit GeV
      
      showerPosition = theShower->position();
      theta = theShower->theta();
      phi= theShower->phi();
      showerX=theShower->x();
      showerY=theShower->y();
      showerZ=theShower->z();
      
      showerId = theShower->getShowerId();
 
      npart = EmcID::barrel_ec(showerId);
      
      //The theta number is defined by Endcap_east(0-5),Barrel(0-43),Endcap_west(0-5) 
      //module is defined  by Endcap_east(0),Barrel(1),Endcap_west(2)      
      
      thetaIndex=EmcID::theta_module(showerId);
      
      phiIndex=EmcID::phi_module(showerId);  
      */
       
       if (Shower5x5>1.1){
	
	iGam.push_back( TrkID );  
	

      }
      
    }
  }
  int nGam = iGam.size();
  log << MSG::DEBUG << "num Good Photon " << nGam  << " , " <<evtRecEvent->totalNeutral() <<endreq;  


  if (nGam==2) {  
    
    m_selectedType = BhabhaType::TwoProngMatched;
    m_selectedTrkID1 =iGam[0];
    m_selectedTrkID2 = iGam[1];
   
  }


 return StatusCode::SUCCESS;
  
}


// * * * * * * * * * * * * 
void 
EmcSelBhaEvent::FillBhabha(){
  
  MsgStream log(msgSvc(), name());

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

  EvtRecTrackIterator  itTrk1 = evtRecTrkCol->begin() + m_selectedTrkID1;

  RecEmcShower *theShower1 = (*itTrk1)->emcShower();
  //RecMdcTrack *theMdcTrk1 = (*itTrk1)->mdcTrack();

  EvtRecTrackIterator  itTrk2 = evtRecTrkCol->begin() + m_selectedTrkID2;

  RecEmcShower *theShower2 = (*itTrk2)->emcShower();
  //RecMdcTrack *theMdcTrk2 = (*itTrk2)->mdcTrack();
  EvtRecTrackIterator  itTrk;
  RecEmcShower *theShower;


  // * * * * * * * * * * * * * * * * * * * * * * * * * ** *  ** *
  //loop all showers of an event set EmcShower and EmcShDigi
  
  EmcShower* aShower =new EmcShower();

  double ene,theta,phi,eseed;
  double showerX,showerY,showerZ;
  long int thetaIndex,phiIndex,numberOfDigis;
  
  RecEmcID showerId;
  unsigned int showerModule;
  
  HepPoint3D showerPosition(0,0,0);
  
  if ( ! m_showerList.empty())  m_showerList.clear(); 
  
  for (int ish=0; ish<2; ish++){

    if (ish==0) {
      itTrk= itTrk1;
      theShower=theShower1;
    }
    if (ish==1) {
      itTrk= itTrk2;
      theShower=theShower2;
    }
    //ene=theShower->energy(); //corrected shower energy unit GeV
    ene=theShower->e5x5();   //uncorrected 5x5 energy unit GeV
    eseed=theShower->eSeed(); //unit GeV
 

    /////////////////
    /*
   RecExtTrack *extTrk = (*itTrk)->extTrack();

      Hep3Vector extpos = extTrk->emcPosition();	
 
      cout<<"Extpos="<<extpos<<endl;


      RecEmcShower *theShower = (*itTrk)->emcShower();

      Hep3Vector emcpos(theShower->x(), theShower->y(), theShower->z());

      cout<<"emcpos="<<emcpos<<endl;
    
      RecEmcID shId= theShower->getShowerId();
      unsigned int nprt= EmcID::barrel_ec(shId);
      //RecEmcID cellId= theShower->cellId();
      HepPoint3D SeedPos(0,0,0);
      SeedPos=m_iGeoSvc->GetCFrontCenter(shId);
      cout<<"SeedPos="<<SeedPos<<endl;
    */
      //////////////////

    showerPosition = theShower->position();
    theta = theShower->theta();
    phi= theShower->phi();
    showerX=theShower->x();
    showerY=theShower->y();
    showerZ=theShower->z();
    
    showerId = theShower->getShowerId();
    showerModule = EmcID::barrel_ec(showerId);
    
    //The theta number is defined by Endcap_east(0-5),Barrel(0-43),Endcap_west(0-5) 
    //module is defined  by Endcap_east(0),Barrel(1),Endcap_west(2)      

    thetaIndex=EmcID::theta_module(showerId);
    
    phiIndex=EmcID::phi_module(showerId);    

    //cout<<thetaIndex<<"   "<<phiIndex<<endl;
    //-------------------

    EmcShDigi* aShDigi= new EmcShDigi();
    EmcShDigi maxima =EmcShDigi();
    list<EmcShDigi> shDigiList;
    RecEmcID cellId;
    unsigned int module;
    double digiEne, time, fraction, digiTheta, digiPhi;
    double  digiX, digiY, digiZ;
    long int digiThetaIndex,digiPhiIndex;  
    HepPoint3D digiPos(0,0,0);
    
    RecEmcFractionMap::const_iterator ciFractionMap;
    
    if ( ! shDigiList.empty())  shDigiList.clear();
    RecEmcFractionMap fracMap5x5=theShower->getFractionMap5x5();
    for(ciFractionMap=fracMap5x5.begin();
	ciFractionMap!=fracMap5x5.end();	
	ciFractionMap++){
      
      digiEne = ciFractionMap->second.getEnergy();  //digit energy unit GeV

      time = ciFractionMap->second.getTime();
      fraction = ciFractionMap->second.getFraction();
      
      cellId=ciFractionMap->second.getCellId();
      
      digiPos=m_iGeoSvc->GetCFrontCenter(cellId);
      digiTheta = digiPos.theta();
      digiPhi = digiPos.phi();   
      digiX = digiPos.x();
      digiY = digiPos.y();
      digiZ = digiPos.z();
      
      module=EmcID::barrel_ec(cellId);
      //The theta number is defined by Endcap_east(0-5),Barrel(0-43),Endcap_west(0-5) 
      //module is defined  by Endcap_east(0),Barrel(1),Endcap_west(2) 

      digiThetaIndex=EmcID::theta_module(cellId);
      
      digiPhiIndex = EmcID::phi_module(cellId);

      //set EmcShDigi
      aShDigi->setEnergy(digiEne);
      aShDigi->setTheta(digiTheta);
      aShDigi->setPhi(digiPhi);
      aShDigi->setModule(module);
      aShDigi->setThetaIndex(digiThetaIndex);
      aShDigi->setPhiIndex(digiPhiIndex);
      aShDigi->setTime(time);
      aShDigi->setFraction(fraction);
      aShDigi->setWhere(digiPos);
      aShDigi->setY(digiX);
      aShDigi->setY(digiY);
      aShDigi->setZ(digiZ);
      
      shDigiList.push_back(*aShDigi);
      
    }
    shDigiList.sort();          //sort energy from small to large
         
    numberOfDigis = shDigiList.size();
    
    maxima = *(--shDigiList.end());
    //cout<<"maxima="<<maxima.energy()<<endl;
    //cout<<maxima.thetaIndex()<<"  max  "<<maxima.phiIndex()<<endl;
    //set EmcShower
    aShower->setEnergy(ene);
    aShower->setTheta(theta);
    aShower->setPhi(phi);
    aShower->setModule(showerModule);
    aShower->setThetaIndex(thetaIndex);
    aShower->setPhiIndex(phiIndex);
    aShower->setNumberOfDigis(numberOfDigis);
    aShower->setDigiList(shDigiList);
    aShower->setMaxima(maxima);
    aShower->setWhere(showerPosition);
    aShower->setX(showerX);
    aShower->setY(showerY);
    aShower->setZ(showerZ);
    m_showerList.push_back(*aShower);
    delete aShDigi;   

  }
  //m_showerList.sort();         //sort energy from small to large
  
  delete aShower;

  ///////////////

  if (m_showerList.size() == 2) {
    //iterator for the bump list
    list<EmcShower>::const_iterator iter_Sh;
    int itrk=0;
    for (iter_Sh  = m_showerList.begin(); 
	 iter_Sh != m_showerList.end();  iter_Sh++) {
      
      itrk++;   
      EmcShower shower;
      shower = EmcShower(); 
      double theta =  iter_Sh->theta();
      shower = *iter_Sh;
         
      //fill the Bhabha event
      // if ( (itrk==1&&theMdcTrk1->charge()>0)
      //   ||(itrk==2&&theMdcTrk2->charge()>0) ){
      if (itrk==1){
	//set properties 
	myBhaEvt->setPositron()->setShower(shower);
	myBhaEvt->setPositron()->setTheta(shower.theta());
	myBhaEvt->setPositron()->setPhi(shower.phi());
	unsigned int newthetaInd ;
	//The thetaIndex is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
	//in Emc Bhabha Calibration
	if (shower.module()==0) newthetaInd = shower.thetaIndex();
	if (shower.module()==1) newthetaInd = shower.thetaIndex() + 6;
	if (shower.module()==2) newthetaInd = 55 - shower.thetaIndex();
	
	myBhaEvt->setPositron()->setThetaIndex(newthetaInd);

	unsigned int  newphiInd=shower.phiIndex();
	myBhaEvt->setPositron()->setPhiIndex(newphiInd);
	myBhaEvt->setPositron()->setFound(true); 


	log << MSG::INFO <<  name() << ": Positron: theta,phi energy "
	    << myBhaEvt->positron()->theta() << ","
	    << myBhaEvt->positron()->shower().phi() << " "
	    << myBhaEvt->positron()->shower().energy()
	    << endreq;

     
	} 
   
	//if ( (itrk==1&&theMdcTrk1->charge()<0)
	//  ||(itrk==2&&theMdcTrk2->charge()<0) ){
    if (itrk==2){
	//set properties including vertex corrected theta 
      myBhaEvt->setElectron()->setShower(shower);
	myBhaEvt->setElectron()->setTheta(shower.theta());
	myBhaEvt->setElectron()->setPhi(shower.phi());	  
	unsigned int newthetaInd;	
	//The thetaIndex is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
      //in Emc Bhabha Calibration
	if (shower.module()==0) newthetaInd = shower.thetaIndex();
	if (shower.module()==1) newthetaInd = shower.thetaIndex() + 6;
	if (shower.module()==2) newthetaInd = 55 - shower.thetaIndex();
      
	myBhaEvt->setElectron()->setThetaIndex(newthetaInd);
	unsigned int newphiInd=shower.phiIndex();
	myBhaEvt->setElectron()->setPhiIndex(newphiInd);	
	myBhaEvt->setElectron()->setFound(true); 
	
	log << MSG::INFO << name() << ": Electron: theta,phi energy "
	    << myBhaEvt->electron()->theta() << ","
	    << myBhaEvt->electron()->shower().phi() << " "
	    << myBhaEvt->electron()->shower().energy()
	    << endreq;

      } 
    }
  } 


}

void
EmcSelBhaEvent::CollectBhabha(){
  
  MsgStream log(msgSvc(), name());

  //check if the Bhabhas were found
  if ( 0 != myBhaEvt->positron()->found() ||
       0 != myBhaEvt->electron()->found() ) {
    
    m_taken++;
    //fill the EmcBhabhas 
    double calibEnergy=0.; 
    double energyError=0.;
    
    //------------- electron found --------------------------------------
    if (myBhaEvt->electron()->found() ) {
      /*
      int ixtalIndex = index(myBhaEvt->electron()->thetaIndex(),
			    myBhaEvt->electron()->phiIndex());
      calibEnergy = m_eMcPeak[ixtalIndex];
      */
            
      unsigned int module, ntheta, nphi;
      if ( myBhaEvt->electron()->thetaIndex()<=5) {
	module=0;
	ntheta=myBhaEvt->electron()->thetaIndex();

      } else if ( myBhaEvt->electron()->thetaIndex()>=50){
	module=2;
	ntheta=55-myBhaEvt->electron()->thetaIndex();

      } else {
	module=1;
	ntheta=myBhaEvt->electron()->thetaIndex()-6;
      }
      nphi=myBhaEvt->electron()->phiIndex();


      RecEmcID shId= EmcID::crystal_id(module,ntheta,nphi);
      HepPoint3D SeedPos(0,0,0);
      SeedPos=m_iGeoSvc->GetCFrontCenter(shId);
      /*
      calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy_lab(myBhaEvt->electron()->theta(),
				  myBhaEvt->electron()->phi(),
				  myBhaEvt->electron()->thetaIndex(),
				  myBhaEvt->electron()->phiIndex(),
				  m_corFun[myBhaEvt->electron()->thetaIndex()],
      				  m_beamEnergy); 
      */
      calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy_lab(SeedPos.theta(),
				  SeedPos.phi(),
				  myBhaEvt->electron()->thetaIndex(),
				  myBhaEvt->electron()->phiIndex(),
				  m_corFun[myBhaEvt->electron()->thetaIndex()],
      				  m_beamEnergy); 
      
     /*				   				  
     calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy(myBhaEvt->electron()->thetaIndex(),
			      myBhaEvt->electron()->phiIndex(),
			      m_corFun[myBhaEvt->electron()->thetaIndex()],
			      m_beamEnergy); 
      */

      if ( calibEnergy > 0 ) {
	
	energyError = myBhaEvt->
	  getErrorDepoMCShowerEnergy(myBhaEvt->electron()->thetaIndex(),
				     myBhaEvt->electron()->phiIndex(),
				     m_eSigma[myBhaEvt->electron()->thetaIndex()]);

     } else
	log << MSG::WARNING << " Did not find MC deposited cluster energy "
	    <<" for this cluster:  thetaInd: " 
	    << myBhaEvt->electron()->thetaIndex()
	    << "  phiInd: " 
	    << myBhaEvt->electron()->phiIndex()
	    << endl
	    << "Will not use this cluster for the Emc "
	    << "Bhabha calibration !"
	    << endreq;
      
      //set all that stuff in an EmcBhabha
      myBhaEvt->setElectron()->setErrorOnCalibEnergy(energyError);
      myBhaEvt->setElectron()->setCalibEnergy(calibEnergy);
    
      //myBhaEvt->electron()->print();

    } else
      log << MSG::INFO<< name()
	  << ": Electron was not selected ! "
	  << endreq;
    
    calibEnergy=0.; 
    energyError=0.;
    
    //---------------- positron found ----------------------------------
    if (myBhaEvt->positron()->found() ) {
      /*
      int ixtalIndex = index(myBhaEvt->positron()->thetaIndex(),
			    myBhaEvt->positron()->phiIndex());
      calibEnergy = m_eMcPeak[ixtalIndex];
      */
       
      unsigned int module, ntheta, nphi;
      if ( myBhaEvt->positron()->thetaIndex()<=5) {
	module=0;
	ntheta=myBhaEvt->positron()->thetaIndex();

      } else if ( myBhaEvt->positron()->thetaIndex()>=50){
	module=2;
	ntheta=55-myBhaEvt->positron()->thetaIndex();

      } else {
	module=1;
	ntheta=myBhaEvt->positron()->thetaIndex()-6;
      }
	nphi=myBhaEvt->positron()->phiIndex();


      RecEmcID shId= EmcID::crystal_id(module,ntheta,nphi);
      HepPoint3D SeedPos(0,0,0);
      SeedPos=m_iGeoSvc->GetCFrontCenter(shId);
      /*
     calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy_lab(myBhaEvt->positron()->theta(),
				  myBhaEvt->positron()->phi(),
				  myBhaEvt->positron()->thetaIndex(),
				  myBhaEvt->positron()->phiIndex(),
				  m_corFun[myBhaEvt->positron()->thetaIndex()],
      				  m_beamEnergy); 
      */
      calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy_lab(SeedPos.theta(),
				  SeedPos.phi(),
				  myBhaEvt->positron()->thetaIndex(),
				  myBhaEvt->positron()->phiIndex(),
				  m_corFun[myBhaEvt->positron()->thetaIndex()],
      				  m_beamEnergy); 
     
      /*
      calibEnergy = myBhaEvt->
	getDepoMCShowerEnergy(myBhaEvt->positron()->thetaIndex(),
			      myBhaEvt->positron()->phiIndex(),
			      m_corFun[myBhaEvt->positron()->thetaIndex()],
			      m_beamEnergy); 
      */
		        
      if ( calibEnergy > 0. ) {
		
	energyError = myBhaEvt->
	  getErrorDepoMCShowerEnergy(myBhaEvt->positron()->thetaIndex(),
				     myBhaEvt->positron()->phiIndex(),
				     m_eSigma[myBhaEvt->positron()->thetaIndex()]);
			
      } else 
	log << MSG::WARNING << " Did not find MC deposited cluster energy "
	    << "for this cluster:  thetaInd: " 
	    << myBhaEvt->positron()->thetaIndex()
	    << "  phiInd: " 
	    << myBhaEvt->positron()->phiIndex()
	    << endl
	    << "Will not use this cluster for the Emc "
	    << "Bhabha calibration !"
	    << endreq;
      
      
      //set all that stuff in an EmcBhabha
      myBhaEvt->setPositron()->setErrorOnCalibEnergy(energyError);
      myBhaEvt->setPositron()->setCalibEnergy(calibEnergy);
  
      //myBhaEvt->positron()->print();
      
    } else
      log << MSG::INFO << name()
	  << ": Positron was not selected ! "
	  << endreq;
    
    //calculate elements of Matrix M and vector R from Bhabha event, 
    //M is in SLAP triad format
    fillMatrix();
    
  } else {
    log << MSG::WARNING << " No Bhabha data for calibration found in event !" 
	<< endreq;
   
  }
  
}


void
EmcSelBhaEvent::fillMatrix( ) {
  
  EmcBhabha _theBhabha=EmcBhabha();
  EmcShower _theShower=EmcShower();
  EmcShDigi _DigiMax=EmcShDigi();
  double _sigmaBha;
  
  // ---- get the current calibration constants
  
  
  // ---- loop over the two particles
  for ( int i = 1; i <= 2; i++ ) { 
    
    if ( i == 2 ) _theBhabha = *(myBhaEvt->electron());	
    else _theBhabha = *(myBhaEvt->positron());	
    //-----------------------------------------------------------
    // ---- fill the matrix only if we found the particle and ---
    // ---- a energy to calibrate on -----
    double _sig=_theBhabha.errorOnCalibEnergy();
    double _calibEne=_theBhabha.calibEnergy();
    double _bhaEne=_theBhabha.shower().energy();
    int  _bhaThetaIndex=_theBhabha.thetaIndex();
    int  _bhaPhiIndex=_theBhabha.phiIndex();
    //cout<<_sig<<"  "<< _calibEne<<"  "<<_bhaEne<<"  "<<endl;
    ///////////////boost to cms 
    double eraw =_bhaEne ;
    double phi = _theBhabha.shower().phi();
    double the = _theBhabha.shower().theta();

    HepLorentzVector ptrk;
    ptrk.setPx(eraw*sin(the)*cos(phi));
    ptrk.setPy(eraw*sin(the)*sin(phi));
    ptrk.setPz(eraw*cos(the));
    ptrk.setE(eraw);

    ptrk=ptrk.boost(-0.011,0,0);
    _bhaEne=ptrk.e();
    //////////////

    double SigCut=0.0;

    SigCut=m_sigmaCut;

    int ixtalIndex = index(_bhaThetaIndex,_bhaPhiIndex);
    double peakCutMin,peakCutMax;
         
    //peak+/- 1 sigma
    if (m_selMethod=="Ithe"){
      peakCutMin= m_eDepEne[_bhaThetaIndex]*m_beamEnergy 
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
      
      peakCutMax=m_eDepEne[_bhaThetaIndex] *m_beamEnergy  
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }
   /*
     if (ixtalIndex==2408){
      peakCutMin= 1.685 
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;

      peakCutMax=1.685 
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }
    */

    //Raw mean+/- 1 RMS
    // peakCutMin= m_eRawMean[ixtalIndex] - m_eRawRMS[ixtalIndex]*SigCut;

    //peakCutMax= m_eRawMean[ixtalIndex] + m_eRawRMS[ixtalIndex]*SigCut;

    // cout<< ixtalIndex<<"  "<<peakCutMin<<"   " <<peakCutMax<<endl;

      
    //peak(ixtal)+/- 1 sigma
    if (m_selMethod=="Ixtal"){
      peakCutMin=  m_eRawPeak[ixtalIndex] *m_beamEnergy  
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
      
      peakCutMax= m_eRawPeak[ixtalIndex] *m_beamEnergy 
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }  

//  if (ixtalIndex==689) cout<<peakCutMin<<"\t"<<peakCutMax<<"\t"<<_bhaEne<<endl;
  /* 
    if (ixtalIndex==2408){
      peakCutMin= 1.685 
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
      
      peakCutMax=1.685 
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;   
    }
    */
    /*
    
    if (_bhaThetaIndex==26&&_bhaPhiIndex==26){
      peakCutMin= 1.75 
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;

      peakCutMax=1.75 
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }
    
    if (_bhaThetaIndex==29&&_bhaPhiIndex==26){
      peakCutMin= 1.9
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;

      peakCutMax=1.9
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }

    if (_bhaThetaIndex==30&&_bhaPhiIndex==26){
      peakCutMin= 1.655
	- SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;

      peakCutMax=1.655 
	+ SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.;
    }
    */

    if ( _theBhabha.found()!=0 && _theBhabha.calibEnergy() > 0. 
	 && _bhaEne >= peakCutMin
	 && _bhaEne <= peakCutMax 
	 && m_measure[ixtalIndex] !=3 ) {


    //if ( _theBhabha.found()!=0 && _theBhabha.calibEnergy() > 0. 
    //	  && _bhaEne >= m_eDepMean[ixtalIndex] 
    //  -  SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.
    //  &&_bhaEne <= m_eDepMean[ixtalIndex]
    //  + SigCut*m_eSigmaExp[_bhaThetaIndex]*m_beamEnergy/100.) {

    // if ( _theBhabha.found()!=0 && _theBhabha.calibEnergy() > 0. 
    //	  && _bhaEne >= m_eDepPeak[ixtalIndex] 
    //  -  SigCut*m_eDepSigma[ixtalIndex]
    // &&_bhaEne <= m_eDepPeak[ixtalIndex]
    //+ SigCut*m_eDepSigma[ixtalIndex]) {

       m_showersAccepted++;
      //the error (measurement + error on the energy to calibrate on)
      _sigmaBha = _theBhabha.sigma2();
      //cout<<"sigma2="<<_sigmaBha<<endl;
      
      _theShower = _theBhabha.shower();

      //The thetaIndex is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
      //in Emc Bhabha Calibration
      
      _DigiMax = _theShower.maxima();
      
      unsigned int newThetaIndMax=999;
      if (_DigiMax.module()==0) newThetaIndMax = _DigiMax.thetaIndex();
      if (_DigiMax.module()==1) newThetaIndMax = _DigiMax.thetaIndex() + 6;
      if (_DigiMax.module()==2) newThetaIndMax = 55 - _DigiMax.thetaIndex();
 
      //     if(_DigiMax.module()==1)
      //	{  // only for MC data
      //  cout<<"theta..,phi="<<_DigiMax.thetaIndex()<<"   "<<_DigiMax.phiIndex()<<endl;
      //}

      int maximaIndex=0;
      maximaIndex = index(newThetaIndMax,_DigiMax.phiIndex());
      //if (maximaIndex>500&&maximaIndex<5000){
      //	cout<<"max="<<maximaIndex<<"   "<<_DigiMax.thetaIndex()
      //<<"   "<<_DigiMax.phiIndex()<<endl;
      //}

      list<EmcShDigi> _DigiList=_theShower.digiList();
      
      list<EmcShDigi>::const_iterator _Digi1,_Digi2;
      
      //------------------------------------------------------
      //---- double loop over the digis to fill the matrix ---
      
      //first loop over Digis 
      for (_Digi1 = _DigiList.begin(); 
	   _Digi1 != _DigiList.end();  _Digi1++) {
 	
	//get Xtal index 
	
	//The thetaIndex is defined by Endcap_east(0-5),Barrel(6-49),Endcap_west(50-55)
	//in Emc Bhabha Calibration
	unsigned int newThetaInd1=999;
	if (_Digi1->module()==0) newThetaInd1 = _Digi1->thetaIndex();
	if (_Digi1->module()==1) newThetaInd1 = _Digi1->thetaIndex() + 6;
	if (_Digi1->module()==2) newThetaInd1 = 55 - _Digi1->thetaIndex();
	
	int Digi1Index = index(newThetaInd1,_Digi1->phiIndex());

	//if (Digi1Index>1000&&Digi1Index<5000){
	// cout<<"max="<<Digi1Index<<"   "<<_Digi1->thetaIndex()
	//     <<"   "<<_Digi1->phiIndex()<<endl;
	//}


	//calculate the vector with Raw data
	double dvec = ( (static_cast<double>(_Digi1->energy())) *
			_theBhabha.calibEnergy() / _sigmaBha);

	//fill the vector 
	if ( m_writeMVToFile )
	  (myCalibData->vectorR(Digi1Index)) += dvec;
	
	//count hits in Xtals and keep theta and phi
	if ( m_writeMVToFile )

	  (myCalibData->xtalHits(Digi1Index))++;
	
	//if ( *(_theShower.maxima()) == *(_Digi1) ) {

	if ( Digi1Index == maximaIndex ) {
	  if ( m_writeMVToFile ){
	     (myCalibData->xtalHitsDir(Digi1Index))++;

	    }
	}
	
	//second loop over Digis 
	for (_Digi2 = _Digi1; 
	     _Digi2 != _DigiList.end();  _Digi2++) {

	  unsigned int newThetaInd2=999;
	  if (_Digi2->module()==0) newThetaInd2 = _Digi2->thetaIndex();
	  if (_Digi2->module()==1) newThetaInd2 = _Digi2->thetaIndex() + 6;
	  if (_Digi2->module()==2) newThetaInd2 = 55 - _Digi2->thetaIndex();	  
	  
	  int Digi2Index = index(newThetaInd2, _Digi2->phiIndex());

	  //calculate the matrix element with raw data
	  double val = 
	    static_cast<double>((( (_Digi1->energy()  * 
				  _Digi2->energy() )
				   ))/_sigmaBha);
	
	  if ( m_writeMVToFile )
	    myCalibData->matrixMEle( Digi1Index, Digi2Index) += val;
	  
	}
      }
      
    } //if paricle found and calibration energy
    
  }//loop over particles
  
}


void
EmcSelBhaEvent::initGeom() {
  
  MsgStream log(msgSvc(), name());
  
  int numberOfXtalsRing;
  
  EmcStructure* theEmcStruc=new EmcStructure() ;
  theEmcStruc->setEmcStruc();

  //number Of Theta Rings
  int nrOfTheRings = theEmcStruc->getNumberOfTheRings();
  
  m_nXtals = theEmcStruc->getNumberOfXtals();  

  //init geometry
  for ( int the = theEmcStruc->startingTheta(); the< nrOfTheRings; the++) {
    
    numberOfXtalsRing = theEmcStruc->crystalsInRing((unsigned int) the );
    
    for ( int phi=0; phi < numberOfXtalsRing; phi++) {
      
      m_index[the][phi] = theEmcStruc->getIndex( (unsigned int)the , (unsigned int)phi);
    
    }

  }
  
  log << MSG::INFO <<  " Emc geometry for Bhabha calibration initialized !! " 
      << endl
      << "Number of Xtals: " << m_nXtals << endreq;
  delete theEmcStruc;
  


}


void 
EmcSelBhaEvent::OutputMV(){
  
  MsgStream log(msgSvc(), name());
  
  //check this first because I sometimes got two endJob transitions
  if ( myCalibData != 0 )

    //if set write the matrix and vector to a file
    if ( m_writeMVToFile ) {
      
      int count;
      char cnum[10];
      if (m_run<0){
	count = sprintf(cnum,"MC%d",-m_run);
      }
      if (m_run>=0){
	count = sprintf(cnum,"%d",m_run);
      }

      std::string runnum="";
      runnum.append(cnum);

      ofstream runnumberOut;
      std::string runnumberFile = m_fileDir;
      runnumberFile += m_fileExt;
      runnumberFile +="runnumbers.dat";
      runnumberOut.open(runnumberFile.c_str(),ios::out|ios::app);

      ifstream runnumberIn;
      runnumberIn.open(runnumberFile.c_str());     
      bool status=false;
      while( !runnumberIn.eof() ) {
	
	std::string num;
	runnumberIn >> num;
	if (runnum==num) {
	  status=true;
	  log << MSG::INFO<< " the runnumber: "<<runnum
	      <<" exists in the file runnumbers.dat" <<endreq;
	  break;
	}else{
	  status=false;
	  log << MSG::INFO<< " the runnumber: "<<runnum
	      <<" does not exist in the file runnumbers.dat" <<endreq;	 
	}
      }
      runnumberIn.close();

      ofstream vectorOut;
      std::string vectorFile = m_fileDir;
      vectorFile += m_fileExt;
      vectorFile += runnum; 
      vectorFile += "CalibVector.dat";
      vectorOut.open(vectorFile.c_str());
      
      ofstream matrixOut;
      std::string matrixFile = m_fileDir;
      matrixFile += m_fileExt;
      matrixFile += runnum; 
      matrixFile += "CalibMatrix.dat";
      matrixOut.open(matrixFile.c_str());
      
      if ( vectorOut.good() && matrixOut.good() &&runnumberOut.good()) {
	
	myCalibData->writeOut(matrixOut, vectorOut);

	log << MSG::INFO<< " Wrote files " 
	    << (vectorFile) << " and " 
	    << (matrixFile) <<endreq;
	

	if ( !status ) {
	  runnumberOut<<runnum<<"\n";
	  log << MSG::INFO<< "Wrote files "<<runnumberFile<< endreq;
	}

      } else
	log << MSG::WARNING << " Could not open vector and/or matrix file !"
	    << endl
	    << "matrix file : " << matrixFile << endl
	    << "vector file : " << vectorFile
	    << endreq;
      
      vectorOut.close();
      matrixOut.close();
      runnumberOut.close();
    }
  
}


double
EmcSelBhaEvent::findPhiDiff( double phi1, double phi2)
{
  double diff;
  diff = phi1 - phi2;  //rad

  while( diff>  pi  ) diff -= twopi;
  while( diff< -pi  ) diff += twopi;
  
  return  diff;
}


void 
EmcSelBhaEvent::readCorFun(){

  ifstream corFunIn;
  std::string corFunFile = m_inputFileDir;
  corFunFile += m_fileExt;
  corFunFile += "cor.conf";
  corFunIn.open(corFunFile.c_str());
  for(int i=0;i<56;i++) {
    corFunIn>>m_corFun[i];

    cout<<"energy corFun="<<m_corFun[i]<<endl;
  }
  corFunIn.close();
}

void 
EmcSelBhaEvent::readEsigma(){

  ifstream EsigmaIn;
  std::string EsigmaFile = m_inputFileDir;
  EsigmaFile += m_fileExt;
  EsigmaFile += "sigma.conf";
  EsigmaIn.open(EsigmaFile.c_str());
  for(int i=0;i<56;i++) {
    EsigmaIn>>m_eSigma[i];
    cout<<"Sigma="<<m_eSigma[i]<<endl;
  }
  EsigmaIn.close();
} 

void 
EmcSelBhaEvent::readDepEneFun(){

  ifstream EDepEneIn;
  std::string EDepEneFile = m_inputFileDir;
  EDepEneFile += m_fileExt;
  EDepEneFile += "peakI.conf";
  EDepEneIn.open(EDepEneFile.c_str());
  for(int i=0;i<56;i++) {
    EDepEneIn>>m_eDepEne[i];
    //m_eDepEne[i]=m_eDepEne[i]-0.020;
    //m_eDepEne[i]=m_eDepEne[i]/1.843*m_beamEnergy;
    cout<<"DepEne="<<m_eDepEne[i]<<endl;
  }
  EDepEneIn.close();

}

void 
EmcSelBhaEvent::readSigmaExp(){
  ifstream ESigmaExpIn;
  std::string ESigmaExpFile = m_inputFileDir;
  ESigmaExpFile += m_fileExt;
  ESigmaExpFile += "sigmaI.conf";
  ESigmaExpIn.open(ESigmaExpFile.c_str());
  for(int i=0;i<56;i++) {
    ESigmaExpIn>>m_eSigmaExp[i];
    cout<<"SigmaExp="<<m_eSigmaExp[i]<<endl;
  }
  ESigmaExpIn.close();


}

void 
EmcSelBhaEvent::readRawPeakIxtal(){


  ifstream EIn;
  std::string EFile = m_inputFileDir;
  EFile += m_fileExt;
  EFile += "findpeak.conf";
  EIn.open(EFile.c_str());

  double Peak[6240];
  int ixtal;
  for(int i=0;i<6240;i++) {
    EIn>>ixtal>>Peak[i];
  m_eRawPeak[ixtal]=Peak[i];
  }
  EIn.close();

  /*
  ifstream EIn1;
  std::string EFile1 = m_inputFileDir;
  EFile1 += m_fileExt;
  EFile1 += "findpeak-mc.conf";
  EIn1.open(EFile1.c_str());

  double Peak1[6240];
  int ixtal1;
  for(int i=0;i<6240;i++) {
    EIn1>>ixtal1>>Peak1[i];
  m_eMcPeak[ixtal1]=Peak1[i];
  }
  EIn1.close();
  */
}



double
EmcSelBhaEvent::Angle2ClosestShower( int ShowerID ) {

  double minDist=999;

  SmartDataPtr<EvtRecEvent> evtRecEvent(eventSvc(), EventModel::EvtRec::EvtRecEvent);
  SmartDataPtr<EvtRecTrackCol> evtRecTrkCol(eventSvc(),  EventModel::EvtRec::EvtRecTrackCol);

  EvtRecTrackIterator  itTrk = evtRecTrkCol->begin() + ShowerID;

  RecEmcShower *theShower = (*itTrk)->emcShower();
  HepLorentzVector theShowerVec(1,1,1,1);
  theShowerVec.setTheta(theShower->theta());
  theShowerVec.setPhi(theShower->phi());
  theShowerVec.setRho(theShower->energy());
  theShowerVec.setE(theShower->energy());    

  for(int j = 0; j < evtRecEvent->totalTracks(); j++){
    EvtRecTrackIterator jtTrk=evtRecTrkCol->begin() + j;

     if(!(*jtTrk)->isEmcShowerValid()) continue;
    if (ShowerID == (*jtTrk)->trackId()) continue;

     RecEmcShower *aShower = (*jtTrk)->emcShower();
    
    if (aShower->energy() >  m_minAngShEnergyCut ){      
	
	HepLorentzVector aShowerVec(1,1,1,1);
	aShowerVec.setTheta(aShower->theta());
	aShowerVec.setPhi(aShower->phi());
	aShowerVec.setRho(aShower->energy());
	aShowerVec.setE(aShower->energy());

	double dist = theShowerVec.angle(aShowerVec);

	if ( dist < minDist )
	  minDist = dist;
	
    }

  }
  
  return minDist;
}

