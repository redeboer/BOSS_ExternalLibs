//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Includes G4Steplimiter as process for the different Particles.
//             However, particles will only be affected by this process, if
//             G4UserLimit is associated to a volume. This method is used in
//             BesTofConstruction::ConstructMRPC()
//Author: Matthias Ullrich
//Created: October, 2011
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: StepLimiter.hh 





#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4LossTableManager.hh"



#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"


#include "G4StepLimiter.hh"
#include "StepLimiter.hh"

StepLimiter::StepLimiter(G4int ver, const G4String& name)
  : G4VPhysicsConstructor(name), verbose(ver)
{
  G4LossTableManager::Instance();
}

StepLimiter::~StepLimiter()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......                                                                                                                                

void StepLimiter::ConstructParticle()
{
  // gamma
  G4Gamma::Gamma();

  // leptons
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();

  // mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

  // barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

  // ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......                                                                                                                                

void StepLimiter::ConstructProcess()
{

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "e-") {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "e+") {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "alpha" ||
               particleName == "He3") {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "GenericIon") {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    } else if (particleName == "pi+" ||
               particleName == "pi-" ||
               particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "proton" ||
	       particleName == "anti_proton" 	       ) {
      pmanager ->AddDiscreteProcess(new G4StepLimiter());
    }

  }

}















