
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/StatusCode.h"
 
#include "GaudiKernel/SmartDataPtr.h"
#include "ReconEvent/ReconEvent.h"
#include "EmcRec/EmcRecTDS.h"

using namespace std;

StatusCode EmcRecTDS::RegisterToTDS(RecEmcHitMap& aHitMap,
    RecEmcClusterMap& aClusterMap, RecEmcShowerMap& aShowerMap)
{
  RegisterCluster(aClusterMap);
  RegisterShower(aShowerMap);
  RegisterHit(aHitMap);
  return StatusCode::SUCCESS;
}

StatusCode EmcRecTDS::RegisterReconEvent()
{
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "EmcRecTDS");

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
       
  //check whether the Recon has been already registered
  DataObject *aReconEvent;
  eventSvc->findObject("/Event/Recon",aReconEvent);
  if(aReconEvent==NULL) {
    log << MSG::INFO << "EmcRecTDS::RegisterReconEvent()" << endreq;
    //then register Recon
    aReconEvent = new ReconEvent();
    StatusCode sc = eventSvc->registerObject("/Event/Recon",aReconEvent);
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not register ReconEvent" <<endreq;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode EmcRecTDS::RegisterHit(RecEmcHitMap& aHitMap)
{
  RegisterReconEvent();
    
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "EmcRecTDS");
  log << MSG::INFO << "EmcRecTDS::RegisterHit()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  
  RecEmcHitCol *aRecEmcHitCol = new RecEmcHitCol;
  RecEmcHitMap::iterator iHitMap;
  for(iHitMap=aHitMap.begin();
      iHitMap!=aHitMap.end();
      iHitMap++){
    aRecEmcHitCol->add(new RecEmcHit(iHitMap->second));
  }

  //check whether the RecEmcHitCol has been already registered
  StatusCode sc;
  DataObject *aRecEmcHitEvent;
  eventSvc->findObject("/Event/Recon/RecEmcHitCol", aRecEmcHitEvent);
  if(aRecEmcHitEvent!=NULL) {
    //then unregister RecEmcHitCol
    sc = eventSvc->unregisterObject("/Event/Recon/RecEmcHitCol");
    delete aRecEmcHitEvent;   //it must be delete to avoid memory leakage
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister EMC shower collection" << endreq;
      return( StatusCode::FAILURE);
    }
  }

  sc = eventSvc->registerObject("/Event/Recon/RecEmcHitCol", aRecEmcHitCol);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EMC hit collection" << endreq;
    return( StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode EmcRecTDS::RegisterCluster(RecEmcClusterMap& aClusterMap)
{
  RegisterReconEvent();

  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "EmcRecTDS");
  log << MSG::INFO << "EmcRecTDS::RegisterCluster()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  RecEmcClusterCol *aRecEmcClusterCol = new RecEmcClusterCol;
  RecEmcClusterMap::iterator iClusterMap;
  for(iClusterMap=aClusterMap.begin();
      iClusterMap!=aClusterMap.end();
      iClusterMap++){
    aRecEmcClusterCol->add(new RecEmcCluster(iClusterMap->second));
  }

  //check whether the RecEmcClusterCol has been already registered
  StatusCode sc;
  DataObject *aRecEmcClusterEvent;
  eventSvc->findObject("/Event/Recon/RecEmcClusterCol", aRecEmcClusterEvent);
  if(aRecEmcClusterEvent!=NULL) {
    //then unregister RecEmcClusterCol
    sc = eventSvc->unregisterObject("/Event/Recon/RecEmcClusterCol");
    delete aRecEmcClusterEvent;   //it must be delete to avoid memory leakage
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister EMC cluster collection" << endreq;
      return( StatusCode::FAILURE);
    }
  }

  sc = eventSvc->registerObject("/Event/Recon/RecEmcClusterCol", aRecEmcClusterCol);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EMC cluster collection" << endreq;
    return( StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode EmcRecTDS::RegisterShower(RecEmcShowerMap& aShowerMap)
{
  RegisterReconEvent();

  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "EmcRecTDS");
  log << MSG::INFO << "EmcRecTDS::RegisterShower()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  SmartDataPtr<RecEmcClusterCol> aClusterCol(eventSvc,"/Event/Recon/RecEmcClusterCol");
  if (!aClusterCol) { 
    log << MSG::INFO << "Could not find emcRecClusterCol" << endreq;
  }

  // ShowerMap --> Shower Container
  RecEmcShowerCol *aRecEmcShowerCol = new RecEmcShowerCol;
  RecEmcShowerVec aShowerVec;

  RecEmcShowerMap::iterator iShowerMap;
  for(iShowerMap=aShowerMap.begin();
      iShowerMap!=aShowerMap.end();
      iShowerMap++) {
    //Find cluster in TDS
    if(aClusterCol) {
      RecEmcID clusterId(iShowerMap->second.getClusterId());
      RecEmcClusterCol::iterator iClusterCol;
      for(iClusterCol=aClusterCol->begin();
          iClusterCol!=aClusterCol->end();
          iClusterCol++){
        if(clusterId==(*iClusterCol)->getClusterId()) {
          iShowerMap->second.Cluster(*iClusterCol);
          break;
          //emcRecClusterCol->InsertShower(*iCluster);
        }
      }
    }
    aShowerVec.push_back(iShowerMap->second);
  }
  sort(aShowerVec.begin(), aShowerVec.end(), greater<RecEmcShower>());
  RecEmcShowerVec::iterator iShowerVec;
  for(iShowerVec=aShowerVec.begin();
      iShowerVec!=aShowerVec.end();
      iShowerVec++) {
    aRecEmcShowerCol->add(new RecEmcShower(*iShowerVec));
  }  

  //check whether the RecEmcShowerCol has been already registered
  StatusCode sc;
  DataObject *aRecEmcShowerEvent;
  eventSvc->findObject("/Event/Recon/RecEmcShowerCol", aRecEmcShowerEvent);
  if(aRecEmcShowerEvent!=NULL) {
    //then unregister RecEmcShowerCol
    StatusCode sc = eventSvc->unregisterObject("/Event/Recon/RecEmcShowerCol");
    delete aRecEmcShowerEvent;   //it must be delete to avoid memory leakage
    if(sc!=StatusCode::SUCCESS) {
      log << MSG::FATAL << "Could not unregister EMC shower collection" << endreq;
      return( StatusCode::FAILURE);
    }
  }

  sc = eventSvc->registerObject("/Event/Recon/RecEmcShowerCol", aRecEmcShowerCol);
  if(sc!=StatusCode::SUCCESS) {
    log << MSG::FATAL << "Could not register EMC shower collection" << endreq;
    return( StatusCode::FAILURE);
  }

  return StatusCode::SUCCESS;
}

