#ifndef HltInfCnv_CXX
#define HltInfCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "Identifier/HltID.h"
#include "HltEvent/HltEvent.h"   //TDS object
#include "HltEvent/HltInf.h"   //TDS object
#include "RootEventData/THltEvent.h" // standard root object
#include "RootEventData/THltInf.h" // standard root object
#include "RootCnvSvc/Hlt/HltCnv.h"
#include "RootCnvSvc/Hlt/HltInfCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service

HltInfCnv::HltInfCnv(ISvcLocator* svc)
  : RootEventBaseCnv(classID(), svc),
    m_old(false)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "HltInfCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_hltInf";
    //declareObject(EventModel::MC::HltInf, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_hltInf);
    m_hltInf=0;
}

StatusCode HltInfCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

  MsgStream log(msgSvc(), "HltInfCnv");
  log << MSG::DEBUG << "HltInfCnv::TObjectToDataObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;

  // create the TDS location for the trigger data
  HltInf* hltInfTds = new HltInf;
  refpObject=hltInfTds;

  // now convert
  if(!m_hltInf) return sc;

  int evt = m_hltInf->getEventType();
  int alg = m_hltInf->getAlgProcess();
  int cri = m_hltInf->getCriteriaTable();
  int ver = m_hltInf->getVersion();
  float etot = m_hltInf->getTotalEnergy();
  hltInfTds->setEventType(evt);
  hltInfTds->setAlgProcess(alg);
  hltInfTds->setCriteriaTable(cri);
  hltInfTds->setVersion(ver);
  hltInfTds->setTotalEnergy(etot);

  try{
    uint32_t nsub= m_hltInf->getNumber();
    uint32_t ncon= m_hltInf->getNCON();
    uint32_t nmdc= nsub     &0XFF;
    uint32_t ntof=(nsub>>8 )&0XFF;
    uint32_t nemc=(nsub>>16)&0XFF;
    uint32_t nmuc= nsub>>24;
    if(nmuc>HltID::getID_MUC_MAX()+1 || nemc>HltID::getID_EMC_MAX()+1
       ||ntof>HltID::getID_TOF_MAX()+1 || nmdc>HltID::getID_MDC_MAX()+1){
      throw exception();
    }
    hltInfTds->setNumber(nsub);
    hltInfTds->setNCON(ncon);

    std::vector<Int_t>    mdcRoot=m_hltInf->getMdcData();
    if(mdcRoot.size()!=nmdc)  {
      throw exception();
    }
    std::vector<Int_t>::iterator it=mdcRoot.begin();
    for(;it!=mdcRoot.end();it++){
      uint32_t data=*it;
      hltInfTds->push_mdc(data);
    }
    std::vector<Int_t>    tofRoot=m_hltInf->getTofData();
    if(tofRoot.size()!=ntof)  {
      throw exception();
    }
    it=tofRoot.begin();
    for(;it!=tofRoot.end();it++){
      uint32_t data=*it;
      hltInfTds->push_tof(data);
    }
    std::vector<Int_t>    emcRoot=m_hltInf->getEmcData();
    if(emcRoot.size()!=nemc)  {
      throw exception();
    }
    it=emcRoot.begin();
    for(;it!=emcRoot.end();it++){
      uint32_t data=*it;
      hltInfTds->push_emc(data);
    }
    std::vector<Int_t>    mucRoot=m_hltInf->getMucData();
    if(mucRoot.size()!=nmuc)  {
      throw exception();
    }
    it=mucRoot.begin();
    for(;it!=mucRoot.end();it++){
      uint32_t data=*it;
      hltInfTds->push_muc(data);
    }
    std::vector<Int_t>    conRoot=m_hltInf->getConData();
    if(conRoot.size()!=ncon)  {
      throw exception();
    }
    it=conRoot.begin();
    for(;it!=conRoot.end();it++){
      uint32_t data=*it;
      hltInfTds->push_con(data);
    }
  }
  catch(...){
    //log << MSG::ERROR   << "***********************************************************" << endreq;           
    //log << MSG::ERROR   << "Input ROOT object is older version, please reconstrut again" << endreq;           
    //log << MSG::ERROR   << " or check out old RootCnvSvc! (mailto: fucd@ihep.ac.cn)" << endreq;
    //log << MSG::WARNING << "***** Continuing run may cause a little memory leakage ****" << endreq;
    //m_old=true;
  }
  
  delete m_hltInf;
  m_hltInf = 0;
  
  return StatusCode::SUCCESS;
}

