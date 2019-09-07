//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: BES beam pipe 
//Author: Liuhm
//Created: May 21, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesPip.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UnionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "BesPipParameter.hh"
#include "BesSubdetector.hh"

BesPip::BesPip()
{
  pipPar = new BesPipParameter();
  pipPar->ReadData();
  for(G4int i=0; i<3; i++)
  {
    goldLayer[i] = pipPar->getGoldLayer(i);
    innerBe[i] = pipPar->getInnerBe(i);
    oilLayer[i] = pipPar->getOilLayer(i);
    outerBe[i] = pipPar->getOuterBe(i);
  }

  for(G4int i=0; i<5; i++)
  {
    innerBeSide[i] = pipPar->getInnerBeSide(i);
    innerAl[i] = pipPar->getInnerAl(i);
    AlRing[i] = pipPar->getAlRing(i);
    outerAl[i] = pipPar->getOuterAl(i);
    AgLayer[i] = pipPar->getAgLayer(i);
    CuLayer[i] = pipPar->getCuLayer(i);
    AlCover[i] = pipPar->getAlCover(i);
  }

  logicalPip = 0;
  physicalPip = 0;

  logicalgoldLayer = 0;
  physicalgoldLayer = 0;

  logicalinnerBe = 0;
  physicalinnerBe = 0;

  logicaloilLayer = 0;
  physicaloilLayer = 0;

  logicalouterBe = 0;
  physicalouterBe = 0;

   logicalinnerBeSide = 0;
   physicalinnerBeSide = 0;

   logicalinnerAl = 0;
   physicalinnerAl = 0;

   logicalAlRing = 0;
   physicalAlRing = 0;

   logicalouterAl = 0;
   physicalouterAl = 0;
  
   logicalAgLayer = 0;
   physicalAgLayer = 0;

   logicalCuLayer = 0;
   physicalCuLayer = 0;

   logicalAlCover = 0;
   physicalAlCover = 0;

  Au = 0;
  Ag = 0;
  Oil = 0;
}

void BesPip::DefineMaterial()
{
  G4double density, a, z,fractionmass;
  G4int nel,natoms;
  G4String name, symbol;

  density=19.32*g/cm3;
  a = 196.967*g/mole;
  Au= new G4Material("Gold",79,a,density);

  density=10.5*g/cm3;
  a = 107.9*g/mole;
  Ag= new G4Material("Silver",47,a,density);

  a = 1.01*g/mole;
  G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
    
  a = 12.01*g/mole;
  G4Element* C  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);

  density = 0.810*g/cm3;
  Oil = new G4Material(name="Oil",density,nel=2);
  Oil->AddElement(C, natoms=18);
  Oil->AddElement(H, natoms=38);
}

