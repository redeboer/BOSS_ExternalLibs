// -*- C++ -*-
//
// Package:     MdcFastTrkAlg
// Module:      FTFinder
// 
// Description:  fast track finder class for MdcFastTrkAlg
//
//

#include <cmath>
#include <iostream>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ThreadGaudi.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/IPartPropSvc.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include "MdcGeomSvc/MdcGeoSuper.h"

#include "Identifier/Identifier.h"
#include "Identifier/MdcID.h"

#include "EventModel/EventHeader.h"
#include "MdcRawEvent/MdcDigi.h"
#include "RawEvent/RawDataUtil.h"
#include "EvTimeEvent/RecEsTime.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "TrigEvent/TrigData.h"
#include "TrigEvent/TrigGTD.h"
#include "TrigEvent/TrigMdc.h"
#include "TrigEvent/TrigGTDProvider.h"


#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "ReconEvent/ReconEvent.h"

#include "MdcFastTrkAlg/FTFinder.h"
#include "MdcFastTrkAlg/FTWire.h"
#include "MdcFastTrkAlg/FTLayer.h"
#include "MdcFastTrkAlg/FTSuperLayer.h"
#include "MdcFastTrkAlg/FTSegment.h"
#include "MdcFastTrkAlg/FTTrack.h"
#include "MdcFastTrkAlg/MdcParameter.h"

#ifndef OnlineMode
#include "McTruth/McParticle.h"

#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IHistogramFactory.h"
#endif


using namespace Event;
//...Globals...

//MCTruth

#ifndef OnlineMode

extern NTuple::Item<long>   g_ntrkMC, g_eventNo;
extern NTuple::Array<float> g_theta0MC, g_phi0MC;
extern NTuple::Array<float> g_pxMC, g_pyMC, g_pzMC, g_ptMC;

//recon

extern NTuple::Item<long>   g_ntrk;
extern NTuple::Array<float> g_px, g_py, g_pz, g_pt, g_p;
extern NTuple::Array<float> g_phi, g_theta;
extern NTuple::Array<float> g_vx, g_vy, g_vz;
extern NTuple::Array<float> g_dr, g_phi0, g_kappa, g_dz, g_tanl;
extern NTuple::Item<float>  g_estime;

extern IHistogram1D*   g_ncellMC;
extern IHistogram1D*   g_ncell;
extern IHistogram1D*   g_naxialhit;
extern IHistogram1D*   g_nstereohit;
extern IHistogram1D*   g_nhit;
extern IHistogram2D*   g_hitmap[20];

extern int num_2Dtrk, num_3Dtrk, num_finaltrk;

#endif


//float FTTrack::xtCoEff = 0.;  
//int   FTTrack::evtTiming = 0;  
//float FTTrack::Testime = 0;

//FTFinder * FTFinder::_tFinder = NULL;  

MdcParameter* FTFinder::param = MdcParameter::instance();

//FTFinder *
//FTFinder::GetPointer(void)
//{
//  if (!_tFinder) _tFinder = new FTFinder;
//  return _tFinder;
//}

FTFinder::FTFinder()
  :// findEventVertex(1),
  // evtTimeCorr(1),
  
  //  minPt(0.07),
  // minDr(7.5),
  // tOffSet(0.),
  // xtCoEff(0.0344),
  // doIt(1),
#ifndef OnlineMode
  // mkMdst(true),
#endif
  // mkTds(true),
  tOffSet(0.),
  t0Estime(-999.),
  tEstFlag(0),
  
  _wire(NULL),
  _layer(NULL),
  _superLayer(NULL),
  _tracks(*(new FTList<FTTrack *>(20))),
  _linkedSegments(new FTList<FTSegment *>(6)),
  _axialSegCollect(10),
  _vx(0.),
  _vy(0.),
  _vz(0.),
  _ExpNo(0),
  _RunNo(0),
  m_total_trk(0),
  pivot(0,0,0)
{
 StatusCode scmgn = Gaudi::svcLocator()->service("MagneticFieldSvc",m_pmgnIMF); 
 if(scmgn!=StatusCode::SUCCESS) {
   std::cout<< "Unable to open Magnetic field service"<<std::endl;
 }
}

void
FTFinder::init()
{
  if (!param->_doIt) return;
//	param->updateAlpha();

  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  static const bool CREATEIFNOTTHERE(true);
  StatusCode PartPropStatus = Gaudi::svcLocator()->service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    // log << MSG::ERROR << " Could not initialize Particle Properties Service" << endreq;
    std::cerr << "Could not initialize Particle Properties Service" << std::endl;  
    //  return PartPropStatus;
    return;
  }
  m_particleTable = p_PartPropSvc->PDT();
  
  // IRawDataProviderSvc* m_rawDataProviderSvc;
  StatusCode RawData = Gaudi::svcLocator()-> service ("RawDataProviderSvc", m_rawDataProviderSvc, CREATEIFNOTTHERE);
  if ( !RawData.isSuccess() ){
    std::cerr  << "Could not load RawDataProviderSvc!" << m_rawDataProviderSvc << endreq;
    return;
  }
 
//  IMdcGeomSvc* mdcGeomSvc;
//  StatusCode sc = Gaudi::svcLocator()->service("MdcGeomSvc", mdcGeomSvc);
//
//  if (sc ==  StatusCode::SUCCESS) {
//   //mdcGeomSvc->Dump();
//   
//  } else {
//    return ;
//  }
//  
//  const int Nwire = mdcGeomSvc->getWireSize();
//  const int Nlyr =  mdcGeomSvc->getLayerSize();
//  const int Nsup =  mdcGeomSvc->getSuperLayerSize();
//
//  if(!Nwire || !Nlyr){
//    std::cerr << "FTFINDER::GEOCDC not found (please put cdctable before l4)" << std::endl;
//    std::cerr << "JOB will stop" << std::endl;
//    exit(-1);
//  }
// 
//  if (!_wire) _wire = (FTWire *) malloc((Nwire+1) * sizeof(FTWire));
//  if (!_layer) _layer = (FTLayer *) malloc(Nlyr * sizeof(FTLayer));
//  if (!_superLayer) _superLayer =
//                      (FTSuperLayer *) malloc(Nsup * sizeof(FTSuperLayer));
// 
//  if (!_wire || !_layer || !_superLayer){
//    std::cerr << "FTFINDER::Cannot allocate geometries" << std::endl;
//    std::cerr << "JOB will stop" << std::endl;
//    exit(-1);
//  }
//
//  int superLayerID = 0;
//  int layerID = 0;
//  int localLayerID = 0;
//  int localWireID = 0;
//  int localID = 0;
//  int wireID;
//  MdcGeoLayer * layer_back = NULL;
//  MdcGeoSuper * superLayer_back = NULL;
//  int k = 0;
//  int Nlayer[12];
//  int Nlocal[12];
//  int NlocalWireID[44];
//
//  for (wireID = 0;wireID <= Nwire; wireID++){
//    MdcGeoLayer * layer = (wireID==Nwire) ? NULL : mdcGeomSvc->Wire(wireID)->Lyr();
//    if (layer != layer_back){
//      layer_back = layer;
//      MdcGeoSuper * superLayer = (wireID==Nwire) ? NULL : mdcGeomSvc->Layer(layerID)->Sup();
//      if (superLayer != superLayer_back){
//        superLayer_back = superLayer;
//        Nlayer[k] = localLayerID;
//        Nlocal[k] = localID;
//        localLayerID = 0;
//        k++;
//      }
//      NlocalWireID[layerID] = localWireID;
//      localID = 0;
//      localWireID = 0;
//      layerID++;
//      localLayerID++;
//    }
//    localID++;
//    localWireID++;
//  }
//
//
//  superLayerID = -1;
//  layerID = -1;
//  localLayerID = 0;
//  localID = 0;
//  layer_back = NULL;
//  superLayer_back = NULL;
//  for (wireID = 0;wireID < Nwire; wireID++){
//    MdcGeoLayer * layer = mdcGeomSvc->Wire(wireID)->Lyr();
//    if (layer != layer_back){
//      layer_back = layer;
//      MdcGeoSuper * superLayer = mdcGeomSvc->Layer(layerID+1)->Sup();
//      if (superLayer != superLayer_back){
//        superLayer_back = superLayer;
//        // initialize super-layer
//        superLayerID++;
//        new(_superLayer+superLayerID) FTSuperLayer(wireID,
//                                                   Nlocal[superLayerID+1],
//                                                   layerID+1,
//                                                   Nlayer[superLayerID+1],
//                                                   superLayerID);
//        localLayerID=0;
//      }
//      // initialize layer
//      layerID++;
//      double slantWithSymbol = (mdcGeomSvc->Layer(layerID)->Slant())*(mdcGeomSvc->Layer(layerID)->Sup()->Type());
//      // slant in new MdcGeomSvc include  symbol 
//       new(_layer+layerID) FTLayer(0.1*layer->Radius(), layer->Slant(),
//                                  0.1*(+layer->Length()/2), 0.1*(-layer->Length()/2), 0.1*layer->Offset(),
//                                  layerID, localLayerID++, NlocalWireID[layerID+1],
//                                  _superLayer[superLayerID]);
//      localID = 0;
//    }
//    // initialize wire
//    const MdcGeoWire * wire = mdcGeomSvc->Wire(wireID); 
//    if (superLayerID == 2 || superLayerID == 3  ||
//        superLayerID == 4 || superLayerID == 9 ||
//        superLayerID == 10){     // axial wire
//      new(_wire+wireID) FTWire(0.1*(float)0.5*(wire->Backward().x()+wire->Forward().x()),
//                               0.1*(float)0.5*(wire->Backward().y()+wire->Forward().y()),
//                               0.,0.,
//                               _layer[layerID], localID++, _wire+Nwire);
//			      
//    }else{                      // stereo wire
//      new(_wire+wireID) FTWire(0.1*(float)wire->Backward().x(),
//                               0.1*(float)wire->Backward().y(),
//                               0.1*(float)wire->Forward().x() - 0.1*(float)wire->Backward().x(),
//                               0.1*(float)wire->Forward().y() - 0.1*(float)wire->Backward().y(),
//                               _layer[layerID], localID++, _wire+Nwire);
//    }
//  }
// 
//  // make virtual wire object for the pointer of boundary's neighbor
//  new(_wire+Nwire) FTWire();
//
//  for (int i = 0; i < Nwire; i++){
//    (*(_wire+i)).initNeighbor(); 
//  }
//  
//  _widbase[0] = 0;
//  for (int k = 1; k < 43; k++){
//    _widbase[k] = _widbase[k-1] + (*(_layer+k-1)).NWire();
//  }

  //minPt = (float)param->_minPt;
  //minDr = (float)param->_minDr;
  //xtCoEff = param->_xtCoEff;
}

