// -------------------------------------------------------------
// File: GeneratorModule/GenModule.cxx
// Description:
//    This class is the base class used to specify the behavior of all
//    event generator modules and is meant to capture the common behavior
//    of these modules. 
//
// Header for this module:-

#include <fstream>

#include "GeneratorModule/GenModule.h"

// Framework Related Headers:-

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/SmartDataPtr.h"


#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/Incident.h"

#include "GaudiKernel/IPartPropSvc.h"

// Other Packages used by this class:-
#include "CLHEP/Random/Ranlux64Engine.h"
#include "CLHEP/Random/RandPoisson.h"

//#include "BesHepMC/GenVertex.h"
#include "HepMC/GenVertex.h"

#include "GeneratorObject/McGenEvent.h"


//---------------------------------------------------------------------------
GenModule::GenModule(const std::string& name, ISvcLocator* pSvcLocator)
  :	Algorithm(name, pSvcLocator),
	m_pRandomEngine(0),
	m_pPoissonGenerator(0)
  //---------------------------------------------------------------------------
{
  declareProperty("FixedMode",m_fixedMode=true);
  declareProperty("MeanInt",m_meanInteractions=1.0);
  declareProperty("RandomSeed",m_randomSeed=1234567);
  declareProperty("StripPartons",m_StripVector);
  m_StripVector.push_back(0);

}

//---------------------------------------------------------------------------
GenModule::~GenModule(){
  // Delete random number objects if they were created

  if(m_pPoissonGenerator) delete m_pPoissonGenerator;
  if(m_pRandomEngine) delete m_pRandomEngine;

} 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
StatusCode GenModule::initialize(){
  //---------------------------------------------------------------------------

  // Inform the user what the mode and conditions are:
  MsgStream log(messageService(), name());

  // Initialize StripPartons variables
  if (m_StripVector[0] > 0) {
    StripPartonsInit();
  } else {
    m_StripPartonSwitch	=	false;
    m_StripVector.clear();
  }

   
  // Get the Particle Properties Service
  IPartPropSvc* p_PartPropSvc;
  //static const bool CREATEIFNOTTHERE(true);
  //StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc, CREATEIFNOTTHERE);
  StatusCode PartPropStatus = service("PartPropSvc", p_PartPropSvc);
  if (!PartPropStatus.isSuccess() || 0 == p_PartPropSvc) {
    log << MSG::ERROR << " Could not initialize Particle Properties Service" << endreq;
    return PartPropStatus;
  }      

  m_particleTable = p_PartPropSvc->PDT();
  
  m_pRandomEngine = new CLHEP::Ranlux64Engine(m_randomSeed);

  if(m_fixedMode) {
    if(m_meanInteractions == 1.0) {
      log << MSG::INFO << "Standard Initialization:  Single Interaction Mode "
	  << endreq;
    }
    else {
      log << MSG::INFO << "Fixed Number of Interactions per Event is: "
	  << m_meanInteractions << endreq;
    }
  }
  else {
    m_pPoissonGenerator = new CLHEP::RandPoisson(*m_pRandomEngine, 
					  m_meanInteractions);

    log << MSG::INFO << 
      "Poisson Distribution of Interactions per Event with Mean: "
        << m_meanInteractions << endreq;
  }
  // Initialize the generator itself
  StatusCode status = this->genInitialize();
  if (status.isFailure()) {
    log << MSG::ERROR << "Could not initialize Generator properly" << endreq;
    return status;
  }  
  StatusCode  status1 = this->genuserInitialize();
  if (status1.isFailure()) {
    log << MSG::ERROR << "Could not initialize user part properly" << endreq;
    return status1;
  } 
  return status;
}

//---------------------------------------------------------------------------
StatusCode GenModule::execute() {
  //---------------------------------------------------------------------------
    
  int numToGenerate;
  StatusCode status;

  MsgStream log(messageService(), name());

  log << MSG::DEBUG << "GenModule::execute()" << endreq;

  // Decide how many interactions to generate
  if(m_fixedMode)  {
    numToGenerate = (int) m_meanInteractions;
  }
  else {
    numToGenerate = m_pPoissonGenerator->fire();
  }

  // Generate as many interactions as requested
  for(int i=0; i<numToGenerate; i++) {

    // Call the code that generates an event
    status = this->callGenerator();

    // Create the MC event and send the GeneratorEvent stored in it to fillEvt
    GenEvent* evt = new GenEvent(1,1);
    status = this->fillEvt(evt);

    // Strip out the requested partons here.
    if (m_StripPartonSwitch)	StripPartons(evt);
    
    // Check if the McCollection already exists
    SmartDataPtr<McGenEventCol> anMcCol(eventSvc(), "/Event/Gen");
    if (anMcCol!=0) {
      // Add event to existing collection
      MsgStream log(messageService(), name());
      log << MSG::INFO << "Add McGenEvent to existing collection" << endreq;
      McGenEvent* mcEvent = new McGenEvent(evt);
      anMcCol->push_back(mcEvent);
    }
    else {
      // Create Collection and add  to the transient store
      McGenEventCol *mcColl = new McGenEventCol;
      McGenEvent* mcEvent = new McGenEvent(evt);
      mcColl->push_back(mcEvent);

      StatusCode sc = eventSvc()->registerObject("/Event/Gen",mcColl);

      if (sc != StatusCode::SUCCESS) {
	MsgStream log(messageService(), name());
	log << MSG::ERROR << "Could not register McGenEvent" << endreq;
	delete mcColl;
	delete evt;
        delete mcEvent;
	return StatusCode::FAILURE;
      }
      else {
//        std::cout << " McGenEventCol made and stored" << std::endl;
      }
      
    }
  }

  // now call the incident service and set the signal.
  IIncidentSvc *incSvc;
  service("IncidentSvc",incSvc);
  incSvc->fireIncident( Incident( name(), "McGenEvent Generated") );

  return status;
}

