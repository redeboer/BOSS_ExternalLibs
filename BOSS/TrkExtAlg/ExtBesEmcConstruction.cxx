#include "TrkExtAlg/ExtBesEmcConstruction.h"

#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

//For code construction --------------------
#include "G4ReflectionFactory.hh"

//#include "BesEmcConstruction.hh"
//#include "BesEmcDetectorMessenger.hh"
#include "TrkExtAlg/ExtBesEmcGeometry.h"
#include "TrkExtAlg/ExtBesCrystalParameterisation.h"
#include "TrkExtAlg/ExtBesEmcEndGeometry.h"
//#include "ReadBoostRoot.hh"

//#include "BesEmcSD.hh"
#include "G4IrregBox.hh"
#include "G4Box.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Trap.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4Polyhedra.hh"
//#include "G4LogicalVolume.hh"
//#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
//#include "G4PVPlacement.hh"
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

#include "G4ios.hh"
//--------------------------------------------
#include "G4Geo/EmcG4Geo.h"


ExtBesEmcConstruction* ExtBesEmcConstruction::fBesEmcConstruction=0;

ExtBesEmcConstruction::ExtBesEmcConstruction()
   :verboseLevel(0),
   solidEMC(0),logicEMC(0),physiEMC(0),
   solidBSCPhi(0),logicBSCPhi(0),physiBSCPhi(0),
   solidBSCTheta(0),logicBSCTheta(0),physiBSCTheta(0),
   solidBSCCrystal(0),logicBSCCrystal(0),physiBSCCrystal(0),
   magField(0),detectorMessenger(0),
   besEMCSD(0),crystalParam(0) 
{
	if(!fBesEmcConstruction) fBesEmcConstruction=this;
	
	startID           = 1;
  	phiNbCrystals     = 0;
  	thetaNbCrystals   = 0;
  	besEMCGeometry = new ExtBesEmcGeometry();
  	emcEnd = new ExtBesEmcEndGeometry();	
}

ExtBesEmcConstruction::~ExtBesEmcConstruction() 
{
  if(crystalParam) delete crystalParam;
  if(besEMCGeometry) delete besEMCGeometry;
  if(emcEnd) delete emcEnd;
}

