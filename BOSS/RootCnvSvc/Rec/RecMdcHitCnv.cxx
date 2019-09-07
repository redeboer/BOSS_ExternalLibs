#ifndef RecMdcHitCnv_CXX
#define RecMdcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"

#include "ReconEvent/ReconEvent.h"
#include "MdcRecEvent/RecMdcHit.h"
#include "Identifier/Identifier.h"

#include "RootEventData/TRecMdcHit.h" // standard root object
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecMdcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;


RecMdcHitCnv::RecMdcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecMdcHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recMdcHitCol";
    //declareObject(EventModel::Recon::RecMdcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recMdcHitCol);
    m_recMdcHitCol=0;
}

StatusCode RecMdcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
  MsgStream log(msgSvc(), "RecMdcHitCnv");
  log << MSG::DEBUG << "RecMdcHitCnv::TObjectToDataObject" << endreq;

  // create the TDS location for the RecMdcHit Collection
  RecMdcHitCol* recMdcHitCol = new RecMdcHitCol;
  refpObject = recMdcHitCol;

  // now convert
  if (!m_recMdcHitCol) return StatusCode::SUCCESS;
  TIter mdcHitIter(m_recMdcHitCol);
  TRecMdcHit *recMdcHitRoot = 0;
  while ((recMdcHitRoot = (TRecMdcHit*)mdcHitIter.Next())) {
    int      id = recMdcHitRoot->getId();
    int      trackId = recMdcHitRoot->getTrkId();
    double   driftDistLeft = recMdcHitRoot->getDriftDistLeft();
    double   driftDistRight = recMdcHitRoot->getDriftDistRight();
    double   errDriftDistLeft = recMdcHitRoot->getErrDriftDistLeft();
    double   errDriftDistRight = recMdcHitRoot->getErrDriftDistRight();
    double   chisqAdd = recMdcHitRoot->getChisqAdd();
    int      flagLR = recMdcHitRoot->getFlagLR(); 
    int      stat = recMdcHitRoot->getStat(); 
    long int mdcId(recMdcHitRoot->getMdcId()); 
    double   tdc = recMdcHitRoot->getTdc();
    double   adc = recMdcHitRoot->getAdc();
    double   driftT = recMdcHitRoot->getDriftT();
    double   doca = recMdcHitRoot->getDoca();
    double   entra = recMdcHitRoot->getEntra();
    double   zhit = recMdcHitRoot->getZhit();
    double   fltLen = recMdcHitRoot->getFltLen();

    RecMdcHit *recMdcHit= new RecMdcHit();
    m_common.m_rootRecMdcHitMap[recMdcHitRoot] = recMdcHit;

    recMdcHit->setId( id );
    recMdcHit->setTrkId( trackId );
    recMdcHit->setDriftDistLeft( driftDistLeft );
    recMdcHit->setDriftDistRight( driftDistRight );
    recMdcHit->setErrDriftDistLeft( errDriftDistLeft );
    recMdcHit->setErrDriftDistRight( errDriftDistRight );
    recMdcHit->setChisqAdd( chisqAdd );
    recMdcHit->setFlagLR( flagLR );
    recMdcHit->setStat( stat );
    recMdcHit->setMdcId( Identifier(mdcId) );
    recMdcHit->setTdc( tdc );
    recMdcHit->setAdc( adc );
    recMdcHit->setDriftT( driftT );
    recMdcHit->setDoca( doca );
    recMdcHit->setEntra( entra );
    recMdcHit->setZhit( zhit );
    recMdcHit->setFltLen( fltLen );

    recMdcHitCol->push_back(recMdcHit);
  }
    return StatusCode::SUCCESS;
}

