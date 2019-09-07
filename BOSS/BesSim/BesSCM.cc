//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
////---------------------------------------------------------------------------//
////Descpirtion: BES beam pipe 
////Author: Liuhm
////Created: May 21, 2003
////Comment:
////---------------------------------------------------------------------------//
////
#include "BesSCM.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trap.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "BesSubdetector.hh"
#include "BesSCMParameter.hh"

BesSCM::BesSCM()
{
  scmPar = new BesSCMParameter();
  scmPar->ReadData();
  for(G4int i=0; i<3; i++)
  {
    innerCryo[i] = scmPar->getInnerCryo(i);
    l1Adia[i] = scmPar->getL1Adia(i);
    innerShield[i] = scmPar->getInnerShield(i);
    l2Adia[i] = scmPar->getL2Adia(i);
    outerShield[i] = scmPar->getOuterShield(i);
    l3Adia[i] = scmPar->getL3Adia(i);
    l1insu[i] = scmPar->getL1insu(i);
    coil[i] = scmPar->getCoil(i);
    l2insu[i] = scmPar->getL2insu(i);
    supp[i] = scmPar->getSupp(i);
    l4Adia[i] = scmPar->getL4Adia(i);
    outerCryo[i] = scmPar->getOuterCryo(i);
  }
  for(G4int i=0; i<5; i++)
  {
    endShield[i] = scmPar->getEndShield(i);
    endCoil[i] = scmPar->getEndCoil(i);
    endCryo[i] = scmPar->getEndCryo(i);
    rein[i] = scmPar->getRein(i);
  }

  for(G4int i=0; i<6; i++)
  {
    pipe1[i] = scmPar->getPipe1(i);
    pipe2[i] = scmPar->getPipe2(i);
    pipe3[i] = scmPar->getPipe3(i);
  }

  for(G4int i=0; i<13; i++)
  {
    hole[i] = scmPar->getHole(i);
  }

  logicalinnCryo = 0;
  physicalinnCryo = 0;

  logicall1Adia = 0;
  physicall1Adia = 0;

  logicalinnShield = 0;
  physicalinnShield = 0;

  logicall2Adia = 0;
  physicall2Adia = 0;

  logicaloutShield = 0;
  physicaloutShield = 0;

  logicall3Adia = 0;
  physicall3Adia = 0;

  logicalendShield = 0;
  physicalendShield1 = 0;
  physicalendShield2 = 0;

  logicall1insu = 0;
  physicall1insu = 0;

  logicalcoil = 0;
  physicalcoil = 0;

  logicall2insu = 0;
  physicall2insu = 0;

  logicalsupp = 0;
  physicalsupp = 0;

  logicall4Adia = 0;
  physicall4Adia = 0;

  logicalendCoil = 0;
  physicalendCoil1 = 0;
  physicalendCoil2 = 0;

  logicalouterCryo = 0;
  physicalouterCryo = 0;

  logicalendCryo = 0;
  physicalendCryo1 = 0;
  physicalendCryo2 = 0;

  logicalrein = 0;
  physicalrein1 = 0;
  physicalrein2 = 0;

  logicalpipe1 = 0;
  physicalpipe1 = 0;

  logicalpipe2 = 0;
  physicalpipe2 = 0;

  logicalpipe3 = 0;
  physicalpipe3 = 0;

  liquidhelium = 0;
  Steel = 0;
  fibre = 0;
}

