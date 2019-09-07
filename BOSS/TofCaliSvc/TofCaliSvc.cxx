//********************************************************
//
//  BESIII Tof Reconstruction 
//  Class: TofCaliSvc
//  Sun Shengsen <sunss@ihep.ac.cn>
//  
//  Description:
//     This service is used to get Tof Calibrarion constants
//     from DataBase and do the Tof data reconstruction.
//
//*********************************************************
#include "TofCaliSvc/TofCaliSvc.h"
#include "TofCaliSvc/BTofCal.h"
#include "TofCaliSvc/BTofCommonCal.h"
#include "TofCaliSvc/ETofCal.h"
#include "TofCaliSvc/EtfCal.h"
#include "TofCaliSvc/EtfBunchCal.h"
#include "TofCaliSvc/TofInfoCal.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "TMath.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "CalibData/CalibModel.h"
#include "GaudiKernel/DataSvc.h"

using namespace std;

// static double radius_Inner =   81.35;
// static double radius_Outer =   87.05;
  
TofCaliSvc::TofCaliSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
  declareProperty("Run",m_run=1);
}

StatusCode TofCaliSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ) {  
  if ( IID_ITofCaliSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<ITofCaliSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TofCaliSvc::initialize ( ) {
  m_hasbeeninitialized=false;

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": Start of run initialisation" << endreq;
  
  StatusCode sc = Service::initialize();
  if ( sc.isFailure() ) return sc;

  IIncidentSvc* incsvc;
  sc = service("IncidentSvc", incsvc);
  int priority = 100;
  if( sc.isSuccess() ){
    incsvc -> addListener(this, "NewRun", priority);
  }

  StatusCode scc;
  
  log<<MSG::INFO << "setProperties()" << endreq;
  scc = service("CalibDataSvc", m_pCalibDataSvc, true);
  if ( !scc.isSuccess() ) {
    log<<MSG::ERROR<<"Could not get IDataProviderSvc interface of CalibXmlCnvSvc"<<endreq;
    return scc;
  } else {
    log << MSG::DEBUG<< "Retrieved IDataProviderSvc interface of CalibXmlCnvSvc"<<endreq;
  }
  // Get properties from the JobOptionsSvc
  scc = setProperties();
  
  return sc;
}  
  
StatusCode TofCaliSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}

TofCaliSvc::~TofCaliSvc(){
  for(vector<BTofCal*>::iterator it1 = fBTofCal.begin(); it1 != fBTofCal.end(); it1++) { delete (*it1); }
  fBTofCal.clear();
  for(vector<ETofCal*>::iterator it2 = fETofCal.begin(); it2 != fETofCal.end(); it2++) { delete (*it2); }
  fETofCal.clear();
  
  if( fEtfCal.size()!=0 ) {
    for(vector<EtfCal*>::iterator it5 = fEtfCal.begin(); it5 != fEtfCal.end(); it5++) { delete (*it5); }
    fEtfCal.clear();
  }
  if( fEtfBunchCal.size()!=0 ) {
    for(vector<EtfBunchCal*>::iterator it6 = fEtfBunchCal.begin(); it6 != fEtfBunchCal.end(); it6++) { delete (*it6); }
    fEtfBunchCal.clear();
  }
  for(vector<BTofCommonCal*>::iterator it3 = fBTofCommonCal.begin(); it3 != fBTofCommonCal.end(); it3++) { delete (*it3); }
  fBTofCommonCal.clear();
  for(vector<TofInfoCal*>::iterator it4 = fTofInfoCal.begin(); it4 != fTofInfoCal.end(); it4++) { delete (*it4); }
  fTofInfoCal.clear();
  return;
}

StatusCode TofCaliSvc::FillfromDatabase(){

  MsgStream log(msgSvc(), name());    
  std::string fullPath = "/Calib/TofCal";
  log << MSG::INFO<<" Tof calib fullPath = "<<fullPath<< endreq;
  SmartDataPtr<CalibData::TofCalibData> test(m_pCalibDataSvc, fullPath);
  if(!test){
    log << MSG::FATAL << "TofCaliSvc could not find TofCalibData in TCDS!!" << endreq;
    return StatusCode::FAILURE;
  }       

  for(vector<BTofCal*>::iterator it1 = fBTofCal.begin(); it1 != fBTofCal.end(); it1++) { delete (*it1); }
  fBTofCal.clear();
  for(vector<ETofCal*>::iterator it2 = fETofCal.begin(); it2 != fETofCal.end(); it2++) { delete (*it2); }
  fETofCal.clear();
  if( fEtfCal.size()!=0 ) {
    for(vector<EtfCal*>::iterator it5 = fEtfCal.begin(); it5 != fEtfCal.end(); it5++) { delete (*it5); }
    fEtfCal.clear();
  }
  if( fEtfBunchCal.size()!=0 ) {
    for(vector<EtfBunchCal*>::iterator it6 = fEtfBunchCal.begin(); it6 != fEtfBunchCal.end(); it6++) { delete (*it6); }
    fEtfBunchCal.clear();
  }
  for(vector<BTofCommonCal*>::iterator it3 = fBTofCommonCal.begin(); it3 != fBTofCommonCal.end(); it3++) { delete (*it3); }
  fBTofCommonCal.clear();
  for(vector<TofInfoCal*>::iterator it4 = fTofInfoCal.begin(); it4 != fTofInfoCal.end(); it4++) { delete (*it4); }
  fTofInfoCal.clear();

  for(int ib=0;ib<176;ib++){
    BTofCal* btof = new BTofCal;
    for( int j=0; j<static_cast<int>(nBarAtten); j++ ) {
      btof->setAtten(j,test->getBTofAtten(ib,j));
    }
    for( int j=0; j<static_cast<int>(nBarSpeed); j++ ) {
      btof->setVeff(j,test->getBTofSpeed(ib,j));
    }
    for( int j=0; j<static_cast<int>(nBarPar); j++){
      btof->setP1(j,test->getBTofPleft(ib,j));
      btof->setP2(j,test->getBTofPright(ib,j));
    }
    for( int j=0; j<static_cast<int>(nBarParOff); j++){
      btof->setPOff1_bunch0(j,test->getBTofPoffleft_bunch0(ib,j));
      btof->setPOff2_bunch0(j,test->getBTofPoffright_bunch0(ib,j));
      btof->setPOff1_bunch1(j,test->getBTofPoffleft_bunch1(ib,j));
      btof->setPOff2_bunch1(j,test->getBTofPoffright_bunch1(ib,j));
      btof->setPOff1_bunch2(j,test->getBTofPoffleft_bunch2(ib,j));
      btof->setPOff2_bunch2(j,test->getBTofPoffright_bunch2(ib,j));
      btof->setPOff1_bunch3(j,test->getBTofPoffleft_bunch3(ib,j));
      btof->setPOff2_bunch3(j,test->getBTofPoffright_bunch3(ib,j));
    }
    for( int j=0; j<static_cast<int>(nBarSigma); j++){
      btof->setFPLeft(j,test->getBTofFleft(ib,j));
      btof->setFPRight(j,test->getBTofFright(ib,j));
    }
    for( int j=0; j<static_cast<int>(nBarSigCnt); j++){
      btof->setFPCounter(j,test->getBTofFcounter(ib,j));
    }

    fBTofCal.push_back(btof);
  }       

  for(int ie=0;ie<96;ie++){
    ETofCal* etof= new ETofCal;
    for( int j=0; j<static_cast<int>(nEndAtten); j++ ) {
      etof->setAtten(j,test->getETofAtten(ie,j));
    }
    for( int j=0; j<static_cast<int>(nEndSpeed); j++ ) {
      etof->setVeff(j,test->getETofSpeed(ie,j));
    }
    for( int j=0; j<static_cast<int>(nEndPar); j++ ) {
      etof->setP(j,test->getETofP(ie,j));
    }
    for( int j=0; j<static_cast<int>(nEndSigma); j++ ) {
      etof->setFPCounter(j,test->getETofFP(ie,j));
    }
    fETofCal.push_back(etof);
  }

  for(int ietf=0;ietf<72;ietf++) {
    for(int jetf=0;jetf<12;jetf++) {
      EtfCal* etf= new EtfCal;
      for( int k=0; k<static_cast<int>(nEtfSpeed); k++ ) {
	etf->setVeff(k,test->getEtfSpeed(ietf,jetf,k));
      }
      for( int k=0; k<static_cast<int>(nEtfPar); k++ ) {
	etf->setP( k,test->getEtfPcombine(ietf,jetf,k));
	etf->setP1(k,test->getEtfPleft(ietf,jetf,k));
	etf->setP2(k,test->getEtfPright(ietf,jetf,k));
      }
      fEtfCal.push_back(etf);
    }
  }

  EtfBunchCal* etfbunch= new EtfBunchCal;
  for( int k=0; k<static_cast<int>(nEtfBunch); k++ ) {
    etfbunch->setBunchP(k,test->getEtfPBunch(k));
  }
  fEtfBunchCal.push_back(etfbunch);

  BTofCommonCal* bcomtof = new BTofCommonCal;
  for(int i=0; i<static_cast<int>(nBarOffset); i++) {
    bcomtof->setOffset( i, test->getBTofOffset(i) );
  }
  for(int i=0; i<static_cast<int>(nBarSigCor); i++) {
    bcomtof->setSigmaCorr( i, test->getBTofSigmaCorr(i) );
  }
  fBTofCommonCal.push_back(bcomtof);

  TofInfoCal* tofinfo = new TofInfoCal;
  tofinfo->setRunBegin( test->getRunBegin() );
  tofinfo->setRunEnd( test->getRunEnd() );
  tofinfo->setVersion( test->getVersion() );
  tofinfo->setQCorr( test->getQCorr() );
  tofinfo->setQElec( test->getQElec() );
  tofinfo->setMisLable( test->getMisLable() );
  for( unsigned int i=0; i<5; i++ ) {
    tofinfo->setBrEast( i, test->getBrEast(i) );
    tofinfo->setBrWest( i, test->getBrWest(i) );
    if( (test->getVersion())<=8 ) { // useless
      tofinfo->setEndcap( i, test->getEndcap(i) );
    }
    else {
      tofinfo->setEtf( i, test->getEndcap(i) );
    }
  }
  fTofInfoCal.push_back(tofinfo);

  return StatusCode::SUCCESS;
}


const double TofCaliSvc::BTDelay1(unsigned id) {
  double tdelay = 0.0;
  if( id < 176 ) {
    tdelay = fBTofCal[id]->getP1(0);
  }
  else {
    MsgStream log(msgSvc(), name());
    log<<MSG::ERROR<<"TofCaliSvc::BTDelay1() -- Wrong TOFID is provided!  ID = "<<id<<endreq;
  }
  return tdelay;
}


const double TofCaliSvc::BTDelay2(unsigned id) {
  double tdelay = 0.0;
  if( id < 176 ) {
    tdelay = fBTofCal[id]->getP2(0);
  }
  else {
    MsgStream log(msgSvc(), name());
    log<<MSG::ERROR<<"TofCaliSvc::BTDelay2() -- Wrong TOFID is provided!  ID = "<<id<<endreq;
  }
  return tdelay;
}


const double TofCaliSvc::ETDelay(unsigned id) {
  double tdelay = 0.0;
  if( id < 96 ) {
    tdelay = fETofCal[id]->getP(0);
  }
  else {
    MsgStream log(msgSvc(), name());
    log<<MSG::ERROR<<"TofCaliSvc::ETDelay() -- Wrong TOFID is provided!  ID = "<<id<<endreq;
  }
  return tdelay;
}


const double TofCaliSvc::BTCorr1(double ADC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double p1[nBarPar];
  for( int i=0; i<static_cast<int>(nBarPar); i++ ) {
    p1[i]=fBTofCal[id]->getP1(i);
  }

  log << MSG::DEBUG << "BTCorr1   id =" << id  << "  1=" << p1[0] << " 2=" << p1[1] << " 3=" << p1[2] << " 4=" << p1[3]  << " 5=" << p1[4] << " 6=" << p1[5] << " 7=" << p1[6] << " 8=" << p1[7] << " 9=" << p1[8] << endreq;

  double tcorr1 = p1[0] + p1[1]/TMath::Sqrt(ADC) + p1[2]*z/TMath::Sqrt(ADC) + p1[3]/ADC + p1[4]*z + p1[5]*z*z + p1[6]*z*z*z;

  return tcorr1;
}


const double TofCaliSvc::BTCorr2(double ADC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double p2[nBarPar];
  for( int i=0; i<static_cast<int>(nBarPar); i++ ) {
    p2[i]=fBTofCal[id]->getP2(i);
  }

  log << MSG::DEBUG << "BTCorr2   id =" << id  << "  1=" << p2[0] << " 2=" << p2[1] << " 3=" << p2[2] << " 4=" << p2[3]  << " 5=" << p2[4] << " 6=" << p2[5] << " 7=" << p2[6] << " 8=" << p2[7] << " 9=" << p2[8] << endreq;

  double tcorr2 = p2[0] + p2[1]/TMath::Sqrt(ADC) + p2[2]*z/TMath::Sqrt(ADC) + p2[3]/ADC + p2[4]*z + p2[5]*z*z + p2[6]*z*z*z;

  return tcorr2;
}


const double TofCaliSvc::BTCorrOffset1(double z,unsigned id,double t0){
  MsgStream log(msgSvc(), name());    

  int numBunch = fTofInfoCal[0]->getRunEnd();
  int whichBunch = (static_cast<int>(t0/(12000./499.8/(numBunch*1.0))+0.1))%numBunch;

  double poff1[nBarParOff];
  if( whichBunch==0 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff1[i]=fBTofCal[id]->getPOff1_bunch0(i);
    }
  }
  else if( whichBunch==1 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff1[i]=fBTofCal[id]->getPOff1_bunch1(i);
    }
  }
  else if( whichBunch==2 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff1[i]=fBTofCal[id]->getPOff1_bunch2(i);
    }
  }
  else if( whichBunch==3 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff1[i]=fBTofCal[id]->getPOff1_bunch3(i);
    }
  }

  double tCorrOffset1 = 0.0;
  if( z>=0.0 ) {
    tCorrOffset1 = poff1[0] + poff1[1]*z + poff1[2]*z*z + poff1[3]*z*z*z + poff1[4]*z*z*z*z + poff1[5]*z*z*z*z*z + poff1[6]*z*z*z*z*z*z;
  }
  else {
    tCorrOffset1 = poff1[10] + poff1[11]*z + poff1[12]*z*z + poff1[13]*z*z*z + poff1[14]*z*z*z*z + poff1[15]*z*z*z*z*z + poff1[16]*z*z*z*z*z*z;
  }

  log << MSG::DEBUG << "BTCorrOff1   id =" << id << " bunch number=" << whichBunch << " TCorrOffset1=" << tCorrOffset1 << "  1=" << poff1[0] << " 2=" << poff1[1] << " 3=" << poff1[2] << " 4=" << poff1[3]  << " 5=" << poff1[4] << " 6=" << poff1[5] << " 7=" << poff1[6] << " 8=" << poff1[7] << " 9=" << poff1[8] << endreq;

  return tCorrOffset1;
}


