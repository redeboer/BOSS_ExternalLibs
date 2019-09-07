#ifndef MucDigiCnv_CXX
#define MucDigiCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "RawEvent/DigiEvent.h"   //TDS object
#include "MucRawEvent/MucDigi.h"   //TDS object
#include "RootEventData/TMucDigi.h" // standard root object
#include "RootEventData/TDigiEvent.h"
#include "RootCnvSvc/Digi/DigiCnv.h"
#include "RootCnvSvc/Digi/MucDigiCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<MucDigiCnv> s_factory;
//const ICnvFactory& MucDigiCnvFactory = s_factory;

MucDigiCnv::MucDigiCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MucDigiCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mucDigiCol";
    //declareObject(EventModel::Digi::MucDigiCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mucDigiCol);
    m_mucDigiCol=0;

}

StatusCode MucDigiCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MucDigiCnv");
    log << MSG::DEBUG << "MucDigiCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MucDigi Collection
    MucDigiCol* mucDigiTdsCol = new MucDigiCol;
    refpObject=mucDigiTdsCol;


    // now convert
    if (!m_mucDigiCol) return sc;
    TIter mucDigiIter(m_mucDigiCol);
    TMucDigi *mucDigiRoot = 0;
    while ((mucDigiRoot = (TMucDigi*)mucDigiIter.Next())) {
        unsigned int  id  = mucDigiRoot->getIntId();
	unsigned int  time = mucDigiRoot->getTimeChannel();
	unsigned int  charge = mucDigiRoot->getChargeChannel();
        int trackIndex = mucDigiRoot->getTrackIndex();
	
        MucDigi *mucDigiTds = new MucDigi(id);
        m_common.m_rootMucDigiMap[mucDigiRoot] = mucDigiTds;
	
	mucDigiTds->setTimeChannel(time);
	mucDigiTds->setChargeChannel(charge);
        mucDigiTds->setTrackIndex(trackIndex);
        mucDigiTdsCol->push_back(mucDigiTds);
     }
    //m_mucDigiCol->Delete();  // wensp add 2005/12/30
    delete m_mucDigiCol;
    m_mucDigiCol = 0; 
   return StatusCode::SUCCESS;
}

StatusCode MucDigiCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MucDigiCnv");
  log << MSG::DEBUG << "MucDigiCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  MucDigiCol * mucDigiColTds=dynamic_cast<MucDigiCol *> (obj);
  if (!mucDigiColTds) {
    log << MSG::ERROR << "Could not downcast to MucDigiCol" << endreq;
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
    log << MSG::ERROR << "MucDigiCnv:Could not downcast to TDS DigiEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDigiCnv()->createRep(evt,addr); 
  TDigiEvent *recEvt=m_cnvSvc->getDigiCnv()->getWriteObject();

  const TObjArray *m_mucDigiCol = recEvt->getMucDigiCol();
  if (!m_mucDigiCol) return sc;
  recEvt->clearMucDigiCol(); //necessary in case there is I/O at the same time since array is static
  MucDigiCol::const_iterator mucDigiTds;

  for (mucDigiTds = mucDigiColTds->begin(); mucDigiTds != mucDigiColTds->end(); mucDigiTds++) {
    UInt_t time      = (*mucDigiTds)->getTimeChannel();
    UInt_t charge    = (*mucDigiTds)->getChargeChannel();
    UInt_t id        = (*mucDigiTds)->getIntId();
    Int_t trackIndex = (*mucDigiTds)->getTrackIndex();
       
    TMucDigi *mucDigiRoot = new TMucDigi();
    //m_common.m_mucDigiMap[(*mucDigiTds)] = mucDigiRoot;

    mucDigiRoot->initialize(id, time ,charge);
    mucDigiRoot->setTrackIndex(trackIndex);
    recEvt->addMucDigi(mucDigiRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










