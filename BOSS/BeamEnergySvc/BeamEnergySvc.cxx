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
#include "BeamEnergySvc/BeamEnergySvc.h"

#include <math.h>

using namespace std; 


BeamEnergySvc::BeamEnergySvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc), 
  m_run(-1),
  m_isRunValid(false),
  m_beamE(0)
{
declareProperty("Host",m_host="bes3db2.ihep.ac.cn");
}

BeamEnergySvc::~BeamEnergySvc()
{
}

StatusCode BeamEnergySvc::initialize()
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << "@initialize()" << endreq;

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
  m_run=0;
  return sc;
}

StatusCode BeamEnergySvc::finalize()
{
  MsgStream log(messageService(), name());
  log << MSG::INFO << "@initialize()" << endreq;

  StatusCode sc = Service::finalize();

  return sc;
}

StatusCode BeamEnergySvc::queryInterface(const InterfaceID& riid, void** ppvIF)
{
  if ( IBeamEnergySvc::interfaceID().versionMatch(riid) ) {
	*ppvIF = dynamic_cast<IBeamEnergySvc*>(this);
  }
  else {
	return Service::queryInterface(riid, ppvIF);
  }
  addRef();
  return StatusCode::SUCCESS;
}

void BeamEnergySvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  //  if( run<0 || run>11000 ) return;

  if ( inc.type() == "NewRun" ){
	log << MSG::DEBUG << "New Run:" << run << endreq;
	getBeamEnergyInfo();
  }
}

bool BeamEnergySvc::isRunValid() {
  return m_isRunValid;
}

double BeamEnergySvc::getbeamE(){
  if (!isRunValid()){
	std::cerr << "ERROR in BeamEnergySvc: runNo is invalid!" << std::endl;
  }
  return m_beamE;
}

void BeamEnergySvc::getBeamEnergyInfo(){
  MsgStream log(messageService(), name());
  SmartDataPtr<Event::EventHeader> eventHeader(m_eventSvc,"/Event/EventHeader");
  int  run = eventHeader->runNumber();
  
  if(run<0){
	log << MSG::INFO << "This data is the MC sample with the Run Number: " << run << endreq;
	m_isRunValid = true;
	m_beamE = 1.8865;
	return ;
	//run=-run;
  }
  m_run = run;
  char stmt1[400];

  snprintf(stmt1, 1024,
	  "select BER_PRB, BPR_PRB "
	  "from RunParams where run_number = %d", run);

  DatabaseRecordVector res;
  int row_no  = m_dbsvc->query("run", stmt1, res);
  if(row_no==0){
	log << MSG::FATAL << "can not find result for BeamEnergySvc" << endreq;
	exit(1);
  }

  m_isRunValid = true;

  DatabaseRecord* records = res[0]; 
  double E_E=0, E_P=0;
  E_E = records->GetDouble("BER_PRB");
  E_P = records->GetDouble("BPR_PRB");
  m_beamE=(E_E+E_P)/2.0;

  return;
}
