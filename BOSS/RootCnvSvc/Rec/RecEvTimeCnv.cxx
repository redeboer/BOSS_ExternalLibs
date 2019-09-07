#ifndef RecEvTimeCnv_CXX
#define RecEvTimeCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "EvTimeEvent/RecEsTime.h"
#include "ReconEvent/ReconEvent.h"

//#include "RootEventData/TMucTrack.h" // standard root object
#include "RootEventData/TRecEvTime.h" // standard root object
//#include "RootEventData/TDstEvent.h"
#include "RootEventData/TRecTrackEvent.h"

#include "RootCnvSvc/Rec/RecTrackCnv.h"
#include "RootCnvSvc/Rec/RecEvTimeCnv.h"
#include "RootCnvSvc/RootAddress.h"

#include <vector>

using namespace std;

// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<RecEvTimeCnv> s_factory;
//const ICnvFactory& RecEvTimeCnvFactory = s_factory;

RecEvTimeCnv::RecEvTimeCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{

    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "RecEvTimeCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    //m_rootTreename ="Rec";
    m_rootBranchname ="m_recEvTimeCol";
   // declareObject(EventModel::Recon::EvTimeCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_recEvTimeCol);
    m_recEvTimeCol=0;
}

StatusCode RecEvTimeCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object
MsgStream log(msgSvc(), "RecEvTimeCnv");
    log << MSG::DEBUG << "RecEvTimeCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MucTrack Collection
    RecEsTimeCol* evTimeTdsCol = new RecEsTimeCol;
    refpObject=evTimeTdsCol;
 // now convert
    if (!m_recEvTimeCol) return sc;
    TIter evTimeIter(m_recEvTimeCol);
    TRecEvTime *evTimeRoot = 0;
    while ((evTimeRoot = (TRecEvTime*)evTimeIter.Next())) {
      double estime=evTimeRoot->estime();
      int   status=evTimeRoot->status();
      double quality=evTimeRoot->quality();
      //double estime=evTimeRoot->getTest();
      //int status=evTimeRoot->getStat();
      
      RecEsTime *evTimeTds = new RecEsTime();
      m_common.m_rootRecEvTimeMap[evTimeRoot] = evTimeTds;
      evTimeTds->setTest( estime );
      evTimeTds->setStat( status );
      evTimeTds->setQuality( quality );
      evTimeTdsCol->push_back(evTimeTds);
    }
    delete m_recEvTimeCol;
    m_recEvTimeCol = 0;

    return StatusCode::SUCCESS;
}

StatusCode RecEvTimeCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {
 MsgStream log(msgSvc(), "RecEvTimeCnv");
  log << MSG::DEBUG << "RecEvTimeCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  RecEsTimeCol * evTimeColTds=dynamic_cast<RecEsTimeCol *> (obj);
  if (!evTimeColTds) {
    log << MSG::ERROR << "Could not downcast to RecEsTimeCol" << endreq;
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
    log << MSG::ERROR << "RecEvTimeCnv:Could not downcast to TDS ReconEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getRecTrackCnv()->createRep(evt,addr);
  TRecTrackEvent *recEvt=m_cnvSvc->getRecTrackCnv()->getWriteObject();
  const TObjArray *m_recEvTimeCol = recEvt->getEvTimeCol();
  if (!m_recEvTimeCol) return sc;
  recEvt->clearEvTimeCol(); //necessary in case there is I/O at the same time since array is static
  RecEsTimeCol::const_iterator evTimeTds;
  for (evTimeTds = evTimeColTds->begin(); evTimeTds != evTimeColTds->end(); evTimeTds++) {
    //Double_t estime = (*evTimeTds)->estime();
    //Int_t status = (*evTimeTds)->status();
    Double_t estime = (*evTimeTds)->getTest();
    Int_t    status = (*evTimeTds)->getStat();
    Double_t quality = (*evTimeTds)->getQuality();
    TRecEvTime *evTimeRoot = new TRecEvTime();
    //m_common.m_RecEvTimeMap[(*evTimeTds)] = evTimeRoot;

    evTimeRoot->setTest( estime );
    evTimeRoot->setStats( status );
    evTimeRoot->setQuality( quality );

    recEvt->addEvTime(evTimeRoot);
  }
 
  return StatusCode::SUCCESS;
}
#endif
