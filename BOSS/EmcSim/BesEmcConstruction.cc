

//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "G4ThreeVector.hh"
#include "G4ReflectionFactory.hh"

#include "BesEmcConstruction.hh"
#include "BesEmcDetectorMessenger.hh"
#include "BesEmcGeometry.hh"
#include "BesCrystalParameterisation.hh"
#include "BesEmcEndGeometry.hh"
#include "ReadBoostRoot.hh"
#include "BesEmcParameter.hh"

#include "BesEmcSD.hh"
#include "G4IrregBox.hh"
#include "G4Box.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trap.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"
#include "globals.hh"
#include "G4UniformMagField.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4SDManager.hh"
#include "G4RunManager.hh"
#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4AffineTransform.hh"

#include "G4ios.hh"
#include "G4Geo/EmcG4Geo.h"

BesEmcConstruction* BesEmcConstruction::fBesEmcConstruction=0;

BesEmcConstruction* BesEmcConstruction::GetBesEmcConstruction()
{ return fBesEmcConstruction;}

BesEmcConstruction::BesEmcConstruction()
  :verboseLevel(0),
   solidEMC(0),logicEMC(0),physiEMC(0),logicBSCWorld(0),
   solidBSCPhi(0),logicBSCPhi(0),physiBSCPhi(0),
   solidBSCTheta(0),logicBSCTheta(0),physiBSCTheta(0),
   solidBSCCrystal(0),logicBSCCrystal(0),physiBSCCrystal(0),
   magField(0),detectorMessenger(0),besEMCSD(0),crystalParam(0),
   logicEnd(0),logicEndPhi(0),logicEndCasing(0),logicEndCrystal(0),
   logicRear(0),logicRearCasing(0),logicOrgGlass(0),logicPD(0),
   logicAlPlate(0),logicPreAmpBox(0),logicAirInPABox(0),
   logicHangingPlate(0),logicOCGirder(0),logicCable(0),logicWaterPipe(0),
   logicSupportBar(0),logicSupportBar1(0),logicEndRing(0),logicGear(0),
   logicTaperRing1(0),logicTaperRing2(0),logicTaperRing3(0)
{
  if(fBesEmcConstruction)
    { G4Exception("BesEmcConstruction constructed twice."); }
  fBesEmcConstruction=this;
  //for debug
  //  G4Exception("BesEmcConstruction::BesEmcConstruction() starting........");
  startID           = 1;
  phiNbCrystals     = 0;
  thetaNbCrystals   = 0;
  besEMCGeometry = new BesEmcGeometry();
  emcEnd = new BesEmcEndGeometry();
}

BesEmcConstruction::~BesEmcConstruction()
{
  if(detectorMessenger) delete detectorMessenger;
  if(crystalParam) delete crystalParam;
  if(besEMCGeometry) delete besEMCGeometry;
  if(emcEnd) delete emcEnd;
  
  BesEmcParameter::Kill();
}

