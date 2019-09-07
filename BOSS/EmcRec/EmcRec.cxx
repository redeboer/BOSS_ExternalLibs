#include <iostream>
#include <fstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "EventModel/EventHeader.h"
#include "EmcRawEvent/EmcDigi.h"
#include "EmcRecEventModel/RecEmcEventModel.h"
#include "McTruth/McParticle.h"
#include "McTruth/EmcMcHit.h"
#include "RawEvent/RawDataUtil.h"

#include "EmcRec/EmcRec.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"


#include "EmcRec/EmcRecParameter.h"
#include "EmcRec/EmcRecFastCluster2Shower.h"
#include "EmcRec/EmcRecCluster2Shower.h"
#include "EmcRec/EmcRecTofMatch.h"
#include "EmcRec/EmcRecTofDigitCalib.h"
//#include "EmcRec/EmcRecFindTofShower.h"
#include "EmcRec/EmcRecTDS.h"
#include "RawDataProviderSvc/RawDataProviderSvc.h"
#include "RawDataProviderSvc/EmcRawDataProvider.h"
// tianhl for mt
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////
EmcRec::EmcRec(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),fCluster2Shower(0)
{   
  m_event=0;
  fPositionMode.push_back("log");
  fPositionMode.push_back("5x5");
  // Declare the properties  
  m_propMgr.declareProperty("Output",fOutput=0);
  m_propMgr.declareProperty("EventNb",fEventNb=0);
  m_propMgr.declareProperty("DigiCalib",fDigiCalib=false);
  m_propMgr.declareProperty("TofEnergy",fTofEnergy=false);
  m_propMgr.declareProperty("OnlineMode",fOnlineMode=false);
  m_propMgr.declareProperty("TimeMin",fTimeMin=0);
  m_propMgr.declareProperty("TimeMax",fTimeMax=60);
  m_propMgr.declareProperty("PositionMode",fPositionMode);

  //Method == 0 use old correction parameter
  //Method == 1 use new correction parameter
  m_propMgr.declareProperty("MethodMode",fMethodMode=1);
  //PosCorr=0, no position correction, and PosCorr=1 with position correction
  m_propMgr.declareProperty("PosCorr",fPosCorr=1);
  m_propMgr.declareProperty("ElecSaturation",fElecSaturation=1);

  IJobOptionsSvc* jobSvc;
  pSvcLocator->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("EmcRecAlg", &m_propMgr);

  // Get EmcRecParameter
  EmcRecParameter::lock();
  EmcRecParameter& Para=EmcRecParameter::GetInstance();
  Para.SetDigiCalib(fDigiCalib);
  Para.SetTimeMin(fTimeMin);
  Para.SetTimeMax(fTimeMax);
  Para.SetMethodMode(fMethodMode);
  Para.SetPosCorr(fPosCorr);
  Para.SetPositionMode(fPositionMode);
  Para.SetElecSaturation(fElecSaturation);

  EmcRecParameter::unlock();

}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcRec::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  //Get RawDataProviderSvc
  // tianhl for mt
  std::string rawDataProviderSvc_name("RawDataProviderSvc");
  StatusCode sc = service(rawDataProviderSvc_name.c_str(), m_rawDataProviderSvc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::ERROR << "EmcRec Error: Can't get RawDataProviderSvc." << endmsg;
  }

  fDigit2Hit.SetAlgName(name()); 