void BesSCM::DefineMaterial()
{
  G4double density, a, z,fractionmass;
  G4int nel;
  G4String name, symbol;
  density = 0.1248*g/cm3;
  a = 4.00*g/mole;
  liquidhelium = new G4Material(name="liquidhelium",z=2.0,a,density);

  density = 7.14*g/cm3;
  a = 52.00*g/mole;
  G4Material* Chromium = new G4Material(name="Chromium",z=24.0,a,density);

  density = 7.87*g/cm3;
  a = 55.845*g/mole;
  G4Material* Iron = new G4Material(name="Iron",z=26.0,a,density);

  density = 8.91*g/cm3;
  a = 58.693*g/mole;
  G4Material* Nickel = new G4Material(name="Nickel",z=28.0,a,density);

  density = 7.47*g/cm3;
  a = 54.938*g/mole;
  G4Material* Manganese = new G4Material(name="Manganese",z=25.0,a,density);

  density = 7.801*g/cm3;
  Steel = new G4Material(name="Steel",density,nel=4);
  Steel->AddMaterial(Iron, fractionmass = 0.69);
  Steel->AddMaterial(Chromium, fractionmass = 0.19);
  Steel->AddMaterial(Manganese, fractionmass = 0.02);
  Steel->AddMaterial(Nickel, fractionmass = 0.10);

  a = 12.01*g/mole;
  G4Element* C  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);
  a = 1.01*g/mole;
  G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
  a = 16.00*g/mole;
  G4Element* O  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);

  density = 2.25*mg/cm3;
  fibre = new G4Material(name="fibre",density,nel=3);
  fibre->AddElement(C, fractionmass = 0.625);
  fibre->AddElement(H, fractionmass = 0.042);
  fibre->AddElement(O, fractionmass = 0.333);
}