void BesPip::Construct(G4LogicalVolume* logicalbes)
{
  DefineMaterial();

   //G4RotationMatrix* xRot = new G4RotationMatrix;
  //xRot->rotateX(90*deg);

  //the logical volume of beam pipe
  G4Tubs* solidPip1 = new G4Tubs("solidPip1",0.,33.7,134,0,360);
  G4Tubs* solidPip2 = new G4Tubs("solidPip2",0.,48,66,0,360);
  G4UnionSolid* solidPip_tmp = new G4UnionSolid("solidPip_tmp",solidPip1,solidPip2,0,G4ThreeVector(0,0,-167));
  G4UnionSolid* solidPip = new G4UnionSolid("solidPip",solidPip_tmp,solidPip2,0,G4ThreeVector(0,0,167));
  logicalPip = new G4LogicalVolume(solidPip, G4Material::GetMaterial("Beam"),"logicalPip");
  physicalPip = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalPip,"physicalPip",logicalbes,false,0);

  //the volume of gold layer
  G4Tubs* solidgoldLayer = new G4Tubs("solidgoldLayer",goldLayer[0],goldLayer[1],goldLayer[2]/2,0,360);
  logicalgoldLayer = new G4LogicalVolume(solidgoldLayer, Au,"logicalgoldLayer");
  physicalgoldLayer = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalgoldLayer,"physicalgoldLayer",logicalPip,false,0);

  //the volume of inner Be pipe
  G4Tubs* solidinnerBe = new G4Tubs("solidinnerBe",innerBe[0],innerBe[1],innerBe[2]/2,0,360);
  logicalinnerBe = new G4LogicalVolume(solidinnerBe, G4Material::GetMaterial("Beryllium"),"logicalinnerBe");
  physicalinnerBe = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalinnerBe,"physicalinnerBe",logicalPip,false,0);

  //the volume of oil layer
  G4Tubs* solidoilLayer = new G4Tubs("solidoilLayer",oilLayer[0],oilLayer[1],oilLayer[2]/2,0,360);
  logicaloilLayer = new G4LogicalVolume(solidoilLayer, Oil,"logicaloilLayer");
  physicaloilLayer = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaloilLayer,"physicaloilLayer",logicalPip,false,0);

  //the volume of outer Be pipe
   G4Tubs* solidouterBe = new G4Tubs("solidouterBe",outerBe[0],outerBe[1],outerBe[2]/2,0,360);
  logicalouterBe = new G4LogicalVolume(solidouterBe, G4Material::GetMaterial("Beryllium"),"logicalouterBe");
  physicalouterBe = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalouterBe,"physicalouterBe",logicalPip,false,0);

  //the volume of inner side Be layer
   G4Tubs* solidinnerBeSide = new G4Tubs("solidinnerBeSide",innerBeSide[0],innerBeSide[1],innerBeSide[2]/2,0,360);
  logicalinnerBeSide = new G4LogicalVolume(solidinnerBeSide, G4Material::GetMaterial("Beryllium"),"logicalinnerBeSide");
  physicalinnerBeSide = new G4PVPlacement(0,G4ThreeVector(0,0,innerBeSide[3]),logicalinnerBeSide,"physicalinnerBeSide1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,innerBeSide[4]),logicalinnerBeSide,"physicalinnerBeSide2",logicalPip,false,1);

  //the volume of inner Al layer
   G4Tubs* solidinnerAl = new G4Tubs("solidinnerAl",innerAl[0],innerAl[1],innerAl[2]/2,0,360);
  logicalinnerAl = new G4LogicalVolume(solidinnerAl, G4Material::GetMaterial("Aluminium"),"logicalinnerAl");
  physicalinnerAl = new G4PVPlacement(0,G4ThreeVector(0,0,innerAl[3]),logicalinnerAl,"physicalinnerAl1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,innerAl[4]),logicalinnerAl,"physicalinnerAl2",logicalPip,false,1);

  //the volume of Al ring
   G4Tubs* solidAlRing = new G4Tubs("solidAlRing",AlRing[0],AlRing[1],AlRing[2]/2,0,360);
  logicalAlRing = new G4LogicalVolume(solidAlRing, G4Material::GetMaterial("Aluminium"),"logicalAlRing");
  physicalAlRing = new G4PVPlacement(0,G4ThreeVector(0,0,AlRing[3]),logicalAlRing,"physicalAlRing1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,AlRing[4]),logicalAlRing,"physicalAlRing2",logicalPip,false,1);

  //the volume of outer Al layer
   G4Tubs* solidouterAl = new G4Tubs("solidouterAl",outerAl[0],outerAl[1],outerAl[2]/2,0,360);
  logicalouterAl = new G4LogicalVolume(solidouterAl, G4Material::GetMaterial("Aluminium"),"logicalouterAl");
  physicalouterAl = new G4PVPlacement(0,G4ThreeVector(0,0,outerAl[3]),logicalouterAl,"physicalouterAl1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,outerAl[4]),logicalouterAl,"physicalouterAl2",logicalPip,false,1);

  //the volume of Ag layer
   G4Tubs* solidAgLayer = new G4Tubs("solidAgLayer",AgLayer[0],AgLayer[1],AgLayer[2]/2,0,360);
  logicalAgLayer = new G4LogicalVolume(solidAgLayer, Ag,"logicalAgLayer");
  physicalAgLayer = new G4PVPlacement(0,G4ThreeVector(0,0,AgLayer[3]),logicalAgLayer,"physicalAgLayer1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,AgLayer[4]),logicalAgLayer,"physicalAgLayer2",logicalPip,false,1);

  //the volume of  Cu layer
   G4Tubs* solidCuLayer = new G4Tubs("solidCuLayer",CuLayer[0],CuLayer[1],CuLayer[2]/2,0,360);
  logicalCuLayer = new G4LogicalVolume(solidCuLayer, G4Material::GetMaterial("Copper"),"logicalCuLayer");
  physicalCuLayer = new G4PVPlacement(0,G4ThreeVector(0,0,CuLayer[3]),logicalCuLayer,"physicalCuLayer1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,CuLayer[4]),logicalCuLayer,"physicalCuLayer2",logicalPip,false,1);

  //the volume of Al cover
   G4Tubs* solidAlCover = new G4Tubs("solidAlCover",AlCover[0],AlCover[1],AlCover[2]/2,0,360);
  logicalAlCover = new G4LogicalVolume(solidAlCover, G4Material::GetMaterial("Aluminium"),"logicalAlCover");
  physicalAlCover = new G4PVPlacement(0,G4ThreeVector(0,0,AlCover[3]),logicalAlCover,"physicalAlCover1",logicalPip,false,0);
                        new G4PVPlacement(0,G4ThreeVector(0,0,AlCover[4]),logicalAlCover,"physicalAlCover2",logicalPip,false,1);

  G4VisAttributes* visPip = new G4VisAttributes(G4Colour(0.,0.,1.));
  logicalPip->SetVisAttributes(visPip);
  //logicalPip->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visgold = new G4VisAttributes(G4Colour(1.,1.,0.));
  logicalgoldLayer->SetVisAttributes(visgold);
//  logicalgoldLayer->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visBe = new G4VisAttributes(G4Colour(0.,1.,0.));
  logicalinnerBe->SetVisAttributes(visBe);
//  logicalinnerBe->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visOil = new G4VisAttributes(G4Colour(1.,1.,1.));
  logicaloilLayer->SetVisAttributes(visOil);
//  logicaloilLayer->SetVisAttributes(G4VisAttributes::Invisible);
  logicalouterBe->SetVisAttributes(visBe);
//  logicalouterBe->SetVisAttributes(G4VisAttributes::Invisible);
  logicalinnerBeSide->SetVisAttributes(visBe);
//  logicalinnerBeSide->SetVisAttributes(G4VisAttributes::Invisible);
  logicalinnerAl->SetVisAttributes(visOil);
//  logicalinnerAl->SetVisAttributes(G4VisAttributes::Invisible);
  logicalAlRing->SetVisAttributes(visOil);
//  logicalAlRing->SetVisAttributes(G4VisAttributes::Invisible);
  logicalouterAl->SetVisAttributes(visOil);
//  logicalouterAl->SetVisAttributes(G4VisAttributes::Invisible);
  logicalAgLayer->SetVisAttributes(visBe);
//  logicalAgLayer->SetVisAttributes(G4VisAttributes::Invisible);
  logicalCuLayer->SetVisAttributes(visPip);
//  logicalCuLayer->SetVisAttributes(G4VisAttributes::Invisible);
  logicalAlCover->SetVisAttributes(visOil);
//  logicalAlCover->SetVisAttributes(G4VisAttributes::Invisible);
}
