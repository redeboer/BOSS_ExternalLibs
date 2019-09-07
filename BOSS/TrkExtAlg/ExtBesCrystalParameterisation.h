//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef ExtBesCrystalParameterisation_H
#define ExtBhesCrystalParameterisation_H 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"

//class G4VphysicalVolume;
//class G4Trap;
//class G4VSolid;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class ExtBesEmcGeometry;

class ExtBesCrystalParameterisation : public G4VPVParameterisation
{
public:
  ExtBesCrystalParameterisation(
			      const G4int ,
			      const G4int ,
			      const G4int ,
			      ExtBesEmcGeometry*,
			      const G4int);
  virtual ~ExtBesCrystalParameterisation();
  void ComputeTransformation (
			      const G4int ,
			      G4VPhysicalVolume* )const;
  void ComputeDimensions (G4Trap & , const G4int ,
			  const G4VPhysicalVolume* )const;
  G4Material* ComputeMaterial(const G4int,G4VPhysicalVolume *);
  G4VSolid*   ComputeSolid(const G4int,G4VPhysicalVolume *);
  void ComputeIDAndSide(const G4VPhysicalVolume*)const;
  inline void SetVerboseLevel(G4int val){fVerboseLevel=val;};
private:
  G4int fCrystalID;
  G4int fVerboseLevel;
  G4int fStartID;
  G4int fAllCrystals;
  G4bool fFlagLeft;
  ExtBesEmcGeometry* fBesEmcGeometry;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