const double TofCaliSvc::BTCorrOffset2(double z,unsigned id,double t0){
  MsgStream log(msgSvc(), name());    

  int numBunch = fTofInfoCal[0]->getRunEnd();
  int whichBunch = (static_cast<int>(t0/(12000./499.8/(numBunch*1.0))+0.1))%numBunch;

  double poff2[nBarParOff];
  if( whichBunch==0 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff2[i]=fBTofCal[id]->getPOff2_bunch0(i);
    }
  }
  else if( whichBunch==1 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff2[i]=fBTofCal[id]->getPOff2_bunch1(i);
    }
  }
  else if( whichBunch==2 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff2[i]=fBTofCal[id]->getPOff2_bunch2(i);
    }
  }
  else if( whichBunch==3 ) {
    for( int i=0; i<static_cast<int>(nBarParOff); i++ ) {
      poff2[i]=fBTofCal[id]->getPOff2_bunch3(i);
    }
  }

  double tCorrOffset2 = 0.0;
  if( z>=0.0 ) {
    tCorrOffset2 = poff2[0] + poff2[1]*z + poff2[2]*z*z + poff2[3]*z*z*z + poff2[4]*z*z*z*z + poff2[5]*z*z*z*z*z + poff2[6]*z*z*z*z*z*z;
  }
  else {
    tCorrOffset2 = poff2[10] + poff2[11]*z + poff2[12]*z*z + poff2[13]*z*z*z + poff2[14]*z*z*z*z + poff2[15]*z*z*z*z*z + poff2[16]*z*z*z*z*z*z;
  }

  log << MSG::DEBUG << "BTCorrOff2   id =" << id << " bunch number=" << whichBunch << " TCorrOffset2=" << tCorrOffset2 << "  1=" << poff2[0] << " 2=" << poff2[1] << " 3=" << poff2[2] << " 4=" << poff2[3]  << " 5=" << poff2[4] << " 6=" << poff2[5] << " 7=" << poff2[6] << " 8=" << poff2[7] << " 9=" << poff2[8] << endreq;

  return tCorrOffset2;
}


