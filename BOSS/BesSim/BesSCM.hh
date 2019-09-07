//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Superconducting Magnet detector as a concrete class
//             it's better to define an envelope then position it in BES
//Author: Cao Guofu
//Created: July 27, 2007
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesSCM_H
#define BesSCM_H 1

#include "BesSubdetector.hh"
#include "globals.hh"
#include "G4VPhysicalVolume.hh"

class G4LogicalVolume;
class BesSCMParameter;

class BesSCM : public BesSubdetector
{  
public:
		    
  BesSCM();
  virtual ~BesSCM(){;}

  void Construct(G4LogicalVolume* logicbes);
  void DefineMaterial();

private:
    BesSCMParameter* scmPar;
    G4double innerCryo[3];
    G4double l1Adia[3];
    G4double innerShield[3];
    G4double l2Adia[3];
    G4double outerShield[3];
    G4double l3Adia[3];
    G4double endShield[5];
    G4double l1insu[3];
    G4double coil[3];
    G4double l2insu[3];
    G4double supp[3];
    G4double l4Adia[3];
    G4double endCoil[5];
    G4double outerCryo[3];
    G4double endCryo[5];
    G4double rein[5];
    G4double pipe1[6];
    G4double pipe2[6];
    G4double pipe3[6];
    G4double hole[13];

    G4LogicalVolume* logicalSCM;
    G4VPhysicalVolume* physicalSCM;

    G4LogicalVolume* logicalinnCryo;
    G4VPhysicalVolume* physicalinnCryo;

    G4LogicalVolume* logicall1Adia;
    G4VPhysicalVolume* physicall1Adia;

    G4LogicalVolume* logicalinnShield;
    G4VPhysicalVolume* physicalinnShield;

    G4LogicalVolume* logicall2Adia;
    G4VPhysicalVolume* physicall2Adia;

    G4LogicalVolume* logicaloutShield;
    G4VPhysicalVolume* physicaloutShield;

    G4LogicalVolume* logicall3Adia;
    G4VPhysicalVolume* physicall3Adia;

    G4LogicalVolume* logicalendShield;
    G4VPhysicalVolume* physicalendShield1;
    G4VPhysicalVolume* physicalendShield2;

    G4LogicalVolume* logicall1insu;
    G4VPhysicalVolume* physicall1insu;

    G4LogicalVolume* logicalcoil;
    G4VPhysicalVolume* physicalcoil;

    G4LogicalVolume* logicall2insu;
    G4VPhysicalVolume* physicall2insu;

    G4LogicalVolume* logicalsupp;
    G4VPhysicalVolume* physicalsupp;

    G4LogicalVolume* logicall4Adia;
    G4VPhysicalVolume* physicall4Adia;

    G4LogicalVolume* logicalendCoil;
    G4VPhysicalVolume* physicalendCoil1;
    G4VPhysicalVolume* physicalendCoil2;

    G4LogicalVolume* logicalouterCryo;
    G4VPhysicalVolume* physicalouterCryo;

    G4LogicalVolume* logicalendCryo;
    G4VPhysicalVolume* physicalendCryo1;
    G4VPhysicalVolume* physicalendCryo2;

    G4LogicalVolume* logicalrein;
    G4VPhysicalVolume* physicalrein1;
    G4VPhysicalVolume* physicalrein2;

    G4LogicalVolume* logicalpipe1;
    G4VPhysicalVolume* physicalpipe1;

    G4LogicalVolume* logicalpipe2;
    G4VPhysicalVolume* physicalpipe2;

    G4LogicalVolume* logicalpipe3;
    G4VPhysicalVolume* physicalpipe3;

    G4Material* liquidhelium;
    G4Material* Steel;
    G4Material* fibre;
};
#endif
