//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxCosmicSewer.cxx,v 1.11 2012/07/20 05:48:16 zhangy Exp $
//
// Description:
//	Class MdcxCosmicSewer
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Steve Wagner    		Original Author
//	Zhang Yao(zhangyao@ihep.ac.cn)
//
// Copyright Information:
//	Copyright (C) 1997		BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcxReco/MdcxCosmicSewer.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "EventModel/EventHeader.h"
#include "EvTimeEvent/RecEsTime.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TrkBase/TrkFit.h"
#include "TrkFitter/TrkHelixMaker.h"
#include "TrkFitter/TrkLineMaker.h"
#include "MdcxReco/Mdcxprobab.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcRecoHitOnTrack.h"
#include "MdcTrkRecon/MdcTrack.h"
#include "TrkBase/TrkHitList.h"
#include "MdcPrintSvc/IMdcPrintSvc.h"
#include "TrkBase/TrkExchangePar.h"
#include "MdcGeom/EntranceAngle.h"

//----------------
// Constructors --
//----------------

MdcxCosmicSewer::MdcxCosmicSewer(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator) 
{

  declareProperty("debug",          m_debug = false);
  declareProperty("hist",           m_hist  = false);

  declareProperty("doSag",          m_doSag = false);

  declareProperty("cosmicSewPar",   m_cosmicSewPar);
  declareProperty("cosmicSewSkip",  m_cosmicSewSkip=false);

  declareProperty("countPropTime",  m_countPropTime = true);
  declareProperty("lineFit",        m_lineFit = false);
}

//--------------
// Destructor --
//--------------
MdcxCosmicSewer::~MdcxCosmicSewer() {
  delete m_bfield;
  delete m_context;
}

StatusCode MdcxCosmicSewer::beginRun(){  
  //Initailize MdcDetector
  m_gm = MdcDetector::instance(m_doSag); 
  if(NULL == m_gm) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

//--------------
// Operations --
//--------------
StatusCode MdcxCosmicSewer::initialize(){  
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  log << MSG::INFO << "in initialize()" << endreq;	

  i_evt=0;
  m_nSewed = 0;

  //Initailize magnetic filed 
  sc = service ("MagneticFieldSvc",m_pIMF);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Unable to open Magnetic field service"<<endreq;
  }
  m_bfield = new BField(m_pIMF);
  log << MSG::INFO << "field z = "<<m_bfield->bFieldNominal()<< endreq;
  m_context = new TrkContextEv(m_bfield);


  // Get MdcCalibFunSvc
  IMdcCalibFunSvc* imdcCalibSvc; 
  sc = service ("MdcCalibFunSvc", imdcCalibSvc);
  m_mdcCalibFunSvc = dynamic_cast<MdcCalibFunSvc*>(imdcCalibSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  // Get RawDataProviderSvc
  IRawDataProviderSvc* iRawDataProvider; 
  sc = service ("RawDataProviderSvc", iRawDataProvider);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  }
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*>(iRawDataProvider);

  //Initailize MdcUtilitySvc
  IMdcUtilitySvc * imdcUtilitySvc;
  sc = service ("MdcUtilitySvc", imdcUtilitySvc);
  m_mdcUtilitySvc = dynamic_cast<MdcUtilitySvc*> (imdcUtilitySvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcUtilitySvc!" << endreq;
    return StatusCode::FAILURE;
  }

  //Initailize MdcPrintSvc
  IMdcPrintSvc* imdcPrintSvc;
  sc = service ("MdcPrintSvc", imdcPrintSvc);
  m_mdcPrintSvc = dynamic_cast<MdcPrintSvc*> (imdcPrintSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcPrintSvc!" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_hist){
    //book ntuple
    NTuplePtr ntCsmcSew(ntupleSvc(), "MdcxReco/csmc");
    if ( ntCsmcSew ) { m_xtupleCsmcSew = ntCsmcSew;}
    else {
      m_xtupleCsmcSew = ntupleSvc()->book ("MdcxReco/csmc", CLID_ColumnWiseTuple, "MdcxReco reconsturction results");
      if ( m_xtupleCsmcSew ) {
	sc = m_xtupleCsmcSew->addItem ("dD0",      m_csmcD0);
	sc = m_xtupleCsmcSew->addItem ("dPhi0",    m_csmcPhi0);
	sc = m_xtupleCsmcSew->addItem ("dZ0",      m_csmcZ0);
	sc = m_xtupleCsmcSew->addItem ("dOmega",   m_csmcOmega);
	sc = m_xtupleCsmcSew->addItem ("dPt",      m_csmcPt);
	sc = m_xtupleCsmcSew->addItem ("dTanl",    m_csmcTanl);
      }else{
	log << MSG::FATAL << "Could not book MdcxReco/csmc!" << endreq;
	return StatusCode::FAILURE;
      }
    }//end of book
  }

  return StatusCode::SUCCESS;
}

