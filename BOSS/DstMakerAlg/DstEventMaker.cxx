#include "DstMakerAlg/DstEventMaker.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"

#include "TofRecEvent/RecTofTrack.h"
#include "EmcRecEventModel/RecEmcShower.h"
#include "MucRecEvent/RecMucTrack.h"
#include "MucRecEvent/MucRecHit.h"
#include "MdcRecEvent/RecMdcDedx.h"
#include "MdcRecEvent/RecMdcDedxHit.h"
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ExtEvent/RecExtTrack.h"  // track extrapolation  add 2005-09-02
//#include "ExtEvent/ExtMucHit.h"
#include "MdcRecEvent/RecMdcKalTrack.h" // Mdc Kal Add 2005-09-24

#include "EventModel/EventModel.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"

#include "DstEvent/DstEvent.h"
#include "DstEvent/DstTofTrack.h"
#include "DstEvent/DstEmcShower.h"
#include "DstEvent/DstMdcTrack.h"
#include "DstEvent/DstMucTrack.h"
#include "DstEvent/DstMdcDedx.h"
#include "DstEvent/DstExtTrack.h"      // track extrapolation  add 2005-09-02
#include "DstEvent/DstMdcKalTrack.h"   //Mdc Kal add 2005-09-24

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

#include <vector>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <map>

using namespace std;
using namespace Event;
//using CLHEP::HepVector;
using CLHEP::Hep3Vector;
using CLHEP::HepSymMatrix;

//typedef std::vector<MdcRecHit*> HitRefVec;

/////////////////////////////////////////////////////////////////////////////

