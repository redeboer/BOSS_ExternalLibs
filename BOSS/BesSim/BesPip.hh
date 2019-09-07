//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Descpirtion: Beam Pipe detector as a concrete class
//             it's better to define an envelope then position it in BES
//Author: Liuhm
//Created: May 21, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesPip_H
#define BesPip_H 1

#include "BesSubdetector.hh"
#include "globals.hh"
class G4LogicalVolume;
class BesPipParameter;

class BesPip : public BesSubdetector
{  
  public:
  
   BesPip();
   virtual ~BesPip(){;}

   //construct EMC detector
   
   void Construct(G4LogicalVolume* logicbes);
   void DefineMaterial();
   
  private: 
  //your private data member (geometry, etc) here:
  BesPipParameter* pipPar;
  G4double goldLayer[3];
  G4double innerBe[3];
  G4double oilLayer[3];
  G4double outerBe[3];

  G4double innerBeSide[5];
  G4double innerAl[5];
  G4double AlRing[5];
  G4double outerAl[5];
  G4double AgLayer[5];
  G4double CuLayer[5];
  G4double AlCover[5];
  

  G4LogicalVolume* logicalPip;
  G4VPhysicalVolume* physicalPip;

  G4LogicalVolume* logicalgoldLayer;
  G4VPhysicalVolume* physicalgoldLayer;

  G4LogicalVolume* logicalinnerBe;
  G4VPhysicalVolume* physicalinnerBe;

  G4LogicalVolume* logicaloilLayer;
  G4VPhysicalVolume* physicaloilLayer;

  G4LogicalVolume* logicalouterBe;
  G4VPhysicalVolume* physicalouterBe;

  G4LogicalVolume* logicalinnerBeSide;
  G4VPhysicalVolume* physicalinnerBeSide; 

  G4LogicalVolume* logicalinnerAl;
  G4VPhysicalVolume* physicalinnerAl;

  G4LogicalVolume* logicalAlRing;
  G4VPhysicalVolume* physicalAlRing;

  G4LogicalVolume* logicalouterAl;
  G4VPhysicalVolume* physicalouterAl;

  G4LogicalVolume* logicalAgLayer;
  G4VPhysicalVolume* physicalAgLayer;

  G4LogicalVolume* logicalCuLayer;
  G4VPhysicalVolume* physicalCuLayer;

  G4LogicalVolume* logicalAlCover;
  G4VPhysicalVolume* physicalAlCover;

  G4Material* Oil;
  G4Material* Au;
  G4Material* Ag;
};
#endif





