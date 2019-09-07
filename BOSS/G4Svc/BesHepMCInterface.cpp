#include "G4Svc/BesHepMCInterface.h"
#include "HepMC/GenEvent.h"

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IMessageSvc.h"

#include "GeneratorObject/McGenEvent.h"
#include "GaudiKernel/SmartDataPtr.h"

BesHepMCInterface::BesHepMCInterface()
{
  p_evtSvc = 0;
}
BesHepMCInterface::~BesHepMCInterface()
{
	//std::cout<< "\b the BesHepMCInterface is being destroyed "<<std::endl;
}

HepMC::GenEvent* BesHepMCInterface::GenerateHepMCEvent()
{
  
  if (p_evtSvc == 0) {     
    //std::cout<<" standard interface to EvtDataSvc for retrieving HepMC events"<<std::endl;
    ISvcLocator* svcLocator = Gaudi::svcLocator(); // from Bootstrap
    StatusCode sc=svcLocator->service("EventDataSvc", p_evtSvc);
    if (sc.isFailure()) 
    {
	    //std::cout<<"BesHepMCInterface could not access EventDataSvc!!"<<std::endl;
    }
  }
	int n = 0;
  //std::cout <<" BesHepMCInterface::GenerateAnEvent "<<std::endl;

	SmartDataPtr<McGenEventCol> mcCollptr( p_evtSvc, "/Event/Gen");

	if ( mcCollptr != 0 ) {
	  //std::cout <<" could retrieve the collection "<<std::endl;

	  n = mcCollptr->size();
	  
	  //std::cout <<" nr of events "<<n<<std::endl;

	  McGenEventCol::const_iterator it = mcCollptr->begin();

	  McGenEvent* mcEvent = (McGenEvent* ) (*it); 	
	  // getting only the first event here.		
	  // std::cout <<"iterator"<<std::endl;
		
	  HepMC::GenEvent *p_evt;
		
	  //std::cout <<"hepmc event"<<std::endl;
	  p_evt = mcEvent->getGenEvt();
	  //std::cout << "eventNumber = " << p_evt-> event_number()  << std::endl;

	  //std::cout << " BesHepMCInterface:: --> " <<std::endl;
	  //std::cout << " particles_size = " << p_evt->particles_size() 
		//    << " vertices_size = " << p_evt->vertices_size()
		//    << std::endl;

	  //std::cout <<"got it"<<std::endl;
          
	  return p_evt;
  	}

    else {
	  std::cout << "no McGenEventCollection found."  << std::endl;
  	}     
	  return 0;
}