StatusCode MdcxCosmicSewer::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;	


  setFilterPassed(false);
  // Get event No.
  SmartDataPtr<Event::EventHeader> evtHead(eventSvc(),"/Event/EventHeader");
  if (!evtHead) {
    log << MSG::FATAL<< "Could not retrieve event header" << endreq;
    return StatusCode::FAILURE;
  } 
  long t_runNo = evtHead->runNumber();
  long t_evtNo = evtHead->eventNumber();
  if (m_debug) std::cout << "sew "<<++i_evt<<" run "<<t_runNo<<" evt " << t_evtNo << std::endl;


  // Get bunch time t0 (ns)
  m_bunchT0 = -999.;
  SmartDataPtr<RecEsTimeCol> aevtimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if (!aevtimeCol || aevtimeCol->size()==0) {
    log << MSG::WARNING<< "Could not find RecEsTimeCol"<< endreq;
    return StatusCode::SUCCESS;
  }
  RecEsTimeCol::iterator iter_evt = aevtimeCol->begin();
  for(; iter_evt!=aevtimeCol->end(); iter_evt++){
    m_bunchT0 =  (*iter_evt)->getTest();
  }

  //read event data 
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  SmartDataPtr<RecMdcHitCol> recMdcHitCol(eventSvc(),EventModel::Recon::RecMdcHitCol);
  if (!recMdcTrackCol || ! recMdcHitCol) return StatusCode::SUCCESS; 
  if (2!=recMdcTrackCol->size()){
    clearRecMdcTrackHit();
    return StatusCode::SUCCESS;
  }
  if(m_debug)std::cout<<name()<<" nTk==2 begin sewing"<<  std::endl;


  //get best track
  double dParam[5]={0.,0.,0.,0.,0.};
  float bprob =0.;
  float chisum =0.; 
  RecMdcTrackCol::iterator besthel;

  int besthelId = -999;
  RecMdcTrackCol::iterator it = recMdcTrackCol->begin();
  for (int iTk=0; it != recMdcTrackCol->end(); it++,iTk++) {
    RecMdcTrack* tk = *it;
    double Bz = m_bfield->bFieldNominal();
    double d0 = -tk->helix(0);    //cm
    double phi0 = tk->helix(1)+ CLHEP::halfpi;
    double omega = Bz*tk->helix(2)/-333.567;
    double z0 = tk->helix(3);    //cm
    double tanl = tk->helix(4);    //cm

    if(phi0>Constants::pi)phi0-=Constants::twoPi;
    if(phi0<-Constants::pi)phi0+=Constants::twoPi;

    if(m_debug)std::cout<<__FILE__<<" "<<__LINE__<< "tk"<<iTk
      <<"("<<d0<<","<<phi0<<"," <<","<<omega<<","<<z0<<","<<tanl<<")"<<std::endl;

    if(iTk==0){//store param1
      dParam[0]=d0;
      dParam[1]=phi0;
      dParam[2]=omega;
      dParam[3]=z0;
      dParam[4]=tanl;
    }else{//calc. diff between param1 and param2
      dParam[0] += d0;
      dParam[1] -= (phi0+Constants::pi);
      dParam[2] -= omega;
      dParam[3] -= z0;
      dParam[4] += tanl;
    }

    if(phi0<0) {
      besthelId = iTk;
      besthel = it;
    }
    float bchisq = tk->chi2();
    int   bndof  = tk->ndof();
    bprob=Mdcxprobab(bndof,bchisq);
    chisum += bchisq;
    //if (bprob > bestprob){
    //bestprob = bprob; 
    //besthel = it;
    //}
  }//end for recMdcTrackCol

  if(besthelId == -999) return StatusCode::SUCCESS;


  TrkHelixMaker helixfactory;
  TrkLineMaker linefactory; 

  if(m_debug){
    std::cout<<__FILE__<<" param diff: " << "\n D0 " << dParam[0]
      << "\n Phi0 " << dParam[1] << "\n Omega " << dParam[2]
      << "\n Z0 " << dParam[3] << "\n Tanl " << dParam[4] << std::endl;
  }

  if(m_hist){
    m_csmcD0= dParam[0];
    m_csmcPhi0=dParam[1];
    m_csmcOmega=dParam[2];
    m_csmcZ0=dParam[3];
    m_csmcTanl=dParam[4];
    m_xtupleCsmcSew->write();
  }

  //get track 5 parameters
  if( (fabs(dParam[0])>m_cosmicSewPar[0]) ||
      (fabs(dParam[1])>m_cosmicSewPar[1]) || 
      (fabs(dParam[2])>m_cosmicSewPar[2]) ||
      (fabs(dParam[3])>m_cosmicSewPar[3]) ||
      (fabs(dParam[4])>m_cosmicSewPar[4]) ){
    if(m_debug)std::cout<<__FILE__<<" 2 trk param not satisfied skip!"<<std::endl;
    if(m_debug){
      if(fabs(dParam[0])>m_cosmicSewPar[0]) std::cout<<" cut by d0 "<<std::endl;
      if(fabs(dParam[1])>m_cosmicSewPar[1]) std::cout<<" cut by phi0 "<<std::endl;
      if(fabs(dParam[2])>m_cosmicSewPar[2]) std::cout<<" cut by omega "<<std::endl;
      if(fabs(dParam[3])>m_cosmicSewPar[3]) std::cout<<" cut by z0"<<std::endl;
      if(fabs(dParam[4])>m_cosmicSewPar[4]) std::cout<<" cut by tanl"<<std::endl;
    }
    clearRecMdcTrackHit();
    return StatusCode::SUCCESS;
  }

  //got 2 sew-able trks, get helix param
  RecMdcTrack* tk = *besthel;
  double Bz = m_bfield->bFieldNominal();
  double d0 = -tk->helix(0);    //cm
  double phi0 = tk->helix(1)+ CLHEP::halfpi;
  double omega = Bz*tk->helix(2)/-333.567;
  double z0 = tk->helix(3);    //cm
  double tanl = tk->helix(4);    //cm

  TrkExchangePar tt(d0,phi0,omega,z0,tanl);
  if(m_debug)std::cout<<__FILE__<<" best helix: No."<<tk->trackId()<<" Pat param=("<<d0<<" "<<phi0
    <<" "<<omega<<" "<<z0<<" "<<tanl<<")"<<  std::endl;


  //new track
  TrkRecoTrk* newTrack;
  if(m_lineFit){
    newTrack = linefactory.makeTrack(tt, chisum, *m_context, m_bunchT0*1.e-9);
    linefactory.setFlipAndDrop(*newTrack, true, true);
  }else{
    newTrack = helixfactory.makeTrack(tt, chisum, *m_context, m_bunchT0*1.e-9);
    helixfactory.setFlipAndDrop(*newTrack, true, true);
  }


  // combine hit list
  HitRefVec skippedHits; 
  it = recMdcTrackCol->begin();
  for (int iTk=0; it != recMdcTrackCol->end(); it++,iTk++) {
    RecMdcTrack* tk = *it;
    HitRefVec hits = tk->getVecHits();
    HepVector helixPatPar = m_mdcUtilitySvc->besPar2PatPar(tk->helix());
    MdcxHitsToHots(helixPatPar,newTrack,hits,skippedHits);
  }


  //------------------------------------
  //do fit
  //------------------------------------
  TrkErrCode err=newTrack->hits()->fit();
  const TrkFit* newFit = newTrack->fitResult(); 


  if (!newFit) {
    // Fit bad
    if(m_debug)std::cout<<__FILE__<<" sew fit failed!!!"<<  std::endl;
    HitRefVec::iterator it= skippedHits.begin();   
    for (;it!=skippedHits.end();++it){ delete it->data();}
    delete newTrack;
    clearRecMdcTrackHit();
  } else {
    //------------------------------------
    // Fit good
    //------------------------------------
    if(m_lineFit){
      linefactory.setFlipAndDrop(*newTrack, false, false);
    }else{
      helixfactory.setFlipAndDrop(*newTrack, false, false);
    }
    if(m_debug){
      std::cout<<__FILE__<<" sew cosmic fit good "<<  std::endl;
      newTrack->print(std::cout);
    }

    SmartDataPtr<MdcHitCol> m_hitCol(eventSvc(), "/Event/Hit/MdcHitCol");
    if (!m_hitCol){
      m_hitCol= new MdcHitCol;
      StatusCode sc = eventSvc()->registerObject("/Event/Hit/MdcHitCol",m_hitCol);
      if(!sc.isSuccess()) {
	std::cout<< " Could not register MdcHitCol" <<endreq;
	return StatusCode::FAILURE;
      }
    }
    uint32_t getDigiFlag = 0;
    MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);
    const MdcDigi* m_digiMap[43][288];
    MdcDigiVec::iterator iter = mdcDigiVec.begin();
    for (; iter != mdcDigiVec.end(); iter++ ) {
      const MdcDigi* aDigi = *iter;
      const Identifier id= aDigi->identify();
      int layer = MdcID::layer(id);
      int wire = MdcID::wire(id);
      m_digiMap[layer][wire] = aDigi;
    }

    //calc. doca of skipped hits
    HitRefVec::iterator itHitSkipped = skippedHits.begin();   
    for (;itHitSkipped!=skippedHits.end();++itHitSkipped){
      RecMdcHit* h = *itHitSkipped;
      Identifier id(h->getMdcId());
      int layer = MdcID::layer(id);
      int wire  = MdcID::wire(id);
      double fltLen = 0.;
      HepVector helix = newFit->helix(fltLen).params();
      HepSymMatrix err = newFit->helix(fltLen).covariance();
      double docaFltLen = h->getFltLen();
      //double doca = m_mdcUtilitySvc->docaPatPar(layer,wire,docaFltLen,helix,err);//yzhang 2012-07-19 
      double doca = m_mdcUtilitySvc->docaPatPar(layer,wire,helix,err);
      double newDoca = fabs(doca);
      int flagLR = h->getFlagLR();
      if ( flagLR == 0 ){ newDoca = -fabs(doca); }//left driftDist <0
      h->setDoca(newDoca);
      if(m_debug>=3)std::cout<<"("<<layer<<","<<wire<<") new doca "<<newDoca<<" resid "<<fabs(fabs(newDoca)-fabs(h->getDriftDistLeft()))<<std::endl;
      if(m_debug>=3&&fabs(fabs(newDoca)-fabs(h->getDriftDistLeft()))>0.02) std::cout<<" bad resid  "<<fabs(fabs(newDoca)-fabs(h->getDriftDistLeft()))<<" doca  "<<newDoca<<" dd  "<<h->getDriftDistLeft()<<std::endl;

      //calc drift dist of skipped hit
      MdcHit *thehit = new MdcHit(m_digiMap[layer][wire], m_gm);
      thehit->setCosmicFit(true);
      thehit->setCalibSvc(m_mdcCalibFunSvc);
      thehit->setCountPropTime(m_countPropTime);
      m_hitCol->push_back(thehit);

      HepPoint3D poca; Hep3Vector tempDir;
      getInfo(helix,0,poca,tempDir);
      if(m_debug>3)std::cout<<"track poca  "<<poca<<std::endl;
      HepPoint3D pos; Hep3Vector dir;
      getInfo(helix,docaFltLen,pos,dir);
      //std::cout<<" pos  "<<pos<<" dir  "<<dir<<"   "<<std::endl;
      if(pos.y()>poca.y()){
	int wireAmb = patAmbig(flagLR);
	//double tofold = (docaFltLen/Constants::c + m_bunchT0)*1.e-9;
	double tof = docaFltLen/Constants::c + (m_bunchT0)*1.e-9;
	//std::cout<<layer<<" "<<wire<<" tofold  "<<tofold*1.e9-m_bunchT0 <<" tof  "<<tof*1.e9-m_bunchT0<<" diff  "<<(tof-tofold)*1.e9<<std::endl;
	//tof = tofold;
	double eAngle = EntranceAngle(dir.phi() - thehit->phi(pos.z()));
	double dAngle = Constants::pi/2 - 1.*dir.theta();
	double z = pos.z();
	double dist = thehit->driftDist(tof, wireAmb, eAngle, dAngle, z);
	double sigma = thehit->sigma(dist,wireAmb,eAngle,dAngle,z);

	if(m_debug>3&&fabs(fabs(h->getDoca())-fabs(dist))>0.02)
	  std::cout<<"("<<layer<<","<<wire<<") old doca "<<h->getDoca()<<" old ddr "<<h->getDriftDistRight()
	    <<" new dd "<<dist <<" newAmbig "<<wireAmb
	    //<<" tof "<<tof <<" z "<<z <<" eAngle "<<eAngle <<" dTime "<<thehit->driftTime(tof,z)
	    <<" old sigma  "<<h->getErrDriftDistLeft()
	    <<" new sigma  "<<sigma<<"   "<<std::endl;
	h->setDriftDistLeft(-1.*abs(dist));
	h->setDriftDistRight(abs(dist));
	h->setErrDriftDistLeft(sigma);
	h->setErrDriftDistRight(sigma);
      }
    }

    //-------------------------------------------------------
    //Store TDS
    //-------------------------------------------------------
    store(newTrack, skippedHits);//newTrack have been deleted

    setFilterPassed(true);
    m_nSewed++;
  }// end if newFit

  if(m_debug) { m_mdcPrintSvc->printRecMdcTrackCol(); } 

  return StatusCode::SUCCESS;
}

