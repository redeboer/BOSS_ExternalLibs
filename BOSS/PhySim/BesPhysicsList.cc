
#include "BesPhysicsList.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include <iomanip>   

#include "BesGeneralPhysics.hh"
#include "BesEMPhysics.hh"
#include "BesMuonPhysics.hh"
#include "BesHadronPhysics.hh"
#include "BesIonPhysics.hh"

BesPhysicsList::BesPhysicsList():  G4VModularPhysicsList()
{
  // default cut value  (1.0mm) 
  defaultCutValue = 1.0*mm;
  //  SetVerboseLevel(1);

  // General Physics
  RegisterPhysics( new BesGeneralPhysics("general") );

  // EM Physics
  RegisterPhysics( new BesEMPhysics("standard EM"));

  // Muon Physics
  RegisterPhysics(  new BesMuonPhysics("muon"));

   // Hadron Physics
  RegisterPhysics(  new BesHadronPhysics("hadron"));

  // Ion Physics
  RegisterPhysics( new BesIonPhysics("ion"));


}

BesPhysicsList::~BesPhysicsList()
{
}

void BesPhysicsList::SetCuts()
{
  //  " G4VUserPhysicsList::SetCutsWithDefault" method sets 
  //   the default cut value for all particle types 
  SetCutsWithDefault();   
}























