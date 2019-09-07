//********************************************************
//
//  Class: EstTofCaliSvc 
//  Sun Shengsen <sunss@ihep.ac.cn>
//  
//  Description:
//     This service is used to get Tof Calibrarion constants
//     from DataBase and do the Tof data reconstruction.
//
//*********************************************************
#include "EstTofCaliSvc/EstTofCaliSvc.h"
#include "EstTofCaliSvc/BTofCal.h"
#include "EstTofCaliSvc/BTofCommonCal.h"
#include "EstTofCaliSvc/ETofCal.h"
#include "EstTofCaliSvc/EtfCal.h"
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
  
EstTofCaliSvc::EstTofCaliSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
  declareProperty("Run",m_run=1);
}

StatusCode EstTofCaliSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_IEstTofCaliSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<IEstTofCaliSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode EstTofCaliSvc::initialize ( ) {
  m_hasbeeninitialized=false;
  m_st=false;

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
  
StatusCode EstTofCaliSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}

EstTofCaliSvc::~EstTofCaliSvc(){
  for(vector<BTofCal*>::iterator it1 = fBTofCal.begin(); it1 != fBTofCal.end(); it1++) { delete (*it1); }
  fBTofCal.clear();
  for(vector<ETofCal*>::iterator it2 = fETofCal.begin(); it2 != fETofCal.end(); it2++) { delete (*it2); }
  fETofCal.clear();
  if( fEtfCal.size()!=0 ) {
    for(vector<EtfCal*>::iterator it5 = fEtfCal.begin(); it5 != fEtfCal.end(); it5++) { delete (*it5); }
    fEtfCal.clear();
  }
  for(vector<BTofCommonCal*>::iterator it3 = fBTofCommonCal.begin(); it3 != fBTofCommonCal.end(); it3++) { delete (*it3); }
  fBTofCommonCal.clear();
  return;
}
StatusCode EstTofCaliSvc::FillfromDatabase(){

  MsgStream log(msgSvc(), name());    
  std::string fullPath = "/Calib/EstTofCal";
  log << MSG::INFO<<" Tof calib fullPath = "<<fullPath<< endreq;
  SmartDataPtr<CalibData::TofCalibData> test(m_pCalibDataSvc, fullPath);
  if(!test){
    log << MSG::FATAL << "EstTofCaliSvc could not find TofCalibData in TCDS!!" << endreq;
    m_st=false;    
    return StatusCode::FAILURE;
  }       

  m_st=true;

  for(vector<BTofCal*>::iterator it1 = fBTofCal.begin(); it1 != fBTofCal.end(); it1++) { delete (*it1); }
  fBTofCal.clear();
  for(vector<ETofCal*>::iterator it2 = fETofCal.begin(); it2 != fETofCal.end(); it2++) { delete (*it2); }
  fETofCal.clear();
  if( fEtfCal.size()!=0 ) {
    for(vector<EtfCal*>::iterator it5 = fEtfCal.begin(); it5 != fEtfCal.end(); it5++) { delete (*it5); }
    fEtfCal.clear();
  }
  for(vector<BTofCommonCal*>::iterator it3 = fBTofCommonCal.begin(); it3 != fBTofCommonCal.end(); it3++) { delete (*it3); }
  fBTofCommonCal.clear();

  for(int ib =0;ib<176;ib++){
    BTofCal* btof = new BTofCal;
    for(int i=0;i<static_cast<int>(nBarPar);i++){
      btof->setP1(i,test->getBTofPleft(ib,i));
      btof->setP2(i,test->getBTofPright(ib,i));
    }
    fBTofCal.push_back(btof);
  }       

  for(int ie=0;ie<96;ie++){
    ETofCal* etof= new ETofCal;
    for(int i=0;i<static_cast<int>(nEndPar);i++){
      etof->setP(i,test->getETofP(ie,i));
    }
    fETofCal.push_back(etof);
  }

  for(int ietf=0;ietf<72;ietf++) {
    for(int jetf=0;jetf<12;jetf++) {
      EtfCal* etf= new EtfCal;
      for( int k=0; k<static_cast<int>(nEtfPar); k++ ) {
	etf->setP( k,test->getEtfPcombine(ietf,jetf,k));
	etf->setP1(k,test->getEtfPleft(ietf,jetf,k));
	etf->setP2(k,test->getEtfPright(ietf,jetf,k));
      }
      fEtfCal.push_back(etf);
    }
  }

  BTofCommonCal* bcomtof = new BTofCommonCal;
  for(int i=0; i<static_cast<int>(nBarOffset); i++) {
    bcomtof->setOffset( i, test->getBTofOffset(i) );
  }
  fBTofCommonCal.push_back(bcomtof);

  return StatusCode::SUCCESS;

}


