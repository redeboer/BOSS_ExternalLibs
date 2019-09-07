#ifndef RecEmcClusterCnv_CXX
#define RecEmcClusterCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "EmcRecEventModel/RecEmcCluster.h"

#include "RootEventData/TRecEmcCluster.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecEmcClusterCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecEmcClusterCnv> s_factory;
//const ICnvFactory& RecEmcClusterCnvFactory = s_factory;

RecEmcClusterCnv::RecEmcClusterCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  //cout<<"in RecEmcClusterCnv::constructor: clusterid= "<<aCluster->clID()<<endl;

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecEmcClusterCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recEmcClusterCol";
    //declareObject(EventModel::Recon::RecEmcClusterCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recEmcClusterCol);
    m_recEmcClusterCol=0;
}

StatusCode RecEmcClusterCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "RecEmcClusterCnv");
  log << MSG::DEBUG << "RecEmcClusterCnv::TObjectToDataObject" << endreq;
  //cout << "RecEmcClusterCnv::TObjectToDataObject" << endl;
  StatusCode sc=StatusCode::SUCCESS;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);

  SmartDataPtr<RecEmcHitCol> emcRecHitCol(eventSvc,
      EventModel::Recon::RecEmcHitCol);
  if(!emcRecHitCol) log << MSG::INFO << "can't retrieve RecEmcHitCol" << endreq;
  
  // create the TDS location for the RecEmcCluster Collection
  RecEmcClusterCol* emcClusterTdsCol = new RecEmcClusterCol;
  refpObject=emcClusterTdsCol;

  // now convert
  if (!m_recEmcClusterCol) return sc;
  TIter emcClusterIter(m_recEmcClusterCol);
  TRecEmcCluster *emcClusterRoot = 0;
  while ((emcClusterRoot = (TRecEmcCluster*)emcClusterIter.Next())) {
    RecEmcID clusterId(emcClusterRoot->clusterId());

    RecEmcCluster *emcClusterTds = new RecEmcCluster();
    m_common.m_rootRecEmcClusterMap[emcClusterRoot] = emcClusterTds;

    emcClusterTds->ClusterId(clusterId);

    vector<int> vecShowers = emcClusterRoot->vecShowers();
    vector<int>::iterator iVecShower;
    vector<RecEmcID> vecShowerId;
    for(iVecShower=vecShowers.begin();
        iVecShower!=vecShowers.end();
        iVecShower++) {
      RecEmcID id(*iVecShower);
      vecShowerId.push_back(id);
    }
    emcClusterTds->ShowerIdVec(vecShowerId);
    
    if(emcRecHitCol) {
      vector<Int_t> vecHits = emcClusterRoot->vecHits();
      vector<Int_t> vecSeeds = emcClusterRoot->vecSeeds();
      vector<Int_t>::iterator iVecHit;
      
      //put hit map into cluster
      for(iVecHit=vecHits.begin();
          iVecHit!=vecHits.end();
          iVecHit++) {
        
        RecEmcID id(*iVecHit);
        
        RecEmcHitCol::iterator iHit;
        for(iHit=emcRecHitCol->begin();
            iHit!=emcRecHitCol->end();
            iHit++) {
          
          RecEmcID idHit((*iHit)->getCellId());

          if(id==idHit) {
            //RecEmcFraction frac(*(*iHit));
            //frac.Fraction(iHit->second);
            emcClusterTds->Insert(*(*iHit));
            break;
          }
        } //RecEmcHitCol
      } //VecHit

      //put seed map into cluster
      for(iVecHit=vecSeeds.begin();
          iVecHit!=vecSeeds.end();
          iVecHit++) {
        
        RecEmcID id(*iVecHit);
        
        RecEmcHitCol::iterator iHit;
        for(iHit=emcRecHitCol->begin();
            iHit!=emcRecHitCol->end();
            iHit++) {
          
          RecEmcID idHit((*iHit)->getCellId());

          if(id==idHit) {
            //RecEmcFraction frac(*(*iHit));
            //frac.Fraction(iHit->second);
            emcClusterTds->InsertSeed(*(*iHit));
            break;
          }
        } //RecEmcHitCol
      } //VecHit

    }
    
    emcClusterTdsCol->push_back(emcClusterTds);
  }


    return StatusCode::SUCCESS;
}

StatusCode RecEmcClusterCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "RecEmcClusterCnv");
  log << MSG::DEBUG << "RecEmcClusterCnv::DataObjectToTObject" << endreq;
  //cout<<"RecEmcClusterCnv::DataObjectToTObject"<<endl;
  StatusCode sc=StatusCode::SUCCESS;
 
  RecEmcClusterCol * emcClusterColTds=dynamic_cast<RecEmcClusterCol *> (obj);
  if (!emcClusterColTds) {
    log << MSG::ERROR << "Could not downcast to RecEmcClusterCol" << endreq;
    return StatusCode::FAILURE;
  }

  DataObject *evt;
  m_eds->findObject(EventModel::Recon::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get RecEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "RecEmcClusterCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_emcClusterCol = recEvt->getEmcClusterCol();
  if (!m_emcClusterCol) return sc;

  //necessary in case there is I/O at the same time since array is static
  recEvt->clearEmcClusterCol(); 

  RecEmcClusterCol::const_iterator emcClusterTds;

  for(emcClusterTds = emcClusterColTds->begin();
      emcClusterTds != emcClusterColTds->end();
      emcClusterTds++) {
    Int_t clusterId = (*emcClusterTds)->getClusterId();
    
    vector<Int_t> vecHits;
    RecEmcHitMap::const_iterator iHitMap;
    for(iHitMap=(*emcClusterTds)->Begin();
        iHitMap!=(*emcClusterTds)->End();
        iHitMap++) {
      vecHits.push_back(iHitMap->first);
    }

    vector<Int_t> vecSeeds;
    RecEmcHitMap::const_iterator iSeedMap;
    for(iSeedMap=(*emcClusterTds)->BeginSeed();
        iSeedMap!=(*emcClusterTds)->EndSeed();
        iSeedMap++) {
      vecSeeds.push_back(iSeedMap->first);
    }

    vector<RecEmcID> vecShowerId=(*emcClusterTds)->getShowerIdVec();
    vector<RecEmcID>::iterator iShowerId;
    vector<Int_t> vecShowers;
    for(iShowerId=vecShowerId.begin();
        iShowerId!=vecShowerId.end();
        iShowerId++) {
       vecShowers.push_back(*iShowerId);
    }

    //cout<<"clusterId="<<RecEmcID(clusterId)<<endl;

    TRecEmcCluster *emcClusterRoot = new TRecEmcCluster();
    //m_common.m_recEmcClusterMap[(*emcClusterTds)] = emcClusterRoot;

    emcClusterRoot->setClusterId(clusterId);
    emcClusterRoot->setVecHits(vecHits);
    emcClusterRoot->setVecSeeds(vecSeeds);
    emcClusterRoot->setVecShowers(vecShowers);

    recEvt->addEmcCluster(emcClusterRoot);
  }
  
  return StatusCode::SUCCESS;
}
#endif