void
FTFinder::term()
{
  if (param->_doIt) clear();
  delete &_tracks;
  delete _linkedSegments;
  if (!param->_doIt) return;
  free(_wire);
  free(_layer);
  free(_superLayer);
}

void
FTFinder::begin_run(){

  eventNo = 0;
  runNo=0;
  expflag=0;
  if (!param->_doIt) return;
  _ExpNo = 0;
  _RunNo = 0;

  IMdcGeomSvc* mdcGeomSvc;
  StatusCode sc = Gaudi::svcLocator()->service("MdcGeomSvc", mdcGeomSvc);

  if (sc ==  StatusCode::SUCCESS) {
   //mdcGeomSvc->Dump();
   
  } else {
    return ;
  }
  
  const int Nwire = mdcGeomSvc->getWireSize();
  const int Nlyr =  mdcGeomSvc->getLayerSize();
  const int Nsup =  mdcGeomSvc->getSuperLayerSize();

  if(!Nwire || !Nlyr){
    std::cerr << "FTFINDER::GEOCDC not found (please put cdctable before l4)" << std::endl;
    std::cerr << "JOB will stop" << std::endl;
    exit(-1);
  }
 
  if (!_wire) _wire = (FTWire *) malloc((Nwire+1) * sizeof(FTWire));
  if (!_layer) _layer = (FTLayer *) malloc(Nlyr * sizeof(FTLayer));
  if (!_superLayer) _superLayer =
                      (FTSuperLayer *) malloc(Nsup * sizeof(FTSuperLayer));
 
  if (!_wire || !_layer || !_superLayer){
    std::cerr << "FTFINDER::Cannot allocate geometries" << std::endl;
    std::cerr << "JOB will stop" << std::endl;
    exit(-1);
  }

  int superLayerID = 0;
  int layerID = 0;
  int localLayerID = 0;
  int localWireID = 0;
  int localID = 0;
  int wireID;
  MdcGeoLayer * layer_back = NULL;
  MdcGeoSuper * superLayer_back = NULL;
  int k = 0;
  int Nlayer[12];
  int Nlocal[12];
  int NlocalWireID[44];

  for (wireID = 0;wireID <= Nwire; wireID++){
    MdcGeoLayer * layer = (wireID==Nwire) ? NULL : mdcGeomSvc->Wire(wireID)->Lyr();
    if (layer != layer_back){
      layer_back = layer;
      MdcGeoSuper * superLayer = (wireID==Nwire) ? NULL : mdcGeomSvc->Layer(layerID)->Sup();
      if (superLayer != superLayer_back){
        superLayer_back = superLayer;
        Nlayer[k] = localLayerID;
        Nlocal[k] = localID;
        localLayerID = 0;
        k++;
      }
      NlocalWireID[layerID] = localWireID;
      localID = 0;
      localWireID = 0;
      layerID++;
      localLayerID++;
    }
    localID++;
    localWireID++;
  }


  superLayerID = -1;
  layerID = -1;
  localLayerID = 0;
  localID = 0;
  layer_back = NULL;
  superLayer_back = NULL;
  for (wireID = 0;wireID < Nwire; wireID++){
    MdcGeoLayer * layer = mdcGeomSvc->Wire(wireID)->Lyr();
    if (layer != layer_back){
      layer_back = layer;
      MdcGeoSuper * superLayer = mdcGeomSvc->Layer(layerID+1)->Sup();
      if (superLayer != superLayer_back){
        superLayer_back = superLayer;
        // initialize super-layer
        superLayerID++;
        new(_superLayer+superLayerID) FTSuperLayer(wireID,
                                                   Nlocal[superLayerID+1],
                                                   layerID+1,
                                                   Nlayer[superLayerID+1],
                                                   superLayerID);
        localLayerID=0;
      }
      // initialize layer
      layerID++;
      double slantWithSymbol = (mdcGeomSvc->Layer(layerID)->Slant())*(mdcGeomSvc->Layer(layerID)->Sup()->Type());
      // slant in new MdcGeomSvc include  symbol 
       new(_layer+layerID) FTLayer(0.1*layer->Radius(), layer->Slant(),
                                  0.1*(+layer->Length()/2), 0.1*(-layer->Length()/2), 0.1*layer->Offset(),
                                  layerID, localLayerID++, NlocalWireID[layerID+1],
                                  _superLayer[superLayerID]);
      localID = 0;
    }
    // initialize wire
    const MdcGeoWire * wire = mdcGeomSvc->Wire(wireID); 
    if (superLayerID == 2 || superLayerID == 3  ||
        superLayerID == 4 || superLayerID == 9 ||
        superLayerID == 10){     // axial wire
      new(_wire+wireID) FTWire(0.1*(float)0.5*(wire->Backward().x()+wire->Forward().x()),
                               0.1*(float)0.5*(wire->Backward().y()+wire->Forward().y()),
                               0.,0.,
                               _layer[layerID], localID++, _wire+Nwire);
			      
    }else{                      // stereo wire
      new(_wire+wireID) FTWire(0.1*(float)wire->Backward().x(),
                               0.1*(float)wire->Backward().y(),
                               0.1*(float)wire->Forward().x() - 0.1*(float)wire->Backward().x(),
                               0.1*(float)wire->Forward().y() - 0.1*(float)wire->Backward().y(),
                               _layer[layerID], localID++, _wire+Nwire);
    }
  }
 
  // make virtual wire object for the pointer of boundary's neighbor
  new(_wire+Nwire) FTWire();

  for (int i = 0; i < Nwire; i++){
    (*(_wire+i)).initNeighbor(); 
  }
  
  _widbase[0] = 0;
  for (int k = 1; k < 43; k++){
    _widbase[k] = _widbase[k-1] + (*(_layer+k-1)).NWire();
  }
}

void
FTFinder::event() {
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  
  if (!param->_doIt) return;
  //--
  // clear old information
  //--
  clear();

  //check whether  Recon  already registered
  DataObject *aReconEvent;
  eventSvc->findObject("/Event/Recon",aReconEvent);
  if(!aReconEvent) {
    // register ReconEvent Data Object to TDS;
    ReconEvent* recevt = new ReconEvent;
    StatusCode sc = eventSvc->registerObject("/Event/Recon",recevt );
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return;
    }
  }
  //register Event start time
  IDataManagerSvc *dataManSvc = dynamic_cast<IDataManagerSvc*> (eventSvc);
  DataObject *aEsTimeEvent;
  eventSvc->findObject("/Event/Recon/RecEsTimeCol",aEsTimeEvent);
  if(aEsTimeEvent != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecEsTimeCol");
    eventSvc->unregisterObject("/Event/Recon/RecEsTimeCol");
  }

  RecEsTimeCol *aRecEsTimeCol = new RecEsTimeCol;
  StatusCode est;
  est = eventSvc->registerObject("/Event/Recon/RecEsTimeCol",aRecEsTimeCol);
  if( est.isFailure() ) {
      log << MSG::FATAL << "Could not register RecEsTimeCol" << endreq;
      return;
   }
  log << MSG::DEBUG << "RecEsTimeCol registered successfully!" <<endreq; 
  //--
  // update wirehit information
  //--
  updateMdc();

  //--
  // segment finding
  //--
  for (int i =  0; i^11; i++){
    (*(_superLayer+i)).mkSegmentList();
  }

  //--
  // reduce noise and get start time from segment fit
  for(int j=0;j<10;j++){
    (*(_superLayer+j)).reduce_noise(tEstime);
  }

  getTestime();
  
  //--
  // register t0Estime to TDS
  //--
  //if(t0Estime!=-999&&t0Estime<24) registT0();

  //--
  // axial segment linking
  //--
  mkTrackList();

  //--
  // 2D track fitting
  //--
  //(*_superLayer).reAppendSalvage();

  int n = _tracks.length();
  log << MSG::DEBUG << "number of 2D tracks: " << n <<endreq;
#ifndef OnlineMode
  num_2Dtrk+=n;
