#ifndef RecMdcTrackCnv_CXX
#define RecMdcTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"
#include "Identifier/MdcID.h"
#include "Identifier/Identifier.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"  
#include "MdcRecEvent/RecMdcTrack.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "RootEventData/TRecMdcTrack.h"
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecMdcTrackCnv> s_factory;
//const ICnvFactory& RecMdcTrackCnvFactory = s_factory;

RecMdcTrackCnv::RecMdcTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecMdcTrackCnv");
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recMdcTrackCol";
    //declareObject(EventModel::Recon::RecMdcTrackCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recMdcTrackCol);
    m_recMdcTrackCol=0;
}

StatusCode RecMdcTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "RecMdcTrackCnv");
  log << MSG::DEBUG << "RecMdcTrackCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the RecMdcTrack Collection
  RecMdcTrackCol* recMdcTrackCol = new RecMdcTrackCol;
  refpObject = recMdcTrackCol;
  // now convert
  if (!m_recMdcTrackCol) return StatusCode::SUCCESS;
  TIter mdcTrackIter(m_recMdcTrackCol);

  //----------get hit smartRefVector--------
  //HitRefVec *hitRefVec;
  //int nTk=0;
  //while (mdcTrackIter.Next()){ nTk++; }
  //hitRefVec = new HitRefVec[nTk];

  IDataProviderSvc* dataSvc = 0;
  StatusCode sc = serviceLocator()->getService ("EventDataSvc",  
		    IDataProviderSvc::interfaceID(), (IInterface*&)dataSvc);
  if (!sc.isSuccess()) {
    log << MSG::FATAL << "Could not get EventDataSvc in RecMdcTrackCnv" << endreq;
    return( StatusCode::FAILURE);
  }
  SmartDataPtr<RecMdcHitCol> recMdcHitCol(dataSvc,"/Event/Recon/RecMdcHitCol");
  if (!recMdcHitCol) {
    log << MSG::FATAL << "Could not find RecMdcHitCol" << endreq;
    return( StatusCode::FAILURE);
  }
  //RecMdcHitCol::iterator iter = recMdcHitCol->begin();
  //for (;iter != recMdcHitCol->end(); iter++ ) {
  //  int tkId = (*iter)->getTrkId(); 
  //  hitRefVec[tkId].push_back(*iter);
  //}

  //---------------------------------------
  mdcTrackIter = m_recMdcTrackCol;
  TRecMdcTrack *recMdcTrackRoot = 0;
  while ((recMdcTrackRoot = (TRecMdcTrack*)mdcTrackIter.Next())) {
    int      trackId =  recMdcTrackRoot->trackId();
    int      charge =	recMdcTrackRoot->charge();     
    double   helix[5];
    for(int i=0;i<5; i++){
      helix[i] = recMdcTrackRoot->helix(i);
    }
    double   pxy   = recMdcTrackRoot->pxy();
    double   px    = recMdcTrackRoot->px();
    double   py    = recMdcTrackRoot->py();
    double   pz    = recMdcTrackRoot->pz();
    double   p     = recMdcTrackRoot->p();
    double   theta = recMdcTrackRoot->theta();
    double   phi   = recMdcTrackRoot->phi();
    double   x     = recMdcTrackRoot->x();
    double   y     = recMdcTrackRoot->y();
    double   z     = recMdcTrackRoot->z();
    double   r     = recMdcTrackRoot->r();
    int      stat  = recMdcTrackRoot->stat();      
    double   chi   = recMdcTrackRoot->chi2();      
    int      ndof  = recMdcTrackRoot->ndof();       
    int      nlayer= recMdcTrackRoot->nlayer();       
    double err[15];
    for (int i=0; i<15; i++){
      err[i] = recMdcTrackRoot->err(i);
    }
    int      nh     = recMdcTrackRoot->nhits();     
    int      ns     = recMdcTrackRoot->nster();    
    // vector<int> vecHits = recMdcTrackRoot->vecHits();
    double   vx0    = recMdcTrackRoot->vx0();
    double   vy0    = recMdcTrackRoot->vy0();
    double   vz0    = recMdcTrackRoot->vz0();
    double   fiterm = recMdcTrackRoot->fiTerm();
    RecMdcTrack *recMdcTrack = new RecMdcTrack();
    m_common.m_rootRecMdcTrackMap[recMdcTrackRoot] = recMdcTrack;

    recMdcTrack->setTrackId( trackId );
    recMdcTrack->setCharge( charge );
    recMdcTrack->setHelix( helix );
    recMdcTrack->setPxy( pxy );   
    recMdcTrack->setPx( px );   
    recMdcTrack->setPy( py );   
    recMdcTrack->setPz( pz );   
    recMdcTrack->setP( p );   
    recMdcTrack->setTheta( theta );   
    recMdcTrack->setPhi( phi );   
    recMdcTrack->setX( x );   
    recMdcTrack->setY( y );    
    recMdcTrack->setZ( z );   
    recMdcTrack->setR( r );   
    recMdcTrack->setStat( stat );  
    recMdcTrack->setChi2( chi );  
    recMdcTrack->setNdof( ndof );  
    recMdcTrack->setError( err );   
    recMdcTrack->setNhits( nh );  
    recMdcTrack->setNster( ns ); 
    recMdcTrack->setNlayer( nlayer ); 
    // recMdcTrack->setVecHits( vecHits );
    //rec
    recMdcTrack->setVX0( vx0 );   
    recMdcTrack->setVY0( vy0 );    
    recMdcTrack->setVZ0( vz0 );   
    recMdcTrack->setFiTerm( fiterm );  

    HitRefVec theHitRefVec;

    RecMdcHitCol::iterator iter = recMdcHitCol->begin();
    for (;iter != recMdcHitCol->end(); iter++){
  
      //cout<<" (*iter)->getTrkId(): "<<(*iter)->getTrkId()<<endl;
      if((*iter)->getTrkId() == trackId){
	SmartRef<RecMdcHit> refhit(*iter);
	theHitRefVec.push_back(refhit);
      }
    }

    recMdcTrack->setVecHits(theHitRefVec);
    
    int nhits = recMdcTrack->getVecHits().size();
          
    //std::cout<<" mdc hits: "<<nhits<< std::endl;
	
    for(int ii=0; ii <nhits ; ii++){

      //cout<<"ddl: "<<(recMdcTrack->getVecHits()[ii])->getDriftDistLeft()<<endl;
      //cout<<"erddl: "<<(recMdcTrack->getVecHits()[ii])->getErrDriftDistLeft()<<endl;
      Identifier id(recMdcTrack->getVecHits()[ii]->getMdcId());
      int layer = MdcID::layer(id);
      int wire = MdcID::wire(id);
      //cout<<"layer: "<<layer<<" wire: "<<wire<<endl;
    } 
      recMdcTrackCol->push_back(recMdcTrack);
  }

  delete m_recMdcTrackCol;
  m_recMdcTrackCol = 0;  
  return StatusCode::SUCCESS;
}




