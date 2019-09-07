#include <CLHEP/Units/PhysicalConstants.h>
#include <CLHEP/Geometry/Point3D.h>
#include "MdcTrkRecon/MdcMakeHits.h"
#include "MdcData/MdcHitMap.h"
#include "MdcData/MdcHit.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/PropertyMgr.h" 
#include "MdcRawEvent/MdcDigi.h"
#include "EventModel/Event.h"
#include "MdcGeom/MdcSWire.h"
#include "MdcGeom/MdcLayer.h"
#include "MdcGeom/MdcDetector.h"
#include "Identifier/MdcID.h"
#include "MdcCalibFunSvc/IMdcCalibFunSvc.h"
#include "MdcCalibFunSvc/MdcCalibFunSvc.h"
#include "GaudiKernel/INTupleSvc.h"
#include "RawDataProviderSvc/IRawDataProviderSvc.h"
#include "RawDataProviderSvc/MdcRawDataProvider.h"

#include <vector>
#include <iostream>
using namespace std;
using namespace Event;


extern const CLID & CLID_MdcHitCol;


/////////////////////////////////////////////////////////////////////////////

MdcMakeHits::MdcMakeHits(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{
  // Declare the properties  
  declareProperty("cosmicFit",    m_cosmicFit= false);
  declareProperty("doSag",        m_doSag= false);
  declareProperty("countPropTime",m_countPropTime = true);

  declareProperty("getDigiFlag",  m_getDigiFlag = 0);
  declareProperty("maxMdcDigi",   m_maxMdcDigi= 0);
  declareProperty("keepBadTdc",   m_keepBadTdc= 0);
  declareProperty("dropHot",      m_dropHot= 0);
  declareProperty("keepUnmatch",  m_keepUnmatch= 0);
  declareProperty("minMdcDigi",	  m_minMdcDigi = 0);
  declareProperty("combineTracking",m_combineTracking = false);//yzhang 2010-05-28 

#ifdef MDCPATREC_RESLAYER
  declareProperty("resLayer",  m_resLayer= -1);
#endif
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcMakeHits::beginRun(){
  _gm = MdcDetector::instance(m_doSag);
  if(NULL == _gm) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcMakeHits::initialize(){

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in initialize()" << endreq;

  StatusCode sc;

  IMdcCalibFunSvc* imdcCalibSvc; 
  sc = service ("MdcCalibFunSvc",imdcCalibSvc);
  m_mdcCalibFunSvc = dynamic_cast<MdcCalibFunSvc*> (imdcCalibSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load MdcCalibFunSvc!" << endreq;
  }

  IRawDataProviderSvc* irawDataProviderSvc;
  sc = service ("RawDataProviderSvc", irawDataProviderSvc);
  m_rawDataProviderSvc = dynamic_cast<RawDataProviderSvc*> (irawDataProviderSvc);
  if ( sc.isFailure() ){
    log << MSG::FATAL << "Could not load RawDataProviderSvc!" << endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcMakeHits::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in execute()" << endreq;
  StatusCode sc;

  DataObject *pnode = 0;
  sc = eventSvc()->retrieveObject("/Event/Hit",pnode);
  if(!sc.isSuccess()) {
    pnode = new DataObject;
    sc = eventSvc()->registerObject("/Event/Hit",pnode);
    if(!sc.isSuccess()) {
      log << MSG::FATAL << " Could not register hit branch" <<endreq;
      return StatusCode::FAILURE;
    }
  }

  SmartIF<IDataManagerSvc> dataManSvc(eventSvc()); 
  DataObject *hitCol;
  eventSvc()->findObject("/Event/Hit/MdcHitCol",hitCol);
  if(hitCol!= NULL) {
    dataManSvc->clearSubTree("/Event/Hit/MdcHitCol");
    eventSvc()->unregisterObject("/Event/Hit/MdcHitCol");
  }
  MdcHitCol *mdcHitCol = new MdcHitCol;
  sc = eventSvc()->registerObject("/Event/Hit/MdcHitCol",mdcHitCol);
  if(!sc.isSuccess()) {
    log << MSG::FATAL << " Could not register hit collection" <<endreq;
    return StatusCode::FAILURE;
  } 

  DataObject *hitMap;
  eventSvc()->findObject("/Event/Hit/MdcHitMap",hitMap);
  if(hitMap!= NULL) {
    dataManSvc->clearSubTree("/Event/Hit/MdcHitMap");
    eventSvc()->unregisterObject("/Event/Hit/MdcHitMap");
  }
  MdcHitMap *mdcHitMap = new MdcHitMap(*_gm);
  sc = eventSvc()->registerObject("/Event/Hit/MdcHitMap",mdcHitMap);
  if(!sc.isSuccess()) {
    log << MSG::FATAL << " Could not register hit map" <<endreq;
    return StatusCode::FAILURE;
  } 

  // retrieve Mdc digi vector form RawDataProviderSvc 
  uint32_t getDigiFlag = 0;
  getDigiFlag += m_maxMdcDigi;
  if(m_dropHot || m_combineTracking)getDigiFlag |= MdcRawDataProvider::b_dropHot;
  if(m_keepBadTdc)  getDigiFlag |= MdcRawDataProvider::b_keepBadTdc;
  if(m_keepUnmatch) getDigiFlag |= MdcRawDataProvider::b_keepUnmatch;
  MdcDigiVec mdcDigiVec = m_rawDataProviderSvc->getMdcDigiVec(getDigiFlag);

  //std::cout<<__FILE__<<" nDigi = "<<mdcDigiVec.size()<<  std::endl;
  if ((int)mdcDigiVec.size()<m_minMdcDigi){
    log << MSG::WARNING << " Skip this event for MdcDigiVec.size() < "<<m_minMdcDigi << endreq;
    return StatusCode::SUCCESS;
  }

  MdcDigiVec::iterator iter = mdcDigiVec.begin();
  for (;iter != mdcDigiVec.end(); iter++ ) {
    const MdcDigi* aDigi = *iter;
    MdcHit *hit = new MdcHit(aDigi, _gm);
    hit->setCalibSvc(m_mdcCalibFunSvc);
    hit->setCosmicFit(m_cosmicFit);
    hit->setCountPropTime(m_countPropTime);
#ifdef MDCPATREC_RESLAYER
    hit->setResLayer(m_resLayer);
#endif
    mdcHitCol->push_back(hit);
    mdcHitMap->addHit(*hit);
  }
  return StatusCode::SUCCESS;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
StatusCode MdcMakeHits::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "in finalize()" << endreq;

  return StatusCode::SUCCESS;
}