const double TofCaliSvc::TOffset() {
  double toffset = fBTofCommonCal[0]->getOffset(0);
  return toffset;
}


const double TofCaliSvc::BTime1(double ADC,double TDC,double z,unsigned id,double t0){
  MsgStream log(msgSvc(), name());    

  double tcorr1 = BTCorr1( ADC, z, id );
  double tcorr2 = BTCorrOffset1( z, id, t0);
  double tcorr3 = fBTofCommonCal[0]->getOffset(0);

  log << MSG::DEBUG << "BTime1   id =" << id  << "  tcorr=" << tcorr1 << "  toffset=" << tcorr2 << "  total offset=" << tcorr3 << endreq;

  double time = TDC - tcorr1 - tcorr2 - tcorr3;

  if(time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::BTime1() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<endreq;
  }
  return time;     
}


const double TofCaliSvc::BTime2(double ADC,double TDC,double z,unsigned id, double t0){
  MsgStream log(msgSvc(), name());    

  double tcorr1 = BTCorr2( ADC, z, id );
  double tcorr2 = BTCorrOffset2( z, id ,t0);
  double tcorr3 = fBTofCommonCal[0]->getOffset(0);

  log << MSG::DEBUG << "BTime2   id =" << id  << "  tcorr=" << tcorr1 << "  toffset=" << tcorr2 << "  total offset=" << tcorr3 << endreq;

  double time = TDC - tcorr1 - tcorr2 - tcorr3;

  if(time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::BTime2() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<endreq;
  }
  return time;     
}


