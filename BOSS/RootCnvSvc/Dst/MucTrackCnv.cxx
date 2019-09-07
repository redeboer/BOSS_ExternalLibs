#ifndef MucTrackCnv_CXX
#define MucTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstMucTrack.h"   //TDS object
#include "RootEventData/TMucTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/MucTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<MucTrackCnv> s_factory;
//const ICnvFactory& MucTrackCnvFactory = s_factory;

MucTrackCnv::MucTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MucTrackCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mucTrackCol";
    //declareObject(EventModel::Dst::DstMucTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mucTrackCol);
    m_mucTrackCol=0;
}

StatusCode MucTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MucTrackCnv");
    log << MSG::DEBUG << "MucTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MucTrack Collection
    DstMucTrackCol* mucTrackTdsCol = new DstMucTrackCol;
    refpObject=mucTrackTdsCol;


    // now convert
    if (!m_mucTrackCol) return sc;
    TIter mucTrackIter(m_mucTrackCol);
    TMucTrack *mucTrackRoot = 0;
    while ((mucTrackRoot = (TMucTrack*)mucTrackIter.Next())) {

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
      //vector<int> vecHits = mucTrackRoot->vecHits();
      //added by LI Chunhua
      double krechi2 = mucTrackRoot->kalRechi2();
      int  kdof = mucTrackRoot->kaldof();
      double kdepth = mucTrackRoot->kaldepth();
      int   kbrlay = mucTrackRoot->kalbrLastLayer();
      int   keclay = mucTrackRoot->kalecLastLayer();
      //*********************

      DstMucTrack *mucTrackTds = new DstMucTrack();
      m_common.m_rootMucTrackMap[mucTrackRoot] = mucTrackTds;
      
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
      
      //mucTrackTds->setVecHits(vecHits);   
      //added by LI Chunhua 2013/02/01
      mucTrackTds->setkalRechi2(krechi2);
      mucTrackTds->setkalDof(kdof);
      mucTrackTds->setkalDepth(kdepth);
      mucTrackTds->setkalbrLastLayer(kbrlay);
      mucTrackTds->setkalecLastLayer(keclay);
      //******************
      mucTrackTdsCol->push_back(mucTrackTds); 
      //delete mucTrackTds; // wensp add 2005/12/31
     // mucTrackTds = NULL;      
    }

    // m_mucTrackCol->Delete();  // wensp add 2005/12/30
    delete m_mucTrackCol;
    m_mucTrackCol = 0;    
    return StatusCode::SUCCESS;
}

StatusCode MucTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MucTrackCnv");
  log << MSG::DEBUG << "MucTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstMucTrackCol * mucTrackColTds=dynamic_cast<DstMucTrackCol *> (obj);
  if (!mucTrackColTds) {
    log << MSG::ERROR << "Could not downcast to MucTrackCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Dst::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get DstEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  DstEvent * devtTds=dynamic_cast<DstEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "MucTrackCnv:Could not downcast to TDS DigiEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_mucTrackCol = recEvt->getMucTrackCol();
  if (!m_mucTrackCol) return sc;
  recEvt->clearMucTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstMucTrackCol::const_iterator mucTrackTds;

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

    Double_t px =  (*mucTrackTds)->px() ;
    Double_t py =  (*mucTrackTds)->py() ;
    Double_t pz =  (*mucTrackTds)->pz() ;
    
    Double_t distance = (*mucTrackTds)->distance();
    Double_t deltaphi = (*mucTrackTds)->deltaPhi();
    //added by LI Chunhua
    Double_t krechi2 = (*mucTrackTds)->kalRechi2();
    Int_t  kdof = (*mucTrackTds)->kaldof();
    Double_t kdepth = (*mucTrackTds)->kaldepth();
    Int_t   kbrlay = (*mucTrackTds)->kalbrLastLayer();
    Int_t   keclay = (*mucTrackTds)->kalecLastLayer();
    //*********************

    //vector<Int_t> vecHits = (*mucTrackTds)->vecHits();
           
    TMucTrack *mucTrackRoot = new TMucTrack();
    //m_common.m_mucTrackMap[(*mucTrackTds)] = mucTrackRoot;

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

    mucTrackRoot->setPx( px ); 
    mucTrackRoot->setPy( py ); 
    mucTrackRoot->setPz( pz ); 
    
    mucTrackRoot->setDistance(distance);
    mucTrackRoot->setDeltaPhi(deltaphi);
    //added by LI Chunhua 2013/02/01
    mucTrackRoot->setkalRechi2(krechi2);
    mucTrackRoot->setkalDof(kdof);
    mucTrackRoot->setkalDepth(kdepth);
    mucTrackRoot->setkalbrLastLayer(kbrlay);
    mucTrackRoot->setkalecLastLayer(keclay);
    //******************
    //mucTrackRoot->setVecHits(vecHits);   

    recEvt->addMucTrack(mucTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