const double EstTofCaliSvc::BTCorr1(double ADC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double p1[nBarPar];
  for( int i=0; i<static_cast<int>(nBarPar); i++ ) {
    p1[i]=fBTofCal[id]->getP1(i);
  }

  log << MSG::DEBUG << "BTCorr1   id =" << id  << "  1=" << p1[0] << " 2=" << p1[1] << " 3=" << p1[2] << " 4=" << p1[3]  << " 5=" << p1[4] << " 6=" << p1[5] << " 7=" << p1[6] << " 8=" << p1[7] << " 9=" << p1[8] << endreq;

  double tcorr1 = p1[0] + p1[1]/TMath::Sqrt(ADC) + p1[2]*z/TMath::Sqrt(ADC) + p1[3]/ADC + p1[4]*z + p1[5]*z*z + p1[6]*z*z*z;

  return tcorr1;
}


const double EstTofCaliSvc::BTCorr2(double ADC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double p2[nBarPar];
  for( int i=0; i<static_cast<int>(nBarPar); i++ ) {
    p2[i]=fBTofCal[id]->getP2(i);
  }

  log << MSG::DEBUG << "BTCorr2   id =" << id  << "  1=" << p2[0] << " 2=" << p2[1] << " 3=" << p2[2] << " 4=" << p2[3]  << " 5=" << p2[4] << " 6=" << p2[5] << " 7=" << p2[6] << " 8=" << p2[7] << " 9=" << p2[8] << endreq;

  double tcorr2 = p2[0] + p2[1]/TMath::Sqrt(ADC) + p2[2]*z/TMath::Sqrt(ADC) + p2[3]/ADC + p2[4]*z + p2[5]*z*z + p2[6]*z*z*z;

  return tcorr2;
}


const double EstTofCaliSvc::TOffset() {
  double toffset = fBTofCommonCal[0]->getOffset(0);
  return toffset;
}


const double EstTofCaliSvc::BTime1(double ADC,double TDC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double tcorr1 = BTCorr1( ADC, z, id );
  double tcorr3 = fBTofCommonCal[0]->getOffset(0);

  log << MSG::DEBUG << "BTime1   id =" << id  << "  tcorr=" << tcorr1 << "  total offset=" << tcorr3 << endreq;

  double time = TDC - tcorr1 - tcorr3;

  if(time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::BTime1() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<endreq;
  }
  return time;     
}


const double EstTofCaliSvc::BTime2(double ADC,double TDC,double z,unsigned id){
  MsgStream log(msgSvc(), name());    

  double tcorr1 = BTCorr2( ADC, z, id );
  double tcorr3 = fBTofCommonCal[0]->getOffset(0);

  log << MSG::DEBUG << "BTime2   id =" << id  << "  tcorr=" << tcorr1 << "  total offset=" << tcorr3 << endreq;

  double time = TDC - tcorr1 - tcorr3;

  if(time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::BTime2() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<endreq;
  }
  return time;     
}


const double EstTofCaliSvc::ETime(double ADC,double TDC,double rHit,unsigned id){
  MsgStream log(msgSvc(), name());

  double p[nEndPar];
  for( int i=0; i<static_cast<int>(nEndPar); i++ ) {
    p[i]=fETofCal[id]->getP(i);
  }

  log << MSG::DEBUG << "ETime   id =" << id  << "  1=" << p[0] << " 2=" << p[1] << " 3=" << p[2] << " 4=" << p[3]  << " 5=" << p[4] << " 6=" << p[5] << " 7=" << p[6] << endreq;

  double time=TDC-(p[0]+p[1]/TMath::Sqrt(ADC)+p[2]/ADC+p[3]*ADC+p[4]*rHit+p[5]*rHit*rHit+p[6]*rHit*rHit*rHit);

  if(time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::ETime() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" r = "<< rHit <<" ID = "<<id<<endreq;
  }

  return time;
}


const double EstTofCaliSvc::EtfTime(double ADC1,double ADC2,double TDC1,double TDC2,unsigned id,unsigned int strip){
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

  log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << "  1=" << p[0] << " 2=" << p[1] << " 3=" << p[2] << " 4=" << p[3]  << " 5=" << p[4] << " 6=" << p[5] << " 7=" << p[6] << " 8=" << p[7] << " 9=" << p[8] << " tcorr=" << tcorr << " time=" << time << endreq;
  if( time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::EtfTime() -- A minus time is given : "<< time <<" [Input] ADC1 = "<<ADC1<<" ADC2 = "<<ADC2<<" TDC1 = "<<TDC1<<" TDC2 = "<<TDC2<<" ID = "<<id<<" Strip = "<<strip<<endreq;
  }

  return time;
}