#ifndef OnlineMode
  if(!(m_rawDataProviderSvc->isOnlineMode())) {
    m_tuple=0;
    StatusCode status;

    if(fOutput>=1) {
      NTuplePtr nt(ntupleSvc(),"FILE301/n1");
      if ( nt ) m_tuple = nt;
      else {
        m_tuple=ntupleSvc()->book("FILE301/n1",CLID_ColumnWiseTuple,"EmcRec");
        if( m_tuple ) {
          status = m_tuple->addItem ("pid",pid); 
          status = m_tuple->addItem ("tp",tp);
          status = m_tuple->addItem ("ttheta",ttheta);
          status = m_tuple->addItem ("tphi",tphi);
          status = m_tuple->addItem ("nrun",nrun);
          status = m_tuple->addItem ("nrec",nrec);
          status = m_tuple->addItem ("nneu",nneu);
          status = m_tuple->addItem ("ncluster",ncluster);
          status = m_tuple->addItem ("npart",npart);
          status = m_tuple->addItem ("ntheta",ntheta);
          status = m_tuple->addItem ("nphi",nphi);
          status = m_tuple->addItem ("ndigi",ndigi);
          status = m_tuple->addItem ("nhit",nhit);
          status = m_tuple->addItem ("pp1",4,pp1);
          status = m_tuple->addItem ("theta1",theta1);
          status = m_tuple->addItem ("phi1",phi1);
          status = m_tuple->addItem ("dphi1",dphi1);
          status = m_tuple->addItem ("eseed",eseed);
          status = m_tuple->addItem ("e3x3",e3x3);
          status = m_tuple->addItem ("e5x5",e5x5);
          status = m_tuple->addItem ("enseed",enseed);
          status = m_tuple->addItem ("etof2x1",etof2x1);
          status = m_tuple->addItem ("etof2x3",etof2x3);
          status = m_tuple->addItem ("cluster2ndMoment",cluster2ndMoment);
          status = m_tuple->addItem ("secondMoment",secondMoment);
          status = m_tuple->addItem ("latMoment",latMoment);
          status = m_tuple->addItem ("a20Moment",a20Moment);
          status = m_tuple->addItem ("a42Moment",a42Moment);
          status = m_tuple->addItem ("mpi0",mpi0);
          status = m_tuple->addItem ("thtgap1",thtgap1);
          status = m_tuple->addItem ("phigap1",phigap1);
          status = m_tuple->addItem ("pp2",4,pp2);
        }
        else    {   // did not manage to book the N tuple....
          log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple) << endmsg;
          return StatusCode::FAILURE;
        }
      }
    }
    fCluster2Shower = new EmcRecCluster2Shower;
  } else {
    fCluster2Shower = new EmcRecFastCluster2Shower;
  }
#else
  fCluster2Shower = new EmcRecFastCluster2Shower;
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcRec::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "in execute()" << endreq;   
  /// Reconstruction begins.
  /// State 1: Initialize digit map
  int event, run;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL <<name() << " Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  run=eventHeader->runNumber();
  event=eventHeader->eventNumber();
  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  if(run>0) Para.SetDataMode(1);
  else Para.SetDataMode(0);


  if(fEventNb!=0&&m_event%fEventNb==0) {
        log << MSG::FATAL <<m_event<<"-------: "<<run<<","<<event<<endreq;
  }
  m_event++;
  if(fOutput>=2) {
    log << MSG::DEBUG <<"===================================="<<endreq;
    log << MSG::DEBUG <<"run= "<<run<<"; event= "<<event<<endreq;
  }

  Hep3Vector posG;  //initial photon position
#ifndef OnlineMode
  if(!(m_rawDataProviderSvc->isOnlineMode())) {
    if(fOutput>=1&&run<0) { //run<0 means MC
      // Retrieve mc truth
      SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
      if (!mcParticleCol) {
        log << MSG::WARNING << "Could not find McParticle" << endreq;
      } else {
        HepLorentzVector pG;
        McParticleCol::iterator iter_mc = mcParticleCol->begin();
        for (;iter_mc != mcParticleCol->end(); iter_mc++) {
          log << MSG::INFO
            << " particleId = " << (*iter_mc)->particleProperty()
            << endreq;
          pG = (*iter_mc)->initialFourMomentum();
          posG = (*iter_mc)->initialPosition().v();
        }
        ttheta = pG.theta();
        tphi = pG.phi();
        if(tphi<0) {
          tphi=twopi+tphi;
        }
        tp = pG.rho();

        // Retrieve EMC truth
        SmartDataPtr<EmcMcHitCol> emcMcHitCol(eventSvc(),"/Event/MC/EmcMcHitCol");
        if (!emcMcHitCol) {
          log << MSG::WARNING << "Could not find EMC truth" << endreq;
        }

        RecEmcID mcId;
        unsigned int mcTrackIndex;
        double mcPosX=0,mcPosY=0,mcPosZ=0;
        double mcPx=0,mcPy=0,mcPz=0;
        double mcEnergy=0;

        EmcMcHitCol::iterator iterMc;
        for(iterMc=emcMcHitCol->begin();iterMc!=emcMcHitCol->end();iterMc++){
          mcId=(*iterMc)->identify();
          mcTrackIndex=(*iterMc)->getTrackIndex();
          mcPosX=(*iterMc)->getPositionX();
          mcPosY=(*iterMc)->getPositionY();
          mcPosZ=(*iterMc)->getPositionZ();
          mcPx=(*iterMc)->getPx();
          mcPy=(*iterMc)->getPy();
          mcPz=(*iterMc)->getPz();
          mcEnergy=(*iterMc)->getDepositEnergy();

          if(fOutput>=2){
            //cout<<"mcId="<<mcId<<"\t"<<"mcTrackIndex="<<mcTrackIndex<<endl;
            //cout<<"mcPosition:\t"<<mcPosX<<"\t"<<mcPosY<<"\t"<<mcPosZ<<endl;
            //cout<<"mcP:\t"<<mcPx<<"\t"<<mcPy<<"\t"<<mcPz<<endl;
            //cout<<"mcDepositEnergy=\t"<<mcEnergy<<endl;
          }
        }
      }
    } //fOutput>=1
  } //isOnlineMode

