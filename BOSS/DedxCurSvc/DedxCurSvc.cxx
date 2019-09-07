//********************************************************************
// DedxCurSvc package
// This Service is used for TOF measured pulse height correction.
// For BESIII, from run 7991 to run 10881, 
// TOF electronics QTC correction are done online.
// This service is used for convert the corrected Q back to RAW Q
//                      Sun Shengsen   2009-9-17
//********************************************************************
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "DedxCurSvc/DedxCurSvc.h"
#include "TTree.h"
#include "TBuffer.h"
#include "TBufferFile.h"

#include <math.h>

using namespace std; 

DedxCurSvc::DedxCurSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
declareProperty("BossRelease",m_bossRelease="default");
declareProperty("BossVer",m_sftver="default");
declareProperty("ParVer",m_calParVer="default");
declareProperty("DbStatus",m_dbStatus="OK");
declareProperty("Type",m_type="default");
}


StatusCode DedxCurSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_IDedxCurSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<IDedxCurSvc*> (this); 
  } 
  else return Service::queryInterface(riid, ppvInterface) ; 
  return StatusCode::SUCCESS;
}


StatusCode DedxCurSvc::initialize ( ) {
  
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
  

   sc = serviceLocator()->service("DatabaseSvc",m_dbsvc,true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find DatabaseSvc " << endreq;
    return sc;
  }


  sc = serviceLocator()->service("EventDataSvc", m_eventSvc, true);
  if (sc .isFailure() ) {
    log << MSG::ERROR << "Unable to find EventDataSvc " << endreq;
    return sc;
  }
  
  StatusCode scc;
  log<<MSG::INFO << "setProperties()" << endreq;
  scc = setProperties();
 
  m_runFromMax=0;
  m_runToMin=0;

  return sc;
}  

  
StatusCode DedxCurSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}


DedxCurSvc::~DedxCurSvc(){
}



void DedxCurSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "New Run:" << run << endreq;
    getDedxCurveInfo();
  }
}


void DedxCurSvc::getDedxCurveInfo(){
  MsgStream log(messageService(), name());
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  if(run<0){
    log << MSG::INFO << "This data is the MC sample with the Run Number: " << run << endreq;
    if(m_type=="Sim"){
      run=-run;
      log << MSG::INFO << "Reverse the sign of Run Number" << endreq;
    }
  }
  //  unsigned long *lengths;
  MYSQL_RES *res_set;
  MYSQL_ROW row;
  char stmt1[400];

  std::string sftver;
  
  if(m_sftver == "default" && m_bossRelease == "default")
  {
    log << MSG::FATAL << " BossRelease and BossVer for DedxCurve not set!" << endreq;
    exit(1);
  }

  if( (run<m_runFromMax || run>m_runToMin) ) 
  { 
    if(m_sftver == "default")
    {
      sprintf(stmt1,"select RunFrom, RunTo, SftVer, ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType = 'DedxCurve'", m_bossRelease.c_str(), run, run);
      std::cout<<stmt1<<std::endl;
      DatabaseRecordVector rest;
      int row_no = m_dbsvc->query("offlinedb",stmt1,rest);
      if(row_no==0) {
        log << MSG::FATAL << "can not find result for DedxCur with: " << stmt1 << endreq;
        exit(1);
      }
      if(row_no>1) {
        log << MSG::FATAL << "find more than 1 results for DedxCur with: " << stmt1 << endreq;
        exit(1);
      }
      DatabaseRecord* recordst = rest[0];
      sftver = recordst->GetString("SftVer");
      m_calParVer = recordst->GetString("ParVer");
      m_runFromMax = atoi((recordst->GetString("RunFrom")).c_str());
      m_runToMin = atoi((recordst->GetString("RunTo")).c_str());
      log << MSG::INFO << "get from CalVtxLumVer, m_runFromMax: "<< m_runFromMax << " m_runToMin: "<<m_runToMin<< endreq;
    }
    else
      sftver = m_sftver;
  
    if(m_calParVer!="default")
      sprintf(stmt1,"select RunFrom, RunTo, DedxCurvePar,DedxSigmaPar from DedxCurvePar where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and DedxCurveParVer = %s and Status='%s'",sftver.c_str(),run,run,m_calParVer.c_str(), m_dbStatus.c_str());
    else
      sprintf(stmt1,"select RunFrom, RunTo, DedxCurvePar,DedxSigmaPar from DedxCurvePar where SftVer = '%s' and RunFrom <= %d and RunTo >= %d and Status='%s' order by DedxCurveParVer desc",sftver.c_str(),run,run, m_dbStatus.c_str());
    std::cout<<stmt1<<std::endl; 
    
    DatabaseRecordVector res;
    int row_no  = m_dbsvc->query("offlinedb", stmt1, res);
    if(row_no==0){
     log << MSG::FATAL << "can not find result for DedxCur with: "<< stmt1 << endreq;
     exit(1);
    }
    if(row_no>1){
      log << MSG::FATAL << "find more than 1 results for DedxCur with: "<< stmt1 << endreq;
      exit(1);
    }
    
    DatabaseRecord* records = res[0];
    int runFrom = atoi((records->GetString("RunFrom")).c_str());
    int runTo = atoi((records->GetString("RunTo")).c_str());
    if(m_sftver=="default")
    {
      m_runFromMax = runFrom>m_runFromMax? runFrom:m_runFromMax;
      m_runToMin = runTo<m_runToMin? runTo:m_runToMin;
    }
    else
    {
      m_runFromMax = runFrom;
      m_runToMin = runTo;
    }
    log << MSG::INFO << "get from DedxCurvePar runFrom: "<< runFrom << " runTo: "<<runTo<< endreq;
    log << MSG::INFO << "m_runFromMax: "<< m_runFromMax << " m_runToMin: "<<m_runToMin<< endreq;

    TBuffer *buf1 = new TBufferFile(TBuffer::kRead);
    buf1->SetBuffer((*records)["DedxCurvePar"],327680,kFALSE);
    TBuffer *buf2 = new TBufferFile(TBuffer::kRead);
    buf2->SetBuffer((*records)["DedxSigmaPar"],327680,kFALSE);
    
    
    TTree* curvetree = new TTree(); 
    curvetree->Streamer(*buf1); 
    
    TTree* sigmatree = new TTree(); 
    sigmatree->Streamer(*buf2);
    
    double curve[50];
    double sigma[50];
    int SigmaSize;
    int CurveSize;
    curvetree -> SetBranchAddress("curve", curve);
    curvetree -> SetBranchAddress("CurveSize", &CurveSize);
    sigmatree -> SetBranchAddress("sigma", sigma);
    sigmatree -> SetBranchAddress("SigmaSize", &SigmaSize);
    
    Int_t nentries_curve = (Int_t)curvetree ->GetEntries();
    Int_t nentries_sigma = (Int_t)sigmatree ->GetEntries();
    curvetree -> GetEntry(0);
    if(CurveSize>50){
      log << MSG::ERROR<<"CurveSize larger than designed number"<<endreq;
      exit(1);}  
    for(int i=0; i<CurveSize;i++){
        m_curve[i]=curve[i];
       }
    
    sigmatree-> GetEntry(0);
    if(SigmaSize>50){
      log << MSG::ERROR<<"SigmaSize larger than designed number"<<endreq;
      exit(1);}
    for(int i=0; i<SigmaSize;i++){
        m_sigma[i]=sigma[i];}
      m_curve_size=CurveSize;
      m_sigma_size=SigmaSize;
     delete curvetree;
     delete sigmatree;
  }
   return;
}