void BesSCM::Construct(G4LogicalVolume* logicalbes)
{
  DefineMaterial();

  G4RotationMatrix* xRot = new G4RotationMatrix;
  xRot->rotateX(90*deg);
  //the chimney of SCM from outer to inner
  G4Tubs* hole1 = new G4Tubs("hole1",hole[0],hole[1],hole[2]/2,0,360);
  G4LogicalVolume* logicalhole1 = new G4LogicalVolume(hole1, Steel,"logicalhole1");
  //G4VPhysicalVolume* physicalhole1 = 
  new G4PVPlacement(xRot,G4ThreeVector(0,(outerCryo[1]+hole[2]/2)*mm,1640*mm),logicalhole1,"physicalhole1",logicalbes,false,0);

  G4Tubs* hole2 = new G4Tubs("hole2",hole[3],hole[4],hole[5]/2,0,360);
  G4LogicalVolume* logicalhole2 = new G4LogicalVolume(hole2, Steel,"logicalhole2");
  //G4VPhysicalVolume* physicalhole2 = 
  new G4PVPlacement(xRot,G4ThreeVector(0,(outerCryo[1]+hole[2]+hole[5]/2)*mm,1640*mm),logicalhole2,"physicalhole2",logicalbes,false,0);

  G4Tubs* hole3 = new G4Tubs("hole3",hole[6],hole[7],hole[12]/2,0,360);
  G4LogicalVolume* logicalhole3 = new G4LogicalVolume(hole3, Steel,"logicalhole3");
  //G4VPhysicalVolume* physicalhole3 = 
  new G4PVPlacement(xRot,G4ThreeVector(0,(outerCryo[1]+hole[2]+hole[12]/2)*mm,1640*mm),logicalhole3,"physicalhole3",logicalbes,false,0);

  G4Tubs* hole4 = new G4Tubs("hole4",hole[8],hole[9],hole[12]/2,0,360);
  G4LogicalVolume* logicalhole4 = new G4LogicalVolume(hole4, G4Material::GetMaterial("Aluminium"),"logicalhole4");
  //G4VPhysicalVolume* physicalhole4 = 
  new G4PVPlacement(xRot,G4ThreeVector(0,(l3Adia[1]+pipe2[1]+pipe2[4]+hole[12]/2)*mm,1640*mm),logicalhole4,"physicalhole4",logicalbes,false,0);

  G4Box* hole5 = new G4Box("hole5",hole[10]/2,hole[11]/2,hole[12]/2);
  G4LogicalVolume* logicalhole5 = new G4LogicalVolume(hole5, G4Material::GetMaterial("Aluminium"),"logicalhole5");
  //G4VPhysicalVolume* physicalhole5 = 
  new G4PVPlacement(xRot,G4ThreeVector(0,(l3Adia[1]+pipe2[1]+pipe2[4]+hole[12]/2)*mm,1640*mm),logicalhole5,"physicalhole5",logicalbes,false,0);


  G4Tubs* solidSCM_tmp = new G4Tubs("solidSCM_tmp",innerCryo[0]*mm,1700*mm,(outerCryo[2]+2*endCryo[2])/2.0*mm,0,360);
  G4Tubs* hole_tmp1 = new G4Tubs("hole_tmp1",0,hole[1],(1700.-1617.5+2)/2.*mm,0,360);
  G4SubtractionSolid* solidSCM = new G4SubtractionSolid("solidSCM",solidSCM_tmp,hole_tmp1,xRot,G4ThreeVector(0,(1700.+1617.5+2)/2*mm,1640*mm) );
  logicalSCM = new G4LogicalVolume(solidSCM, G4Material::GetMaterial("Air"),"logicalSCM");
  physicalSCM = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalSCM,"physicalSCM",logicalbes,false,0);

  G4Tubs* tub1 = new G4Tubs("tub1",innerCryo[0]*mm,innerCryo[1]*mm,innerCryo[2]/2.0*mm,0,360);
  logicalinnCryo = new G4LogicalVolume(tub1, Steel,"logicalinnCryo");
  physicalinnCryo = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalinnCryo,"physicalinnCryo",logicalSCM,false,0);

  G4Tubs* tub2 = new G4Tubs("tub2",l1Adia[0]*mm,l1Adia[1]*mm,l1Adia[2]/2.0*mm,0,360);
  logicall1Adia = new G4LogicalVolume(tub2, fibre,"logicall1Adia");
  physicall1Adia = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall1Adia,"physicall1Adia",logicalSCM,false,0);

  G4Tubs* tub3 = new G4Tubs("tub3",innerShield[0]*mm,innerShield[1]*mm,innerShield[2]/2.0*mm,0,360);
  logicalinnShield = new G4LogicalVolume(tub3, G4Material::GetMaterial("Aluminium"),"logicalinnShield");
  physicalinnShield = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalinnShield,"physicalinnShield",logicalSCM,false,0);

  G4Tubs* tub4 = new G4Tubs("tub4",l2Adia[0]*mm,l2Adia[1]*mm,l2Adia[2]/2.0*mm,0,360);
  logicall2Adia = new G4LogicalVolume(tub4, fibre,"logicall2Adia");
  physicall2Adia = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall2Adia,"physicall2Adia",logicalSCM,false,0);

  G4Tubs* hole_tmp3 = new G4Tubs("hole_tmp3",0,hole[8]*mm,25*mm,0,360);
  G4Tubs* tub5_tmp = new G4Tubs("tub5_tmp",outerShield[0]*mm,outerShield[1]*mm,outerShield[2]/2.0*mm,0,360);
  G4SubtractionSolid* tub5 = new G4SubtractionSolid("tub5",tub5_tmp,hole_tmp3,xRot,G4ThreeVector(0,(outerShield[0]+outerShield[1])/2*mm,1640*mm) );
  logicaloutShield = new G4LogicalVolume(tub5, G4Material::GetMaterial("Aluminium"),"logicaloutShield");
  physicaloutShield = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaloutShield,"physicaloutShield",logicalSCM,false,0);

  G4Tubs* tub6_tmp = new G4Tubs("tub6_tmp",l3Adia[0]*mm,l3Adia[1]*mm,l3Adia[2]/2.0*mm,0,360);
  G4SubtractionSolid* tub6 = new G4SubtractionSolid("tub6",tub6_tmp,hole_tmp3,xRot,G4ThreeVector(0,(l3Adia[0]+l3Adia[1])/2*mm,1640*mm) );
  logicall3Adia = new G4LogicalVolume(tub6, fibre,"logicall3Adia");
  physicall3Adia = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall3Adia,"physicall3Adia",logicalSCM,false,0);

  G4Tubs* tub7 = new G4Tubs("tub7",endShield[0]*mm,endShield[1]*mm,endShield[2]/2.0*mm,0,360);
  logicalendShield = new G4LogicalVolume(tub7, G4Material::GetMaterial("Aluminium"),"logicalendShield");
  physicalendShield1 = new G4PVPlacement(0,G4ThreeVector(0,0,endShield[3]*mm),logicalendShield,"physicalendShield1",logicalSCM,false,0);
  physicalendShield2 = new G4PVPlacement(0,G4ThreeVector(0,0,endShield[4]*mm),logicalendShield,"physicalendShield2",logicalSCM,false,1);

  G4Tubs* tub8 = new G4Tubs("tub8",l1insu[0]*mm,l1insu[1]*mm,l1insu[2]/2.0*mm,0,360); 
  logicall1insu = new G4LogicalVolume(tub8, fibre,"logicall1insu");
  physicall1insu = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall1insu,"physicall1insu",logicalSCM,false,0);

  G4Tubs* tub9 = new G4Tubs("tub9",coil[0]*mm,coil[1]*mm,coil[2]/2.0*mm,0,360);
  logicalcoil = new G4LogicalVolume(tub9, G4Material::GetMaterial("Aluminium"),"logicalcoil");
  physicalcoil = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalcoil,"physicalcoil",logicalSCM,false,0);

  G4Tubs* tub10 = new G4Tubs("tub10",l2insu[0]*mm,l2insu[1]*mm,l2insu[2]/2.0*mm,0,360);
  logicall2insu = new G4LogicalVolume(tub10, fibre,"logicall2insu");
  physicall2insu = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall2insu,"physicall2insu",logicalSCM,false,0);

  G4Tubs* tub11 = new G4Tubs("tub11",supp[0]*mm,supp[1]*mm,supp[2]/2.0*mm,0,360);
  logicalsupp = new G4LogicalVolume(tub11, G4Material::GetMaterial("Aluminium"),"logicalsupp");
  physicalsupp = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalsupp,"physicalsupp",logicalSCM,false,0);

  G4Tubs* tub12 = new G4Tubs("tub12",l4Adia[0]*mm,l4Adia[1]*mm,l4Adia[2]/2.0*mm,0,360);
  logicall4Adia = new G4LogicalVolume(tub12, fibre,"logicall4Adia");
  physicall4Adia = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicall4Adia,"physicall4Adia",logicalSCM,false,0);

  G4Tubs* tub13 = new G4Tubs("tub13",endCoil[0]*mm,endCoil[1]*mm,endCoil[2]/2.0*mm,0,360);
  logicalendCoil = new G4LogicalVolume(tub13, G4Material::GetMaterial("Aluminium"),"logicalendCoil");
  physicalendCoil1 = new G4PVPlacement(0,G4ThreeVector(0,0,endCoil[3]*mm),logicalendCoil,"physicalendCoil1",logicalSCM,false,0);
  physicalendCoil2 = new G4PVPlacement(0,G4ThreeVector(0,0,endCoil[4]*mm),logicalendCoil,"physicalendCoil2",logicalSCM,false,1);

  G4Tubs* hole_tmp2 = new G4Tubs("hole_tmp2",0,hole[1],18*mm,0,360);
  G4Tubs* tub14_tmp = new G4Tubs("tub14_tmp",outerCryo[0]*mm,outerCryo[1]*mm,outerCryo[2]/2.0*mm,0,360);
  G4SubtractionSolid* tub14 = new G4SubtractionSolid("tub14",tub14_tmp,hole_tmp2,xRot,G4ThreeVector(0,(outerCryo[1]-8)*mm,1640*mm) );
  logicalouterCryo = new G4LogicalVolume(tub14, Steel,"logicalouterCryo");
  physicalouterCryo = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalouterCryo,"physicalouterCryo",logicalSCM,false,0);

  G4Tubs* tub15 = new G4Tubs("tub15",endCryo[0]*mm,endCryo[1]*mm,endCryo[2]/2.0*mm,0,360);
  logicalendCryo = new G4LogicalVolume(tub15, Steel,"logicalendCryo");
  physicalendCryo1 = new G4PVPlacement(0,G4ThreeVector(0,0,endCryo[3]*mm),logicalendCryo,"physicalendCryo1",logicalSCM,false,0);
  physicalendCryo2 = new G4PVPlacement(0,G4ThreeVector(0,0,endCryo[4]*mm),logicalendCryo,"physicalendCryo2",logicalSCM,false,1);