const double EstTofCaliSvc::EtfTime1(double ADC,double TDC,double z,unsigned id,unsigned int strip){
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

  log << MSG::DEBUG << "EtfTime1   module =" << id  << " strip=" << strip << "  1=" << p1[0] << " 2=" << p1[1] << " 3=" << p1[2] << " 4=" << p1[3]  << " 5=" << p1[4] << " 6=" << p1[5] << " 7=" << p1[6] << " 8=" << p1[7] << " 9=" << p1[8] << " tcorr=" << tcorr1 << " time1=" << time1 << endreq;
  if( time1<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::EtfTime1() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<" Strip = "<<strip<<endreq;
  }

  return time1;
}


const double EstTofCaliSvc::EtfTime2(double ADC,double TDC,double z,unsigned id,unsigned int strip){
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

  log << MSG::DEBUG << "EtfTime2   module =" << id  << " strip=" << strip << "  1=" << p2[0] << " 2=" << p2[1] << " 3=" << p2[2] << " 4=" << p2[3]  << " 5=" << p2[4] << " 6=" << p2[5] << " 7=" << p2[6] << " 8=" << p2[7] << " 9=" << p2[8] << " tcorr=" << tcorr2 << " time2=" << time2 << endreq;
  if( time2<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::EtfTime2() -- A minus time is given : "<< time <<" [Input] ADC = "<<ADC<<" TDC = "<<TDC<<" z = "<<z<<" ID = "<<id<<" Strip = "<<strip<<endreq;
  }

  return time2;
}


const double EstTofCaliSvc::EtfTime(double TDC1,double TDC2,unsigned id,unsigned int strip){
  MsgStream log(msgSvc(), name());    

  double time = -99;
  double t, tcorr;

  if( TDC1>0 && TDC2>0 ) {
    t = (TDC1+TDC2)/2.0;
    tcorr = fEtfCal[12*id+strip]->getP(0);
    time = t - tcorr;
    log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << " t1=" << TDC1 << " t2=" << TDC2 << "  combine const=" << tcorr << " time=" << time << endreq;
  }
  else if( TDC1>0 && TDC2<0 ) {
    t = TDC1;
    tcorr =  fEtfCal[12*id+strip]->getP1(0);
    time = t - tcorr;
    log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << " t1=" << TDC1 << "  left const=" << tcorr << " time=" << time << endreq;
  }
  else if( TDC1<0 && TDC2>0 ) {
    t = TDC2;
    tcorr =  fEtfCal[12*id+strip]->getP2(0);
    time = t - tcorr;
    log << MSG::DEBUG << "EtfTime   module =" << id  << " strip=" << strip << " t2=" << TDC2 << "  right const=" << tcorr << " time=" << time << endreq;
  }

  if( time<0.) {
    log<<MSG::WARNING<<"TofCaliSvc::EtfTime() -- A minus time is given : "<< time <<" [Input]  TDC1 = "<<TDC1<<" TDC2 = "<<TDC2<<" ID = "<<id<<" Strip = "<<strip<<endreq;
  }

  return time;
}


BTofCal* EstTofCaliSvc::BTof(unsigned id) const{
  return  fBTofCal[id];
}

ETofCal* EstTofCaliSvc::ETof(unsigned id) const{
  return  fETofCal[id];
}    

EtfCal* EstTofCaliSvc::EtfTof(unsigned id) const{
  return  fEtfCal[id];
}    

BTofCommonCal* EstTofCaliSvc::BTofCommon() const{
  return  fBTofCommonCal[0];
}
	
void EstTofCaliSvc::Dump(){
  std::cout<<"Now We can get the TOF Calibtration Service"<<std::endl;
  std::cout<<"Barrel TOF Counter Number = "<<fBTofCal.size()<<std::endl;
  std::cout<<"Endcap TOF Counter Number = "<<fETofCal.size()<<std::endl;

  cout<<"The O  Barrel TOF PMT1 p1[0]  is"<<" "<<BTof(0)->getP1(0)<<endl; 
  cout<<"The 0  Barrel TOF PMT1 p1[1]  is"<<" "<<BTof(0)->getP1(1)<<endl; 
}

void EstTofCaliSvc::handle(const Incident& inc){
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
