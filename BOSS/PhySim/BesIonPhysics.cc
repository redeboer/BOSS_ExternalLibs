
#include "BesIonPhysics.hh"

#include "globals.hh"
#include "G4ios.hh"
#include <iomanip>   


BesIonPhysics::BesIonPhysics(const G4String& name)
                 :  G4VPhysicsConstructor(name)
{
}

BesIonPhysics::~BesIonPhysics()
{
}

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// Nuclei
#include "G4IonConstructor.hh"

void BesIonPhysics::ConstructParticle()
{
  //  Construct light ions
  G4IonConstructor pConstructor;
  pConstructor.ConstructParticle();  
}


#include "G4ProcessManager.hh"


void BesIonPhysics::ConstructProcess()
{
  G4ProcessManager * pManager = 0;
  
  // Elastic Process
  theElasticModel = new G4LElastic();
  theElasticProcess.RegisterMe(theElasticModel);

  // Generic Ion
  pManager = G4GenericIon::GenericIon()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theElasticProcess);

  pManager->AddProcess(&fIonMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&fIonIonisation,         -1, 2, 2);

  // Deuteron 
  pManager = G4Deuteron::Deuteron()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theElasticProcess);

  fDeuteronModel = new G4LEDeuteronInelastic();
  fDeuteronProcess.RegisterMe(fDeuteronModel);
  pManager->AddDiscreteProcess(&fDeuteronProcess);

  pManager->AddProcess(&fDeuteronMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&fDeuteronIonisation,        -1, 2, 2);

   // Triton 
  pManager = G4Triton::Triton()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theElasticProcess);

  fTritonModel = new G4LETritonInelastic();
  fTritonProcess.RegisterMe(fTritonModel);
  pManager->AddDiscreteProcess(&fTritonProcess);

  pManager->AddProcess(&fTritonMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&fTritonIonisation,        -1, 2, 2);
 
  // Alpha 
  pManager = G4Alpha::Alpha()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theElasticProcess);

  fAlphaModel = new G4LEAlphaInelastic();
  fAlphaProcess.RegisterMe(fAlphaModel);
  pManager->AddDiscreteProcess(&fAlphaProcess);

  pManager->AddProcess(&fAlphaMultipleScattering, -1, 1, 1);
  pManager->AddProcess(&fAlphaIonisation,        -1, 2, 2);
 
  // He3
  pManager = G4He3::He3()->GetProcessManager();
  // add process
  pManager->AddDiscreteProcess(&theElasticProcess);

  pManager->AddProcess(&fHe3MultipleScattering, -1, 1, 1);
  pManager->AddProcess(&fHe3Ionisation,        -1, 2, 2);

   
}



