//
//File: ExtPhysicsList.cc
//Date: 2005.3.9
//Author: L.L.Wang
//

#include "TrkExtAlg/ExtPhysicsList.h"

#include "TrkExtAlg/ExthIonisation.h"
#include "TrkExtAlg/ExteIonisation.h"
#include "TrkExtAlg/ExtMuIonisation.h"
//#include "TrkExtAlg/ExtMultipleScattering.h"

#include "G4ParticleTypes.hh"

#include "G4MultipleScattering.hh"
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4hIonisation.hh"

#include "G4ProcessManager.hh"
  
ExtPhysicsList::ExtPhysicsList()
{
	defaultCutValue = 0.5*mm;
}

ExtPhysicsList::~ExtPhysicsList(){;}

void ExtPhysicsList::ConstructParticle()
{
     // In this method, static member functions should be called
     // for all particles which you want to use.
     // This ensures that objects of these particle types will be
     // created in the program.
   G4Gamma:: GammaDefinition();
   G4Electron:: ElectronDefinition();
   G4Positron:: PositronDefinition();
   G4MuonPlus:: MuonPlusDefinition();
   G4MuonMinus:: MuonMinusDefinition();
   G4PionPlus:: PionPlusDefinition();
   G4PionMinus:: PionMinusDefinition();
   G4KaonPlus:: KaonPlusDefinition();
   G4KaonMinus:: KaonMinusDefinition();
   G4Proton:: ProtonDefinition();
   G4AntiProton:: AntiProtonDefinition();
}
 

void ExtPhysicsList::ConstructProcess()
{
   // Define transportation process (must be invoked)
   AddTransportation();
   
   //Add Energy Loss Process
   theParticleIterator->reset();
   while( (*theParticleIterator)() )
     {
	G4ParticleDefinition* particle = theParticleIterator->value();
	G4ProcessManager* pmanager = particle->GetProcessManager();
	G4String particleName = particle->GetParticleName();
	if(particleName == "e-" || particleName == "e+")
	  {
//	     pmanager->AddProcess( new G4MultipleScattering,-1,1,1);
//	     pmanager->AddProcess( new G4eIonisation,-1,2,2);
	     pmanager->AddProcess( new ExteIonisation,-1,1,1);
//	     pmanager->AddProcess( new G4eBremsstrahlung,-1,2,2);
	  }
	else if(particleName == "mu-" || particleName == "mu+")
	  {
//	     pmanager->AddProcess( new G4MultipleScattering,-1,1,1);
//	     pmanager->AddProcess( new G4MuIonisation,-1,2,2);
	     pmanager->AddProcess( new ExtMuIonisation,-1,1,1);
//	     pmanager->AddProcess( new G4MuBremsstrahlung,-1,2,2);
	  }
	else if((!particle->IsShortLived()) &&  (particle->GetPDGCharge() != 0.0) && (particleName != "chargedgeantino"))
	  {
//	     pmanager->AddProcess( new G4MultipleScattering,-1,1,1);
//	     pmanager->AddProcess( new G4hIonisation,-1,2,2);
//	     pmanager->AddProcess( new ExtMultipleScattering,-1,1,1);
	     pmanager->AddProcess( new ExthIonisation,-1,1,1);          
	  }
     }
}


void ExtPhysicsList::SetCuts()
{
   // " G4VUserPhysicsList::SetCutsWithDefault" method sets
   //   the default cut value for all particle types 
   SetCutsWithDefault();

} 