/*
const double TofCaliSvc::BTime(double tleft,double tright,double z,unsigned id){
  for(int i=0;i<4;i++)
    m_w[i]=fBTofCal[id]->getW(i);  
  double f1=m_w[0]+m_w[1]*z+m_w[2]*z*z;
  double f2=1-f1;
  double wt=f1*tleft+f2*tright+m_w[3];
  if(wt<0.) {
    MsgStream log(msgSvc(), name());
    log<<MSG::WARNING<<"TofCaliSvc::BTime() -- A minus time is given : "<<wt <<" [Input] TDC1 = "<<tleft<<" TDC2 = "<<tright<<" z = "<<z<<" ID = "<<id<<endreq;
  }

  return wt;
}
*/


const double TofCaliSvc::BTimeCounter(double tleft,double tright,double z,unsigned id){

  double sigmaCorr = fBTofCommonCal[0]->getSigmaCorr(0);
  double sigmaCorr2 = sigmaCorr*sigmaCorr;

  double sigmaLeft   = BSigma1( z, id );
  double sigmaLeft2  = sigmaLeft*sigmaLeft;
  double sigmaRight  = BSigma2( z, id );
  double sigmaRight2 = sigmaRight*sigmaRight;

  double fraction = ( sigmaRight2 - sigmaCorr2 )/( sigmaLeft2 + sigmaRight2 - 2.0*sigmaCorr2);

  double wt=fraction*tleft + (1.0-fraction)*tright;
  if(wt<0.) {
    MsgStream log(msgSvc(), name());
    log<<MSG::WARNING<<"TofCaliSvc::BTimeCounter() -- A minus time is given : "<<wt <<" [Input] z = "<<z<<" ID = "<<id<<endreq;
  }

  return wt;
}

const double TofCaliSvc::BTimeCluster(double tlayer1,double tlayer2,double z1,double z2,unsigned id1, unsigned id2){

  double sigmaCorr = fBTofCommonCal[0]->getSigmaCorr(0);
  double sigmaCorr2 = sigmaCorr*sigmaCorr;

  double sigmaInner  = BSigmaCounter( z1, id1 );
  double sigmaInner2 = sigmaInner*sigmaInner;
  double sigmaOuter  = BSigmaCounter( z2, id2 );
  double sigmaOuter2 = sigmaOuter*sigmaOuter;

  double fraction = ( sigmaOuter2 - sigmaCorr2 )/( sigmaInner2 + sigmaOuter2 - 2.0*sigmaCorr2);

  double wt=fraction*tlayer1 + (1.0-fraction)*tlayer2;
  if(wt<0.) {
    MsgStream log(msgSvc(), name());
    log<<MSG::WARNING<<"TofCaliSvc::BTimeCluster() -- A minus time is given : "<<wt <<" [Input] z1 = "<<z1<<" [Input] z2 = "<<z2<<" ID1 = "<< id1 << " ID2=" << id2 << endreq;
  }
  return wt;
}


