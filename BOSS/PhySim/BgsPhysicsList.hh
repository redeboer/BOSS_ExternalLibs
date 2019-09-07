//-----------------------------------------------------------------------------
// File and Version Information:
// $Id: BgsPhysicsList.hh,v 1.1 2007/10/12 07:30:20 caogf Exp $
//
// Description:
//	Class BgsPhysicsList
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bill Lockman
//
// Copyright Information:
//	Copyright (C) 2000	U.C. Santa Cruz
//
// Created:
// Modification history:
//
//-----------------------------------------------------------------------------

#ifndef BgsPhysicsList_h
#define BgsPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

//#include "G3Data/GVertex.hh"

class BgsLooperDeath;
class BgsChargedStepDeath;
class BgsPhysicsRegistrar;
class BgsControl;

class G4PhotoElectricEffect;
class G4ComptonScattering;
class G4GammaConversion;
class G4MultipleScattering;
class G4eIonisation;
class G4eBremsstrahlung;
class G4eplusAnnihilation;
class G4MuIonisation;
class G4MuBremsstrahlung;
class G4MuPairProduction;
class G4hIonisation;
class G4HadronElasticProcess;
class G4LElastic;
class G4ParticleDefinition;

class G4ProcessManager;
class G4VProcess;
class G4CascadeInterface;


class BgsPhysicsList: public G4VUserPhysicsList
{
public:
//  BgsPhysicsList( BgsControl *theControl, 
//  		  BgsPhysicsRegistrar *pr);
    BgsPhysicsList();
    virtual ~BgsPhysicsList();

  // define cuts
  void SetCuts();
  
  void SetStatusEmProcess();

protected:
  // Construct particle and physics
  void ConstructParticle();
  void ConstructProcess();
 
  // methods to construct particles 
  void ConstructBosons();
  void ConstructLeptons();
  void ConstructMesons();
  void ConstructBaryons();
  void ConstructIons();

  // methods to construct physics processes and register them
//  void AddBgsTransportation( G4double maxTrackStepSize, G4double
//maxVacStepSize, const G4String& vacName );
//  void AddParameterisation();
  void ConstructGeneral();
  void ConstructEM();
  void ConstructLeptHad();
  void ConstructHad();
  void ConstructNeutrinoGenocide();
  void ConstructIonFix();
  void ConstructNeutronFix();
    
/*    
  void AddProcess( G4VProcess* process, 
        	   G4int ordAtRestDoIt,
        	   G4int ordAlongSteptDoIt,
        	   G4int ordPostStepDoIt,
		   G4ProcessManager* manager,
		   const char* category,
		   GVertex::Cause cause=GVertex::unknown  );

  void AddDiscreteProcess( G4VProcess* process, 
			   G4ProcessManager* manager,
			   const char* category,
		           GVertex::Cause cause=GVertex::unknown  );

  void AddElasticHadronProcess( G4HadronElasticProcess* process,
        			G4LElastic* model,  
                                G4ProcessManager* manager,
			        const char* category,
		                GVertex::Cause cause=GVertex::unknown  );


*/
private:
//  BgsControl* control;

  G4bool first;
  
//  BgsPhysicsRegistrar* physicsRegistrar;
  
//  BgsLooperDeath* theLooperDeath;
//  BgsChargedStepDeath* theStepDeath;

  // Bertini cascade model for inelastic p, n, pi

  G4CascadeInterface* bertini_model;
};

#endif