void BesEmcConstruction::Construct(G4LogicalVolume* logicBes)
{
  besEMCGeometry->ComputeEMCParameters();
  detectorMessenger = new BesEmcDetectorMessenger(this,besEMCGeometry);
  emcEnd->ComputeParameters();

  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  if (!besEMCSD) {
    besEMCSD = new BesEmcSD("CalorSD",this,besEMCGeometry);
    SDman->AddNewDetector( besEMCSD );
  }

  // Construction
  G4cout<<"--------ReadBoostRoot::GetEmc()="<<ReadBoostRoot::GetEmc()<<G4endl;
  if(ReadBoostRoot::GetEmc()==2)
  {
    logicEMC = EmcG4Geo::Instance()->GetTopVolume();

    if(logicEMC){
      physiEMC = new G4PVPlacement(0,
          G4ThreeVector(0.0 ,0.0 ,0.0),
          logicEMC, "physicalEMC",logicBes, false, 0);
      G4cout<<"logicEmc:  ===  "<<logicEMC<<"  physiEmc "<<physiEMC<<G4endl;

      GetLogicalVolume();
      SetVisAndSD();
    }
  }
  else {
    //for debug
    //  G4Exception("BesEmcConstruction::Construct() starting............");
    //
    DefineMaterials();
    phiNbCrystals     = (*besEMCGeometry).BSCNbPhi;
    thetaNbCrystals   = (*besEMCGeometry).BSCNbTheta*2;

    G4double da=0.001*deg;  //delta angle to avoid overlap
    
    //
    //BSC
    //
    solidBSC = new G4Tubs("solidBSC",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).BSCRmax+(*besEMCGeometry).SPBarThickness+(*besEMCGeometry).SPBarThickness1+2.1*mm,   //radius from 942mm to 940 mm
      (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz,
			0.*deg,
			360.*deg);

    solidESC = new G4Cons("solidESC",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,
        (*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
        (*emcEnd).WorldDz/2,0.*deg,360.*deg);

    solidEMC = new G4UnionSolid("solidEMC0",
        solidBSC,
        solidESC,
        0,
        G4ThreeVector(0,0,(*emcEnd).WorldZPosition));
    
    G4RotationMatrix *rotateESC = new G4RotationMatrix();
    rotateESC->rotateY(180.*deg);
    
    solidEMC = new G4UnionSolid("solidEMC",
        solidEMC,
        solidESC,
        rotateESC,
        G4ThreeVector(0,0,-(*emcEnd).WorldZPosition));
    
    logicEMC = new G4LogicalVolume(solidEMC,
			 G4Material::GetMaterial("Air"),
			 "logicalEMC");
    
    physiEMC = new G4PVPlacement(0,
        0,
        logicEMC,
        "physicalEMC",
        logicBes,
        false,
        0);

    solidBSCWorld = new G4SubtractionSolid("solidBSCWorld0",
          solidBSC,
          solidESC,
          0,
          G4ThreeVector(0,0,(*emcEnd).WorldZPosition));

    solidBSCWorld = new G4SubtractionSolid("solidBSCWorld",
        solidBSCWorld,
        solidESC,
        rotateESC,
        G4ThreeVector(0,0,-(*emcEnd).WorldZPosition));

    logicBSCWorld = new G4LogicalVolume(solidBSCWorld,
        G4Material::GetMaterial("Air"),
        "logicalBSCWorld");

    G4RotationMatrix *rotBSC = new G4RotationMatrix();
    rotBSC->rotateY(180.*deg);
    physiBSCWorld = new G4PVPlacement(rotBSC,
        0,
        logicBSCWorld,
        "physicalBSCWorld",
        logicEMC,
        false,
        0);
    
    G4RotationMatrix *rotateMatrix[200];
    G4double oOp,ox,oy,oz; 
    G4double delta = 0*deg;
    G4ThreeVector axis = G4ThreeVector(0,0,0);
    oOp=(*besEMCGeometry).BSCRmin/sin(0.5*(*besEMCGeometry).BSCPhiDphi+90*deg)
      *sin((*besEMCGeometry).BSCAngleRotat);
    G4double ll=(*besEMCGeometry).BSCCryLength;
    G4double rr=(*besEMCGeometry).BSCRmin;
    G4double oj=sqrt(ll*ll+rr*rr-2*ll*rr*cos(180.*deg-(*besEMCGeometry).BSCAngleRotat));
    G4double oij=90.*deg-(*besEMCGeometry).BSCPhiDphi/2.-(*besEMCGeometry).BSCAngleRotat;
    G4double doj=asin(sin(180.*deg-(*besEMCGeometry).BSCAngleRotat)/oj*ll);
    G4double ioj=(*besEMCGeometry).BSCPhiDphi/2.+doj;
    G4double ij=oj/sin(oij)*sin(ioj);
    G4double dOp=rr/sin(90.*deg-(*besEMCGeometry).BSCPhiDphi/2.)
      *sin(90.*deg+(*besEMCGeometry).BSCPhiDphi/2.-(*besEMCGeometry).BSCAngleRotat);
    G4double cOp=rr/sin(90.*deg+(*besEMCGeometry).BSCPhiDphi/2.)
      *sin(90.*deg-(*besEMCGeometry).BSCPhiDphi/2.-(*besEMCGeometry).BSCAngleRotat);
    G4double ch=(dOp+ll)/cos((*besEMCGeometry).BSCPhiDphi)-cOp;
    G4double hi=(dOp+ll)*tan((*besEMCGeometry).BSCPhiDphi)-ij;
    G4double oh=sqrt(ch*ch+rr*rr-2*ch*rr*cos(180*deg-(*besEMCGeometry).BSCAngleRotat));
    G4double hoi=asin(sin(180*deg-oij)/oh*hi);
    G4double dok=asin(sin(180*deg-(*besEMCGeometry).BSCAngleRotat)/oh*ch);
    if(verboseLevel>3)
      G4cout << "oj=" <<oj/cm<<G4endl
        << "oij="<<oij/deg<<G4endl
        << "doj="<<doj/deg<<G4endl
        << "ioj="<<ioj/deg<<G4endl
        << "ij="<<ij/cm<<G4endl
        << "dOp="<<dOp/cm<<G4endl
        << "cOp="<<cOp/cm<<G4endl
        << "ch="<<ch/cm<<G4endl
        << "hi="<<hi/cm<<G4endl
        << "oh="<<oh/cm<<G4endl
        << "hoi="<<hoi/deg<<G4endl
        << "dok="<<dok/deg<<G4endl;

    // Phi Cell
    solidBSCPhiTub = new G4Tubs(
        "solidBSCPhiTub",
        dOp,
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).BSCDz,
        360.*deg-(*besEMCGeometry).BSCPhiDphi,
        (*besEMCGeometry).BSCPhiDphi);
    solidConsPhi = new G4Cons("consPhi1",
        (*besEMCGeometry).BSCRmin1,
        (*besEMCGeometry).BSCRmax1,
        (*besEMCGeometry).BSCRmin2,
        (*besEMCGeometry).BSCRmax2,
        (*besEMCGeometry).BSCDz1/2,
        0.*deg,
        360.*deg);
    solidBSCPhi1 = new G4SubtractionSolid("solidBSCPhi1",
        solidBSCPhiTub,
        solidConsPhi,
        0,
        G4ThreeVector(0,0,(*besEMCGeometry).BSCDz-(*besEMCGeometry).BSCDz1/2));
    solidConsPhi = new G4Cons("consPhi2",
        (*besEMCGeometry).BSCRmin2,
        (*besEMCGeometry).BSCRmax2,
        (*besEMCGeometry).BSCRmin1,
        (*besEMCGeometry).BSCRmax1,
        (*besEMCGeometry).BSCDz1/2,
        0.*deg,
        360.*deg);
    solidBSCPhi = new G4SubtractionSolid("solidBSCPhi",
        solidBSCPhi1,
        solidConsPhi,
        0,
        G4ThreeVector(0,0,(*besEMCGeometry).BSCDz1/2-(*besEMCGeometry).BSCDz));

    logicBSCPhi = new G4LogicalVolume(solidBSCPhi,
        G4Material::GetMaterial("Air"),
        "logicalBSCPhi");

    G4int i;
    for(G4int j=0;j<(*besEMCGeometry).BSCNbPhi;j++)  //=============
    {
      if(j<(*besEMCGeometry).BSCNbPhi/2) {  //0~59
        i=(*besEMCGeometry).BSCNbPhi/2-j-1;
      } else {  //60~119
        i=(*besEMCGeometry).BSCNbPhi*3/2-j-1;
      }
      rotateMatrix[i] = new G4RotationMatrix();
      rotateMatrix[i]->rotateZ(-i*(*besEMCGeometry).BSCPhiDphi
          -(*besEMCGeometry).BSCAngleRotat
          -(*besEMCGeometry).BSCPhiDphi/2.
          -hoi);
      rotateMatrix[i]->getAngleAxis(delta, axis);
      //G4cout << "The axis of crystals in the world system is: "
      //   << delta/deg << "(deg)(delta) "
      //<< axis  << "(Z axis)"<< G4endl;
      ox=oOp*cos(-90.*deg+(*besEMCGeometry).BSCAngleRotat+hoi
          +i*(*besEMCGeometry).BSCPhiDphi);
      oy=oOp*sin(-90.*deg+(*besEMCGeometry).BSCAngleRotat+hoi
          +i*(*besEMCGeometry).BSCPhiDphi);
      oz=0*cm;

      ostringstream strPhi;
      strPhi << "physicalBSCPhi"  << j;

      physiBSCPhi = new G4PVPlacement(rotateMatrix[i],
          G4ThreeVector(ox,oy,oz),
          logicBSCPhi,
          strPhi.str(),
          logicBSCWorld,
          false,
          j);
      //G4cout << G4ThreeVector(ox/cm,oy/cm,oz/cm) <<"(cm)" << G4endl
      //   << (-(*besEMCGeometry).BSCAngleRotat+(i-1)*(*besEMCGeometry).BSCPhiDphi)/deg <<"(degree)" << G4endl;
    }

    //
    //Crystals
    //
    G4double zHalfLength[50];
    G4double thetaAxis[50];
    G4double phiAxis[50];
    G4double yHalfLength1[50];
    G4double xHalfLength2[50];
    G4double xHalfLength1[50];
    G4double tanAlpha1[50];
    G4double yHalfLength2[50];
    G4double xHalfLength4[50];
    G4double xHalfLength3[50];
    G4double tanAlpha2[50];
    G4double xPosition[50];
    G4double yPosition[50];
    G4double zPosition[50];
    G4double thetaPosition[50];
    for(i=0;i<(*besEMCGeometry).BSCNbTheta;i++)
    {
      zHalfLength[i]  = (*besEMCGeometry).zHalfLength[i];
      thetaAxis[i]    = (*besEMCGeometry).thetaAxis[i];
      phiAxis[i]      = (*besEMCGeometry).phiAxis[i];
      yHalfLength1[i] = (*besEMCGeometry).yHalfLength1[i];
      xHalfLength2[i] = (*besEMCGeometry).xHalfLength2[i];
      xHalfLength1[i] = (*besEMCGeometry).xHalfLength1[i];
      tanAlpha1[i]    = (*besEMCGeometry).tanAlpha1[i];
      yHalfLength2[i] = (*besEMCGeometry).yHalfLength2[i];
      xHalfLength4[i] = (*besEMCGeometry).xHalfLength4[i];
      xHalfLength3[i] = (*besEMCGeometry).xHalfLength3[i];
      tanAlpha2[i]    = (*besEMCGeometry).tanAlpha2[i];
      xPosition[i]    = (*besEMCGeometry).xPosition[i];
      yPosition[i]    = (*besEMCGeometry).yPosition[i];
      zPosition[i]    = (*besEMCGeometry).zPosition[i];
      thetaPosition[i]= (*besEMCGeometry).thetaPosition[i];
      if(verboseLevel>4)	
        G4cout << "The sizes of the "<<i+1<<" crystal are:" << G4endl
          <<"zHalfLength ="<<zHalfLength[i]/cm<< "(cm)," << G4endl
          << "thetaAxis  ="<<thetaAxis[i]/deg << "(deg),"<< G4endl
          << "phiAxis    ="<< phiAxis[i]/deg  << "(deg),"<< G4endl
          << "yHalfLength1="<<yHalfLength1[i]/cm<<"(cm),"<< G4endl
          << "xHalfLength1="<<xHalfLength1[i]/cm<<"(cm),"<< G4endl
          << "xHalfLength2="<<xHalfLength2[i]/cm<<"(cm),"<< G4endl
          << "tanAlpha1   ="<< tanAlpha1[i]              << G4endl
          << "yHalfLength2="<<yHalfLength2[i]/cm<<"(cm),"<< G4endl
          << "xHalfLength3="<<xHalfLength3[i]/cm<<"(cm),"<< G4endl
          << "xHalfLength4="<<xHalfLength4[i]/cm<<"(cm),"<< G4endl
          << "tanAlpha2   =" << tanAlpha2[i]    << "."   << G4endl;
    }
    besEMCGeometry->ModifyForCasing();

    solidBSCCrystal = new G4Trap("solidCrystal",
        100*cm, 100*deg, 100*deg,
        100*cm, 100*cm,  100*cm,  100*deg,
        100*cm, 100*cm,  100*cm,  100*deg);

    logicBSCCrystal = new G4LogicalVolume(solidBSCCrystal,
        fCrystalMaterial,
        "logicalCrystal");

    crystalParam = new BesCrystalParameterisation
      (startID,
       thetaNbCrystals,
       (*besEMCGeometry).BSCNbTheta*2,
       besEMCGeometry,
       verboseLevel);

    //---------------------------------------------------------------------------------
    //rear substance
    solidRear = new G4Box("solidRearBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxDz/2);

    logicRear = new G4LogicalVolume(solidRear,
        G4Material::GetMaterial("Air"),
        "logicalRearBox");

    //organic glass
    solidOrgGlass = new G4Box("solidOrganicGlass",
        (*besEMCGeometry).orgGlassLengthX/2,
        (*besEMCGeometry).orgGlassLengthY/2,
        (*besEMCGeometry).orgGlassLengthZ/2);

    logicOrgGlass = new G4LogicalVolume(solidOrgGlass,
        organicGlass,
        "logicalOrganicGlass");

    physiOrgGlass = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz-(*besEMCGeometry).orgGlassLengthZ)/2),
        logicOrgGlass,
        "physicalOrganicGlass",
        logicRear,
        false,
        0);

    //casing
    solidCasingBox = new G4Box("solidCasingBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearCasingThickness/2);

    solidAirHole = new G4Box("solidAirHole",
        (*besEMCGeometry).orgGlassLengthX/2,
        (*besEMCGeometry).orgGlassLengthY/2,
        (*besEMCGeometry).rearBoxDz/2);      //any value more than casing thickness

    solidRearCasing = new G4SubtractionSolid("solidRearCasing",
        solidCasingBox,
        solidAirHole,
        0,
        0);

    logicRearCasing = new G4LogicalVolume(solidRearCasing,
        rearCasingMaterial,
        "logicalRearCasing");

    physiRearCasing = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz-(*besEMCGeometry).rearCasingThickness)/2),
        logicRearCasing,
        "physicalRearCasing",
        logicRear,
        false,
        0);

    //Al Plate
    solidAlBox = new G4Box("solidAlBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).AlPlateDz/2);

    solidAlPlate = new G4SubtractionSolid("solidAlPlate",
        solidAlBox,
        solidAirHole,
        0,
        0);

    logicAlPlate = new G4LogicalVolume(solidAlPlate,
        G4Material::GetMaterial("Aluminium"),
        "logicalAlPlate");

    physiAlPlate = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
            -(*besEMCGeometry).rearCasingThickness
            -(*besEMCGeometry).AlPlateDz/2)),
        logicAlPlate,
        "physicalAlPlate",
        logicRear,
        false,
        0);

    //photodiode
    solidPD = new G4Box("solidPD",
        (*besEMCGeometry).PDLengthX,     //two PD
        (*besEMCGeometry).PDLengthY/2,
        (*besEMCGeometry).PDLengthZ/2);

    logicPD = new G4LogicalVolume(solidPD,
        G4Material::GetMaterial("M_Silicon"),
        "logicalPD");

    physiPD = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
            -(*besEMCGeometry).orgGlassLengthZ
            -(*besEMCGeometry).PDLengthZ/2)),
        logicPD,
        "physicalPD",
        logicRear,
        false,
        0);

    //preamplifier box
    solidPreAmpBox = new G4Box("solidPreAmpBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).PABoxDz/2);

    logicPreAmpBox = new G4LogicalVolume(solidPreAmpBox,
        G4Material::GetMaterial("Aluminium"),
        "logicalPreAmpBox");

    physiPreAmpBox = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
            -(*besEMCGeometry).rearCasingThickness
            -(*besEMCGeometry).AlPlateDz
            -(*besEMCGeometry).PABoxDz/2)),
        logicPreAmpBox,
        "physicalPreAmpBox",
        logicRear,
        false,
        0);

    //air in preamplifier box
    solidAirInPABox = new G4Box("solidAirInPABox",
        (*besEMCGeometry).rearBoxLength/2-(*besEMCGeometry).PABoxThickness,
        (*besEMCGeometry).rearBoxLength/2-(*besEMCGeometry).PABoxThickness,
        (*besEMCGeometry).PABoxDz/2-(*besEMCGeometry).PABoxThickness);

    logicAirInPABox = new G4LogicalVolume(solidAirInPABox,
        G4Material::GetMaterial("Air"),
        "logicalAirInPABox");

    physiAirInPABox = new G4PVPlacement(0,
        0,
        logicAirInPABox,
        "physicalAirInPABox",
        logicPreAmpBox,
        false,
        0);

    //stainless steel for hanging the crystal
    solidHangingPlate = new G4Box("solidHangingPlate",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).HangingPlateDz/2);

    logicHangingPlate = new G4LogicalVolume(solidHangingPlate,stainlessSteel,"logicalHangingPlate");

    physiHangingPlate = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
            -(*besEMCGeometry).rearCasingThickness
            -(*besEMCGeometry).AlPlateDz
            -(*besEMCGeometry).PABoxDz
            -(*besEMCGeometry).HangingPlateDz/2)),
        logicHangingPlate,
        "physicalHangingPlate",
        logicRear,
        false,
        0);

    //water pipe
    solidWaterPipe = new G4Tubs("solidWaterPipe",
        0,
        (*besEMCGeometry).waterPipeDr,
        (*besEMCGeometry).BSCDz,
        0.*deg,
        360.*deg);

    logicWaterPipe = new G4LogicalVolume(solidWaterPipe,waterPipe,"logicalWaterPipe");

    physiWaterPipe = new G4PVPlacement(0,
        G4ThreeVector((*besEMCGeometry).cablePosX[0]-2*(*besEMCGeometry).cableDr,
          (*besEMCGeometry).cablePosY[0]-(*besEMCGeometry).cableDr-(*besEMCGeometry).waterPipeDr,
          0),
        logicWaterPipe,
        "physicalWaterPipe",
        logicBSCPhi,
        false,
        0);
    //---------------------------------------------------------------------------------


    //
    //Theta Cell
    //
    G4String nameCrystalAndCasing="CrystalAndCasing";

    G4int id=0;     //ID of crystals after distinguishing left and right
    for(i=startID;i<=thetaNbCrystals;i++) //================
    {
      ostringstream strSolidCasing;
      strSolidCasing << "solidBSCCasing"  << i-1;
      ostringstream strVolumeCasing;
      strVolumeCasing << "logicalBSCCasing"  << i-1;
      ostringstream strPhysiCasing;
      strPhysiCasing << "physicalBSCCasing"  << i-1;

      if(i>(*besEMCGeometry).BSCNbTheta)
      {
        id=i-(*besEMCGeometry).BSCNbTheta-1;
        solidBSCTheta = new G4Trap(strSolidCasing.str(),
            zHalfLength[id],
            thetaAxis[id],
            -phiAxis[id],
            yHalfLength1[id],
            xHalfLength2[id],
            xHalfLength1[id],
            -tanAlpha1[id],
            yHalfLength2[id],
            xHalfLength4[id],
            xHalfLength3[id],
            -tanAlpha2[id]);

        //G4cout<<"in EmcConstr1: "<<strSolidCasing.str()<<" x1="<<xHalfLength1[id]<<" y1="<<yHalfLength1[id]<<" theta="<<thetaAxis[id]
        //<<" phi="<<-phiAxis[id]<<" a1="<<-tanAlpha1[id]<<G4endl;

        logicBSCTheta = new G4LogicalVolume(solidBSCTheta,
            fCasingMaterial,
            strVolumeCasing.str());

        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1] = new G4RotationMatrix();
        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]->rotateZ(-90*deg);
        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]
          ->rotateX(-thetaPosition[id]);


        physiBSCTheta = 
          new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
              G4ThreeVector(xPosition[id],
                yPosition[id],
                zPosition[id]),
              strPhysiCasing.str(),
              logicBSCTheta,
              physiBSCPhi,
              false,
              i-1);

        if(logicBSCTheta)
        {
          G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0,0.,0.));
          rightVisAtt->SetVisibility(true);
          logicBSCTheta->SetVisAttributes(rightVisAtt);
          logicBSCTheta->SetVisAttributes(G4VisAttributes::Invisible);
        }

        ostringstream strRear;
        strRear << "physicalRearBox_1_"  << i-1;

        physiRear = new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
            G4ThreeVector((*besEMCGeometry).rearBoxPosX[id],
              (*besEMCGeometry).rearBoxPosY[id],
              (*besEMCGeometry).rearBoxPosZ[id]),
            strRear.str(),
            logicRear,
            physiBSCPhi,
            false,
            i-1);

        ostringstream strGirder;
        strGirder << "solidOpenningCutGirder_1_"  << i-1;
        solidOCGirder = new G4Cons(strGirder.str(),
            (*besEMCGeometry).OCGirderRmin1[id],
            (*besEMCGeometry).BSCPhiRmax,
            (*besEMCGeometry).OCGirderRmin2[id],
            (*besEMCGeometry).BSCPhiRmax,
            (*besEMCGeometry).OCGirderDz[id]/2,
            360.*deg-(*besEMCGeometry).OCGirderAngle/2,
            (*besEMCGeometry).OCGirderAngle/2-da);

        ostringstream strVGirder;
        strVGirder << "logicalOpenningCutGirder_1_"  << i-1;
        logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,strVGirder.str());
        logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

        ostringstream strPGirder;
        strPGirder << "physicalOpenningCutGirder_1_"  << i-1;
        physiOCGirder = new G4PVPlacement(0,
            G4ThreeVector(0,0,(*besEMCGeometry).OCGirderPosZ[id]),
            logicOCGirder,
            strPGirder.str(),
            logicBSCPhi,
            false,
            0);

        if(id<(*besEMCGeometry).BSCNbTheta-1)
        {
          G4double zLength = (*besEMCGeometry).OCGirderPosZ[id+1]
            -(*besEMCGeometry).OCGirderPosZ[id]
            -(*besEMCGeometry).OCGirderDz[id+1]/2-(*besEMCGeometry).OCGirderDz[id]/2;
          G4double zPosition = (*besEMCGeometry).OCGirderPosZ[id+1]
            -(*besEMCGeometry).OCGirderDz[id+1]/2-zLength/2;

          ostringstream strGirder2;
          strGirder2 << "solidOpenningCutGirder_2_"  << i-1;
          solidOCGirder = new G4Cons(strGirder2.str(),
              (*besEMCGeometry).OCGirderRmin2[id],
              (*besEMCGeometry).BSCPhiRmax,
              (*besEMCGeometry).OCGirderRmin1[id+1],
              (*besEMCGeometry).BSCPhiRmax,
              zLength/2,
              360.*deg-(*besEMCGeometry).OCGirderAngle/2,
              (*besEMCGeometry).OCGirderAngle/2-da);

          ostringstream strVGirder2;
          strVGirder2 << "logicalOpenningCutGirder_2_"  << i-1;
          logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,strVGirder2.str());
          logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

          ostringstream strPGirder2;
          strPGirder2 << "physicalOpenningCutGirder_2_"  << i-1;
          physiOCGirder = new G4PVPlacement(0,
              G4ThreeVector(0,0,zPosition),
              logicOCGirder,
              strPGirder2.str(),
              logicBSCPhi,
              false,
              0);
        } 

        ostringstream strBSCCable;
        strBSCCable << "solidBSCCable_1_"  << i-1;
        solidCable = new G4Tubs(strBSCCable.str(),
            0,
            (*besEMCGeometry).cableDr,
            (*besEMCGeometry).cableLength[id]/2,
            0.*deg,
            360.*deg);

        ostringstream strVBSCCable;
        strVBSCCable << "logicalBSCCable_1_"  << i-1;
        logicCable = new G4LogicalVolume(solidCable,cable,strVBSCCable.str());

        ostringstream strPBSCCable;
        strPBSCCable << "physicalBSCCable_1_"  << i-1;
        physiCable = new G4PVPlacement(0,
            G4ThreeVector((*besEMCGeometry).cablePosX[id],
              (*besEMCGeometry).cablePosY[id],
              (*besEMCGeometry).cablePosZ[id]),
            logicCable,
            strPBSCCable.str(),
            logicBSCPhi,
            false,
            0);
        logicCable->SetVisAttributes(G4VisAttributes::Invisible);
      }
      else
      {
        id=(*besEMCGeometry).BSCNbTheta-i;
        solidBSCTheta = new G4Trap(strSolidCasing.str(),
            zHalfLength[id],
            thetaAxis[id],
            phiAxis[id],
            yHalfLength1[id],
            xHalfLength1[id],
            xHalfLength2[id],
            tanAlpha1[id],
            yHalfLength2[id],
            xHalfLength3[id],
            xHalfLength4[id],
            tanAlpha2[id]);

        // 	    G4cout<<"in EmcConstr2: "<<strSolidCasing.str()<<" x1="<<xHalfLength1[id]<<" y1="<<yHalfLength1[id]<<" theta="<<thetaAxis[id]
        // 		  <<" phi="<<phiAxis[id]<<" a1="<<tanAlpha1[id]<<G4endl;

        logicBSCTheta = new G4LogicalVolume(solidBSCTheta,
            fCasingMaterial,
            strVolumeCasing.str());

        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1] = new G4RotationMatrix();
        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]->rotateZ(-90*deg);
        rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]
          ->rotateX(-180*deg+thetaPosition[id]);
        physiBSCTheta = 
          new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
              G4ThreeVector(xPosition[id],
                yPosition[id],
                -zPosition[id]),
              strPhysiCasing.str(),
              logicBSCTheta,
              physiBSCPhi,
              false,
              i-1);
        if(logicBSCTheta)
        {
          G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0,0.,0.));
          rightVisAtt->SetVisibility(true);
          logicBSCTheta->SetVisAttributes(rightVisAtt);
          logicBSCTheta->SetVisAttributes(G4VisAttributes::Invisible);
        }

        ostringstream strRear;
        strRear << "physicalRearBox_2_"  << i-1;

        physiRear = new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
            G4ThreeVector((*besEMCGeometry).rearBoxPosX[id],
              (*besEMCGeometry).rearBoxPosY[id],
              -(*besEMCGeometry).rearBoxPosZ[id]),
            strRear.str(),
            logicRear,
            physiBSCPhi,
            false,
            i-1);

        ostringstream strGirder;
        strGirder << "solidOpenningCutGirder_3_"  << i-1;
        solidOCGirder = new G4Cons(strGirder.str(),
            (*besEMCGeometry).OCGirderRmin2[id],
            (*besEMCGeometry).BSCPhiRmax,
            (*besEMCGeometry).OCGirderRmin1[id],
            (*besEMCGeometry).BSCPhiRmax,
            (*besEMCGeometry).OCGirderDz[id]/2,
            360.*deg-(*besEMCGeometry).OCGirderAngle/2,
            (*besEMCGeometry).OCGirderAngle/2-da);

        ostringstream strVGirder;
        strVGirder << "logicalOpenningCutGirder_3_"  << i-1;
        logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,strVGirder.str());
        logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

        ostringstream strPGirder;
        strPGirder << "physicalOpenningCutGirder_3_"  << i-1;
        physiOCGirder = new G4PVPlacement(0,
            G4ThreeVector(0,0,-(*besEMCGeometry).OCGirderPosZ[id]),
            logicOCGirder,
            strPGirder.str(),
            logicBSCPhi,
            false,
            0);

        if(id<(*besEMCGeometry).BSCNbTheta-1)
        {
          G4double zLength = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderPosZ[id]
            -(*besEMCGeometry).OCGirderDz[id+1]/2-(*besEMCGeometry).OCGirderDz[id]/2;
          G4double zPosition = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderDz[id+1]/2-zLength/2;

          ostringstream strGirder2;
          strGirder2 << "solidOpenningCutGirder_4_"  << i-1;
          solidOCGirder = new G4Cons(strGirder2.str(),
              (*besEMCGeometry).OCGirderRmin1[id+1],
              (*besEMCGeometry).BSCPhiRmax,
              (*besEMCGeometry).OCGirderRmin2[id],
              (*besEMCGeometry).BSCPhiRmax,
              zLength/2,
              360.*deg-(*besEMCGeometry).OCGirderAngle/2,
              (*besEMCGeometry).OCGirderAngle/2-da);

          ostringstream strVGirder2;
          strVGirder2 << "logicalOpenningCutGirder_4_"  << i-1;
          logicOCGirder 
            = new G4LogicalVolume(solidOCGirder,stainlessSteel,strVGirder2.str());
          logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

          ostringstream strPGirder2;
          strPGirder2 << "physicalOpenningCutGirder_4_"  << i-1;
          physiOCGirder = new G4PVPlacement(0,
              G4ThreeVector(0,0,-zPosition),
              logicOCGirder,
              strPGirder2.str(),
              logicBSCPhi,
              false,
              0);
        }

        ostringstream strBSCCable;
        strBSCCable << "solidBSCCable_2_"  << i-1;
        solidCable = new G4Tubs(strBSCCable.str(),
            0,
            (*besEMCGeometry).cableDr,
            (*besEMCGeometry).cableLength[id]/2,
            0.*deg,
            360.*deg);

        ostringstream strVBSCCable;
        strVBSCCable << "logicalBSCCable_2_"  << i-1;
        logicCable = new G4LogicalVolume(solidCable,cable,strVBSCCable.str());

        ostringstream strPBSCCable;
        strPBSCCable << "physicalBSCCable_2_"  << i-1;
        physiCable = new G4PVPlacement(0,
            G4ThreeVector((*besEMCGeometry).cablePosX[id],
              (*besEMCGeometry).cablePosY[id],
              -(*besEMCGeometry).cablePosZ[id]),
            logicCable,
            strPBSCCable.str(),
            logicBSCPhi,
            false,
            0);
        logicCable->SetVisAttributes(G4VisAttributes::Invisible);

      }

      ostringstream strCrystal;
      strCrystal << "physicalCrystal"  << i-1;
      physiBSCCrystal = new G4PVParameterised(
          strCrystal.str(),
          logicBSCCrystal,
          physiBSCTheta,
          kZAxis,
          1,//for this method,it must be 1.
          crystalParam);
      (*besEMCGeometry).physiBSCCrystal[i]=physiBSCCrystal;
      //G4cout << (*besEMCGeometry).physiBSCCrystal[i] << G4endl;
      physiBSCCrystal->SetCopyNo(i);


      if(verboseLevel>4)
        G4cout << "BesEmcConstruction*****************************"<< G4endl
          << "point of crystal =" <<physiBSCCrystal << G4endl
          //	       << "point of mother  =" <<physiBSCCrystal->GetMotherPhysical() << G4endl
          << "point of excepted=" <<physiBSCTheta << G4endl;
      //G4Exception("BesEMCConstruction::Construct() starting............");
    }

    //
    //always return the physical World
    //
    if(verboseLevel>0)PrintEMCParameters();
    //  return physiBSC;

    ConstructSPFrame(logicBSCWorld,besEMCGeometry);
    ConstructEndGeometry(logicEMC);
  }

  //Set vis attributes and sensitive detector 
  SetVisAndSD();

  //list geo tree
  if(logicEMC&&physiEMC&&verboseLevel>4){
    G4cout<<"logicEmc "<<logicEMC<<"  physiEmc "<<physiEMC<<G4endl;
    G4cout<<"list geo tree"<<G4endl;

    int NdaughterofEMC = logicEMC->GetNoDaughters();

    for(int i = 0; i < NdaughterofEMC; i++)
    {
      G4LogicalVolume *daughterofEmc = logicEMC->GetDaughter(i)->GetLogicalVolume();
      G4cout<<i<<"/"<<NdaughterofEMC<<" name: "<<daughterofEmc->GetName()<<" "<<daughterofEmc<<" shape: "<<daughterofEmc->GetSolid()->GetName()<<G4endl;
      int NdaughterofEmc_2 = daughterofEmc->GetNoDaughters();
      for(int j = 0; j < NdaughterofEmc_2; j++)
      {
        G4LogicalVolume *daughterofEmc_2 = daughterofEmc->GetDaughter(j)->GetLogicalVolume();
        G4cout<<"     --> "<<j<<"/"<<NdaughterofEmc_2<<" name: "<<daughterofEmc_2->GetName()<<" "<<daughterofEmc_2<<" shape: "<<daughterofEmc_2->GetSolid()->GetName()<<G4endl;
        int NdaughterofEmc_3 = daughterofEmc_2->GetNoDaughters();
        for(int k = 0; k < NdaughterofEmc_3; k++)
        {
          G4LogicalVolume *daughterofEmc_3 = daughterofEmc_2->GetDaughter(k)->GetLogicalVolume();
          G4cout<<"          --> "<<k<<"/"<<NdaughterofEmc_3<<" name: "<<daughterofEmc_3->GetName()<<" "<<daughterofEmc_3<<" shape: "<<daughterofEmc_3->GetSolid()->GetName()<<G4endl;
          int NdaughterofEmc_4 = daughterofEmc_3->GetNoDaughters();
          for(int m = 0; m < NdaughterofEmc_4; m++)
          {
            G4LogicalVolume *daughterofEmc_4 = daughterofEmc_3->GetDaughter(m)->GetLogicalVolume();
            G4cout<<"               --> "<<m<<"/"<<NdaughterofEmc_4<<" name: "<<daughterofEmc_4->GetName()<<" "<<daughterofEmc_4<<" shape: "<<daughterofEmc_4->GetSolid()->GetName()<<G4endl; 
            if(daughterofEmc_3->GetSolid()->GetName().contains("solidBSCCasing"))
            {
              G4Trap *Crystal = (G4Trap *)daughterofEmc_3->GetSolid();
              double hz = Crystal->GetZHalfLength();
              double hx1 = Crystal->GetXHalfLength1();
              double hx2 = Crystal->GetXHalfLength2();
              double hx3 = Crystal->GetXHalfLength3();
              double hx4 = Crystal->GetXHalfLength4();
              double hy1 = Crystal->GetYHalfLength1();
              double hy2 = Crystal->GetYHalfLength2();
              double tanalpha1 = Crystal->GetTanAlpha1();
              double tanalpha2 = Crystal->GetTanAlpha2();
              G4cout<<"                   --> "<<hx1<<" "<<hx2<<" "<<hx3<<" "<<hx4<<" "<<hy1<<" "<<hy2<<" "<<hz<<" "<<tanalpha1<<" "<<tanalpha2<<G4endl;

            }//if(SolidCrystal)
          }//4
        }//3
      }//2
    }//1
  }


}

