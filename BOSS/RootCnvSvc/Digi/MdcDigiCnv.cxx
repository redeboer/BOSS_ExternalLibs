#ifndef MdcDigiCnv_CXX
#define MdcDigiCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"   //TDS object
#include "MdcRawEvent/MdcDigi.h"   //TDS object
#include "RootEventData/TMdcDigi.h" // standard root object
#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/MdcDigiCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<MdcDigiCnv> s_factory;
//const ICnvFactory& MdcDigiCnvFactory = s_factory;

MdcDigiCnv::MdcDigiCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MdcDigiCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mdcDigiCol";
    //declareObject(EventModel::Digi::MdcDigiCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mdcDigiCol);
    m_mdcDigiCol=0;

}

StatusCode MdcDigiCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MdcDigiCnv");
    log << MSG::DEBUG << "MdcDigiCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcDigi Collection
    MdcDigiCol* mdcDigiTdsCol = new MdcDigiCol;
    refpObject=mdcDigiTdsCol;
//     sc = m_eds->registerObject(EventModel::Digi::MdcDigiCol, mdcDigiTdsCol);
//     if (sc.isFailure()) {
//         log << "Failed to register MdcDigi Collection" << endreq;
//         return StatusCode::FAILURE;
//     }


    // now convert
    if (!m_mdcDigiCol) return sc;
    TIter mdcDigiIter(m_mdcDigiCol);
    TMdcDigi *mdcDigiRoot = 0;
    while ((mdcDigiRoot = (TMdcDigi*)mdcDigiIter.Next())) {
        unsigned int  id  = mdcDigiRoot->getIntId();
	unsigned int  time = mdcDigiRoot->getTimeChannel();
	unsigned int  charge = mdcDigiRoot->getChargeChannel();
	unsigned int  overflow = mdcDigiRoot->getOverflow();
	int trackIndex = mdcDigiRoot->getTrackIndex();
	
        MdcDigi *mdcDigiTds = new MdcDigi(id);
        m_common.m_rootMdcDigiMap[mdcDigiRoot] = mdcDigiTds;
	
	mdcDigiTds->setTimeChannel(time);
	mdcDigiTds->setChargeChannel(charge);
	mdcDigiTds->setOverflow(overflow);
        mdcDigiTds->setTrackIndex(trackIndex);

        mdcDigiTdsCol->push_back(mdcDigiTds);
     }
    //m_mdcDigiCol->Delete();  // wensp add 2005/12/30
    delete m_mdcDigiCol;
    m_mdcDigiCol = 0; 
   return StatusCode::SUCCESS;
}

StatusCode MdcDigiCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MdcDigiCnv");
  log << MSG::DEBUG << "MdcDigiCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  MdcDigiCol * mdcDigiColTds=dynamic_cast<MdcDigiCol *> (obj);
  if (!mdcDigiColTds) {
    log << MSG::ERROR << "Could not downcast to MdcDigiCol" << endreq;
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
    log << MSG::ERROR << "MdcDigiCnv:Could not downcast to TDS DigiEvent" << endreq;
  }

  IOpaqueAddress *addr;

  
  m_cnvSvc->getDigiCnv()->createRep(evt,addr);
 
  //createRep(evt,addr);

  TDigiEvent *recEvt=m_cnvSvc->getDigiCnv()->getWriteObject();

  const TObjArray *m_mdcDigiCol = recEvt->getMdcDigiCol();
  if (!m_mdcDigiCol) return sc;
  recEvt->clearMdcDigiCol(); //necessary in case there is I/O at the same time since array is static
  MdcDigiCol::const_iterator mdcDigiTds;

  for (mdcDigiTds = mdcDigiColTds->begin(); mdcDigiTds != mdcDigiColTds->end(); mdcDigiTds++) {
    UInt_t overflow  = (*mdcDigiTds)->getOverflow();
    UInt_t time      = (*mdcDigiTds)->getTimeChannel();
    UInt_t charge    = (*mdcDigiTds)->getChargeChannel();
    UInt_t id        = (*mdcDigiTds)->getIntId();
    Int_t trackIndex = (*mdcDigiTds)->getTrackIndex();
       
    TMdcDigi *mdcDigiRoot = new TMdcDigi();
    //m_common.m_mdcDigiMap[(*mdcDigiTds)] = mdcDigiRoot;

    mdcDigiRoot->initialize(id, time ,charge);
    mdcDigiRoot->setOverflow(overflow);
    mdcDigiRoot->setTrackIndex(trackIndex);
    recEvt->addMdcDigi(mdcDigiRoot);
  }

  return StatusCode::SUCCESS;
}

#endif