#endif
  //cout<<"EmcRec1--1"<<endl;
  /// Retrieve EMC digi
  fDigitMap.clear();
  fHitMap.clear();
  fClusterMap.clear();
  fShowerMap.clear();

  // Get EmcCalibConstSvc.
  IEmcCalibConstSvc *emcCalibConstSvc = 0;
  StatusCode sc = service("EmcCalibConstSvc", emcCalibConstSvc);
  if(sc != StatusCode::SUCCESS) {
    ;
    //cout << "EmcRec Error: Can't get EmcCalibConstSvc." << endl;
  }

  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
    log << MSG::FATAL << "Could not find EMC digi" << endreq;
    return( StatusCode::FAILURE);
  }
  //cout<<"EmcRec1--2"<<endl;
  EmcDigiCol::iterator iter3;
  for (iter3=emcDigiCol->begin();iter3!= emcDigiCol->end();iter3++) {
    RecEmcID id((*iter3)->identify());
    double adc=RawDataUtil::EmcCharge((*iter3)->getMeasure(),
        (*iter3)->getChargeChannel());
    double tdc=RawDataUtil::EmcTime((*iter3)->getTimeChannel());

    //for cable connection correction
    unsigned int nnpart=EmcID::barrel_ec(id);
    unsigned int nnthe=EmcID::theta_module(id);
    unsigned int nnphi=EmcID::phi_module(id);

    int index = emcCalibConstSvc->getIndex(nnpart,nnthe,nnphi);
    int ixtalNumber=emcCalibConstSvc->getIxtalNumber(index);
 

    if(run>0&&ixtalNumber>0) {
      unsigned int npartNew=emcCalibConstSvc->getPartID(ixtalNumber);
      unsigned int ntheNew=emcCalibConstSvc->getThetaIndex(ixtalNumber);
      unsigned int nphiNew=emcCalibConstSvc->getPhiIndex(ixtalNumber);
      id=EmcID::crystal_id(npartNew,ntheNew,nphiNew);
    }//-------

    //*2016-12
 
 /*
    unsigned int nphiNew;
    nphiNew=nnphi;
    if(nnpart==0) {
      if (nnthe==0){
	if (nnphi==36) nphiNew=40;
	if (nnphi==40) nphiNew=36;    
	if (nnphi==37) nphiNew=41;
	if (nnphi==41) nphiNew=37;  	 
      }//----
      if (nnthe==1){
	if (nnphi==36) nphiNew=40;
	if (nnphi==40) nphiNew=36;    
	if (nnphi==37) nphiNew=41;
	if (nnphi==41) nphiNew=37;  	 
      }//----
      if (nnthe==2){
	if (nnphi==45) nphiNew=50;
	if (nnphi==50) nphiNew=45;    
	if (nnphi==46) nphiNew=51;
	if (nnphi==51) nphiNew=46;
	if (nnphi==47) nphiNew=52;
	if (nnphi==52) nphiNew=47;	 
      }//----
      if (nnthe==3){
	if (nnphi==45) nphiNew=50;
	if (nnphi==50) nphiNew=45;    
	if (nnphi==46) nphiNew=51;
	if (nnphi==51) nphiNew=46;
	
      }//----
      if (nnthe==4){
	if (nnphi==54) nphiNew=60;
	if (nnphi==60) nphiNew=54;    
	if (nnphi==55) nphiNew=61;
	if (nnphi==61) nphiNew=55;
	if (nnphi==56) nphiNew=62;
	if (nnphi==62) nphiNew=56;	 
      }//----	 
      if (nnthe==5){
	if (nnphi==54) nphiNew=60;
	if (nnphi==60) nphiNew=54;    
	if (nnphi==55) nphiNew=61;
	if (nnphi==61) nphiNew=55;
	if (nnphi==56) nphiNew=62;
	if (nnphi==62) nphiNew=56;	 
      }//----
      
      id=EmcID::crystal_id(nnpart,nnthe,nphiNew); 
    }
    */


    //2017-12
    if (run>=52940&&run<=52995){
    unsigned int ntheNew,nphiNew;
    ntheNew=nnthe;
    nphiNew=nnphi;
    if(nnpart==2) {
      if (nnthe==0){
	if (nnphi==58) nphiNew=60;
	if (nnphi==59) nphiNew=61;    
	if (nnphi==60) nphiNew=58;
	if (nnphi==61) nphiNew=59;  
	ntheNew=0;	 
      }//----
      if (nnthe==1){
	if (nnphi==58) nphiNew=60;
	if (nnphi==59) nphiNew=61;    
	if (nnphi==60) nphiNew=58;
	if (nnphi==61) nphiNew=59;  
 	ntheNew=1;	 
      }//----
      if (nnthe==2){
	if (nnphi==73) nphiNew=75;
	if (nnphi==74) nphiNew=76;    
	if (nnphi==75) nphiNew=73;
	if (nnphi==76) nphiNew=74;
	ntheNew=2;
	 
      }//----
      if (nnthe==3){
	if (nnphi==73) nphiNew=75;
	if (nnphi==74) nphiNew=76;    
	if (nnphi==75) nphiNew=73;
	if (nnphi==76) nphiNew=74;
	ntheNew=3;	
      }//----
      if (nnthe==3&&nnphi==72){
	ntheNew=2;
	nphiNew=77;
      }
      if (nnthe==2&&nnphi==77){
	ntheNew=3;
	nphiNew=72;
      }

      if (nnthe==4){
	if (nnphi==87) nphiNew=90;
	if (nnphi==88) nphiNew=91;    
	if (nnphi==89) nphiNew=92;
	if (nnphi==90) nphiNew=87;
	if (nnphi==91) nphiNew=88;
	if (nnphi==92) nphiNew=89;
	ntheNew=4;	 
      }//----	 
      if (nnthe==5){
	if (nnphi==87) nphiNew=90;
	if (nnphi==88) nphiNew=91;    
	if (nnphi==89) nphiNew=92;
	if (nnphi==90) nphiNew=87;
	if (nnphi==91) nphiNew=88;
	if (nnphi==92) nphiNew=89;
	ntheNew=5;	 
      }//----
      
      id=EmcID::crystal_id(nnpart,ntheNew,nphiNew); 
    }
    } 
    ///==========





    //ixtalNumber=-9 tag dead channel
    if (ixtalNumber==-9) {
      adc=0.0;
    }

    //ixtalNumber=-99 tag hot channel
    if (ixtalNumber==-99) {
      adc=0.0;
    }

    RecEmcDigit aDigit;
    aDigit.Assign(id,adc,tdc);
    fDigitMap[id]=aDigit;
  }
  //cout<<"EmcRec1--3"<<endl;
  if(fOutput>=2) {
    RecEmcDigitMap::iterator iDigitMap;
    for(iDigitMap=fDigitMap.begin();
        iDigitMap!=fDigitMap.end();
        iDigitMap++){
      //cout<<iDigitMap->second;
    }
  }
  // DigitMap ok

  // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
  /// State 2: DigitMap --> HitMap
  fDigit2Hit.Convert(fDigitMap,fHitMap);
  if(fOutput>=2) {
    RecEmcHitMap::iterator iHitMap;
    for(iHitMap=fHitMap.begin();
        iHitMap!=fHitMap.end();
        iHitMap++){
      //cout<<iHitMap->second;
    }
    fDigit2Hit.Output(fHitMap);
  }   
  //cout<<"EmcRec1--4"<<endl;

  /// State 3: HitMap --> ClusterMap
  fHit2Cluster.Convert(fHitMap,fClusterMap);
  //
  if(fOutput>=2) {
    RecEmcClusterMap::iterator iClusterMap;
    for(iClusterMap=fClusterMap.begin();
        iClusterMap!=fClusterMap.end();
        iClusterMap++){
      //cout<<iClusterMap->second;
    }
  }
  //cout<<"EmcRec1--5"<<endl;
  /// State 4: ClusterMap --> ShowerMap
  fCluster2Shower->Convert(fClusterMap,fShowerMap);
  //
  if(fOutput>=2) {
    RecEmcShowerMap::iterator iShowerMap;
    for(iShowerMap=fShowerMap.begin();
        iShowerMap!=fShowerMap.end();
        iShowerMap++) {
      //cout<<iShowerMap->second;
    }
  }
  //cout<<"EmcRec1--6"<<endl;
 
  /// State 6: Register to TDS
  EmcRecTDS tds;
  tds.RegisterToTDS(fHitMap,fClusterMap,fShowerMap);
  if(fOutput>=2) {
    tds.CheckRegister();
  }
  //cout<<"EmcRec1--7"<<endl;
  // oooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