StatusCode MdcxCosmicSewer::finalize() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;
  std::cout<<name()<<" after sewed got "<<m_nSewed<<" cosmic tracks"<<std::endl;
  return StatusCode::SUCCESS;
}

void MdcxCosmicSewer::MdcxHitsToHots(HepVector& helix, TrkRecoTrk* newTrack, HitRefVec& recMdcHits, HitRefVec& skippedHits) {

  TrkHitList* trkHitList = newTrack->hits();
  //store new MdcHit
  SmartDataPtr<MdcHitCol> m_hitCol(eventSvc(), "/Event/Hit/MdcHitCol");
  if (!m_hitCol){
    m_hitCol= new MdcHitCol;
    StatusCode sc = eventSvc()->registerObject("/Event/Hit/MdcHitCol",m_hitCol);
    if(!sc.isSuccess()) {
      std::cout<< " Could not register MdcHitCol" <<endreq;
      return;
    }
  } 

  //get MdcDigi pointer
  uint32_t getDigiFlag = 0;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag); 
  if (0 == mdcDigiVec.size()){
    std::cout << " No hits in MdcDigiVec" << std::endl;
    return; 
  }
  const MdcDigi* m_digiMap[43][288];
  MdcDigiVec::iterator iter = mdcDigiVec.begin();
  for (; iter != mdcDigiVec.end(); iter++ ) {
    const MdcDigi* aDigi = *iter;
    const Identifier id= aDigi->identify();
    int layer = MdcID::layer(id);
    int wire = MdcID::wire(id);
    m_digiMap[layer][wire] = aDigi;
  }


  HepPoint3D poca; Hep3Vector tempDir;
  getInfo(helix,0,poca,tempDir);
  if(m_debug>3)std::cout<<"track poca  "<<poca<<std::endl;
  // new MdcRecoHitOnTracks
  HitRefVec::iterator itHit = recMdcHits.begin();   
  for (;itHit!=recMdcHits.end();++itHit){
    RecMdcHit* h = *itHit;
    int layer = MdcID::layer(h->getMdcId());
    int wire = MdcID::wire(h->getMdcId());
    // new fltLen and ambig
    double newFltLen = h->getFltLen();
    HepPoint3D pos; Hep3Vector dir;
    getInfo(helix,h->getFltLen(),pos,dir);
    int newAmbig = patAmbig(h->getFlagLR());
    if(pos.y()>poca.y()){//yzhang TEMP 2012-07-17 
      newFltLen *= -1.;
      newAmbig *= -1;
      if(m_debug>3) std::cout<<" Up track, change sign of fltLen  "<<std::endl;
    }
    int newFlagLR = bes3FlagLR(newAmbig);
    // calc. position of this point
    if(m_cosmicSewSkip && layer<20){
      RecMdcHit* newSkippedHit = new RecMdcHit();
      newSkippedHit->setDriftDistLeft(h->getDriftDistLeft());
      newSkippedHit->setDriftDistRight(h->getDriftDistRight());
      newSkippedHit->setErrDriftDistLeft(h->getErrDriftDistLeft());
      newSkippedHit->setErrDriftDistRight(h->getErrDriftDistRight());
      newSkippedHit->setChisqAdd(h->getChisqAdd());
      newSkippedHit->setFlagLR(newFlagLR);
      newSkippedHit->setStat(h->getStat());
      newSkippedHit->setMdcId(h->getMdcId());
      newSkippedHit->setTdc(h->getTdc());
      newSkippedHit->setAdc(h->getAdc());
      newSkippedHit->setDriftT(h->getDriftT());
      newSkippedHit->setDoca(999.);
      //newSkippedHit->setDoca(h->getDoca());
      newSkippedHit->setEntra(h->getEntra());
      newSkippedHit->setZhit(h->getZhit());
      newSkippedHit->setFltLen(newFltLen);
      skippedHits.push_back(newSkippedHit);
    }else{
      MdcHit *thehit = new MdcHit(m_digiMap[layer][wire], m_gm); 
      thehit->setCosmicFit(true);
      thehit->setCalibSvc(m_mdcCalibFunSvc);
      thehit->setCountPropTime(m_countPropTime);
      m_hitCol->push_back(thehit);

      MdcRecoHitOnTrack temp(*thehit, newAmbig, m_bunchT0);//m_bunchT0 nano second here
      MdcHitOnTrack* newhot = &temp;
      newhot->setFltLen(newFltLen);
      if(m_debug>3) std::cout<<name()<<" ("<<layer<<","<<wire<<") fltLen "<<h->getFltLen()<<" newFltLen "<<newFltLen<<" newAmbig "<<newAmbig<<" pos.y() "<<pos.y()<<std::endl;

      trkHitList->appendHot(newhot);
    }
  }//end of loop hits
}