void BesEmcConstruction::ConstructEndGeometry(G4LogicalVolume* logicEMC)
{
  G4Material* fCrystalMaterial = G4Material::GetMaterial("Cesiumiodide");
  G4VisAttributes* crystalVisAtt= new G4VisAttributes(G4Colour(0.5,0,1.0));
  crystalVisAtt->SetVisibility(false);
  G4VisAttributes* endPhiVisAtt= new G4VisAttributes(G4Colour(0,1.0,0));
  endPhiVisAtt->SetVisibility(false);
  const G4double zoomConst = 0.995;
  const G4double da=0.001*deg;

  //world volume of endcap
  //east end
  solidEnd = new G4Cons("solidEndWorld",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,
      (*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
      (*emcEnd).WorldDz/2,0.*deg,360.*deg);
  logicEnd = new G4LogicalVolume(solidEnd, G4Material::GetMaterial("Aluminium"), "logicalEndWorld", 0, 0, 0);
  physiEnd = new G4PVPlacement(0,               // no rotation
      G4ThreeVector(0,0,(*emcEnd).WorldZPosition),
      logicEnd,               // its logical volume
      "physicalEndWorld0",             // its name
      logicEMC,               // its mother  volume
      false,                  // no boolean operations
      0);                     // no field specific to volume
  if(logicEnd)
    logicEnd->SetVisAttributes(G4VisAttributes::Invisible);


  //west end
  G4RotationMatrix *rotateEnd = new G4RotationMatrix();
  rotateEnd->rotateY(180.*deg);
  physiEnd = new G4PVPlacement(rotateEnd,
      G4ThreeVector(0,0,-(*emcEnd).WorldZPosition),
      logicEnd,
      "physicalEndWorld2",
      logicEMC,
      false,
      2);

  ////////////////////////////////////////////////////////////////////////
  // emc endcap sectors (east)                                            //
  //////////////////////////////////////////////////////////////////////////
  //                                    20mm gap                          // 
  //                                      ||                              //
  //                                \   7 || 6   /                        //
  //                           -   8 \    ||    / 5   -                   //
  //                             -    \   ||   /    -                     //
  //                        _  9   -   \  ||  /   -   4  _                //
  //                          - _    -  \ || /  -    _ -                  //
  //                              - _  - \||/ -  _ -                      //
  //                         10        - -||- -         3                 //
  //                      ----------------||----------------              //
  //                         11        - -||- -         2                 //
  //                              _ -  - /||\ -  - _                      //
  //                          _ -    -  / || \  -    - _                  //
  //                        -  12  -   /  ||  \   -   1  -                //
  //                             -    /   ||   \    -                     //
  //                           -  13 /    ||    \  0  -                   //
  //                                /  14 || 15  \                        //
  //                                      ||                              //
  ////////////////////////////////////////////////////////////////////////

  // 1/16 of endcap world,which has some symmetry
  // sector 0-6,8-14
  solidEndPhi = new G4Cons("solidEndPhi0",
      (*emcEnd).SectorRmin1,(*emcEnd).SectorRmax1,(*emcEnd).SectorRmin2,(*emcEnd).SectorRmax2,
      (*emcEnd).SectorDz/2,0.*deg,22.5*deg-da);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "logicalEndPhi0", 0, 0, 0);
  for(G4int i=0;i<14;i++)
  {
    if((i!=6)&&(i!=7))
    {
      G4RotationMatrix *rotatePhi = new G4RotationMatrix();
      rotatePhi->rotateZ(-i*22.5*deg+67.5*deg);
      ostringstream strEndPhi;
      strEndPhi << "physicalEndPhi"  << i;
      physiEndPhi = new G4PVPlacement(rotatePhi,//0,logicEndPhi,strEndPhi.str(),logicEnd,false,i);
          G4ThreeVector(0,0,(*emcEnd).SectorZPosition),logicEndPhi,strEndPhi.str(),logicEnd,false,i);
    }
  }
  if(logicEndPhi)
    logicEndPhi->SetVisAttributes(endPhiVisAtt);

  for(G4int i=0;i<35;i++)
  {
    ostringstream strEndCasing;
    strEndCasing << "solidEndCasing_0_"  << i;

    //-************tranform to new coodinate!    liangyt 2007.5.7  *******
    G4ThreeVector newfPnt[8];
    G4ThreeVector center(0.0, 0.0, 0.0);
    G4ThreeVector rotAngle(0.0, 0.0, 0.0);

    TransformToArb8( (*emcEnd).fPnt[i], newfPnt, center, rotAngle );

    emcEnd->Zoom(newfPnt,zoomConst);    //change emcEnd.fPnt[i] to newfPnt

    G4RotationMatrix *rotatePhiIrregBox = new G4RotationMatrix();
    rotatePhiIrregBox->rotateX(rotAngle.x());
    rotatePhiIrregBox->rotateY(rotAngle.y());
    rotatePhiIrregBox->rotateZ(rotAngle.z());
    //-*******************************************************************

    solidEndCasing = new G4IrregBox(strEndCasing.str(),(*emcEnd).zoomPoint);   //liangyt

    ostringstream strVEndCasing;
    strVEndCasing << "logicalEndCasing_0_"  << i;
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCasingMaterial,strVEndCasing.str());

    ostringstream strPEndCasing;
    strPEndCasing << "physicalEndCasing_0_"  << i;
    physiEndCasing = new G4PVPlacement(rotatePhiIrregBox,center,
        logicEndCasing,strPEndCasing.str(),logicEndPhi,false,i);  //change with rot and pos now!

    ostringstream strEndCrystal;
    strEndCrystal << "solidEndCrystal_0_"  << i;

    emcEnd->ModifyForCasing((*emcEnd).zoomPoint,i);
    solidEndCrystal = new G4IrregBox(strEndCrystal.str(),(*emcEnd).cryPoint);

    ostringstream strVEndCrystal;
    strVEndCrystal << "logicalEndCrystal_0_"  << i;
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,strVEndCrystal.str());

    ostringstream strPEndCrystal;
    strPEndCrystal << "physicalEndCrystal_0_"  << i;
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,strPEndCrystal.str(),logicEndCasing,false,i);

    logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    logicEndCrystal->SetVisAttributes(crystalVisAtt);
    logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }

  // the top area which has 20 mm gap
  // sector 6,14
  solidEndPhi = new G4Cons("solidEndPhi1",
      (*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,  
      (*emcEnd).WorldDz/2,67.5*deg,22.5*deg-da);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "logicalEndPhi1", 0, 0, 0);
  for(G4int i=0;i<2;i++)
  {
    G4RotationMatrix *rotatePhi = new G4RotationMatrix();
    rotatePhi->rotateZ(-i*180.*deg);
    ostringstream strEndPhi;
    strEndPhi << "physicalEndPhi"  << i*8+6;
    physiEndPhi = new G4PVPlacement(rotatePhi,G4ThreeVector(0,0,(*emcEnd).SectorZPosition),
        logicEndPhi,strEndPhi.str(),logicEnd,false,i*8+6);
  }
  if(logicEndPhi)
    logicEndPhi->SetVisAttributes(endPhiVisAtt);

  for(G4int i=0;i<35;i++)
  {
    ostringstream strEndCasing;
    strEndCasing << "solidEndCasing_1_"  << i;

    //-************tranform to new coodinate!    liangyt 2007.5.7  *******
    G4ThreeVector newfPnt[8];
    G4ThreeVector center(0.0, 0.0, 0.0);
    G4ThreeVector rotAngle(0.0, 0.0, 0.0);

    TransformToArb8( (*emcEnd).fPnt1[i], newfPnt, center, rotAngle );

    emcEnd->Zoom(newfPnt,zoomConst);    //change emcEnd.fPnt[i] to newfPnt

    G4RotationMatrix *rotatePhiIrregBox = new G4RotationMatrix();
    rotatePhiIrregBox->rotateX(rotAngle.x());
    rotatePhiIrregBox->rotateY(rotAngle.y());
    rotatePhiIrregBox->rotateZ(rotAngle.z());
    //-*******************************************************************

    solidEndCasing = new G4IrregBox(strEndCasing.str(),(*emcEnd).zoomPoint);

    ostringstream strVEndCasing;
    strVEndCasing << "logicalEndCasing_1_"  << i;
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCasingMaterial,strVEndCasing.str());

    ostringstream strPEndCasing;
    strPEndCasing << "physicalEndCasing_1_"  << i;
    physiEndCasing = new G4PVPlacement(rotatePhiIrregBox,center,
        logicEndCasing,strPEndCasing.str(),logicEndPhi,false,i);  //change with rot and pos now!

    ostringstream strEndCrystal;
    strEndCrystal << "solidEndCrystal_1_"  << i;

    emcEnd->ModifyForCasing((*emcEnd).zoomPoint,i);
    solidEndCrystal = new G4IrregBox(strEndCrystal.str(),(*emcEnd).cryPoint);

    ostringstream strVEndCrystal;
    strVEndCrystal << "logicalEndCrystal_1_"  << i;
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,strVEndCrystal.str());

    ostringstream strPEndCrystal;
    strPEndCrystal << "physicalEndCrystal_1_"  << i;
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,strPEndCrystal.str(),logicEndCasing,false,i);

    logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    logicEndCrystal->SetVisAttributes(crystalVisAtt);
    logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }

  (*emcEnd).ReflectX();

  // sector 7,15
  for(G4int i=0;i<35;i++)
    for (G4int j=0;j<8;j++)
      (*emcEnd).fPnt1[i][j].rotateZ(-90.*deg);

  solidEndPhi = new G4Cons("solidEndPhi2",
      (*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
      (*emcEnd).WorldDz/2,0*deg,22.5*deg-da);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "logicalEndPhi2", 0, 0, 0);
  for(G4int i=0;i<2;i++)
  {
    G4RotationMatrix *rotatePhi = new G4RotationMatrix();
    rotatePhi->rotateZ(-i*180.*deg-90.*deg);
    ostringstream strEndPhi;
    strEndPhi << "physicalEndPhi"  << i*8+7;
    physiEndPhi = new G4PVPlacement(rotatePhi,G4ThreeVector(0,0,(*emcEnd).SectorZPosition),
        logicEndPhi,strEndPhi.str(),logicEnd,false,i*8+7);
  }
  if(logicEndPhi)
    logicEndPhi->SetVisAttributes(endPhiVisAtt);

  for(G4int i=0;i<35;i++)
  {
    ostringstream strEndCasing;
    strEndCasing << "solidEndCasing_2_"  << i;

    //-************tranform to new coodinate!    liangyt 2007.5.7  *******
    G4ThreeVector newfPnt[8];
    G4ThreeVector center(0.0, 0.0, 0.0);
    G4ThreeVector rotAngle(0.0, 0.0, 0.0);

    TransformToArb8( (*emcEnd).fPnt1[i], newfPnt, center, rotAngle );

    emcEnd->Zoom(newfPnt,zoomConst);    //change emcEnd.fPnt[i] to newfPnt

    G4RotationMatrix *rotatePhiIrregBox = new G4RotationMatrix();
    rotatePhiIrregBox->rotateX(rotAngle.x());
    rotatePhiIrregBox->rotateY(rotAngle.y());
    rotatePhiIrregBox->rotateZ(rotAngle.z());
    //-*******************************************************************

    solidEndCasing = new G4IrregBox(strEndCasing.str(),(*emcEnd).zoomPoint);

    ostringstream strVEndCasing;
    strVEndCasing << "logicalEndCasing_2_"  << i;
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCasingMaterial,strVEndCasing.str());

    ostringstream strPEndCasing;
    strPEndCasing << "physicalEndCasing_2_"  << i;
    physiEndCasing = new G4PVPlacement(rotatePhiIrregBox,center,
        logicEndCasing,strPEndCasing.str(),logicEndPhi,false,i);  //change with rot and pos now!

    ostringstream strEndCrystal;
    strEndCrystal << "solidEndCrystal_2_"  << i;

    emcEnd->ModifyForCasing((*emcEnd).zoomPoint,i);
    solidEndCrystal = new G4IrregBox(strEndCrystal.str(),(*emcEnd).cryPoint);

    ostringstream strVEndCrystal;
    strVEndCrystal << "logicalEndCrystal_2_"  << i;
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,strVEndCrystal.str());

    ostringstream strPEndCrystal;
    strPEndCrystal << "physicalEndCrystal_2_"  << i;
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,strPEndCrystal.str(),logicEndCasing,false,i);

    logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    logicEndCrystal->SetVisAttributes(crystalVisAtt);
    logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }
}

