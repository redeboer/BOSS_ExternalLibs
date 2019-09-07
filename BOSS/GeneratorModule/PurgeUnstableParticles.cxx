#include "GeneratorModule/PurgeUnstableParticles.h"
#include "GeneratorObjects/McEventCollection.h"

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"

#include "GaudiKernel/IDataManagerSvc.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataSvc.h"

static const AlgFactory<PurgeUnstableParticles>    Factory;
const IAlgFactory& PurgeUnstableParticlesFactory = Factory;

PurgeUnstableParticles::PurgeUnstableParticles
  (const std::string& name, ISvcLocator* pSvcLocator) :
    Algorithm(name, pSvcLocator) {}


StatusCode PurgeUnstableParticles::initialize(){

  IDataManagerSvc* m_DMS;
  StatusCode sc = serviceLocator()->getService("EventDataSvc", 
					       IID_IDataManagerSvc, 
					       (IInterface*&)m_DMS);
  if (sc.isFailure()) { return sc; }

  std::string rootEventName = m_DMS->rootEventName();
  m_mcEventColl = rootEventName + "/McEventCollection";

  return StatusCode::SUCCESS;

}

StatusCode PurgeUnstableParticles::execute(){

  SmartDataPtr<McEventCollection> mcCollptr(eventDataService(),
					    m_mcEventColl);
  if(mcCollptr) {
    McEventCollection::iterator it;
    for(it=mcCollptr->begin(); it!=mcCollptr->end(); it++) {
      (*it)->purgeUnstableParticles();
      
    }
  }

  return StatusCode::SUCCESS;

}

StatusCode PurgeUnstableParticles::finalize(){

  return StatusCode::SUCCESS;

}