StatusCode RecMdcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
 
  MsgStream log(msgSvc(), "RecMdcHitCnv");
  log << MSG::DEBUG << "RecMdcHitCnv::DataObjectToTObject" << endreq;
 
  RecMdcHitCol * mdcHitColTds=dynamic_cast<RecMdcHitCol *> (obj);
  if (!mdcHitColTds) {
    log << MSG::ERROR << "Could not downcast to RecMdcHitCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Recon::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get ReconEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  ReconEvent * devtTds=dynamic_cast<ReconEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "RecMdcHitCnv:Could not downcast to TDS ReconEvent" << endreq;
  }

  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr); 
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();

  const TObjArray *m_recMdcHitCol = recEvt->getRecMdcHitCol();
  if (!m_recMdcHitCol) return StatusCode::SUCCESS;
  recEvt->clearRecMdcHitCol(); //necessary in case there is I/O at the same time since array is static

  RecMdcHitCol::const_iterator recMdcHit;
  for (recMdcHit = mdcHitColTds->begin(); recMdcHit != mdcHitColTds->end(); recMdcHit++) {
     Bool_t     isGrouped =(*recMdcHit)->isGrouped();
     Int_t      id =(*recMdcHit)->getId();
     Int_t      trackId =(*recMdcHit)->getTrkId();
     Double_t   driftDistLeft =(*recMdcHit)->getDriftDistLeft();
     Double_t   driftDistRight =(*recMdcHit)->getDriftDistRight();
     Double_t   errDriftDistLeft =(*recMdcHit)->getErrDriftDistLeft();
     Double_t   errDriftDistRight =(*recMdcHit)->getErrDriftDistRight();
     Double_t   chisqAdd =(*recMdcHit)->getChisqAdd();
     Int_t      flagLR =(*recMdcHit)->getFlagLR(); 
     Int_t      stat =(*recMdcHit)->getStat(); 
     UInt_t     mdcId =(*recMdcHit)->getMdcId().get_value(); 
     Double_t   tdc =(*recMdcHit)->getTdc();
     Double_t   adc =(*recMdcHit)->getAdc();
     Double_t   driftT =(*recMdcHit)->getDriftT();
     Double_t   doca =(*recMdcHit)->getDoca();
     Double_t   entra =(*recMdcHit)->getEntra();
     Double_t   zhit =(*recMdcHit)->getZhit();
     Double_t   fltLen =(*recMdcHit)->getFltLen();

     TRecMdcHit *recMdcHitRoot = new TRecMdcHit();
     //m_common.m_recMdcHitMap[(*recMdcHit)] = recMdcHitRoot;

     recMdcHitRoot->setIsGrouped( isGrouped );
     recMdcHitRoot->setId( id );
     recMdcHitRoot->setTrkId( trackId );
     recMdcHitRoot->setDriftDistLeft( driftDistLeft );
     recMdcHitRoot->setDriftDistRight( driftDistRight );
     recMdcHitRoot->setErrDriftDistLeft( errDriftDistLeft );
     recMdcHitRoot->setErrDriftDistRight( errDriftDistRight );
     log<<MSG::INFO<<" recMdcHitRoot.ddl: "<<recMdcHitRoot->getDriftDistLeft()
                   <<" recMdcHitRoot.erddl: "<<recMdcHitRoot->getErrDriftDistLeft()
	           <<endreq;
     recMdcHitRoot->setChisqAdd( chisqAdd );
     recMdcHitRoot->setFlagLR( flagLR );
     recMdcHitRoot->setStat( stat );
     recMdcHitRoot->setMdcId( mdcId );
     recMdcHitRoot->setTdc( tdc );
     recMdcHitRoot->setAdc( adc );
     recMdcHitRoot->setDriftT( driftT );
     recMdcHitRoot->setDoca( doca );
     recMdcHitRoot->setEntra( entra );
     recMdcHitRoot->setZhit( zhit );
     recMdcHitRoot->setFltLen( fltLen );

     recEvt->addRecMdcHit(recMdcHitRoot);
  }
  return StatusCode::SUCCESS;
}
#endif