StatusCode EmcRecTDS::CheckRegister()
{
  IMessageSvc* msgSvc;
  Gaudi::svcLocator()->service("MessageSvc", msgSvc);
  MsgStream log(msgSvc, "EmcRecTDS");
  log << MSG::INFO << "EmcRecTDS::CheckRegister()" << endreq;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  // check RecEmcHitCol registered
  SmartDataPtr<RecEmcHitCol> aHitCol(eventSvc,"/Event/Recon/RecEmcHitCol");
  if (!aHitCol) { 
    log << MSG::FATAL << "Could not find emcRecHitCol" << endreq;
    return( StatusCode::FAILURE);
  }

  RecEmcHitCol::iterator iHitCol;
  for(iHitCol=aHitCol->begin();
      iHitCol!=aHitCol->end();
      iHitCol++){
    //cout<<*(*iHitCol);
  }

  // check RecEmcClusterCol registered
  SmartDataPtr<RecEmcClusterCol> aClusterCol(eventSvc,"/Event/Recon/RecEmcClusterCol");
  if (!aClusterCol) { 
    log << MSG::FATAL << "Could not find emcRecClusterCol" << endreq;
    return( StatusCode::FAILURE);
  }

  RecEmcClusterCol::iterator iClusterCol;
  for(iClusterCol=aClusterCol->begin();
      iClusterCol!=aClusterCol->end();
      iClusterCol++){
    //cout<<*(*iClusterCol);
  }

  // check RecEmcShowerCol registered
  SmartDataPtr<RecEmcShowerCol> aShowerCol(eventSvc,"/Event/Recon/RecEmcShowerCol");
  if (!aShowerCol) { 
    log << MSG::FATAL << "Could not find emcRecShowerCol" << endreq;
    return( StatusCode::FAILURE);
  }

  RecEmcShowerCol::iterator iShowerCol;
  for(iShowerCol=aShowerCol->begin();
      iShowerCol!=aShowerCol->end();
      iShowerCol++){
    //cout<<*(*iShowerCol);
  }

  return StatusCode::SUCCESS;
}