void MdcxCosmicSewer::store(TrkRecoTrk* aTrack, HitRefVec& skippedHits) {
  StatusCode sc;
  MsgStream log(msgSvc(), name());

  assert (aTrack != NULL);

  //IDataManagerSvc *dataManSvc;
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
  DataObject *aTrackCol;
  eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
  if(aTrackCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
    eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
  }

  DataObject *aRecHitCol;
  eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
  if(aRecHitCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
    eventSvc()->unregisterObject("/Event/Recon/RecMdcHitCol");
  }

  SmartDataPtr<RecMdcTrackCol> trackList(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackList) {
    RecMdcTrackCol *trackList = new RecMdcTrackCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcTrackCol, trackList);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcTrack collection" <<endreq;
    }
  }

  SmartDataPtr<RecMdcHitCol> hitList(eventSvc(),EventModel::Recon::RecMdcHitCol);
  if (!hitList) {
    hitList = new RecMdcHitCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcHitCol, hitList);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcHit collection" <<endreq;
    }
  }
  int trackId = trackList->size() - 1;

  if(m_debug)std::cout<<__FILE__<<" "<<__LINE__<<" STORED"<<  std::endl;
  MdcTrack mdcTrack(aTrack);//aTrack have been deleted in ~MdcTrack() 

  //tkStat:  0,PatRec; 1,MdcxReco; 2,Tsf; 3,CurlFinder; -1,Combined cosmic
  int tkStat = -1;
  mdcTrack.storeTrack(trackId, trackList, hitList, tkStat);

  //store hits on track
  RecMdcTrackCol::iterator it = trackList->begin();
  HitRefVec hl = (*it)->getVecHits();
  HitRefVec hitRefVec;
  HitRefVec::iterator itHit = hl.begin();   
  int ihl=0;
  for (;itHit!=hl.end();++itHit){
    ihl++;
    RecMdcHit* h = *itHit;
    SmartRef<RecMdcHit> refHit(h);
    hitRefVec.push_back(refHit);
  }

  if(m_debug) std::cout<<__FILE__<<" "<<__LINE__<<"refHit stored "<< ihl<<  std::endl;
  //store skipped hits
  HitRefVec::iterator itHitSkipped = skippedHits.begin();   
  int iskipped=0;
  for (;itHitSkipped!=skippedHits.end();++itHitSkipped){
    iskipped++;
    (*itHitSkipped)->setTrkId(trackId);
    hitList->push_back(*itHitSkipped);
    SmartRef<RecMdcHit> refHitSkipped(*itHitSkipped);
    hitRefVec.push_back(refHitSkipped);
  }
  if(m_debug) std::cout<<__FILE__<<" "<<__LINE__<<"skippedHits stored "<< iskipped<<  std::endl;
  (*it)->setVecHits(hitRefVec);

  (*it)->setNhits((*it)->getVecHits().size());
  (*it)->setNster(-1);
  //omega==0, line fit :ndof = nhit-4
  //omega>0, helix fit :ndof = nhit-5
  if((*it)->helix(2)<0.00001)(*it)->setNdof((*it)->getNhits()-4);
  else (*it)->setNdof((*it)->getNhits()-5);

  if(m_debug) std::cout<<__FILE__<<" "<<__LINE__<<" stored "<< (*it)->getVecHits().size()<<  std::endl;

}

