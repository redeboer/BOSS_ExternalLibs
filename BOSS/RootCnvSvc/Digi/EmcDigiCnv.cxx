#ifndef EmcDigiCnv_CXX
#define EmcDigiCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"   //TDS object
#include "EmcRawEvent/EmcDigi.h"   //TDS object
#include "RootEventData/TEmcDigi.h" // standard root object
#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/EmcDigiCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<EmcDigiCnv> s_factory;
//const ICnvFactory& EmcDigiCnvFactory = s_factory;

EmcDigiCnv::EmcDigiCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "EmcDigiCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_emcDigiCol";
    //declareObject(EventModel::Digi::EmcDigiCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_emcDigiCol);
    m_emcDigiCol=0;
}

StatusCode EmcDigiCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "EmcDigiCnv");
    log << MSG::DEBUG << "EmcDigiCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcDigi Collection
    EmcDigiCol* emcDigiTdsCol = new EmcDigiCol;
    refpObject=emcDigiTdsCol;


    // now convert
    if (!m_emcDigiCol) return sc;
    TIter emcDigiIter(m_emcDigiCol);
    TEmcDigi *emcDigiRoot = 0;
    while ((emcDigiRoot = (TEmcDigi*)emcDigiIter.Next())) {
        unsigned int  id  = emcDigiRoot->getIntId();
	unsigned int  time = emcDigiRoot->getTimeChannel();
	unsigned int  charge = emcDigiRoot->getChargeChannel();
	unsigned int  measure = emcDigiRoot->getMeasure();
        int trackIndex = emcDigiRoot->getTrackIndex();
	
        EmcDigi *emcDigiTds = new EmcDigi(id);
        m_common.m_rootEmcDigiMap[emcDigiRoot] = emcDigiTds;
	
	emcDigiTds->setTimeChannel(time);
	emcDigiTds->setChargeChannel(charge);
	emcDigiTds->setMeasure( measure);
        emcDigiTds->setTrackIndex(trackIndex);

        emcDigiTdsCol->push_back(emcDigiTds);
     }
    //m_emcDigiCol->Delete();  // wensp add 2005/12/30
    delete m_emcDigiCol;
    m_emcDigiCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode EmcDigiCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "EmcDigiCnv");
  log << MSG::DEBUG << "EmcDigiCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
 EmcDigiCol * emcDigiColTds=dynamic_cast<EmcDigiCol *> (obj);
  if (!emcDigiColTds) {
    log << MSG::ERROR << "Could not downcast to EmcDigiCol" << endreq;
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
    log << MSG::ERROR << "EmcDigiCnv:Could not downcast to TDS DigiEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDigiCnv()->createRep(evt,addr); 
  TDigiEvent *recEvt=m_cnvSvc->getDigiCnv()->getWriteObject();

  const TObjArray *m_emcDigiCol = recEvt->getEmcDigiCol();
  if (!m_emcDigiCol) return sc;
  recEvt->clearEmcDigiCol(); //necessary in case there is I/O at the same time since array is static
  EmcDigiCol::const_iterator emcDigiTds;

  for (emcDigiTds = emcDigiColTds->begin(); emcDigiTds != emcDigiColTds->end(); emcDigiTds++) {
    UInt_t measure  = (*emcDigiTds)->getMeasure();
    UInt_t time      = (*emcDigiTds)->getTimeChannel();
    UInt_t charge    = (*emcDigiTds)->getChargeChannel();
    UInt_t id        = (*emcDigiTds)->getIntId();
    Int_t trackIndex = (*emcDigiTds)->getTrackIndex();
       
    TEmcDigi *emcDigiRoot = new TEmcDigi();
    //m_common.m_emcDigiMap[(*emcDigiTds)] = emcDigiRoot;

    emcDigiRoot->initialize(id, time ,charge);
    emcDigiRoot->setMeasure(measure);
    emcDigiRoot->setTrackIndex(trackIndex);

    recEvt->addEmcDigi(emcDigiRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