void ExtBesEmcConstruction::Construct(G4LogicalVolume* logicBes)
{
  /*
        G4String GeometryPath = getenv("SIMUTILROOT");
        GeometryPath+="/dat/Emc.gdml";
        
        m_config.SetURI(GeometryPath);
        m_config.SetSetupName( "Emc" );
        m_config.SetSetupVersion( "1.0" );
        m_config.SetType( "G4" );
        m_sxp.Configure(&m_config);
        m_sxp.Initialize();

        //construct Emc
        m_sxp.Run();

        logicalEmc = FindLogicalVolume("logicalEmc");
        physicalEmc = new G4PVPlacement(0,G4ThreeVector(0.0 ,0.0 ,0.0),logicalEmc, "BSC",logicBes, false, 0);

        m_sxp.Finalize();
  */

//new version of geometry construction from gdml  
  logicEMC = EmcG4Geo::Instance()->GetTopVolume();
  if(logicEMC) physiEMC = new G4PVPlacement(0,G4ThreeVector(0.0 ,0.0 ,0.0),logicEMC, "physicalEMC",logicBes, false, 0);

  
//geometry construction by code
/*
  besEMCGeometry->ComputeEMCParameters();
  emcEnd->ComputeParameters();
  DefineMaterials();
  phiNbCrystals     = (*besEMCGeometry).BSCNbPhi;
    thetaNbCrystals   = (*besEMCGeometry).BSCNbTheta*2;
    //
    //BSC
    //
    //G4cout << "Acquired " << G4Material::GetMaterial("Air") << G4endl;
	
    solidBSC = new G4Tubs("BSC",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).BSCRmax+(*besEMCGeometry).SPBarThickness+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz,
			0.*deg,
			360.*deg);

    solidESC = new G4Tubs("ESC",(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,(*emcEnd).WorldDz/2,0.*deg,360.*deg);

    solidEMC = new G4UnionSolid("EMC",
        solidBSC,
        solidESC,
        0,
        G4ThreeVector(0,0,(*emcEnd).WorldZPosition));

    solidEMC = new G4UnionSolid("EMC",
        solidEMC,
        solidESC,
        0,
        G4ThreeVector(0,0,-(*emcEnd).WorldZPosition));
    
    logicEMC = new G4LogicalVolume(solidEMC,
			 G4Material::GetMaterial("Air"),
			 "EMC");
    
    physiEMC = new G4PVPlacement(0,
        0,
        logicEMC,
        "EMC",
        logicBes,
        false,
        0);

    //  G4RotationMatrix *rotateMatrixBSC;
    //  rotateMatrixBSC = new G4RotationMatrix();
    //  G4ThreeVector newAxisX=G4ThreeVector(1.,0,0);
    //  G4ThreeVector newAxisY=G4ThreeVector(0,-1.,0);
    //  G4ThreeVector newAxisZ=G4ThreeVector(0,0,-1.);
    //  rotateMatrixBSC->rotateAxes(newAxisX,newAxisY,newAxisZ);
    //  rotateMatrixBSC->rotateX(180.*deg);
    //  physiBSC = new G4PVPlacement(rotateMatrixBSC,
    //		       G4ThreeVector(0,0,-BSCDz/2.),
    //		       "BSC",
    //		       logicBSC,
    //		       logicEMC,
    //		       false,
    //		       1);
    //
    // Phi Cell
    solidBSCPhiTub = new G4Tubs(
			   "BSCPhiTub",
			   (*besEMCGeometry).BSCRmin,
			   (*besEMCGeometry).BSCPhiRmax,
			   (*besEMCGeometry).BSCDz,
			   360.*deg-(*besEMCGeometry).BSCPhiDphi,
			   (*besEMCGeometry).BSCPhiDphi);
    solidConsPhi = new G4Cons("consPhi",
         (*besEMCGeometry).BSCRmin1,
         (*besEMCGeometry).BSCRmax1,
         (*besEMCGeometry).BSCRmin2,
         (*besEMCGeometry).BSCRmax2,
         (*besEMCGeometry).BSCDz1/2,
         0.*deg,
         360.*deg);
    solidBSCPhi1 = new G4SubtractionSolid("BSCPhi1",
         solidBSCPhiTub,
         solidConsPhi,
         0,
         G4ThreeVector(0,0,(*besEMCGeometry).BSCDz-(*besEMCGeometry).BSCDz1/2));
    solidConsPhi = new G4Cons("consPhi",
         (*besEMCGeometry).BSCRmin2,
         (*besEMCGeometry).BSCRmax2,
         (*besEMCGeometry).BSCRmin1,
         (*besEMCGeometry).BSCRmax1,
         (*besEMCGeometry).BSCDz1/2,
         0.*deg,
         360.*deg);
    solidBSCPhi = new G4SubtractionSolid("BSCPhi",
         solidBSCPhi1,
         solidConsPhi,
         0,
         G4ThreeVector(0,0,(*besEMCGeometry).BSCDz1/2-(*besEMCGeometry).BSCDz));
  
    logicBSCPhi = new G4LogicalVolume(solidBSCPhi,
				    G4Material::GetMaterial("Air"),
				    "BSCPhi");

    //new geometry by Fu Chengdong
    G4RotationMatrix *rotateMatrix[200];
    G4double oOp,ox,oy,oz;  
    G4int i;
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
    
    i=1;
    for(i=1;i<=(*besEMCGeometry).BSCNbPhi;i++)
    {
      rotateMatrix[i-1] = new G4RotationMatrix();
      rotateMatrix[i-1]->rotateZ(-(i-1)*(*besEMCGeometry).BSCPhiDphi
          -(*besEMCGeometry).BSCAngleRotat
          -(*besEMCGeometry).BSCPhiDphi/2.
          -hoi);
      rotateMatrix[i-1]->getAngleAxis(delta, axis);
      //G4cout << "The axis of crystals in the world system is: "
      //   << delta/deg << "(deg)(delta) "
      //<< axis  << "(Z axis)"<< G4endl;
      ox=oOp*cos(-90.*deg+(*besEMCGeometry).BSCAngleRotat
          +(i-1)*(*besEMCGeometry).BSCPhiDphi);
      oy=oOp*sin(-90.*deg+(*besEMCGeometry).BSCAngleRotat
          +(i-1)*(*besEMCGeometry).BSCPhiDphi);
      oz=0*cm;
      physiBSCPhi = new G4PVPlacement(rotateMatrix[i-1],
          G4ThreeVector(ox,oy,oz),
          logicBSCPhi,
          "BSCPhi",
          logicEMC,
          false,
          i-1);
    }
*/      
      /*  
  G4RotationMatrix *rotateMatrix[200];
  //  rotateMatrix = new G4RotationMatrix();
  //  rotateMatrix->rotateZ(BSCAngleRotat-BSCPhiDphi);
  G4double oOp,ox,oy,oz; 
  G4int i;
  G4double delta = 0*deg;
  G4ThreeVector axis = G4ThreeVector(0,0,0);
  oOp=(*besEMCGeometry).BSCRmin/sin(0.5*(*besEMCGeometry).BSCPhiDphi+90*deg)
    *sin((*besEMCGeometry).BSCAngleRotat);
  i=1;
  for(i=1;i<=(*besEMCGeometry).BSCNbPhi;i++)
    {
      rotateMatrix[i-1] = new G4RotationMatrix();
      rotateMatrix[i-1]->rotateZ(-(i-1)*(*besEMCGeometry).BSCPhiDphi
				 +(*besEMCGeometry).BSCAngleRotat
				 +(*besEMCGeometry).BSCPhiDphi/2.);
      rotateMatrix[i-1]->getAngleAxis(delta, axis);
      //G4cout << "The axis of crystals in the world system is: "
        //   << delta/deg << "(deg)(delta) "
	     //<< axis  << "(Z axis)"<< G4endl;
	ox=oOp*cos(90.*deg-(*besEMCGeometry).BSCAngleRotat
		   +(i-1)*(*besEMCGeometry).BSCPhiDphi);
	oy=oOp*sin(90.*deg-(*besEMCGeometry).BSCAngleRotat
		   +(i-1)*(*besEMCGeometry).BSCPhiDphi);
	oz=0*cm;
	physiBSCPhi = new G4PVPlacement(rotateMatrix[i-1],
					G4ThreeVector(ox,oy,oz),
          logicBSCPhi,
					"BSCPhi",
					logicEMC,
					false,
					i);
	//G4cout << G4ThreeVector(ox/cm,oy/cm,oz/cm) <<"(cm)" << G4endl
	  //   << (-(*besEMCGeometry).BSCAngleRotat+(i-1)*(*besEMCGeometry).BSCPhiDphi)/deg <<"(degree)" << G4endl;
     }*/
    //
    //Crystals
    //
/*    G4double zHalfLength[50];
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
      }
    
    besEMCGeometry->ModifyForCasing();
    
    solidBSCCrystal = new G4Trap("Crystal",
				 100*cm, 100*deg, 100*deg,
				 100*cm, 100*cm,  100*cm,  100*deg,
				 100*cm, 100*cm,  100*cm,  100*deg);
    
    logicBSCCrystal = new G4LogicalVolume(solidBSCCrystal,
					  fCrystalMaterial,
					  "Crystal");
    
    crystalParam = new ExtBesCrystalParameterisation
    (startID,
     thetaNbCrystals,
     (*besEMCGeometry).BSCNbTheta*2,
     besEMCGeometry,
     verboseLevel);

    //---------------------------------------------------------------------------------
    //rear substance
    solidRear = new G4Box("RearBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxDz/2);

    logicRear = new G4LogicalVolume(solidRear,
        G4Material::GetMaterial("Air"),
        "RearBox");

    //organic glass
    solidOrgGlass = new G4Box("OrganicGlass",
        (*besEMCGeometry).orgGlassLengthX/2,
        (*besEMCGeometry).orgGlassLengthY/2,
        (*besEMCGeometry).orgGlassLengthZ/2);

    logicOrgGlass = new G4LogicalVolume(solidOrgGlass,
        organicGlass,
        "OrganicGlass");

    physiOrgGlass = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz-(*besEMCGeometry).orgGlassLengthZ)/2),
        logicOrgGlass,
        "OrganicGlass",
        logicRear,
        false,
        0);

    //casing
    solidCasingBox = new G4Box("CasingBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearCasingThickness/2);

    solidAirHole = new G4Box("AirHole",
        (*besEMCGeometry).orgGlassLengthX/2,
        (*besEMCGeometry).orgGlassLengthY/2,
        (*besEMCGeometry).rearBoxDz/2);      //any value more than casing thickness

    solidRearCasing = new G4SubtractionSolid("RearCasing",
        solidCasingBox,
        solidAirHole,
        0,
        0);

    logicRearCasing = new G4LogicalVolume(solidRearCasing,
        rearCasingMaterial,
        "RearCasing");

    physiRearCasing = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz-(*besEMCGeometry).rearCasingThickness)/2),
        logicRearCasing,
        "RearCasing",
        logicRear,
        false,
        0);

    //Al Plate
    solidAlBox = new G4Box("AlBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).AlPlateDz/2);

    solidAlPlate = new G4SubtractionSolid("AlPlate",
        solidAlBox,
        solidAirHole,
        0,
        0);

    logicAlPlate = new G4LogicalVolume(solidAlPlate,
        G4Material::GetMaterial("Aluminium"),
        "AlPlate");

    physiAlPlate = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
                            -(*besEMCGeometry).rearCasingThickness
                            -(*besEMCGeometry).AlPlateDz/2)),
        logicAlPlate,
        "AlPlate",
        logicRear,
        false,
        0);

    //photodiode
    solidPD = new G4Box("PD",
        (*besEMCGeometry).PDLengthX,     //two PD
        (*besEMCGeometry).PDLengthY/2,
        (*besEMCGeometry).PDLengthZ/2);

    logicPD = new G4LogicalVolume(solidPD,
        G4Material::GetMaterial("Silicon"),
        "PD");

    physiPD = new G4PVPlacement(0,
         G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
                             -(*besEMCGeometry).orgGlassLengthZ
                             -(*besEMCGeometry).PDLengthZ/2)),
         logicPD,
         "PD",
         logicRear,
         false,
         0);

    //preamplifier box
    solidPreAmpBox = new G4Box("PreAmpBox",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).PABoxDz/2);

    logicPreAmpBox = new G4LogicalVolume(solidPreAmpBox,
        G4Material::GetMaterial("Aluminium"),
        "PreAmpBox");

    physiPreAmpBox = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
                            -(*besEMCGeometry).rearCasingThickness
                            -(*besEMCGeometry).AlPlateDz
                            -(*besEMCGeometry).PABoxDz/2)),
        logicPreAmpBox,
        "PreAmpBox",
        logicRear,
        false,
        0);

    //air in preamplifier box
    solidAirInPABox = new G4Box("AirInPABox",
        (*besEMCGeometry).rearBoxLength/2-(*besEMCGeometry).PABoxThickness,
        (*besEMCGeometry).rearBoxLength/2-(*besEMCGeometry).PABoxThickness,
        (*besEMCGeometry).PABoxDz/2-(*besEMCGeometry).PABoxThickness);

    logicAirInPABox = new G4LogicalVolume(solidAirInPABox,
        G4Material::GetMaterial("Air"),
        "AirInPABox");
    
    physiAirInPABox = new G4PVPlacement(0,
        0,
        logicAirInPABox,
        "AirInPABox",
        logicPreAmpBox,
        false,
        0);

    //stainless steel for hanging the crystal
    solidHangingPlate = new G4Box("HangingPlate",
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).rearBoxLength/2,
        (*besEMCGeometry).HangingPlateDz/2);

    logicHangingPlate = new G4LogicalVolume(solidHangingPlate,stainlessSteel,"HangingPlate");

    physiHangingPlate = new G4PVPlacement(0,
        G4ThreeVector(0,0,-((*besEMCGeometry).rearBoxDz/2
            -(*besEMCGeometry).rearCasingThickness
            -(*besEMCGeometry).AlPlateDz
            -(*besEMCGeometry).PABoxDz
            -(*besEMCGeometry).HangingPlateDz/2)),
        logicHangingPlate,
        "HangingPlate",
        logicRear,
        false,
        0);

    //water pipe
    solidWaterPipe = new G4Tubs("WaterPipe",
        0,
        (*besEMCGeometry).waterPipeDr,
        (*besEMCGeometry).BSCDz,
        0.*deg,
        360.*deg);

    logicWaterPipe = new G4LogicalVolume(solidWaterPipe,stainlessSteel,"WaterPipe");
    
    physiWaterPipe = new G4PVPlacement(0,
        G4ThreeVector((*besEMCGeometry).cablePosX[0]-2*(*besEMCGeometry).cableDr,
          (*besEMCGeometry).cablePosY[0]-(*besEMCGeometry).cableDr-(*besEMCGeometry).waterPipeDr,
          0),
        logicWaterPipe,
        "WaterPipe",
        logicBSCPhi,
        false,
        0);
    //---------------------------------------------------------------------------------
    
    //
    //Theta Cell
    //
    G4String nameCrystalAndCasing="CrystalAndCasing";
    G4int id=0;     //ID of crystals after distinguishing left and right
    for(i=startID;i<=thetaNbCrystals;i++)
      {
	if(i>(*besEMCGeometry).BSCNbTheta)
	  {
	    id=i-(*besEMCGeometry).BSCNbTheta-1;
	    solidBSCTheta = new G4Trap(nameCrystalAndCasing,
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
	    
	  logicBSCTheta = new G4LogicalVolume(solidBSCTheta,
					      fCasingMaterial,
					      nameCrystalAndCasing);
	  
	  rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1] = new G4RotationMatrix();
	  rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]->rotateZ(-90*deg);
	  rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]
	    ->rotateX(-thetaPosition[id]);
	  physiBSCTheta = 
	    new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
			      G4ThreeVector(xPosition[id],
					    yPosition[id],
					    zPosition[id]),
			      nameCrystalAndCasing,
			      logicBSCTheta,
			      physiBSCPhi,
			      false,
			      thetaNbCrystals-i);
	  if(logicBSCTheta)
	    logicBSCTheta->SetVisAttributes(G4VisAttributes::Invisible);

    if(id<(*besEMCGeometry).BSCNbTheta-1)
    {
      physiRear = new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
        G4ThreeVector((*besEMCGeometry).rearBoxPosX[id],
          (*besEMCGeometry).rearBoxPosY[id],
          (*besEMCGeometry).rearBoxPosZ[id]),
        "RearBox",
        logicRear,
        physiBSCPhi,
        false,
        thetaNbCrystals-i);

      solidOCGirder = new G4Cons("OpenningCutGirder",
        (*besEMCGeometry).OCGirderRmin1[id],
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).OCGirderRmin2[id],
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).OCGirderDz[id]/2,
        360.*deg-(*besEMCGeometry).OCGirderAngle/2,
        (*besEMCGeometry).OCGirderAngle/2);

      logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,"OpenningCutGirder");
      //logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);
    
      physiOCGirder = new G4PVPlacement(0,
        G4ThreeVector(0,0,(*besEMCGeometry).OCGirderPosZ[id]),
        logicOCGirder,
        "OpenningCutGirder",
        logicBSCPhi,
        false,
        0);
      
      G4double zLength = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderPosZ[id]
        -(*besEMCGeometry).OCGirderDz[id+1]/2-(*besEMCGeometry).OCGirderDz[id]/2;
      G4double zPosition = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderDz[id+1]/2-zLength/2;
      
      solidOCGirder = new G4Cons("OpenningCutGirder",
          (*besEMCGeometry).OCGirderRmin2[id],
          (*besEMCGeometry).BSCPhiRmax,
          (*besEMCGeometry).OCGirderRmin1[id+1],
          (*besEMCGeometry).BSCPhiRmax,
          zLength/2,
          360.*deg-(*besEMCGeometry).OCGirderAngle/2,
          (*besEMCGeometry).OCGirderAngle/2);
      
      logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,"OpenningCutGirder");
      //logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

      physiOCGirder = new G4PVPlacement(0,
          G4ThreeVector(0,0,zPosition),
          logicOCGirder,
          "OpenningCutGirder",
          logicBSCPhi,
          false,
          0);
    }
    
    solidCable = new G4Tubs("BSCCable",
        0,
        (*besEMCGeometry).cableDr,
        (*besEMCGeometry).cableLength[id]/2,
        0.*deg,
        360.*deg);

    logicCable = new G4LogicalVolume(solidCable,cable,"BSCCable");

    physiCable = new G4PVPlacement(0,
        G4ThreeVector((*besEMCGeometry).cablePosX[id],
          (*besEMCGeometry).cablePosY[id],
          (*besEMCGeometry).cablePosZ[id]),
        logicCable,
        "BSCCable",
        logicBSCPhi,
        false,
        0);
    //logicCable->SetVisAttributes(G4VisAttributes::Invisible);
  
	  }
	else
	  {
	    id=(*besEMCGeometry).BSCNbTheta-i;
	    solidBSCTheta = new G4Trap(nameCrystalAndCasing,
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
	    
	    logicBSCTheta = new G4LogicalVolume(solidBSCTheta,
						fCasingMaterial,
						nameCrystalAndCasing);
	    
	    rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1] = new G4RotationMatrix();
	    rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]->rotateZ(-90*deg);
	    rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1]
	      ->rotateX(-180*deg+thetaPosition[id]);
	    physiBSCTheta = 
	      new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
				G4ThreeVector(xPosition[id],
          yPosition[id],
          -zPosition[id]),
				nameCrystalAndCasing,
				logicBSCTheta,
				physiBSCPhi,
				false,
				thetaNbCrystals-i);
	    if(logicBSCTheta)
	      {
		//G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0,0.,0.));
		//rightVisAtt->SetVisibility(true);
		//logicBSCTheta->SetVisAttributes(rightVisAtt);
		//logicBSCTheta->SetVisAttributes(G4VisAttributes::Invisible);
	      }
      
      if(id<(*besEMCGeometry).BSCNbTheta-1)
      {
    physiRear = new G4PVPlacement(rotateMatrix[(*besEMCGeometry).BSCNbPhi+i-1],
        G4ThreeVector((*besEMCGeometry).rearBoxPosX[id],
          (*besEMCGeometry).rearBoxPosY[id],
          -(*besEMCGeometry).rearBoxPosZ[id]),
        "RearBox",
        logicRear,
        physiBSCPhi,
        false,
        thetaNbCrystals-i);
    
	  solidOCGirder = new G4Cons("OpenningCutGirder",
        (*besEMCGeometry).OCGirderRmin2[id],
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).OCGirderRmin1[id],
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).OCGirderDz[id]/2,
        360.*deg-(*besEMCGeometry).OCGirderAngle/2,
        (*besEMCGeometry).OCGirderAngle/2);

    logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,"OpenningCutGirder");
    //logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);
    
    physiOCGirder = new G4PVPlacement(0,
        G4ThreeVector(0,0,-(*besEMCGeometry).OCGirderPosZ[id]),
        logicOCGirder,
        "OpenningCutGirder",
        logicBSCPhi,
        false,
        0);
    
    G4double zLength = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderPosZ[id]
      -(*besEMCGeometry).OCGirderDz[id+1]/2-(*besEMCGeometry).OCGirderDz[id]/2;
    G4double zPosition = (*besEMCGeometry).OCGirderPosZ[id+1]-(*besEMCGeometry).OCGirderDz[id+1]/2-zLength/2;
      
    solidOCGirder = new G4Cons("OpenningCutGirder",
        (*besEMCGeometry).OCGirderRmin1[id+1],
        (*besEMCGeometry).BSCPhiRmax,
        (*besEMCGeometry).OCGirderRmin2[id],
        (*besEMCGeometry).BSCPhiRmax,
        zLength/2,
        360.*deg-(*besEMCGeometry).OCGirderAngle/2,
        (*besEMCGeometry).OCGirderAngle/2);
      
    logicOCGirder = new G4LogicalVolume(solidOCGirder,stainlessSteel,"OpenningCutGirder");
    //logicOCGirder->SetVisAttributes(G4VisAttributes::Invisible);

    physiOCGirder = new G4PVPlacement(0,
        G4ThreeVector(0,0,-zPosition),
        logicOCGirder,
        "OpenningCutGirder",
        logicBSCPhi,
        false,
        0);
      }
    
	  solidCable = new G4Tubs("BSCCable",
        0,
        (*besEMCGeometry).cableDr,
        (*besEMCGeometry).cableLength[id]/2,
        0.*deg,
        360.*deg);

    logicCable = new G4LogicalVolume(solidCable,cable,"BSCCable");

    physiCable = new G4PVPlacement(0,
        G4ThreeVector((*besEMCGeometry).cablePosX[id],
          (*besEMCGeometry).cablePosY[id],
          -(*besEMCGeometry).cablePosZ[id]),
        logicCable,
        "BSCCable",
        logicBSCPhi,
        false,
        0);
    //logicCable->SetVisAttributes(G4VisAttributes::Invisible);
	  }

	physiBSCCrystal = new G4PVParameterised(
					      "Crystal",
					      logicBSCCrystal,
					      physiBSCTheta,
					      kZAxis,
					      1,//for this method,it must be 1.
					      crystalParam);
	(*besEMCGeometry).physiBSCCrystal[i]=physiBSCCrystal;
	//G4cout << (*besEMCGeometry).physiBSCCrystal[i] << G4endl;
	physiBSCCrystal->SetCopyNo(thetaNbCrystals-i);
	if(verboseLevel>4)
	G4cout << "BesEmcConstruction*****************************"<< G4endl
	       << "point of crystal =" <<physiBSCCrystal << G4endl
	  //	       << "point of mother  =" <<physiBSCCrystal->GetMotherPhysical() << G4endl
	       << "point of excepted=" <<physiBSCTheta << G4endl;
	//G4Exception("BesEMCConstruction::Construct() starting............");
      }
*/    
    //                               
    // Sensitive Detectors: Absorber and Gap
    //
    //if (logicBSCCrystal)
    //  logicBSCCrystal->SetSensitiveDetector(besEMCSD);
    
    //                                        
    // Visualization attributes
    //
    /*    if(logicEMC)
      {
	G4VisAttributes* bscVisAtt= new G4VisAttributes(G4Colour(0.5,0.5,0.5));
	bscVisAtt->SetVisibility(false);
	logicEMC->SetVisAttributes(bscVisAtt);
	//logicBSC->SetVisAttributes(G4VisAttributes::Invisible);
      }
    if(logicBSCPhi)
      {
	G4VisAttributes* rightVisAtt= new G4VisAttributes(G4Colour(1.0,0.,1.0));
	rightVisAtt->SetVisibility(false);
	logicBSCPhi->SetVisAttributes(rightVisAtt);
	//logicBSCPhi->SetVisAttributes(G4VisAttributes::Invisible);
      }
    if(logicBSCCrystal)
      {
	G4VisAttributes* crystalVisAtt= new G4VisAttributes(G4Colour(0,0,1.0));
	crystalVisAtt->SetVisibility(true);
	logicBSCCrystal->SetVisAttributes(crystalVisAtt);
	//logicBSCCrystal->SetVisAttributes(G4VisAttributes::Invisible);
      }
    if(logicOrgGlass)
    {
      G4VisAttributes* glassVisAtt = new G4VisAttributes(G4Colour(0.5,1.0,1.0));
      glassVisAtt->SetVisibility(false);
      logicOrgGlass->SetVisAttributes(glassVisAtt);
    }
    if(logicRearCasing)
    {
      logicRearCasing->SetVisAttributes(G4VisAttributes::Invisible);
    }
    if(logicAlPlate)
    {
      G4VisAttributes* AlPlateVisAtt = new G4VisAttributes(G4Colour(0.9,0.9,1.0));
      AlPlateVisAtt->SetVisibility(true);
      logicAlPlate->SetVisAttributes(AlPlateVisAtt);
      logicAlPlate->SetVisAttributes(G4VisAttributes::Invisible);
    }
    logicRear->SetVisAttributes(G4VisAttributes::Invisible);
    logicPreAmpBox->SetVisAttributes(G4VisAttributes::Invisible);
    logicHangingPlate->SetVisAttributes(G4VisAttributes::Invisible);
    logicAirInPABox->SetVisAttributes(G4VisAttributes::Invisible);
    logicPD->SetVisAttributes(G4VisAttributes::Invisible);
    logicWaterPipe->SetVisAttributes(G4VisAttributes::Invisible);
    //
    //always return the physical World
    //
    if(verboseLevel>0)PrintEMCParameters();
    //  return physiBSC;
    */
//    ConstructEndGeometry(logicEMC);
//    ConstructSPFrame(logicEMC,besEMCGeometry);  
}


