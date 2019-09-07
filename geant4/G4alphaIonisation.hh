//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: G4alphaIonisation.hh,v 1.1 2009/11/10 11:50:30 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4alphaIonisation
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 28.10.2009
//
// Modifications:
//
//
// Class Description:
//
// This class manages the ionisation process for He ions. Effective charge,
// nuclear stopping power, energy loss corrections are taken into account.
// It inherites from G4VEnergyLossLoss.
//

// -------------------------------------------------------------------
//

#ifndef G4alphaIonisation_h
#define G4alphaIonisation_h 1

#include "G4VEnergyLossProcess.hh"

class G4Material;

class G4alphaIonisation : public G4VEnergyLossProcess
{
public:

  G4alphaIonisation(const G4String& name = "alphaIoni");

  virtual ~G4alphaIonisation();

  virtual G4bool IsApplicable(const G4ParticleDefinition& p);

  // Print out of the class parameters
  virtual void PrintInfo();

  void ActivateNuclearStopping(G4bool);

protected:

  virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition*,
					   const G4ParticleDefinition*);

  virtual G4double MinPrimaryEnergy(const G4ParticleDefinition* p,
				   const G4Material*, G4double cut);

  inline G4double BetheBlochEnergyThreshold();

  inline G4bool NuclearStoppingFlag();

private:

  // hide assignment operator
  G4alphaIonisation & operator=(const G4alphaIonisation &right);
  G4alphaIonisation(const G4alphaIonisation&);

  const G4ParticleDefinition* theParticle;

  G4double   mass;
  G4double   ratio;
  G4double   eth;

  G4bool     isInitialised;
  G4bool     nuclearStopping;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void G4alphaIonisation::ActivateNuclearStopping(G4bool val)
{
  nuclearStopping = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4double G4alphaIonisation::BetheBlochEnergyThreshold()
{
  return eth;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline G4bool G4alphaIonisation::NuclearStoppingFlag()
{
  return nuclearStopping;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
