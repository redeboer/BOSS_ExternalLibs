// --------------------------------------------------
// 
// File:  GeneratorModule/GenModule.h
// Description:
//    This class is the base class used to specify the behavior of all
//    event generator module and is meant to capture the common behavior
//    of these modules.  GenModule inherits the Algorithm interface.  
//    
//    The common behavior currently consists of:
//        - Providing a facility for generating multiple generator events 
//           within a single physics event (to mimic multiple interactions).  
//        - Providing the access to the HepMC ParticleProperties Table
//        - Creating the McEvent and if necessary the McEventCollection
//    In the future, this module will also provide the common interface to 
//    special purpose decay packages such as QQ (for B decays) and Tauola 
//    (for tau decays)
// 
//    The following virtual methods should be overloaded from the child class:
//         StatusCode genInitialize()
//         StatusCode callGenerator()
//         StatusCode genFinalize()
//         StatusCode fillEvt(GeneratorEvent* evt)
//

#ifndef GENERATORMODULEGENMODULE_H
#define GENERATORMODULEGENMODULE_H

#include "GaudiKernel/Algorithm.h"
#include "HepPDT/ParticleDataTable.hh"
//#include "BesHepMC/GenEvent.h"
#include "HepMC/GenEvent.h"
#include "CLHEP/Random/RandomEngine.h"
#include "CLHEP/Random/RandPoisson.h"

// The following line should replace the
// above one when using CLHEP/HepMC
//  #include "CLHEP/HepMC/GenEvent.h"

using HepMC::GenEvent;

class GenModule:public Algorithm {
public:
  GenModule(const std::string& name, ISvcLocator* pSvcLocator);
  virtual ~GenModule();
  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();

  virtual StatusCode genInitialize();
  virtual StatusCode genuserInitialize();
  virtual StatusCode callGenerator();
  virtual StatusCode genFinalize();
  virtual StatusCode fillEvt(GenEvent* evt);

protected:
  // Here are setable properties:-
  bool 		m_fixedMode;
  double 		m_meanInteractions;
  int 		m_randomSeed;
  std::vector<int> 	m_AllPartons;
  std::vector<int> 	m_StripVector;
  bool		m_StripPartonSwitch;
  void		StripPartonsInit(void);
  void		StripPartons(GenEvent* evt);
    
  // Here are local class data members:-
  CLHEP::HepRandomEngine* m_pRandomEngine;
  CLHEP::RandPoisson* m_pPoissonGenerator;

  // The following is temporary.  The particle properties should come 
  // from the ParticlePropertiesService, but that means replacing the
  // Gaudi one with the BES one
  HepPDT::ParticleDataTable* m_particleTable;
};
#endif

