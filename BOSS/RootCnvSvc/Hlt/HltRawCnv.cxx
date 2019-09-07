#ifndef HltRawCnv_CXX
#define HltRawCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "HltEvent/HltEvent.h"   //TDS object
#include "HltEvent/HltRaw.h"   //TDS object
#include "RootEventData/THltRaw.h" // standard root object
#include "RootEventData/THltEvent.h"
#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/Hlt/HltRawCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<HltRawCnv> s_factory;
//const ICnvFactory& HltRawCnvFactory = s_factory;

HltRawCnv::HltRawCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "HltRawCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_hltRawCol";
    //declareObject(EventModel::Hlt::HltRawCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_hltRawCol);
    m_hltRawCol=0;

}

StatusCode HltRawCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "HltRawCnv");
    log << MSG::DEBUG << "HltRawCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the HltRaw Collection
    HltRawCol* hltRawTdsCol = new HltRawCol;
    refpObject=hltRawTdsCol;
//     sc = m_eds->registerObject(EventModel::Hlt::HltRawCol, hltRawTdsCol);
//     if (sc.isFailure()) {
//         log << "Failed to register HltRaw Collection" << endreq;
//         return StatusCode::FAILURE;
//     }


    // now convert
    if (!m_hltRawCol) return sc;
    TIter hltRawIter(m_hltRawCol);
    THltRaw *hltRawRoot = 0;
    while ((hltRawRoot = (THltRaw*)hltRawIter.Next())) {
        unsigned int  id  = hltRawRoot->getIntId();
	unsigned int  time = hltRawRoot->getTimeChannel();
	unsigned int  charge = hltRawRoot->getChargeChannel();
	
        HltRaw *hltRawTds = new HltRaw(id);
        m_common.m_rootHltRawMap[hltRawRoot] = hltRawTds;
	
	hltRawTds->setTimeChannel(time);
	hltRawTds->setChargeChannel(charge);

        hltRawTdsCol->push_back(hltRawTds);
     }
    //m_hltRawCol->Delete();  // wensp add 2005/12/30
    delete m_hltRawCol;
    m_hltRawCol = 0; 
   return StatusCode::SUCCESS;
}

StatusCode HltRawCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "HltRawCnv");
  log << MSG::DEBUG << "HltRawCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  HltRawCol * hltRawColTds=dynamic_cast<HltRawCol *> (obj);
  if (!hltRawColTds) {
    log << MSG::ERROR << "Could not downcast to HltRawCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Hlt::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get HltEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  HltEvent * devtTds=dynamic_cast<HltEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "HltRawCnv:Could not downcast to TDS HltEvent" << endreq;
  }

  IOpaqueAddress *addr;

  
  m_cnvSvc->getHltCnv()->createRep(evt,addr);
 
  //createRep(evt,addr);

  THltEvent *recEvt=m_cnvSvc->getHltCnv()->getWriteObject();

  const TObjArray *m_hltRawCol = recEvt->getHltRawCol();
  if (!m_hltRawCol) return sc;
  recEvt->clearHltRawCol(); //necessary in case there is I/O at the same time since array is static
  HltRawCol::const_iterator hltRawTds;

  for (hltRawTds = hltRawColTds->begin(); hltRawTds != hltRawColTds->end(); hltRawTds++) {
    UInt_t time      = (*hltRawTds)->getTimeChannel();
    UInt_t charge    = (*hltRawTds)->getChargeChannel();
    UInt_t id        = (*hltRawTds)->getIntId();

    THltRaw *hltRawRoot = new THltRaw();
    //m_common.m_hltRawMap[(*hltRawTds)] = hltRawRoot;

    hltRawRoot->initialize(id, time ,charge);
    recEvt->addHltRaw(hltRawRoot);
  }

  return StatusCode::SUCCESS;
}

#endif










