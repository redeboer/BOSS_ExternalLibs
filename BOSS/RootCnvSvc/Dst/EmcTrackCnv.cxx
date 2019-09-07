#ifndef EmcTrackCnv_CXX
#define EmcTrackCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstEmcShower.h"   //TDS object
#include "RootEventData/TEmcTrack.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/EmcTrackCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<EmcTrackCnv> s_factory;
//const ICnvFactory& EmcTrackCnvFactory = s_factory;

EmcTrackCnv::EmcTrackCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the /Event path on the TDS.
  MsgStream log(msgSvc(), "EmcTrackCnv");
  // log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
  m_rootTreename ="Rec";
  m_rootBranchname ="m_emcTrackCol";
  //declareObject(EventModel::Dst::DstEmcShowerCol, objType(), m_rootTreename, m_rootBranchname);
  m_adresses.push_back(&m_emcTrackCol);
  m_emcTrackCol=0;

}

StatusCode EmcTrackCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

  MsgStream log(msgSvc(), "EmcTrackCnv");
  log << MSG::DEBUG << "EmcTrackCnv::TObjectToDataObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  // create the TDS location for the EmcTrack Collection
  DstEmcShowerCol* emcTrackTdsCol = new DstEmcShowerCol;
  refpObject=emcTrackTdsCol;


  // now convert
  if (!m_emcTrackCol) return sc;
  TIter emcTrackIter(m_emcTrackCol);
  TEmcTrack *emcTrackRoot = 0;
  while ((emcTrackRoot = (TEmcTrack*)emcTrackIter.Next())) {
    int       trackId = emcTrackRoot->trackId();
    int       numHits = emcTrackRoot->numHits(); 
    int       status =	emcTrackRoot->status(); 
    int       cellId =	emcTrackRoot->cellId(); 
    int       module =	emcTrackRoot->module(); 
    double    x =	emcTrackRoot->x();      
    double    y =	emcTrackRoot->y();      
    double    z =	emcTrackRoot->z();      
    double    dtheta =	emcTrackRoot->dtheta(); 
    double    dphi   =	emcTrackRoot->dphi();   
    double    energy =  emcTrackRoot->energy();  
    double    dE    = 	emcTrackRoot->dE();     
    double  eSeed =  emcTrackRoot->eSeed();
    double  e3x3 =  emcTrackRoot->e3x3();
    double  e5x5 =  emcTrackRoot->e5x5();
    double  time =  emcTrackRoot->time();
    double  secondMoment = emcTrackRoot->secondMoment();
    double  latMoment = emcTrackRoot->latMoment();
    double  a20Moment = emcTrackRoot->a20Moment();
    double  a42Moment = emcTrackRoot->a42Moment();

    HepSymMatrix matrix(3);
    matrix[0][0]=emcTrackRoot->err(0);
    matrix[1][1]=emcTrackRoot->err(1);
    matrix[2][2]=emcTrackRoot->err(2);
    matrix[0][1]=emcTrackRoot->err(3);
    matrix[0][2]=emcTrackRoot->err(4);
    matrix[1][2]=emcTrackRoot->err(5);

    DstEmcShower *emcTrackTds = new DstEmcShower();
    m_common.m_rootEmcShowerMap[emcTrackRoot] = emcTrackTds;

    emcTrackTds->setTrackId(trackId );
    emcTrackTds->setNumHits(numHits );    
    emcTrackTds->setStatus(status );     
    emcTrackTds->setCellId( cellId );     
    emcTrackTds->setModule( module);     
    HepPoint3D pos(x,y,z);
    emcTrackTds->setPosition(pos);
    emcTrackTds->setEnergy( energy  );    
    emcTrackTds->setDE( dE  );     
    emcTrackTds->setDtheta( dtheta     );  
    emcTrackTds->setDphi( dphi     );  
    emcTrackTds->setESeed(eSeed);
    emcTrackTds->setE3x3(e3x3);
    emcTrackTds->setE5x5(e5x5);
    emcTrackTds->setTime(time);
    emcTrackTds->setSecondMoment(secondMoment); 
    emcTrackTds->setLatMoment(latMoment); 
    emcTrackTds->setA20Moment(a20Moment); 
    emcTrackTds->setA42Moment(a42Moment); 
    emcTrackTds->setErrorMatrix( matrix );   

    emcTrackTdsCol->push_back(emcTrackTds);
    //	   delete emcTrackTds;
    // emcTrackTds = NULL;
  }

  //m_emcTrackCol->Delete();  // wensp add 2005/12/30
  delete m_emcTrackCol;
  m_emcTrackCol = 0;  
  return StatusCode::SUCCESS;
}