/*
  G4Trap* trap = new G4Trap("trap",rein[2]/2.0*mm,0,0,rein[1]/2*mm,rein[1]*std::tan(pi/8)*mm,2,0,rein[1]/2*mm,rein[1]*std::tan(pi/8)*mm,2,0);
  G4Tubs* tub16 = new G4Tubs("tub16",0,outerCryo[1]*mm,(rein[2]/2.0+3)*mm,0,360);

  G4SubtractionSolid* solidrein = new G4SubtractionSolid("solidrein",trap,tub16,0,G4ThreeVector(0,847.5*mm,0) );
  logicalrein = new G4LogicalVolume(solidrein, Steel,"logicalrein");
  
  for(int i=0; i<8; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalrein1_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(45*i*deg);
    physicalrein1 = new G4PVPlacement(zRot,G4ThreeVector(-rein[1]/2*std::sin(pi/4*i)*mm,-rein[1]/2*std::cos(pi/4*i)*mm,rein[3]*mm),logicalrein,osnamerein.str(),logicalSCM,false,i);
  }
  for(G4int i=0; i<4; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalrein2_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(45*i*deg);
    physicalrein2 = new G4PVPlacement(zRot,G4ThreeVector(-rein[1]/2*std::sin(pi/4*i)*mm,-rein[1]/2*std::cos(pi/4*i)*mm,rein[4]*mm),logicalrein,osnamerein.str(),logicalSCM,false,8+i);
  }

  for(G4int i=5; i<8; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalrein2_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(45*i*deg);
    physicalrein2 = new G4PVPlacement(zRot,G4ThreeVector(-rein[1]/2*std::sin(pi/4*i)*mm,-rein[1]/2*std::cos(pi/4*i)*mm,rein[4]*mm),logicalrein,osnamerein.str(),logicalSCM,false,8+i);
   }
*/
  //pipe of inner 77k
  G4Box* solidpipe1 = new G4Box("solidpipe1",pipe1[2]/2*mm,(pipe1[1]+pipe1[4])/2*mm,pipe1[5]/2*mm);
  logicalpipe1 = new G4LogicalVolume(solidpipe1, G4Material::GetMaterial("Air"),"logicalpipe1");
  G4Tubs* tub17 = new G4Tubs("tub17",pipe1[0]*mm,pipe1[1]*mm,pipe1[5]/2*mm,0,360);
  G4Tubs* tub18 = new G4Tubs("tub18",0,pipe1[1]*mm,pipe1[5]/2*mm,0,360);
  G4Tubs* tub19 = new G4Tubs("tub19",0,pipe1[0]*mm,pipe1[5]/2*mm,0,360);
  G4Box* solidbox1 = new G4Box("solidbox1",pipe1[2]/2*mm,pipe1[3]/2*mm,pipe1[5]/2*mm);
  G4SubtractionSolid* solidsub1 = new G4SubtractionSolid("solidsub1",solidbox1,tub18,0,G4ThreeVector(0,(pipe1[4]-pipe1[3]/2)*mm,0) );
  G4LogicalVolume* logicalsub1 = new G4LogicalVolume(solidsub1, G4Material::GetMaterial("Aluminium"),"logicalsub1");
  G4LogicalVolume* logicaltub17 = new G4LogicalVolume(tub17, G4Material::GetMaterial("Aluminium"),"logicaltub17");
  G4LogicalVolume* logicaltub19 = new G4LogicalVolume(tub19,liquidhelium,"logicaltub19");
  //G4VPhysicalVolume* physicaltub17 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub17,"physicaltub17",logicalpipe1,false,0);
  //G4VPhysicalVolume* physicaltub19 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub19,"physicaltub19",logicalpipe1,false,0);
  //G4VPhysicalVolume* physicalsub1 = 
  new G4PVPlacement(0,G4ThreeVector(0,-(pipe1[4]-pipe1[3]/2)*mm,0),logicalsub1,"physicalsub1",logicalpipe1,false,0);
  for(G4int i=0; i<24; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalpipe1_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(15*i*deg);
    //pipe of inner 77k
    physicalpipe1=new G4PVPlacement(zRot,G4ThreeVector(-(std::sqrt(innerShield[0]*innerShield[0]-pipe1[2]/2*pipe1[2]/2)-
				    (pipe1[1]+pipe1[4])/2)*std::sin(15*pi/180*i)*mm,-(std::sqrt(innerShield[0]*innerShield[0]-
				    pipe1[2]/2*pipe1[2]/2)-(pipe1[1]+pipe1[4])/2)*std::cos(15*pi/180*i)*mm,0),
		                    logicalpipe1,osnamerein.str(),logicalSCM,false,0);
  }
  //pipe of outer 77k
  G4Box* solidpipe2 = new G4Box("solidpipe2",pipe2[2]/2*mm,(pipe2[1]+pipe2[4])/2*mm,pipe2[5]/2*mm);
  logicalpipe2 = new G4LogicalVolume(solidpipe2,G4Material::GetMaterial("Air"),"logicalpipe2");
  G4Tubs* tub20 = new G4Tubs("tub20",pipe2[0]*mm,pipe2[1]*mm,pipe2[5]/2*mm,0,360);
  G4Tubs* tub21 = new G4Tubs("tub21",0,pipe2[1]*mm,pipe2[5]/2*mm,0,360);
  G4Tubs* tub22 = new G4Tubs("tub22",0,pipe2[0]*mm,pipe2[5]/2*mm,0,360);
  G4Box* solidbox2 = new G4Box("solidbox2",pipe2[2]/2*mm,pipe2[3]/2*mm,pipe2[5]/2*mm);
  G4SubtractionSolid* solidsub2 = new G4SubtractionSolid("solidsub2",solidbox2,tub21,0,G4ThreeVector(0,(pipe2[4]-pipe2[3]/2)*mm,0) );
  G4LogicalVolume* logicalsub2 = new G4LogicalVolume(solidsub2,G4Material::GetMaterial("Aluminium"),"logicalsub2");
  G4LogicalVolume* logicaltub20 = new G4LogicalVolume(tub20,G4Material::GetMaterial("Aluminium"),"logicaltub20");
  G4LogicalVolume* logicaltub22 = new G4LogicalVolume(tub22,liquidhelium,"logicaltub22");
  //G4VPhysicalVolume* physicaltub20 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub20,"physicaltub20",logicalpipe2,false,0);
  //G4VPhysicalVolume* physicaltub22 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub22,"physicaltub22",logicalpipe2,false,0);
  //G4VPhysicalVolume* physicalsub2 = 
  new G4PVPlacement(0,G4ThreeVector(0,-(pipe2[4]-pipe2[3]/2)*mm,0),logicalsub2,"physicalsub2",logicalpipe2,false,0);
  for(G4int i=0; i<24; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalpipe2_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(15*i*deg);
    physicalpipe2= new G4PVPlacement(zRot,G4ThreeVector(-(l3Adia[1]+(pipe2[1]+pipe2[4])/2)*std::sin(15*pi/180*i)*mm,
			              -(l3Adia[1]+(pipe2[1]+pipe2[4])/2)*std::cos(15*pi/180*i)*mm,0),logicalpipe2,osnamerein.str(),
		                     logicalSCM,false,0);
  }

  //pipe of 4k
  G4Box* solidpipe3 = new G4Box("solidpipe3",pipe3[2]/2*mm,(pipe3[1]+pipe3[4])/2*mm,pipe3[5]/2*mm);
  logicalpipe3 = new G4LogicalVolume(solidpipe3,G4Material::GetMaterial("Air"),"logicalpipe3");
  G4Tubs* tub23 = new G4Tubs("tub23",pipe3[0]*mm,pipe3[1]*mm,pipe3[5]/2*mm,0,360);
  G4Tubs* tub24 = new G4Tubs("tub24",0,pipe3[1]*mm,pipe3[5]/2*mm,0,360);
  G4Tubs* tub25 = new G4Tubs("tub25",0,pipe3[0]*mm,pipe3[5]/2*mm,0,360);
  G4Box* solidbox3 = new G4Box("solidbox3",pipe3[2]/2*mm,pipe3[3]/2*mm,pipe3[5]/2*mm);
  G4SubtractionSolid* solidsub3 = new G4SubtractionSolid("solidsub3",solidbox3,tub24,0,G4ThreeVector(0,(pipe3[4]-pipe3[3]/2)*mm,0));
  G4LogicalVolume* logicalsub3 = new G4LogicalVolume(solidsub3,G4Material::GetMaterial("Aluminium"),"logicalsub3");
  G4LogicalVolume* logicaltub23 = new G4LogicalVolume(tub23,G4Material::GetMaterial("Aluminium"),"logicaltub23");
  G4LogicalVolume* logicaltub25 = new G4LogicalVolume(tub25,liquidhelium,"logicaltub25");
  //G4VPhysicalVolume* physicaltub23 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub23,"physicaltub23",logicalpipe3,false,0);
  //G4VPhysicalVolume* physicaltub25 = 
  new G4PVPlacement(0,G4ThreeVector(0,0,0),logicaltub25,"physicaltub25",logicalpipe3,false,0);
  //G4VPhysicalVolume* physicalsub3 = 
  new G4PVPlacement(0,G4ThreeVector(0,-(pipe3[4]-pipe3[3]/2)*mm,0),logicalsub3,"physicalsub3",logicalpipe3,false,0);
  for(G4int i=0; i<24; i++)
  {
    std::ostringstream osnamerein;
    osnamerein << "physicalpipe3_"<<i;
    G4RotationMatrix* zRot = new G4RotationMatrix;
    zRot->rotateZ(15*i*deg);
    physicalpipe3= new G4PVPlacement(zRot,G4ThreeVector(-(l4Adia[1]+(pipe3[1]+pipe3[4])/2)*std::sin(15*pi/180*i)*mm,
                                     -(l4Adia[1]+(pipe3[1]+pipe3[4])/2)*std::cos(15*pi/180*i)*mm,0),logicalpipe3,osnamerein.str(),
                                     logicalSCM,false,0);
  }

  //G4VisAttributes* visSCM = new G4VisAttributes(G4Colour(1.,1.,1.));
  G4VisAttributes* visSCM1 = new G4VisAttributes(G4Colour(0.,0.,1.));
  logicalSCM->SetVisAttributes(visSCM1);
  logicalinnCryo->SetVisAttributes(visSCM1);
