#ifndef McParticleCnv_CXX
#define McParticleCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"   //TDS object
#include "McTruth/McParticle.h"   //TDS object
#include "RootEventData/TMcParticle.h" // standard root object
#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/McParticleCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<McParticleCnv> s_factory;
//const ICnvFactory& McParticleCnvFactory = s_factory;

McParticleCnv::McParticleCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "McParticleCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mcParticleCol";
    //declareObject(EventModel::MC::McParticleCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mcParticleCol);
    m_mcParticleCol=0;
}

StatusCode McParticleCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "McParticleCnv");
    log << MSG::DEBUG << "McParticleCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcMc Collection
    McParticleCol* mcParticleTdsCol = new McParticleCol;
    refpObject=mcParticleTdsCol;


    // now convert
    if (!m_mcParticleCol) return sc;
    TIter mcParticleIter(m_mcParticleCol);
    TMcParticle *mcParticleRoot = 0;
    
    vector<int> mothers;
    // vector<vector<int> > daughtList;

    while ((mcParticleRoot = (TMcParticle*)mcParticleIter.Next())) {
        unsigned int particleID = mcParticleRoot ->getParticleID();
	unsigned int trackIndex = mcParticleRoot ->getTrackIndex();

	int vertexIndex0 = mcParticleRoot ->getVertexIndex0();
	int vertexIndex1 = mcParticleRoot ->getVertexIndex1();
	unsigned int  statusFlags = mcParticleRoot ->getStatusFlags();
	
	double xInitialPosition = mcParticleRoot->getInitialPositionX();
	double yInitialPosition = mcParticleRoot->getInitialPositionY();
	double zInitialPosition = mcParticleRoot->getInitialPositionZ();
	double tInitialPosition = mcParticleRoot->getInitialPositionT();
	
	double xFinalPosition =   mcParticleRoot->getFinalPositionX();
	double yFinalPosition =   mcParticleRoot->getFinalPositionY();
	double zFinalPosition =   mcParticleRoot->getFinalPositionZ();
	double tFinalPosition =   mcParticleRoot->getFinalPositionT();
	
	double xInitialMomentum = mcParticleRoot->getInitialMomentumX();
	double yInitialMomentum = mcParticleRoot->getInitialMomentumY();
	double zInitialMomentum = mcParticleRoot->getInitialMomentumZ();
	double eInitialMomentum = mcParticleRoot->getInitialMomentumE();

	//double xFinalMomentum =   mcParticleRoot->getFinalMomentumX();
	//double yFinalMomentum =   mcParticleRoot->getFinalMomentumY();
	//double zFinalMomentum =   mcParticleRoot->getFinalMomentumZ();
	//double eFinalMomentum =   mcParticleRoot->getFinalMomentumE();

	HepLorentzVector initialMomentum(xInitialMomentum, yInitialMomentum, zInitialMomentum, eInitialMomentum);
	HepLorentzVector initialPosition(xInitialPosition, yInitialPosition, zInitialPosition, tInitialPosition);
	//HepLorentzVector finalMomentum(xFinalMomentum, yFinalMomentum, zFinalMomentum, eFinalMomentum);
	HepLorentzVector finalPosition(xFinalPosition, yFinalPosition, zFinalPosition, tFinalPosition);
	
        
	int mother =  mcParticleRoot->getMother();
	vector<int> daughters = mcParticleRoot->getDaughters();

        mothers.push_back(mother);
	//daughtList.push_back(daughters);
	
        McParticle *mcParticleTds = new McParticle;
        m_common.m_rootMcParticleMap[mcParticleRoot] = mcParticleTds;
	
	
	mcParticleTds->initialize(particleID, statusFlags, initialMomentum, initialPosition);
	mcParticleTds->setTrackIndex(trackIndex);
	mcParticleTds->addStatusFlag(statusFlags);

	mcParticleTds->setVertexIndex0(vertexIndex0);
	mcParticleTds->setVertexIndex1(vertexIndex1);
	//mcParticleTds->finalize(finalMomentum, finalPosition);
  mcParticleTds->finalize(finalPosition);

        mcParticleTdsCol->push_back(mcParticleTds);
     }
        
    //Set Mother and DaughterList 
    McParticleCol::iterator iter;
    int i =0;
    for (iter = mcParticleTdsCol->begin(); iter != mcParticleTdsCol->end(); iter++,i++){
      //cout<<"    *****   mothers[ "<<i<<"] = "<<mothers[i]<<endl;
      if(mothers[i] != -99 ){
	      McParticleCol::iterator mcParticleTds;
        for (mcParticleTds = mcParticleTdsCol->begin(); mcParticleTds != mcParticleTdsCol->end(); mcParticleTds++) {
	        int trackIndex = (*mcParticleTds)->trackIndex();
	        if( trackIndex == mothers[i] ){ 
             (*iter)->setMother(*mcParticleTds);
	           (*mcParticleTds)->addDaughter(*iter);
	           break;
          }
	     	}
      }
      else{
        (*iter)->setMother(*iter);
      }
    }
    //m_mcParticleCol->Delete();  // wensp add 2005/12/30
    delete m_mcParticleCol;
    m_mcParticleCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode McParticleCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "McParticleCnv");
  log << MSG::DEBUG << "McParticleCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
  McParticleCol *mcParticleCnvTds=dynamic_cast<McParticleCol *> (obj);
  if (!mcParticleCnvTds) {
    log << MSG::ERROR << "Could not downcast to McParticleCol" << endreq;
    return StatusCode::FAILURE;
  }
 
  DataObject *evt;
  m_eds->findObject(EventModel::MC::Event,evt);
  if (evt==NULL) {
    log << MSG::ERROR << "Could not get McEvent in TDS "  << endreq;
    return StatusCode::FAILURE;
  }
  McEvent * devtTds=dynamic_cast<McEvent *> (evt);
  if (!devtTds) {
    log << MSG::ERROR << "McParticleCnv:Could not downcast to TDS McEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getMcCnv()->createRep(evt,addr); 
  TMcEvent *McEvt=m_cnvSvc->getMcCnv()->getWriteObject();

  const TObjArray *m_mcParticleCol = McEvt->getMcParticleCol();
  if (!m_mcParticleCol) { return sc; }
  McEvt->clearMcParticleCol(); //necessary in case there is I/O at the same time since array is static
  
  McParticleCol::const_iterator mcParticleTds;

  for (mcParticleTds = mcParticleCnvTds->begin(); mcParticleTds != mcParticleCnvTds->end(); mcParticleTds++) {
    Int_t particleID  = (*mcParticleTds)->particleProperty();
    Int_t trackIndex  = (*mcParticleTds)->trackIndex();

    Int_t vertexIndex0 = (*mcParticleTds)->vertexIndex0();
    Int_t vertexIndex1 = (*mcParticleTds)->vertexIndex1();
    Int_t statusFlags = (*mcParticleTds)->statusFlags();

    HepLorentzVector initialPosition = (*mcParticleTds)->initialPosition();
    HepLorentzVector finalPosition   = (*mcParticleTds)->finalPosition();
    HepLorentzVector initialFourMomentum = (*mcParticleTds)->initialFourMomentum();
    //HepLorentzVector finalFourMomentum   = (*mcParticleTds)->finalFourMomentum();

    //cout << " HepID " << (*mcParticleTds)->particleProperty() << endl;
    //cout << " init pos " << initialPosition.x() << "  " << initialPosition.y() << "  " << initialPosition.z() << endl;
    Int_t mother = -99;
    if(!(*mcParticleTds)->primaryParticle()) mother = ((*mcParticleTds)->mother()).trackIndex();
 
   vector<Int_t> daughters;
    SmartRefVector<McParticle> daughterList = (*mcParticleTds)->daughterList();
    //SmartRefVector<McParticle>::iterator iter; 
    for (int iPar = 0; iPar < daughterList.size(); iPar++) {
      //cout <<"daughter Index   " <<daughterList[iPar]->getTrackIndex()<<endl; 
      daughters.push_back(daughterList[iPar]->trackIndex());
    }
    /*    
    for (int idau = 0; idau<daughters.size();idau++){
      cout <<"daughter Index      " <<daughters[idau]<<endl;
    }
    
    cout<<"###############################"<<endl;
    */
    TMcParticle *mcParticleRoot = new TMcParticle();
    //m_common.m_mcParticleMap[(*mcParticleTds)] = mcParticleRoot;

    mcParticleRoot->setParticleID(particleID);
    mcParticleRoot->setTrackIndex(trackIndex);

    mcParticleRoot->setVertexIndex0(vertexIndex0);
    mcParticleRoot->setVertexIndex1(vertexIndex1);

    mcParticleRoot->setStatusFlags(statusFlags);

    mcParticleRoot->setInitialPositionX(initialPosition.x());
    mcParticleRoot->setInitialPositionY(initialPosition.y());
    mcParticleRoot->setInitialPositionZ(initialPosition.z());
    mcParticleRoot->setInitialPositionT(initialPosition.t());

    mcParticleRoot->setFinalPositionX(finalPosition.x());
    mcParticleRoot->setFinalPositionY(finalPosition.y());
    mcParticleRoot->setFinalPositionZ(finalPosition.z());
    mcParticleRoot->setFinalPositionT(finalPosition.t());

    mcParticleRoot->setInitialMomentumX(initialFourMomentum.x());
    mcParticleRoot->setInitialMomentumY(initialFourMomentum.y());
    mcParticleRoot->setInitialMomentumZ(initialFourMomentum.z());
    mcParticleRoot->setInitialMomentumE(initialFourMomentum.e());

    //mcParticleRoot->setFinalMomentumX(finalFourMomentum.x());
    //mcParticleRoot->setFinalMomentumY(finalFourMomentum.y());
    //mcParticleRoot->setFinalMomentumZ(finalFourMomentum.z());
    //mcParticleRoot->setFinalMomentumE(finalFourMomentum.e());

    mcParticleRoot->setMother(mother);
    mcParticleRoot->setDaughters(daughters);
    /*
    vector<int> dau = mcParticleRoot->getDaughters();       

    for (int idau = 0; idau<daughters.size();idau++){
      cout <<"daughter Index root      " <<dau[idau]<<endl;
    } 
    */
    
    McEvt->addMcParticle(mcParticleRoot);
  }
  return StatusCode::SUCCESS;
}
#endif

