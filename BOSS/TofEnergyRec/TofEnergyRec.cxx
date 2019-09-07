// add bhabha selection code 9/17
//
//  Package: TofEnergyRec
//  BESIII Tof Energy Reconstruction Algorithm. 
//  Created by He Miao (EPC IHEP)
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
#include "RawDataProviderSvc/TofData.h"
#include "DstEvent/TofHitStatus.h"
#include "EvTimeEvent/RecEsTime.h"
#include "ExtEvent/RecExtTrack.h"
#include "DstEvent/TofHitStatus.h"
#include "TofRecEvent/RecTofTrack.h"
#include "TofRecEvent/RecBTofCalHit.h"
#include "TofRecEvent/RecETofCalHit.h"
#include "TofRecEvent/RecBTofHit.h"
#include "TofRecEvent/RecETofHit.h"
#include "TofGeomSvc/ITofGeomSvc.h"
#include "TofCaliSvc/ITofCaliSvc.h"

#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "TofEnergyRec/TofEnergyRec.h"
#include "TofEnergyRec/TofShower.h"
#include <iostream>


using namespace std;
using namespace Event;

/////////////////////////////////////////////////////////////////////////////

IRawDataProviderSvc* tofDigiSvc;

TofEnergyRec::TofEnergyRec(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{   
  declareProperty("Output",           m_output = false);
  declareProperty("EventNb",          m_eventNb = 0);
  declareProperty("CalibConst",       m_calibConst = 0.0085);
  declareProperty("IsData",           m_isData = true);
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofEnergyRec::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TofEnergyRec in initialize()" << endreq;   

  m_event = 0;
  m_tofShower = new TofShower;
  m_tofShower->setCalibConst(m_calibConst);
  m_tofShower->setIsData(m_isData);
  //m_tofShower->readCalibPar();

  if(m_output) {
    cout<<"--------Book Tof Energy Ntuple!"<<endl;
    NTuplePtr nt(ntupleSvc(), "FILE209/sci");
    NTuplePtr nt1(ntupleSvc(), "FILE209/shower");
    NTuplePtr nt2(ntupleSvc(), "FILE209/seed");
    if ( nt&&nt1&&nt2 ) {
      m_tuple = nt;
      m_tuple1 = nt1;
      m_tuple2 = nt2;
    } else {
      m_tuple = ntupleSvc()->book ("FILE209/sci", CLID_ColumnWiseTuple, "TofEnergyRec");
      m_tuple1 = ntupleSvc()->book ("FILE209/shower", CLID_ColumnWiseTuple, "TofEnergyRec");
      m_tuple2 = ntupleSvc()->book ("FILE209/seed", CLID_ColumnWiseTuple, "TofEnergyRec");
      if ( m_tuple&&m_tuple1&&m_tuple2 ) {
        m_tofShower->BookNtuple(m_tuple,m_tuple1,m_tuple2);
      }
      else    { 
        log << MSG::ERROR << " ----- TofShower cannot book N-tuple:" << long(m_tuple) << endmsg;
      }
    }
  }


  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofEnergyRec::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "TofEnergyRec in execute()!!" << endreq;

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if( !eventHeader ) {
    log << MSG::FATAL << "TofEnergyRec could not find Event Header!" << endreq;
    return StatusCode::FAILURE;
  }
  int run = eventHeader->runNumber();
  int event = eventHeader->eventNumber();
  if( m_eventNb!=0 && m_event%m_eventNb== 0 ) std::cout << "--------event: " << m_event 
    << ", run= " << run << "  event= " << event << std::endl;
  log << MSG::INFO << "run= " << run << "  event= " << event << endreq;
  m_event++;


  IRawDataProviderSvc* tofDigiSvc;
  StatusCode sc = service("RawDataProviderSvc", tofDigiSvc);
  if(sc != StatusCode::SUCCESS) {
    log << MSG::FATAL << "TofEnergyRec could not find RawDataProviderSvc!" << endreq;
    return StatusCode::SUCCESS;
  }

  //check whether the Recon has been already registered
  DataObject *aReconEvent;
  eventSvc()->findObject("/Event/Recon",aReconEvent);
  if(aReconEvent==NULL) {
    log << MSG::INFO << "Register ReconEvent" << endreq;
    //then register Recon
    aReconEvent = new ReconEvent();
    sc = eventSvc()->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return StatusCode::FAILURE;
    }
  }

  //Register RecTofTrackCol
  SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc(),EventModel::Recon::RecTofTrackCol);
  if (!tofTrackCol) {
    log << MSG::INFO << "Register RecTofTrackCol!" << endreq;
    tofTrackCol = new RecTofTrackCol;
    sc = eventSvc()->registerObject(EventModel::Recon::RecTofTrackCol, tofTrackCol);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register RecTofTrackCol!" <<endreq;
    }
  }



  //Retrieve RecEsTimeCol 
  SmartDataPtr<RecEsTimeCol> estimeCol(eventSvc(),"/Event/Recon/RecEsTimeCol");
  if( !estimeCol || ( estimeCol->size() == 0 ) ) {
    log << MSG::WARNING << "TofRec Could not find RecEsTimeCol!   Run = " << run << " Event = " << event << endreq;                                                                               
    return StatusCode::SUCCESS;
  }
  RecEsTimeCol::iterator iter_ESTime=estimeCol->begin();
  double t0=0;
  t0  = (*iter_ESTime)->getTest();
  int t0Stat = (*iter_ESTime)->getStat();


  std::vector<TofData*> tofDataVec;
  tofDataVec = tofDigiSvc->tofDataVectorTof();


  m_tofShower->findShower(tofDataVec, tofTrackCol, t0);

  return StatusCode::SUCCESS;

}

//   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

StatusCode TofEnergyRec::finalize() {
  cout << "Total event:" << m_event << endl;
  delete m_tofShower;
  return StatusCode::SUCCESS;
}
