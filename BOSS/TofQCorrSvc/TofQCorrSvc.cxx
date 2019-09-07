//********************************************************************
// TofQCorrSvc package
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
#include "CalibData/Tof/TofCalibData.h"
#include "CalibData/Tof/bTofCalibBase.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "TofQCorrSvc/TofQCorrSvc.h"
#include <math.h>

using namespace std; 

TofQCorrSvc::TofQCorrSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
}


StatusCode TofQCorrSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_ITofQCorrSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<ITofQCorrSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}


StatusCode TofQCorrSvc::initialize ( ) {
  
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
   m_runNo=0;
  return sc;
}  

  
StatusCode TofQCorrSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}


TofQCorrSvc::~TofQCorrSvc(){
}


const double TofQCorrSvc::getBarEast(int layer,int no) {
  return  m_Bar_East[layer][no];
}    
	

const double TofQCorrSvc::getBarWest(int layer,int no){
  return  m_Bar_West[layer][no];
} 


const double TofQCorrSvc::getEndcap(int layer,int no){
  return m_Endcap[layer][no];
}


void TofQCorrSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  if( run<0 || run>11000 ) return;

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "New Run:" << run << endreq;
    getTofQCorrInfo();
  }
}


void TofQCorrSvc::getTofQCorrInfo(){
  MsgStream log(messageService(), name());
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  if(run<0){
    log << MSG::INFO << "This data is the MC sample with the Run Number: " << run << endreq;
    run=-run;
  }
  //  unsigned long *lengths;
  char stmt1[200];
  sprintf(stmt1,"select TOFCalib "
	  "from RunParams where run_number = %d", run);

  DatabaseRecordVector res;
  int row_no = m_dbsvc->query("run",stmt1,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    exit(1);
  }
  long int runNo;
  DatabaseRecord* records1 = res[0];
  sscanf((*records1)["TOFCalib"], "%ld", &runNo);

  char stmt2[200];
  sprintf(stmt2,"select be,end,counter,a1,a2,a3 "
          "from TOF_Q where run_number = %ld", runNo);

  res.clear();
  row_no = m_dbsvc->query("run",stmt2,res);
  if(!row_no){
    log << MSG::ERROR<<"search errror"<<endreq;
    exit(1);
  }

 long int be,end,counter;
  double sql_result[3];

  for(int jjj=0;jjj<row_no;jjj++){
    DatabaseRecord* records2 = res[jjj];
    sscanf((*records2)["be"], "%ld", &be);
    sscanf((*records2)["end"], "%ld", &end);
    sscanf((*records2)["counter"], "%ld", &counter);
    sscanf((*records2)["a1"], "%lf", &sql_result[0]);
    sscanf((*records2)["a2"], "%lf", &sql_result[1]);
    sscanf((*records2)["a3"], "%lf", &sql_result[2]);
    if(be==0&&end==0){
      m_Endcap[counter][0]=sql_result[0];
      m_Endcap[counter][1]=sql_result[1];
      m_Endcap[counter][2]=sql_result[2];
    }
    if(be==1&&end==0){
      m_Bar_East[counter][0]=sql_result[0];
      m_Bar_East[counter][1]=sql_result[1];
      m_Bar_East[counter][2]=sql_result[2];
    }
    if(be==1&&end==1){
      m_Bar_West[counter][0]=sql_result[0];
      m_Bar_West[counter][1]=sql_result[1];
      m_Bar_West[counter][2]=sql_result[2];
      }
}
res.clear();
}


const double TofQCorrSvc::BQRaw1(int id, double q) {
  MsgStream log(msgSvc(), name());
  double qraw = -999.0;
  if( id<0 || id>175 ) {
    log << MSG::WARNING << "BQRaw1 :  TofId is out of range, tofid = " << id << endreq;
    return qraw;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }
  double a = m_Bar_East[id][2];
  double b = m_Bar_East[id][1];
  double c = m_Bar_East[id][0] - q/2.5;
  qraw = ( sqrt( b*b - 4.0*a*c ) - b )/( 2.0*a );

  return qraw;
}


const double TofQCorrSvc::BQRaw2(int id, double q) {
  MsgStream log(msgSvc(), name());
  double qraw = -999.0;
  if( id<0 || id>175 ) {
    log << MSG::WARNING << "BQRaw2 :  TofId is out of range, tofid = " << id << endreq;
    return qraw;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }
  double a = m_Bar_West[id][2];
  double b = m_Bar_West[id][1];
  double c = m_Bar_West[id][0] - q/2.5;
  qraw = ( sqrt( b*b - 4.0*a*c ) - b )/( 2.0*a );
  return qraw;
}


const double TofQCorrSvc::EQRaw(int id, double q) {
  MsgStream log(msgSvc(), name());
  double qraw = -999.0;
  if( id<0 || id>95 ) {
    log << MSG::WARNING << "EQRaw :  TofId is out of range, tofid = " << id << endreq;
    return qraw;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }
  double a = m_Endcap[id][2];
  double b = m_Endcap[id][1];
  double c = m_Endcap[id][0] - q/2.5;
  qraw = ( sqrt( b*b - 4.0*a*c ) - b )/( 2.0*a );
  return qraw;
}
