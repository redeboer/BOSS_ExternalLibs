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
// $Id: ExteIonisation.cxx,v 1.7 2015/10/27 11:02:53 sunss Exp $
// GEANT4 tag $Name: TrkExtAlg-00-00-65 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:     ExteIonisation
//
// Author:        Laszlo Urban
//
// Creation date: 20.03.1997
//
// Modifications:
//
// 07-04-98 remove 'tracking cut' of the ionizing particle, mma
// 04-09-98 new methods SetBining() PrintInfo()
// 07-09-98 Cleanup
// 02-02-99 correction inDoIt , L.Urban
// 10-02-00 modifications , new e.m. structure, L.Urban
// 28-05-01 V.Ivanchenko minor changes to provide ANSI -wall compilation
// 09-08-01 new methods Store/Retrieve PhysicsTable (mma)
// 13-08-01 new function ComputeRestrictedMeandEdx()  (mma)
// 17-09-01 migration of Materials to pure STL (mma)
// 21-09-01 completion of RetrievePhysicsTable() (mma)
// 29-10-01 all static functions no more inlined (mma)
// 07-11-01 particleMass and Charge become local variables
// 26-03-02 change access to cuts in BuildLossTables (V.Ivanchenko)
// 30-04-02 V.Ivanchenko update to new design
// 23-12-02 Change interface in order to move to cut per region (VI)
// 26-12-02 Secondary production moved to derived classes (VI)
// 13-02-03 SubCutoff regime is assigned to a region (V.Ivanchenko)
// 23-05-03 Define default integral + BohrFluctuations (V.Ivanchenko)
// 03-06-03 Fix initialisation problem for STD ionisation (V.Ivanchenko)
// 08-08-03 STD substitute standard  (V.Ivanchenko)
// 12-11-03 G4EnergyLossSTD -> G4EnergyLossProcess (V.Ivanchenko)
// ------------------------------------------------------------------------------------ 
// 15-04-05 Omit the correction of momentum direction due to production of secenderies.
//          This is needed for my Track Extrapotation. (L.L.Wang)
// ------------------------------------------------------------------------------------
// 08-11-04 Migration to new interface of Store/Retrieve tables (V.Ivantchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "TrkExtAlg/ExteIonisation.h"
#include "G4Electron.hh"
#include "G4MollerBhabhaModel.hh"
#include "G4UniversalFluctuation.hh"
#include "G4BohrFluctuations.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

using namespace std;

ExteIonisation::ExteIonisation(const G4String& name)
  : G4VEnergyLossProcess(name),
    theElectron(G4Electron::Electron()),
    isElectron(true),
    isInitialised(false)
{
  SetDEDXBinning(120);
  SetLambdaBinning(120);
  SetMinKinEnergy(0.1*keV);
  SetMaxKinEnergy(100.0*TeV);
  SetLossFluctuations(false);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

ExteIonisation::~ExteIonisation()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void ExteIonisation::InitialiseEnergyLossProcess(const G4ParticleDefinition* part,const G4ParticleDefinition*)
{
  if(!isInitialised) {
	  if(part == G4Positron::Positron()) isElectron = false;
	  SetSecondaryParticle(theElectron);

	  flucModel = new G4UniversalFluctuation();

	  G4VEmModel* em = new G4MollerBhabhaModel();
	  em->SetLowEnergyLimit(0.1*keV);
	  em->SetHighEnergyLimit(100.0*TeV);
	  AddEmModel(1, em, flucModel);
	  isInitialised = true;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void ExteIonisation::PrintInfo()
{
  G4cout << "      Delta cross sections from Moller+Bhabha, "
         << "good description from 1 KeV to 100 GeV."
         << G4endl;
}

//....ooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