#ifndef OnlineMode   
  if(!(m_rawDataProviderSvc->isOnlineMode())) {
    if(fOutput>=1) {
      nrun=run;
      nrec=event;

      //cout<<"cm="<<cm<<"\tmm="<<mm<<"\tGeV="<<GeV<<"\tMeV="<<MeV<<endl;
      ndigi=fDigitMap.size();
      nhit=fHitMap.size();
      ncluster=fClusterMap.size();
      RecEmcShowerVec fShowerVec;
      RecEmcShowerMap::iterator iShowerMap;
      for(iShowerMap=fShowerMap.begin();
          iShowerMap!=fShowerMap.end();
          iShowerMap++) {
        fShowerVec.push_back(iShowerMap->second);
      }
      sort(fShowerVec.begin(), fShowerVec.end(), greater<RecEmcShower>());
      nneu=fShowerMap.size();

      RecEmcShower aShower;
      //aShower.e5x5(-99.);
      RecEmcShowerVec::iterator iShowerVec;
      iShowerVec=fShowerVec.begin();
      npart=-99;
      ntheta=-99;
      nphi=-99;
      if(iShowerVec!=fShowerVec.end()) {
        aShower=*iShowerVec;
        RecEmcID id=aShower.getShowerId();
        npart=EmcID::barrel_ec(id);
        ntheta=EmcID::theta_module(id);
        nphi=EmcID::phi_module(id);
        pp1[0]=aShower.energy()*aShower.x()/aShower.position().mag();
        pp1[1]=aShower.energy()*aShower.y()/aShower.position().mag();
        pp1[2]=aShower.energy()*aShower.z()/aShower.position().mag();
        pp1[3]=aShower.energy();
        theta1=(aShower.position()-posG).theta();
        phi1=(aShower.position()-posG).phi();
        if(phi1<0) {
          phi1=twopi+phi1;
        }
        thtgap1=aShower.ThetaGap();
        phigap1=aShower.PhiGap();
        RecEmcID seed,nseed;
        seed=aShower.getShowerId();
        nseed=aShower.NearestSeed();
        eseed=aShower.eSeed();
        e3x3=aShower.e3x3();
        e5x5=aShower.e5x5();
        etof2x1=aShower.getETof2x1();
        etof2x3=aShower.getETof2x3();
        if(aShower.getCluster()) {
          cluster2ndMoment=aShower.getCluster()->getSecondMoment();
        }
        secondMoment=aShower.secondMoment();
        latMoment=aShower.latMoment();
        a20Moment=aShower.a20Moment();
        a42Moment=aShower.a42Moment();
        if(nseed.is_valid()) {
          enseed=fHitMap[nseed].getEnergy();
        } else {
          enseed=0;
        }

        dphi1=phi1-tphi;
        if(dphi1<-pi) dphi1=dphi1+twopi;
        if(dphi1>pi)  dphi1=dphi1-twopi;
      }

      if(iShowerVec!=fShowerVec.end()) {
        iShowerVec++;
        if(iShowerVec!=fShowerVec.end()) {
          aShower=*iShowerVec;
          pp2[0]=aShower.energy()*aShower.x()/aShower.position().mag();
          pp2[1]=aShower.energy()*aShower.y()/aShower.position().mag();
          pp2[2]=aShower.energy()*aShower.z()/aShower.position().mag();
          pp2[3]=aShower.energy();
        }
      }

      //for pi0
      if(fShowerVec.size()>=2) {
        RecEmcShowerVec::iterator iShowerVec1,iShowerVec2;
        iShowerVec1=fShowerVec.begin();
        iShowerVec2=fShowerVec.begin()+1;
        double e1=(*iShowerVec1).energy();
        double e2=(*iShowerVec2).energy();
        double angle=(*iShowerVec1).position().angle((*iShowerVec2).position());
        mpi0=sqrt(2*e1*e2*(1-cos(angle)));
      }
      m_tuple->write();      
    }
  }
#endif

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode EmcRec::finalize() {
  EmcRecParameter::Kill();
  if(fCluster2Shower) delete fCluster2Shower;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  return StatusCode::SUCCESS;
}
