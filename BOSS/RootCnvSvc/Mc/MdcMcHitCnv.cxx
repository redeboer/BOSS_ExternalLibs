#ifndef MdcMcHitCnv_CXX
#define MdcMcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"   //TDS object
#include "McTruth/MdcMcHit.h"   //TDS object
#include "RootEventData/TMdcMc.h" // standard root object
#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/MdcMcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<MdcMcHitCnv> s_factory;
//const ICnvFactory& MdcMcHitCnvFactory = s_factory;

MdcMcHitCnv::MdcMcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MdcMcHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mdcMcHitCol";
    //declareObject(EventModel::MC::MdcMcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mdcMcHitCol);
    m_mdcMcHitCol=0;
}

StatusCode MdcMcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MdcMcHitCnv");
    log << MSG::DEBUG << "MdcMcHitCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcMc Collection
    MdcMcHitCol* mdcMcTdsCol = new MdcMcHitCol;
    refpObject=mdcMcTdsCol;


    // now convert
    if (!m_mdcMcHitCol) return sc;
    TIter mdcMcIter(m_mdcMcHitCol);
    TMdcMc *mdcMcRoot = 0;
    while ((mdcMcRoot = (TMdcMc*)mdcMcIter.Next())) {
        unsigned int id = mdcMcRoot ->getId();
		unsigned int trackIndex = mdcMcRoot ->getTrackIndex();
		double xPosition =   mdcMcRoot ->getPositionX() ;
		double yPosition =   mdcMcRoot ->getPositionY() ;
		double zPosition =   mdcMcRoot ->getPositionZ() ;
		double  driftDistance= mdcMcRoot ->getDriftDistance() ;
		double depositEnergy = mdcMcRoot ->getDepositEnergy() ;
	  int positionFlag = mdcMcRoot->getPositionFlag();
      
        	MdcMcHit *mdcMcTds = new MdcMcHit;
		m_common.m_rootMdcMcHitMap[mdcMcRoot] = mdcMcTds;
	
		mdcMcTds->setIdentifier(id);
		mdcMcTds->setTrackIndex(trackIndex);
		mdcMcTds->setPositionX(xPosition);
		mdcMcTds->setPositionY(yPosition);
		mdcMcTds->setPositionZ(zPosition);
		mdcMcTds->setDriftDistance(driftDistance);
		mdcMcTds->setDepositEnergy(depositEnergy);
    mdcMcTds->setPositionFlag(positionFlag);
    
        mdcMcTdsCol->push_back(mdcMcTds);
     }
    //m_mdcMcHitCol->Delete();  // wensp add 2005/12/30
    delete m_mdcMcHitCol;
    m_mdcMcHitCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode MdcMcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MdcMcHitCnv");
  log << MSG::DEBUG << "MdcMcHitCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
 MdcMcHitCol * mdcMcHitCnvTds=dynamic_cast<MdcMcHitCol *> (obj);
  if (!mdcMcHitCnvTds) {
    log << MSG::ERROR << "Could not downcast to MdcMcHitCol" << endreq;
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
    log << MSG::ERROR << "MdcMcHitCnv:Could not downcast to TDS McEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getMcCnv()->createRep(evt,addr); 
  TMcEvent *McEvt=m_cnvSvc->getMcCnv()->getWriteObject();

  const TObjArray *m_mdcMcHitCol = McEvt->getMdcMcHitCol();
  if (!m_mdcMcHitCol) return sc;
  McEvt->clearMdcMcHitCol(); //necessary in case there is I/O at the same time since array is static
  MdcMcHitCol::const_iterator mdcMcTds;

  for (mdcMcTds = mdcMcHitCnvTds->begin(); mdcMcTds != mdcMcHitCnvTds->end(); mdcMcTds++) {
		UInt_t id  = ((*mdcMcTds)->identify()).get_value() ;
		UInt_t trackIndex = (*mdcMcTds) ->getTrackIndex();
		Double_t xPosition =   (*mdcMcTds) ->getPositionX() ;
		Double_t yPosition =   (*mdcMcTds) ->getPositionY() ;
		Double_t zPosition =   (*mdcMcTds) ->getPositionZ() ;
		Double_t driftDistance=   (*mdcMcTds) ->getDriftDistance();
		Double_t depositEnergy = (*mdcMcTds) ->getDepositEnergy() ;
    Int_t positionFlag = (*mdcMcTds) ->getPositionFlag();
    
		TMdcMc *mdcMcRoot = new TMdcMc();
		//m_common.m_mdcMcHitMap[(*mdcMcTds)] = mdcMcRoot;

		mdcMcRoot->setId(id);
		mdcMcRoot->setTrackIndex(trackIndex);
		mdcMcRoot->setPositionX(xPosition);
		mdcMcRoot->setPositionY(yPosition);
		mdcMcRoot->setPositionZ(zPosition);
		mdcMcRoot->setDriftDistance(driftDistance);
		mdcMcRoot->setDepositEnergy(depositEnergy);
    mdcMcRoot->setPositionFlag(positionFlag);

		McEvt->addMdcMc(mdcMcRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










