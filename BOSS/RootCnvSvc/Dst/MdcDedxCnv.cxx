#ifndef DedxCnv_CXX
#define DedxCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "DstEvent/DstEvent.h"   //TDS object
#include "DstEvent/DstMdcDedx.h"   //TDS object
#include "RootEventData/TMdcDedx.h" // standard root object
#include "RootEventData/TDstEvent.h"

#include "RootCnvSvc/Dst/DstCnv.h"
#include "RootCnvSvc/Dst/MdcDedxCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<DedxCnv> s_factory;
//const ICnvFactory& DedxCnvFactory = s_factory;

MdcDedxCnv::MdcDedxCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MdcDedxCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mdcDedxCol";
    //declareObject(EventModel::Dst::DstDedxCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mdcDedxCol);
    m_mdcDedxCol=0;
}

StatusCode MdcDedxCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MdcDedxCnv");
    log << MSG::DEBUG << "MdcDedxCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the Dedx Collection
    DstMdcDedxCol* mdcDedxTdsCol = new DstMdcDedxCol;
    refpObject=mdcDedxTdsCol;


    // now convert
    if (!m_mdcDedxCol) return sc;
    TIter mdcDedxIter(m_mdcDedxCol);
    TMdcDedx *mdcDedxRoot = 0;
    while ((mdcDedxRoot = (TMdcDedx*)mdcDedxIter.Next())) {
     
     int trackId =  mdcDedxRoot->trackId();	    
     int particleId = mdcDedxRoot->particleId(); 
     int status = mdcDedxRoot->status();     
     int truncAlg = mdcDedxRoot->truncAlg();
     /*
     double pb[5];
     for ( int i = 0; i < 5; i++) 
          pb[i] = dedxRoot->prob(i);
     
     double numSigmaE  = dedxRoot->numSigmaE();   
     double numSigmaMu = dedxRoot->numSigmaMu();  
     double numSigmaPi = dedxRoot->numSigmaPi();  
     double numSigmaK = dedxRoot->numSigmaK();   
     double numSigmaP = dedxRoot->numSigmaP();   
     */
     double chi[5];
     chi[0] = mdcDedxRoot->chiE();
     chi[1] = mdcDedxRoot->chiMu();
     chi[2] = mdcDedxRoot->chiPi();
     chi[3] = mdcDedxRoot->chiK();
     chi[4] = mdcDedxRoot->chiP();
     int numGoodHits = mdcDedxRoot->numGoodHits(); 
     int numTotalHits = mdcDedxRoot->numTotalHits();

     double probPH = mdcDedxRoot->probPH();      
     double normPH = mdcDedxRoot->normPH();
     double errorPH = mdcDedxRoot->errorPH();
     double twentyPH = mdcDedxRoot->twentyPH();        
     //double fracErrPH = dedxRoot-> fracErrPH();  
     //double minIronPH = dedxRoot->minIronPH();   
     //double corrPH = dedxRoot->corrPH();   
	
     DstMdcDedx *mdcDedxTds = new DstMdcDedx();
     m_common.m_rootMdcDedxMap[mdcDedxRoot] = mdcDedxTds;

     mdcDedxTds->setTrackId(trackId);
     mdcDedxTds->setParticleId(particleId);
     mdcDedxTds->setStatus (status);
     mdcDedxTds->setTruncAlg(truncAlg);
    /*
     dedxTds->setProb(pb); 
     dedxTds->setNumSigmaE(numSigmaE);       
     dedxTds->setNumSigmaMu(numSigmaMu);     
     dedxTds->setNumSigmaPi(numSigmaPi);     
     dedxTds->setNumSigmaK(numSigmaK);       
     dedxTds->setNumSigmaP(numSigmaP);       
     */
     mdcDedxTds->setChi(chi);
     
     mdcDedxTds->setNumGoodHits( numGoodHits);     
     mdcDedxTds->setNumTotalHits( numTotalHits); 

     mdcDedxTds->setProbPH(probPH);             
     mdcDedxTds->setNormPH(normPH);
     mdcDedxTds->setErrorPH(errorPH);
     mdcDedxTds->setTwentyPH(twentyPH);  
     //dedxTds->setFracErrPH(fracErrPH);       
     //dedxTds->setMinIronPH(minIronPH);       
     //dedxTds->setCorrPH(corrPH); 
     
     mdcDedxTdsCol->push_back(mdcDedxTds); 
     //delete dedxTds;
     // dedxTds = NULL;
     }
    //m_dedxCol->Delete();
    delete m_mdcDedxCol;
    m_mdcDedxCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode MdcDedxCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MdcDedxCnv");
  log << MSG::DEBUG << "MdcDedxCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  DstMdcDedxCol * mdcDedxColTds=dynamic_cast<DstMdcDedxCol *> (obj);
  if (!mdcDedxColTds) {
    log << MSG::ERROR << "Could not downcast to MdcDedxCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::Dst::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get DstEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  DstEvent * devtTds=dynamic_cast<DstEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "MdcDedxCnv:Could not downcast to TDS DstEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getDstCnv()->createRep(evt,addr); 
  TDstEvent *recEvt=m_cnvSvc->getDstCnv()->getWriteObject();

  const TObjArray *m_mdcDedxCol = recEvt->getMdcDedxCol();
  if (!m_mdcDedxCol) return sc;
  recEvt->clearMdcDedxCol(); //necessary in case there is I/O at the same time since array is static
  DstMdcDedxCol::const_iterator mdcDedxTds;

  for (mdcDedxTds = mdcDedxColTds->begin(); mdcDedxTds != mdcDedxColTds->end(); mdcDedxTds++) {
    Int_t trackId = (*mdcDedxTds)->trackId();
    Int_t particleId = (*mdcDedxTds)->particleId(); 
    Int_t status = (*mdcDedxTds)->status();     
    Int_t truncAlg =(*mdcDedxTds)->truncAlg();  
    /*
    Double_t pb[5];
    for (Int_t i = 0; i < 5; i++) 
         pb[i] = (*dedxTds)->prob(i);
    */
    Double_t chiE  = (*mdcDedxTds)->chi(0);   
    Double_t chiMu = (*mdcDedxTds)->chi(1);  
    Double_t chiPi = (*mdcDedxTds)->chi(2);  
    Double_t chiK = (*mdcDedxTds)->chi(3);   
    Double_t chiP = (*mdcDedxTds)->chi(4);   
  
    Int_t numGoodHits = (*mdcDedxTds)->numGoodHits(); 
    Int_t numTotalHits = (*mdcDedxTds)->numTotalHits();

    Double_t probPH = (*mdcDedxTds)->probPH();      
    Double_t normPH = (*mdcDedxTds)->normPH();
    Double_t errorPH = (*mdcDedxTds)->errorPH();
    Double_t twentyPH =(*mdcDedxTds)->twentyPH();
    //Double_t fracErrPH = (*dedxTds)-> fracErrPH();  
    //Double_t minIronPH = (*dedxTds)->minIronPH();   
    //Double_t corrPH = (*dedxTds)->corrPH();      
    log << MSG::INFO <<"check Reconstrunction root"<<" particle Id is : "<<particleId
         <<"track id is : "<<trackId
         <<" and status is : "<<status<<endreq;

       
    TMdcDedx *mdcDedxRoot = new TMdcDedx();
    //m_common.m_mdcDedxMap[(*mdcDedxTds)] = mdcDedxRoot;
  
    mdcDedxRoot->setTrackId(trackId);
    mdcDedxRoot->setParticleId(particleId);  
    mdcDedxRoot->setStatus (status); 
    mdcDedxRoot->setTruncAlg(truncAlg);
    //dedxRoot->setProb(pb); 
    mdcDedxRoot->setChiE(chiE);       
    mdcDedxRoot->setChiMu(chiMu);     
    mdcDedxRoot->setChiPi(chiPi);     
    mdcDedxRoot->setChiK(chiK);       
    mdcDedxRoot->setChiP(chiP);       

    mdcDedxRoot->setNumGoodHits( numGoodHits);     
    mdcDedxRoot->setNumTotalHits( numTotalHits); 

    mdcDedxRoot->setProbPH(probPH);             
    mdcDedxRoot->setNormPH(normPH);
    mdcDedxRoot->setErrorPH(errorPH);
    mdcDedxRoot->setTwentyPH(twentyPH);  
    // dedxRoot->setFracErrPH(fracErrPH);       
    //dedxRoot->setMinIronPH(minIronPH);       
    //dedxRoot->setCorrPH(corrPH);             
    log << MSG::INFO << "check Reconstrunction root"<<" particle Id is : "<<particleId
        <<"track id is : "<<trackId
        <<" and status is : "<<status<<endreq;
    
    
    
    recEvt->addMdcDedx(mdcDedxRoot);
  }

  return StatusCode::SUCCESS;
}
#endif
