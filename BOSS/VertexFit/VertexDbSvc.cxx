//     This service is used to read the vertex information from the database
//
//     an example to use this service is shown in test/test_read.cxx
//
//    the joboption for the example is shown in share/job-test.txt
//  
//     the joboption for this service is shown in  share/jobOptions_VertexDbSvc.txt


#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifndef BEAN
#include "VertexFit/VertexDbSvc.h"
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"


#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include <TMath.h>
#else
#include "VertexFit/VertexDbSvc.h"
#endif

using namespace std;


#ifndef BEAN
VertexDbSvc::VertexDbSvc( const string& name, ISvcLocator* svcloc) :
  Service (name, svcloc){
  // declare properties
  declareProperty("Host" , host = std::string("bes3db2.ihep.ac.cn"));
  declareProperty("DbName" , dbName = std::string("offlinedb"));
  declareProperty("UserName" , userName = std::string("guest"));
  declareProperty("Password" , password = std::string("guestpass"));
  declareProperty("BossVer" , m_bossver = std::string("default"));
  declareProperty("VerPar" , m_verpar = std::string("default"));
  declareProperty("BossRelease",m_bossRelease = std::string("default"));
}

VertexDbSvc::~VertexDbSvc(){
}

StatusCode VertexDbSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
     if( IID_IVertexDbSvc.versionMatch(riid) ){
	  *ppvInterface = static_cast<IVertexDbSvc*> (this);
     } else{
	  return Service::queryInterface(riid, ppvInterface);
     }
     return StatusCode::SUCCESS;
}

StatusCode VertexDbSvc::initialize(){
  MsgStream log(messageService(), name());
  log << MSG::INFO << "VertexDbSvc::initialize()" << endreq;

  StatusCode sc = Service::initialize();
  if( sc.isFailure() ) return sc;
  
   
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
  return StatusCode::SUCCESS;
}

StatusCode VertexDbSvc::finalize(){
     MsgStream log(messageService(), name());
     log << MSG::INFO << "VertexDbSvc::finalize()" << endreq;
   //  if(m_connect_offline) delete m_connect_offline;
    return StatusCode::SUCCESS;
}

void VertexDbSvc::handle(const Incident& inc){
     MsgStream log( messageService(), name() );
     log << MSG::DEBUG << "handle: " << inc.type() << endreq;

     if ( inc.type() == "NewRun" ){
	  log << MSG::DEBUG << "NewRun" << endreq;
         m_bossver = "default";
         getVertexTableInfo();
     } 
}


#else
// -------------------------- BEAN ------------------------------------

VertexDbSvc* VertexDbSvc::m_vdb = 0;

//-----------------------------------------------------------------------------
VertexDbSvc::VertexDbSvc()
//-----------------------------------------------------------------------------
{
  // use functions instead of "declareProperty"
  dbName    = "offlinedb";
#ifdef ROOTEVENTDATAVERSION
  m_bossver = ROOTEVENTDATAVERSION;
  m_bossRelease = ROOTEVENTDATAVERSION;
#else
  m_bossver = "default";
  m_bossRelease = "default";
#endif
  m_verpar  = "default";
  
  m_dbsvc = DatabaseSvc::instance();
}

//-----------------------------------------------------------------------------
void VertexDbSvc::handle(int new_run)
//-----------------------------------------------------------------------------
{
  static int save_run = 0;
  if( new_run == save_run ) return;

  cout << "Begin New Run " << new_run << endl;
  getVertexTableInfo(new_run);
  save_run = new_run;
}
#endif

double* VertexDbSvc::PrimaryVertex() 
{
  if( !m_isRunNumberValid ) {
    cerr << "WARNING in VertexDbSvc: runNo is invalid!\n";
    memset(m_primaryVertex,0,sizeof(m_primaryVertex));
  }
  return m_primaryVertex;
}

double* VertexDbSvc::SigmaPrimaryVertex() 
{
  if( !m_isRunNumberValid ) {
    cerr << "WARNING in VertexDbSvc: runNo is invalid!\n";
    memset(m_sigmaPrimaryVertex,0,sizeof(m_sigmaPrimaryVertex));
  }
  return m_sigmaPrimaryVertex;
}