//  logicalinnCryo->SetVisAttributes(G4VisAttributes::Invisible);
  logicalouterCryo->SetVisAttributes(visSCM1);
//  logicalouterCryo->SetVisAttributes(G4VisAttributes::Invisible);
  logicalendCryo->SetVisAttributes(visSCM1);
//  logicalendCryo->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visSCM2 = new G4VisAttributes(G4Colour(0.,1.,0.));
  logicall1Adia->SetVisAttributes(visSCM2);
//  logicall1Adia->SetVisAttributes(G4VisAttributes::Invisible);
  logicall2Adia->SetVisAttributes(visSCM2);
//  logicall2Adia->SetVisAttributes(G4VisAttributes::Invisible);
  logicall3Adia->SetVisAttributes(visSCM2);
//  logicall3Adia->SetVisAttributes(G4VisAttributes::Invisible);
  logicall4Adia->SetVisAttributes(visSCM2);
//  logicall4Adia->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visSCM3 = new G4VisAttributes(G4Colour(1.,0.,0.));
  logicalinnShield->SetVisAttributes(visSCM3);
//  logicalinnShield->SetVisAttributes(G4VisAttributes::Invisible);
  logicaloutShield->SetVisAttributes(visSCM3);
//  logicaloutShield->SetVisAttributes(G4VisAttributes::Invisible);
  logicalendShield->SetVisAttributes(visSCM3);
