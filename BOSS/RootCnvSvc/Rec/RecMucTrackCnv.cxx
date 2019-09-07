#ifndef RecMucTrackCnv_CXX
#define RecMucTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "MucRecEvent/RecMucTrack.h"

//#include "RootEventData/TMucTrack.h" // standard root object
#include "RootEventData/TRecMucTrack.h" // standard root object
//#include "RootEventData/TDstEvent.h"
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMucTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecMucTrackCnv> s_factory;
//const ICnvFactory& RecMucTrackCnvFactory = s_factory;


RecMucTrackCnv::RecMucTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecMucTrackCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    //RecMucTrack *atrack = new RecMucTrack();
    //RecMucTrackCol *atrackCol = new RecMucTrackCol;
    //cout<<"in RecMucTrackCnv "<<atrack->clID()<<"  "<<atrackCol->clID()<<"  "<<CLID_ObjectVector<<endl; 
    m_rootBranchname ="m_recMucTrackCol";
    //declareObject(EventModel::Recon::RecMucTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recMucTrackCol);
    m_recMucTrackCol=0;
}

StatusCode RecMucTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "RecMucTrackCnv");
    log << MSG::DEBUG << "RecMucTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MucTrack Collection
    RecMucTrackCol* mucTrackTdsCol = new RecMucTrackCol;
    refpObject=mucTrackTdsCol;


    // now convert
    if (!m_recMucTrackCol) return sc;
    TIter mucTrackIter(m_recMucTrackCol);
    TRecMucTrack *mucTrackRoot = 0;
    while ((mucTrackRoot = (TRecMucTrack*)mucTrackIter.Next())) {

      int trackId = mucTrackRoot->trackId();	    
      int id = mucTrackRoot-> id();    
      int st = mucTrackRoot->status();    
      int type = mucTrackRoot->type();      
      
      int startPart = mucTrackRoot->startPart();
      int endPart   = mucTrackRoot->endPart();
      int brLastLayer = mucTrackRoot->brLastLayer();
      int ecLastLayer = mucTrackRoot->ecLastLayer();
      int numHits = mucTrackRoot->numHits();     
      int numLayers = mucTrackRoot->numLayers(); 
      int maxHitsInLayer = mucTrackRoot->maxHitsInLayer();

      double depth = mucTrackRoot->depth();    
      double chi2  = mucTrackRoot->chi2();      
      int    dof   = mucTrackRoot->dof();       
      double rms   = mucTrackRoot->rms();       

        
      
      double xPos =  mucTrackRoot->xPos() ;
      double yPos =  mucTrackRoot->yPos() ;
      double zPos =  mucTrackRoot->zPos() ;

      double xPosSigma =  mucTrackRoot->xPosSigma() ;
      double yPosSigma =  mucTrackRoot->yPosSigma() ;
      double zPosSigma =  mucTrackRoot->zPosSigma() ;
      
      double px =  mucTrackRoot->px() ;
      double py =  mucTrackRoot->py() ;
      double pz =  mucTrackRoot->pz() ;

      double distance = mucTrackRoot->distance();
      double deltaphi = mucTrackRoot->deltaPhi();
      //cout<<"in RecMucTrackCnv  distance = "<<distance<<" xpos= "<<xPos<<endl;

      vector<int> vecHits = mucTrackRoot->vecHits();
	    vector<int> expHits = mucTrackRoot->expHits();
      vector<float> distHits = mucTrackRoot->distHits();
      
      //added by LI Chunhua
      double krechi2 = mucTrackRoot->kalRechi2();
      int    kdof = mucTrackRoot->kaldof();
      double kdepth = mucTrackRoot->kaldepth();
      int   kbrlay = mucTrackRoot->kalbrLastLayer();
      int   keclay = mucTrackRoot->kalecLastLayer();
      //*********************
      //cout<<"in RecMucTrackCnv  size = "<<vecHits.size()<<"    "<<expHits.size()<<"   "<<distHits.size()<<endl;
      
      RecMucTrack *mucTrackTds = new RecMucTrack();
      m_common.m_rootRecMucTrackMap[mucTrackRoot] = mucTrackTds;
      
      mucTrackTds->setTrackId( trackId );
      mucTrackTds->setId( id );      
      mucTrackTds->setStatus( st ); 
      mucTrackTds->setType( type );

      mucTrackTds->setStartPart( startPart );
      mucTrackTds->setEndPart( endPart );
      mucTrackTds->setBrLastLayer( brLastLayer );
      mucTrackTds->setEcLastLayer( ecLastLayer );
      mucTrackTds->setNumHits( numHits );         
      mucTrackTds->setNumLayers( numLayers );   
      mucTrackTds->setMaxHitsInLayer( maxHitsInLayer );

      mucTrackTds->setDepth( depth ); 
      mucTrackTds->setChi2( chi2 );   
      mucTrackTds->setDof( dof );    
      mucTrackTds->setRms( rms ); 

      mucTrackTds->setXPos( xPos ); 
      mucTrackTds->setYPos( yPos ); 
      mucTrackTds->setZPos( zPos ); 

      mucTrackTds->setXPosSigma( xPosSigma );
      mucTrackTds->setYPosSigma( yPosSigma );
      mucTrackTds->setZPosSigma( zPosSigma );
      
      mucTrackTds->setPx( px ); 
      mucTrackTds->setPy( py ); 
      mucTrackTds->setPz( pz ); 
  
      mucTrackTds->setDistance(distance);
      mucTrackTds->setDeltaPhi(deltaphi);
      mucTrackTds->setVecHits(vecHits);     //need fix
      mucTrackTds->setExpHits(expHits);
      mucTrackTds->setDistHits(distHits);
      
      //added by LI Chunhua 2013/02/01
      mucTrackTds->setkalRechi2(krechi2);
      mucTrackTds->setkalDof(kdof);
      mucTrackTds->setkalDepth(kdepth);
      mucTrackTds->setkalbrLastLayer(kbrlay);
      mucTrackTds->setkalecLastLayer(keclay);
      //******************

      //cout<<"in RecMucTrackCnv::T->D  set mucpos"<<endl;

      mucTrackTdsCol->push_back(mucTrackTds); 
      //delete mucTrackTds; // wensp add 2005/12/31
     // mucTrackTds = NULL;      
    }

    // m_mucTrackCol->Delete();  // wensp add 2005/12/30
    delete m_recMucTrackCol;
    m_recMucTrackCol = 0;    
    return StatusCode::SUCCESS;
}

