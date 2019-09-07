#ifndef RecEmcShowerCnv_CXX
#define RecEmcShowerCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/Bootstrap.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "EmcRecEventModel/RecEmcEventModel.h"

#include "RootEventData/TRecEmcShower.h"
#include "RootEventData/TRecTrackEvent.h"


#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecEmcShowerCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecEmcShowerCnv> s_factory;
//const ICnvFactory& RecEmcShowerCnvFactory = s_factory;

RecEmcShowerCnv::RecEmcShowerCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

  RecEmcShower *aShower = new RecEmcShower();
  //cout<<"in RecEmcShowerCnv::constructor:: showid= "<<aShower->clID()<<endl;
  delete aShower;

  // Here we associate this converter with the /Event path on the TDS.
  MsgStream log(msgSvc(), "RecEmcShowerCnv");
  //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
  //m_rootTreename ="Rec";
  m_rootBranchname ="m_recEmcShowerCol";
  //declareObject(EventModel::Recon::RecEmcShowerCol, objType(), m_rootTreename, m_rootBranchname);
  m_adresses.push_back(&m_recEmcShowerCol);
  m_recEmcShowerCol=0;
}

StatusCode RecEmcShowerCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

  MsgStream log(msgSvc(), "RecEmcShowerCnv");
  log << MSG::DEBUG << "RecEmcShowerCnv::TObjectToDataObject" << endreq;
  //cout << "RecEmcShowerCnv::TObjectToDataObject" << endl;
  StatusCode sc=StatusCode::SUCCESS;

  IDataProviderSvc* eventSvc;
  Gaudi::svcLocator()->service("EventDataSvc", eventSvc);
  
  SmartDataPtr<RecEmcHitCol> emcRecHitCol(eventSvc,
      EventModel::Recon::RecEmcHitCol);
  if(!emcRecHitCol) log << MSG::INFO << "can't retrieve RecEmcHitCol" << endreq;
  
  SmartDataPtr<RecEmcClusterCol> emcRecClusterCol(eventSvc,
      EventModel::Recon::RecEmcClusterCol);
  if(!emcRecClusterCol) log << MSG::INFO << "can't retrieve RecEmcClusterCol" << endreq;
  
  // create the TDS location for the RecEmcShower Collection
  RecEmcShowerCol* emcShowerTdsCol = new RecEmcShowerCol;
  refpObject=emcShowerTdsCol;

  // now convert
  if (!m_recEmcShowerCol) return sc;
  TIter emcShowerIter(m_recEmcShowerCol);
  TRecEmcShower *emcShowerRoot = 0;
  while ((emcShowerRoot = (TRecEmcShower*)emcShowerIter.Next())) {
    int       trackId = emcShowerRoot->trackId();
    int       numHits = emcShowerRoot->numHits(); 
    int       status =	emcShowerRoot->status(); 
    int       cellId =	emcShowerRoot->cellId(); 
    int       module =	emcShowerRoot->module(); 
    double    x =	emcShowerRoot->x();      
    double    y =	emcShowerRoot->y();      
    double    z =	emcShowerRoot->z();      
    double    theta = 	emcShowerRoot->theta();  
    double    dtheta =	emcShowerRoot->dtheta(); 
    double    phi    =	emcShowerRoot->phi();    
    double    dphi   =	emcShowerRoot->dphi();   
    double    energy =  emcShowerRoot->energy();  
    double    dE    = 	emcShowerRoot->dE();     
    double  eSeed =  emcShowerRoot->eSeed();
    double  e3x3 =  emcShowerRoot->e3x3();
    double  e5x5 =  emcShowerRoot->e5x5();
    double  eall =  emcShowerRoot->eAll();
    double  elepton =  emcShowerRoot->eLepton();
    double  time = emcShowerRoot->time();
    double  secondMoment = emcShowerRoot->secondMoment();
    double  latMoment = emcShowerRoot->latMoment();
    double  a20Moment = emcShowerRoot->a20Moment();
    double  a42Moment = emcShowerRoot->a42Moment();

    HepSymMatrix matrix(3);
    matrix[0][0]=emcShowerRoot->err(0);
    matrix[1][1]=emcShowerRoot->err(1);
    matrix[2][2]=emcShowerRoot->err(2);
    matrix[0][1]=emcShowerRoot->err(3);
    matrix[0][2]=emcShowerRoot->err(4);
    matrix[1][2]=emcShowerRoot->err(5);

    RecEmcID showerId(cellId);
    RecEmcID clusterId(emcShowerRoot->clusterId());
    
    RecEmcIDVector id3x3;
    vector<Int_t> cellId3x3 = emcShowerRoot->cellId3x3();
    vector<Int_t>::iterator iCellId;
    //cout<<"id3x3"<<endl;
    for(iCellId=cellId3x3.begin();
        iCellId!=cellId3x3.end();
        iCellId++) {
      RecEmcID id(*iCellId);
      //cout<<"id="<<id<<endl;
      id3x3.push_back(id);
    }

    RecEmcIDVector id5x5;
    vector<Int_t> cellId5x5 = emcShowerRoot->cellId5x5();
    //cout<<"id5x5"<<endl;
    for(iCellId=cellId5x5.begin();
        iCellId!=cellId5x5.end();
        iCellId++) {
      RecEmcID id(*iCellId);
      //cout<<"id="<<id<<endl;
      id5x5.push_back(id);
    }

    RecEmcShower *emcShowerTds = new RecEmcShower();
    m_common.m_rootRecEmcShowerMap[emcShowerRoot] = emcShowerTds;

    emcShowerTds->setTrackId(trackId );
    emcShowerTds->setNumHits(numHits );    
    emcShowerTds->setStatus(status );     
    emcShowerTds->setCellId( cellId );     
    emcShowerTds->setModule( module);     

    HepPoint3D pos(x,y,z);
    emcShowerTds->setPosition(pos);
    emcShowerTds->setEnergy( energy  );    
    emcShowerTds->setDE( dE  );     
    emcShowerTds->setDtheta( dtheta     );  
    emcShowerTds->setDphi( dphi     );  
    emcShowerTds->setESeed(eSeed);
    emcShowerTds->setE3x3(e3x3);
    emcShowerTds->setE5x5(e5x5);
    emcShowerTds->EAll(eall);
    emcShowerTds->ELepton(elepton);
    emcShowerTds->setTime(time);
    emcShowerTds->setSecondMoment(secondMoment); 
    emcShowerTds->setLatMoment(latMoment); 
    emcShowerTds->setA20Moment(a20Moment); 
    emcShowerTds->setA42Moment(a42Moment); 
    emcShowerTds->setErrorMatrix( matrix );   

    emcShowerTds->ShowerId(showerId);     
    emcShowerTds->ClusterId(clusterId);     
    emcShowerTds->CellId3x3(id3x3);
    emcShowerTds->CellId5x5(id5x5);
    
    //put fraction map into shower
    if(emcRecHitCol) {
      map<Int_t, Double_t> cellIdMap=emcShowerRoot->cellIdMap();
      map<Int_t, Double_t>::iterator iCellIdMap;
      for(iCellIdMap=cellIdMap.begin();
          iCellIdMap!=cellIdMap.end();
          iCellIdMap++) {
        
        RecEmcID id(iCellIdMap->first);
        
        RecEmcHitCol::iterator iHit;
        for(iHit=emcRecHitCol->begin();
            iHit!=emcRecHitCol->end();
            iHit++) {
          
          RecEmcID idHit((*iHit)->getCellId());

          if(id==idHit) {
            RecEmcFraction frac(*(*iHit));
            frac.Fraction(iCellIdMap->second);
            emcShowerTds->Insert(frac);
            break;
          }
        } //RecEmcHitCol
      } //CellIdMap
    }

    if(emcRecClusterCol) {
      RecEmcClusterCol::iterator iCluster;
      for(iCluster=emcRecClusterCol->begin();
          iCluster!=emcRecClusterCol->end();
          iCluster++) {
        if(clusterId==(*iCluster)->getClusterId()) {
          emcShowerTds->Cluster(*iCluster);
          break;
          //emcRecClusterCol->InsertShower(*iCluster);
        }
      }
    }

    emcShowerTdsCol->push_back(emcShowerTds);
    //	   delete emcShowerTds;
    // emcShowerTds = NULL;
  }

  //m_recEmcShowerCol->Delete();  // wensp add 2005/12/30
  delete m_recEmcShowerCol;
  m_recEmcShowerCol = 0;  

  return StatusCode::SUCCESS;
}