StatusCode HltInfCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "HltInfCnv");
  log << MSG::DEBUG << "HltInfCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  HltInf * hltInfCnvTds=dynamic_cast<HltInf*> (obj);
  if (!hltInfCnvTds) {
    log << MSG::ERROR << "Could not downcast to HltInf" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Hlt::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get HltInf in TDS "  << endreq;
    return StatusCode::FAILURE;
  }

  HltEvent * devtTds=dynamic_cast<HltEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "HltInfCnv:Could not downcast to TDS HltInf" << endreq;
  }

  IOpaqueAddress *addr;
  m_cnvSvc->getHltCnv()->createRep(evt,addr); 

  THltEvent *hltInf=m_cnvSvc->getHltCnv()->getWriteObject();

  const TObject *tHltInf = hltInf->getHltInf();
  if (!tHltInf) return sc;
  hltInf->clearHltInf();
  
  Int_t type = hltInfCnvTds->getEventType();
  Int_t alg = hltInfCnvTds->getAlgProcess();
  Int_t cri = hltInfCnvTds->getCriteriaTable();
  Int_t ver = hltInfCnvTds->getVersion();
  Double_t etot = hltInfCnvTds->getTotalEnergy();
  Int_t nsub = hltInfCnvTds->getNumber();
  Int_t ncon = hltInfCnvTds->getNCON();

  THltInf *hltInfRoot = new THltInf();

  hltInfRoot->setEventType(type);
  hltInfRoot->setAlgProcess(alg);
  hltInfRoot->setCriteriaTable(cri);
  hltInfRoot->setVersion(ver);
  hltInfRoot->setTotalEnergy(etot);
  hltInfRoot->setNumber(nsub);
  hltInfRoot->setNCON(ncon);


  std::vector<uint32_t> mdcTds=hltInfCnvTds->getDataVec(HltID::MDC);
  std::vector<Int_t>    mdcRoot;
  std::vector<uint32_t>::iterator it= mdcTds.begin();
  for(;it!=mdcTds.end();it++){
    Int_t data=*it;
    mdcRoot.push_back(data);
  }
  hltInfRoot->setMdcData(mdcRoot);
  
  std::vector<uint32_t> tofTds=hltInfCnvTds->getDataVec(HltID::TOF);
  std::vector<Int_t>    tofRoot;
  for(it= tofTds.begin();it!=tofTds.end();it++){
    Int_t data=*it;
    tofRoot.push_back(data);
  }
  hltInfRoot->setTofData(tofRoot);

  std::vector<uint32_t> emcTds=hltInfCnvTds->getDataVec(HltID::EMC);
  std::vector<Int_t>    emcRoot;
  for(it= emcTds.begin();it!=emcTds.end();it++){
    Int_t data=*it;
    emcRoot.push_back(data);
  }
  hltInfRoot->setEmcData(emcRoot);

  std::vector<uint32_t> mucTds=hltInfCnvTds->getDataVec(HltID::MUC);
  std::vector<Int_t>    mucRoot;
  for(it= mucTds.begin();it!=mucTds.end();it++){
    Int_t data=*it;
    mucRoot.push_back(data);
  }
  hltInfRoot->setMucData(mucRoot);

  std::vector<uint32_t> conTds=hltInfCnvTds->getDataVec(HltID::CON);
  std::vector<Int_t>    conRoot;
  for(it= conTds.begin();it!=conTds.end();it++){
    Int_t data=*it;
    conRoot.push_back(data);
  }
  hltInfRoot->setConData(conRoot);

  hltInf->addHltInf(hltInfRoot);

  return StatusCode::SUCCESS;
}
#endif