#endif
  for (int i = 0; i^n; i++){
    if (!_tracks[i]->r_phiFit()){
      delete _tracks[i];
      log<<MSG::DEBUG<<"===========>deleted 2D track :  "<< i+1 <<endreq;
      n = _tracks.remove(i);
    }
  }

  if(t0Estime!=-999){
    //begin to make Event start time
    RecEsTime *arecestime = new RecEsTime;
    arecestime -> setTest(t0Estime);
    arecestime -> setStat(tEstFlag);
    aRecEsTimeCol->push_back(arecestime);
  }
  if (!n){
    makeTds();
    return;
  }

  ///--
  /// vertex(r-phi) fit and event timing correction
  //--
  int vtx_flag = VertexFit(0);
  evtTiming = (param->_evtTimeCorr) ? CorrectEvtTiming() : 0;

  ///--
  /// 2D track re-fitting
  //--
  if(param->_hitscut==1){
    for (int i = 0; i^n; i++){
      _tracks[i]->r_phiReFit(_vx, _vy, vtx_flag);
    }
  }

  //--
  //cut bad hits from 2D track re-fitting
  if(param->_hitscut==2){
    for (int i = 0; i^n; i++){
      for(int j = 0; j<2; j++){
	i_rPhiFit= _tracks[i]->r_phi2Fit(_vx, _vy, vtx_flag);
	if(i_rPhiFit!=-99) _tracks[i]->r_phi3Fit(i_rPhiFit,_vx, _vy, vtx_flag);
      }
      _tracks[i]->r_phi4Fit(_vx, _vy, vtx_flag);
    }
  }

  //--
  // stereo segment linking
  //--
  mkTrack3D();

  //--
  // final track fittng
  //--
  n = _tracks.length();
  log<< MSG::DEBUG <<"number of 3D tracks: " << n << endreq;

#ifndef OnlineMode
  num_3Dtrk+=n;
#endif

  for (int i = 0; i^n; i++) {

#ifndef OnlineMode
    log<<MSG::DEBUG<<"=======>3D track: "<<i<<endreq;
    _tracks[i]->printout();
#endif

    if (_tracks[i]->get_nhits() < 18) {
      delete _tracks[i];
      log<< MSG::DEBUG <<"================>deleted 3D track :  "<< i+1 <<endreq;
      n = _tracks.remove(i);
    }
  }
  
  if (!n){
    makeTds();
    return;
  }

  for (int i = 0; i^n; i++){
    _tracks[i]->s_zFit();
  }

  //--
  // find primary event vertex
  //--
  if (param->_findEventVertex){
    VertexFit(1);
  }
  
  n = _tracks.length();
  log<<MSG::DEBUG<<"final number of tracks: " << n << endreq;

#ifndef OnlineMode
  num_finaltrk+=n;
  if (param->_mkMdst) makeMdst();
#endif

  //--
  // output tracking result into TDS
  // by wangdy
  //--
  if (param->_mkTds) makeTds();

}

int
FTFinder::updateMdc(void){
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);


  if (eventSvc) {
     //log << MSG::INFO << "Could find event Svc" << endreq;
  } else {
     log << MSG::FATAL << "Could not find Event Header" << endreq;
    return 0;
  }


  // Part 1: Get the event header, print out event and run number

  SmartDataPtr<Event::EventHeader> eventHeader(eventSvc,"/Event/EventHeader");
  if (!eventHeader) {
    log << MSG::FATAL << "Could not find Event Header" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::DEBUG << "MdcFastTrkAlg: retrieved event: " << eventHeader->eventNumber()  << "  run: " <<  eventHeader->runNumber() << endreq;
  eventNo = eventHeader->eventNumber();
  runNo = eventHeader->runNumber();
  if(runNo>0) expflag=1;
  else  expflag=0;
  int digiId;

#ifndef OnlineMode
  g_eventNo = eventHeader->eventNumber();

  // Part 2: Retrieve MC truth
   SmartDataPtr<McParticleCol> mcParticleCol(eventSvc,"/Event/MC/McParticleCol");
   if (!mcParticleCol) {
    log << MSG::WARNING << "Could not find McParticle" << endreq;
   // return( StatusCode::FAILURE);
   }
   else{
     McParticleCol ::iterator iter_mc = mcParticleCol->begin();
     digiId = 0;
     int ntrkMC = 0;
     int charge = 0;
     for (;iter_mc != mcParticleCol->end(); iter_mc++, digiId++) {
       log << MSG::DEBUG << "MDC digit No: " << digiId << endreq;
       log << MSG::DEBUG
	   << " particleId = " << (*iter_mc)->particleProperty ()
	   << endreq;
       int  statusFlags = (*iter_mc)->statusFlags();
       int  pid = (*iter_mc)->particleProperty();
       if(pid >0) {
         if(m_particleTable->particle( pid )){ 
	   charge = (int)m_particleTable->particle( pid )->charge();
	 }
       } else if ( pid <0 ) {
         if(m_particleTable->particle( -pid )){
	   charge = (int)m_particleTable->particle( -pid )->charge();
	   charge *= -1;
	 }
       } else {
	 log << MSG::WARNING << "wrong particle id, please check data" <<endreq;
       }
       
       if(charge==0 || abs(cos((*iter_mc)->initialFourMomentum().theta()))>0.93) continue; 
       
       g_theta0MC[ntrkMC] = (*iter_mc)->initialFourMomentum().theta();
       g_phi0MC[ntrkMC] = (*iter_mc)->initialFourMomentum().phi();
       g_pxMC[ntrkMC] = (*iter_mc)->initialFourMomentum().px();
       g_pyMC[ntrkMC] = (*iter_mc)->initialFourMomentum().py();
       g_pzMC[ntrkMC] = (*iter_mc)->initialFourMomentum().pz();
       g_ptMC[ntrkMC] = 0.001 * sqrt(g_pxMC[ntrkMC]*g_pxMC[ntrkMC] + g_pyMC[ntrkMC]*g_pyMC[ntrkMC]);
       ntrkMC++;
     }
     g_ntrkMC = ntrkMC;
   }
#endif

#ifdef MultiThread
   //Part 3: Retrieve MDC digi
    SmartDataPtr<MdcDigiCol> mdcDigiVec(eventSvc,"/Event/Digi/MdcDigiCol");
    if (!mdcDigiVec) {
     log << MSG::FATAL << "Could not find MDC digi" << endreq;
     return( StatusCode::FAILURE);
    }
   MdcDigiCol::iterator iter1 = mdcDigiVec->begin();
   digiId = 0;
   Identifier mdcId;
   int layerId;
   int wireId;
   for (;iter1 != mdcDigiVec->end(); iter1++, digiId++) {
#else
   //--use RawDataProvider to get MdcDigi
   //  bool m_dropRecHits=true;//or false 
   MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec();
   MdcDigiVec::iterator iter1 = mdcDigiVec.begin();
   digiId = 0;
   Identifier mdcId;
   int layerId;
   int wireId;
   for (;iter1 != mdcDigiVec.end(); iter1++, digiId++) {
#endif
     mdcId = (*iter1)->identify();
     layerId = MdcID::layer(mdcId);
     wireId = MdcID::wire(mdcId);
#ifndef OnlineMode
     log << MSG::DEBUG << "MDC digit No: " << digiId << endreq;
     log << MSG::DEBUG
	 << " time_channel = " << RawDataUtil::MdcTime((*iter1)->getTimeChannel())
	 << " charge_channel = " << RawDataUtil::MdcCharge((*iter1)->getChargeChannel())
	 << " layerId = " << layerId 
	 << " wireId = " << wireId
	 << endreq;
#endif

        const int localwid = wireId; 
	const int wid = localwid + _widbase[layerId];

#ifndef OnlineMode
       g_ncellMC->fill((float)wid, 1.0);
#endif
        //... skip invalid wireID's
        if (wid < 0 || wid > 6795) continue;
        FTWire & w = *(_wire + wid);
        float tdc =  RawDataUtil::MdcTime((*iter1)->getTimeChannel());
        float adc =  RawDataUtil::MdcCharge((*iter1)->getChargeChannel());

#ifndef OnlineMode
        if (eventNo == 466)  {
             g_hitmap[0]->fill(w.x(), w.y());
        }

        if (eventNo == 538)  {
             g_hitmap[1]->fill(w.x(), w.y());
        }

        if (eventNo == 408)  {
             g_hitmap[2]->fill(w.x(), w.y());
        }

        if (eventNo == 499)  {
             g_hitmap[3]->fill(w.x(), w.y());
        }

        if (eventNo == 603)  {
             g_hitmap[4]->fill(w.x(), w.y());
        }

        if (eventNo == 938)  {
             g_hitmap[5]->fill(w.x(), w.y());
        }
#endif
        // tdc/adc calibration
	//new tdc include drift time and flight time
        float time = tdc-sqrt(w.x()*w.x()+w.y()*w.y())/30;
        w.time(time);
        w.wireId(wid);
        w.setAdc(adc); 
 
        //... check adc validation
        const FTLayer & lyr = w.layer();

        //... save to the array
	//w.distance(time*40*0.0001);  //original
	w.distance(0.0); //by X.-R. Lu
	w.setChi2(999);
        w.state(FTWireHit);
        FTSuperLayer & sup = (FTSuperLayer &) lyr.superLayer();
        sup.appendHit(&w);
   }
   
   return 1;
}


void
FTFinder::clear(void){
  evtTiming = 0;
  
  if ( _superLayer ) {
     for (int i = 0; i^11; i++) (*(_superLayer+i)).clear();
  }
  _tracks.deleteAll();
  _axialSegCollect.deleteAll();
  _vx = -99999.;
  _vy = -99999.;
  _vz = -99999.;
  for (int i = 0; i < 10; i++) {
    tEstime[i].clear();
  }
}

int 
FTFinder::getTestime(void)
{
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

   float sumT=0,estime=0;
   int n=0;
   t0Estime=-999;
   // for(int i=0;i<4;i++){
   for(int i=0;i<10;i++){
    for(int j=0;j<tEstime[i].length();j++){
      if(tEstime[i][j]!=0){
	sumT+=tEstime[i][j];
	n++;
      }
    }
   }
   if(n!=0){
     estime=sumT/n;
		 estime+=_t0cal;
     if(estime>-1){  // && estime<50){
       //if(m_nbunch==3){
       if(expflag==0){      
	 int nbunch=((int)(estime-tOffSet))/_bunchtime;
	 if(((int)(estime-tOffSet))%(int)_bunchtime>_bunchtime/2) nbunch=nbunch+1;
	 t0Estime=nbunch*_bunchtime+tOffSet;
	 //cout<<"_bunchtime: "<< _bunchtime<<" , t0Estime :  "<< t0Estime<<endl;
       }
       else{
	 t0Estime=estime;
       }
       //if(m_nbunch==6){
       /* int nbunch=((int)(estime-tOffSet))/4;
	  if(((int)(estime-tOffSet))%4>2) nbunch=nbunch+1;
	  t0Estime=nbunch*4+tOffSet;
       */
       // if(t0Estime>-1 && t0Estime<24)  FTTrack::Testime=t0Estime;
       int trigtimming=0;
       // Retrieve trigger timming information
       // timing system: TOF:1,  MDC:2, EMC:3, NONE:0
       SmartDataPtr<TrigData> trigData(eventSvc,"/Event/Trig/TrigData");
       if (trigData) {
	 trigtimming=trigData->getTimingType();
	 log << MSG::INFO <<"Timing type: "<< trigData->getTimingType() << endreq;
       }
       if(trigtimming==1)  tEstFlag=117;
       if(trigtimming==2)  tEstFlag=127;
       if(trigtimming==3)  tEstFlag=137;
       if(trigtimming==0)  tEstFlag=107;
#ifndef OnlineMode
       g_estime=estime;
#endif
     }
   }
}

void
FTFinder::mkTrackList(void)
{

  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);

  MsgStream log(msgSvc, "FTFinder");

  FTTrack ** tracks = (FTTrack **)malloc( 6 * sizeof(FTTrack *));
  int * Nsame = (int *)malloc( 6 * sizeof(int));
  int ntrack_cand = 0;
  int i,j,k,l;

  //for (int i =0; i < tracks().lenght(); i++) {
  //  FTTrack* fttrk = tracks()[i];
  //  fttrk->setFTFinder(this);
  //}

  FTList<FTSegment *> inner_segments(10);
  FTList<FTSegment *> outer_segments(10);

  linkAxialSuperLayer234(inner_segments);
  linkAxialSuperLayer910(outer_segments);

  _axialSegCollect.append(inner_segments);
  _axialSegCollect.append(outer_segments);

  int innerN = inner_segments.length();
  int outerN = outer_segments.length();

  log << MSG::DEBUG << innerN <<" segments in inner axial layers!"<<endreq;
  log << MSG::DEBUG << outerN <<" segments in outer axial layers!"<<endreq;

  for (l = 0; l^innerN; l++){
    if (inner_segments[l]->track()) continue;
    FTSegment *inner = inner_segments[l];
    FTTrack *track_cand = NULL;

#ifndef OnlineMode
    inner->printout();
#endif
    
    if(outerN==0){ 
      track_cand = linkAxialSegments(&inner, NULL);
      if(!track_cand) continue;
      _linkedSegments = new FTList<FTSegment *>(5);
      //      track_cand->printout();
      // append this to _tracks
      FTList<FTSegment *>& segments = track_cand->axial_segments();
      int n=segments.length();
      for (i = 0; i < n; i++){
   	segments[i]->track(track_cand);
      }
      _tracks.append(track_cand);
      track_cand->setFTFinder(this);
      ntrack_cand++;
      continue;
    }
    
    for(k = 0; k^outerN; k++) {
      if (outer_segments[k]->track()) { //already used
	if(k==outerN-1) {  //if the last outer segment
	  if (inner_segments[l]->track()) continue;   //inner segment never used
	  track_cand = linkAxialSegments(&inner, NULL);
	  if(!track_cand) continue;
	  _linkedSegments = new FTList<FTSegment *>(5);
	  // append this to _tracks
	  FTList<FTSegment *>& segments = track_cand->axial_segments();
	  int n=segments.length();
	  for (i = 0; i < n; i++){
	    segments[i]->track(track_cand);
	  }
	  _tracks.append(track_cand);
	  track_cand->setFTFinder(this);
	  ntrack_cand++;
	}
	continue;
      }

      FTSegment *outer = outer_segments[k];      
      track_cand = linkAxialSegments(&inner, &outer);

      if (!track_cand ){  // link failed
	if( k == outerN-1 ) { //if the last outer segment
	  if (inner_segments[l]->track()) continue;  //inner segment never used
	  track_cand = linkAxialSegments(&inner, NULL);
	}
	if(!track_cand) continue;
      }
      //  else {
      //    innerN = inner_segments.remove(l);
      //    outerN = outer_segments.remove(k);
      //  }
      //track_cand->printout();      

      ntrack_cand++;
      _linkedSegments = new FTList<FTSegment *>(5);
      
      //compair this to appended track candidate
      
      FTList<FTSegment *>& segments = track_cand->axial_segments();
      int n = segments.length();
      
      //compare this to the appended track candidate
      if(inner_segments[l]->track()) { //already used
	FTTrack * trk_tmp = inner_segments[l]->track();
	int nTrks = _tracks.length();
	int cache_i=0;
	for (i = 0; i^nTrks; i++){
	  if (_tracks[i] == trk_tmp){
	    cache_i = i;
	    break;
	  }
	}
	FTList<FTSegment *> & segments2 = trk_tmp->axial_segments();
	int n2 = segments2.length();
	if(n >= n2 && track_cand->chi2_kappa_tmp() < trk_tmp->chi2_kappa_tmp()){
	  // swap this and appended one
	  for (i = 0; i < n2; i++){
	    segments2[i]->track(NULL);
	  }	   
	  for (i = 0; i < n; i++){
	    segments[i]->track(track_cand);
	  }
	  _tracks.replace(cache_i,track_cand);
	  track_cand->setFTFinder(this);
	  delete trk_tmp;
	}else{
	  delete track_cand;
	}
      }else{
	// append this
	for (i = 0; i < n; i++){
	  segments[i]->track(track_cand);
	}
	_tracks.append(track_cand);
	track_cand->setFTFinder(this);
      }
    } // end of loop of outerN
  } //end of loop innerN
  
  free(tracks);
  free(Nsame);

#ifndef OnlineMode
  int n=_tracks.length();
  for(int i=0; i<n; i++){
    FTList<FTSegment *> & segments = _tracks[i]->axial_segments(); 
    log << MSG::DEBUG <<" loop connected axial track: " << i <<endreq;  
    int l=segments.length();    
    for(int j=0; j<l; j++){
      segments[j]->printout();
    }
  }
#endif

}

FTTrack *
FTFinder::linkAxialSegments(FTSegment **inner, FTSegment ** outer)
{
  float chi2_kappa = param->_chi2_kappa;
  _linkedSegments->clear();
  if(!outer) {
      // allow only 2, 3, 4,  axial segments in one track
      int n = (*inner)->wireHits().length();
      _linkedSegments->append(*inner);
      if(n >= 7) return (new FTTrack(*_linkedSegments, (*inner)->kappa(), chi2_kappa));
      else return NULL;
  }
  int n = _linkedSegments->append(*outer);
  float SigmaK = (*outer)->kappa();
  float SigmaRR = (*outer)->r();
  SigmaRR *= SigmaRR;
  float SigmaKRR = SigmaK*SigmaRR;
  float SigmaKKRR = SigmaK*SigmaKRR;
  FTSegment & s = * (*_linkedSegments)[n-1];
  FTSegment * innerSegment = NULL;
  float SigmaK_cache, SigmaRR_cache, SigmaKRR_cache, SigmaKKRR_cache;

  float Min_chi2 = param->_Min_chi2;
  float inX = s.incomingX();
  float inY = s.incomingY();
  //const FTWire & innerBoundHit = * s.innerBoundHits().first();
  //float in_r = innerBoundHit.layer().r();
  //float incomingPhi = innerBoundHit.phi();
  float in_r = s.innerBoundHits().first()->layer().r();
  float incomingPhi = s.incomingPhi();

  FTSegment* next = *inner;
  const FTWire & NextOuterBoundHit = * next->outerBoundHits().first();
    
  //float deltaPhi =fabs(incomingPhi - next->outgoingPhi());
  //if (deltaPhi > param->_deltaPhi  && deltaPhi < (2*M_PI-param->_deltaPhi)) return NULL;   
  float outX = next->outgoingX();
  float outY = next->outgoingY();

  //////////////////////////////////////////////////////
  float _trk_d = -2*(-1. / 2.99792458 /m_pmgnIMF->getReferField())/s.kappa();
  float _angle1 = asin(NextOuterBoundHit.layer().r()/_trk_d);
  float _angle2 = asin(s.outerBoundHits().first()->layer().r()/_trk_d);
  float _ang_diff = _angle2 - _angle1;
  float _diff = s.outgoingPhi() - next->outgoingPhi();
  _diff = _diff - (int(_diff/M_PI))*2*M_PI;
  float _require = _ang_diff - _diff;
  //cut of connecting inner and outer segments
  if (_require < -0.10 || _require > 0.11) return NULL;

  float SegK = next->kappa();
  float SegRR = next->r();
  SegRR *= SegRR;
  const float out_r = NextOuterBoundHit.layer().r();
  // kappa = -2. * alpha * ((Vout X Vin)_z / |Vin|*|Vout|) / |Vin-Vout|
  float GapK = 2.*(-1. / 2.99792458 /m_pmgnIMF->getReferField())*(inX*outY-inY*outX) /
          (in_r*out_r*sqrt((inX-outX)*(inX-outX)+(inY-outY)*(inY-outY)));
  //float GapRR = (currentLayer==j+1||currentLayer==j+2) ? 0.5*(in_r+out_r) : in_r+out_r;
  float GapRR = 0.5*(in_r+out_r);
  GapRR*=GapRR;
  float SigmaK_tmp = (SigmaK + SegK + GapK);
  float SigmaRR_tmp = SigmaRR + SegRR + GapRR;
  float SigmaKRR_tmp = SigmaKRR + SegK*SegRR + GapK*GapRR;
  float SigmaKKRR_tmp = SigmaKKRR + SegK*SegK*SegRR + GapK*GapK*GapRR;
  float MuK_tmp = SigmaK_tmp/(2*n+1);
  float chi2 = (MuK_tmp*MuK_tmp*SigmaRR_tmp
          - 2.*MuK_tmp*SigmaKRR_tmp + SigmaKKRR_tmp)/(2*n+1);
  if ((chi2-chi2_kappa) < Min_chi2){
    Min_chi2 = chi2;
    innerSegment = next;
    SigmaK_cache = SigmaK_tmp;
    SigmaRR_cache = SigmaRR_tmp;
    SigmaKRR_cache = SigmaKRR_tmp;
    SigmaKKRR_cache = SigmaKKRR_tmp;
  }
  if (innerSegment){
    n = _linkedSegments->append(*inner);
    SigmaK = SigmaK_cache;
    SigmaRR = SigmaRR_cache;
    SigmaKRR = SigmaKRR_cache;
    SigmaKKRR = SigmaKKRR_cache;
    chi2_kappa = Min_chi2;
    
    if (n > 1) return (new FTTrack(*_linkedSegments,SigmaK/(2*n-1),chi2_kappa));
  }else{
    // allow only 3, 4, 5 axial segments in one track
    /*n = (*inner)->wireHits().length();
      _linkedSegments->clear();
      _linkedSegments->append(*inner);
      if(n > 10) return (new FTTrack(*_linkedSegments, SegK, Min_chi2));*/
    return NULL;
  }
  //if (fabs(SigmaK/(2*n-1)) > 1.2/minPt) return NULL;
  return NULL;
  
}

void
FTFinder::linkAxialSuperLayer234(FTList<FTSegment *> & inner_segments)
{
  FTList<FTSegment *>  _segments34(10);
  FTList<FTSegment *>& SuperLayer3Segments = _superLayer[3].segments();
  FTList<FTSegment *>& SuperLayer4Segments = _superLayer[4].segments();
  linkAxialSegments_step(SuperLayer3Segments, SuperLayer4Segments,
			 _segments34, param->_D_phi2, param->_chi2_2);
  _segments34.append(SuperLayer3Segments);
  SuperLayer3Segments.clear();

  FTList<FTSegment *>& SuperLayer2Segments = _superLayer[2].segments();
  linkAxialSegments_step(SuperLayer2Segments, _segments34,
			 inner_segments, param->_D_phi1, param->_chi2_1);
  inner_segments.append(_segments34);

  //zoujh: connect the 2&4 superlayers leap over 3
  int n = SuperLayer2Segments.length();
  int m = SuperLayer4Segments.length();
  for (int i = 0; i^n; i++) {
    FTSegment * inner = SuperLayer2Segments[i];
    //inner->printout();
    float in_outerPhi = inner->outgoingPhi();
    for (int j = 0; j^m; j++) {
      FTSegment * outer = SuperLayer4Segments[j];
      //      outer->printout();
      float out_innerPhi = outer->incomingPhi();
      float D_phi = fabs(in_outerPhi - out_innerPhi);
      if (D_phi > M_PI) D_phi = 2*M_PI - D_phi;
      /////////////////////////////////////////////////////
      if (D_phi > M_PI/12.5) continue;
      float inX   = inner->outgoingX();
      float inY   = inner->outgoingY();
      float outX  = outer->incomingX();
      float outY  = outer->incomingY();
      float in_r  = inner->outerBoundHits().first()->layer().r();
      float out_r = outer->innerBoundHits().first()->layer().r();
      float GapK = 2.*(-1. / 2.99792458 /m_pmgnIMF->getReferField())*(inY*outX-inX*outY) /
          (in_r*out_r*sqrt((inX-outX)*(inX-outX)+(inY-outY)*(inY-outY)));
      //float cache_in = ((inner->kappa()+outer->kappa()+GapK)/3.0 - inner->kappa())/in_r;
      float cache_in  = ((outer->kappa()+GapK)/3.0 - inner->kappa()*2.0/3.0)/in_r;
      float cache_out = ((inner->kappa()+GapK)/3.0 - outer->kappa()*2.0/3.0)/out_r;
      float cache_gap = ((inner->kappa()+outer->kappa())/3.0-GapK*2.0/3.0)*2.0/(in_r+out_r);
      float chi2_z = cache_in*cache_in + cache_out*cache_out + cache_gap*cache_gap;
      if (chi2_z > param->_D_phi1) continue;
      /////////////////////////////////////////////////////
      inner->connect_outer(outer->wireHits(),outer->outerBoundHits());
      inner->update();
      inner_segments.append(inner);
      n = SuperLayer2Segments.remove(i);
      m = SuperLayer4Segments.remove(j);
      delete outer;
      break;
    }
  }
  inner_segments.append(SuperLayer2Segments);
  SuperLayer2Segments.clear();
  inner_segments.append(SuperLayer4Segments);
  SuperLayer4Segments.clear();
}

void
FTFinder::linkAxialSuperLayer910(FTList<FTSegment *> & outer_segments)
{
  FTList<FTSegment *>& SuperLayer9Segments = _superLayer[9].segments();
  FTList<FTSegment *>& SuperLayer10Segments = _superLayer[10].segments();
  linkAxialSegments_step(SuperLayer9Segments, SuperLayer10Segments,
			 outer_segments, param->_D_phi3, param->_chi2_3);
  outer_segments.append(SuperLayer9Segments);
  SuperLayer9Segments.clear();
  outer_segments.append(SuperLayer10Segments);
  SuperLayer10Segments.clear();
}

void
FTFinder::linkAxialSegments_step(FTList<FTSegment *>& InnerSegments,
                                 FTList<FTSegment *>& OuterSegments,
				 FTList<FTSegment *>& Connected,
				 float maxDphi, float maxChi2)
{
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "FTFinder");

  int n = InnerSegments.length();
  int m = OuterSegments.length();
  for (int i = 0; i^n; i++){
     FTSegment * inner = InnerSegments[i];
#ifndef OnlineMode
     //     log<<MSG::DEBUG<<"linking: "<<endreq;
     //     inner->printout();
#endif
     const FTLayer & in_outerBound = inner->outerBoundHits().first()->layer();
     float in_outerPhi = inner->outgoingPhi();
     float min_Dphi = M_PI/2;
     int   min_Dphi_index = -1;
     for (int j = 0; j^m; j++) {
       FTSegment * outer = OuterSegments[j];
#ifndef OnlineMode
       //       log<<MSG::DEBUG<<"segment: "<<j<<endreq;
       //       outer->printout();
#endif
       float D_phi = fabs(in_outerPhi - outer->incomingPhi());
       if (D_phi > M_PI) D_phi = 2*M_PI - D_phi;
       if (D_phi > min_Dphi) continue;
       float inX   = inner->incomingX();
       float inY   = inner->incomingY();
       float outX  = outer->outgoingX();
       float outY  = outer->outgoingY();
       float in_r  = inner->innerBoundHits().first()->layer().r();
       float out_r = outer->outerBoundHits().first()->layer().r();
       float allK = 2.*(-1. / 2.99792458 /m_pmgnIMF->getReferField())*(inY*outX-inX*outY) /
	 (in_r*out_r*sqrt((inX-outX)*(inX-outX)+(inY-outY)*(inY-outY)));
       //float cache_in  = ((inner->kappa() + outer->kappa() + allK)/3.0 - inner->kappa())/in_r;
       float cache_in  = ((outer->kappa() + allK)/3.0 - inner->kappa()*2/3.0)/in_r;
       float cache_out = ((inner->kappa() + allK)/3.0 - outer->kappa()*2/3.0)/out_r;
       float cache_all = ((inner->kappa()+outer->kappa())/3.0-allK*2/3.0)*2.0/(in_r+out_r);
       float chi2_z = cache_in*cache_in + cache_out*cache_out + cache_all*cache_all;
       log<<MSG::DEBUG<<"D_phi: "<< D_phi <<" chi2_z: "<< chi2_z <<" maxChi2: " <<maxChi2 <<endreq;
       if (chi2_z > maxChi2) continue;
       min_Dphi = D_phi;
       min_Dphi_index = j;
     }
     if (min_Dphi_index < 0) continue;
     log<<MSG::DEBUG<<"min_Dphi: "<< min_Dphi <<endreq;
     FTSegment * outer = OuterSegments[min_Dphi_index];
     const FTLayer & out_innerBound = outer->innerBoundHits().first()->layer();
     switch (out_innerBound.layerId() - in_outerBound.layerId()) {
       case 1:
	 if (min_Dphi > maxDphi) continue;
	 break;
       case 2:
	 if (min_Dphi > maxDphi*1.5) continue;
	 break;
       case 3:
	 if (min_Dphi > maxDphi*2.25) continue;
	 break;
       default:
	 continue;
     }
     inner->connect_outer(outer->wireHits(), outer->outerBoundHits());
     inner->update();
     Connected.append(inner);
     n = InnerSegments.remove(i);
     delete outer;
     m = OuterSegments.remove(min_Dphi_index);
     log<<MSG::DEBUG<<"DONE!!"<<endreq;
  }
  return;
}

void
FTFinder::mkTrack3D(void){
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "FTFinder");
  
  int n = _tracks.length();
  // select segment candidate and cache sz
  FTList<FTSegment *> multi_trk_cand(20);
  for (int i=8; i>=0 ; i-- ){
    //if (i==2 || i==3 || i==4) continue;
    if (i == 4) i -= 3;
    FTList<FTSegment *> & segments = _superLayer[i].segments();
    int m = segments.length();
    for (int j = 0; j^m; j++){
      FTSegment * s = segments[j];
#ifndef OnlineMode
	s->printout();
#endif
      int nTrack = 0;
      FTTrack * t;
      for (int k = 0; k^n; k++){
#ifndef OnlineMode
	log<<MSG::DEBUG<<"coupling to track: " << k <<endreq;
	//_tracks[k]->printout();
#endif
	if (s->update3D(_tracks[k])){ // calculate s and z
	  t = _tracks[k];
	  nTrack++;
	}
      }
      switch(nTrack){
      case 0:
	continue;
      case 1:
	t->append_stereo_cache(s);
	break;
      default:
	multi_trk_cand.append(s);
	break;
      }
    }
    // whose relation between this is unique
    for (int j = 0; j^n; j++) _tracks[j]->updateSZ();
  }
  // link segments by tanLambda & dz
  for (int i = 0; i^n; i++) _tracks[i]->linkStereoSegments(); 
  n = multi_trk_cand.length();
  for (int i = 0; i^n; i++) multi_trk_cand[i]->linkStereoSegments();
}

int
FTFinder::VertexFit2D()
{
  int nTrks = _tracks.length();
  if (nTrks < 2){
    _vx = -99999.;
    _vy = -99999.;
    _vz = -99999.;
    return 0;
  }
  FTList<float> px(10);
  FTList<float> py(10);
  FTList<float> dx(10);
  FTList<float> dy(10);
  FTList<double> weight(10);
  for (int i = 0; i < nTrks; i++){
    const Lpav & la = _tracks[i]->lpav();
    HepVector a = la.Hpar(pivot);
    
    const float dr_i = a(1);
    if (fabs(a(1)) > 1.5) continue;
    const float px_i = - sin(a(2));
    const float py_i = cos(a(2));

    //const float dx_i = dr_i*py_i;
    //const float dy_i = -dr_i*px_i;

    float weight_i = la.chisq()/(la.nc()*0.02);

    px.append(px_i);
    py.append(py_i);
    dx.append(dr_i*py_i);
    dy.append(-dr_i*px_i);
    weight.append(exp(-weight_i*weight_i));
  }
  if (dx.length() < 2){
    _vx = -99999.;
    _vy = -99999.;
    _vz = -99999.;
    return 0;
  }
  double S_weight = 0.;
  for (int i = dx.length() - 1; i; i--){
    const float px_i = px[i];
    const float py_i = py[i];
    const float dx_i = dx[i];
    const float dy_i = dy[i];
    const float weight_i = weight[i];
    for (int j = 0; j < i; j++){
      const float px_j = px[j];
      const float py_j = py[j];
      //const float weight_j = weight[j];
      
      const float ddx = dx[j] - dx_i;
      const float ddy = dx[j] - dy_i;
    
      const float tmp_par = px_i*py_j - px_j*py_i;
      const float par = (py_j*ddx-px_j*ddy)/tmp_par;

      double weight_ij = weight_i*weight[j];
      S_weight += weight_i*weight[j];
      if (tmp_par==0 ||
	  par < -0.5 || (py_i*ddx-px_i*ddy)/tmp_par < -0.5 ||
	  fabs((px_i*px_j + py_i*py_j)/
	       sqrt((px_i*px_i+py_i*py_i)*(px_j*px_j+py_j*py_j)))>0.86){
	_vx += (dx_i+0.5*ddx)*weight_ij;
	_vy += (dy_i+0.5*ddy)*weight_ij;
      }else{
	_vx += (dx_i+par*px_i)*weight_ij;
	_vy += (dy_i+par*py_i)*weight_ij;
      }
    }
  }

  int rtn_flag = 0;
  if (S_weight == 0.){
    _vx = -99999.;
    _vy = -99999.;
    _vz = -99999.;
  }else{
    _vx /= S_weight;
    _vy /= S_weight;
    _vz = -99999.;
    rtn_flag = 1;
  }
  return rtn_flag;

}

int
FTFinder::VertexFit(int z_flag)
{
  _vx = 0.;
  _vy = 0.;
  _vz = 0.;
  if (!z_flag){
    return VertexFit2D();
  }
  int n = _tracks.length();
  if (n < 2){
    _vx = -99999.;
    _vy = -99999.;
    _vz = -99999.;
    return 0;
  }
  FTList<float> px(10);
  FTList<float> py(10);
  FTList<float> pz(10);
  FTList<float> dx(10);
  FTList<float> dy(10);
  FTList<float> dz(10);
  FTList<float> pmag2(10);
  FTList<float> weight(10);
  FTList<float> weight_z(10);
  FTList<float> sigma2_r(10);
  FTList<float> sigma2_z(10);
  
  for (int i = 0; i < n; i++){
    const Lpav & la = _tracks[i]->lpav();
    if (la.nc() <= 3) continue;
    const zav & za = _tracks[i]->Zav();
    if(za.nc() > 2 && za.b() > -900){
      pmag2.append(1+za.b()*za.b());
      pz.append(za.a());
      dz.append(za.b());
      sigma2_z.append(za.chisq()/(za.nc()-2));
      weight_z.append(exp(-(za.chisq()/(za.nc()-2))));
    }else{
      continue;
    }     

    HepVector a = la.Hpar(pivot);
    
    const float dr_i = a(1);
    const float px_i = - std::sin(a(2));
    const float py_i = std::cos(a(2));

    px.append(px_i);
    py.append(py_i);
    dx.append(dr_i*py_i);
    dy.append(-dr_i*px_i);
    sigma2_r.append(std::sqrt(la.chisq())/(la.nc()-3));
    weight.append(exp(-(std::sqrt(la.chisq())/(la.nc()-3))));
  }
  
  n = dx.length();
  if (n < 2){
    _vx = -9999.;
    _vy = -9999.;
    _vz = -9999.;
    return 0;
  }

  FTList<float> vtx_x(20);
  FTList<float> vtx_y(20);
  FTList<float> vtx_z(20);
  FTList<float> weight2(20);
  FTList<float> weight2_z(20);
  FTList<float> vtx_chi2(20);
  unsigned n_vtx(0);
  for (int i = n - 1; i; i--){
    for (int j = 0; j < i; j++){
      // min. chi2 fit w/ line approximation
      const float pij_x = py[i]*pz[j]-pz[i]*py[j];
      const float pij_y = pz[i]*px[j]-px[i]*pz[j];
      const float pij_z = px[i]*py[j]-py[i]*px[j];

      if (pij_x == 0.0f && pij_y == 0.0f && pij_z == 0.0f ) continue;

      const float sr = sigma2_r[i]+sigma2_r[j]; 
      const float sz = sigma2_z[i]+sigma2_z[j];

      const float ddx = dx[i]-dx[j];
      const float ddy = dy[i]-dy[j];
      const float ddz = dz[i]-dz[j];

      const float pij_mag2 = pij_x*pij_x/(sr*sz)+pij_y*pij_y/(sr*sz)+pij_z*pij_z/(sr*sr);

      const float d_i = (pij_x*(py[j]*ddz-pz[j]*ddy)/(sr*sz)+
			 pij_y*(pz[j]*ddx-px[j]*ddz)/(sr*sz)+
			 pij_z*(px[j]*ddy-py[j]*ddx)/(sr*sr))/pij_mag2;

      const float d_j = (pij_x*(py[i]*ddz-pz[i]*ddy)/(sr*sz)+
			 pij_y*(pz[i]*ddx-px[i]*ddz)/(sr*sz)+
			 pij_z*(px[i]*ddy-py[i]*ddx)/(sr*sr))/pij_mag2;

      const float vtx_x_i = dx[i] + px[i]*d_i;
      const float vtx_y_i = dy[i] + py[i]*d_i;
      const float vtx_z_i = dz[i] + pz[i]*d_i;

      const float vtx_x_j = dx[j] + px[j]*d_j;
      const float vtx_y_j = dy[j] + py[j]*d_j;
      const float vtx_z_j = dz[j] + pz[j]*d_j;

      const float weight_ij = weight[i]+weight[j];
      vtx_x.append((weight[j]*vtx_x_i + weight[i]*vtx_x_j)/weight_ij);
      vtx_y.append((weight[j]*vtx_y_i + weight[i]*vtx_y_j)/weight_ij);
      vtx_z.append((weight_z[j]*vtx_z_i + weight_z[i]*vtx_z_j)/(weight_z[i]+weight_z[j]));
      
      weight2.append(exp(-sr));
      weight2_z.append(exp(-sz));
      vtx_chi2.append(((vtx_x_i-vtx_x_j)*(vtx_x_i-vtx_x_j)+(vtx_y_i-vtx_y_j)*(vtx_y_i-vtx_y_j))/sr +
		      (vtx_z_i-vtx_z_j)*(vtx_z_i-vtx_z_j)/sz);
      n_vtx++;
    }
  }
  n = vtx_chi2.length();
  float S_weight(0.0f);
  float S_weight_z(0.0f);
  for (int i = 0; i != n; i++){
    if (vtx_chi2[i] > 10.) continue;
    float w(std::exp(-vtx_chi2[i]));
    _vx += vtx_x[i]*weight2[i]*w;
    _vy += vtx_y[i]*weight2[i]*w;
    _vz += vtx_z[i]*weight2_z[i]*w;
    S_weight += weight2[i]*w;
    S_weight_z += weight2_z[i]*w;
  }
  int rtn_flag = 0;
  if (S_weight <= 0.){
    _vx = -9999.;
    _vy = -9999.;
  }else{
    _vx /= S_weight;
    _vy /= S_weight;
    rtn_flag = 1;
  }
  if (!z_flag) return rtn_flag;
  if (S_weight_z <= 0.){
    _vz = -9999.;
  }else{
    _vz /= S_weight_z;
    rtn_flag++;
  }
  return rtn_flag;
}

