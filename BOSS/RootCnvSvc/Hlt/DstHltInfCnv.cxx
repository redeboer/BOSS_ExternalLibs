#ifndef DstHltInfCnv_CXX
#define DstHltInfCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "Identifier/HltID.h"
#include "HltEvent/HltEvent.h"   //TDS object
#include "HltEvent/DstHltInf.h"   //TDS object
#include "RootEventData/THltEvent.h" // standard root object
#include "RootEventData/TDstHltInf.h" // standard root object
#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/Hlt/DstHltInfCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service

DstHltInfCnv::DstHltInfCnv(ISvcLocator* svc)
  : RootEventBaseCnv(classID(), svc),
    m_old(false)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "DstHltInfCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_dstHltInf";
    //declareObject(EventModel::MC::DstHltInf, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_dstHltInf);
    m_dstHltInf=0;
}

StatusCode DstHltInfCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

  MsgStream log(msgSvc(), "DstHltInfCnv");
  log << MSG::DEBUG << "DstHltInfCnv::TObjectToDataObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  // create the TDS location for the trigger data
  DstHltInf* dstHltInfTds = new DstHltInf;
  refpObject=dstHltInfTds;

  // now convert
  if(!m_dstHltInf) return sc;

  int evt = m_dstHltInf->getEventType();
  int alg = m_dstHltInf->getAlgProcess();
  int cri = m_dstHltInf->getCriteriaTable();
  int ver = m_dstHltInf->getVersion();
  float etot = m_dstHltInf->getTotalEnergy();
  
  dstHltInfTds->setEventType(evt);
  dstHltInfTds->setAlgProcess(alg);
  dstHltInfTds->setCriteriaTable(cri);
  dstHltInfTds->setVersion(ver);
  dstHltInfTds->setTotalEnergy(etot);
  
  try{
    uint32_t nsub= m_dstHltInf->getNumber();
    uint32_t ncon= m_dstHltInf->getNCON();
    uint32_t nmdc= nsub     &0XFF;
    uint32_t ntof=(nsub>>8 )&0XFF;
    uint32_t nemc=(nsub>>16)&0XFF;
    uint32_t nmuc= nsub>>24;
    if(nmuc>HltID::getID_MUC_MAX()+1 || nemc>HltID::getID_EMC_MAX()+1
       ||ntof>HltID::getID_TOF_MAX()+1 || nmdc>HltID::getID_MDC_MAX()+1){
      throw exception();
    }    
    dstHltInfTds->setNumber(nsub);
    dstHltInfTds->setNCON(ncon);
  }
  catch(...){
    //log << MSG::ERROR   << "***********************************************************" << endreq;
    //log << MSG::ERROR   << "Input ROOT object is older version, please reconstrut again" << endreq;
    //log << MSG::ERROR   << " or check out old RootCnvSvc! (mailto: fucd@ihep.ac.cn)" << endreq;
    //log << MSG::WARNING << "***** Continuing run may cause a little memory leakage ****" << endreq;
    //m_old=true;
  }
  
  delete m_dstHltInf;
  m_dstHltInf = 0;
  
  return StatusCode::SUCCESS;
}

StatusCode DstHltInfCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "DstHltInfCnv");
  log << MSG::DEBUG << "DstHltInfCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstHltInf * dstHltInfCnvTds=dynamic_cast<DstHltInf*> (obj);
  if (!dstHltInfCnvTds) {
    log << MSG::ERROR << "Could not downcast to DstHltInf" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Hlt::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get DstHltInf in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  HltEvent* devtTds=dynamic_cast<HltEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "DstHltInfCnv:Could not downcast to TDS DstHltInf" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getHltCnv()->createRep(evt,addr); 

  THltEvent *hltInf=m_cnvSvc->getHltCnv()->getWriteObject();

  const TObject *tHltInf = hltInf->getDstHltInf();
  if (!tHltInf) return sc;
  hltInf->clearDstHltInf();

  Int_t typ = dstHltInfCnvTds->getEventType();
  Int_t alg = dstHltInfCnvTds->getAlgProcess();
  Int_t cri = dstHltInfCnvTds->getCriteriaTable();
  Int_t ver = dstHltInfCnvTds->getVersion();
  Double_t etot = dstHltInfCnvTds->getTotalEnergy();
  Int_t nsub = dstHltInfCnvTds->getNumber();
  Int_t ncon = dstHltInfCnvTds->getNCON();

  TDstHltInf *dstHltInfRoot = new TDstHltInf();

  dstHltInfRoot->setEventType(typ);
  dstHltInfRoot->setAlgProcess(alg);
  dstHltInfRoot->setCriteriaTable(cri);
  dstHltInfRoot->setVersion(ver);
  dstHltInfRoot->setTotalEnergy(etot);
  dstHltInfRoot->setNumber(nsub);
  dstHltInfRoot->setNCON(ncon);

  hltInf->addDstHltInf(dstHltInfRoot);

  return StatusCode::SUCCESS;
}
#endif
