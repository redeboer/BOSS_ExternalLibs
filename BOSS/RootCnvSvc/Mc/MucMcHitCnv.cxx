#ifndef MucMcHitCnv_CXX
#define MucMcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"   //TDS object
#include "McTruth/MucMcHit.h"   //TDS object
#include "RootEventData/TMucMc.h" // standard root object
#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/MucMcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<MucMcHitCnv> s_factory;
//const ICnvFactory& MucMcHitCnvFactory = s_factory;

MucMcHitCnv::MucMcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "MucMcHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_mucMcHitCol";
    //declareObject(EventModel::MC::MucMcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_mucMcHitCol);
    m_mucMcHitCol=0;
}

StatusCode MucMcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "MucMcHitCnv");
    log << MSG::DEBUG << "MucMcHitCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcMc Collection
    MucMcHitCol* mucMcTdsCol = new MucMcHitCol;
    refpObject=mucMcTdsCol;


    // now convert
    if (!m_mucMcHitCol) return sc;
    TIter mucMcIter(m_mucMcHitCol);
    TMucMc *mucMcRoot = 0;
    while ((mucMcRoot = (TMucMc*)mucMcIter.Next())) {
        unsigned int id = mucMcRoot ->getId();
		unsigned int trackIndex = mucMcRoot ->getTrackIndex();
		double xPosition =   mucMcRoot ->getPositionX() ;
		double yPosition =   mucMcRoot ->getPositionY() ;
		double zPosition =   mucMcRoot ->getPositionZ() ;
		double px=   mucMcRoot ->getPx() ;
		double py =   mucMcRoot ->getPy() ;
		double pz =   mucMcRoot ->getPz() ;
	
         	MucMcHit *mucMcTds = new MucMcHit;
		m_common.m_rootMucMcHitMap[mucMcRoot] = mucMcTds;
	
		mucMcTds->setIdentifier(id);
		mucMcTds->setTrackIndex(trackIndex);
		mucMcTds->setPositionX(xPosition);
		mucMcTds->setPositionY(yPosition);
		mucMcTds->setPositionZ(zPosition);
		mucMcTds->setPx(px);
		mucMcTds->setPy(py);
		mucMcTds->setPz(pz);

        mucMcTdsCol->push_back(mucMcTds);
     }
    //m_mucMcHitCol->Delete();  // wensp add 2005/12/30
    delete m_mucMcHitCol;
    m_mucMcHitCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode MucMcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "MucMcHitCnv");
  log << MSG::DEBUG << "MucMcHitCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
 MucMcHitCol * mucMcHitCnvTds=dynamic_cast<MucMcHitCol *> (obj);
  if (!mucMcHitCnvTds) {
    log << MSG::ERROR << "Could not downcast to MucMcHitCol" << endreq;
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
    log << MSG::ERROR << "MucMcHitCnv:Could not downcast to TDS McEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getMcCnv()->createRep(evt,addr); 
  TMcEvent *McEvt=m_cnvSvc->getMcCnv()->getWriteObject();

  const TObjArray *m_mucMcHitCol = McEvt->getMucMcHitCol();
  if (!m_mucMcHitCol) return sc;
  McEvt->clearMucMcHitCol(); //necessary in case there is I/O at the same time since array is static
  MucMcHitCol::const_iterator mucMcTds;

  for (mucMcTds = mucMcHitCnvTds->begin(); mucMcTds != mucMcHitCnvTds->end(); mucMcTds++) {
		UInt_t id  = ((*mucMcTds)->identify()).get_value() ;
		UInt_t trackIndex = (*mucMcTds) ->getTrackIndex();
		Double_t xPosition =   (*mucMcTds) ->getPositionX() ;
		Double_t yPosition =   (*mucMcTds) ->getPositionY() ;
		Double_t zPosition =   (*mucMcTds) ->getPositionZ() ;
		Double_t px=   (*mucMcTds) ->getPx() ;
		Double_t py =   (*mucMcTds) ->getPy() ;
		Double_t pz =   (*mucMcTds) ->getPz() ;
       
		TMucMc *mucMcRoot = new TMucMc();
		//m_common.m_mucMcHitMap[(*mucMcTds)] = mucMcRoot;

		mucMcRoot->setId(id);
		mucMcRoot->setTrackIndex(trackIndex);
		mucMcRoot->setPositionX(xPosition);
		mucMcRoot->setPositionY(yPosition);
		mucMcRoot->setPositionZ(zPosition);
		mucMcRoot->setPx(px);
		mucMcRoot->setPy(py);
		mucMcRoot->setPz(pz);

		McEvt->addMucMc(mucMcRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