const double TofCaliSvc::BSigma1(double z, unsigned id) {
  MsgStream log(msgSvc(), name());

  double fpleft[nBarSigma];
  for( int i=0; i<static_cast<int>(nBarSigma); i++ ) {
    fpleft[i]=fBTofCal[id]->getFPLeft(i);  
  }

  log << MSG::DEBUG << "BSigma1   id =" << id  << "  1=" << fpleft[0] << " 2=" << fpleft[1] << " 3=" << fpleft[2] << " 4=" << fpleft[3]  << " 5=" << fpleft[4] << endreq;

  double sigmaLeft = fpleft[0] + fpleft[1]*z + fpleft[2]*z*z + fpleft[3]*z*z*z + fpleft[4]*z*z*z*z;

  return sigmaLeft;
}


const double TofCaliSvc::BSigma2(double z, unsigned id) {
  MsgStream log(msgSvc(), name());

  double fpright[nBarSigma];
  for( int i=0; i<static_cast<int>(nBarSigma); i++ ) {
    fpright[i]=fBTofCal[id]->getFPRight(i);  
  }

  log << MSG::DEBUG << "BSigma2   id =" << id  << "  1=" << fpright[0] << " 2=" << fpright[1] << " 3=" << fpright[2] << " 4=" << fpright[3]  << " 5=" << fpright[4] << endreq;

  double sigmaRight = fpright[0] + fpright[1]*z + fpright[2]*z*z + fpright[3]*z*z*z + fpright[4]*z*z*z*z;

  return sigmaRight;
}


const double TofCaliSvc::BSigmaCounter(double z, unsigned id) {
  MsgStream log(msgSvc(), name());

  double fplayer[nBarSigCnt];
  for( int i=0; i<static_cast<int>(nBarSigCnt); i++ ) {
    fplayer[i]=fBTofCal[id]->getFPCounter(i);  
  }

  log << MSG::DEBUG << "BSigmaCounter   id =" << id  << "  1=" << fplayer[0] << " 2=" << fplayer[1] << " 3=" << fplayer[2] << " 4=" << fplayer[3]  << " 5=" << fplayer[4] << endreq;

  double sigma = fplayer[0] + fplayer[1]*z + fplayer[2]*z*z + fplayer[3]*z*z*z + fplayer[4]*z*z*z*z;

  return sigma;
}


const double TofCaliSvc::BSigmaCluster(double z1, double z2, unsigned id1, unsigned id2) {
  double sigmaInner  = BSigmaCounter(z1, id1);
  double sigmaInner2 = sigmaInner*sigmaInner;
  double sigmaOuter  = BSigmaCounter(z2, id2);
  double sigmaOuter2 = sigmaOuter*sigmaOuter;
  double sigmaCorr   = fBTofCommonCal[0]->getSigmaCorr(0);
  double sigmaCorr2  = sigmaCorr*sigmaCorr;
  double sigma = (sigmaInner2*sigmaOuter2-sigmaCorr2*sigmaCorr2)/(sigmaInner2+sigmaOuter2-2.0*sigmaCorr2);
  sigma = sqrt(sigma);
  return sigma;
}


const double TofCaliSvc::ETime(double ADC,double TDC,double rHit,unsigned id){
  MsgStream log(msgSvc(), name());

  double p[nEndPar];
  for( int i=0; i<static_cast<int>(nEndPar); i++ ) {
    p[i]=fETofCal[id]->getP(i);
  }

  log << MSG::DEBUG << "ETime   id =" << id  << "  1=" << p[0] << " 2=" << p[1] << " 3=" << p[2] << " 4=" << p[3]  << " 5=" << p[4] << " 6=" << p[5] << " 7=" << p[6] << endreq;

  double time=TDC-(p[0]+p[1]/TMath::Sqrt(ADC)+p[2]/ADC+p[3]*ADC+p[4]*rHit+p[5]*rHit*rHit+p[6]*rHit*rHit*rHit);

  if(time<0.) {
    MsgStream log(msgSvc(), name());
    log<<MSG::WARNING<<"TofCaliSvc::ETime() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" r = "<< rHit <<" ID = "<<id<<endreq;
  }

  return time;
}


const double TofCaliSvc::ESigma(double r, unsigned id) {
  MsgStream log(msgSvc(), name());

  double fp[nEndSigma];
  for( int i=0; i<static_cast<int>(nEndSigma); i++) {
    fp[i]=fETofCal[id]->getFPCounter(i);  
  }

  log << MSG::DEBUG << "ESigma   id =" << id  << "  1=" << fp[0] << " 2=" << fp[1] << " 3=" << fp[2] << endreq;

  double sigma = fp[0] + fp[1]*r + fp[2]*r*r;

  return sigma;
}


const double TofCaliSvc::EtfTime(double ADC1,double ADC2,double TDC1,double TDC2,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double q = (ADC1+ADC2)/2.0;
  double t = (TDC1+TDC2)/2.0;

  double p[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p[i]=fEtfCal[12*id+strip]->getP(i);
  }

  double tcorr = p[0] 
               + p[1]/TMath::Sqrt(q)
               + p[2]/q
               + p[3]*q
               + p[4]*q*q
               + p[5]*q*q*q
               + p[6]*q*q*q*q;
  double time = t - tcorr;

  int ibunch = static_cast<int>(t0/(8.0*12000./499.8/8./4. )+0.1)%4;
  double pbunch = EtfBunchP(ibunch);

  time = time - pbunch;

  log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << "  1=" << p[0] << " 2=" << p[1] << " 3=" << p[2] << " 4=" << p[3]  << " 5=" << p[4] << " 6=" << p[5] << " 7=" << p[6] << " 8=" << p[7] << " 9=" << p[8] << " tcorr=" << tcorr << "    t0=" << t0 << "  pbunch=" << pbunch << "    time=" << time << endreq;

  return time;
}