G4int BesEmcConstruction::ComputeEndCopyNb(G4int num)
{
  G4int copyNb;
  switch(num){
    case 30:
      copyNb = 5;
      break;
    case 31:
      copyNb = 6;
      break;
    case 32:
      copyNb = 14;
      break;
    case 33:
      copyNb = 15;
      break;
    case 34:
      copyNb = 16;
      break;
    default:
      copyNb = num;
      break;
  }
  return copyNb;
}

void BesEmcConstruction::ConstructSPFrame(G4LogicalVolume* logicEMC, BesEmcGeometry *besEMCGeometry)
{
  G4double rmax=(*besEMCGeometry).BSCRmax+2.*mm;  //radius from 942mm to 940mm
  solidSupportBar = new G4Tubs("solidSupportBar0",
      rmax+(*besEMCGeometry).SPBarThickness1,
      rmax+(*besEMCGeometry).SPBarThickness+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCDz
      +(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz,
      0.*deg,
      360.*deg);

  logicSupportBar = new G4LogicalVolume(solidSupportBar,stainlessSteel,"logicalSupportBar0");

  physiSupportBar = new G4PVPlacement(0,0,logicSupportBar,"physicalSupportBar0",logicEMC,false,0);

  solidSupportBar1 = new G4Tubs("solidSupportBar1",
      rmax,
      rmax+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3,
      (*besEMCGeometry).BSCPhiDphi-(*besEMCGeometry).SPBarDphi/2,
      (*besEMCGeometry).SPBarDphi);

  logicSupportBar1 = new G4LogicalVolume(solidSupportBar1,stainlessSteel,"logicalSupportBar1");

  for(G4int i=0;i<(*besEMCGeometry).BSCNbPhi/2;i++)
  {
    G4RotationMatrix *rotateSPBar = new G4RotationMatrix();
    rotateSPBar->rotateZ((*besEMCGeometry).BSCPhiDphi-i*2*(*besEMCGeometry).BSCPhiDphi);
    ostringstream strSupportBar1;
    strSupportBar1 << "physicalSupportBar1_"  << i;
    physiSupportBar1 = new G4PVPlacement(rotateSPBar,0,
        logicSupportBar1,strSupportBar1.str(),logicEMC,false,0);
  }

  //end ring
  solidEndRing = new G4Tubs("solidEndRing",
      (*besEMCGeometry).EndRingRmin,
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr/2,
      (*besEMCGeometry).EndRingDz/2,
      0.*deg,
      360.*deg);

  solidGear = new G4Tubs("solidGear",
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr/2,
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr,
      (*besEMCGeometry).EndRingDz/2,
      0.*deg,
      (*besEMCGeometry).BSCPhiDphi);

  //taper ring
  solidTaperRing1 = new G4Tubs("solidTaperRing1",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).TaperRingRmin1+(*besEMCGeometry).TaperRingThickness1,
      (*besEMCGeometry).TaperRingInnerLength/2,
      0.*deg,
      360.*deg);

  solidTaperRing2 = new G4Cons("solidTaperRing2",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).TaperRingRmin1+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingRmin2,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingDz/2,
      0.*deg,
      360.*deg);

  solidTaperRing3 = new G4Cons("solidTaperRing3",
      (*besEMCGeometry).BSCRmax2,
      (*besEMCGeometry).BSCRmax2+(*besEMCGeometry).TaperRingOuterLength1,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr+(*besEMCGeometry).TaperRingOuterLength,
      (*besEMCGeometry).TaperRingThickness3/2,
      0.*deg,
      360.*deg);

  logicEndRing = new G4LogicalVolume(solidEndRing,stainlessSteel,"logicalEndRing");
  logicGear = new G4LogicalVolume(solidGear,stainlessSteel,"logicalGear");
  logicTaperRing1 = new G4LogicalVolume(solidTaperRing1,stainlessSteel,"logicalTaperRing1");
  logicTaperRing2 = new G4LogicalVolume(solidTaperRing2,stainlessSteel,"logicalTaperRing2");
  logicTaperRing3 = new G4LogicalVolume(solidTaperRing3,stainlessSteel,"logicalTaperRing3");

  for(G4int i=0;i<2;i++)
  {
    G4RotationMatrix *rotateSPRing = new G4RotationMatrix();
    G4double zEndRing,z1,z2,z3;
    if(i==0)
    {
      zEndRing = (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz/2;
      z1 = (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3
        -(*besEMCGeometry).TaperRingDz-(*besEMCGeometry).TaperRingInnerLength/2;
      z2 = (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3-(*besEMCGeometry).TaperRingDz/2;
      z3 = (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3/2;
    }
    else
    {
      rotateSPRing->rotateY(180.*deg);
      zEndRing = -((*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz/2);
      z1 = -((*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3
          -(*besEMCGeometry).TaperRingDz-(*besEMCGeometry).TaperRingInnerLength/2);
      z2 = -((*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3-(*besEMCGeometry).TaperRingDz/2);
      z3 = -((*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3/2);
    }

    ostringstream strEndRing;
    strEndRing << "physicalEndRing_"  << i;
    physiEndRing = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,zEndRing),
        logicEndRing,strEndRing.str(),logicEMC,false,0);

    for(G4int j=0;j<(*besEMCGeometry).BSCNbPhi/2;j++)
    {
      G4RotationMatrix *rotateGear = new G4RotationMatrix();
      rotateGear->rotateZ((*besEMCGeometry).BSCPhiDphi/2-j*2*(*besEMCGeometry).BSCPhiDphi);

      ostringstream strGear;
      strGear << "physicalGear_"  << i << "_" <<j;
      physiGear = new G4PVPlacement(rotateGear,G4ThreeVector(0,0,zEndRing),
          logicGear,strGear.str(),logicEMC,false,0);
    }

    ostringstream strTaperRing1;
    strTaperRing1 << "physicalTaperRing1_"  << i;
    physiTaperRing1 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z1),
        logicTaperRing1,strTaperRing1.str(),logicEMC,false,0);

    ostringstream strTaperRing2;
    strTaperRing2 << "physicalTaperRing2_"  << i;
    physiTaperRing2 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z2),
        logicTaperRing2,strTaperRing2.str(),logicEMC,false,0);

    ostringstream strTaperRing3;
    strTaperRing3 << "physicalTaperRing3_"  << i;
    physiTaperRing3 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z3),
        logicTaperRing3,strTaperRing3.str(),logicEMC,false,0);
  }
}

