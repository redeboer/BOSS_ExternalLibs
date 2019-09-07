//  TofQElecSvc
//  Sun Shengsen 2009-9-17
//
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
//#include "GaudiKernel/ISvcFactory.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "TofQElecSvc/TofQElecSvc.h"
#include "TMath.h"

using namespace std;

TofQElecSvc::TofQElecSvc( const std::string& name, ISvcLocator* svcloc ) : Service(name, svcloc) {
  declareProperty("Run",m_run=1);
}


StatusCode TofQElecSvc::queryInterface (const InterfaceID& riid, void** ppvInterface ){
  
  if ( IID_ITofQElecSvc.versionMatch(riid) ) { 
    *ppvInterface = static_cast<ITofQElecSvc*> (this); 
  } else { 
    return Service::queryInterface(riid, ppvInterface) ; 
  }
  return StatusCode::SUCCESS;
}

StatusCode TofQElecSvc::initialize ( ) {
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
    log<<MSG::ERROR<<"Could not get IDataProviderSvc interface of QElecXmlCnvSvc"<<endreq;
    return scc;
  } else {
    log << MSG::DEBUG<< "Retrieved IDataProviderSvc interface of QElecXmlCnvSvc"<<endreq;
  }
  // Get properties from the JobOptionsSvc
  scc = setProperties();
  m_data=0;
  return sc;
}  
  
StatusCode TofQElecSvc::finalize ( ) {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << name() << ": End of Run" << endreq;
  return StatusCode::SUCCESS;
}


TofQElecSvc::~TofQElecSvc(){
  delete m_data;
  return;
}


StatusCode TofQElecSvc::FillfromDatabase(){

  MsgStream log(msgSvc(), name());    
  std::string fullPath = "/Calib/TofQElec";
  log << MSG::INFO<<" Tof calib fullPath = "<<fullPath<< endreq;
  SmartDataPtr<CalibData::TofElecData> test(m_pCalibDataSvc, fullPath);
  if(!test){
    log << MSG::FATAL << "TofQElecSvc could not find TofQElecData in TCDS!!" << endreq;
    return StatusCode::FAILURE;
  }       

  if(m_data) delete m_data;
  m_data = new CalibData::TofElecData;
  m_data->sim(test);
   
  /*
  for( vector<CalibData::BarrelElec*>::iterator iter1 = fBarrelElec.begin(); iter1 != fBarrelElec.end(); iter1++ ) {
    delete *iter1;
  }
  fBarrelElec.clear();
  for( vector<CalibData::EndcapElec*>::iterator iter2 = fEndcapElec.begin(); iter2 != fEndcapElec.end(); iter2++ ) {
    delete *iter2;
  }
  fEndcapElec.clear();

  for( unsigned ib=0; ib<176; ib++ ) {
    BarrelElec* bElec = new BarrelElec;
    for( unsigned int j=0; j<4; j++ ) {
      bElec->setNumEast( j, ib );
      bElec->setNumWest( j, ib );
    }
    for( unsigned int k=0; k<11; k++ ) {
      bElec->setP1( k, ib );
      bElec->setP2( k, ib );
      bElec->setSimP1( k, ib );
      bElec->setSimP2( k, ib );
    }
    fBarrelElec.push_back( bElec );
  }

  for( unsigned ie=0; ie<96; ie++ ) {
    EndcapElec* eElec = new EndcapElec;
    for( unsigned int j=0; j<4; j++ ) {
      eElec->setNum( j, ie );
    }
    for( unsigned int k=0; k<11; k++ ) {
      eElec->setP( k, ie );
      eElec->setSimP( k, ie );
    }
    fEndcapElec.push_back( eElec );
  }
  */

  return StatusCode::SUCCESS;
}


void TofQElecSvc::handle(const Incident& inc){
  MsgStream log( messageService(), name() );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;

  if ( inc.type() == "NewRun" ){
    log << MSG::DEBUG << "New Run" << endreq;
    StatusCode sc= FillfromDatabase();
  }
  return;
}