StatusCode RecMdcTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "RecMdcTrackCnv");
  log << MSG::DEBUG << "RecMdcTrackCnv::DataObjectToTObject" << endreq;
 
  RecMdcTrackCol * mdcTrackColTds=dynamic_cast<RecMdcTrackCol *> (obj);
  if (!mdcTrackColTds) {
    log << MSG::ERROR << "Could not downcast to MdcTrackCol" << endreq;
    return StatusCode::FAILURE;
  }
  DataObject *evt;
  StatusCode sc = m_eds->findObject(EventModel::Recon::Event,evt);
  if (!sc.isSuccess()) {
    log << MSG::ERROR << "Could not get ReconEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "RecMdcTrackCnv:Could not downcast to TDS ReconEvent" << endreq;
  }

  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr);
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_recMdcTrackCol = recEvt->getRecMdcTrackCol();
  if (!m_recMdcTrackCol) return StatusCode::SUCCESS;
  //necessary in case there is I/O at the same time since array is static
  recEvt->clearRecMdcTrackCol();

  RecMdcTrackCol::const_iterator recMdcTrack;
  for (recMdcTrack = mdcTrackColTds->begin(); recMdcTrack != mdcTrackColTds->end(); recMdcTrack++) {
     Int_t      trackId =(*recMdcTrack)->trackId();
     Double_t   helix[5];
     for(int i=0;i<5;i++){
       log<<MSG::INFO<<" recMdcTrack.helix("<<i<<"): "<<(*recMdcTrack)->helix(i)<<endreq;
       helix[i] = (*recMdcTrack)->helix(i);     
     }

     Int_t      stat =	(*recMdcTrack)->stat();      
     Double_t   chi =	(*recMdcTrack)->chi2();      
     Int_t      ndof =	(*recMdcTrack)->ndof();       
     Int_t      ns =    (*recMdcTrack)->nster();
     Int_t      nlayer =(*recMdcTrack)->nlayer();       

     Double_t er[15];
     for (Int_t i=0; i<15; i++){
       er[i] = (*recMdcTrack)->err(i);
     }
     Int_t      nh = 	(*recMdcTrack)->getNhits();     
     // vector<Int_t> vecHits = (*recMdcTrack)->vecHits();
     Double_t   vx0 =	(*recMdcTrack)->getVX0();         
     Double_t   vy0 =	(*recMdcTrack)->getVY0();         
     Double_t   vz0 = 	(*recMdcTrack)->getVZ0();
     Double_t   fiterm =(*recMdcTrack)->getFiTerm();

     TRecMdcTrack *recMdcTrackRoot = new TRecMdcTrack();
     //m_common.m_recMdcTrackMap[(*recMdcTrack)] = recMdcTrackRoot;

     recMdcTrackRoot->setTrackId( trackId );
     recMdcTrackRoot->setHelix(helix);
     log<<MSG::INFO<<" test,recMdcTrackRoot.px: "<<recMdcTrackRoot->px()
                   <<" recMdcTrackRoot.py: "<<recMdcTrackRoot->py()
	           <<" recMdcTrackRoot.pz: "<<recMdcTrackRoot->pz()
		   <<endreq;
       
     recMdcTrackRoot->setStat( stat );  
     recMdcTrackRoot->setChi2( chi );  
     recMdcTrackRoot->setNdof( ndof );
     recMdcTrackRoot->setNlayer( nlayer );  
     recMdcTrackRoot->setErr( er );   
     recMdcTrackRoot->setNhits( nh );  
     recMdcTrackRoot->setNster( ns  );  
     // recMdcTrackRoot->setVecHits(vecHits);
     recMdcTrackRoot->setVX0( vx0 );
     recMdcTrackRoot->setVY0( vy0 );
     recMdcTrackRoot->setVZ0( vz0 );
     recMdcTrackRoot->setFiTerm( fiterm );

     recEvt->addRecMdcTrack(recMdcTrackRoot);
  }


  return StatusCode::SUCCESS;
}
#endif