void MdcxCosmicSewer::clearRecMdcTrackHit(){
  MsgStream log(msgSvc(), name());
  StatusCode sc;
  //-------------------------------------------------------
  //clear TDS
  //-------------------------------------------------------
  //IDataManagerSvc *dataManSvc;
  SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
  DataObject *aTrackCol;
  eventSvc()->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
  if(aTrackCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
    eventSvc()->unregisterObject("/Event/Recon/RecMdcTrackCol");
  }
  DataObject *aRecHitCol;
  eventSvc()->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
  if(aRecHitCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
    eventSvc()->unregisterObject("/Event/Recon/RecMdcHitCol");
  }
  SmartDataPtr<RecMdcTrackCol> trackListTemp(eventSvc(),EventModel::Recon::RecMdcTrackCol);
  if (!trackListTemp) {
    RecMdcTrackCol *trackListTemp= new RecMdcTrackCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcTrackCol, trackListTemp);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcTrack collection" <<endreq;
    }
  }
  SmartDataPtr<RecMdcHitCol> hitListTemp(eventSvc(),EventModel::Recon::RecMdcHitCol);
  if (!hitListTemp) {
    RecMdcHitCol *hitListTemp= new RecMdcHitCol;
    sc =  eventSvc()->registerObject(EventModel::Recon::RecMdcHitCol, hitListTemp);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register RecMdcHit collection" <<endreq;
    }
  }
}