void ExtBesEmcConstruction::DefineMaterials()
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
  G4Material* Tyvek = new G4Material(name="Polyethylene", density, ncomponents=2);
  Tyvek->AddElement(C, natoms=1);
  Tyvek->AddElement(H, natoms=2);

  density = 1.39*g/cm3;
  G4Material* Mylar = new G4Material(name="PolyethyleneTerephthlate", density, ncomponents=3);
  Mylar->AddElement(C, natoms=5);
  Mylar->AddElement(H, natoms=4);
  Mylar->AddElement(O, natoms=2);

  density = 1.18*g/cm3;
  organicGlass = new G4Material(name="OrganicGlass", density, ncomponents=3);
  organicGlass->AddElement(C, natoms=5);
  organicGlass->AddElement(H, natoms=7);
  organicGlass->AddElement(O, natoms=2);

  G4Material *Fe = new G4Material(name="Iron", z=26., a=55.85*g/mole, density=7.87*g/cm3);
  G4Material *Cr = new G4Material(name="Chromium", z=24., a=52.00*g/mole, density=8.72*g/cm3);
  G4Material *Ni = new G4Material(name="Nickel", z=28., a=58.69*g/mole, density=8.72*g/cm3);

  stainlessSteel = new G4Material(name="0Cr18Ni9", density=7.85*g/cm3, ncomponents=3);
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

  waterPipe = new G4Material(name="WaterPipe", density, ncomponents=2);
  fractionmass = dWater*aWater/(dWater*aWater+dCopper*aCopper);
  waterPipe->AddMaterial(H2O, fractionmass);
  fractionmass = dCopper*aCopper/(dWater*aWater+dCopper*aCopper);
  waterPipe->AddMaterial(Cu, fractionmass);

  cable  = new G4Material(name="Cable", density=4.*g/cm3, ncomponents=1);
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
  
  G4Material *Si=G4Material::GetMaterial("Silicon");
  if(Si==NULL)
  {
    Si = new G4Material(name="Silicon", z=14., a=28.0855*g/mole, density=2.33*g/cm3);
  }
  
  //for debug
  G4double totalThickness=(*besEMCGeometry).fTyvekThickness
    +(*besEMCGeometry).fAlThickness+(*besEMCGeometry).fMylarThickness;
  density = (Tyvek->GetDensity()*(*besEMCGeometry).fTyvekThickness+
	     Al->GetDensity()*(*besEMCGeometry).fAlThickness+
	     Mylar->GetDensity()*(*besEMCGeometry).fMylarThickness)
    /totalThickness;
  G4Material* Casing = new G4Material(name="Casing", density, ncomponents=3);
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


