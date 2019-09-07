#ifndef MdcTrackCnv_CXX
#define MdcTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "TClonesArray.h"
#include "EventModel/EventModel.h"
#include "DstEvent/DstEvent.h"  //TDS object  
#include "DstEvent/DstMdcTrack.h"   //TDS object
#include "RootEventData/TMdcTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"
#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/MdcTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"
#include <vector>
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Geometry/Point3D.h"

using namespace std;


MdcTrackCnv::MdcTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MdcTrackCnv");
    m_rootBranchname ="m_mdcTrackCol";
    m_adresses.push_back(&m_mdcTrackCol);
    m_mdcTrackCol=0;
}

StatusCode MdcTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
    // creation of TDS object from root object
    MsgStream log(msgSvc(), "MdcTrackCnv");
    log << MSG::DEBUG << "MdcTrackCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcTrack Collection
    DstMdcTrackCol* mdcTrackTdsCol = new DstMdcTrackCol;
    refpObject=mdcTrackTdsCol;

    // now convert
    if (!m_mdcTrackCol) return sc;
    TIter mdcTrackIter(m_mdcTrackCol);
    TMdcTrack *mdcTrackRoot = 0;
    while ((mdcTrackRoot = (TMdcTrack*)mdcTrackIter.Next())) {

     int      trackId = mdcTrackRoot->trackId();
     int      charge  =	mdcTrackRoot->charge();     
     double   helix[5];
     for(int i=0; i<5; i++){
       helix[i] = mdcTrackRoot->helix(i);
       log<<MSG::INFO<<"in MdcTrackCnv, helix["<<i<<"]: "<<helix[i]<<endreq;
     }
     double   pxy =	mdcTrackRoot->pxy();       
     double   px =	mdcTrackRoot->px();        
     double   py =	mdcTrackRoot->py();        
     double   pz =	mdcTrackRoot->pz();        
     double   p =	mdcTrackRoot->p();         
     double   theta =	mdcTrackRoot->theta();     
     double   phi =	mdcTrackRoot->phi();       
     double   x =	mdcTrackRoot->x();         
     double   y =	mdcTrackRoot->y();         
     double   z = 	mdcTrackRoot->z();         
     double   r =	mdcTrackRoot->r();         
     int      stat =    mdcTrackRoot->stat();      
     double   chi =	mdcTrackRoot->chi2();      
     int      ndof =	mdcTrackRoot->ndof();       
     int      ns =      mdcTrackRoot->nster();
     int      nlayer =  mdcTrackRoot->nlayer();
     
     double err[15];
     for (int i=0; i<15; i++){
       err[i] = mdcTrackRoot->err(i);
     }
   
     double poca[3];
     poca[0] = x;
     poca[1] = y;
     poca[2] = z;

     log<<MSG::INFO<<"in MdcTrackCnv, mdcTrackRoot.nster "<<ns<<endreq;
     
     DstMdcTrack *mdcTrackTds = new DstMdcTrack();
     m_common.m_rootMdcTrackMap[mdcTrackRoot] = mdcTrackTds;

     mdcTrackTds->setTrackId( trackId );
    log<<MSG::INFO<<" dstMdcTrack trkid: "<<mdcTrackTds->trackId()<<endreq;
  
     mdcTrackTds->setCharge( charge );
     mdcTrackTds->setHelix(helix);
     mdcTrackTds->setPoca(poca);
     
     log<<MSG::INFO<<"dstMdcTrack helix: "<<mdcTrackTds->helix()<<endreq;
    
     mdcTrackTds->setPxy( pxy );   
     mdcTrackTds->setPx( px );   
     mdcTrackTds->setPy( py );   
     mdcTrackTds->setPz( pz );   
     mdcTrackTds->setP( p );   
     mdcTrackTds->setTheta( theta );   
     mdcTrackTds->setPhi( phi );   
     mdcTrackTds->setX( x );   
     mdcTrackTds->setY( y );    
     mdcTrackTds->setZ( z );   
     mdcTrackTds->setR( r );   
     mdcTrackTds->setStat( stat );  
     mdcTrackTds->setChi2( chi );  
     mdcTrackTds->setNdof( ndof );  
     mdcTrackTds->setError( err );   
     mdcTrackTds->setNster( ns  ); 
     mdcTrackTds->setNlayer( nlayer ); 
     
     log<<MSG::INFO<<" dstMdcTrack nster: "<<mdcTrackTds->nster()<<endreq;
       
     mdcTrackTdsCol->push_back(mdcTrackTds);
    
     }
    delete m_mdcTrackCol;
    m_mdcTrackCol = 0;  
    return StatusCode::SUCCESS;
}

StatusCode MdcTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MdcTrackCnv");
  log << MSG::DEBUG << "MdcTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstMdcTrackCol * mdcTrackColTds=dynamic_cast<DstMdcTrackCol *> (obj);
  if (!mdcTrackColTds) {
    log << MSG::ERROR << "Could not downcast to MdcTrackCol" << endreq;
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
    log << MSG::ERROR << "MdcTrackCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_mdcTrackCol = recEvt->getMdcTrackCol();
  if (!m_mdcTrackCol) return sc;
  recEvt->clearMdcTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstMdcTrackCol::const_iterator mdcTrackTds;

  for (mdcTrackTds = mdcTrackColTds->begin(); mdcTrackTds != mdcTrackColTds->end(); mdcTrackTds++) {
     Int_t      trackId =(*mdcTrackTds)->trackId();
     Double_t   helix[5];
     for(int i=0;i<5;i++){
       helix[i] = (*mdcTrackTds)->helix(i) ; 
     }    
     Int_t    stat = (*mdcTrackTds)->stat();      
     Double_t chi =  (*mdcTrackTds)->chi2();      
     Int_t    ndof = (*mdcTrackTds)->ndof();       
     Double_t err[15];
     for (Int_t i=0; i<15; i++){  
       err[i] = (*mdcTrackTds)->err(i);
     }
     Int_t ns = (*mdcTrackTds)->nster();
     Int_t fL = (*mdcTrackTds)->firstLayer();
     Int_t lL = (*mdcTrackTds)->lastLayer();
     Int_t nlayer = (*mdcTrackTds)->nlayer();       

     TMdcTrack *mdcTrackRoot = new TMdcTrack();
     //m_common.m_mdcTrackMap[(*mdcTrackTds)] = mdcTrackRoot;
     mdcTrackRoot->setTrackId( trackId );
    
     mdcTrackRoot->setHelix(helix);
     log<<MSG::INFO<<"test,trackId: "<<mdcTrackRoot->trackId()<<endreq;
     log<<MSG::INFO<<"test,px: "<<mdcTrackRoot->px()
                   <<"test,py: "<<mdcTrackRoot->py()
	           <<"test,pz: "<<mdcTrackRoot->pz()<<endreq;
 
     mdcTrackRoot->setStat( stat );  
     mdcTrackRoot->setChi2( chi );  
     mdcTrackRoot->setNdof( ndof );  
     mdcTrackRoot->setErr( err );   
     mdcTrackRoot->setNster( ns  );  
     mdcTrackRoot->setFirstLayer(fL);
     mdcTrackRoot->setLastLayer(lL);
     mdcTrackRoot->setNlayer(nlayer);
     
     recEvt->addMdcTrack(mdcTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