#ifndef BEAN
StatusCode VertexDbSvc::getVertexTableInfo(){
  MsgStream log(messageService(), name());
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
#else
//-----------------------------------------------------------------------------
void VertexDbSvc::getVertexTableInfo(int run)
//-----------------------------------------------------------------------------
{
#endif
  m_isRunNumberValid = false;
  int save_run = run;

  if( run < 0 ) {
#ifndef BEAN
    log << MSG::INFO << "This data is the MC sample with the Run Number: " << run << endreq;
#else
    cout << "This data is the MC sample with the Run Number: " << run << endl;
#endif
    run = -run;
  }
//#ifndef BEAN
//  if(m_bossver=="default") m_bossver = getenv("BES_RELEASE");
//#endif

  bool ret_vtx = getReadBunchInfo(run);

  if( !ret_vtx ) {
    cout << " VertexDbSvc:: can not found vertex information for run:"
         << run << ", boss version " << m_bossver << endl;
    exit(1);
  }

/*  if( !ret_vtx && save_run<0 ) {
    bool ret = false;
    int real_run = run;
    for(int kk = 1; kk <= 10000; kk++) {
       real_run = run+kk;
       if( (ret = getReadBunchInfo(real_run)) ) break;

       if( run-kk > 0 ) { 
         real_run = run-kk;
         if( (ret = getReadBunchInfo(real_run)) ) break;
       }
    }
    if( !ret ) {
#ifndef BEAN
      log << MSG::ERROR << "Can not find vertex information for run:" <<run<< endreq;
#else
      cout << "Can not find vertex information for run:" << run << endl;
#endif
      exit(1);
    }
#ifndef BEAN
    log << MSG::INFO << "Use Bunch infor. of run " << real_run 
        << " instead of run " << run << endreq;
#else
    cout << "Use Bunch infor. of run " << real_run
         << " instead of run " << run << endl;
#endif
  }
*/
  m_isRunNumberValid = true;
#ifndef BEAN
  log << MSG::INFO << "Successfully fetch the vertex information for run: " 
      << save_run << endreq;
  return StatusCode::SUCCESS;
#else
  cout << "Successfully fetch the vertex information for run: " 
       << save_run << endl;
#endif
}

//-----------------------------------------------------------------------------
bool VertexDbSvc::getReadBunchInfo(int run)
//-----------------------------------------------------------------------------
{
  if(m_bossver == "default") {
    if(m_bossRelease == "default") {
#ifndef BEAN
      MsgStream log(messageService(), name());
      log << MSG::FATAL << "ERROR BossRelease must be set! Current value is " 
          << m_bossRelease << "." << endreq;
#else
      cout << "ERROR BossRelease must be set! Current value is " 
           << m_bossRelease << "." << endl;
#endif
      exit(1);
    }
    else {
      char stmt1[400];
      sprintf(stmt1, "select SftVer, ParVer from CalVtxLumVer where BossRelease = '%s' and RunFrom <= %d and RunTo >= %d and DataType = 'LumVtx'", m_bossRelease.c_str(), run, run);

      DatabaseRecordVector records;
      int rowNo = m_dbsvc->query("offlinedb",stmt1,records);
      if(rowNo == 0) {
#ifndef BEAN
        MsgStream log(messageService(), name());
        log << MSG::ERROR << "ERROR: can not find records for run = " << run 
            << " and BossRelease = " << m_bossRelease << endreq;
#else
        cout << "ERROR: can not find records for run = " << run 
             << " and BossRelease = " << m_bossRelease << endl;
#endif
        exit(1);
      }
      DatabaseRecord* recordst = records[0];
      m_bossver = recordst->GetString("SftVer");
      m_verpar =  recordst->GetString("ParVer");
      cout << "Using the SftVer and ParVer (" << m_bossver 
           << ", " << m_verpar << ") for run " << run << ". " << endl;
    }
  }

  string stmt = "select Vx, Vy, Vz, SigmaVx, SigmaVy, SigmaVz ";
  stringstream tmp;
  tmp << "from BeamPar where RunNo = " << run
      << " and SftVer=\'" << m_bossver << "\'";
  if( m_verpar == "default" ) {
    tmp << " group by ParVer";
  } else {
    tmp << " and ParVer = " << m_verpar;
  }
  stmt += tmp.str();
  // cerr << "query(" << dbName << ", " << stmt << ", res);" << endl;

  DatabaseRecordVector res;
  int row_no = m_dbsvc->query(dbName,stmt,res);

  if( row_no > 0 ) {
    DatabaseRecord& dbrec = *res[row_no-1];
    m_primaryVertex[0] = dbrec.GetDouble("Vx");
    m_primaryVertex[1] = dbrec.GetDouble("Vy");
    m_primaryVertex[2] = dbrec.GetDouble("Vz");
    m_sigmaPrimaryVertex[0] = dbrec.GetDouble("SigmaVx");
    m_sigmaPrimaryVertex[1] = dbrec.GetDouble("SigmaVy");
    m_sigmaPrimaryVertex[2] = dbrec.GetDouble("SigmaVz");
    return true;
  }
  
  return false;
}
