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




#ifndef StepLimiter_h
#define StepLimiter_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class StepLimiter : public G4VPhysicsConstructor
{
public:
  StepLimiter(G4int ver = 0, const G4String& name = "StepLimiter");
  virtual ~StepLimiter();

  virtual void ConstructParticle();
  virtual void ConstructProcess();

private:

  G4int verbose;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