void ExtBesEmcConstruction::ConstructEndGeometry(G4LogicalVolume* logicEM)
{
  G4Material* fCrystalMaterial = G4Material::GetMaterial("Cesiumiodide");
  //  G4VisAttributes* crystalVisAtt= new G4VisAttributes(G4Colour(0.5,0,1.0));
  //crystalVisAtt->SetVisibility(true);
  //G4VisAttributes* endPhiVisAtt= new G4VisAttributes(G4Colour(0,1.0,0));
  //endPhiVisAtt->SetVisibility(false);
  
  //world volume of endcap
  //east end
  solidEnd = new G4Cons("EndWorld",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
                  (*emcEnd).WorldDz/2,0.*deg,360.*deg);
  logicEnd = new G4LogicalVolume(solidEnd, G4Material::GetMaterial("Air"), "EndWorld", 0, 0, 0);
  physiEnd = new G4PVPlacement(0,               // no rotation
                G4ThreeVector(0,0,(*emcEnd).WorldZPosition),
                logicEnd,               // its logical volume
                "EndWorld",             // its name
                logicEMC,               // its mother  volume
                false,                  // no boolean operations
                0);                     // no field specific to volume
  //  if(logicEnd)
  //  logicEnd->SetVisAttributes(G4VisAttributes::Invisible);

  
  //west end
  G4RotationMatrix *rotateEnd = new G4RotationMatrix();
  rotateEnd->rotateY(180.*deg);
  physiEnd = new G4PVPlacement(rotateEnd,
     G4ThreeVector(0,0,-(*emcEnd).WorldZPosition),
     logicEnd,
     "EndWorld",
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
  solidEndPhi = new G4Cons("EndPhi",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
                     (*emcEnd).WorldDz/2,0*deg,22.5*deg);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "EndPhi", 0, 0, 0);
  for(G4int i=0;i<14;i++)
  {
    if((i!=6)&&(i!=7))
    {
      G4RotationMatrix *rotatePhi = new G4RotationMatrix();
      rotatePhi->rotateZ(-i*22.5*deg+67.5*deg);
      physiEndPhi = new G4PVPlacement(rotatePhi,0,logicEndPhi,"EndPhi",logicEnd,false,i);
    }
  }
  //if(logicEndPhi)
  //  logicEndPhi->SetVisAttributes(endPhiVisAtt);
  
  for(G4int i=0;i<35;i++)
  {
    G4int copyNb = ComputeEndCopyNb(i);
    
    solidEndCasing = new G4IrregBox("EndCasing",(*emcEnd).fPnt[i]);
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCasingMaterial,"EndCasing");
    physiEndCasing = new G4PVPlacement(0,0,logicEndCasing,"EndCasing",logicEndPhi,false,copyNb);
    
    emcEnd->ModifyForCasing((*emcEnd).fPnt[i],i);
    solidEndCrystal = new G4IrregBox("EndCrystal",(*emcEnd).cryPoint);
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,"EndCrystal");
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,"EndCrystal",logicEndCasing,false,copyNb);
    
    //logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    //logicEndCrystal->SetVisAttributes(crystalVisAtt);
    //logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }

  
  // the top area which has 20 mm gap
  // sector 6,14
  solidEndPhi = new G4Cons("EndPhi",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,  
                     (*emcEnd).WorldDz/2,67.5*deg,22.5*deg);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "EndPhi", 0, 0, 0);
  for(G4int i=0;i<2;i++)
  {
    G4RotationMatrix *rotatePhi = new G4RotationMatrix();
    rotatePhi->rotateZ(-i*180.*deg);
    physiEndPhi = new G4PVPlacement(rotatePhi,0,logicEndPhi,"EndPhi",logicEnd,false,i*8+6);
  }
  //if(logicEndPhi)
  //  logicEndPhi->SetVisAttributes(endPhiVisAtt);

  for(G4int i=0;i<35;i++)
  {
    G4int copyNb = ComputeEndCopyNb(i);
    solidEndCasing = new G4IrregBox("EndCasing",(*emcEnd).fPnt1[i]);
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCasingMaterial,"EndCasing");
    physiEndCasing = new G4PVPlacement(0,0,logicEndCasing,"EndCasing",logicEndPhi,false,copyNb);

    emcEnd->ModifyForCasing((*emcEnd).fPnt1[i],i);
    solidEndCrystal = new G4IrregBox("EndCrystal",(*emcEnd).cryPoint);
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,"EndCrystal");
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,"EndCrystal",logicEndCasing,false,copyNb);
    
    //logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    //logicEndCrystal->SetVisAttributes(crystalVisAtt);
    //logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }

  (*emcEnd).ReflectX();

  // sector 7,15
  for(G4int i=0;i<35;i++)
    for (G4int j=0;j<8;j++)
      (*emcEnd).fPnt1[i][j].rotateZ(-90.*deg);
  
  solidEndPhi = new G4Cons("EndPhi",(*emcEnd).WorldRmin1,(*emcEnd).WorldRmax1,(*emcEnd).WorldRmin2,(*emcEnd).WorldRmax2,
                           (*emcEnd).WorldDz/2,0*deg,22.5*deg);
  logicEndPhi = new G4LogicalVolume(solidEndPhi, G4Material::GetMaterial("Air"), "EndPhi", 0, 0, 0);
  for(G4int i=0;i<2;i++)
  {
    G4RotationMatrix *rotatePhi = new G4RotationMatrix();
    rotatePhi->rotateZ(-i*180.*deg-90.*deg);
    physiEndPhi = new G4PVPlacement(rotatePhi,0,logicEndPhi,"EndPhi",logicEnd,false,i*8+7);
  }
  //if(logicEndPhi)
  //  logicEndPhi->SetVisAttributes(endPhiVisAtt);

  for(G4int i=0;i<35;i++)
  {
    G4int copyNb = ComputeEndCopyNb(i);
    solidEndCasing = new G4IrregBox("EndCasing",(*emcEnd).fPnt1[i]);
    logicEndCasing = new G4LogicalVolume(solidEndCasing,fCrystalMaterial,"EndCasing");
    physiEndCasing = new G4PVPlacement(0,0,logicEndCasing,"EndCasing",logicEndPhi,false,copyNb);

    emcEnd->ModifyForCasing((*emcEnd).fPnt1[i],i);
    solidEndCrystal = new G4IrregBox("EndCrystal",(*emcEnd).cryPoint);
    logicEndCrystal = new G4LogicalVolume(solidEndCrystal,fCrystalMaterial,"EndCrystal");
    physiEndCrystal = new G4PVPlacement(0,0,logicEndCrystal,"EndCrystal",logicEndCasing,false,copyNb);
    
    //logicEndCasing->SetVisAttributes(G4VisAttributes::Invisible);
    //logicEndCrystal->SetVisAttributes(crystalVisAtt);
    //logicEndCrystal->SetSensitiveDetector(besEMCSD);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ExtBesEmcConstruction::ConstructSPFrame(G4LogicalVolume* logicEMC, ExtBesEmcGeometry* besEMCGeometry)
{
  //G4VisAttributes* ringVisAtt= new G4VisAttributes(G4Colour(0.5,0.25,0.));
  //ringVisAtt->SetVisibility(false);
  
  solidSupportBar = new G4Tubs("SupportBar",
      (*besEMCGeometry).BSCRmax+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCRmax+(*besEMCGeometry).SPBarThickness+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3+(*besEMCGeometry).EndRingDz,
      0.*deg,
      360.*deg);

  logicSupportBar = new G4LogicalVolume(solidSupportBar,stainlessSteel,"SupportBar");

  physiSupportBar = new G4PVPlacement(0,0,logicSupportBar,"SupportBar",logicEMC,false,0);

  solidSupportBar1 = new G4Tubs("SupportBar1",
      (*besEMCGeometry).BSCRmax,
      (*besEMCGeometry).BSCRmax+(*besEMCGeometry).SPBarThickness1,
      (*besEMCGeometry).BSCDz+(*besEMCGeometry).TaperRingThickness3,
      (*besEMCGeometry).BSCPhiDphi-(*besEMCGeometry).SPBarDphi/2,
      (*besEMCGeometry).SPBarDphi);

  logicSupportBar1 = new G4LogicalVolume(solidSupportBar1,stainlessSteel,"SupportBar1");

  for(G4int i=0;i<(*besEMCGeometry).BSCNbPhi/2;i++)
  {
    G4RotationMatrix *rotateSPBar = new G4RotationMatrix();
    rotateSPBar->rotateZ((*besEMCGeometry).BSCPhiDphi-i*2*(*besEMCGeometry).BSCPhiDphi);
    physiSupportBar1 = new G4PVPlacement(rotateSPBar,0,logicSupportBar1,"SupportBar1",logicEMC,false,0);
  }

  //end ring
  solidEndRing = new G4Tubs("EndRing",
      (*besEMCGeometry).EndRingRmin,
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr/2,
      (*besEMCGeometry).EndRingDz/2,
      0.*deg,
      360.*deg);

  solidGear = new G4Tubs("Gear",
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr/2,
      (*besEMCGeometry).EndRingRmin+(*besEMCGeometry).EndRingDr,
      (*besEMCGeometry).EndRingDz/2,
      0.*deg,
      (*besEMCGeometry).BSCPhiDphi);

  //taper ring
  solidTaperRing1 = new G4Tubs("TaperRing1",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).TaperRingRmin1+(*besEMCGeometry).TaperRingThickness1,
      (*besEMCGeometry).TaperRingInnerLength/2,
      0.*deg,
      360.*deg);

  solidTaperRing2 = new G4Cons("TaperRing2",
      (*besEMCGeometry).TaperRingRmin1,
      (*besEMCGeometry).TaperRingRmin1+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingRmin2,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingDz/2,
      0.*deg,
      360.*deg);
  
  solidTaperRing3 = new G4Cons("TaperRing3",
      (*besEMCGeometry).BSCRmax2,
      (*besEMCGeometry).BSCRmax2+(*besEMCGeometry).TaperRingOuterLength1,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr,
      (*besEMCGeometry).TaperRingRmin2+(*besEMCGeometry).TaperRingDr+(*besEMCGeometry).TaperRingOuterLength,
      (*besEMCGeometry).TaperRingThickness3/2,
      0.*deg,
      360.*deg);

  logicEndRing = new G4LogicalVolume(solidEndRing,stainlessSteel,"EmcEndRing");
  logicGear = new G4LogicalVolume(solidGear,stainlessSteel,"Gear");
  logicTaperRing1 = new G4LogicalVolume(solidTaperRing1,stainlessSteel,"TaperRing1");
  logicTaperRing2 = new G4LogicalVolume(solidTaperRing2,stainlessSteel,"TaperRing2");
  logicTaperRing3 = new G4LogicalVolume(solidTaperRing3,stainlessSteel,"TaperRing3");

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
    
    physiEndRing = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,zEndRing),
        logicEndRing,"EndRing",logicEMC,false,0);
    
    for(G4int j=0;j<(*besEMCGeometry).BSCNbPhi/2;j++)
    {
      G4RotationMatrix *rotateGear = new G4RotationMatrix();
      rotateGear->rotateZ((*besEMCGeometry).BSCPhiDphi/2-j*2*(*besEMCGeometry).BSCPhiDphi);
      physiGear = new G4PVPlacement(rotateGear,G4ThreeVector(0,0,zEndRing),
          logicGear,"Gear",logicEMC,false,0);
    }
    
    physiTaperRing1 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z1),
        logicTaperRing1,"TaperRing1",logicEMC,false,0);
   
    physiTaperRing2 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z2),
        logicTaperRing2,"TaperRing2",logicEMC,false,0);
  
    physiTaperRing3 = new G4PVPlacement(rotateSPRing,G4ThreeVector(0,0,z3),
        logicTaperRing3,"TaperRing3",logicEMC,false,0);
  }
  /*
  logicSupportBar->SetVisAttributes(ringVisAtt);
  logicSupportBar1->SetVisAttributes(ringVisAtt);
  logicEndRing->SetVisAttributes(ringVisAtt);
  logicGear->SetVisAttributes(ringVisAtt);
  logicTaperRing1->SetVisAttributes(ringVisAtt);
  logicTaperRing2->SetVisAttributes(ringVisAtt);
  logicTaperRing3->SetVisAttributes(ringVisAtt);
  */
}

