#ifndef TofDigiCnv_CXX
#define TofDigiCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"   //TDS object
#include "TofRawEvent/TofDigi.h"   //TDS object
#include "RootEventData/TTofDigi.h" // standard root object
#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/TofDigiCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<TofDigiCnv> s_factory;
//const ICnvFactory& TofDigiCnvFactory = s_factory;

TofDigiCnv::TofDigiCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "TofDigiCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_tofDigiCol";
    //declareObject(EventModel::Digi::TofDigiCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_tofDigiCol);
    m_tofDigiCol=0;
}

StatusCode TofDigiCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "TofDigiCnv");
    log << MSG::DEBUG << "TofDigiCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the TofDigi Collection
    TofDigiCol* tofDigiTdsCol = new TofDigiCol;
    refpObject=tofDigiTdsCol;


    // now convert
    if (!m_tofDigiCol) return sc;
    TIter tofDigiIter(m_tofDigiCol);
    TTofDigi *tofDigiRoot = 0;
    while ((tofDigiRoot = (TTofDigi*)tofDigiIter.Next())) {
        unsigned int  id  = tofDigiRoot->getIntId();
	unsigned int  time = tofDigiRoot->getTimeChannel();
	unsigned int  charge = tofDigiRoot->getChargeChannel();
	unsigned int  overflow = tofDigiRoot->getOverflow();
        int trackIndex = tofDigiRoot->getTrackIndex();
	
        TofDigi *tofDigiTds = new TofDigi(id);
        m_common.m_rootTofDigiMap[tofDigiRoot] = tofDigiTds;
	
	tofDigiTds->setTimeChannel(time);
	tofDigiTds->setChargeChannel(charge);
	tofDigiTds->setOverflow(overflow);
        tofDigiTds->setTrackIndex(trackIndex);

        tofDigiTdsCol->push_back(tofDigiTds);
     }
    //m_tofDigiCol->Delete();  // wensp add 2005/12/30
    delete m_tofDigiCol;
    m_tofDigiCol = 0; 
   return StatusCode::SUCCESS;
}

StatusCode TofDigiCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "TofDigiCnv");
  log << MSG::DEBUG << "TofDigiCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  TofDigiCol * tofDigiColTds=dynamic_cast<TofDigiCol *> (obj);
  if (!tofDigiColTds) {
    log << MSG::ERROR << "Could not downcast to TofDigiCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Digi::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get DigiEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  DigiEvent * devtTds=dynamic_cast<DigiEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "TofDigiCnv:Could not downcast to TDS DigiEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDigiCnv()->createRep(evt,addr); 
  
  TDigiEvent *recEvt=m_cnvSvc->getDigiCnv()->getWriteObject();

  const TObjArray *m_tofDigiCol = recEvt->getTofDigiCol();
  if (!m_tofDigiCol) return sc;
  recEvt->clearTofDigiCol(); //necessary in case there is I/O at the same time since array is static
  TofDigiCol::const_iterator tofDigiTds;
  
  for (tofDigiTds = tofDigiColTds->begin(); tofDigiTds != tofDigiColTds->end(); tofDigiTds++) {
    UInt_t overflow  = (*tofDigiTds)->getOverflow();
    UInt_t time      = (*tofDigiTds)->getTimeChannel();
    UInt_t charge    = (*tofDigiTds)->getChargeChannel();
    UInt_t id        = (*tofDigiTds)->getIntId();
    Int_t trackIndex = (*tofDigiTds)->getTrackIndex();
   
    TTofDigi *tofDigiRoot = new TTofDigi();
    //m_common.m_tofDigiMap[(*tofDigiTds)] = tofDigiRoot;

    tofDigiRoot->initialize(id, time ,charge);
    tofDigiRoot->setOverflow(overflow);
    tofDigiRoot->setTrackIndex(trackIndex);

    recEvt->addTofDigi(tofDigiRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










