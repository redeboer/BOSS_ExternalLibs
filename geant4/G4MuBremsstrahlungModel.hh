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
// $Id: G4MuBremsstrahlungModel.hh,v 1.22 2009/02/20 14:48:16 vnivanch Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4MuBremsstrahlungModel
//
// Author:        Vladimir Ivanchenko on base of Laszlo Urban code
// 
// Creation date: 18.05.2002
//
// Modifications:
//
// 23-12-02 Change interface in order to move to cut per region (V.Ivanchenko)
// 27-01-03 Make models region aware (V.Ivanchenko)
// 13-02-03 Add name (V.Ivanchenko)
// 10-02-04 Add lowestKinEnergy (V.Ivanchenko)
// 08-04-05 Major optimisation of internal interfaces (V.Ivantchenko)
// 13-02-06 Add ComputeCrossSectionPerAtom (mma)
// 11-10-07 Add ignoreCut flag (V.Ivanchenko) 
// 28-02-08 Reorganized protected methods and members (V.Ivanchenko) 
// 06-03-08 Remove obsolete methods and members (V.Ivanchenko) 
//

//
// Class Description:
//
// Implementation of bremssrahlung by muons

// -------------------------------------------------------------------
//

#ifndef G4MuBremsstrahlungModel_h
#define G4MuBremsstrahlungModel_h 1

#include "G4VEmModel.hh"
#include "G4NistManager.hh"

class G4Element;
class G4ParticleChangeForLoss;

class G4MuBremsstrahlungModel : public G4VEmModel
{

public:

  G4MuBremsstrahlungModel(const G4ParticleDefinition* p = 0,
                          const G4String& nam = "MuBrem");

  virtual ~G4MuBremsstrahlungModel();

  virtual void Initialise(const G4ParticleDefinition*, const G4DataVector&);

  virtual G4double MinEnergyCut(const G4ParticleDefinition*,
				const G4MaterialCutsCouple*);
			      
  virtual G4double ComputeCrossSectionPerAtom(
				 const G4ParticleDefinition*,
				 G4double kineticEnergy,
				 G4double Z, G4double A,
				 G4double cutEnergy,
				 G4double maxEnergy);
				 			       
  virtual G4double ComputeDEDXPerVolume(const G4Material*,
                                const G4ParticleDefinition*,
                                G4double kineticEnergy,
                                G4double cutEnergy);
			      
  virtual void SampleSecondaries(std::vector<G4DynamicParticle*>*,
				 const G4MaterialCutsCouple*,
				 const G4DynamicParticle*,
				 G4double tmin,
				 G4double maxEnergy);

  inline void SetLowestKineticEnergy(G4double e);

protected:

  G4double ComputMuBremLoss(G4double Z, G4double tkin, G4double cut);
  
  G4double ComputeMicroscopicCrossSection(G4double tkin,
					  G4double Z,
					  G4double cut);

  virtual G4double ComputeDMicroscopicCrossSection(G4double tkin,
						   G4double Z,
						   G4double gammaEnergy);

  inline void SetParticle(const G4ParticleDefinition*);

private:

  G4DataVector* ComputePartialSumSigma(const G4Material* material,
				       G4double tkin, G4double cut);

  const G4Element* SelectRandomAtom(const G4MaterialCutsCouple* couple) const;

  // hide assignment operator
  G4MuBremsstrahlungModel & operator=(const  G4MuBremsstrahlungModel &right);
  G4MuBremsstrahlungModel(const  G4MuBremsstrahlungModel&);

protected:

  const G4ParticleDefinition* particle;
  G4NistManager* nist;
  G4double mass;
  G4double rmass;
  G4double cc;
  G4double coeff;
  G4double sqrte;
  G4double bh;
  G4double bh1;
  G4double btf;
  G4double btf1;

private:

  G4ParticleDefinition*       theGamma;
  G4ParticleChangeForLoss*    fParticleChange;

  G4double highKinEnergy;
  G4double lowKinEnergy;
  G4double lowestKinEnergy;
  G4double minThreshold;

  std::vector<G4DataVector*> partialSumSigma;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void G4MuBremsstrahlungModel::SetLowestKineticEnergy(G4double e) 
{
  lowestKinEnergy = e;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline
void G4MuBremsstrahlungModel::SetParticle(const G4ParticleDefinition* p)
{
  if(!particle) {
    particle = p;
    mass = particle->GetPDGMass();
    rmass=mass/electron_mass_c2 ;
    cc=classic_electr_radius/rmass ;
    coeff= 16.*fine_structure_const*cc*cc/3. ;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