G4int ExtBesEmcConstruction::ComputeEndCopyNb(G4int num)
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


void ExtBesEmcConstruction::PrintEMCParameters()
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

void ExtBesEmcConstruction::SetCrystalMaterial(G4String materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial = G4Material::GetMaterial(materialChoice);
  if (pttoMaterial)
     {fCrystalMaterial = pttoMaterial;
      logicBSCCrystal->SetMaterial(pttoMaterial);
      PrintEMCParameters();
     }
}


void ExtBesEmcConstruction::SetCasingMaterial(G4String materialChoice)
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

void ExtBesEmcConstruction::SetCasingThickness(G4ThreeVector val)
{
  // change Gap thickness and recompute the calorimeter parameters
  (*besEMCGeometry).fTyvekThickness = val('X');
  (*besEMCGeometry).fAlThickness    = val('Y');
  (*besEMCGeometry).fMylarThickness = val('Z');
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCRmin(G4double val)
{
  (*besEMCGeometry).BSCRmin = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCNbPhi(G4int val)
{
  (*besEMCGeometry).BSCNbPhi = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCNbTheta(G4int val)
{
  (*besEMCGeometry).BSCNbTheta = val;
}

void ExtBesEmcConstruction::SetStartIDTheta(G4int val)
{
  startID = val;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCCrystalLength(G4double val)
{
  (*besEMCGeometry).BSCCryLength = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCYFront0(G4double val)
{
  (*besEMCGeometry).BSCYFront0 = val;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCYFront(G4double val)
{
  (*besEMCGeometry).BSCYFront = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCPosition0(G4double val)
{
  (*besEMCGeometry).BSCPosition0 = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetBSCPosition1(G4double val)
{
  (*besEMCGeometry).BSCPosition1 = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ExtBesEmcConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if(magField) delete magField;     //delete the existing magn field

  if(fieldValue!=0.)            // create a new one if non nul
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
void ExtBesEmcConstruction::UpdateGeometry()
{
  ;//G4RunManager::GetRunManager()->DefineWorldVolume(BesDetectorConstruction::Construct());
}