//  logicalendShield->SetVisAttributes(G4VisAttributes::Invisible);
  
  G4VisAttributes* visSCM4 = new G4VisAttributes(G4Colour(0.,1.,1.));
  logicall1insu->SetVisAttributes(visSCM4);
//  logicall1insu->SetVisAttributes(G4VisAttributes::Invisible);
  logicall2insu->SetVisAttributes(visSCM4);
//  logicall2insu->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visSCM5 = new G4VisAttributes(G4Colour(1.,0.,1.));
  logicalcoil->SetVisAttributes(visSCM5);
//  logicalcoil->SetVisAttributes(G4VisAttributes::Invisible);
  logicalendCoil->SetVisAttributes(visSCM5);
//  logicalendCoil->SetVisAttributes(G4VisAttributes::Invisible);
  G4VisAttributes* visSCM6 = new G4VisAttributes(G4Colour(1.,1.,0.));
  logicalsupp->SetVisAttributes(visSCM6);
//  logicalsupp->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM7 = new G4VisAttributes(G4Colour(0.,0.,0.));
//  logicalrein->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalpipe1->SetVisAttributes(G4VisAttributes::Invisible);
  logicalpipe2->SetVisAttributes(visSCM2);
//  logicalpipe2->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalpipe3->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub17->SetVisAttributes(visSCM2);
//  logicaltub17->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub19->SetVisAttributes(visSCM5);
//  logicaltub19->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub20->SetVisAttributes(visSCM1);
//  logicaltub20->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub22->SetVisAttributes(visSCM1);
//  logicaltub22->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub23->SetVisAttributes(visSCM2);
//  logicaltub23->SetVisAttributes(G4VisAttributes::Invisible);
  logicaltub25->SetVisAttributes(visSCM5);
//  logicaltub25->SetVisAttributes(G4VisAttributes::Invisible);
  logicalsub1->SetVisAttributes(visSCM4);
//  logicalsub1->SetVisAttributes(G4VisAttributes::Invisible);
  logicalsub2->SetVisAttributes(visSCM4);
//  logicalsub2->SetVisAttributes(G4VisAttributes::Invisible);
  
  logicalsub3->SetVisAttributes(visSCM4);
//  logicalsub3->SetVisAttributes(G4VisAttributes::Invisible);
  logicalhole1->SetVisAttributes(visSCM3);
//  logicalhole1->SetVisAttributes(G4VisAttributes::Invisible);
  logicalhole2->SetVisAttributes(visSCM3);
//  logicalhole2->SetVisAttributes(G4VisAttributes::Invisible);
  logicalhole3->SetVisAttributes(visSCM3);
//  logicalhole3->SetVisAttributes(G4VisAttributes::Invisible);
  logicalhole4->SetVisAttributes(visSCM3);
//  logicalhole4->SetVisAttributes(G4VisAttributes::Invisible);
  logicalhole5->SetVisAttributes(visSCM3);
//  logicalhole5->SetVisAttributes(G4VisAttributes::Invisible);
}