void MdcxCosmicSewer::getInfo(HepVector helix, double fltLen, HepPoint3D& pos, Hep3Vector & dir){
  if(m_lineFit) return;//line fit FIXME yzhang 2012-07-19 
  double d0     = helix[0];
  double phi0   = helix[1];
  double omega  = helix[2];
  double z0     = helix[3];
  double tanDip = helix[4];
  double cDip = 1./sqrt(1.+tanDip*tanDip); 
  double sDip = tanDip * cDip;
  double phi00 = phi0;  // Don't normalize
  double ang = phi00 + cDip*fltLen*omega; 
  double cang = cos(ang);
  double sang = sin(ang);
  double sphi0 = sin(phi00);
  double cphi0 = cos(phi00);
  HepPoint3D referencePoint(0.,0.,0.);//yzhang 2012-07-17 TEMP
  double xt = (sang - sphi0)/omega - d0*sphi0 + referencePoint.x();
  double yt = -(cang - cphi0)/omega + d0*cphi0 + referencePoint.y();
  double zt = z0 + fltLen*sDip + referencePoint.z();
  pos.setX(xt);
  pos.setY(yt);
  pos.setZ(zt);

  dir.setX(cang * cDip);
  dir.setY(sang * cDip);
  dir.setZ(sDip);
}

int MdcxCosmicSewer::patAmbig(int bes3FlagLR){
  int ambig = 0;
  if ( bes3FlagLR == 0 ){ 
    ambig = 1; //left driftDist <0
  }else if( bes3FlagLR == 1){ 
    ambig = -1;//right driftDist >0
  }else if( bes3FlagLR == 2){ 
    ambig = 0; //don't know 
  }
  return ambig;
}

int MdcxCosmicSewer::bes3FlagLR(int patAmbig){
  int flagLR = 2;
  if ( patAmbig == 1 ){ 
    flagLR = 0; //left driftDist <0
  }else if( patAmbig == -1){ 
    flagLR = 1; //right driftDist >0
  }else if( patAmbig == 0){ 
    flagLR = 2; //don't know 
  }
  return flagLR;
}
