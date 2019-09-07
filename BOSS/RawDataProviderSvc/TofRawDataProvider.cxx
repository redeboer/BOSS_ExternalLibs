#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"
#include "EventModel/EventHeader.h"
#include "Identifier/Identifier.h"
#include "Identifier/TofID.h"
#include "TofRawEvent/TofDigi.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "TofQCorrSvc/ITofQCorrSvc.h"
#include "TofQElecSvc/ITofQElecSvc.h"
#include "DetVerSvc/IDetVerSvc.h"
#include "RawDataProviderSvc/TofRawDataProvider.h"

#include "RawEvent/RawDataUtil.h"

#include <cmath>
// tianhl for mt
#include "GaudiKernel/ThreadGaudi.h"
// tianhl for mt

using namespace std;

IDetVerSvc*   detVerSvc;
ITofCaliSvc*  tofCaliSvc;
ITofQCorrSvc* tofQCorrSvc;
ITofQElecSvc* tofQElecSvc;

TofRawDataProvider::TofRawDataProvider() :
  RawDataProviderBase(),
  m_hasFilled(0)
{}


TofRawDataProvider::TofRawDataProvider( const char* name ) :
  RawDataProviderBase( name ),
  m_hasFilled(0)
{}


TofRawDataProvider::~TofRawDataProvider() {
  IterTofDataMap iter = m_tofDataMap.begin();
  for( ; iter != m_tofDataMap.end(); iter++ ) {
    delete (*iter).second;
  }
  m_tofDataMap.clear();

  m_tofDataMapOnline.clear();
  m_tofDataVectorOnline.clear();
  m_tofDataMapEstime.clear();
  m_tofDataVectorEstime.clear();
  m_tofDataMapTof.clear();
  m_tofDataVectorTof.clear();
  m_tofDataMapEmc.clear();
  m_tofDataVectorEmc.clear();

  return;
}


StatusCode TofRawDataProvider::initialize( bool mode, ISvcLocator* pSvcLoc, IMessageSvc* pMsg ) {
  MsgStream log( m_msgSvc, m_name );
  RawDataProviderBase::initialize( pSvcLoc,pMsg );

  // Get Detector Version Service
  StatusCode sc_det = m_svcLocator->service("DetVerSvc", detVerSvc);
  if( sc_det == StatusCode::SUCCESS ) {
    log << MSG::INFO << "TofRawDataProvider Get Detector Version Service Sucessfully!" << endreq;
  }
  else {
    log << MSG::ERROR << "TofRawDataProvider Can NOT Retrieve DetVerSvc instance" << endreq;
    return StatusCode::FAILURE;
  }

  //Get TOF Calibtration Service
  StatusCode scc = m_svcLocator->service("TofCaliSvc", tofCaliSvc);
  if( scc ==  StatusCode::SUCCESS ) { 
    log << MSG::INFO << "TofRec Get Calibration Service Sucessfully!" << endreq;
  }
  else if(!mode) {
    log << MSG::ERROR << "TofRec Get Calibration Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  //Get TOF Q Correction Service
  StatusCode scq = m_svcLocator->service("TofQCorrSvc", tofQCorrSvc);
  if (scq ==  StatusCode::SUCCESS) { 
    log << MSG::INFO << "TofRec Get Q Correction Service Sucessfully!" << endreq;
  } else if(!mode){
    log << MSG::ERROR << "TofRec Get Q Correction Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  //Get TOF Q Electronics Saturation Service
  StatusCode sce = m_svcLocator->service("TofQElecSvc", tofQElecSvc);
  if (sce ==  StatusCode::SUCCESS) { 
    log << MSG::INFO << "TofRec Get Q Electronics Calibration Service Sucessfully!" << endreq;
  } else if(!mode){
    log << MSG::ERROR << "TofRec Get Q Electronics Calibration Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}


void TofRawDataProvider::handle(const Incident& inc){
  MsgStream log( m_msgSvc, m_name );
  log << MSG::DEBUG << "handle: " << inc.type() << endreq;
  if ( inc.type() == "BeginEvent" ){
    log << MSG::DEBUG << "Begin Event" << endreq;
    IterTofDataMap iter = m_tofDataMap.begin();
    for( ; iter != m_tofDataMap.end(); iter++ ) {
      delete (*iter).second;
    }
    IterTofDataMap iterOnline = m_tofDataMapOnline.begin();
    for( ; iterOnline != m_tofDataMapOnline.end(); iterOnline++ ) {
      delete (*iterOnline).second;
    }
    m_tofDataMap.clear();
    m_tofDataMapOnline.clear();
    m_tofDataVectorOnline.clear();
    m_tofDataMapEstime.clear();
    m_tofDataVectorEstime.clear();
    m_tofDataMapTof.clear();
    m_tofDataVectorTof.clear();
    m_tofDataMapEmc.clear();
    m_tofDataVectorEmc.clear();

    m_hasFilled = false;
  }
  return;
}


// tofDataMapFull includes all possible combinations.
void TofRawDataProvider::tofDataMapFull() {

  MsgStream log(m_msgSvc, m_name);
  log << MSG::INFO << "TofRawDataProvider::tofDataMapFull()" << endreq;

  if( !m_tofDataMap.empty() ) {

    log << MSG::WARNING << "TofRawDataProvider::tofDataMapFull: m_tofDataMap is NOT empty!" << endreq;

    IterTofDataMap iter = m_tofDataMap.begin();
    for( ; iter != m_tofDataMap.end(); iter++ ) {
      delete (*iter).second;
    }
    m_tofDataMap.clear();
  }
   // tianhl for mt
    std::string evtDataSvc_name("EventDataSvc");
    if(isGaudiThreaded(m_name)){
      evtDataSvc_name += getGaudiThreadIDfromName(m_name);
    }
    // tianhl for mt

  IDataProviderSvc* eventSvc;
  StatusCode sc = m_svcLocator->service( evtDataSvc_name.c_str(),eventSvc, true );
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "TofRawDataProvider::tofDataMapFull: ERROR Could not load EventDataSvc" << endreq;
    return;
  }

  // Event Header
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
  if( !eventHeader ) {
    log << MSG::FATAL << "TofRawDataProvider could not find Event Header!" << endreq;
    return;
  }
  m_run = eventHeader->runNumber();
  m_event = eventHeader->eventNumber();

  bool mc = ( m_run < 0 ) ? true:false;
  bool qCorr = bool( tofCaliSvc->QCorr() );
  qCorr = qCorr && (!mc);
  bool qElec = bool( tofCaliSvc->QElec() );
  bool misLable = bool( tofCaliSvc->MisLable() );
  misLable = ( !mc && misLable );

  int identmp = -1;
  vector<int> deadId;
  for( unsigned int i=0; i<5; i++ ) {
    identmp = tofCaliSvc->BrEast(i);
    if( identmp != 0x2fffffff ) {
      deadId.push_back( identmp );
    }
    identmp = tofCaliSvc->BrWest(i);
    if( identmp != 0x2fffffff ) {
      deadId.push_back( identmp );
    }
    identmp = tofCaliSvc->Endcap(i);
    if( identmp != 0x2fffffff ) {
      deadId.push_back( identmp );
    }
  }

  // get TDS data in a common class
  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc,"/Event/Digi/TofDigiCol");
  if( !tofDigiCol ) {
    log << MSG::WARNING << "TofRawDataProvider::tofDataMapFull: Could not find Tof Digi Data!" << endreq;
    return;
  }

  int qnumber = 0;
  int tnumber = 0;

  std::vector<Adc*> chargeVec;
  std::vector<Tdc*> timeVec;

  TofDigiCol::iterator iter = tofDigiCol->begin();
  for( ; iter != tofDigiCol->end(); iter++ ) {
    Identifier iden = (*iter)->identify();
    unsigned int iden_value = iden.get_value();

    bool sameId = false;
    vector<int>::iterator iter_dead = deadId.begin();
    for( ; iter_dead != deadId.end(); iter_dead++ ) {
      if( iden_value == (*iter_dead) ) {
	sameId = true;
	break;
      }
    }
    if( sameId ) {
      if( mc ) { continue; }
      else {
	std::string bore;
	if( (TofID::barrel_ec(iden))==1 ) { bore=string("Barrel");}
	else if( (TofID::barrel_ec(iden))==0 ) { bore=string("East Endcap"); }
	else if( (TofID::barrel_ec(iden))==2 ) { bore=string("West Endcap"); }
	else if( (TofID::barrel_ec(iden))==3 ) { bore=string("ETF(MRPC)");   }

	log << MSG::ERROR << "TofRawDataProvider::tofDataMapFull: Dead Channel Number is not Correct, please check TOF Calibration Constants!  " << bore << endreq;
      }
    }

    if( TofID::is_scin(iden) && !(TofID::is_barrel(iden) ) && (TofID::end(iden))==1 ) continue;

    unsigned int overflow = (*iter)->getOverflow();
    if( TofID::is_scin(iden) && ( ( overflow & 0xfe000000 ) == 0xfe000000 ) ) continue;


    //    if( ( m_detVerSvc->phase() == 1 ) && TofID::is_mrpc( iden ) ) continue;
    //    if( ( m_detVerSvc->phase() >= 3 ) && TofID::is_scin( iden ) && !( TofID::is_barrel( iden ) ) ) continue;;

    // for 150203 data, 92Scin + 2MRPC
    //    if( !mc && ( m_detVerSvc->phase() == 2 ) && TofID::is_mrpc( iden ) ) {
    if( m_run>=40203 && m_run<=43253 && TofID::is_mrpc( iden ) ) {
      // ETF Module 36 ==> 44
      if( ( iden_value & 0x2fffffe0 ) == 0x2000c800 ) {
	iden_value = 0x2000c900 + ( iden_value & 0x1f );
	iden = Identifier( iden_value );
      }
      // ETF Module 37 ==> 43
      if( ( iden_value & 0x2fffffe0 ) == 0x2000c820 ) {
	iden_value = 0x2000c8e0 + ( iden_value & 0x1f );
	iden = Identifier( iden_value );
      }
    }
    // end 150203 data

    // overflow 0x   1 1         1 1            1 1
    //            no Q T / multi Q T / overflow Q T

    bool multiQ = ( ( overflow & 0x8 ) != 0 );
    bool multiT = ( ( overflow & 0x4 ) != 0 );

    unsigned int tdcChannel = (*iter)->getTimeChannel();
    unsigned int adcChannel = (*iter)->getChargeChannel();
    if( TofID::is_scin(iden) && ( ( overflow & 0x2 ) != 0 ) ) { 
      adcChannel = ( adcChannel | 0x80000 ); 
    }

    if( (adcChannel&0x7fffffff) != 0x7fffffff ) {
      Adc* adc = new Adc;
      if( qCorr ) adc->setCorr();
      if( qElec ) adc->setElec();

      // for 150203 data, 92Scin + 2MRPC
      //      if( !mc && ( m_detVerSvc->phase() == 2 ) && TofID::is_mrpc( iden ) ) {
      if( m_run>=40203 && m_run<=43253 && TofID::is_mrpc( iden ) ) {
	adcChannel += 0x18000;
      }
      // end 150203 data

      adc->setValue( iden, adcChannel );
      qnumber++;
      adc->setNumber( qnumber );
      chargeVec.push_back( adc );
    }

    if( tdcChannel != 0x7fffffff ) {
      Tdc* tdc = new Tdc;

      // for 150203 data, 92Scin + 2MRPC
      //      if( !mc && ( m_detVerSvc->phase(0 == 2 ) && TofID::is_mrpc( iden ) ) {
      if( m_run>=40203 && m_run<=43253 && TofID::is_mrpc( iden ) ) {
	tdcChannel += 0x18000;
      }
      // end 150203 data

      tdc->setValue( tdcChannel );
      tnumber++;
      tdc->setNumber( tnumber );
      timeVec.push_back( tdc );
    }

    if( multiT || multiQ ) {
      TofDigiCol::iterator iter_next = iter + 1;
      if( iter_next != tofDigiCol->end() ) {
	unsigned int idenNext_value = ((*iter_next)->identify()).get_value();
	if( iden_value == idenNext_value ) continue;
      }
    }

    qnumber = 0;
    tnumber = 0;

    std::vector<PmtData*> pmtDataVec;
    if( TofID::is_scin( iden ) ) {
      std::vector<Tdc*>::iterator iter_t = timeVec.begin();
      for( ; iter_t != timeVec.end(); iter_t++ ) {
	int tclock = (*iter_t)->clock();
	PmtData* pmt = new PmtData;
	pmt->setIdentify( iden.get_value() );
	pmt->setTdc( (*iter_t) );
	std::vector<Adc*>::iterator iter_q = chargeVec.begin();
	for( ; iter_q != chargeVec.end(); iter_q++ ) {
	  if( (*iter_q)->times() != -1 ) { continue; }
	  int qclock = (*iter_q)->clock();
	  if( abs(tclock-qclock)<2 ) {
	    if( ( pmt->quality() & 0x2 ) != 0 ) {         // pmt has Q
	      if( (*iter_q)->value() > pmt->adc() ) {
		pmt->qtimesmm();
		pmt->setAdc( (*iter_q) );	      
	      }
	    }
	    else {                                        // pmt has no Q
	      pmt->setAdc( (*iter_q) );
	    }
	  }
	}
	pmtDataVec.push_back( pmt );
      }

      std::vector<Adc*>::iterator iter_q = chargeVec.begin();
      for( ; iter_q != chargeVec.end(); iter_q++ ) {
	if( (*iter_q)->times() != -1 ) { continue; }
	PmtData* pmt = new PmtData;
	pmt->setIdentify( iden.get_value() );
	pmt->setAdc( (*iter_q) );
	pmtDataVec.push_back( pmt );
      }

    }

    if( TofID::is_mrpc( iden ) ) {
      std::vector<Tdc*>::iterator iter_t = timeVec.begin();
      for( ; iter_t != timeVec.end(); iter_t++ ) {
	std::vector<Adc*>::iterator iter_q = chargeVec.begin();
	for( ; iter_q != chargeVec.end(); iter_q++ ) {
	  if( ( (*iter_q)->qtc() - (*iter_t)->value() ) > totForward && ( (*iter_q)->qtc() - (*iter_t)->value() ) < totBackward ) {
	    PmtData* pmt = new PmtData;
	    pmt->setIdentify( iden.get_value() );
	    pmt->setTdc( (*iter_t) );
	    pmt->setAdc( (*iter_q) );
	    pmtDataVec.push_back( pmt );
	  }
	}
	if( (*iter_t)->times() == -1 ) {
	  PmtData* pmt = new PmtData;
	  pmt->setTdc( (*iter_t) );
	  pmtDataVec.push_back( pmt );
	}
      }

      std::vector<Adc*>::iterator iter_q = chargeVec.begin();
      for( ; iter_q != chargeVec.end(); iter_q++ ) {
	if( (*iter_q)->times() != -1 ) { continue; }
	PmtData* pmt = new PmtData;
	pmt->setIdentify( iden.get_value() );
	pmt->setAdc( (*iter_q) );
	pmtDataVec.push_back( pmt );
      }

    }

    chargeVec.clear();
    timeVec.clear();

    unsigned int iden_value_key = (iden_value & 0xfffffffe );
    if( TofID::is_scin( iden ) ) {
      int barrel_ec = TofID::barrel_ec( iden );
      //      int layer     = TofID::layer( iden );
      int tofid     = TofID::phi_module( iden );
      int end       = TofID::end( iden );
      if( ( barrel_ec == 0 ) || ( barrel_ec == 2 ) ) {
	std::vector<PmtData*>::iterator iter_pmt = pmtDataVec.begin();
	for( ; iter_pmt != pmtDataVec.end(); iter_pmt++ ) {
	  TofData* tof = new TofData;
	  if( misLable ) {
	    tof->setMisLable();
	    if( barrel_ec==2 && tofid==42 ) {
	      unsigned int iden91 = TofID::getIntID( 2, 0, 91-48, 0 ); 
	      iden_value_key = iden91;
	      iden = TofID::cell_id( iden91 );
	    }
	    if( barrel_ec==2 && tofid==43 ) {
	      unsigned int iden90 = TofID::getIntID( 2, 0, 90-48, 0 );
	      iden_value_key = iden90;
	      iden = TofID::cell_id( iden90 );
	    }
	  }
	  tof->setIdentify( iden );
	  tof->setForward( (*iter_pmt) );
	  tof->setTMatched( true );
	  m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
	}
      }
      else if( barrel_ec == 1 ) {
	unsigned int count = m_tofDataMap.count( iden_value_key );
	if( count == 0 ) {
	  std::vector<PmtData*>::iterator iter_pmt = pmtDataVec.begin();
	  for( ; iter_pmt != pmtDataVec.end(); iter_pmt++ ) {
	    TofData* tof = new TofData;
	    tof->setIdentify( iden );
	    if( end == 0 ) {
	      tof->setForward( (*iter_pmt) );
	    }
	    else {
	      tof->setBackward( (*iter_pmt) );
	    }
	    tof->setTMatched( true );
	    m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
	  }
	}
	else {
	  pair< IterTofDataMap, IterTofDataMap > range = m_tofDataMap.equal_range( iden_value_key );
	  std::vector<PmtData*>::iterator iter_pmt = pmtDataVec.begin();
	  for( ; iter_pmt != pmtDataVec.end(); iter_pmt++ ) {
	    bool used = false;
	    IterTofDataMap iter = range.first;
	    for( unsigned int tofDataNumber = 0; tofDataNumber < count; tofDataNumber++, iter++ ) {

	      bool matched = false;
	      if( end == 0 ) { // east, forward
		if( ( (*iter_pmt)->quality() & 0x1 ) != 0 ) { // pmt has east time
		  if( ( ((*iter).second)->quality() & 0x1 ) != 0 ) { // tofmap has west time
		    double time1 = (*iter_pmt)->tdc();
		    double time2 = ((*iter).second)->tdc2();
		    matched = ( abs(time1-time2)<timeDiff );
		  }
		  else { // tofmap has no west time
		    if( ( ((*iter).second)->quality() & 0x2 ) != 0 ) { // tofmap has no west time, but west charge
		      double time1 = (*iter_pmt)->tclock();
		      double time2 = (*iter_pmt)->qclock();
		      double time3 = ((*iter).second)->qclock2();
		      matched = ( ( abs(time1-time3)<=tClockDiff ) || ( abs(time2-time3)<=tClockDiff ) );
		    }
		  }
		}
		else { // pmt has no east time
		  if( ( (*iter_pmt)->quality() & 0x2 ) != 0 ) { // pmt has no east time, but east charge
		    if( ( ((*iter).second)->quality() & 0x1 ) != 0 ) { // tofmap has west time
		      if( ( ( ((*iter).second)->quality() & 0x2 ) != 0 ) ) { // tofmap has west time and west charge
			double time1 = (*iter_pmt)->qclock();
			double time2 = ((*iter).second)->tclock2();
			double time3 = ((*iter).second)->qclock2();
			matched = ( ( abs(time1-time2)<=tClockDiff ) || ( abs(time1-time3)<=tClockDiff ) );
		      }
		    }
		    else { // tofmap has no west time, but west charge
		      if( ( ( ((*iter).second)->quality() & 0x2 ) != 0 ) ) {
			double time1 = (*iter_pmt)->qclock();
			double time2 = ((*iter).second)->qclock2();
			matched = ( abs(time1-time2)<=tClockDiff );			
		      }
		    } 
		  }
		}
	      
		if( matched ) {
		  used = true;
		  if( ( ( (*iter).second)->quality() & 0xc ) == 0 ) {
		    ((*iter).second)->setForward( (*iter_pmt) );
		  }
		  else {
		    TofData* tof = new TofData;
		    tof->setIdentify( iden );
		    tof->setForward( (*iter_pmt) );
		    tof->setBackward( ((*iter).second)->backward() );
		    tof->setTMatched( true );
		    m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
		  }
		}
	      
	      } // end east, forward
	      else { // west, backward
		if( ( (*iter_pmt)->quality() & 0x1 ) != 0 ) { // pmt has west time
		  if( ( ((*iter).second)->quality() & 0x4 ) != 0 ) { // tofmap has east time
		    double time1 = (*iter_pmt)->tdc();
		    double time2 = ((*iter).second)->tdc1();
		    matched = ( abs(time1-time2)<timeDiff );
		  }
		  else { // tofmap has no east time
		    if( ( ((*iter).second)->quality() & 0x8 ) != 0 ) { // tofmap has no east time, but east charge
		      double time1 = (*iter_pmt)->tclock();
		      double time2 = (*iter_pmt)->qclock();
		      double time3 = ((*iter).second)->qclock1();
		      matched = ( ( abs(time1-time3)<=tClockDiff ) || ( abs(time2-time3)<=tClockDiff ) );
		    }
		  }
		}
		else { // pmt has no west time
		  if( ( (*iter_pmt)->quality() & 0x2 ) != 0 ) { // pmt has no west time, but west charge
		    if( ( ((*iter).second)->quality() & 0x4 ) != 0 ) { // tofmap has east time
		      if( ( ( ((*iter).second)->quality() & 0x8 ) != 0 ) ) { // tofmap has east time and east charge
			double time1 = (*iter_pmt)->qclock();
			double time2 = ((*iter).second)->tclock1();
			double time3 = ((*iter).second)->qclock1();
			matched = ( ( abs(time1-time2)<=tClockDiff ) || ( abs(time1-time3)<=tClockDiff ) );
		      }
		      else {
		      }
		    }
		    else { // tofmap has no west time, but west charge
		      if( ( ( ((*iter).second)->quality() & 0x8 ) != 0 ) ) {
			double time1 = (*iter_pmt)->qclock();
			double time2 = ((*iter).second)->qclock1();
			matched = ( abs(time1-time2)<=tClockDiff );			
		      }
		    } 
		  }
		}

		if( matched ) {
		  used = true;
		  if( ( ( (*iter).second)->quality() & 0x3 ) == 0 ) {
		    ((*iter).second)->setBackward( (*iter_pmt) );
		  }
		  else {
		    TofData* tof = new TofData;
		    tof->setIdentify( iden );
		    tof->setForward( ((*iter).second)->forward() );
		    tof->setBackward( (*iter_pmt) );
		    tof->setTMatched( true );
		    m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
		  }
		}
	      }
	    }

	    if( ! used ) {
	      TofData* tof = new TofData;
	      tof->setIdentify( iden );
	      if( end == 0 ) {
		tof->setForward( (*iter_pmt) );
	      }
	      else {
		tof->setBackward( (*iter_pmt) );
	      } 
	      tof->setTMatched( true );
	      m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
	    }
	  }
	}
      }
    }
    //  mrpc endcap tof
    if( TofID::is_mrpc( iden ) ) {
      //      int barrel_ec = TofID::barrel_ec( iden );
      int endcap    = TofID::endcap( iden );
      int tofid     = TofID::module( iden );
      int strip     = TofID::strip( iden );
      int end       = TofID::end( iden );

      unsigned int count = m_tofDataMap.count( iden_value_key );
      if( count == 0 ) {
	std::vector<PmtData*>::iterator iter_pmt = pmtDataVec.begin();
	for( ; iter_pmt != pmtDataVec.end(); iter_pmt++ ) {
	  TofData* tof = new TofData;
	  tof->setIdentify( iden );
	  if( end == 0 ) {
	    tof->setForward( (*iter_pmt) );
	  }
	  else {
	    tof->setBackward( (*iter_pmt) );
	  }
	  tof->setTMatched( true );
	  m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
	}
      }
      else {
	pair< IterTofDataMap, IterTofDataMap > range = m_tofDataMap.equal_range( iden_value_key );
	std::vector<PmtData*>::iterator iter_pmt = pmtDataVec.begin();
	for( ; iter_pmt != pmtDataVec.end(); iter_pmt++ ) {
	  bool used = false;
	  IterTofDataMap iter = range.first;
	  for( unsigned int tofDataNumber = 0; tofDataNumber < count; tofDataNumber++, iter++ ) {
	    bool matched = false;
	    if( end == 0 ) { // east, forward
	      if( ( (*iter_pmt)->quality() & 0x1 ) != 0 ) {  // pmt has east time
		if( ( ((*iter).second)->quality() & 0x1 ) != 0 ) { // tofmap has west time
		  double time1 = (*iter_pmt)->tdc();
		  double time2 = ((*iter).second)->tdc2();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		  if( endcap==0 && tofid==13 && strip==5 ) { matched = ( abs(time1-time2)<(100+timeDiffMRPC) ); }
		}
	      }
		/*
		else {
		  if( ( ((*iter).second)->quality() & 0x2 ) != 0 ) {
		    double time1 = (*iter_pmt)->tdc();
		    double time2 = ((*iter).second)->qtc2();
		    matched = ( abs(time1-time2)<timeDiffMRPC );
		  }
		}
	      }
	      else {
		if( ( ((*iter).second)->quality() & 0x1 ) != 0 ) {
		  double time1 = (*iter_pmt)->qtc();
		  double time2 = ((*iter).second)->tdc2();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		}
		else {
		  if( ( ((*iter).second)->quality() & 0x2 ) != 0 ) {
		    double time1 = (*iter_pmt)->qtc();
		    double time2 = ((*iter).second)->qtc2();
		    matched = ( abs(time1-time2)<timeDiffMRPC );
		  }
		}
	      }

	      if( ( (*iter_pmt)->quality() & 0x3 ) == 0x3 ) { // pmt has east time
		if( ( ((*iter).second)->quality() & 0x3 ) == 0x3 ) { // tofmap has west time
		  double time1 = (*iter_pmt)->tdc();
		  double time2 = ((*iter).second)->tdc2();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		}
              }
	      */
	      if( matched ) {
		used = true;
		if( ( ( (*iter).second)->quality() & 0xc ) == 0 ) {
		  ((*iter).second)->setForward( (*iter_pmt) );
		}
		else {
		  TofData* tof = new TofData;
		  tof->setIdentify( iden );
		  tof->setForward( (*iter_pmt) );
		  tof->setBackward( ((*iter).second)->backward() );	  
		  tof->setTMatched( true );
		  m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
		}
	      }
	    } // end east, forward
	    else { // west, backward
	      if( ( (*iter_pmt)->quality() & 0x1 ) != 0 ) { // pmt has west time
		if( ( ((*iter).second)->quality() & 0x4 ) != 0 ) { // tofmap has east time
		  double time1 = (*iter_pmt)->tdc();
		  double time2 = ((*iter).second)->tdc1();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		  if( endcap==0 && tofid==13 && strip==5 ) { matched = ( abs(time1-time2)<(100+timeDiffMRPC) ); }
		}
	      }
		/*
		else {
		  if( ( ((*iter).second)->quality() & 0x8 ) != 0 ) {
		    double time1 = (*iter_pmt)->tdc();
		    double time2 = ((*iter).second)->qtc1();
		    matched = ( abs(time1-time2)<timeDiffMRPC );
		  }
		}
	      }
	      else {
		if( ( ((*iter).second)->quality() & 0x4 ) != 0 ) {
		  double time1 = (*iter_pmt)->qtc();
		  double time2 = ((*iter).second)->tdc1();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		}
		else {
		  if( ( ((*iter).second)->quality() & 0x8 ) != 0 ) {
		    double time1 = (*iter_pmt)->qtc();
		    double time2 = ((*iter).second)->qtc1();
		    matched = ( abs(time1-time2)<timeDiffMRPC );
		  }
		}
	      }

	      if( ( (*iter_pmt)->quality() & 0x3 ) == 0x3 ) { // pmt has west time
		if( ( ((*iter).second)->quality() & 0xc ) == 0xc ) { // tofmap has east time
		  double time1 = (*iter_pmt)->tdc();
		  double time2 = ((*iter).second)->tdc1();
		  matched = ( abs(time1-time2)<timeDiffMRPC );
		}
              }
	      */
	      if( matched ) {
		used = true;
		if( ( ( (*iter).second)->quality() & 0x3 ) == 0 ) {
		  ((*iter).second)->setBackward( (*iter_pmt) );
		}
		else {
		  TofData* tof = new TofData;
		  tof->setIdentify( iden );
		  tof->setForward( ((*iter).second)->forward() );
		  tof->setBackward( (*iter_pmt) );
		  tof->setTMatched( true );
		  m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
		}
	      }
	    }
	  }

	  if( ! used ) {
	    TofData* tof = new TofData;
	    tof->setIdentify( iden );
	    if( end == 0 ) {
	      tof->setForward( (*iter_pmt) );
	    }
	    else {
	      tof->setBackward( (*iter_pmt) );
	    } 
	    tof->setTMatched( true );
	    m_tofDataMap.insert( make_pair( iden_value_key, tof ) );
	  }
	}
      }
    }

    pmtDataVec.clear();

  }

  return;

}//close tofDataMapFull


// onlineMode only T is combined, no Q reserved
TofDataMap& TofRawDataProvider::tofDataMapOnlineMode(uint32_t control) {

  MsgStream log(m_msgSvc, m_name);
  log << MSG::INFO << "TofRawDataProvider::tofDataMapOnlineMode()" << endreq;

  if( ! m_tofDataMapOnline.empty() ) {
    if(control){
      IterTofDataMap iter = m_tofDataMapOnline.begin();
      for( ; iter != m_tofDataMapOnline.end(); iter++ ) {
	delete (*iter).second;
      }
      m_tofDataMapOnline.clear();
    }
    else return m_tofDataMapOnline;
  }
    // tianhl for mt
    std::string evtDataSvc_name("EventDataSvc");
    if(isGaudiThreaded(m_name)){
      evtDataSvc_name += getGaudiThreadIDfromName(m_name);
    }
    // tianhl for mt

  IDataProviderSvc* eventSvc;
  StatusCode sc = m_svcLocator->service( evtDataSvc_name.c_str(), eventSvc, true );
  if( !sc.isSuccess() ) {
    log << MSG::FATAL << "TofRawDataProvider::tofDataMapOnlineMode(): ERROR Could not load EventDataSvc" << endreq;
    return m_tofDataMapOnline;
  }

  // get TDS data in a common class
  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc,"/Event/Digi/TofDigiCol");
  if( !tofDigiCol ) {
    log << MSG::WARNING << "TofRawDataProvider::tofDataMapOnlineMode(): Could not find Tof Digi Data!" << endreq;
    return m_tofDataMapOnline;
  } 

  TofDigiCol::iterator iter = tofDigiCol->begin();
  for( ; iter != tofDigiCol->end(); iter++ ) {
    unsigned int overflow   = (*iter)->getOverflow();
    if( ( overflow & 0xfe000000 ) == 0xfe000000 ) continue;
    // overflow 0x   1 1         1 1            1 1
    //            no Q T / multi Q T / overflow Q T
    
    bool noT = ( ( overflow & 0x10 ) != 0 );
    //    bool multiQ = ( ( overflow & 0x8 ) != 0 );
    //    bool multiT = ( ( overflow & 0x4 ) != 0 );
    if( noT ) continue;
    
    Identifier iden = (*iter)->identify();
    unsigned int iden_value = iden.get_value();
    int  barrel_ec  = TofID::barrel_ec(iden);
    int  id         = TofID::phi_module(iden);
    int  end        = TofID::end(iden);

    unsigned int tdcChannel = (*iter)->getTimeChannel();
    if( tdcChannel == 0x7fffffff ) continue;

    Tdc* tdc = new Tdc;
    tdc->setValue( tdcChannel );
    PmtData* pmt = new PmtData;
    pmt->setTdc( tdc );

    unsigned int iden_value_key = ( iden_value & 0xfffffffe );
    if( barrel_ec == 0 || barrel_ec == 2 ) {
      TofData* tof = new TofData;
      tof->setIdentify( iden );
      tof->setForward( pmt );
      m_tofDataMapOnline.insert( make_pair( iden_value_key, tof ) );
    }
    else if( barrel_ec == 1 || barrel_ec == 3 ) {
      unsigned int count = m_tofDataMapOnline.count( iden_value_key );
	
      if( count == 0 ) {
	TofData* tof = new TofData;
	tof->setIdentify( iden );
	if( end == 0 ) {
	  tof->setForward( pmt );
	}
	else {
	  tof->setBackward( pmt );
	} 
	m_tofDataMapOnline.insert( make_pair( iden_value_key, tof ) );
      }
      else {
	bool used = false;
	pair< IterTofDataMap, IterTofDataMap > range = m_tofDataMapOnline.equal_range( iden_value_key );
	IterTofDataMap iter = range.first;
	for( unsigned int i=0; i<count; i++, iter++ ) {
	  if( ( end == 0 ) && ( ( ( (*iter).second)->quality() & 0x1 ) != 0 ) ) {
	    double time1 = pmt->tdc();
	    double time2 = ((*iter).second)->tdc2();
	    if( ( TofID::is_scin( iden ) && abs(time1-time2)<timeDiff ) || ( TofID::is_mrpc( iden ) && abs(time1-time2)<timeDiffMRPC ) ) {
	      used = true;
	      if( ( ( (*iter).second)->quality() & 0x4 ) == 0 ) {
		((*iter).second)->setForward( pmt );
	      }
	      else {
		TofData* tof = new TofData;
		tof->setIdentify( iden );
		tof->setForward( pmt );
		tof->setBackward( ((*iter).second)->backward() );
		m_tofDataMapOnline.insert( make_pair( iden_value_key, tof ) );
	      }
	    }
	  }
	  else if( ( end == 1 ) && ( ( ((*iter).second)->quality() & 0x4 ) != 0 ) ) {
	    double time1 = ((*iter).second)->tdc1();
	    double time2 = pmt->tdc();
	      
	    if( ( TofID::is_scin( iden ) && abs(time1-time2)<timeDiff ) || ( TofID::is_mrpc( iden ) && abs(time1-time2)<timeDiffMRPC ) ) {
	      used = true;
	      if( ( ( (*iter).second)->quality() & 0x1 ) == 0 ) {
		((*iter).second)->setBackward( pmt );
	      }
	      else {
		TofData* tof = new TofData;
		tof->setIdentify( iden );
		tof->setForward( ((*iter).second)->forward() );
		tof->setBackward( pmt );
		m_tofDataMapOnline.insert( make_pair( iden_value_key, tof ) );
	      }
	    }
	  }
	}
	if( ! used ) {
	  TofData* tof = new TofData;
	  tof->setIdentify( iden );
	  if( end == 0 ) {
	    tof->setForward( pmt );
	  }
	  else {
	    tof->setBackward( pmt );
	  } 
	  m_tofDataMapOnline.insert( make_pair( iden_value_key, tof ) );
	}
      }
    }
  }

  return m_tofDataMapOnline;

}


TofDataVector& TofRawDataProvider::tofDataVectorOnlineMode(uint32_t control) {

  MsgStream log(m_msgSvc, m_name);
  log << MSG::INFO << "TofRawDataProvider::tofDataVectorOnlineMode()" << endreq;
  
  if( m_tofDataVectorOnline.size() != 0 ) {
    if(!control) return m_tofDataVectorOnline;
    else         m_tofDataVectorOnline.clear();
  }
  TofDataMap tofRawDataMap = TofRawDataProvider::tofDataMapOnlineMode(control);
  IterTofDataMap iter = tofRawDataMap.begin();
  for( ; iter != tofRawDataMap.end(); iter++ ) {
    m_tofDataVectorOnline.push_back( iter->second );
  }

  return m_tofDataVectorOnline;
}

 
// Event Start Time, only Time is required.
TofDataMap& TofRawDataProvider::tofDataMapEstime() {

  if( !m_hasFilled ) {
    TofRawDataProvider::tofDataMapFull();
    m_hasFilled = true;
  }

  if( !m_tofDataMapEstime.empty() ) {
    m_tofDataMapEstime.clear();
  }

  IterTofDataMap iter = m_tofDataMap.begin();
  for( ; iter != m_tofDataMap.end(); iter++ ) {
    if( ( ( ((*iter).second)->quality() & 0x5 ) != 0 ) && ((*iter).second)->tmatched() ) {
      m_tofDataMapEstime.insert( make_pair( (*iter).first, (*iter).second ) );
    }
  }

  iter = m_tofDataMapEstime.begin();
  while( iter != m_tofDataMapEstime.end() ) {
    unsigned int iden_value = (*iter).first;
    unsigned int count = m_tofDataMapEstime.count( iden_value );
    for( unsigned int i=0; i != count; i++, iter++ ) {
      ((*iter).second)->setTimes( count );
    }
  }

  return m_tofDataMapEstime;
}


TofDataVector& TofRawDataProvider::tofDataVectorEstime() {
  if( m_tofDataVectorEstime.size() != 0 ) return m_tofDataVectorEstime;

  TofDataMap tofDataMapEstime = TofRawDataProvider::tofDataMapEstime();
  IterTofDataMap iter = tofDataMapEstime.begin();
  for( ; iter != tofDataMapEstime.end(); iter++ ) {
    m_tofDataVectorEstime.push_back( iter->second );
  }
  return m_tofDataVectorEstime;
}


// TOF reconstruction, both time and charge are required
TofDataMap& TofRawDataProvider::tofDataMapTof( double estime ) {

  if( !m_hasFilled ) {
    TofRawDataProvider::tofDataMapFull();
    m_hasFilled = true;
  }

  if( !m_tofDataMapTof.empty() ) {
    m_tofDataMapTof.clear();
  }

  IterTofDataMap iter = m_tofDataMap.begin();
  for( ; iter != m_tofDataMap.end(); iter++ ) {
    if( ( ( ((*iter).second)->quality() & 0xc ) == 0xc ) || ( ( ((*iter).second)->quality() & 0x3 ) == 0x3 ) ) {
      if( estime > 1.0e-6 ) {
	double tdc1 = ((*iter).second)->tdc1();
	double tdc2 = ((*iter).second)->tdc2();
	bool  forward = ( ( ( estime - tdc1 ) < timeBackward ) && ( ( tdc1 - estime ) < timeForward ) );
	bool backward = ( ( ( estime - tdc2 ) < timeBackward ) && ( ( tdc2 - estime ) < timeForward ) );
	if( TofID::is_mrpc( TofID::cell_id((*iter).first) ) && m_run>=43680 && m_run<43809 ) {
	  double offset = -264.0;
	  forward = ( ( ( estime - tdc1 ) < ( timeBackward - offset ) ) && ( ( tdc1 - estime ) < ( timeForward + offset ) ) );
	  backward = ( ( ( estime - tdc2 ) < ( timeBackward + offset ) ) && ( ( tdc2 - estime ) < ( timeForward + offset ) ) );
	}
	if( !forward && !backward ) continue;	
      }
      m_tofDataMapTof.insert( make_pair( (*iter).first, (*iter).second ) );
    }
  }

  iter = m_tofDataMapTof.begin();
  while( iter != m_tofDataMapTof.end() ) {
    unsigned int iden_value = (*iter).first;
    unsigned int count = m_tofDataMapTof.count( iden_value );
    for( unsigned int i=0; i != count; i++, iter++ ) {
      ((*iter).second)->setTimes( count );
    }
  }

  return m_tofDataMapTof;
}


TofDataVector& TofRawDataProvider::tofDataVectorTof( double estime ) {
  if( m_tofDataVectorTof.size() != 0 ) {
    m_tofDataVectorTof.clear();
  }

  TofDataMap tofDataMapTof = TofRawDataProvider::tofDataMapTof( estime );
  IterTofDataMap iter = tofDataMapTof.begin();
  for( ; iter != tofDataMapTof.end(); iter++ ) {
    m_tofDataVectorTof.push_back( iter->second );
  }
  return m_tofDataVectorTof;
}


// EMC reconstruction, only charge are required
TofDataMap& TofRawDataProvider::tofDataMapEmc( double estime ) {

  if( !m_hasFilled ) {
    TofRawDataProvider::tofDataMapFull();
    m_hasFilled = true;
  }

  if( !m_tofDataMapEmc.empty() ) {
    m_tofDataMapEmc.clear();
  }

  IterTofDataMap iter = m_tofDataMap.begin();
  for( ; iter != m_tofDataMap.end(); iter++ ) {
    if( ( ((*iter).second)->quality() & 0xa ) != 0 ) {

      if( estime > 1.0e-6 ) {
	int t0clock = static_cast<int>(estime/24.0);
	int clock1 = ((*iter).second)->qclock1();
	int clock2 = ((*iter).second)->qclock2();
	bool  forward = ( ( ( t0clock - clock1 ) < tClockBackward ) && ( ( clock1 - t0clock ) < tClockForward ) );
	bool backward = ( ( ( t0clock - clock2 ) < tClockBackward ) && ( ( clock2 - t0clock ) < tClockForward ) );
	if(  TofID::is_mrpc( TofID::cell_id((*iter).first) ) && m_run>=43680 && m_run<43809 ) {
	  double offset = -11.0;
	  forward = ( ( ( t0clock - clock1 ) < ( tClockBackward -offset ) ) && ( ( clock1 - t0clock ) < ( tClockForward + offset ) ) );
	  backward = ( ( ( t0clock - clock2 ) < ( tClockBackward - offset ) ) && ( ( clock2 - t0clock ) < ( tClockForward + offset ) ) );
	}
	if( !forward && !backward ) continue;
      }

      m_tofDataMapEmc.insert( make_pair( (*iter).first, (*iter).second ) );
    }
  }

  iter = m_tofDataMapEmc.begin();
  while( iter != m_tofDataMapEmc.end() ) {
    unsigned int iden_value = (*iter).first;
    unsigned int count = m_tofDataMapEmc.count( iden_value );
    for( unsigned int i=0; i != count; i++, iter++ ) {
      ((*iter).second)->setTimes( count );
    }
  }

  return m_tofDataMapEmc;
}


TofDataVector& TofRawDataProvider::tofDataVectorEmc( double estime ) {
  if( m_tofDataVectorEmc.size() != 0 ) {
    m_tofDataVectorEmc.clear();
  }

  TofDataMap tofDataMapEmc = TofRawDataProvider::tofDataMapEmc( estime );
  IterTofDataMap iter = tofDataMapEmc.begin();
  for( ; iter != tofDataMapEmc.end(); iter++ ) {
    m_tofDataVectorEmc.push_back( iter->second );
  }
  return m_tofDataVectorEmc;
}
