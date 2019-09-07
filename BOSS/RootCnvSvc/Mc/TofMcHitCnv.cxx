#ifndef TofMcHitCnv_CXX
#define TofMcHitCnv_CXX  1

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/ObjectVector.h"

#include "TClonesArray.h"

#include "EventModel/EventModel.h"
#include "McTruth/McEvent.h"   //TDS object
#include "McTruth/TofMcHit.h"   //TDS object
#include "RootEventData/TTofMc.h" // standard root object
#include "RootEventData/TMcEvent.h"
#include "RootCnvSvc/Mc/McCnv.h"
#include "RootCnvSvc/Mc/TofMcHitCnv.h"
#include "RootCnvSvc/RootAddress.h"


// Instantiation of a static factory class used by clients to create
// instances of this service
//static CnvFactory<TofMcHitCnv> s_factory;
//const ICnvFactory& TofMcHitCnvFactory = s_factory;

TofMcHitCnv::TofMcHitCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
    // Here we associate this converter with the /Event path on the TDS.
    MsgStream log(msgSvc(), "TofMcHitCnv");
    //log << MSG::DEBUG << "Constructor called for " << objType() << endreq;
    m_rootBranchname ="m_tofMcHitCol";
    //declareObject(EventModel::MC::TofMcHitCol, objType(), m_rootTreename, m_rootBranchname);
    m_adresses.push_back(&m_tofMcHitCol);
    m_tofMcHitCol=0;
}

StatusCode TofMcHitCnv::TObjectToDataObject(DataObject*& refpObject) {
  // creation of TDS object from root object

    MsgStream log(msgSvc(), "TofMcHitCnv");
    log << MSG::DEBUG << "TofMcHitCnv::TObjectToDataObject" << endreq;
    StatusCode sc=StatusCode::SUCCESS;

    // create the TDS location for the MdcMc Collection
    TofMcHitCol* tofMcTdsCol = new TofMcHitCol;
    refpObject=tofMcTdsCol;


    // now convert
    if (!m_tofMcHitCol) return sc;
    TIter tofMcIter(m_tofMcHitCol);
    TTofMc *tofMcRoot = 0;
    while ((tofMcRoot = (TTofMc*)tofMcIter.Next())) {
      unsigned int id = tofMcRoot ->getId();
		unsigned int trackIndex = tofMcRoot ->getTrackIndex();
		double xPosition =   tofMcRoot ->getPositionX() ;
		double yPosition =   tofMcRoot ->getPositionY() ;
		double zPosition =   tofMcRoot ->getPositionZ() ;
		double px=   tofMcRoot ->getPx() ;
		double py =   tofMcRoot ->getPy() ;
		double pz =   tofMcRoot ->getPz() ;
		double trackLength = tofMcRoot->getTrackLength() ;
		double flightTime = tofMcRoot->getFlightTime() ;
		
	
        	TofMcHit *tofMcTds = new TofMcHit;
		m_common.m_rootTofMcHitMap[tofMcRoot] = tofMcTds;
	
		tofMcTds->setIdentifier(id);
		tofMcTds->setTrackIndex(trackIndex);
		tofMcTds->setPositionX(xPosition);
		tofMcTds->setPositionY(yPosition);
		tofMcTds->setPositionZ(zPosition);
		tofMcTds->setPx(px);
		tofMcTds->setPy(py);
		tofMcTds->setPz(pz);
		tofMcTds->setTrackLength(trackLength);
		tofMcTds->setFlightTime(flightTime);
		

        tofMcTdsCol->push_back(tofMcTds);
     }
    //m_tofMcHitCol->Delete();  // wensp add 2005/12/30
    delete m_tofMcHitCol;
    m_tofMcHitCol = 0;
   return StatusCode::SUCCESS;
}

StatusCode TofMcHitCnv::DataObjectToTObject(DataObject* obj,RootAddress* rootaddr) {

  MsgStream log(msgSvc(), "TofMcHitCnv");
  log << MSG::DEBUG << "TofMcHitCnv::DataObjectToTObject" << endreq;
  StatusCode sc=StatusCode::SUCCESS;
 
 TofMcHitCol * tofMcHitCnvTds=dynamic_cast<TofMcHitCol *> (obj);
  if (!tofMcHitCnvTds) {
    log << MSG::ERROR << "Could not downcast to TofMcHitCol" << endreq;
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
    log << MSG::ERROR << "TofMcHitCnv:Could not downcast to TDS McEvent" << endreq;
  }
  IOpaqueAddress *addr;

  m_cnvSvc->getMcCnv()->createRep(evt,addr); 
  TMcEvent *McEvt=m_cnvSvc->getMcCnv()->getWriteObject();

  const TObjArray *m_tofMcHitCol = McEvt->getTofMcHitCol();
  if (!m_tofMcHitCol) return sc;
  McEvt->clearTofMcHitCol(); //necessary in case there is I/O at the same time since array is static
  TofMcHitCol::const_iterator tofMcTds;

  for (tofMcTds = tofMcHitCnvTds->begin(); tofMcTds != tofMcHitCnvTds->end(); tofMcTds++) {
		UInt_t id  = ((*tofMcTds)->identify()).get_value() ;
		UInt_t trackIndex = (*tofMcTds) ->getTrackIndex();
		Double_t xPosition =   (*tofMcTds) ->getPositionX() ;
		Double_t yPosition =   (*tofMcTds) ->getPositionY() ;
		Double_t zPosition =   (*tofMcTds) ->getPositionZ() ;
		Double_t px=   (*tofMcTds) ->getPx() ;
		Double_t py =   (*tofMcTds) ->getPy() ;
		Double_t pz =   (*tofMcTds) ->getPz() ;
		Double_t trackLength = (*tofMcTds) ->getTrackLength() ;
		Double_t flightTime = (*tofMcTds) ->getFlightTime() ;
       
		TTofMc *tofMcRoot = new TTofMc();
		//m_common.m_tofMcHitMap[(*tofMcTds)] = tofMcRoot;

		tofMcRoot->setId(id);
		tofMcRoot->setTrackIndex(trackIndex);
		tofMcRoot->setPositionX(xPosition);
		tofMcRoot->setPositionY(yPosition);
		tofMcRoot->setPositionZ(zPosition);
		tofMcRoot->setPx(px);
		tofMcRoot->setPy(py);
		tofMcRoot->setPz(pz);
		tofMcRoot->setTrackLength(trackLength);
		tofMcRoot->setFlightTime(flightTime);

		McEvt->addTofMc(tofMcRoot);
  }

  return StatusCode::SUCCESS;
}
#endif