//Set vis attributes and sensitive detector
void BesEmcConstruction::SetVisAndSD()
{
  //-------------------------------------------------------------
  //Barrel
  G4VisAttributes* bscVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  bscVisAtt->SetVisibility(false);
  logicEMC->SetVisAttributes(bscVisAtt);
  if(logicBSCWorld)
    logicBSCWorld->SetVisAttributes(G4VisAttributes::Invisible);

  if (logicBSCCrystal) {
    //G4cout<<"find BSCCrystal "<<G4endl;
    G4VisAttributes* crystalVisAtt= new G4VisAttributes(G4Colour(0,0,1.0));
    crystalVisAtt->SetVisibility(true);
    logicBSCCrystal->SetVisAttributes(crystalVisAtt);
    logicBSCCrystal->SetSensitiveDetector(besEMCSD);
  }

  if(logicBSCPhi) {
    G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
    rightVisAtt->SetVisibility(false);
    logicBSCPhi->SetVisAttributes(rightVisAtt);
  }

  if(logicRear)
    logicRear->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicOrgGlass)
    logicOrgGlass->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicRearCasing)
    logicRearCasing->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicAlPlate) 
    logicAlPlate->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicPD) {
    logicPD->SetVisAttributes(G4VisAttributes::Invisible);
    logicPD->SetSensitiveDetector(besEMCSD);
  }
  if(logicPreAmpBox)
    logicPreAmpBox->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicAirInPABox)
    logicAirInPABox->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicHangingPlate)
    logicHangingPlate->SetVisAttributes(G4VisAttributes::Invisible);
  if(logicWaterPipe)
    logicWaterPipe->SetVisAttributes(G4VisAttributes::Invisible);

  //-------------------------------------------------------------
  //Support system
  G4VisAttributes* ringVisAtt= new G4VisAttributes(G4Colour(0.5,0.25,0.));
  ringVisAtt->SetVisibility(false);
  if(logicSupportBar)
    logicSupportBar->SetVisAttributes(ringVisAtt);
  if(logicSupportBar1)
    logicSupportBar1->SetVisAttributes(ringVisAtt);
  if(logicEndRing)
    logicEndRing->SetVisAttributes(ringVisAtt);
  if(logicGear)
    logicGear->SetVisAttributes(ringVisAtt);
  if(logicTaperRing1)
    logicTaperRing1->SetVisAttributes(ringVisAtt);
  if(logicTaperRing2)
    logicTaperRing2->SetVisAttributes(ringVisAtt);
  if(logicTaperRing3)
    logicTaperRing3->SetVisAttributes(ringVisAtt);

  //-------------------------------------------------------------
  //Endcap
  G4VisAttributes* endPhiVisAtt= new G4VisAttributes(G4Colour(0,1.0,0));
  endPhiVisAtt->SetVisibility(false);
  if(logicEnd)
    logicEnd->SetVisAttributes(endPhiVisAtt);
}