const double TofCaliSvc::EtfTime1(double ADC,double TDC,double z,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double p1[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p1[i]=fEtfCal[12*id+strip]->getP1(i);
  }

  double tcorr1 = p1[0] 
                + p1[1]/TMath::Sqrt(ADC)
                + p1[2]*ADC
                + p1[3]*ADC*ADC
                + p1[4]*ADC*ADC*ADC
                + p1[5]*ADC*ADC*ADC*ADC
                + p1[6]*z/TMath::Sqrt(ADC)
                + p1[7]/ADC
                + p1[8]*z/ADC
                + p1[9]*z*ADC
                + p1[10]*z
                + p1[11]*z*z
                + p1[12]*z*z*z;
  double time1 = TDC - tcorr1;

  int ibunch = static_cast<int>(t0/(8.0*12000./499.8/8./4. )+0.1)%4;
  double pbunch = EtfBunchP(ibunch);

  time1 = time1 - pbunch;

  log << MSG::DEBUG << "EtfTime1   module =" << id  << " strip=" << strip << "  1=" << p1[0] << " 2=" << p1[1] << " 3=" << p1[2] << " 4=" << p1[3]  << " 5=" << p1[4] << " 6=" << p1[5] << " 7=" << p1[6] << " 8=" << p1[7] << " 9=" << p1[8] << " tcorr=" << tcorr1 << "    t0=" << t0 << "  pbunch=" << pbunch << "    time1=" << time1 << endreq;

  return time1;
}


const double TofCaliSvc::EtfTime2(double ADC,double TDC,double z,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double p2[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p2[i]=fEtfCal[12*id+strip]->getP2(i);
  }

  double tcorr2 = p2[0]
                + p2[1]/TMath::Sqrt(ADC)
                + p2[2]*ADC
                + p2[3]*ADC*ADC
                + p2[4]*ADC*ADC*ADC
                + p2[5]*ADC*ADC*ADC*ADC
                + p2[6]*z/TMath::Sqrt(ADC)
                + p2[7]/ADC
                + p2[8]*z/ADC
                + p2[9]*z*ADC
                + p2[10]*z
                + p2[11]*z*z
                + p2[12]*z*z*z;
  double time2 = TDC - tcorr2;

  int ibunch = static_cast<int>(t0/(8.0*12000./499.8/8./4. )+0.1)%4;
  double pbunch = EtfBunchP(ibunch);

  time2 = time2 - pbunch;

  log << MSG::DEBUG << "EtfTime2   module =" << id  << " strip=" << strip << "  1=" << p2[0] << " 2=" << p2[1] << " 3=" << p2[2] << " 4=" << p2[3]  << " 5=" << p2[4] << " 6=" << p2[5] << " 7=" << p2[6] << " 8=" << p2[7] << " 9=" << p2[8] << " tcorr=" << tcorr2 << "    t0=" << t0 << "  pbunch=" << pbunch << "    time2=" << time2 << endreq;

  return time2;
}


const double TofCaliSvc::EtfTimeMC(double ADC1,double ADC2,double TDC1,double TDC2,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double q = (ADC1+ADC2)/2.0;
  double t = (TDC1+TDC2)/2.0;

  double p[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p[i]=fEtfCal[12*id+strip]->getP(i);
  }

  double tcorr = p[0] + p[1]/TMath::Sqrt(q);
  double time = t - tcorr;

  log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << "  1=" << p[0] << " 2=" << p[1] << " tcorr=" << tcorr << "    t0=" << t0 << "    time=" << time << endreq;

  return time;
}


const double TofCaliSvc::EtfTimeMC1(double ADC,double TDC,double z,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double p1[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p1[i]=fEtfCal[12*id+strip]->getP1(i);
  }

  double tcorr1 = p1[0] + p1[1]/TMath::Sqrt(ADC) + p1[2]*z;
  double time1 = TDC - tcorr1;

  log << MSG::DEBUG << "EtfTime1   module =" << id  << " strip=" << strip << "  1=" << p1[0] << " 2=" << p1[1] << " 3=" << p1[2] << " tcorr=" << tcorr1 << "    t0=" << t0 << "    time1=" << time1 << endreq;

  return time1;
}


const double TofCaliSvc::EtfTimeMC2(double ADC,double TDC,double z,unsigned id,unsigned int strip,double t0){
  MsgStream log(msgSvc(), name());    

  double p2[nEtfPar];
  for( int i=0; i<static_cast<int>(nEtfPar); i++ ) {
    p2[i]=fEtfCal[12*id+strip]->getP2(i);
  }

  double tcorr2 = p2[0] + p2[1]/TMath::Sqrt(ADC) + p2[2]*z;
  double time2 = TDC - tcorr2;

  log << MSG::DEBUG << "EtfTime2   module =" << id  << " strip=" << strip << "  1=" << p2[0] << " 2=" << p2[1] << " 3=" << p2[2] << " tcorr=" << tcorr2 << "    t0=" << t0 << "    time2=" << time2 << endreq;

  return time2;
}