int
FTFinder::findBestVertex(void)
{
  int nTrks = _tracks.length();
  if (nTrks < 2){
    _vx = -9999.;
    _vy = -9999.;
    _vz = -9999.;
    return 0;
  }
  float min_dr = 9999.;
  float phi0 = 9999.;
  for (int i = 0; i < nTrks; i++){
    HepVector a = _tracks[i]->lpav().Hpar(pivot);
    if (fabs(a(1)) < fabs(min_dr)){
      min_dr = a(1);
      phi0 = a(2);
    }
  }
  _vx = min_dr*cos(phi0);
  _vy = min_dr*sin(phi0);
  return 1;
}

Hep3Vector
FTFinder::vertex(void) const{
  return Hep3Vector(_vx,_vy,_vz);
}

int
FTFinder::CorrectEvtTiming(void)
{
  int nTrks = _tracks.length();
  float weight_sum = 0.;
  float dt_sum2 = 0.;
  for (int i = 0; i^nTrks; i++){
    float dt_sum = 0.;
    float dtt_sum = 0.;
    int nHits = 0;
    const Lpav & la = _tracks[i]->lpav();
    FTList<FTSegment *>& axial_sgmnts = _tracks[i]->axial_segments();
    int m = axial_sgmnts.length();
    for (int j = 0; j^m; j++){
      FTList<FTWire *>& hits = axial_sgmnts[j]->wireHits();
      int l = hits.length();
      for (int k = 0; k^l; k++){
        FTWire & h = * hits[k];
        const float x = h.x();
        const float y = h.y();
        double d0 = fabs(la.d((double)x,(double)y));
        if (d0 >= 0.47f*h.layer().csize()) continue;
        nHits++;
        float dt = x2t(h.layer(), d0) - h.time();
	//   float dt = d0/(40*0.0001) - h.time();
        dt_sum += dt;
        dtt_sum += (dt*dt);
      }
    }
    if (!nHits) continue;
    float weight_t = exp(-(dtt_sum - (dt_sum*dt_sum/nHits))/(nHits*1600));
    weight_sum += (nHits*weight_t);
    dt_sum2 += (dt_sum*weight_t);
  }
  //  cout<<"event correction time: "<< int(dt_sum2/weight_sum)<<endl;
  return int(dt_sum2/weight_sum);

}

#ifndef OnlineMode
void
FTFinder::makeMdst(void)
{
  //  static const double pi_mass(0.13956995);
  const int nTrks = _tracks.length();
  int Ntable(0);
  
  for (int i = 0; i<nTrks; i++){

    const FTTrack & trk = * _tracks[i];
    FTList<FTSegment *> &axialSegments = trk.axial_segments(); 
    FTList<FTSegment *> &stereoSegments = trk.stereo_segments(); 
    int axialSegmentsN = axialSegments.length();  
    int stereoSegmentsN = stereoSegments.length();  
    for (int i = 0; i < axialSegmentsN ; i++) {
        //FTSuperLayer& superLayer= axialSegments[i]->superLayer();
        FTList<FTWire *> &wires = axialSegments[i]->wireHits();   
        int wiresN = wires.length();
        for (int j=0; j < wiresN; j++) {
            //int id = wires[j]->localId();
            g_ncell->fill(wires[j]->getWireId());
        }
    }

    for (int i = 0; i < stereoSegmentsN ; i++) {
        //FTSuperLayer& superLayer = stereoSegments[i]->superLayer();
        FTList<FTWire *> &wires = stereoSegments[i]->wireHits();
        int wiresN = wires.length();
        for (int j=0; j < wiresN; j++) {
            //int id = wires[j]->localId();
            g_ncell->fill(wires[j]->getWireId());
        }
    }
      
    const Helix& h = * trk.helix();
    if (h.tanl() < -9000.) continue;

    Ntable++;
    Hep3Vector p(h.momentum());
    HepPoint3D v(h.x());    
    //float m_charge = (h.kappa() > 0) ? 1. : -1.;
    float m_px = p.x();
    g_px[Ntable-1] = p.x();
    float m_py = p.y();
    g_py[Ntable-1] = p.y();
    float m_pz = p.z();
    g_pz[Ntable-1] = p.z();
    g_p[Ntable-1] = p.mag();
    g_phi[Ntable-1] = atan2(m_py, m_px);

    g_dr[Ntable-1] = h.dr();
    g_phi0[Ntable-1] = h.phi0();
    g_kappa[Ntable-1] = h.kappa();
    g_pt[Ntable-1] = 1/fabs(h.kappa());
    g_dz[Ntable-1] = h.dz();
    g_tanl[Ntable-1] = h.tanl();
    g_theta[Ntable-1] = atan2(1/fabs(h.kappa()),(double)(m_pz));
    g_vx[Ntable-1] = v(0);
    g_vy[Ntable-1] = v(1);
    g_vz[Ntable-1] = v(2);
  }  
  g_ntrk = Ntable;

}
#endif



//-------------------------------
// output tracking results to TDS by wangdy
//-------------------------------

StatusCode
FTFinder::makeTds(void) 
{
  IMessageSvc *msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "FTFinder");

  IDataProviderSvc* eventSvc = NULL;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  if (eventSvc) {
     //log << MSG::DEBUG << "makeTds:event Svc has been found" << endreq;
  } else {
     log << MSG::FATAL << "makeTds:Could not find eventSvc" << endreq;
    return StatusCode::FAILURE ;
  }

  RecMdcTrackCol* trkcol = new RecMdcTrackCol;   
  RecMdcHitCol* hitcol = new RecMdcHitCol;

  //make RecMdcTrackCol
#ifndef OnlineMode
  log << MSG::DEBUG << "beginning to make RecMdcTrackCol" <<endreq; 