//Get logical volumes from gdml
void BesEmcConstruction::GetLogicalVolume()
{
  //-------------------------------------------------------------
  //Barrel
  logicBSCWorld = FindLogicalVolume("logicalBSCWorld"); 
  logicBSCCrystal = FindLogicalVolume("logicalCrystal");
  logicBSCPhi = FindLogicalVolume("logicalBSCPhi");
  logicRear = FindLogicalVolume("logicalRearBox");
  logicOrgGlass = FindLogicalVolume("logicalOrganicGlass");
  logicRearCasing = FindLogicalVolume("logicalRearCasing");
  logicAlPlate = FindLogicalVolume("logicalAlPlate");
  logicPD = FindLogicalVolume("logicalPD");
  logicPreAmpBox = FindLogicalVolume("logicalPreAmpBox");
  logicAirInPABox = FindLogicalVolume("logicalAirInPABox");
  logicHangingPlate = FindLogicalVolume("logicalHangingPlate");
  logicWaterPipe = FindLogicalVolume("logicalWaterPipe");

  for(int i = 0; i < 44; i++){
    std::ostringstream osnameBSCCasing;
    osnameBSCCasing << "logicalBSCCasing"<<i;
    logicBSCTheta = FindLogicalVolume( osnameBSCCasing.str() );
    if(logicBSCTheta)
    {
      G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0, 0.0,0.0));
      rightVisAtt->SetVisibility(false);
      logicBSCTheta->SetVisAttributes(rightVisAtt);
    }

    std::ostringstream osnameBSCCable1;
    osnameBSCCable1 << "logicalBSCCable_1_"<<i;
    logicCable = FindLogicalVolume( osnameBSCCable1.str() );
    if(logicCable)
      logicCable->SetVisAttributes(G4VisAttributes::Invisible);

    std::ostringstream osnameBSCCable2;
    osnameBSCCable2 << "logicalBSCCable_2_"<<i;
    logicCable = FindLogicalVolume( osnameBSCCable2.str() );
    if(logicCable)
      logicCable->SetVisAttributes(G4VisAttributes::Invisible);

    std::ostringstream osnameOCGirder1;
    osnameOCGirder1 <<"logicalOpenningCutGirder_1_"<<i;
    logicOCGirder = FindLogicalVolume( osnameOCGirder1.str() );
    if(logicOCGirder)
      logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

    std::ostringstream osnameOCGirder2;
    osnameOCGirder2 <<"logicalOpenningCutGirder_2_"<<i;
    logicOCGirder = FindLogicalVolume( osnameOCGirder2.str() );
    if(logicOCGirder)
      logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

    std::ostringstream osnameOCGirder3;
    osnameOCGirder3 <<"logicalOpenningCutGirder_3_"<<i;
    logicOCGirder = FindLogicalVolume( osnameOCGirder3.str() );
    if(logicOCGirder)
      logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

    std::ostringstream osnameOCGirder4;
    osnameOCGirder4 <<"logicalOpenningCutGirder_4_"<<i;
    logicOCGirder = FindLogicalVolume( osnameOCGirder4.str() );
    if(logicOCGirder)
      logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);
  }

  //-------------------------------------------------------------
  //Support system
  logicSupportBar = FindLogicalVolume("logicalSupportBar0");
  logicSupportBar1 = FindLogicalVolume("logicalSupportBar1");
  logicEndRing = FindLogicalVolume("logicalEndRing");
  logicGear = FindLogicalVolume("logicalGear");
  logicTaperRing1 = FindLogicalVolume("logicalTaperRing1");
  logicTaperRing2 = FindLogicalVolume("logicalTaperRing2");
  logicTaperRing3 = FindLogicalVolume("logicalTaperRing3");

  //-------------------------------------------------------------
  //Endcap
  logicEnd = FindLogicalVolume("logicalEndWorld");

  for(G4int sector=0;sector<3;sector++) {
    std::ostringstream osnameEndPhi;
    osnameEndPhi<<"logicalEndPhi"<<sector;
    logicEndPhi = FindLogicalVolume(osnameEndPhi.str());
    if(logicEndPhi) {
      logicEndPhi->SetVisAttributes(G4VisAttributes::Invisible);
    } else {
      G4cout<<"Can't find logicEndPhi!"<<G4endl;
    }

    for(G4int cryNb=0;cryNb<35;cryNb++) {

      std::ostringstream osnameEndCrystal;
      osnameEndCrystal<<"logicalEndCrystal_"<<sector<<"_"<<cryNb;
      logicEndCrystal = FindLogicalVolume( osnameEndCrystal.str() );
      if(logicEndCrystal) {
        logicEndCrystal->SetSensitiveDetector(besEMCSD);
        G4VisAttributes* crystalVisAtt
          = new G4VisAttributes(G4Colour(0.5,0,1.0));
        crystalVisAtt->SetVisibility(false);
        logicEndCrystal->SetVisAttributes(crystalVisAtt);
      } else {
        G4cout<<"Can't find: "<<osnameEndCrystal.str()<<G4endl;
      }

      std::ostringstream osnameEndCasing;
      osnameEndCasing<<"logicalEndCasing_"<<sector<<"_"<<cryNb;
      logicEndCasing = FindLogicalVolume( osnameEndCasing.str() );
      if(logicEndCasing) {
        logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
      } else {
        G4cout<<"Can't find: "<<osnameEndCasing.str()<<G4endl;
      }
    }
  }
}