const double TofCaliSvc::EtfTime(double tleft,double tright){
  MsgStream log(msgSvc(), name());    

  double time = (tleft+tright)/2.0;
  log << MSG::DEBUG << "EtfTime   tleft=" << tleft << " tright=" << tright << " time=" << time << endreq;

  return time;
}


const double TofCaliSvc::EtfBunchP(int index){
  MsgStream log(msgSvc(), name());    

  double pbunch = 0.0;
  if( fEtfBunchCal.size()>0 && index>=0 && index<4 ) {
    pbunch=fEtfBunchCal[0]->getBunchP(index);
  }

  return pbunch;
}


const double TofCaliSvc::BPulseHeight1( double ADC, double zHit, double sint, unsigned int id ) {
  double length = 230.0;
  double A = 1.0;
  double Latten = fBTofCal[id]->getAtten(0);
  double q0 = ADC*sint*TMath::Exp((0.5*length-zHit)/Latten)/A;
  return q0;
}


const double TofCaliSvc::BPulseHeight2( double ADC, double zHit, double sint, unsigned int id ) {
  double length = 230.0;
  double A = 1.0;
  double Latten = fBTofCal[id]->getAtten(0);
  double q0 = ADC*sint*TMath::Exp((0.5*length+zHit)/Latten)/A;
  return q0;
}


const double TofCaliSvc::BPulseHeight( double ADC1, double ADC2, double zHit, double sint, unsigned int id ) {
  double length = 230.0;
  double Latten    = fBTofCal[id]->getAtten(0);
  double gainRatio = fBTofCal[id]->getAtten(1);
  double A2        = fBTofCal[id]->getAtten(2);
  double q = ( ADC1*sint*TMath::Exp((0.5*length-zHit)/Latten)+ADC2*sint*TMath::Exp((0.5*length+zHit)/Latten) )/( A2*(1.0+TMath::Exp(gainRatio)) );
  return q;
}


const double TofCaliSvc::BPh( double ADC1, double ADC2, double zHit, unsigned int id ) {
  double length = 230.0;
  double Latten    = fBTofCal[id]->getAtten(0);
  double gainRatio = fBTofCal[id]->getAtten(1);
  double q = ( ADC1*TMath::Exp((0.5*length-zHit)/Latten)+ADC2*TMath::Exp((0.5*length+zHit)/Latten) )/(1.0+TMath::Exp(gainRatio));
  return q;
}


const double TofCaliSvc::EPulseHeight( double ADC, double rHit, double cost, unsigned int id ) {
  double length = 44.5;
  double a0 = fETofCal[id]->getAtten(0);
  double a1 = fETofCal[id]->getAtten(1);
  double a2 = fETofCal[id]->getAtten(2);
  double a3 = fETofCal[id]->getAtten(3);

  double q0 = ADC*fabs(cost)*TMath::Exp(-(a1*(rHit-length)+a2*(rHit-length)*(rHit-length)))/a3;

  return q0;
}


const double TofCaliSvc::EPh( double ADC, double rHit, unsigned int id ) {
  double length = 44.5;
  double a0 = fETofCal[id]->getAtten(0);
  double a1 = fETofCal[id]->getAtten(1);
  double a2 = fETofCal[id]->getAtten(2);
  double q0 = ADC*TMath::Exp(-(a0+a1*(rHit-length)+a2*(rHit-length)*(rHit-length)));
  return q0;
}


const double TofCaliSvc::getQ0(unsigned id){
  return fBTofCal[id]->getAtten(3);
}


const double TofCaliSvc::BVeff(unsigned id){
  if(id<176){ 
    return fBTofCal[id]->getVeff(0);
  }else{
    std::cout<<"bad id="<<id<<std::endl;
    return 0.;}
}


const double TofCaliSvc::BAtten(unsigned id){
  if( id > 175 ) return -1;
  return fBTofCal[id]->getAtten(0);
}

const double  TofCaliSvc::BGainForward(unsigned id){
  if( id > 175 ) return -1;
  double gainRatio = fBTofCal[id]->getAtten(1);
  double A2        = fBTofCal[id]->getAtten(2);
  double A1        = A2*TMath::Exp(gainRatio);
  return A1;
}

const double  TofCaliSvc::BGainBackward(unsigned id){
  if( id > 175 ) return -1;
  return fBTofCal[id]->getAtten(2);
}

const double TofCaliSvc::EVeff(unsigned id){
  if( id > 175 ) return -1;
  return fETofCal[id]->getVeff(0);
}

const double TofCaliSvc::EtfVeff(unsigned int id, unsigned int strip){
  if(id<72 && strip<12){ 
    return fEtfCal[12*id+strip]->getVeff(0);
  }else{
    std::cout<<"bad id="<<id<<" strip="<<strip<<std::endl;
    return 0.;
  }
}


const double TofCaliSvc::ZTDC(double tleft, double tright, unsigned id) {
  double ztdc = -1000.0;
  if( id > 175 ) return ztdc;
  double veff  = fBTofCal[id]->getVeff(0);
  double delay = fBTofCal[id]->getVeff(1);
  ztdc  = 0.5*veff*(tright-tleft-delay);
  return ztdc;
}

const double TofCaliSvc::ZTDC1(double tright, unsigned id, double z) {
  double tleft = -1000.0;
  if( id > 175 ) return tleft;
  if( tright < -900.0 ) return tleft;
  double veff  = fBTofCal[id]->getVeff(0);
  double delay = fBTofCal[id]->getVeff(1);
  tleft = tright-delay-2.0*z/veff;
  return tleft;
}

