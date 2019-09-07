//     This service is used to read the TofEnergyCalibSvc  information from the database
//
//     an example to use this service is shown in test/test_read.cxx
//
//    the joboption for the example is shown in share/job-test.txt
//  
//     the joboption for this service is shown in  share/jobOptions_TofEnergyCalibSvc.txt


#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "TofEnergyCalibSvc/TofEnergyCalibSvc.h"
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

using namespace std;


TofEnergyCalibSvc::TofEnergyCalibSvc( const string& name, ISvcLocator* svcloc) :
	Service (name, svcloc){
		// declare properties
		declareProperty("Host" , host = std::string("bes3db2.ihep.ac.cn"));
		declareProperty("DbName" , dbName = std::string("offlinedb"));
		declareProperty("UserName" , userName = std::string("guest"));
		declareProperty("Password" , password = std::string("guestpass"));
		declareProperty("IsData" , m_IsData = std::string("default"));
		declareProperty("BossRelease",m_bossRelease = std::string("default"));
		//	m_dbsvc = DatabaseSvc::instance();
	}

TofEnergyCalibSvc::~TofEnergyCalibSvc(){
}

StatusCode TofEnergyCalibSvc::queryInterface(const InterfaceID& riid, void** ppvInterface){
	if( IID_ITofEnergyCalibSvc.versionMatch(riid) ){
		*ppvInterface = static_cast<ITofEnergyCalibSvc*> (this);
	} else{
		return Service::queryInterface(riid, ppvInterface);
	}
	return StatusCode::SUCCESS;
}

StatusCode TofEnergyCalibSvc::initialize(){
	MsgStream log(messageService(), name());
	log << MSG::INFO << "TofEnergyCalibSvc::initialize()" << endreq;

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

StatusCode TofEnergyCalibSvc::finalize(){
	MsgStream log(messageService(), name());
	log << MSG::INFO << "TofEnergyCalibSvc::finalize()" << endreq;
	//  if(m_connect_offline) delete m_connect_offline;
	return StatusCode::SUCCESS;
}

void TofEnergyCalibSvc::handle(const Incident& inc){
	MsgStream log( messageService(), name() );
	log << MSG::DEBUG << "handle: " << inc.type() << endreq;

	if ( inc.type() == "NewRun" ){
		log << MSG::DEBUG << "NewRun" << endreq;
		if(!getTofEnergyCalibSvcInfo()){
			log << MSG::ERROR << "can not initilize Tof energy Calib Constants" << endreq;
		}
	} 
}


bool TofEnergyCalibSvc::getTofEnergyCalibSvcInfo(){
	MsgStream log(messageService(), name());
	SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
	int  run = eventHeader->runNumber();
  cout<<"Run in getTofEnergyCalibSvcInfo() is: "<<run<<endl;

	char stmt1[400];
	if(run<0) {
		cout << "This data is the MC sample with the Run Number: " << run << endl;
		run = -run;
	}
	
	if(m_bossRelease=="default") m_bossRelease = getenv("BES_RELEASE");
	sprintf(stmt1, "select CalibConst,Para1,Para2,Para3,Para4,Para5 from TofEnergyCalib where BossVer = '%s' and RunFrom <= %d and RunTo >= %d and IsData = '%s'", m_bossRelease.c_str(), run, run, m_IsData.c_str());
	cout<<"stmt is:"<<stmt1<<endl;
	DatabaseRecordVector res;
	int row_no = m_dbsvc->query("offlinedb",stmt1,res);
	if( row_no > 0 ) {
		DatabaseRecord& dbrec = *res[row_no-1];
		m_CalibConst = dbrec.GetDouble("CalibConst");
		m_Para1 = dbrec.GetDouble("Para1");
		m_Para2 = dbrec.GetDouble("Para2");
		m_Para3 = dbrec.GetDouble("Para3");
		m_Para4 = dbrec.GetDouble("Para4");
		m_Para5 = dbrec.GetDouble("Para5");
		cout<<"m_CalibConst is:"<<m_CalibConst<<endl;
		cout << "Successfully fetch TofEnergyCalibSvc information for run: "
			<< run << endl;
		return true;
	} else if( row_no<=0 ) {
		cout << " TofEnergyCalibSvc:: can not found TofEnergyCalibSvc information of run:"
			<< run << ", boss version " << m_bossRelease << endl;
		exit(1);
		return false;
	}
	return true;
}