StatusCode EmcTrackCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "EmcTrackCnv");
  log << MSG::DEBUG << "EmcTrackCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  DstEmcShowerCol * emcTrackColTds=dynamic_cast<DstEmcShowerCol *> (obj);
  if (!emcTrackColTds) {
    log << MSG::ERROR << "Could not downcast to EmcTrackCol" << endreq;
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
    log << MSG::ERROR << "EmcTrackCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_emcTrackCol = recEvt->getEmcTrackCol();
  if (!m_emcTrackCol) return sc;
  recEvt->clearEmcTrackCol(); //necessary in case there is I/O at the same time since array is static
  DstEmcShowerCol::const_iterator emcTrackTds;

  for (emcTrackTds = emcTrackColTds->begin(); emcTrackTds != emcTrackColTds->end(); emcTrackTds++) {
    Int_t       trackId = (*emcTrackTds)->trackId();
    Int_t       numHits = (*emcTrackTds)->numHits(); 
    Int_t       status =(*emcTrackTds)->status(); 
    Int_t       cellId =(*emcTrackTds)->cellId(); 
    Int_t       module =(*emcTrackTds)->module(); 
    Double_t    x =	(*emcTrackTds)->x();      
    Double_t    y =	(*emcTrackTds)->y();      
    Double_t    z =	(*emcTrackTds)->z();      
    Double_t    dtheta =(*emcTrackTds)->dtheta(); 
    Double_t    dphi   =(*emcTrackTds)->dphi();   
    Double_t    energy =  (*emcTrackTds)->energy();  
    Double_t    dE    = (*emcTrackTds)->dE();     
    Double_t    eSeed  = (*emcTrackTds)->eSeed();
    Double_t    e3x3  = (*emcTrackTds)->e3x3();
    Double_t    e5x5  = (*emcTrackTds)->e5x5();
    Double_t    time  = (*emcTrackTds)->time();
    Double_t    secondMoment = (*emcTrackTds)->secondMoment();
    Double_t    latMoment = (*emcTrackTds)->latMoment();
    Double_t    a20Moment = (*emcTrackTds)->a20Moment();
    Double_t    a42Moment = (*emcTrackTds)->a42Moment();

    HepSymMatrix matrix   = (*emcTrackTds)->errorMatrix();
    Double_t    err[6];
    err[0] = matrix[0][0];
    err[1] = matrix[1][1];
    err[2] = matrix[2][2];
    err[3] = matrix[0][1];
    err[4] = matrix[0][2];
    err[5] = matrix[1][2];

    TEmcTrack *emcTrackRoot = new TEmcTrack();
    //m_common.m_emcShowerMap[(*emcTrackTds)] = emcTrackRoot;

    //emcTrackRoot->initialize(numHits ,status ,cellId ,module ,x ,dx,y ,dy,z ,dz,theta ,dtheta ,phi,dphi ,energy ,dE,cosx ,cosy ,cosz);
    emcTrackRoot->setTrackId(trackId );
    emcTrackRoot->setNumHits(numHits );    
    emcTrackRoot->setStatus(status );     
    emcTrackRoot->setCellId( cellId );     
    emcTrackRoot->setModule( module);     
    emcTrackRoot->setX( x    );     
    emcTrackRoot->setY( y    );     
    emcTrackRoot->setZ( z    );     
    emcTrackRoot->setEnergy( energy  );    
    emcTrackRoot->setDE( dE  );     
    emcTrackRoot->setDtheta( dtheta     );  
    emcTrackRoot->setDphi( dphi     );  
    emcTrackRoot->setESeed( eSeed     );
    emcTrackRoot->setE3x3( e3x3    );
    emcTrackRoot->setE5x5( e5x5     );  
    emcTrackRoot->setTime( time     );  
    emcTrackRoot->setSecondMoment(secondMoment); 
    emcTrackRoot->setLatMoment(latMoment); 
    emcTrackRoot->setA20Moment(a20Moment); 
    emcTrackRoot->setA42Moment(a42Moment); 
    emcTrackRoot->setErr( err );   

    recEvt->addEmcTrack(emcTrackRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
