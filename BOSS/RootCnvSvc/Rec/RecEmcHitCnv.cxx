#ifndef RecEmcHitCnv_CXX
#define RecEmcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "EmcRecEventModel/RecEmcHit.h"

#include "RootEventData/TRecEmcHit.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecEmcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecEmcHitCnv> s_factory;
//const ICnvFactory& RecEmcHitCnvFactory = s_factory;

RecEmcHitCnv::RecEmcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  //cout<<"in RecEmcHitCnv::constructor: hitid= "<<aHit->clID()<<endl;

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecEmcHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootTreename ="Rec";
    m_rootBranchname ="m_recEmcHitCol";
    //declareObject(EventModel::Recon::RecEmcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recEmcHitCol);
    m_recEmcHitCol=0;
}

StatusCode RecEmcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "RecEmcHitCnv");
  log << MSG::DEBUG << "RecEmcHitCnv::TObjectToDataObject" << endreq;
  //cout<<"RecEmcHitCnv::TObjectToDataObject"<<endl;
  StatusCode sc=StatusCode::SUCCESS;

  // create the TDS location for the RecEmcHit Collection
  RecEmcHitCol* emcHitTdsCol = new RecEmcHitCol;
  refpObject=emcHitTdsCol;

  // now convert
  if (!m_recEmcHitCol) return sc;
  TIter emcHitIter(m_recEmcHitCol);
  TRecEmcHit *emcHitRoot = 0;
  while ((emcHitRoot = (TRecEmcHit*)emcHitIter.Next())) {
    RecEmcID cellId(emcHitRoot->cellId());
    double energy = emcHitRoot->energy();
    double time = emcHitRoot->time();

    //cout<<"cellId="<<cellId<<"\tenergy="<<energy<<"\ttime="<<time<<endl;
    
    RecEmcHit *emcHitTds = new RecEmcHit();
    m_common.m_rootRecEmcHitMap[emcHitRoot] = emcHitTds;

    emcHitTds->Assign(cellId,energy,time);
    
    emcHitTdsCol->push_back(emcHitTds);
  }

  delete m_recEmcHitCol;
  m_recEmcHitCol = 0; 

  return StatusCode::SUCCESS;
}

StatusCode RecEmcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "RecEmcHitCnv");
  log << MSG::DEBUG << "RecEmcHitCnv::DataObjectToTObject" << endreq;
  //cout<<"RecEmcHitCnv::DataObjectToTObject"<<endl;
  StatusCode sc=StatusCode::SUCCESS;

  RecEmcHitCol * emcHitColTds=dynamic_cast<RecEmcHitCol *> (obj);
  if (!emcHitColTds) {
    log << MSG::ERROR << "Could not downcast to RecEmcHitCol" << endreq;
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
    log << MSG::ERROR << "RecEmcHitCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_emcHitCol = recEvt->getEmcHitCol();
  if (!m_emcHitCol) return sc;

  //necessary in case there is I/O at the same time since array is static
  recEvt->clearEmcHitCol(); 

  RecEmcHitCol::const_iterator emcHitTds;

  for(emcHitTds = emcHitColTds->begin();
      emcHitTds != emcHitColTds->end();
      emcHitTds++) {
    Int_t cellId = (*emcHitTds)->getCellId();
    Double_t energy = (*emcHitTds)->getEnergy();
    Double_t time = (*emcHitTds)->getTime();

    TRecEmcHit *emcHitRoot = new TRecEmcHit();
    //m_common.m_recEmcHitMap[(*emcHitTds)] = emcHitRoot;
    emcHitRoot->setCellId(cellId);
    emcHitRoot->setEnergy(energy);
    emcHitRoot->setTime(time);

    recEvt->addEmcHit(emcHitRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