#endif
  int ntrk = tracks().length(); 
  int trkid=0;   
  for (int i =0; i < ntrk; i++) {
    RecMdcTrack* trk = new RecMdcTrack;
    FTTrack* fttrk = tracks()[i];

#ifndef OnlineMode    
    //    fttrk->printout();
#endif

    if (fttrk->helix()->tanl() < -9000.){
      log << MSG::DEBUG << "deleted trackId = " << i+1 << " due to tanl = " << fttrk->helix()->tanl() << endreq;   
      delete trk;	
      continue;
    }
    //    int trackindex = i;
    HepPoint3D pos = fttrk->helix()->pivot();
    int charge = -1;
    HepVector m_a(5,0);
    m_a = fttrk->helix()->a();
    m_a[2]=-m_a[2];
    HepSymMatrix m_ea = fttrk->helix()->Ea();
    float fiterm = fttrk->lpav().phi(77.0);
    float chi2lpav =  fttrk->lpav().chisq();
    float chi2zav = fttrk->Zav().chisq();
     
// note: this alg itself can not provide error matrix,so m_ea above is empty;
//       the following error matrix values are based on BESII results      
//       the values of online Bhabha events are multiplied by factor 10 
//                          wangdy 20050418
     m_ea[0][0] = 0.0085;
     m_ea[1][1] = 0.000011;
     m_ea[2][2] = 0.0018;
     m_ea[3][3] = 1.2;
     m_ea[4][4] = 0.00026;
     m_ea[1][0] = m_ea[0][1] = -0.00029;
     m_ea[2][0] = m_ea[0][2] = charge*0.004;
     m_ea[2][1] = m_ea[1][2] = charge*0.00012;
     m_ea[3][0] = m_ea[0][3] = -0.017;
     m_ea[3][1] = m_ea[1][3] = 0.0;
     m_ea[3][2] = m_ea[2][3] = 0.0;
     m_ea[4][0] = m_ea[0][4] = 0.0;
     m_ea[4][1] = m_ea[1][4] = 0.0;
     m_ea[4][2] = m_ea[2][4] = 0.0;
     m_ea[4][3] = m_ea[3][4] = -0.018;
     
    trk->setTrackId(trkid);
    trk->setHelix(m_a);
    trk->setPivot(pos);
    trk->setError(m_ea);
    trk->setFiTerm(fiterm);
    trk->setChi2(chi2lpav+chi2zav);
#ifndef OnlineMode
    log <<MSG::DEBUG << " trackId = " << i  << endreq;   
    log <<MSG::DEBUG <<"fast-tracking  kappa "<<m_a[2]
	<<"   fast-tracking tanl "<<m_a[4]
        <<endreq;
    log <<MSG::DEBUG <<"push_backed kappa "<<trk->helix(2)
	<<"   push_backed tanl "<< trk->helix(4)
        <<endreq;

    log << MSG::DEBUG << "beginning to make hitlist and RecMdcHitCol " <<endreq;
#endif

    HitRefVec  hitrefvec;

    int hitindex = 0;    

    //axial segments part
    FTList<FTSegment *>&  seglist_ax = fttrk->axial_segments();
    int nseg_ax = seglist_ax.length();
    int ntrackhits = 0;
    for (int iseg_ax = 0; iseg_ax < nseg_ax; iseg_ax++) {
      FTList<FTWire *>& hitlist_ax = seglist_ax[iseg_ax]->wireHits();
      int nhitlist_ax = hitlist_ax.length();
      ntrackhits += nhitlist_ax;
      for( int ihit_ax = 0; ihit_ax < nhitlist_ax; ihit_ax++,hitindex++) {
	FTWire wire_ax = *(hitlist_ax[ihit_ax]);
        double  dd = wire_ax.distance();
        double  adc = RawDataUtil::MdcChargeChannel(wire_ax.getAdc());
	double  tdc = wire_ax.time();
	double  chi2 = wire_ax.getChi2();
	const Identifier mdcid = MdcID::wire_id(wire_ax.layer().layerId(),wire_ax.localId());

        RecMdcHit*  hit = new RecMdcHit;
	hit->setId(hitindex);
        hit->setDriftDistLeft( dd );
        hit->setDriftDistRight( dd );
        hit->setAdc( adc );
	hit->setTdc( tdc );
	hit->setMdcId( mdcid );
	hit->setChisqAdd( chi2 );
#ifndef OnlineMode
        log << MSG::DEBUG << "Hit DriftDistLeft " << hit->getDriftDistLeft() << endreq;     
        log << MSG::DEBUG << "MDC Hit ADC " << hit->getAdc() << endreq;
        log << MSG::DEBUG << "Hit MDC  Identifier " << hit->getMdcId() << endreq;     
        log << MSG::DEBUG << "Hit Chisq " << hit->getChisqAdd() << endreq;     
#endif
        hitcol->push_back(hit);
        SmartRef<RecMdcHit> refhit(hit);
        hitrefvec.push_back(refhit);
      }
    }

    //stereo segments part
    FTList<FTSegment *>&  seglist_st = fttrk->stereo_segments();
    int nseg_st = seglist_st.length();
    int ntrackster = 0;
    
    for (int iseg_st = 0; iseg_st < nseg_st; iseg_st++) {
      FTList<FTWire *>& hitlist_st = seglist_st[iseg_st]->wireHits();
      int nhitlist_st = hitlist_st.length();
      ntrackhits += nhitlist_st;
      ntrackster += nhitlist_st;
      for( int ihit_st = 0; ihit_st < nhitlist_st; ihit_st++,hitindex++) {
	FTWire wire_st = *(hitlist_st[ihit_st]);
        double  dd = wire_st.distance();
        //double  adc = wire_st.getAdc();
	double  adc = RawDataUtil::MdcChargeChannel(wire_st.getAdc());
        double  tdc = wire_st.time();	 
	double  chi2 = wire_st.getChi2();
	const Identifier mdcid = MdcID::wire_id(wire_st.layer().layerId(),wire_st.localId());

        RecMdcHit*  hit = new RecMdcHit;
	hit->setId(hitindex);
        hit->setDriftDistLeft( dd );
        hit->setDriftDistRight( dd );
        hit->setAdc( adc );
	hit->setTdc( tdc); 
        hit->setMdcId( mdcid );
	hit->setChisqAdd( chi2 );
#ifndef OnlineMode
        log << MSG::DEBUG << "Z Hit DriftDistLeft " << hit->getDriftDistLeft() << endreq;     
        log << MSG::DEBUG << "Z MDC Hit ADC " << hit->getAdc() << endreq;
        log << MSG::DEBUG << "Z Hit MDC  Identifier " << hit->getMdcId() << endreq;     
        log << MSG::DEBUG << "Z Hit Chisq " << hit->getChisqAdd() << endreq;     
#endif
        hitcol->push_back(hit);
	SmartRef<RecMdcHit> refhit(hit);
        hitrefvec.push_back(refhit);
      } 
    }
    trk->setNhits(ntrackhits);
    trk->setNster(ntrackster);
    trk->setVecHits(hitrefvec);   
    trkcol->push_back(trk);
    trkid++;
#ifndef OnlineMode
    g_naxialhit->fill((float)(ntrackhits-ntrackster), 1.0);
    g_nstereohit->fill((float)ntrackster, 1.0);
    g_nhit->fill((float)ntrackhits, 1.0);
#endif
  } 

  IDataManagerSvc *dataManSvc = dynamic_cast<IDataManagerSvc*> (eventSvc); 
  DataObject *aTrackCol;
  eventSvc->findObject("/Event/Recon/RecMdcTrackCol",aTrackCol);
  if(aTrackCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcTrackCol");
    eventSvc->unregisterObject("/Event/Recon/RecMdcTrackCol");
  }
  DataObject *aRecHitCol;
  eventSvc->findObject("/Event/Recon/RecMdcHitCol",aRecHitCol);
  if(aRecHitCol != NULL) {
    dataManSvc->clearSubTree("/Event/Recon/RecMdcHitCol");
    eventSvc->unregisterObject("/Event/Recon/RecMdcHitCol");
  }

  //register RecMdcHitCol
  StatusCode hitsc;
  hitsc = eventSvc->registerObject("/Event/Recon/RecMdcHitCol", hitcol);
  if( hitsc.isFailure() ) {
    log << MSG::FATAL << "Could not register RecMdcHitCol" << endreq;
    return hitsc;
  }
#ifndef OnlineMode
  log << MSG::DEBUG << "RecMdcHitCol registered successfully!" <<endreq;

  RecMdcTrackCol::iterator bef = trkcol->begin();
  for( ; bef != trkcol->end(); bef++) {
    log <<MSG::DEBUG <<" registered kappa"<<(*bef)->helix(2)
	<<"   registered tanl"<< (*bef)->helix(4)
        <<endreq;    
  }
#endif

  //register RecMdcTrackCol
  StatusCode trksc;
  trksc = eventSvc->registerObject("/Event/Recon/RecMdcTrackCol", trkcol);
  if( trksc.isFailure() ) {
    log << MSG::FATAL << "Could not register RecMdcHit" << endreq;
    return trksc; 
  }
#ifndef OnlineMode
  log << MSG::DEBUG << "RecMdcTrackCol registered successfully!" <<endreq;
  
  //check the result:RecMdcHitCol   
  SmartDataPtr<RecMdcHitCol> newhitCol(eventSvc,"/Event/Recon/RecMdcHitCol");
  if (!newhitCol) {
    log << MSG::FATAL << "Could not find RecMdcHit" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::DEBUG << "Begin to check RecMdcHitCol"<<endreq;
  RecMdcHitCol::iterator iter_hit = newhitCol->begin();
  for( ; iter_hit != newhitCol->end(); iter_hit++){ 
    log << MSG::DEBUG << "retrieved MDC Hit:"
        << "   DDL " <<(*iter_hit)->getDriftDistLeft()
        << "   DDR " <<(*iter_hit)->getDriftDistRight()
        << "   ADC " <<(*iter_hit)->getAdc() << endreq;
  }
  
  //check the result:RecMdcTrackCol   
  SmartDataPtr<RecMdcTrackCol> newtrkCol(eventSvc,"/Event/Recon/RecMdcTrackCol");
  if (!newtrkCol) { 
    log << MSG::FATAL << "Could not find RecMdcTrackCol" << endreq;
    return( StatusCode::FAILURE);
  }
  log << MSG::DEBUG << "Begin to check RecMdcTrackCol"<<endreq; 
  RecMdcTrackCol::iterator iter_trk = newtrkCol->begin();
   for( ; iter_trk != newtrkCol->end(); iter_trk++){
     log << MSG::DEBUG << "retrieved MDC track:"
	 << "Track Id: " << (*iter_trk)->trackId()
	 << "   Pivot: " << (*iter_trk)->poca()[0] << " "
	 << (*iter_trk)->poca()[1] << " " << (*iter_trk)->poca()[2] 
	 << endreq ;
     
     /*<< "Phi0: " << (*iter_trk)->getFi0() << "   Error of Phi0 "
       << (*iter_trk)->getError()(2,2) << endreq
       /<< "kappa: " << (*iter_trk)->getCpa() << "   Error of kappa "
       << (*iter_trk)->getError()(3,3) << endreq
	  << "Tanl: " << (*iter_trk)->getTanl() << "   Error of Tanl "
	  << (*iter_trk)->getError()(5,5) << endreq      
	  << "Chisq of fit: "<< (*iter_trk)->getChisq()
	  << "   Phi terminal: "<< (*iter_trk)->getFiTerm()
	  << endreq
	  << "Number of hits: "<< (*iter_trk)->getNhits()
	  << "   Number of stereo hits " << (*iter_trk)->getNster()
	  << endreq;*/
     
     log << MSG::DEBUG << "hitList of this  track:" << endreq;
     HitRefVec gothits = (*iter_trk)->getVecHits();
     HitRefVec::iterator it_gothit = gothits.begin();
     for( ; it_gothit != gothits.end(); it_gothit++){
       log << MSG::DEBUG << "hits Id: "<<(*it_gothit)->getId()
	   << "   hits DDL&DDR " <<(*it_gothit)->getDriftDistLeft()
	   << "   hits MDC IDentifier " <<(*it_gothit)->getMdcId()
	   << endreq	 
	   << "   hits TDC " <<(*it_gothit)->getTdc()  
	   << "   hits ADC " <<(*it_gothit)->getAdc() << endreq;
     } 
     
   }
#endif
   
   return StatusCode::SUCCESS;
}
  