const unsigned int TofQElecSvc::Board( bool barrel, int id, bool eastEnd ) {
  MsgStream log(msgSvc(), name());    

  if( barrel ) {
    if( id<0 || id>175 ) {
      log << MSG::WARNING <<"Board : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }
  else {
    if( id<0 || id>95 ) {
      log << MSG::WARNING <<"Board : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }

  unsigned int board;
  if( barrel ) {
    if( eastEnd ) {
      board = (m_data->getBTof(id)).getNumEast(0);
    }
    else {
      board = (m_data->getBTof(id)).getNumWest(0);
    }
  }
  else {
    board = (m_data->getETof(id)).getNum(0);
  }

  return board;
}


const unsigned int TofQElecSvc::Crate( bool barrel, int id, bool eastEnd ) {
  MsgStream log(msgSvc(), name());    

  if( barrel ) {
    if( id<0 || id>175 ) {
      log << MSG::WARNING <<"Crate : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }
  else {
    if( id<0 || id>95 ) {
      log << MSG::WARNING <<"Crate : TofId is out of Range, tofid=" << id << endreq;
      return 1000;
    }
  }

  unsigned int crate;
  if( barrel ) {
    if( eastEnd ) {
      crate = (m_data->getBTof(id)).getNumEast(2);
    }
    else {
      crate = (m_data->getBTof(id)).getNumWest(2);
    }
  }
  else {
    crate = (m_data->getETof(id)).getNum(2);
  }

  return crate;
}


const unsigned int TofQElecSvc::Fee( bool barrel, int id, bool eastEnd ) {
  MsgStream log(msgSvc(), name());    

  if( barrel ) {
    if( id<0 || id>175 ) {
      log << MSG::WARNING <<"Fee : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }
  else {
    if( id<0 || id>95 ) {
      log << MSG::WARNING <<"Fee : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }

  unsigned int crate;
  if( barrel ) {
    if( eastEnd ) {
      crate = (m_data->getBTof(id)).getNumEast(3);
    }
    else {
      crate = (m_data->getBTof(id)).getNumWest(3);
    }
  }
  else {
    crate = (m_data->getETof(id)).getNum(3);
  }

  return crate;
}


const unsigned int TofQElecSvc::Channel( bool barrel, int id, bool eastEnd ) {
  MsgStream log(msgSvc(), name());    

  if( barrel ) {
    if( id<0 || id>175 ) {
      log << MSG::WARNING <<"Channel : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }
  else {
    if( id<0 || id>95 ) {
      log << MSG::WARNING <<"Channel : TofId is out of Range, tofid=" << id << endreq;  
      return 1000;
    }
  }

  unsigned int channel;
  if( barrel ) {
    if( eastEnd ) {
      channel = (m_data->getBTof(id)).getNumEast(1);
    }
    else {
      channel = (m_data->getBTof(id)).getNumWest(1);
    }
  }
  else {
    channel = (m_data->getETof(id)).getNum(1);
  }

  return channel;
}


const double TofQElecSvc::BQTC1(int id, double q) {
  double qtcNew = -999.0;
  MsgStream log(msgSvc(), name());    
  if( id<0 || id>175 ) {
    log << MSG::WARNING <<"BQTC1 : TofId is out of Range, tofid=" << id << endreq;
    return qtcNew;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }

  double ratio = (m_data->getBTof(id)).getP1(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getBTof(id)).getP1(i+1);
  }
  q = q*ratio;
  qtcNew = p[0] + p[1]*q + p[2]*q*q + p[3]*q*q*q + p[4]*q*q*q*q + p[7]*exp(p[5]*(q-p[6]));

  //  cout << "East tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<8; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qtcNew;
}


const double TofQElecSvc::BQTC2(int id, double q) {
  double qtcNew = -999.0;
  MsgStream log(msgSvc(), name());    
  if( id<0 || id>175 ) {
    log << MSG::WARNING <<"BQTC2 : TofId is out of Range, tofid=" << id << endreq;
    return qtcNew;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }

  double ratio = (m_data->getBTof(id)).getP2(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getBTof(id)).getP2(i+1);
  }
  q = q*ratio;
  qtcNew = p[0] + p[1]*q + p[2]*q*q + p[3]*q*q*q + p[4]*q*q*q*q + p[7]*exp(p[5]*(q-p[6]));

  //  cout << "West tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<8; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qtcNew;
}


const double TofQElecSvc::EQTC(int id, double q) {
  double qtcNew = -999.0;
  MsgStream log(msgSvc(), name());    
  if( id<0 || id>95 ) {
    log << MSG::WARNING <<"EQTC : TofId is out of Range, tofid=" << id << endreq;
    return qtcNew;
  }
  if( fabs(q-10000.0)<1.0e-6 ) { return 10000.0; }
  else if( fabs(q+999.0)<1.0e-6 ) { return -999.0; }

  double ratio = (m_data->getETof(id)).getP(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getETof(id)).getP(i+1);
  }
  q = q*ratio;
  qtcNew = p[0] + p[1]*q + p[2]*q*q + p[3]*q*q*q + p[4]*q*q*q*q + p[7]*exp(p[5]*(q-p[6]));

  //  cout << "Endcap tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<8; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qtcNew;
}


const double TofQElecSvc::BQChannel1(int id, double qtc) {
  double qChannel = -999.0;
  MsgStream log(msgSvc(), name());
  if( id<0 || id>175 ) {
    log << MSG::ERROR <<"BChannel1 : TofId is out of Range, tofid=" << id << endreq;  
    return qChannel;
  }

  double ratio = (m_data->getBTof(id)).getSimP1(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getBTof(id)).getSimP1(i+1);
  }
  qChannel = ( p[0] + p[1]*qtc + p[2]*qtc*qtc + p[3]*qtc*qtc*qtc )*( 1.0 - TMath::Erf((qtc-p[4])/p[5]) ) + ( p[6] + p[7]*qtc )*( 1.0 + TMath::Erf((qtc-p[8])/p[9]) );
  qChannel = ratio*qChannel;

  //  cout << "Sim East tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<11; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qChannel;
}


const double TofQElecSvc::BQChannel2(int id, double qtc) {
  double qChannel = -999.0;
  MsgStream log(msgSvc(), name());    
  if( id<0 || id>175 ) {
    log << MSG::ERROR <<"BChannel2 : TofId is out of Range, tofid=" << id << endreq;  
    return qChannel;
  }

  double ratio = (m_data->getBTof(id)).getSimP2(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getBTof(id)).getSimP2(i+1);
  }
  qChannel = ( p[0] + p[1]*qtc + p[2]*qtc*qtc + p[3]*qtc*qtc*qtc )*( 1.0 - TMath::Erf((qtc-p[4])/p[5]) ) + ( p[6] + p[7]*qtc )*( 1.0 + TMath::Erf((qtc-p[8])/p[9]) );
  qChannel = ratio*qChannel;

  //  cout << "Sim West tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<11; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qChannel;
}


const double TofQElecSvc::EQChannel(int id, double qtc) {
  double qChannel = -999.0;
  MsgStream log(msgSvc(), name());    
  if( id<0 || id>95 ) {
    log << MSG::ERROR <<"EQTC : TofId is out of Range, tofid=" << id << endreq;
    return qChannel;
  }

  double ratio = (m_data->getETof(id)).getSimP(0);
  double p[10];
  for(unsigned int i=0; i<10; i++ ) {
    p[i] = (m_data->getETof(id)).getSimP(i+1);
  }
  qChannel = ( p[0] + p[1]*qtc + p[2]*qtc*qtc + p[3]*qtc*qtc*qtc )*( 1.0 - TMath::Erf((qtc-p[4])/p[5]) ) + ( p[6] + p[7]*qtc )*( 1.0 + TMath::Erf((qtc-p[8])/p[9]) );
  qChannel = ratio*qChannel;

  //  cout << "Endcap tofid=" << id << " ratio=" << ratio;
  //  for( unsigned int i=0; i<11; i++ ) { cout << " P" << i << "= " << p[i]; }
  //  cout << endl;

  return qChannel;
}