StatusCode RecEmcShowerCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) 
{
  MsgStream log(msgSvc(), "RecEmcShowerCnv");
  log << MSG::DEBUG << "RecEmcShowerCnv::DataObjectToTObject" << endreq;
  //cout << "RecEmcShowerCnv::DataObjectToTObject" << endl;
  StatusCode sc=StatusCode::SUCCESS;

  RecEmcShowerCol * emcShowerColTds=dynamic_cast<RecEmcShowerCol *> (obj);
  if (!emcShowerColTds) {
    log << MSG::ERROR << "Could not downcast to RecEmcShowerCol" << endreq;
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
    log << MSG::ERROR << "RecEmcShowerCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_emcShowerCol = recEvt->getEmcShowerCol();
  if (!m_emcShowerCol) return sc;
  recEvt->clearEmcShowerCol(); //necessary in case there is I/O at the same time since array is static
  RecEmcShowerCol::const_iterator emcShowerTds;

  for (emcShowerTds = emcShowerColTds->begin(); emcShowerTds != emcShowerColTds->end(); emcShowerTds++) {
    Int_t       trackId = (*emcShowerTds)->trackId();
    Int_t       numHits = (*emcShowerTds)->numHits(); 
    Int_t       status =(*emcShowerTds)->status(); 
    Int_t       cellId =(*emcShowerTds)->getShowerId(); 
    Int_t       clusterId =	(*emcShowerTds)->getClusterId(); 
    Int_t       module =(*emcShowerTds)->module(); 
    Double_t    x =	(*emcShowerTds)->x();      
    Double_t    y =	(*emcShowerTds)->y();      
    Double_t    z =	(*emcShowerTds)->z();      
    Double_t    theta = (*emcShowerTds)->theta();  
    Double_t    dtheta =(*emcShowerTds)->dtheta(); 
    Double_t    phi    =(*emcShowerTds)->phi();    
    Double_t    dphi   =(*emcShowerTds)->dphi();   
    Double_t    energy =  (*emcShowerTds)->energy();  
    Double_t    dE    = (*emcShowerTds)->dE();     
    Double_t    eSeed  = (*emcShowerTds)->eSeed();
    Double_t    e3x3  = (*emcShowerTds)->e3x3();
    Double_t    e5x5  = (*emcShowerTds)->e5x5();
    Double_t    eall  = (*emcShowerTds)->getEAll();
    Double_t    elepton  = (*emcShowerTds)->getELepton();
    Double_t    time  = (Int_t)(*emcShowerTds)->time();
    Double_t    secondMoment = (*emcShowerTds)->secondMoment();
    Double_t    latMoment = (*emcShowerTds)->latMoment();
    Double_t    a20Moment = (*emcShowerTds)->a20Moment();
    Double_t    a42Moment = (*emcShowerTds)->a42Moment();

    HepSymMatrix matrix   = (*emcShowerTds)->errorMatrix();
    //cout<<"matrix: \n"<<matrix<<endl;
    Double_t    err[6];
    err[0] = matrix[0][0];
    err[1] = matrix[1][1];
    err[2] = matrix[2][2];
    err[3] = matrix[0][1];
    err[4] = matrix[0][2];
    err[5] = matrix[1][2];

    map<Int_t, Double_t> cellIdMap;
    RecEmcFractionMap::const_iterator pFractionMap;

    for(pFractionMap=(*emcShowerTds)->Begin();
        pFractionMap!=(*emcShowerTds)->End();
        pFractionMap++) {
      Int_t id = pFractionMap->first;
      Double_t frac = pFractionMap->second.getFraction();
      cellIdMap[id]=frac;
    }

    vector<Int_t> cellId3x3;
    RecEmcIDVector id3x3 = (*emcShowerTds)->getCellId3x3();
    ci_RecEmcIDVector ciCellId;
    for(ciCellId=id3x3.begin();
        ciCellId!=id3x3.end();
        ciCellId++) {
      Int_t id = *ciCellId;
      cellId3x3.push_back(id);
    }

    vector<Int_t> cellId5x5;
    RecEmcIDVector id5x5 = (*emcShowerTds)->getCellId5x5();
    for(ciCellId=id5x5.begin();
        ciCellId!=id5x5.end();
        ciCellId++) {
      Int_t id = *ciCellId;
      cellId5x5.push_back(id);
    }

    TRecEmcShower *emcShowerRoot = new TRecEmcShower();
    //m_common.m_recEmcShowerMap[(*emcShowerTds)] = emcShowerRoot;

    //emcShowerRoot->initialize(numHits ,status ,cellId ,module ,x ,dx,y ,dy,z ,dz,theta ,dtheta ,phi,dphi ,energy ,dE,cosx ,cosy ,cosz);
    emcShowerRoot->setTrackId(trackId );
    emcShowerRoot->setNumHits(numHits );    
    emcShowerRoot->setStatus(status );     
    emcShowerRoot->setCellId( cellId );     
    emcShowerRoot->setClusterId( clusterId );     
    emcShowerRoot->setModule( module);     
    emcShowerRoot->setX( x    );     
    emcShowerRoot->setY( y    );     
    emcShowerRoot->setZ( z    );     
    emcShowerRoot->setEnergy( energy  );    
    emcShowerRoot->setDE( dE  );     
    emcShowerRoot->setTheta( theta  );  
    emcShowerRoot->setDtheta( dtheta     );  
    emcShowerRoot->setPhi( phi       );  
    emcShowerRoot->setDphi( dphi     );  
    emcShowerRoot->setESeed( eSeed     );
    emcShowerRoot->setE3x3( e3x3    );
    emcShowerRoot->setE5x5( e5x5     );  
    emcShowerRoot->setEAll( eall     );  
    emcShowerRoot->setELepton( elepton     );  
    emcShowerRoot->setTime( time );
    emcShowerRoot->setSecondMoment(secondMoment); 
    emcShowerRoot->setLatMoment(latMoment); 
    emcShowerRoot->setA20Moment(a20Moment); 
    emcShowerRoot->setA42Moment(a42Moment); 
    emcShowerRoot->setErr( err );   
    //emcShowerRoot->setVecHits(vecHits);

    emcShowerRoot->setCellIdMap(cellIdMap);
    emcShowerRoot->setCellId3x3(cellId3x3);
    emcShowerRoot->setCellId5x5(cellId5x5);

    recEvt->addEmcShower(emcShowerRoot);
  }
  return StatusCode::SUCCESS;
}
#endif