StatusCode RecMucTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "RecMucTrackCnv");
  log << MSG::DEBUG << "RecMucTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  RecMucTrackCol * mucTrackColTds=dynamic_cast<RecMucTrackCol *> (obj);
  if (!mucTrackColTds) {
    log << MSG::ERROR << "Could not downcast to RecMucTrackCol" << endreq;
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
    log << MSG::ERROR << "RecMucTrackCnv:Could not downcast to TDS DigiEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_recMucTrackCol = recEvt->getMucTrackCol();
  if (!m_recMucTrackCol) return sc;
  recEvt->clearMucTrackCol(); //necessary in case there is I/O at the same time since array is static
  RecMucTrackCol::const_iterator mucTrackTds;

  for (mucTrackTds = mucTrackColTds->begin(); mucTrackTds != mucTrackColTds->end(); mucTrackTds++) {
    Int_t trackId = (*mucTrackTds)->trackId();
    Int_t id = (*mucTrackTds)-> id();    
    Int_t st = (*mucTrackTds)->status();    
    Int_t type = (*mucTrackTds)->type();      
    
    Int_t startPart = (*mucTrackTds)->startPart();
    Int_t endPart   = (*mucTrackTds)->endPart();
    Int_t brLastLayer = (*mucTrackTds)->brLastLayer();
    Int_t ecLastLayer = (*mucTrackTds)->ecLastLayer();
    Int_t numHits = (*mucTrackTds)->numHits();     
    Int_t numLayers = (*mucTrackTds)->numLayers(); 
    Int_t maxHitsInLayer = (*mucTrackTds)->maxHitsInLayer();
    
    Double_t depth = (*mucTrackTds)->depth();    
    Double_t chi2  = (*mucTrackTds)->chi2();      
    Int_t    dof   = (*mucTrackTds)->dof();       
    Double_t rms   = (*mucTrackTds)->rms();       
    
    Double_t xPos =  (*mucTrackTds)->xPos() ;
    Double_t yPos =  (*mucTrackTds)->yPos() ;
    Double_t zPos =  (*mucTrackTds)->zPos() ;
    
    Double_t xPosSigma =  (*mucTrackTds)->xPosSigma() ;
    Double_t yPosSigma =  (*mucTrackTds)->yPosSigma() ;
    Double_t zPosSigma =  (*mucTrackTds)->zPosSigma() ;
    
    Double_t distance = (*mucTrackTds)->distance();
    Double_t deltaphi = (*mucTrackTds)->deltaPhi();
    
    Double_t px =  (*mucTrackTds)->px() ;
    Double_t py =  (*mucTrackTds)->py() ;
    Double_t pz =  (*mucTrackTds)->pz() ;


    vector<Int_t> vecHits = (*mucTrackTds)->getVecHits();
    vector<Int_t> expHits = (*mucTrackTds)->getExpHits();
    vector<Float_t> distHits = (*mucTrackTds)->getDistHits();
    //added by LI Chunhua
    Double_t krechi2 = (*mucTrackTds)->kalRechi2();
    Int_t  kdof = (*mucTrackTds)->kaldof();
    Double_t kdepth = (*mucTrackTds)->kaldepth();
    Int_t   kbrlay = (*mucTrackTds)->kalbrLastLayer();
    Int_t   keclay = (*mucTrackTds)->kalecLastLayer();
    //*********************

    /*
    cout<<"in RecMucTrackCnv  "<<vecHits.size()<<endl;
    for(int i = 0; i < vecHits.size(); i++){
      cout<<"  vec i = "<<i<<"   "<<vecHits[i]<<endl;
    }

    for(int i = 0; i < expHits.size(); i++){
      cout<<"  exp i = "<<i<<"   "<<expHits[i]<<endl;
    }

    for(int i = 0; i < distHits.size(); i++){
      cout<<"  dist i = "<<i<<"   "<<distHits[i]<<endl;
    }
    */
    
    TRecMucTrack *mucTrackRoot = new TRecMucTrack();  //liangyt
    //m_common.m_RecMucTrackMap[(*mucTrackTds)] = mucTrackRoot;

    mucTrackRoot->setTrackId( trackId );
    mucTrackRoot->setId( id );      
    mucTrackRoot->setStatus( st ); 
    mucTrackRoot->setType( type );
    
    mucTrackRoot->setStartPart( startPart );
    mucTrackRoot->setEndPart( endPart );
    mucTrackRoot->setBrLastLayer( brLastLayer );
    mucTrackRoot->setEcLastLayer( ecLastLayer );
    mucTrackRoot->setNumHits( numHits );         
    mucTrackRoot->setNumLayers( numLayers );   
    mucTrackRoot->setMaxHitsInLayer( maxHitsInLayer );
    
    mucTrackRoot->setDepth( depth ); 
    mucTrackRoot->setChi2( chi2 );   
    mucTrackRoot->setDof( dof );    
    mucTrackRoot->setRms( rms ); 
    
    mucTrackRoot->setXPos( xPos ); 
    mucTrackRoot->setYPos( yPos ); 
    mucTrackRoot->setZPos( zPos );

    mucTrackRoot->setXPosSigma( xPosSigma );
    mucTrackRoot->setYPosSigma( yPosSigma );
    mucTrackRoot->setZPosSigma( zPosSigma );
        
    mucTrackRoot->setDistance(distance);
    mucTrackRoot->setDeltaPhi(deltaphi);
    
    //cout<<"in RecMucTrackCnv  xyz = "<<xPos<<"  "<<yPos<<"  "<<zPos<<" depth= "<<depth<<" "<<maxHitsInLayer<<"  "<<deltaphi<<endl;

    mucTrackRoot->setPx( px ); 
    mucTrackRoot->setPy( py ); 
    mucTrackRoot->setPz( pz ); 
    
    mucTrackRoot->setVecHits(vecHits);   
    mucTrackRoot->setExpHits(expHits);
    mucTrackRoot->setDistHits(distHits);
    
    //added by LI Chunhua 2013/02/01
    mucTrackRoot->setkalRechi2(krechi2);
    mucTrackRoot->setkalDof(kdof);
    mucTrackRoot->setkalDepth(kdepth);
    mucTrackRoot->setkalbrLastLayer(kbrlay);
    mucTrackRoot->setkalecLastLayer(keclay);
    //******************
    recEvt->addMucTrack(mucTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
