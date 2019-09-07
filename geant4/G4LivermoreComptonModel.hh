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
// $Id: G4LivermoreComptonModel.hh,v 1.3 2009/04/17 10:29:20 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
//
// Author: Sebastien Inserti
//         30 October 2008
//
// History:
// --------

#ifndef G4LivermoreComptonModel_h
#define G4LivermoreComptonModel_h 1

#include "G4VEmModel.hh"
#include "G4Electron.hh"
#include "G4ParticleChangeForGamma.hh"
#include "G4CrossSectionHandler.hh"
#include "G4LogLogInterpolation.hh"
#include "G4CompositeEMDataSet.hh"
#include "G4ShellData.hh"
#include "G4DopplerProfile.hh"
#include "G4ForceCondition.hh"
#include "G4Gamma.hh"

class G4LivermoreComptonModel : public G4VEmModel
{

public:

  G4LivermoreComptonModel(const G4ParticleDefinition* p = 0, 
		          const G4String& nam = "LivermoreCompton");

  virtual ~G4LivermoreComptonModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double ComputeCrossSectionPerAtom( const G4ParticleDefinition*,
                                               G4double kinEnergy, 
                                               G4double Z, 
                                               G4double A=0, 
                                               G4double cut=0,
                                               G4double emax=DBL_MAX );

  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

protected:

  G4ParticleChangeForGamma* fParticleChange;

  //G4double GetMeanFreePath(const G4Track& track, 
  //			   G4double previousStepSize, 
  //			   G4ForceCondition* condition);
private:

  G4double lowEnergyLimit;  
  G4double highEnergyLimit; 
  G4bool isInitialised;
  G4int verboseLevel;
  
  G4VEMDataSet* meanFreePathTable;
  G4VEMDataSet* scatterFunctionData;
  G4VCrossSectionHandler* crossSectionHandler;

  G4ShellData shellData;
  G4DopplerProfile profileData;

  G4LivermoreComptonModel & operator=(const  G4LivermoreComptonModel &right);
  G4LivermoreComptonModel(const  G4LivermoreComptonModel&);

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif
