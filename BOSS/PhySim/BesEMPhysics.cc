
#include "BesEMPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   


BesEMPhysics::BesEMPhysics(const G4String& name)
               :  G4VPhysicsConstructor(name)
{
}

BesEMPhysics::~BesEMPhysics()
{
}

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#include "G4Gamma.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"

#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

//#include "G4StepLimiter.hh"

void BesEMPhysics::ConstructParticle()
{
  // gamma
  G4Gamma::GammaDefinition();
 
  // electron
  G4Electron::ElectronDefinition();
  G4Positron::PositronDefinition();
  G4NeutrinoE::NeutrinoEDefinition();
  G4AntiNeutrinoE::AntiNeutrinoEDefinition();
}


#include "G4ProcessManager.hh"


void BesEMPhysics::ConstructProcess()
{
  G4ProcessManager * pManager = 0;
  
  // Gamma Physics
  pManager = G4Gamma::Gamma()->GetProcessManager();
  pManager->AddDiscreteProcess(&thePhotoEffect);
  pManager->AddDiscreteProcess(&theComptonEffect);
  pManager->AddDiscreteProcess(&thePairProduction);

  // Electron Physics
  pManager = G4Electron::Electron()->GetProcessManager();

  pManager->AddProcess(&theElectronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&theElectronIonisation,         -1, 2, 2);
  pManager->AddProcess(&theElectronBremsStrahlung,     -1, 3, 3);  
//    pManager->AddProcess(new G4StepLimiter,       -1, -1,4);
    //pManager->AddDiscreteProcess(new G4StepLimiter);
  //Positron Physics
  pManager = G4Positron::Positron()->GetProcessManager();

  pManager->AddProcess(&thePositronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&thePositronIonisation,         -1, 2, 2);
  pManager->AddProcess(&thePositronBremsStrahlung,     -1, 3, 3);  
  pManager->AddProcess(&theAnnihilation,                0,-1, 4);  
}
