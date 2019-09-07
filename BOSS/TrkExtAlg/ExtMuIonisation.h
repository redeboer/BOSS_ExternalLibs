//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// $Id: ExtMuIonisation.h,v 1.6 2015/10/27 11:02:53 sunss Exp $
// GEANT4 tag $Name: TrkExtAlg-00-00-65 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     ExtMuIonisation
//
// Author:        Laszlo Urban
//
// Creation date: 30.05.1997
//
// Modifications:
//
// corrected by L.Urban on 24/09/97
// corrected by L.Urban on 13/01/98
// bugs fixed by L.Urban on 02/02/99
// 10/02/00 modifications , new e.m. structure, L.Urban
// 10-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 14-08-01 new function ComputeRestrictedMeandEdx() + 'cleanup' (mma)
// 19-09-01 come back to previous process name "hIoni"
// 29-10-01 all static functions no more inlined
// 10-05-02 V.Ivanchenko update to new design
// 09-12-02 V.Ivanchenko remove warning
// 26-12-02 Secondary production moved to derived classes (VI)
// 24-01-03 Make models region aware (V.Ivanchenko)
// 05-02-03 Fix compilation warnings (V.Ivanchenko)
// 13-02-03 SubCutoff regime is assigned to a region (V.Ivanchenko)
// 23-05-03 Add fluctuation model as a member function (V.Ivanchenko)
// 03-06-03 Add SetIntegral method to choose fluctuation model (V.Ivanchenko)
// 03-06-03 Fix initialisation problem for STD ionisation (V.Ivanchenko)
// 08-08-03 STD substitute standard  (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// 21-01-04 Migrade to G4ParticleChangeForLoss (V.Ivanchenko)
// ------------------------------------------------------------------------------------
// 15-04-05 Omit the correction of momentum direction due to production of secenderies.
//          This is needed for my Track Extrapotation. (L.L.Wang)
// ------------------------------------------------------------------------------------
//17-08-04 Rename the process "Mu" -> "mu" (V.Ivanchenko)
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)

// Class Description:
//
// This class manages the ionisation process for muons.
// it inherites from G4VContinuousDiscreteProcess via G4VEnergyLossProcess.
//

// -------------------------------------------------------------------
//

#ifndef ExtMuIonisation_h
#define ExtMuIonisation_h 1

#include "G4VEnergyLossProcess.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "globals.hh"
#include "G4VEmModel.hh"

class G4Material;
class G4VEmFluctuationModel;
class G4ParticleChangeForLoss;

class ExtMuIonisation : public G4VEnergyLossProcess
{

public:

  ExtMuIonisation(const G4String& name = "ExtmuIoni");

  virtual ~ExtMuIonisation();

  G4bool IsApplicable(const G4ParticleDefinition& p)
    {return (p.GetPDGCharge() != 0.0 && p.GetPDGMass() > 10.0*MeV);};

  G4double MinPrimaryEnergy(const G4ParticleDefinition* p,
                            const G4Material*, G4double cut);

/*  virtual std::vector<G4Track*>* SecondariesAlongStep(
                             const G4Step&,
			           G4double&,
			           G4double&,
                                   G4double&);
*/

  std::vector<G4DynamicParticle*>* SecondariesPostStep(
                                   G4VEmModel*,
                             const G4MaterialCutsCouple*,
                             const G4DynamicParticle*,
                                   G4double& tcut);

  // Print out of the class parameters
  void PrintInfo();

protected:

  virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition*,
		                           const G4ParticleDefinition*);

//  virtual G4double MaxSecondaryEnergy(const G4DynamicParticle* dynParticle);

private:

  // hide assignment operator
  ExtMuIonisation & operator=(const ExtMuIonisation &right);
  ExtMuIonisation(const ExtMuIonisation&);

  G4double    mass;
  G4double    ratio;

  const G4ParticleDefinition* theParticle;
  const G4ParticleDefinition* theBaseParticle;
  G4VEmFluctuationModel*      flucModel;

  G4ParticleChangeForLoss*    fParticleChange;
//  G4bool                 subCutoff;
  G4bool                 isInitialised;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double ExtMuIonisation::MinPrimaryEnergy(const G4ParticleDefinition*,
                                                    const G4Material*,
                                                          G4double cut)
{
  G4double x = 0.5*cut/electron_mass_c2;
  G4double g = x*ratio + sqrt((1. + x)*(1. + x*ratio*ratio));
  return mass*(g - 1.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*inline G4double ExtMuIonisation::MaxSecondaryEnergy(const G4DynamicParticle* dynParticle)
{
  G4double tau  = dynParticle->GetKineticEnergy()/mass;
  G4double tmax = 2.0*electron_mass_c2*tau*(tau + 2.0)/
                  (1. + 2.0*(tau + 1.0)*ratio + ratio*ratio);

  return tmax;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4VSubCutoffProcessor.hh"
*/

/*inline std::vector<G4DynamicParticle*>*  ExtMuIonisation::SecondariesAlongStep(
                           const G4Step&   step,
	             	         G4double& tmax,
			         G4double& eloss,
                                 G4double& kinEnergy)
{
  std::vector<G4Track*>* newp = 0;
  if(subCutoff) {
    G4VSubCutoffProcessor* sp = SubCutoffProcessor(CurrentMaterialCutsCoupleIndex());
    if (sp) {
      G4VEmModel* model = SelectModel(kinEnergy);
      newp = sp->SampleSecondaries(step,tmax,eloss,model);
    }
  }
  return newp;
}
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//#include "G4VEmModel.hh"

inline std::vector<G4DynamicParticle*>* ExtMuIonisation::SecondariesPostStep(
                                                   G4VEmModel* model,
                                             const G4MaterialCutsCouple* couple,
                                             const G4DynamicParticle* dp,
                                                   G4double& tcut)
{
//  return model->SampleSecondaries(couple, dp, tcut);
  return NULL;//No secondaries, for Track Extrapolation.(Wang L.L.)
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