DstEventMaker::DstEventMaker(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
 // declareProperty("FittingMethod", m_fittingMethod = 2);
 // declareProperty("ConfigFile",  m_configFile = "MDCConfig.xml");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstEventMaker::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstEventMaker::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;

  // Part 1: Get the event header, print out event and run number
  
  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
 

  int eventId = eventHeader->eventNumber() ;
  int runId = eventHeader->runNumber();
  log << MSG::INFO << "DstEventMaker: retrieved event: " <<"  Event Number  "<< eventId << "  run: " <<runId  << endreq;
  
  StatusCode sc = StatusCode::SUCCESS ;
  int trackID;

  // register ReconEvent Data Object to TDS; 2005/09/14
  DstEvent* dstEvt = new DstEvent();
  //dstEvt->setEventId(eventId);
  //dstEvt->setRunId(runId);
  sc = eventSvc()->registerObject(EventModel::Dst::Event, dstEvt);  
 

  //Part 2: Retrieve Mdc Track 
  SmartDataPtr<RecMdcTrackCol> recMdcTrackCol(eventSvc(),"/Event/Recon/RecMdcTrackCol");
  if (!recMdcTrackCol) {
    log << MSG::INFO << "Could not find RecMdcTrackCol" << endreq;
  } else {
    DstMdcTrackCol* dstMdcTrackCol = new DstMdcTrackCol();
    RecMdcTrackCol::iterator iter_mdc = recMdcTrackCol->begin();
    trackID = 0;
    for (;iter_mdc != recMdcTrackCol->end(); iter_mdc++, trackID++) {
      DstMdcTrack* dstMdcTrack = new DstMdcTrack(); 
      *dstMdcTrack = **iter_mdc;
      (*dstMdcTrackCol).push_back(dstMdcTrack);

      log << MSG::INFO 
	<< " Mdc Track ID = " << trackID
	<< " Mdc Track Nster = " << (*iter_mdc)->nster()
	<< endreq; 
    } 
    sc = eventSvc()->registerObject(EventModel::Dst::DstMdcTrackCol,dstMdcTrackCol);
  }

  //Part 3: Retrieve Tof Track 
  SmartDataPtr<RecTofTrackCol> tofTrackCol(eventSvc(),"/Event/Recon/RecTofTrackCol");
  if ( !tofTrackCol ) {
    log << MSG::INFO << "Could not find TofTrackCol" << endreq;
  }
  else {
    DstTofTrackCol* dstTofTrackCol = new DstTofTrackCol();
    RecTofTrackCol::iterator iter_tof = tofTrackCol->begin();
    for( ; iter_tof != tofTrackCol->end(); iter_tof++ ) {
      DstTofTrack* dstTofTrack = new DstTofTrack();
      *dstTofTrack = **iter_tof;
      (*dstTofTrackCol).push_back(dstTofTrack);
      
      log << MSG::INFO
	  << "Tof Track ID = " << (*iter_tof)->tofTrackID()
	  << " Track ID = " << (*iter_tof)->trackID()
	  << " Tof Counter ID = "<<(*iter_tof)->tofID()
	  << " Quality = "<< (*iter_tof)->quality()
	  << endreq;	
    }
    sc = eventSvc()->registerObject(EventModel::Dst::DstTofTrackCol,dstTofTrackCol);
  }


  //Part 4: Retrieve Muc Track 
  SmartDataPtr<RecMucTrackCol> mucTrackCol(eventSvc(),"/Event/Recon/RecMucTrackCol");
  if (!mucTrackCol) {
    log <<MSG::INFO << "Could not find RecMucTrackCol" << endreq;
  } else {
    DstMucTrackCol* dstMucTrackCol = new DstMucTrackCol();
    RecMucTrackCol::iterator iter_muc = mucTrackCol->begin();
    trackID = 0;
    for (;iter_muc != mucTrackCol->end(); iter_muc++, trackID++) {
      DstMucTrack* dstMucTrack = new DstMucTrack();
      *dstMucTrack = **iter_muc;
      (*dstMucTrackCol).push_back(dstMucTrack);
      log<< MSG::INFO 
	<< "Muc Track ID = " << trackID
	<< " Muc NumHits  = " << (*iter_muc)->GetTotalHits()
	<<  " Muc Pos x = " <<  (*iter_muc)->getMucPos().x()
	<<  " Muc Pos y = " <<  (*iter_muc)->getMucPos().y()
	<<  " Muc Pos z = " <<  (*iter_muc)->getMucPos().z()
	<< endreq;
    } 
    sc = eventSvc()->registerObject(EventModel::Dst::DstMucTrackCol,dstMucTrackCol);
  }
 

  //Part 5: Retrieve MdcDedx Track 
        
  SmartDataPtr<RecMdcDedxCol> mdcDedxCol(eventSvc(),"/Event/Recon/RecMdcDedxCol");
  if (!mdcDedxCol) {
    log <<MSG::INFO << "Could not find RecMdcDedxCol" << endreq;
  } else {
  trackID = 0;
  DstMdcDedxCol* dstMdcDedxCol = new DstMdcDedxCol();
  RecMdcDedxCol::iterator iter_dedx = mdcDedxCol->begin();
  for (;iter_dedx != mdcDedxCol->end(); iter_dedx++, trackID++) {
     DstMdcDedx* dstMdcDedx = new DstMdcDedx();
     *dstMdcDedx = **iter_dedx;
     (*dstMdcDedxCol).push_back(dstMdcDedx);
	log << MSG::INFO 
	    << " Dedx TrackID = "<< trackID
      //      << " Num Sigma E = " << (*iter_dedx)->getSigmaDedx(1)
      //     << " Num Sigma Mu = " << (*iter_dedx)->getSigmaDedx(2)
            << endreq;    
  }
  sc = eventSvc()->registerObject(EventModel::Dst::DstMdcDedxCol,dstMdcDedxCol);
  }



  //Part 6: Retrieve Emc shower 
        
  SmartDataPtr<RecEmcShowerCol> emcRecShowerCol(eventSvc(),"/Event/Recon/RecEmcShowerCol");
  if (!emcRecShowerCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find RecEmcShowerCol" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  trackID = 0;
  DstEmcShowerCol* dstEmcTrackCol = new DstEmcShowerCol();
  RecEmcShowerCol::iterator iter_emc = emcRecShowerCol->begin();
  for (;iter_emc != emcRecShowerCol->end(); iter_emc++, trackID++) {
    DstEmcShower* dstEmcTrack = new DstEmcShower(*(*iter_emc));
    (*dstEmcTrackCol).push_back(dstEmcTrack);
    log << MSG::INFO 
      << " Emc Track Id =  " << trackID
      //<< " Emc Shower Cell Id = " << (*iter_emc)->ShowerId()
      << endreq;
  }
  sc = eventSvc()->registerObject(EventModel::Dst::DstEmcShowerCol,dstEmcTrackCol);
 }

 //Part 7: Retrieve Ext shower 
        
  SmartDataPtr<RecExtTrackCol> extTrackCol(eventSvc(),"/Event/Recon/RecExtTrackCol");
  if (!extTrackCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find RecExtTrackCol" << endreq;
    //return( StatusCode::FAILURE);
  }
  else {
  trackID = 0;
  DstExtTrackCol* dstExtTrackCol = new DstExtTrackCol();
  RecExtTrackCol::iterator iter_ext = extTrackCol->begin();
  for (;iter_ext != extTrackCol->end(); iter_ext++, trackID++) {
    DstExtTrack* dstExtTrack = new DstExtTrack();
    *dstExtTrack = **iter_ext;
    (*dstExtTrackCol).push_back(dstExtTrack);
	  log << MSG::INFO 
	      << " Ext Track Id =  " << trackID
	      << "volume name of Tof1 : " << (*iter_ext)->tof1VolumeName()
	      <<" Ext Tof1 Error Z = " <<  (*iter_ext)->tof1PosSigmaAlongZ()
  	    <<" Ext Tof2 Error Z = " <<  (*iter_ext)->tof2PosSigmaAlongZ()
	      <<" Ext Emc Error theta = " <<  (*iter_ext)->emcPosSigmaAlongTheta()
	      <<" Ext Muc Error Z = " <<  (*iter_ext)->mucPosSigmaAlongZ()
        << endreq;    
  }

  sc = eventSvc()->registerObject(EventModel::Dst::DstExtTrackCol,dstExtTrackCol);
  }
  //Part 8: Retrieve MdcKal track 
        
  SmartDataPtr<RecMdcKalTrackCol> mdcKalTrackCol(eventSvc(),"/Event/Recon/RecMdcKalTrackCol");
  if (!mdcKalTrackCol) {
    log <</* MSG::FATAL*/  MSG::INFO<< "Could not find MdcKalTrackCol" << endreq;
    //return( StatusCode::FAILURE);
  }
 else {
   trackID = 0;
   int   nhits[5];
   int   stat[2][5];
   float chisq[2][5];
   int   ndf[2][5];
   int   nster[2][5];
   int   fL[2][5];
   int   lL[2][5];

   DstMdcKalTrackCol* dstMdcKalTrackCol = new DstMdcKalTrackCol();
   RecMdcKalTrackCol::iterator iter_kal = mdcKalTrackCol->begin();
   for (;iter_kal != mdcKalTrackCol->end(); iter_kal++, trackID++) {
     DstMdcKalTrack* dstMdcKalTrack = new DstMdcKalTrack();
     *dstMdcKalTrack = **iter_kal;

     /*
     for (int i=0; i<5; i++){
       nhits[i] = (*iter_kal)->getNhits(i);
       stat[i]  =  (*iter_kal)->getStat(0,i);
       chisq[i] = (*iter_kal)->getChisq(0,i);
       ndf[i]   = (*iter_kal)->getNdf(0,i);
       nster[i]   = (*iter_kal)->getNster(0,i);
       fL[i]  = (*iter_kal)->getFirstLayer(0,i);
       lL[i]  = (*iter_kal)->getLastLayer(0,i);
  
       dstMdcKalTrack->setNhits(nhits[i],i);
       dstMdcKalTrack->setStat(stat[i],i);
       dstMdcKalTrack->setChisq(chisq[i],i);
       dstMdcKalTrack->setNdf(ndf[i],i);
       dstMdcKalTrack->setNster(nster[i],i);
       dstMdcKalTrack->setFirstLayer(fL[i],i);
       dstMdcKalTrack->setLastLayer(lL[i],i);
     }
     dstMdcKalTrack->setTrackId((*iter_kal)->getTrackId());
     
     for(int j=0; j<5; j++){
       dstMdcKalTrack->setPoca((*iter_kal)->getPoca(j),j);
       dstMdcKalTrack->setZHelix((*iter_kal)->getZHelix(j),j);
       dstMdcKalTrack->setZError((*iter_kal)->getZError(j),j);
     }

     */
     
     (*dstMdcKalTrackCol).push_back(dstMdcKalTrack);
 
     log<<MSG::INFO<<" * * * * * * * * * * * * * * * * * * * * * * * * * * *"<<endreq;
     for(int i =0 ; i < 5; i++){
       log << MSG::INFO<<"dstMdcKalTrack ZHelix["<<i<<"] = " <<dstMdcKalTrack->getZHelix(i) <<endreq;
       log << MSG::INFO << "number of total hits contained ["<<i<<"]"<<nhits[i]<<endreq;
       log << MSG::INFO<< "status flag ["<<i<<"]"<<stat[i]<<endreq;
       log << MSG::INFO<< "dstMdcKalTrack ZError ["<<i<<"]"<<dstMdcKalTrack->getZError(i)<<endreq;
       log << MSG::INFO<< "dstMdcKalTrack Poca ["<<i<<"]"<<dstMdcKalTrack->getPoca(i)<<endreq;
     } 
   } 
     sc = eventSvc()->registerObject(EventModel::Dst::DstMdcKalTrackCol, dstMdcKalTrackCol);	
  }

  return sc;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode DstEventMaker::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
