// File and Version Information:
// Description:
//      EvtNavigatorCnv is the concrete converter for the EvtNavigator branch
//

#ifndef EvtNavigatorCnv_CXX
#define EvtNavigatorCnv_CXX 

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RegistryEntry.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "GaudiKernel/IOpaqueAddress.h"

#include "EventModel/EventModel.h"
#include "EventNavigator/EventNavigator.h"
#include "RootEventData/TEvtNavigator.h"
#include "RootCnvSvc/EvtNavigatorCnv.h"
#include "RootCnvSvc/RootEvtSelector.h"
#include "RootCnvSvc/RootAddress.h"
#include "RootCnvSvc/Util.h"

using namespace DataSvcHelpers;

TEvtNavigator* EvtNavigatorCnv::m_evtNavigatorR = 0;
TEvtNavigator* EvtNavigatorCnv::m_evtNavigatorW = 0;

EvtNavigatorCnv::EvtNavigatorCnv(ISvcLocator* svc)
: RootEventBaseCnv(classID(), svc)
{
  // Here we associate this converter with the path on the TDS.
  MsgStream log(msgSvc(), "EvtNavigatorCnv");
  m_rootBranchname = "m_mcMdcMcHits:m_mcMdcTracks:m_mcEmcMcHits:m_mcEmcRecShowers";
  m_adresses.push_back(&m_mcMdcMcHits);
  m_adresses.push_back(&m_mcMdcTracks);
  m_adresses.push_back(&m_mcEmcMcHits);
  m_adresses.push_back(&m_mcEmcRecShowers);

  m_evtNavigatorR = 0;
  m_evtNavigatorW = 0;
}

StatusCode EvtNavigatorCnv::TObjectToDataObject(DataObject*& refpObject) {
    // fills the TDS object from the persistent one
  StatusCode sc=StatusCode::SUCCESS;
    
  MsgStream log(msgSvc(), "EvtNavigatorCnv");

  EventNavigator* evt = new EventNavigator;
  refpObject = evt;
 
  // fill DataObject
  evt->setMcMdcMcHitsIdx(m_mcMdcMcHits);
  evt->setMcMdcTracksIdx(m_mcMdcTracks);  
  evt->setMcEmcMcHitsIdx(m_mcEmcMcHits);
  evt->setMcEmcRecShowersIdx(m_mcEmcRecShowers);

  delete m_evtNavigatorR;
  m_evtNavigatorR = 0;
  
  return sc;
}

StatusCode EvtNavigatorCnv::initialize()   {
 
  StatusCode status = RootEventBaseCnv::initialize();
  m_cnvSvc->setEvtNavigatorCnv(this);

  return status;

}

StatusCode EvtNavigatorCnv::DataObjectToTObject(DataObject* obj, RootAddress* rootaddr) {
  // transforms TDS event to Dst event in root format
  MsgStream log(msgSvc(), "EvtNavigatorCnv");

  EventNavigator * evtNaviTds=dynamic_cast<EventNavigator *> (obj);
  if (!evtNaviTds) {
    log << MSG::ERROR << "Could not downcast to TDS EventNavigator" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_evtNavigatorW = new TEvtNavigator(); 
  m_common.m_EvtNavigator = m_evtNavigatorW;

  m_evtNavigatorW->LoadMcMdcTracks(evtNaviTds->getMcMdcTracksIdx());
  if(evtNaviTds->getMcMdcTracksIdx().empty())
      m_evtNavigatorW->LoadMcMdcMcHits(evtNaviTds->getMcMdcMcHitsIdx());
  m_evtNavigatorW->LoadMcEmcRecShowers(evtNaviTds->getMcEmcRecShowersIdx());
  if(evtNaviTds->getMcEmcRecShowersIdx().empty())
      m_evtNavigatorW->LoadMcEmcMcHits(evtNaviTds->getMcEmcMcHitsIdx());

  // create branch if not yet done
  if (m_branchNrEvtNavigator<0) {
    StatusCode sc = m_rootInterface->createBranch(rootaddr->getTreename(),"EventNavigator",m_evtNavigatorW->ClassName(),&m_evtNavigatorW,m_branchNrEvtNavigator);
    if (sc.isFailure()) {
      delete m_evtNavigatorW;
      log << MSG::ERROR << "Could not create branch TEvtNavigator in " << rootaddr->getTreename() << endreq;
      return StatusCode::SUCCESS;
      //return sc;
    }
  }

  return StatusCode::SUCCESS;
}
#endif

