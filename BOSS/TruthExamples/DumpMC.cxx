#include "TruthExamples/DumpMC.h"
#include "GeneratorModule/GeneratorName.h"
// #include "GeneratorObject/McEventCollection.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/DataSvc.h"

// #include "StoreGate/StoreGateSvc.h"

DumpMC::DumpMC(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
{}

StatusCode DumpMC::initialize(){

  MsgStream log(messageService(), name());
        log << MSG::INFO << ">>> DumpMC from Initialize" << endreq;
        std::cout << "----- DumpMC World From initialize" << std::endl;

	/*      
	StatusCode sc = service("StoreGateSvc", m_sgSvc);
	if (sc.isFailure()) {
	  log << MSG::ERROR << "Could not find StoreGateSvc" << endreq;
	  return sc;
	}
	*/

  // Initialization terminated
        return StatusCode::SUCCESS;
}
StatusCode DumpMC::execute() {

  MsgStream log(messageService(), name());
  log << MSG::INFO << ">>> DumpMC from execute" << endreq;
  std::cout << "----- DumpMC World From execute" << std::endl;
  
  // Read Data from Transient Store
//    SmartDataPtr<McEventCollection> mcCollptr(eventDataService(),
//  					    "/Event/McEventCollection");
//    if(!mcCollptr) {
//      std::cout << " No McCollptr" << std::endl;
//    }
//    else {
//      std::cout << " ---- Begin Iterating Over McEventCollection --- " << std::endl;
//      McEventCollection::iterator it;
//      for(it=mcCollptr->begin(); it!=mcCollptr->end(); it++) {
//        std::cout << "Next event in the bag" << std::endl;
//        std::cout << "     Generator: " << (*it)->generatorName() << std::endl;
//        (*it)->pGenEvt()->print();
//      }
//    }

/*
  const McEventCollection* mcCollptr;
  if ( m_sgSvc->retrieve(mcCollptr).isFailure() ) {
    log << MSG::ERROR << "Could not retrieve McEventCollection"
	<< endreq;
    return StatusCode::FAILURE;
  }
  
  // Loop over all events in McEventCollection
  McEventCollection::const_iterator itr;
  for (itr = mcCollptr->begin(); itr!=mcCollptr->end(); ++itr) {
    std::cout << "Next event in the bag" << std::endl;
    int g_id = (*itr)->signal_process_id();
    GeneratorName_print(g_id);
    std::cout << std::endl;
    (*itr)->print();
  }	  
*/
  
  // End of execution for each event
  return StatusCode::SUCCESS;
}
StatusCode DumpMC::finalize() {

  MsgStream log(messageService(), name());
  log << MSG::INFO << ">>> DumpMC from finalize" << endreq;
  std::cout << "----- DumpMC World From finalize" << std::endl;

  // End of finalization step
        return StatusCode::SUCCESS;
}

