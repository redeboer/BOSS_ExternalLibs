//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesCrystalParameterisation_H
#define BesCrystalParameterisation_H 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"

//class G4VphysicalVolume;
//class G4Trap;
//class G4VSolid;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
class BesEmcGeometry;

class BesCrystalParameterisation : public G4VPVParameterisation
{
public:
  BesCrystalParameterisation(
			      const G4int ,
			      const G4int ,
			      const G4int ,
			      BesEmcGeometry*,
			      const G4int);
  virtual ~BesCrystalParameterisation();
  void ComputeTransformation (
			      const G4int ,
			      G4VPhysicalVolume* )const;

  virtual void ComputeDimensions(G4Box &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Tubs &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Trd &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Trap &,
      const G4int,
      const G4VPhysicalVolume *) const ;

  virtual void ComputeDimensions(G4Cons &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Sphere &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Orb &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Torus &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Para &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Polycone &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Polyhedra &,
      const G4int,
      const G4VPhysicalVolume *) const {}

  virtual void ComputeDimensions(G4Hype &,
      const G4int,
      const G4VPhysicalVolume *) const {}



  G4Material* ComputeMaterial(const G4int,G4VPhysicalVolume *,
      const G4VTouchable *parentTouch=0);
  G4VSolid*   ComputeSolid(const G4int,G4VPhysicalVolume *);
  void ComputeIDAndSide(const G4VPhysicalVolume*)const;
  inline void SetVerboseLevel(G4int val){fVerboseLevel=val;};
private:
  G4int fCrystalID;
  G4int fVerboseLevel;
  G4int fStartID;
  G4int fAllCrystals;
  G4bool fFlagLeft;
  BesEmcGeometry* fBesEmcGeometry;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
