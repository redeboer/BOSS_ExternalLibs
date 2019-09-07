//  Package: TofRec
//  BESIII Tof Reconstruction Algorithm. 
//  Created by Sun Shengsen (EPC IHEP)
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartDataLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/EventHeader.h"
#include "EventModel/EventModel.h"
#include "ReconEvent/ReconEvent.h"
#include "McTruth/McParticle.h"
#include "McTruth/TofMcHit.h"
#include "McTruth/McParticle.h"
#include "EvTimeEvent/RecEsTime.h"
#include "ExtEvent/RecExtTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TofGeomSvc/ITofGeomSvc.h"
#include "TofCaliSvc/ITofCaliSvc.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/TofData.h"
#include "TofRec/TofRec.h"
#include "TofRec/TofTrack.h"
#include "TofRec/TofCheckDigi.h"
#include "TofRec/TofCount.h"
#include "TofRec/TofRecTDS.h"
#include <iostream>

#include "MdcRecEvent/RecMdcTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"

using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

ITofGeomSvc* tofGeomSvc;
ITofCaliSvc* tofCaliSvc;
// ITofQCorrSvc* tofQCorrSvc;
// ITofQElecSvc* tofQElecSvc;
IRawDataProviderSvc* tofDigiSvc;

TofRec::TofRec(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{   
  declareProperty( "AcceleratorStatus", m_acceleratorStatus );
  declareProperty( "MagneticField",     m_magneticField     );
  declareProperty( "ForCalibration",    m_forCalibration    );
  declareProperty( "Data",              m_data              );
  declareProperty( "CalibData",         m_calibData         );
  //  declareProperty( "CalibDataBarrel",   m_calibDataBarrel   );
  declareProperty( "FirstIteration",    m_firstIteration    );
  declareProperty( "CheckTrigger",      m_checkTrigger      );
  declareProperty( "SaveRootFile4Rec",  m_saveRootFile      );
  declareProperty( "PrintOutInfo",      m_printOutInfo      );
  declareProperty( "CheckDigi",         m_checkDigi         );
  declareProperty( "CheckDigiRaw",      m_checkDigiRaw      );
  declareProperty( "CheckDigiExt",      m_checkDigiExt      );
  declareProperty( "CheckMcTruth",      m_checkMcTruth      );
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofRec::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TofRec in initialize()" << endreq;   

  //Get TOF Geomertry Service
  StatusCode scg = service("TofGeomSvc", tofGeomSvc);
  if (scg==  StatusCode::SUCCESS) {
    log << MSG::INFO << "TofRec Get Geometry Service Sucessfully!" << endreq;
  }else {
    log << MSG::ERROR << "TofRec Get Geometry Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  //Get TOF Calibtration Service
  StatusCode scc = service("TofCaliSvc", tofCaliSvc);
  if (scc ==  StatusCode::SUCCESS) { 
    log << MSG::INFO << "TofRec Get Calibration Service Sucessfully!" << endreq;
  } else {
    log << MSG::ERROR << "TofRec Get Calibration Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  //Get TOF Raw Data Provider Service
  StatusCode scd = service("RawDataProviderSvc", tofDigiSvc);
  if (scd ==  StatusCode::SUCCESS) { 
    log << MSG::INFO << "TofRec Get Tof Raw Data Service Sucessfully!" << endreq;
  } else {
    log << MSG::ERROR << "TofRec Get Tof Raw Data Service Failed !" << endreq;
    return StatusCode::FAILURE;
  }

  if( m_checkDigi ) {
    NTuplePtr nt11(ntupleSvc(),"FILE203/digi");
    NTuplePtr nt12(ntupleSvc(),"FILE203/barrel");
    NTuplePtr nt13(ntupleSvc(),"FILE203/endcap");
    NTuplePtr nt14(ntupleSvc(),"FILE203/mrpc");
    NTuplePtr nt15(ntupleSvc(),"FILE203/ext");
    NTuplePtr nt16(ntupleSvc(),"FILE203/tof");
    NTuplePtr nt17(ntupleSvc(),"FILE203/bb");

    if( nt11 || nt12 || nt13 || nt14 || nt15 || nt16 || nt17) {
      m_tuple_digi   = nt11;
      m_tuple_barrel = nt12;
      m_tuple_endcap = nt13;
      m_tuple_endcap = nt14;
      m_tuple_ext    = nt15;
      m_tuple_tof    = nt16;
      m_tuple_bb     = nt17;
    }
    else {
      m_tuple_digi   = ntupleSvc()->book("FILE203/digi",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_barrel = ntupleSvc()->book("FILE203/barrel",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_endcap = ntupleSvc()->book("FILE203/endcap",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_mrpc   = ntupleSvc()->book("FILE203/mrpc",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_ext    = ntupleSvc()->book("FILE203/ext",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_tof    = ntupleSvc()->book("FILE203/tof",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_bb     = ntupleSvc()->book("FILE203/bb",CLID_ColumnWiseTuple,"TofRec");

      if( m_tuple_digi || m_tuple_barrel || m_tuple_endcap || m_tuple_mrpc || m_tuple_ext || m_tuple_tof || m_tuple_bb ) {
	m_checkdigi_tuple = new TofCheckDigi( m_tuple_digi, m_tuple_barrel, m_tuple_endcap, m_tuple_mrpc, m_tuple_ext, m_tuple_tof, m_tuple_bb );
      }
      else {
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_digi) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_barrel) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_endcap) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_mrpc) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_ext) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_tof) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_bb) <<endmsg;
	return StatusCode::FAILURE;
      }
    }
  }

  if( m_saveRootFile ) {
    NTuplePtr nt21(ntupleSvc(),"FILE201/trk");
    NTuplePtr nt22(ntupleSvc(),"FILE201/cbtrk");
    NTuplePtr nt23(ntupleSvc(),"FILE201/cetrk");
    NTuplePtr nt24(ntupleSvc(),"FILE201/cetftrk");

    if( nt21 || nt22 || nt23 || nt24 ) {
      m_tuple_trk     = nt21;
      m_tuple_cbtrk   = nt22;
      m_tuple_cetrk   = nt23;
      m_tuple_cetftrk = nt24;
    }
    else {
      m_tuple_trk   = ntupleSvc()->book("FILE201/trk",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_cbtrk = ntupleSvc()->book("FILE201/cbtrk",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_cetrk = ntupleSvc()->book("FILE201/cetrk",CLID_ColumnWiseTuple,"TofRec");
      m_tuple_cetftrk = ntupleSvc()->book("FILE201/cetftrk",CLID_ColumnWiseTuple,"TofRec");
      if( m_tuple_trk || m_tuple_cbtrk || m_tuple_cetrk || m_tuple_cetftrk) {
	m_checkdata_tuple = new TofCheckData( m_tuple_trk, m_tuple_cbtrk, m_tuple_cetrk, m_tuple_cetftrk );
      }
      else {
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_trk) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_cbtrk) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_cetrk) <<endmsg;
	log << MSG::ERROR <<"Cannot book N-tuple:" << long(m_tuple_cetftrk) <<endmsg;
	return StatusCode::FAILURE;
      }
    }
  }

  if( m_printOutInfo ) { m_printOut = new TofCount; }

  return StatusCode::SUCCESS;
}

StatusCode TofRec::beginRun(){
  MsgStream log(msgSvc(), name());
  log << MSG::INFO <<"TofRec begin_run!"<< endreq;
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofRec::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TofRec in execute()!" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if( !eventHeader ) {
    log << MSG::FATAL << "TofRec could not find Event Header!" << endreq;
    return StatusCode::FAILURE;
  }
  int run = eventHeader->runNumber();
  int event = eventHeader->eventNumber();
  if( ( event % 1000 == 0 ) && m_printOutInfo ) {
    std::cout << "run:" << run << "  event: " << event << std::endl;
  }
  log << MSG::INFO << "run= " << run << "  event= " << event << endreq;

  TofRecTDS tds;
  tds.RegisterNullRecTofTrackCol();

  // Magnetic Field: Colliding data and Cosmic Ray
  if( m_acceleratorStatus == "Colliding" ) {
    // Retrieve Event Start Time
    SmartDataPtr<RecEsTimeCol> estimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
    if( !estimeCol || ( estimeCol->size() == 0 ) ) { 
      log << MSG::WARNING << "TofRec Could not find RecEsTimeCol!   Run = " << run << " Event = " << event << endreq;
      return StatusCode::SUCCESS;
    } 
    RecEsTimeCol::iterator iter_ESTime=estimeCol->begin();
    double t0  = (*iter_ESTime)->getTest();
    int t0Stat = (*iter_ESTime)->getStat();
    log << MSG::INFO << "t0= " << t0 << "  t0Stat= " << t0Stat << endreq;

    // Kalman Filter Track
    SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
    if( !mdcKalTrackCol ) {
      log << MSG::WARNING << "No MdcKalTrackCol in TDS!   Run = " << run << "  Event = " << event << endreq;
      return StatusCode::SUCCESS;
    }
    SmartDataPtr<RecMdcTrackCol> mdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
    if( !mdcTrackCol ) {
      log << MSG::FATAL << "Could NOT find RecMdcTrackCol in TDS!   Run = " << run << "  Event = " << event << endreq;
      return StatusCode::SUCCESS;
    }

    // Tof Get Extrapolated Track
    SmartDataPtr<RecExtTrackCol> extTrackCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
    if( !extTrackCol ) {
      log << MSG::WARNING << "No ExtTrackCol in TDS!   Run = " << run << "  Event = " << event << endreq;
      return StatusCode::SUCCESS;
    }
    else {
      if( m_printOutInfo ) { m_printOut->setExtTrackNum( extTrackCol->size() ); }
      if( m_checkDigi && m_checkDigiExt ) { m_checkdigi_tuple->FillCol( *eventHeader, mdcTrackCol, mdcKalTrackCol, extTrackCol ); }
    }

    //sunss add 08/9/17
    if( m_forCalibration ) {            // Bhabha Events Selection
      if( m_printOutInfo ) { m_printOut->addNumber(0); }
      //      if( t0Stat != 111 && t0Stat != 121 ) return StatusCode::SUCCESS;
      if( t0Stat%10 != 1 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(1); }

      if( extTrackCol->size() != 2 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(2); }

      if( mdcTrackCol->size() != 2 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(3); }

      SmartDataPtr<RecEmcShowerCol> emcShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
      if( !emcShowerCol ) { 
	log << MSG::FATAL << "Could NOT find EmcRecShowerCol in TDS!   Run = " << run << "  Event = " << event << endreq;
	return StatusCode::SUCCESS;
      }
      if( m_printOutInfo ) { m_printOut->addNumber(4); }

      if( emcShowerCol->size() < 2 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(5); }

      RecMdcTrackCol::iterator iter_mdc1 = mdcTrackCol->begin();
      RecMdcTrackCol::iterator iter_mdc2 = mdcTrackCol->begin() + 1;

      RecMdcKalTrackCol::iterator iter_kal1 = mdcKalTrackCol->begin();
      RecMdcKalTrackCol::iterator iter_kal2 = mdcKalTrackCol->begin() + 1;

      RecExtTrackCol::iterator iter_ext1 = extTrackCol->begin();
      RecExtTrackCol::iterator iter_ext2 = extTrackCol->begin() + 1;
      Hep3Vector extPos1 = (*iter_ext1)->emcPosition();
      Hep3Vector extPos2 = (*iter_ext2)->emcPosition();

      RecEmcShowerCol::iterator iter_emc1 = emcShowerCol->begin();
      RecEmcShowerCol::iterator iter_emc2 = emcShowerCol->begin() + 1;
      Hep3Vector emcPos1((*iter_emc1)->x(),(*iter_emc1)->y(),(*iter_emc1)->z());
      Hep3Vector emcPos2((*iter_emc2)->x(),(*iter_emc2)->y(),(*iter_emc2)->z());

      Hep3Vector pep = (*iter_mdc1)->p3();
      Hep3Vector pem = (*iter_mdc2)->p3();
      double delta_angle = 180.0 - pep.angle( pem.unit() )*180.0/pi;
      double delta_phi = abs( (*iter_mdc1)->phi() - (*iter_mdc2)->phi() )*180.0/pi;

      Hep3Vector distant1 = extPos1 - emcPos1;
      Hep3Vector distant2 = extPos2 - emcPos1;
      if( distant1.r() > distant2.r() ) {
	RecEmcShowerCol::iterator iter_tmp = iter_emc1;
	iter_emc1 = iter_emc2;
	iter_emc2 = iter_tmp;
	Hep3Vector emc_tmp = emcPos1;
	emcPos1 = emcPos2;
	emcPos2 = emc_tmp;
      }
      distant1 = extPos1 - emcPos1;
      distant2 = extPos2 - emcPos2;

      double p1 = (*iter_mdc1)->p();
      double p2 = (*iter_mdc2)->p();
      double e1 = (*iter_emc1)->energy();
      double e2 = (*iter_emc2)->energy();
      double etot = 0.0;
      RecEmcShowerCol::iterator iter_emc = emcShowerCol->begin();
      for( ; iter_emc != emcShowerCol->end(); iter_emc++ ) {
	etot += (*iter_emc)->energy();
      }

      if( m_checkDigi ) { m_checkdigi_tuple->FillCol( *eventHeader, extTrackCol, mdcTrackCol, emcShowerCol, mdcKalTrackCol ); }

      if( ( (*iter_mdc1)->charge() + (*iter_mdc2)->charge() )!= 0 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(6); }

      if( delta_angle > 10.0 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(7); }

      if( (*iter_kal1)->getStat(1,0)!=0 || (*iter_kal2)->getStat(1,0)!=0 ) return StatusCode::SUCCESS; 
      if( m_printOutInfo ) { m_printOut->addNumber(8); }

      if( distant1.r()>6.0 || distant2.r()>6.0 ) return StatusCode::SUCCESS;
      if( m_printOutInfo ) { m_printOut->addNumber(9); }

      // Jpsi data taken in 2009
      if( m_data == "jpsi09" ) {
	if( (*iter_mdc1)->x()<-0.2 || (*iter_mdc1)->x()>0.4 || (*iter_mdc1)->y()<-0.5 || (*iter_mdc1)->y()>0.1 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.2 || (*iter_mdc2)->x()>0.4 || (*iter_mdc2)->y()<-0.5 || (*iter_mdc2)->y()>0.1 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<174.0 || delta_phi>186.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.1 || e2 < 1.1 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Psi prime data taken in 2009
      else if( m_data == "psip09" ) {
	if( (*iter_mdc1)->x()<-0.2 || (*iter_mdc1)->x()>0.4 || (*iter_mdc1)->y()<-0.5 || (*iter_mdc1)->y()>0.1 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.2 || (*iter_mdc2)->x()>0.4 || (*iter_mdc2)->y()<-0.5 || (*iter_mdc2)->y()>0.1 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<174.0 || delta_phi>183.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.4 || e2 < 1.4 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Psi double prime data taken in first half of 2010
      else if( m_data == "psipp10" ) {
	if( (*iter_mdc1)->x()<-0.2 || (*iter_mdc1)->x()>1.2 || (*iter_mdc1)->y()<-0.9 || (*iter_mdc1)->y()>0.5 || abs((*iter_mdc1)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.2 || (*iter_mdc2)->x()>1.2 || (*iter_mdc2)->y()<-0.9 || (*iter_mdc2)->y()>0.5 || abs((*iter_mdc2)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<174.0 || delta_phi>186.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.4 || e2 < 1.4 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Psi double prime data taken in end of 2010 and 2011
      else if( m_data == "psipp11" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.3 || (*iter_mdc1)->y()<-0.3 || (*iter_mdc1)->y()>0.15 || abs((*iter_mdc1)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.3 || (*iter_mdc2)->y()<-0.3 || (*iter_mdc2)->y()>0.15 || abs((*iter_mdc2)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<174.0 || delta_phi>184.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.4 || e2 < 1.4 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Psi prime data taken in end of 2011 and 2012
      else if( m_data == "psip12" ) {
	if( (*iter_mdc1)->x()<-0.25 || (*iter_mdc1)->x()>0.3 || (*iter_mdc1)->y()<-0.3 || (*iter_mdc1)->y()>0.15 || abs((*iter_mdc1)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.25 || (*iter_mdc2)->x()>0.3 || (*iter_mdc2)->y()<-0.3 || (*iter_mdc2)->y()>0.15 || abs((*iter_mdc2)->z())>6.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<172.0 || delta_phi>188.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.4 || e2 < 1.4 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Jpsi data taken in 2012
      else if( m_data == "jpsi12" ) {
	if( (*iter_mdc1)->x()<-0.2 || (*iter_mdc1)->x()>0.4 || (*iter_mdc1)->y()<-0.4 || (*iter_mdc1)->y()>0.2 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.2 || (*iter_mdc2)->x()>0.4 || (*iter_mdc2)->y()<-0.4 || (*iter_mdc2)->y()>0.2 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<172.0 || delta_phi>188.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.1 || e2 < 1.1 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      // Y(4260) and Y(4360) taken in 2013
      else if( m_data == "psi13" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.35 || (*iter_mdc1)->y()<-0.35 || (*iter_mdc1)->y()>0.15 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.35 || (*iter_mdc2)->y()<-0.35 || (*iter_mdc2)->y()>0.15 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<172.0 || delta_phi>188.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1 < 1.5 || e2 < 1.5 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      else if( m_data == "rxyz14" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.35 || (*iter_mdc1)->y()<-0.35 || (*iter_mdc1)->y()>0.15 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.35 || (*iter_mdc2)->y()<-0.35 || (*iter_mdc2)->y()>0.15 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<175.0 || delta_phi>185.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1/p1 < 0.75 || e2/p2 < 0.75 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      else if( m_data == "r15" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.35 || (*iter_mdc1)->y()<-0.35 || (*iter_mdc1)->y()>0.15 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.35 || (*iter_mdc2)->y()<-0.35 || (*iter_mdc2)->y()>0.15 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<175.0 || delta_phi>185.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1/p1 < 0.75 || e2/p2 < 0.75 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      else if( m_data == "data16" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.35 || (*iter_mdc1)->y()<-0.35 || (*iter_mdc1)->y()>0.2 || abs((*iter_mdc1)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.35 || (*iter_mdc2)->y()<-0.35 || (*iter_mdc2)->y()>0.2 || abs((*iter_mdc2)->z())>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<170.0 || delta_phi>190.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1/p1 < 0.75 || e2/p2 < 0.75 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      else if( m_data == "data17" ) {
	if( (*iter_mdc1)->x()<-0.15 || (*iter_mdc1)->x()>0.35 || (*iter_mdc1)->y()<-0.3 || (*iter_mdc1)->y()>0.2 || (*iter_mdc1)->z()<-3.5 || (*iter_mdc1)->z()>4.5 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.15 || (*iter_mdc2)->x()>0.35 || (*iter_mdc2)->y()<-0.3 || (*iter_mdc2)->y()>0.2 || (*iter_mdc2)->z()<-3.5 || (*iter_mdc2)->z()>4.5 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<170.0 || delta_phi>190.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1/p1 < 0.75 || e2/p2 < 0.75 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }
      else if( m_data == "jpsi18" ) {
	if( (*iter_mdc1)->x()<-0.1 || (*iter_mdc1)->x()>0.4 || (*iter_mdc1)->y()<-0.3 || (*iter_mdc1)->y()>0.2 || (*iter_mdc1)->z()<-3.5 || (*iter_mdc1)->z()>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(10); }
	if( (*iter_mdc2)->x()<-0.1 || (*iter_mdc2)->x()>0.4 || (*iter_mdc2)->y()<-0.3 || (*iter_mdc2)->y()>0.2 || (*iter_mdc2)->z()<-3.5 || (*iter_mdc2)->z()>4.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(11); }
	if( delta_phi<170.0 || delta_phi>190.0 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(12); }
	if(  m_calibData == "Bhabha" ) {
	  if( e1/p1 < 0.75 || e2/p2 < 0.75 ) return StatusCode::SUCCESS;
	  if( m_printOutInfo ) { m_printOut->addNumber(13); }
	}
      }

      if(  m_calibData == "Bhabha" ) {
	if( ( etot - e1 - e2 ) > 0.3 ) return StatusCode::SUCCESS;
	if( m_printOutInfo ) { m_printOut->addNumber(14); }
      }
      else if( m_calibData == "Dimu" ) {
	if( e1 > 0.5 || e2 > 0.5 )return StatusCode::SUCCESS;	
	if( m_printOutInfo ) { m_printOut->addNumber(13); }
      }
    }
    //sunss add 08/9/17

    TofTrackVec* tofTrackVec = new TofTrackVec;
    RecExtTrackCol::iterator iter_track = extTrackCol->begin();
    for( ; iter_track < extTrackCol->end(); iter_track++ ) {
      RecMdcTrackCol::iterator iter_mdc = mdcTrackCol->begin();
      for( ; iter_mdc != mdcTrackCol->end(); iter_mdc++ ) {
	if( (*iter_mdc)->trackId() == (*iter_track)->trackId() ) break;
      }
      double costheta = cos( (*iter_mdc)->theta() );
      RecMdcKalTrackCol::iterator iter_kal = mdcKalTrackCol->begin();
      for( ; iter_kal != mdcKalTrackCol->end(); iter_kal++ ) {
	if( (*iter_kal)->trackId() == (*iter_track)->trackId() ) break;
      }
      double p[5]   = {-1.0};
      int    kal[5] = {-1};
      if( iter_kal != mdcKalTrackCol->end() ) {
	for( unsigned int i=0; i<5; i++ ) {
	  if( i==0 )      { (*iter_kal)->setPidType(RecMdcKalTrack::electron); }
	  else if( i==1 ) { (*iter_kal)->setPidType(RecMdcKalTrack::muon); }
	  else if( i==2 ) { (*iter_kal)->setPidType(RecMdcKalTrack::pion); }
	  else if( i==3 ) { (*iter_kal)->setPidType(RecMdcKalTrack::kaon); }
	  else if( i==4 ) { (*iter_kal)->setPidType(RecMdcKalTrack::proton); }
	  p[i]   = (*iter_kal)->p3().mag();
	  kal[i] = (*iter_kal)->getStat(0,i);
	}
      }
      TofTrack* tof = new TofTrack( run, event );
      tof->setExtTrack( (*iter_track), costheta, p, kal, t0, t0Stat );

      if( tofTrackVec->size()>0 ) {
	std::vector<TofTrack*>::iterator iterExt = tofTrackVec->begin();
	for( ; iterExt < tofTrackVec->end(); iterExt++ ) {
	  if( (*iterExt)->isNoHit() ) continue;
	  tof->getMultiHit( *iterExt );
	}
      }

      tofTrackVec->push_back( tof );
    }

    if( m_printOutInfo ) {
      m_printOut->setTrack1Col( tofTrackVec );
    }

    // Retrieve Tof Digi Data
    TofDataMap tofDataMap = tofDigiSvc->tofDataMapTof( t0 );
    if( tofDataMap.empty() ) {
      log << MSG::WARNING << "No Tof Data Map in RawDataProviderSvc!   Run=" << run << " Event=" << event << endreq;
    }

    if( m_checkDigi && m_checkDigiRaw ) {
      SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
      if( !tofDigiCol ) {
	log << MSG::ERROR << "TofRec could not find Tof digi! Event = " << event << endreq;
      }
      else { m_checkdigi_tuple->FillCol( *eventHeader, tofDigiCol, t0, t0Stat ); }

      m_checkdigi_tuple->FillCol( *eventHeader, tofDataMap, t0, t0Stat );
    }

    std::vector<int> deadId;
    if( m_forCalibration ) {
      for( unsigned int i=0; i<5; i++ ) {
	int identmp = tofCaliSvc->BrEast(i);
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
    }
 
    std::vector<TofTrack*>::iterator iter = tofTrackVec->begin();
    for( ; iter < tofTrackVec->end(); iter++ ) {
      if( (*iter)->isNoHit() ) continue;
      (*iter)->setTofData( tofDataMap );
      if( m_printOutInfo ) { m_printOut->setTrack2( (*iter) ); }
      if( (*iter)->isNoHit() ) continue;
      (*iter)->match( m_forCalibration, deadId, tofTrackVec );
      if( m_printOutInfo ) { m_printOut->setTrack3( (*iter) ); }
    }

    iter = tofTrackVec->begin();
    for( ; iter < tofTrackVec->end(); iter++ ) {

      (*iter)->setCalibration();

      if( m_checkDigi ) {
	if( m_checkTrigger ) {
	  // retrieve trigger data for physics analysis
	  SmartDataPtr<TrigData> trigData(eventSvc(), "/Event/Trig/TrigData");
	  if (!trigData) {
	    log << MSG::FATAL << "Could not find Trigger Data for physics analysis" << endreq;
	    m_checkdigi_tuple->Fill_TofTrack( *eventHeader, *iter, t0, t0Stat );
	  }
	  else {
	    m_checkdigi_tuple->Fill_TofTrack( *eventHeader, *iter, t0, t0Stat, trigData );
	  }
	}
	else {
	  if( ( run < 0 ) && m_checkMcTruth ) {
	    SmartDataPtr<TofMcHitCol> tofMcCol(eventSvc(),"/Event/MC/TofMcHitCol");
	    SmartDataPtr<McParticleCol> mcParticleCol(eventSvc(),"/Event/MC/McParticleCol");
	    if ( !tofMcCol || !mcParticleCol ) {
	      m_checkdigi_tuple->Fill_TofTrack( *eventHeader, *iter, t0, t0Stat, mdcKalTrackCol );
	    }
	    else {
	      m_checkdigi_tuple->Fill_TofTrack( *eventHeader, *iter, t0, t0Stat, mdcKalTrackCol, tofMcCol, mcParticleCol, m_calibData );
	    }
	  }
	  else {
	    m_checkdigi_tuple->Fill_TofTrack( *eventHeader, *iter, t0, t0Stat, mdcKalTrackCol );
	  }
	}
      }
    }

    tds.RegisterTDS( eventHeader->runNumber(), eventHeader->eventNumber(), tofTrackVec, m_forCalibration, m_calibData );

    clearTofTrackVec( tofTrackVec );

// Check RecTofTrackCol Registered
    SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc(),"/Event/Recon/RecTofTrackCol");  
    if (!tofTrackCol) { 
      log << MSG::FATAL << "TofRec could not find RecTofTrackCol!" << endreq;
      return StatusCode::FAILURE;
    }
    else{
      if( m_saveRootFile ) {
	m_checkdata_tuple->FillCol( *eventHeader, tofTrackCol, mdcKalTrackCol );
      }
    }

    if( m_forCalibration ) {
      SmartDataPtr<RecBTofCalHitCol> bhitCol(eventSvc(),"/Event/Recon/RecBTofCalHitCol");
      if (!bhitCol) { 
	log << MSG::WARNING << "TofRec could not find RecBTofCalHitCol!" << endreq;
      }
      else {
	if( m_saveRootFile ) {
	  m_checkdata_tuple->FillCol( *eventHeader, bhitCol );
	}
      }

      SmartDataPtr<RecETofCalHitCol> ehitCol(eventSvc(),"/Event/Recon/RecETofCalHitCol");
      if (!ehitCol) { 
	log << MSG::WARNING << "TofRec could not find RecETofCalHitCol!" << endreq;
      }
      else {
	if( m_saveRootFile ) {
	  m_checkdata_tuple->FillCol( *eventHeader, ehitCol );
	}
      }
    }

  }
  else {
    log << MSG::FATAL << "In TofRec: AcceleratorStatus is NOT correct! m_acceleratorStatus = " << m_acceleratorStatus << endreq;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;

}

//   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofRec::finalize() {
  if( m_printOutInfo ) {
    if( m_forCalibration ) {
      m_printOut->finalBhabha( m_calibData );
    }
    m_printOut->final();
    delete m_printOut;
  }
  if( m_checkDigi ) delete m_checkdigi_tuple;
  if( m_saveRootFile ) delete m_checkdata_tuple;
  return StatusCode::SUCCESS;
}


//----------------------------------------------------------------------------

void TofRec::clearTofTrackVec( std::vector<TofTrack*>*& tofTrackVec) {
  if( tofTrackVec ) {
    std::vector<TofTrack*>::iterator iter = tofTrackVec->begin();
    for( ; iter < tofTrackVec->end(); iter++ ) {
      delete (*iter);
    }
    tofTrackVec->clear();
    delete tofTrackVec;
  }
  return;
}