void BesEmcConstruction::DefineMaterials()
{
  G4String name, symbol;             //a=mass of a mole;
  G4double a, z, density;            //z=mean number of protons;  
  //  G4int iz, n;                       //iz=number of protons  in an isotope;
  // n=number of nucleons in an isotope;

  G4int ncomponents, natoms;
  G4double fractionmass;
  //G4double abundance, fractionmass;
  //  G4double temperature, pressure;

  //for debug
  //  G4Exception("BesEmcConstruction::DefineMaterials() starting...........");
  //
  // define Elements
  //
  G4Element* H=G4Element::GetElement("Hydrogen");
  if(!H)
  {
    a = 1.01*g/mole;
    H = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
  }
  G4Element* C=G4Element::GetElement("Carbon");
  if(!C)
  {
    a = 12.01*g/mole;
    C = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);
  }
  G4Element* O=G4Element::GetElement("Oxygen");
  if(!O)
  {
    a = 16.00*g/mole;
    O = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);  
  }

  density = 0.344*g/cm3;
  G4Material* Tyvek = new G4Material(name="M_Polyethylene", density, ncomponents=2);
  Tyvek->AddElement(C, natoms=1);
  Tyvek->AddElement(H, natoms=2);

  density = 1.39*g/cm3;
  G4Material* Mylar = new G4Material(name="M_PolyethyleneTerephthlate", density, ncomponents=3);
  Mylar->AddElement(C, natoms=5);
  Mylar->AddElement(H, natoms=4);
  Mylar->AddElement(O, natoms=2);

  density = 1.18*g/cm3;
  organicGlass = new G4Material(name="M_OrganicGlass", density, ncomponents=3);
  organicGlass->AddElement(C, natoms=5);
  organicGlass->AddElement(H, natoms=7);
  organicGlass->AddElement(O, natoms=2);

  G4Material *Fe = new G4Material(name="M_Iron", z=26., a=55.85*g/mole, density=7.87*g/cm3);
  G4Material *Cr = new G4Material(name="M_Chromium", z=24., a=52.00*g/mole, density=8.72*g/cm3);
  G4Material *Ni = new G4Material(name="M_Nickel", z=28., a=58.69*g/mole, density=8.72*g/cm3);

  stainlessSteel = new G4Material(name="M_Cr18Ni9", density=7.85*g/cm3, ncomponents=3);
  stainlessSteel->AddMaterial(Fe, fractionmass=73.*perCent);
  stainlessSteel->AddMaterial(Cr, fractionmass=18.*perCent);
  stainlessSteel->AddMaterial(Ni, fractionmass=9.*perCent);

  G4Material *H2O = G4Material::GetMaterial("Water");
  G4Material *Cu = G4Material::GetMaterial("Copper");
  G4double dWater = 1.*g/cm3; //density
  G4double dCopper = 8.96*g/cm3;
  G4double aWater = ((*besEMCGeometry).waterPipeDr-(*besEMCGeometry).waterPipeThickness)
    *((*besEMCGeometry).waterPipeDr-(*besEMCGeometry).waterPipeThickness);    //area
  G4double aCopper = (*besEMCGeometry).waterPipeDr*(*besEMCGeometry).waterPipeDr-aWater;
  density = (dWater*aWater+dCopper*aCopper)/(aWater+aCopper);

  waterPipe = new G4Material(name="M_WaterPipe", density, ncomponents=2);
  fractionmass = dWater*aWater/(dWater*aWater+dCopper*aCopper);
  waterPipe->AddMaterial(H2O, fractionmass);
  fractionmass = dCopper*aCopper/(dWater*aWater+dCopper*aCopper);
  waterPipe->AddMaterial(Cu, fractionmass);

  cable  = new G4Material(name="M_Cable", density=4.*g/cm3, ncomponents=1);
  cable->AddMaterial(Cu,1);

  //for debug
  //G4Exception("BesEmcConstruction::DefineMaterials() running one.........");
  //
  // predigest the casing of crystals to a mixture
  //

  G4Material* Al=G4Material::GetMaterial("Aluminium");
  if(Al==NULL)
  {
    Al = new G4Material(name="Aluminium", z=13., a=26.98*g/mole, density=2.700*g/cm3);
  }

  G4Material *Si=G4Material::GetMaterial("M_Silicon");
  if(Si==NULL)
  {
    Si = new G4Material(name="M_Silicon", z=14., a=28.0855*g/mole, density=2.33*g/cm3);
  }


  //for debug
  G4double totalThickness=(*besEMCGeometry).fTyvekThickness
    +(*besEMCGeometry).fAlThickness+(*besEMCGeometry).fMylarThickness;
  density = (Tyvek->GetDensity()*(*besEMCGeometry).fTyvekThickness+
      Al->GetDensity()*(*besEMCGeometry).fAlThickness+
      Mylar->GetDensity()*(*besEMCGeometry).fMylarThickness)
    /totalThickness;
  G4Material* Casing = new G4Material(name="M_Casing", density, ncomponents=3);
  Casing->AddMaterial(
      Tyvek,
      fractionmass=Tyvek->GetDensity()/density
      *(*besEMCGeometry).fTyvekThickness
      /totalThickness);
  Casing->AddMaterial(
      Al,
      fractionmass=Al->GetDensity()/density
      *(*besEMCGeometry).fAlThickness
      /totalThickness);
  Casing->AddMaterial(
      Mylar,
      fractionmass=Mylar->GetDensity()/density
      *(*besEMCGeometry).fMylarThickness
      /totalThickness);
  fCasingMaterial = Casing;
  rearCasingMaterial = Tyvek;
  //for debug
  //  G4Exception("BesEmcConstruction::DefineMaterials() running two........");
  fCrystalMaterial = G4Material::GetMaterial("Cesiumiodide");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BesEmcConstruction::PrintEMCParameters()
{
  G4cout << "-------------------------------------------------------"<< G4endl
    << "---> There are "
    << phiNbCrystals << "(max=" << (*besEMCGeometry).BSCNbPhi
    << ") crystals along phi direction and "
    << thetaNbCrystals << "(max=" << (*besEMCGeometry).BSCNbTheta
    << ") crystals along theta direction."<< G4endl
    << "The crystals have sizes of "
    << (*besEMCGeometry).BSCCryLength/cm << "cm(L) and "
    << (*besEMCGeometry).BSCYFront/cm << "cm(Y) with " 
    << fCrystalMaterial->GetName() <<"."<< G4endl
    << "The casing is layer of "
    << (*besEMCGeometry).fTyvekThickness/mm << "mm tyvek,"
    << (*besEMCGeometry).fAlThickness/mm << "mm aluminum and"
    << (*besEMCGeometry).fMylarThickness/mm << "mm mylar."<< G4endl
    << "-------------------------------------------------------"<< G4endl;
  G4cout << G4Material::GetMaterial("PolyethyleneTerephthlate") << G4endl
    << G4Material::GetMaterial("Casing") << G4endl
    << G4Material::GetMaterial("Polyethylene") << G4endl
    << "-------------------------------------------------------"<< G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetCrystalMaterial(G4String materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);     
  if (pttoMaterial)
  {fCrystalMaterial = pttoMaterial;
    logicBSCCrystal->SetMaterial(pttoMaterial); 
    PrintEMCParameters();
  }             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetCasingMaterial(G4String materialChoice)
{
  // search the material by its name 
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);  
  if (pttoMaterial)
  {fCasingMaterial = pttoMaterial;
    logicBSCTheta->SetMaterial(pttoMaterial);
    PrintEMCParameters();
  }             
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetCasingThickness(G4ThreeVector val)
{
  // change Gap thickness and recompute the calorimeter parameters
  (*besEMCGeometry).fTyvekThickness = val('X');
  (*besEMCGeometry).fAlThickness    = val('Y');
  (*besEMCGeometry).fMylarThickness = val('Z');
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCRmin(G4double val)
{
  (*besEMCGeometry).BSCRmin = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCNbPhi(G4int val)
{
  (*besEMCGeometry).BSCNbPhi = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCNbTheta(G4int val)
{
  (*besEMCGeometry).BSCNbTheta = val;
}

void BesEmcConstruction::SetStartIDTheta(G4int val)
{
  startID = val;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCCrystalLength(G4double val)
{
  (*besEMCGeometry).BSCCryLength = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCYFront0(G4double val)
{
  (*besEMCGeometry).BSCYFront0 = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCYFront(G4double val)
{
  (*besEMCGeometry).BSCYFront = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCPosition0(G4double val)
{
  (*besEMCGeometry).BSCPosition0 = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetBSCPosition1(G4double val)
{
  (*besEMCGeometry).BSCPosition1 = val;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr 
    = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if(magField) delete magField;		//delete the existing magn field

  if(fieldValue!=0.)			// create a new one if non nul
  { magField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));        
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
    fmagField=fieldValue;
  } else {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
    fmagField=0.;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//??????????????????????????????????????????????? 
void BesEmcConstruction::UpdateGeometry()
{
  ;//G4RunManager::GetRunManager()->DefineWorldVolume(BesDetectorConstruction::Construct());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  void
BesEmcConstruction::TransformToArb8( const G4ThreeVector fPnt[8], G4ThreeVector newfPnt[8], G4ThreeVector &center, G4ThreeVector &rotAngle )
{
  HepPoint3D point[8];
  center = G4ThreeVector(0.0, 0.0, 0.0);
  for (int i = 0; i < 8; i++) {
    point[i] = HepPoint3D( fPnt[i].x(), fPnt[i].y(), fPnt[i].z() );
    center += point[i];
  }
  center /= 8.0;

  HepPlane3D bottomPlane( point[4], point[5], point[6] );
  HepPoint3D centerProject = bottomPlane.point( center );
  Hep3Vector newZ = center - centerProject;

  rotAngle = RotAngleFromNewZ( newZ );
  G4RotationMatrix *g4Rot = new G4RotationMatrix();
  g4Rot->rotateX( rotAngle.x() );
  g4Rot->rotateY( rotAngle.y() );

  G4AffineTransform *transform = new G4AffineTransform( g4Rot, center );
  transform->Invert();
  for (int i = 0; i < 8; i++) {
    newfPnt[i] = transform->TransformPoint(fPnt[i]);
  }
  delete g4Rot;
  delete transform;
}


  void 
BesEmcConstruction::ThreeVectorTrans(G4ThreeVector fPnt[8], double x[8], double y[8], double z[8])
{
  for (int i = 0; i < 8; i++) {
    x[i] = fPnt[i].x();
    y[i] = fPnt[i].y();
    z[i] = fPnt[i].z();
  }
}


  Hep3Vector
BesEmcConstruction::RotAngleFromNewZ( Hep3Vector newZ )
{
  newZ.setMag(1.0);
  Hep3Vector x0(1, 0, 0), y0(0, 1, 0), z0(0, 0, 1);
  double dx, dy, dz = 0.0; 

  Hep3Vector a(0.0, newZ.y(), newZ.z());
  // three rotated angles, rotate dx angles(rad) by X axis,
  // then rotate dy by new Y axis, then dz by new Z axis;
  // all rotate in left hand system;

  // a, project of final newZ on original YZ plane, 0 <= theta < pi;
  if(a.mag() != 0.0) a.setMag(1.0);
  else cout << "newZ on X axis, a=(0,0,0)" << endl;
  dx = acos(a.dot(z0));
  if(a.dot(z0) == -1.0) dx = 0.0;

  // b, rotate dx angle of z0 around original X axis(x0), 
  Hep3Vector b(0, sin(dx), cos(dx));
  dy = acos(b.dot(newZ));
  if(newZ.x() > 0.0) dy = -dy;

  Hep3Vector v(dx, dy, dz);
  return v;
}
