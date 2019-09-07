#ifndef TrigDataCnv_CXX
#define TrigDataCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "TrigEvent/TrigEvent.h"   //TDS object
#include "TrigEvent/TrigData.h"   //TDS object
#include "RootEventData/TTrigData.h" // standard root object
#include "RootEventData/TTrigEvent.h"
#include "RootCnvSvc/Trig/TrigCnv.h"
#include "RootCnvSvc/Trig/TrigDataCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service

TrigDataCnv::TrigDataCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "TrigDataCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_trigData";
    //declareObject(EventModel::MC::TrigDataCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_trigData);
    m_trigData=0;
}

StatusCode TrigDataCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "TrigDataCnv");
    log << MSG::DEBUG << "TrigDataCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the trigger data
    TrigData* trigDataTds = new TrigData;
    refpObject=trigDataTds;


    // now convert
    if(!m_trigData) return sc;

    const int* trigCond = m_trigData->getTrigCondition();
    const int* trigChan = m_trigData->getTrigChannel();
    int timewindow = m_trigData->getTimeWindow();
    int timing = m_trigData->getTimingType();
    bool prescale = m_trigData->getPreScale();
	
    trigDataTds->setTrigCondition(trigCond);
    trigDataTds->setTrigChannel(trigChan);
    trigDataTds->setTimeWindow(timewindow);
    trigDataTds->setTimingType(timing);
    trigDataTds->setPreScale(prescale);

    delete m_trigData;
    m_trigData = 0;

   return StatusCode::SUCCESS;
}

StatusCode TrigDataCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "TrigDataCnv");
  log << MSG::DEBUG << "TrigDataCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
 TrigData * trigDataCnvTds=dynamic_cast<TrigData*> (obj);
  if (!trigDataCnvTds) {
    log << MSG::ERROR << "Could not downcast to TrigData" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Trig::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get TrigEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  TrigEvent * devtTds=dynamic_cast<TrigEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "TrigDataCnv:Could not downcast to TDS TrigEvent" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getTrigCnv()->createRep(evt,addr); 

  TTrigEvent *TrigEvt=m_cnvSvc->getTrigCnv()->getWriteObject();

  const TObject *ttrigData = TrigEvt->getTrigData();
  if (!ttrigData) return sc;
  
  const int* trigCond = trigDataCnvTds->getTrigCondition();
  const int* trigChan = trigDataCnvTds->getTrigChannel();
  int timewindow = trigDataCnvTds->getTimeWindow();
  int timing = trigDataCnvTds->getTimingType();
  bool prescale = trigDataCnvTds->getPreScale();
       
  TTrigData *trigDataRoot = new TTrigData();
  
  trigDataRoot->setTrigCondition(trigCond);
  trigDataRoot->setTrigChannel(trigChan);
  trigDataRoot->setTimeWindow(timewindow);
  trigDataRoot->setTimingType(timing);
  trigDataRoot->setPreScale(prescale);

  TrigEvt->addTrigData(trigDataRoot);

  return StatusCode::SUCCESS;
}
#endif
