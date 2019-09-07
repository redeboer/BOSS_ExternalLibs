#include "RootIO/RootIoAlg.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "McTruth/McKine.h"

#include "DstEvent/DstTofTrack.h"
#include "DstEvent/DstMdcTrack.h"
#include "DstEvent/DstEmcShower.h"
#include "DstEvent/DstMucTrack.h"
#include "DstEvent/DstMdcDedx.h"
#include "DstEvent/DstExtTrack.h"
#include "DstEvent/DstMdcKalTrack.h"

#include "ReconEvent/ReconEvent.h"

#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/ThreeVector.h"

//#include "MdcGeomSvc/IMdcGeomSvc.h"
//#include "MdcGeomSvc/MdcGeoWire.h"
//#include "MdcGeomSvc/MdcGeoLayer.h"
#include "Identifier/Identifier.h"
#include <vector>
#include <iostream>

using namespace std;

// 2007/09/29
//static const AlgFactory<RootIoAlg>  Factory;
//const IAlgFactory& RootIoAlgFactory = Factory;

/////////////////////////////////////////////////////////////////////////////

RootIoAlg::RootIoAlg(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
//  declareProperty("FittingMethod", m_fittingMethod = 2);
//  declareProperty("ConfigFile",  m_configFile = "MDCConfig.xml");
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootIoAlg::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;
/*
  IMdcGeomSvc* mdcGeomSvc;
  StatusCode sc = service("MdcGeomSvc", mdcGeomSvc);
  if (sc ==  StatusCode::SUCCESS) {              
	  std::cout <<"dump"<<std::endl;
   mdcGeomSvc->Dump();
   std::cout<<"Hi, event routine is running"<<std::endl;
   std::cout<<"1st wire id:"<<mdcGeomSvc->Wire(0)->Id()<<std::endl;
  } else {
    return StatusCode::FAILURE;
  }
*/
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootIoAlg::execute() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  
  // Part 1: Get the event header, print out event and run number

  int event, run;


  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc(),"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::INFO << "RootIoAlg: retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;
 

  int digiId;

  //Part 3: Retrieve MDC digi 
  SmartDataPtr<MdcDigiCol> mdcDigiCol(eventSvc(),"/Event/Digi/MdcDigiCol");
  if (!mdcDigiCol) {
    log << /*MSG::FATAL*/MSG::INFO << "Could not find MDC digi" << endreq;
    //return( StatusCode::FAILURE);
  }
  else {
  MdcDigiCol::iterator iter1 = mdcDigiCol->begin();
  digiId = 0; 
  for (;iter1 != mdcDigiCol->end(); iter1++, digiId++) {
    log << MSG::INFO << "MDC digit No: " << digiId << endreq;
 
    log << MSG::INFO 
        << " time_channel = " << (*iter1)->getTimeChannel()
        << " charge_channel = " << (*iter1)->getChargeChannel() 
        << endreq;     
  }
  }


  //Part 4: Retrieve TOF digi 
  SmartDataPtr<TofDigiCol> tofDigiCol(eventSvc(),"/Event/Digi/TofDigiCol");
  if (!tofDigiCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find TOF digi" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  TofDigiCol::iterator iter2 = tofDigiCol->begin();
  digiId = 0; 
  for (;iter2 != tofDigiCol->end(); iter2++, digiId++) {
    log << MSG::INFO << "TOF digit No: " << digiId << endreq;
    log << MSG::INFO
        << " time_channel = " << (*iter2)->getTimeChannel()
        << " charge_channel = " << (*iter2)->getChargeChannel()
        << endreq;


  }
  }
  //Part 5: Retrieve EMC digi 
  SmartDataPtr<EmcDigiCol> emcDigiCol(eventSvc(),"/Event/Digi/EmcDigiCol");
  if (!emcDigiCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find EMC digi" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  EmcDigiCol::iterator iter3 = emcDigiCol->begin();
  digiId = 0; 
  for (;iter3 != emcDigiCol->end(); iter3++, digiId++) {
    log << MSG::INFO << "Emc digit No: " << digiId << endreq;
 
    log << MSG::INFO 
        << " time_channel = " << (*iter3)->getTimeChannel()
        << " charge_channel = " << (*iter3)->getChargeChannel() 
        << endreq;   
  }
  }
 

 //Part 6: Retrieve DST Tof Track 
  SmartDataPtr<DstTofTrackCol> dstTofTrackCol(eventSvc(),EventModel::Dst::DstTofTrackCol);
  if (!dstTofTrackCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find Dst Tof Track" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  DstTofTrackCol::iterator iter4 = dstTofTrackCol->begin();
  digiId = 0; 
  for (;iter4 != dstTofTrackCol->end(); iter4++, digiId++) {
    log << MSG::INFO << "Dst Tof No: " << digiId << endreq;
 
    log << MSG::INFO
	    <<" TrackID = " << (*iter4)->trackID()
	    <<" TofID = " <<(*iter4)->tofID()
	    <<" TofTrackID = " <<(*iter4)->tofTrackID()
	    <<" Beta = " << (*iter4)->beta()
	    << endreq;  
  }
  }
  
 //Part 7: Retrieve DST Emc Track 
  SmartDataPtr<DstEmcShowerCol> dstEmcShowerCol(eventSvc(),EventModel::Dst::DstEmcShowerCol);
  if (!dstEmcShowerCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find Dst Emc Track" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  DstEmcShowerCol::iterator iter5 = dstEmcShowerCol->begin();
  digiId = 0; 
  for (;iter5 != dstEmcShowerCol->end(); iter5++, digiId++) {
    log << MSG::INFO << "Dst Emc No: " << digiId << endreq;
 
    log << MSG::INFO
      << " Track ID = " <<  (*iter5)->trackId()
    	<<" Cell ID = " << (*iter5)->cellId()
      <<" Total energy observed in Emc = " << (*iter5)->energy()
      << endreq ;	     
  }
  }
  	
//Part 8: Retrieve DST Mdc Track 
  SmartDataPtr<DstMdcTrackCol> dstMdcTrackCol(eventSvc(),EventModel::Dst::DstMdcTrackCol);
  if (!dstMdcTrackCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find Dst Mdc Track" << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  DstMdcTrackCol::iterator iter6 = dstMdcTrackCol->begin();
  digiId = 0; 
  for (;iter6 != dstMdcTrackCol->end(); iter6++, digiId++) {
    log << MSG::INFO << "Dst Mdc No: " << digiId << endreq;
 
    log << MSG::INFO
        << " Track ID = " <<  (*iter6)->trackId()
      	<<" number of  total hits contained = " << (*iter6)->nster()     
      	<<"px = "<<(*iter6)->px()
      	<<"p = "<<(*iter6)->p()
  	<<endreq;
  }
  }

//Part 9: Retrieve DST Dedx Track 
  SmartDataPtr<DstMdcDedxCol> dstMdcDedxCol(eventSvc(),EventModel::Dst::DstMdcDedxCol);
  if (!dstMdcDedxCol) {
    log << /*MSG::FATAL*/MSG::INFO << "Could not find Dst Mdc Dedx " << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  DstMdcDedxCol::iterator iter7 = dstMdcDedxCol->begin();
  digiId = 0; 
  for (;iter7 != dstMdcDedxCol->end(); iter7++, digiId++) {
    log << MSG::INFO << "Dst Mdc Dedx  No: " << digiId << endreq;
 
    log << MSG::INFO
        << " Track ID = " <<  (*iter7)->trackId()
        <<"Chi of E = " << (*iter7)->chi(0)
	<<"Chi of Mu = " << (*iter7)->chi(1)
        <<"Chi of Pi = " << (*iter7)->chi(2)
        <<endreq;
  }
  }
 
//Part 9: Retrieve DST Muc Track 
  SmartDataPtr<DstMucTrackCol> dstMucTrackCol(eventSvc(),EventModel::Dst::DstMucTrackCol);
  if (!dstMucTrackCol) {
    log <</* MSG::FATAL*/MSG::INFO << "Could not find Muc Track " << endreq;
   // return( StatusCode::FAILURE);
  }
  else {
  DstMucTrackCol::iterator iter8 = dstMucTrackCol->begin();
  digiId = 0; 
  for (;iter8 != dstMucTrackCol->end(); iter8++, digiId++) {
    log << MSG::INFO << "Dst Muc Track  No: " << digiId << endreq;
 
    log << MSG::INFO
        << " Track ID = " <<  (*iter8)->trackId()
        <<" Number of hits on track  = " << (*iter8)->numHits()
	<<" maxHitsInLayer " << (*iter8)->maxHitsInLayer()
      	<<" xPos = " << (*iter8)->xPos()
        <<" yPos = " << (*iter8)->yPos() 
        <<" zPos = " << (*iter8)->zPos()
        << endreq;   
  }
  }
  
//Part 10: Retrieve DST Ext Track
  SmartDataPtr<DstExtTrackCol> dstExtTrackCol(eventSvc(),EventModel::Dst::DstExtTrackCol);
   if (!dstExtTrackCol) {
       log <</* MSG::FATAL*/MSG::INFO << "Could not find DstExt Track " << endreq;
      // return( StatusCode::FAILURE);
   }
  else {
  DstExtTrackCol::iterator iter9 = dstExtTrackCol->begin();

  digiId = 0;
  for (;iter9 != dstExtTrackCol->end(); iter9++, digiId++) {
     log << MSG::INFO << "Dst Ext Track  No: " << digiId << endreq;
     log << MSG::INFO
	 << "volume name of Tof1= "<< (*iter9)->tof1VolumeName()
         << "volume number of Tof1 = "<< (*iter9)->tof1VolumeNumber()
         << "Tof1 =  "<< (*iter9)->tof1()
         <<endreq;
     log << MSG::INFO
	 << "volume name of Tof2= "<< (*iter9)->tof2VolumeName()
	 << "volume number of Tof2 = "<< (*iter9)->tof2VolumeNumber()
	 << "Tof2 =  "<< (*iter9)->tof2()            
	 <<endreq;
     log << MSG::INFO
	 <<" Ext Tof1 Error Z = " <<  (*iter9)->tof1PosSigmaAlongZ()
	 <<" Ext Tof2 Error Z = " <<  (*iter9)->tof2PosSigmaAlongZ()
	 <<" Ext Emc Error theta = " <<  (*iter9)->emcPosSigmaAlongTheta()
	 <<" Ext Muc Error Z = " <<  (*iter9)->mucPosSigmaAlongZ()
         << endreq;
  }   
  }
//Part 11: Retrieve MdcKal Track
  SmartDataPtr<DstMdcKalTrackCol> dstMdcKalTrackCol(eventSvc(),EventModel::Dst::DstMdcKalTrackCol);
  if (!dstMdcKalTrackCol) {
      log <</* MSG::FATAL*/MSG::INFO << "Could not find DstMdcKal Track " << endreq;
  }
  else {
     DstMdcKalTrackCol::iterator iter10 = dstMdcKalTrackCol->begin();
     digiId = 0;
     for (;iter10 != dstMdcKalTrackCol->end(); iter10++, digiId++) {
	//log << MSG::INFO << "Dst MdcKal Track  No: " << digiId << endreq;
        //log << MSG::INFO
        //    << " stat of the fit of pid=0:" << (*iter10)->getStat(0) <<endreq;
        //log << MSG::INFO
        //    << "mass assumption during the fit of pid =0 :" << ((*iter10)->getZHelix(0)) <<endreq;
//         
     }
  }
  
/*
    // register ReconEvent Data Object to TDS;
  //ReconEvent* recevt = new ReconEvent;
  //StatusCode sc = eventSvc()->registerObject("/Event/Recon",recevt );

 //Part 6: Retrieve Tof Track    
      // register Tof Track Data Object to TDS;
  int trackID = 0;
  DstTofTrackCol* dstTofTrackCol = new DstTofTrackCol();

  for (trackID = 0;trackID < 10 ; trackID++) {
        log << MSG::INFO << "Tof Track No: " << trackID << endreq;
  
        DstTofTrack* dstTofTrack = new DstTofTrack();

        dstTofTrack->setId(trackID*100);
        dstTofTrack->setFlightTime(trackID*1.1);
        dstTofTrack->setBeta(trackID*1.11);

        log << MSG::INFO
	    <<" Counter Number = " << dstTofTrack->id()
	    <<" Time of Flight = " << dstTofTrack->flightTime()
	    <<" Beta = " << dstTofTrack->beta()
	    << endreq;
 
        (*dstTofTrackCol).push_back(dstTofTrack);
  } 
  StatusCode sc = eventSvc()->registerObject("/Event/Digi/DstTofTrackCol",dstTofTrackCol);

  //Part 7: Retrieve Mdc Track
      // register Mdc Track Data Object to TDS;
  trackID = 0;
  DstMdcTrackCol* dstMdcTrackCol = new DstMdcTrackCol();

  for ( trackID = 0;trackID < 10 ; trackID++) {
        log << MSG::INFO << "Mdc Track No: " << trackID << endreq;
  
        DstMdcTrack* dstMdcTrack = new DstMdcTrack();

        dstMdcTrack->setCharge(trackID*100);
        dstMdcTrack->setPxy(trackID*1.1);
        dstMdcTrack->setPx(trackID*1.11);

        log << MSG::INFO
	    <<" Charge = " << dstMdcTrack->charge()
	    <<" Pxy = " << dstMdcTrack->pxy()
	    <<" Px = " << dstMdcTrack->px()
	    << endreq;
 
        (*dstMdcTrackCol).push_back(dstMdcTrack);
  } 
  sc = eventSvc()->registerObject("/Event/Digi/DstMdcTrackCol",dstMdcTrackCol);

//Part 8: Retrieve Emc Track
      // register Tof Track Data Object to TDS;
  trackID = 0;
  DstEmcShowerCol* dstEmcShowerCol = new DstEmcShowerCol();

  for ( trackID = 0;trackID < 10 ; trackID++) {
        log << MSG::INFO << "Emc Track No: " << trackID << endreq;
  
        DstEmcShower* dstEmcShower = new DstEmcShower();

        dstEmcShower->setNumHits(trackID*100);
        dstEmcShower->setX(trackID*1.1);
        dstEmcShower->setDx(trackID*1.11);

        log << MSG::INFO
	    <<" NumHits = " << dstEmcShower->numHits()
	    <<" X = " << dstEmcShower->x()
	    <<" Dx = " << dstEmcShower->dx()
	    << endreq;
 
        (*dstEmcShowerCol).push_back(dstEmcShower);
  } 
  sc = eventSvc()->registerObject("/Event/Digi/DstEmcShowerCol",dstEmcShowerCol);
   
   //Part 9: Retrieve Muc Track
  SmartDataPtr<DstMucTrackCol> dstMucTrackCol(eventSvc(),"/Event/Digi/DstMucTrackCol");
  if (!dstMucTrackCol) {
    log << MSG::FATAL << "Could not find Dst Muc Track " << endreq;
    return( StatusCode::FAILURE);
  }
  trackID = 0;
  DstMucTrackCol::iterator iter9 = dstMucTrackCol->begin();
 
  for (;iter9 != dstMucTrackCol->end(); iter9++, trackID++) {
    log << MSG::INFO << "Dst Muc Track No.: " << trackID << endreq;
    log << MSG::INFO
        << " Projected x =  " << (*iter9)->xProj()
        << " Projected y =  " << (*iter9)->yProj()
        << " Projected z =  " << (*iter9)->zProj()
        << endreq;
  }
 
       
  //Part 10: Retrieve Dedx
      // register Dedx Data Object to TDS;
  trackID = 0;
  DstDedxCol* dstDedxCol = new DstDedxCol();

  for ( trackID = 0;trackID < 10 ; trackID++) {
        log << MSG::INFO << "Dedx No: " << trackID << endreq;
  
        DstDedx* dstDedx = new DstDedx();

        dstDedx->setParticleId(trackID*100);
        dstDedx->setNumSigmaE(trackID*1.1);
        dstDedx->setNumSigmaMu(trackID*1.11);

        log << MSG::INFO
	    <<" Particle ID from De/Dx fit = " << dstDedx->particleId()
	    <<" Number of sigama from e = " << dstDedx->numSigmaE()
	    <<" Number of sigama from muon = " << dstDedx->numSigmaMu()
	    << endreq;
 
        (*dstDedxCol).push_back(dstDedx);
  } 
 sc = eventSvc()->registerObject("/Event/Digi/DstDedxCol",dstDedxCol);
*/
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode RootIoAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}