const double TofCaliSvc::ZTDC2(double tleft, unsigned id, double z) {
  double tright = -1000.0;
  if( id > 175 ) return tright;
  if( tleft < -900.0 ) return tright;
  double veff   = fBTofCal[id]->getVeff(0);
  double delay  = fBTofCal[id]->getVeff(1);
  tright = tleft+delay+2.0*z/veff;
  return tright;
}

const double TofCaliSvc::ZADC(double qleft, double qright, unsigned id) {
  double zadc = -1000.0;
  if( id > 175 ) return zadc;
  if( qright < 1.0e-6 ) return zadc;
  double ldecay   = fBTofCal[id]->getAtten(0);
  double a1overa2 = fBTofCal[id]->getAtten(1);
  double q1overq2 = log(qleft/qright);
  zadc     = 0.5*ldecay*( q1overq2 - a1overa2 );
  return zadc;
}

const double TofCaliSvc::ZADC1(double qright, unsigned id, double z) {
  double qleft = -1000.0;
  if( id > 175 ) return qleft;
  if( qright < 1.0e-6 ) return qleft;
  double ldecay   = fBTofCal[id]->getAtten(0);
  double a1overa2 = fBTofCal[id]->getAtten(1);
  double q1overq2 = 2.0*z/ldecay + a1overa2;
  qleft    = qright*exp(q1overq2);
  return qleft;
}

const double TofCaliSvc::ZADC2(double qleft, unsigned id, double z) {
  double qright = -1000.0;
  if( id > 175 ) return qright;
  if( qleft < 1.0e-6 ) return qright;
  double ldecay   = fBTofCal[id]->getAtten(0);
  double a1overa2 = fBTofCal[id]->getAtten(1);
  double q1overq2 = 2.0*z/ldecay + a1overa2;
  qright   = qleft*exp(-q1overq2);
  return qright;
}

const double TofCaliSvc::EtfZTDC(double tleft, double tright, unsigned id, unsigned int strip) {
  double ztdc = -1000.0;
  if( id>71 || strip>11 ) return ztdc;
  double delay = fEtfCal[12*id+strip]->getVeff(0);
  double veff  = fEtfCal[12*id+strip]->getVeff(1);
  ztdc = (tright-tleft-delay)/veff;
  return ztdc;
}


const double TofCaliSvc::EAtten(unsigned id){
  if( id > 95 ) return -1;
  return fETofCal[id]->getAtten(0);
}

const double TofCaliSvc::EGain(unsigned id){
  if( id > 95 ) return -1;
  return fETofCal[id]->getAtten(1);
}

const int TofCaliSvc::RunBegin() {
  return fTofInfoCal[0]->getRunBegin();
}
	
const int TofCaliSvc::RunEnd() {
  return fTofInfoCal[0]->getRunEnd();
}
	
const int TofCaliSvc::Version() {
  return fTofInfoCal[0]->getVersion();
}
	
const int TofCaliSvc::QCorr() {
  return fTofInfoCal[0]->getQCorr();
}

const int TofCaliSvc::QElec() {
  return fTofInfoCal[0]->getQElec();
}

const int TofCaliSvc::MisLable() {
  return fTofInfoCal[0]->getMisLable();
}

const int TofCaliSvc::BrEast(unsigned int im) {
  return fTofInfoCal[0]->getBrEast(im);
}

const int TofCaliSvc::BrWest(unsigned int im) {
  return fTofInfoCal[0]->getBrWest(im);
}

const int TofCaliSvc::Endcap(unsigned int im) {
  return fTofInfoCal[0]->getEndcap(im);
}

const int TofCaliSvc::Etf(unsigned int im) {
  return fTofInfoCal[0]->getEndcap(im);
}

BTofCal* TofCaliSvc::BTof(unsigned id) const{
  return  fBTofCal[id];
}

ETofCal* TofCaliSvc::ETof(unsigned id) const{
  return  fETofCal[id];
}    

EtfCal* TofCaliSvc::EtfTof(unsigned id) const{
  return  fEtfCal[id];
}    

BTofCommonCal* TofCaliSvc::BTofCommon() const{
  return  fBTofCommonCal[0];
}

TofInfoCal* TofCaliSvc::TofInfo() const{
  return  fTofInfoCal[0];
}    
	
void TofCaliSvc::Dump(){
  std::cout<<"Now We can get the TOF Calibtration Service"<<std::endl;
  std::cout<<"Barrel TOF Counter Number = "<<fBTofCal.size()<<std::endl;
  std::cout<<"Endcap TOF Counter Number = "<<fETofCal.size()<<std::endl;

  cout<<"The O  Barrel TOF PMT1 p1[0]  is"<<" "<<BTof(0)->getP1(0)<<endl; 
  cout<<"The 0  Barrel TOF PMT1 p1[1]  is"<<" "<<BTof(0)->getP1(1)<<endl; 
  cout<<"The 0  Barrel TOF veff  is"<<" "<<BTof(0)->getVeff(0)<<endl; 
  //  cout<<"The 22rd  Endcap TOF PMT  veff  is"<<" "<<ETof(22)->getVeff()<<endl; 
}

void TofCaliSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "New Run" << endreq;
    //    if(!m_hasbeeninitialized){
    StatusCode sc= FillfromDatabase();
    if(sc.isSuccess()){
      m_hasbeeninitialized=true;
    }
    //    }
  }
}