//---------------------------------------------------------------------------
StatusCode GenModule::finalize() {
  //---------------------------------------------------------------------------

  StatusCode status = this->genFinalize();

  return status;
}

//---------------------------------------------------------------------------
StatusCode GenModule::genInitialize() { 
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode GenModule::genuserInitialize() { 
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode GenModule::callGenerator() { 
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode GenModule::genFinalize() { 
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

//---------------------------------------------------------------------------
StatusCode GenModule::fillEvt(GenEvent* /* evt */) {
  //---------------------------------------------------------------------------
  return StatusCode::SUCCESS;
}

void
GenModule::StripPartonsInit	(void)
{
  MsgStream log(messageService(), name());

  for (int i = 1; i < 9; ++i) { m_AllPartons.push_back(i); m_AllPartons.push_back(-i); } // Quarks
  m_AllPartons.push_back(21); // gluon

  m_StripPartonSwitch =	true;
  m_StripVector.erase(m_StripVector.begin(), m_StripVector.begin()+1);
  // When the user specifies only the StripPartonSwitch or gives a Particle Code
  // which is NOT a parton then strip ALL partons
  if (m_StripVector.size() == 0) {
    log << MSG::INFO << " !!!! NO SPECIFIC PARTON FOR STRIPOUT WAS REQUESTED => STRIPOUT ALL "
	<< endreq;
    m_StripVector = m_AllPartons;
  } else {
    bool	value_ok	=	true;
    std::vector<int>::const_iterator	i =	m_StripVector.begin();
    do {
      if ( std::find(m_AllPartons.begin(),
		     m_AllPartons.end(),
		     *i) == m_AllPartons.end() ) value_ok = false;
      ++i;
    } while (i != m_StripVector.end() && value_ok);
    if (!value_ok) {
      log << MSG::INFO << " !!!! ILEGAL PDG-ID FOR STRIPOUT WAS REQUESTED => STRIPOUT ALL "
	  << endreq;
      m_StripVector = m_AllPartons;
    }
  }
  log << MSG::INFO << " THE FOLLOWING PARTON(S) WILL BE STRIPED OUT ";
  for (std::vector<int>::const_iterator ip = m_StripVector.begin(); ip != m_StripVector.end(); ++ip)
    log << *ip << "  ";
  log << endreq;
  
}

void
GenModule::StripPartons	(GenEvent* evt)
{
  //  	for ( HepMC::GenEvent::particle_iterator p = evt->particles_begin();
  //  		  p != evt->particles_end(); ++p )
  //  	{
  //  //  	    std::cout << " PDG, BAR " << (*p)->pdg_id() << " " << (*p)->barcode() << std::endl;
  //  	    if ( std::find(m_StripVector.begin(),
  //  			   m_StripVector.end(),
  //  			   (*p)->pdg_id()) != m_StripVector.end() )
  //  	    {
  //  //  		std::cout << " REMOVING " << (*p)->pdg_id() << " " << (*p)->barcode() << std::endl;
  //  		HepMC::GenVertex*	pvtx	=	(*p)->production_vertex();
  //  		HepMC::GenVertex*	evtx	=	(*p)->end_vertex();
  //  		if (pvtx) pvtx->remove_particle(*p);
  //  		if (evtx) evtx->remove_particle(*p);
  //  		delete	*p;
  //  	    }
	    
  //  	}
	
  // Loop on all vertices of the event and remove the particle from the vertex
  for ( HepMC::GenEvent::vertex_iterator vtx = evt->vertices_begin();
	vtx != evt->vertices_end(); ++vtx ) {
    // Loop on all children particles and remove the ones that should be stripped out
    for ( HepMC::GenVertex::particle_iterator p = (*vtx)-> particles_begin(HepMC::children);
	  p != (*vtx)->particles_end(HepMC::children);
	  ++p ){
      if ( std::find(m_StripVector.begin(),
		     m_StripVector.end(),
		     (*p)->pdg_id()) != m_StripVector.end() ) {
	if ( (*p)->end_vertex() ) (*p)->end_vertex()->remove_particle(*p);
	if ( (*p)->production_vertex() ) (*p)->production_vertex()->remove_particle(*p);
	delete *p;
      }
    }
    // Loop on all parents particles and remove the ones that should be stripped out
    for ( HepMC::GenVertex::particle_iterator p = (*vtx)-> particles_begin(HepMC::parents);
	  p != (*vtx)->particles_end(HepMC::parents);
	  ++p ) {
      if ( std::find(m_StripVector.begin(),
		     m_StripVector.end(),
		     (*p)->pdg_id()) != m_StripVector.end() ) {
	if ( (*p)->end_vertex() ) (*p)->end_vertex()->remove_particle(*p);
	if ( (*p)->production_vertex() ) (*p)->production_vertex()->remove_particle(*p);
	delete *p;
      }
    }
  }
}

