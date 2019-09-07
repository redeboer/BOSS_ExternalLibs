//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:  Geometry Construction for barrel and endcap Tof
//Author: Dengzy
//Created: Mar, 2004
//Modified: Way of reading txt file in dat/ is changed.
//The material for BC404 and BC408 has been exchanged. The original
//one is opposite. (Liu Yong @ Dec 2008)
//$Id: BesTofConstruction.cc
//

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "BesTofConstruction.hh"
#include "BesTofGeoParameter.hh"
#include "PartProduce.hh"
#include "BesTofSD.hh"
#include "G4Geo/TofG4Geo.h"
#include "G4Geo/MRPCG4Geo.h"
#include "ReadBoostRoot.hh"

#include "G4Material.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Cons.hh"
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"

#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"
#include "G4SDManager.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"



BesTofConstruction::BesTofConstruction()    //Constructor, set variables (G4logical or G4physical volumes) declared in BesTofConstruction.hh
{
    PropertyMgr m_propMgr1;
    m_propMgr1.declareProperty("UserLimits", m_userLimits = 0.03);
    m_propMgr1.declareProperty("IonE", m_ionE = 20);
    IJobOptionsSvc* jobSvc1;
    Gaudi::svcLocator()->service("JobOptionsSvc", jobSvc1);
    jobSvc1->setMyProperties("BesTofConstruction", &m_propMgr1);

    cout<<"BesTofConstruction Property:"<<endl
        <<"  UserLimits= "<<m_userLimits
        <<"  IonE= "<<m_ionE
        <<endl;

    logicalTof  = 0;
    physicalTof = 0;

    logicalBrTof = 0;
    logicalEcTofWest=0;
    logicalEcTofEast=0;

    logicalScinBr1  = 0;
    logicalScinBr2  = 0;
    physicalScinBr1 = 0;
    physicalScinBr2 = 0;

    logicalAlBr1 = 0;
    logicalAlBr2 = 0;
    physicalAlBr1 = 0;
    physicalAlBr2 = 0;

    logicalPVFBr1 = 0;
    logicalPVFBr2 = 0;
    physicalPVFBr1 = 0;
    physicalPVFBr2 = 0;

    logicalBucketBr1= 0;
    logicalBucketBr2= 0;

    physicalBucket1Br1 = 0;
    physicalBucket2Br1 = 0;
    physicalBucket1Br2 = 0;
    physicalBucket2Br2 = 0;

    logicalScinEcWest   = 0;
    logicalScinEcEast = 0;
    physicalScinEcWest = 0;
    physicalScinEcEast = 0;

    logicalAlEcWest = 0;
    logicalAlEcEast = 0;
    physicalAlEcWest = 0;
    physicalAlEcEast = 0;

    logicalPVFEcWest = 0;
    logicalPVFEcEast = 0;
    physicalPVFEcWest = 0;
    physicalPVFEcEast = 0;

    logicalBucketEc = 0;
    physicalBucketEc = 0;

    BC404 = 0;
    BC408 = 0;
    PVF = 0;
    PMTmaterial = 0;

    //MRPC 
    logical_gasLayer=0;
}

BesTofConstruction::~BesTofConstruction()
{}

void BesTofConstruction::Construct(G4LogicalVolume* logicalBes)
{
    DefineMaterial(); //This function just defines my material, used for the TOF

    if ( ReadBoostRoot::GetTof() == 2 )   
    {
        TofG4Geo* aTofG4Geo = new TofG4Geo();
        logicalTof = aTofG4Geo->GetTopVolume();
        if (!logicalTof)
          G4cout<<"BesTofConstruction::Construct(), logicalTof not found"<<G4endl;
        else
        {
            //produce the logical Tof
            physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),
                        logicalTof,"physicalTof",logicalBes,false,0); // Class representing a single volume positioned within and relative to a mother volume.

            //for visual attributes
            logicalScinBr1  = FindLogicalVolume("logicalScinBr1");
            logicalScinBr2  = FindLogicalVolume("logicalScinBr2");

            logicalAlBr1 = FindLogicalVolume("logicalAlBr1");
            logicalAlBr2 = FindLogicalVolume("logicalAlBr2");
            logicalPVFBr1 = FindLogicalVolume("logicalPVFBr1");
            logicalPVFBr2 = FindLogicalVolume("logicalPVFBr2");
            logicalBucketBr1 = FindLogicalVolume("logicalBucketBr1");
            logicalBucketBr2 = FindLogicalVolume("logicalBucketBr2");

            logicalScinEcWest = FindLogicalVolume("logicalScinEcWest");
            logicalScinEcEast = FindLogicalVolume("logicalScinEcEast");
            logicalBucketEc = FindLogicalVolume("logicalBucketEc");
        }
        //m_sxp.Finalize();
        delete aTofG4Geo;
    } //close (ReadBoostRoot::GetTof() == 2)

    else if(ReadBoostRoot::GetTof() == 3 ) //start TOF with MRPC Endcaps
    {
        //This part intialize a function where the Parameter of the TOF are saved!
        BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

        //Get the coordinates of the TOF
        G4double r1=380; //Newly changed from 399 for MRPC to avoid overlap 
        G4double r2=tofPara->GetBucketPosR()+0.5*tofPara->GetBucketDEc()+1;  //=474
        G4double r3=810;
        G4double r4=925;
        G4double a1=1382+tofPara->GetBucketLEc()+1; //1463
        G4double a2=1381; // Changed from 1382 to avoid overlapping with EMC
        //to make the center of tub6 is tofPara.GetzPosEastEc()=1356
        G4double a3=1330;
        G4cout<<"Tof Volume: "<<r1<<" "<<r2<<" "<<r3<<" "<<r4<<" "<<a1<<" "<<a2<<" "<<a3<<G4endl;

        //Tof:  produce a cylinder with inner radius r1 and outer radius r2, z height: 2a1, starting angle 0 untill 360
        G4Tubs* tub1 = new G4Tubs("tub1",r1,r2,a1,0,360);     
        G4Tubs* tub2 = new G4Tubs("tub2",r2-4,r4,a2,0,360);
        G4Tubs* tub3 = new G4Tubs("tub3",0,r3,a3,0,360);
        G4UnionSolid* tub4 = new G4UnionSolid("tub4",tub1,tub2,0,G4ThreeVector(0,0,0) );  //Vereinigt tub 1 und 2 in tub4
        G4SubtractionSolid* solidTof = new G4SubtractionSolid("solidTof",tub4,tub3,0, G4ThreeVector(0,0,0) );  //Subtracts tub4  - tub3
        logicalTof = new G4LogicalVolume(solidTof, G4Material::GetMaterial("Air"),"logicalTof"); //book logical Tof VOlume
        physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTof,"physicalTof",logicalBes,false,0);

        //Logical + Physical volumes for the barrel
        G4Tubs* tub5 = new G4Tubs("tub5",r3,r4,a3,0,360);
        logicalBrTof = new G4LogicalVolume(tub5, G4Material::GetMaterial("Air"),"logicalBrTof");  //book logical Barrel TOF
        G4VPhysicalVolume* physicalBrTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalBrTof,"physicalBrTof",logicalTof,false,0); 


        //Logical + Physical volumes for the endcaps
        G4Tubs* tub6 = new G4Tubs("tub6",r1,r4,(a2-a3)/2,0,360);  //ring: dz/2=2.55cm, xy=52.6 cm
        G4Tubs* tub7 = new G4Tubs("tub7",r1,r2,(a1-a2)/2,0,360);  //ring: dz/2=0.1cm, xy=7.5 cm 
        G4UnionSolid* tub8 = new G4UnionSolid("tub8",tub6,tub7,0,G4ThreeVector(0,0,(a3-a1)/2)); //Unify both tubes,translate z-axes -6.15 cm to the originally one  
        logicalEcTofWest = new G4LogicalVolume(tub8, G4Material::GetMaterial("Air"),"logicalEcTofWest");
        G4VPhysicalVolume* physicalEcTofWest = new G4PVPlacement(0,G4ThreeVector(0,0,tofPara->GetzPosWestEc()),logicalEcTofWest,"physicalEcTofWest",logicalTof,false,0);

        G4UnionSolid* tub9 = new G4UnionSolid("tub9",tub6,tub7,0,G4ThreeVector(0,0,(a1-a3)/2) ); //Unify both tubes,translate z-axes +6.15 cm to the originally one
        logicalEcTofEast = new G4LogicalVolume(tub9, G4Material::GetMaterial("Air"),"logicalEcTofEast");
        G4VPhysicalVolume* physicalEcTofEast = new G4PVPlacement(0,G4ThreeVector(0,0,tofPara->GetzPosEastEc()),logicalEcTofEast,"physicalEcTofEast",logicalTof,false,0);


        //Functions which construct the different parts of the TOF -->See below
        //Construct Tape
        //put these lines before ConstructBr1Tof();
        //so in BesTofSD.cc, no need to change the tofid caculation method
        G4Tubs* tubTape = new G4Tubs("tubTape",866, 866.3, 1150, 0, 360);
        G4LogicalVolume* logicalTape = new G4LogicalVolume(tubTape,G4Material::GetMaterial("tape"),"logicalTape");
        G4VPhysicalVolume* physicalTape = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTape,"physicalTape",logicalBrTof,false,0);
        logicalTape->SetVisAttributes(G4VisAttributes::Invisible);

        ConstructBr1Tof();  
        ConstructBr2Tof();
        ConstructEcTof_mrpc();
    }

    else if (ReadBoostRoot::GetTof() == 4 ) //start TOF with MRPC Endcaps GDML Construction
    {
        MRPCG4Geo* aTofG4Geo = new MRPCG4Geo();
        logicalTof = aTofG4Geo->GetTopVolume();
        if (!logicalTof)
          G4cout<<"BesTofConstruction::Construct(), logicalTof not found"<<G4endl;
        else
        {
            physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTof,"physicalTof",logicalBes,false,0);

            logicalScinBr1  = FindLogicalVolume("logicalScinBr1");
            logicalScinBr2  = FindLogicalVolume("logicalScinBr2");

            logicalAlBr1 = FindLogicalVolume("logicalAlBr1");
            logicalAlBr2 = FindLogicalVolume("logicalAlBr2");
            logicalPVFBr1 = FindLogicalVolume("logicalPVFBr1");
            logicalPVFBr2 = FindLogicalVolume("logicalPVFBr2");
            logicalBucketBr1 = FindLogicalVolume("logicalBucketBr1");
            logicalBucketBr2 = FindLogicalVolume("logicalBucketBr2");

            logical_gasLayer=FindLogicalVolume("logical_gasLayer");
        }  
        delete aTofG4Geo;
    } //close else if(==4)

    else
    {
        //-----------------logicalTof added here
        BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();  

        //Get the coordinates of the TOF
        G4double r1=tofPara->GetEcR1()-1;  //=399 
        G4double r2=tofPara->GetBucketPosR()+0.5*tofPara->GetBucketDEc()+1;  //=474
        G4double r3=810;;
        G4double r4=925;
        G4double a1=1382+tofPara->GetBucketLEc()+1; //1463
        G4double a2=1381; // Changed from 1382 to avoid overlapping with EMC
        //to make the center of tub6 is tofPara.GetzPosEastEc()=1356
        G4double a3=1330;
        G4cout<<"Tof Volume: "<<r1<<" "<<r2<<" "<<r3<<" "<<r4<<" "<<a1<<" "<<a2<<" "<<a3<<G4endl;

        G4Tubs* tub1 = new G4Tubs("tub1",r1,r2,a1,0,360);     //Just produce a cylinder with inner radius r1 and outer radius r2, z height: 2a1, starting angle 0 untill 360
        G4Tubs* tub2 = new G4Tubs("tub2",r2-4,r4,a2,0,360);
        G4Tubs* tub3 = new G4Tubs("tub3",0,r3,a3,0,360);
        G4UnionSolid* tub4 = new G4UnionSolid("tub4",tub1,tub2,0,G4ThreeVector(0,0,0) );  //Vereinigt tub 1 und 2 in tub4
        G4SubtractionSolid* solidTof = new G4SubtractionSolid("solidTof",tub4,tub3,0, G4ThreeVector(0,0,0) );  //Subtracts tub4  - tub3
        logicalTof = new G4LogicalVolume(solidTof, G4Material::GetMaterial("Air"),"logicalTof"); //book logical Tof VOlume
        physicalTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTof,"physicalTof",logicalBes,false,0);

        G4Tubs* tub5 = new G4Tubs("tub5",r3,r4,a3,0,360);
        logicalBrTof = new G4LogicalVolume(tub5, G4Material::GetMaterial("Air"),"logicalBrTof");  //book logical Barrel TOF
        G4VPhysicalVolume* physicalBrTof = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalBrTof,"physicalBrTof",logicalTof,false,0); 

        //Logical + Physical volumes for the endcaps
        G4Tubs* tub6 = new G4Tubs("tub6",r1,r4,(a2-a3)/2,0,360);//r1=39.9cm, r4=92.5cm, zlength=2.55 cm
        G4Tubs* tub7 = new G4Tubs("tub7",r1,r2,(a1-a2)/2,0,360);//r1=39.9cm, r2=47.4cm, zlength=4.05 cm 
        G4UnionSolid* tub8 = new G4UnionSolid("tub8",tub6,tub7,0,G4ThreeVector(0,0,(a3-a1)/2) );// -6.65cm
        logicalEcTofWest = new G4LogicalVolume(tub8, G4Material::GetMaterial("Air"),"logicalEcTofWest");
        G4VPhysicalVolume* physicalEcTofWest = new G4PVPlacement(0,G4ThreeVector(0,0,tofPara->GetzPosWestEc()),logicalEcTofWest,"physicalEcTofWest",logicalTof,false,0);

        G4UnionSolid* tub9 = new G4UnionSolid("tub9",tub6,tub7,0,G4ThreeVector(0,0,(a1-a3)/2) );
        logicalEcTofEast = new G4LogicalVolume(tub9, G4Material::GetMaterial("Air"),"logicalEcTofEast");
        G4VPhysicalVolume* physicalEcTofEast = new G4PVPlacement(0,G4ThreeVector(0,0,tofPara->GetzPosEastEc()),logicalEcTofEast,"physicalEcTofEast",logicalTof,false,0);


        //Construct Tape
        //put these lines before ConstructBr1Tof();
        //so in BesTofSD.cc, no need to change the tofid caculation method
        G4Tubs* tubTape = new G4Tubs("tubTape",866, 866.3, 1150, 0, 360);
        G4LogicalVolume* logicalTape = new G4LogicalVolume(tubTape,G4Material::GetMaterial("tape"),"logicalTape");
        G4VPhysicalVolume* physicalTape = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalTape,"physicalTape",logicalBrTof,false,0);
        logicalTape->SetVisAttributes(G4VisAttributes::Invisible);

        //Functions which construct the different parts of the TOF -->See below
        ConstructBr1Tof();  
        ConstructBr2Tof();
        ConstructEcTof();
    }


    //sensitive detector
    G4SDManager* SDman = G4SDManager::GetSDMpointer();
    BesTofSD* tofSD = new BesTofSD("BesTofSD");
    SDman->AddNewDetector(tofSD);

    if (logicalScinBr1)     logicalScinBr1->SetSensitiveDetector(tofSD);
    if (logicalScinBr2)     logicalScinBr2->SetSensitiveDetector(tofSD);
    if (logicalScinEcWest)  logicalScinEcWest->SetSensitiveDetector(tofSD);
    if (logicalScinEcEast)  logicalScinEcEast->SetSensitiveDetector(tofSD);
    if(logical_gasLayer)   
    {
        logical_gasLayer->SetSensitiveDetector(tofSD); 
        G4cout<<"!!! Find logical_gasLayer !!!"<<G4endl;

        //For limits smaller than 0.02 one has to use the PAI-Model for ionisation!
        logical_gasLayer->SetUserLimits (new G4UserLimits(m_userLimits*mm));
        //logical_gasLayer->SetUserLimits (new G4UserLimits(0.03*mm));
    }


    G4VisAttributes* visBrTof = new G4VisAttributes(G4Colour(1.,0.,0.));
    G4VisAttributes* visEcTof = new G4VisAttributes(G4Colour(0.,1.,0.));

    if (logicalTof)
      logicalTof->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalBrTof)
      logicalBrTof->SetVisAttributes(G4VisAttributes::Invisible);
    //logicalBrTof->SetVisAttributes(visBrTof);
    if (logicalEcTofWest)
      logicalEcTofWest->SetVisAttributes(G4VisAttributes::Invisible);
    //logicalEcTofWest->SetVisAttributes(visEcTof);
    if (logicalEcTofEast)
      logicalEcTofEast->SetVisAttributes(G4VisAttributes::Invisible);
    //logicalEcTofEast->SetVisAttributes(visEcTof);


    if (logicalScinBr1)
      logicalScinBr1->SetVisAttributes(G4VisAttributes::Invisible); 
    if (logicalScinBr2)
      logicalScinBr2->SetVisAttributes(G4VisAttributes::Invisible); 

    if (logicalAlBr1)
      logicalAlBr1->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalAlBr2)
      logicalAlBr2->SetVisAttributes(G4VisAttributes::Invisible);

    if (logicalPVFBr1)
      logicalPVFBr1->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalPVFBr2)
      logicalPVFBr2->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes* visAttBrBuck = new G4VisAttributes(G4Colour(1.,1.,0.));
    if (logicalBucketBr1)
      logicalBucketBr1->SetVisAttributes(visAttBrBuck);
    //logicalBucketBr1->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalBucketBr2)
      logicalBucketBr2->SetVisAttributes(visAttBrBuck);
    //logicalBucketBr2->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes* visAttEcTof = new G4VisAttributes(G4Colour(0.,1.,1.));
    if (logicalScinEcWest)
      logicalScinEcWest->SetVisAttributes(visAttEcTof);
    //logicalScinEc->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalScinEcEast)
      logicalScinEcEast->SetVisAttributes(visAttEcTof);

    G4VisAttributes* visAttEcBuck = new G4VisAttributes(G4Colour(1.,1.,0.));
    if (logicalBucketEc)
      logicalBucketEc->SetVisAttributes(visAttEcBuck);
    //logicalBucketEc->SetVisAttributes(G4VisAttributes::Invisible);

    if (logicalAlEcWest)
      logicalAlEcWest->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalAlEcEast)
      logicalAlEcEast->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalPVFEcWest)
      logicalPVFEcWest->SetVisAttributes(G4VisAttributes::Invisible);
    if (logicalPVFEcEast)
      logicalPVFEcEast->SetVisAttributes(G4VisAttributes::Invisible);
}

void BesTofConstruction::ConstructBr1Tof()
{
    BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

    //Get barrel tof layer1 geometry data
    G4int nScinBr=tofPara->GetnScinBr();           //number of barrel scintillators
    G4double br1L=tofPara->GetBr1L();
    G4double br1TrapW1=tofPara->GetBr1TrapW1();
    G4double br1TrapW2=tofPara->GetBr1TrapW2();
    G4double br1TrapH=tofPara->GetBr1TrapH();
    G4double br1R1=tofPara->GetBr1R1();
    G4double AlThickness=tofPara->GetAlThickness();
    G4double PVFThickness=tofPara->GetPVFThickness();
    //barrel PMT bucket geometry data
    G4double bucketDBr=tofPara->GetBucketDBr();    //diameter of barrel PMT bucket
    G4double bucketLBr=tofPara->GetBucketLBr();    //length of barrel PMT bucket

    //computer from the original data
    G4double angleBr=360./nScinBr*deg;
    G4double scinTrapdx1,scinTrapdx2,scinTrapdx3,scinTrapdx4;
    scinTrapdx1=scinTrapdx3=br1TrapW1/2.;
    scinTrapdx2=scinTrapdx4=br1TrapW2/2.;
    G4double scinTrapdy1,scinTrapdy2;
    scinTrapdy1=scinTrapdy2=0.5*br1TrapH;
    G4double scinTrapdz=br1L/2.;
    G4double scinPosR=br1R1+scinTrapdy1;
    G4double theta = atan((br1TrapW2/2.-br1TrapW1/2.)/br1TrapH);
    G4double delta13 = AlThickness*(1/cos(theta)-tan(theta));
    G4double delta24 = AlThickness*(1/cos(theta)+tan(theta));
    G4double delta13P = (PVFThickness+AlThickness)*(1/cos(theta)-tan(theta));
    G4double delta24P = (PVFThickness+AlThickness)*(1/cos(theta)+tan(theta));

    //construct barrel tof scintillator
    G4Trap* solidScinBr1=new G4Trap("solidScinBr1",scinTrapdz,0*deg,0*deg,
                scinTrapdy1,scinTrapdx1,scinTrapdx2,0*deg,
                scinTrapdy2,scinTrapdx3,scinTrapdx4,0*deg); 
    logicalScinBr1 = new G4LogicalVolume(solidScinBr1,BC408,"logicalScinBr1"); 

    //construct barrel Al film and PVF film
    G4Trap* solidAlBr1 = new  G4Trap("solidAlBr1", scinTrapdz+0.001,0*deg,0*deg,
                scinTrapdy1+AlThickness, scinTrapdx1+delta13, scinTrapdx2+delta24 ,0*deg,
                scinTrapdy2+AlThickness, scinTrapdx3+delta13, scinTrapdx4+delta24 ,0*deg);

    logicalAlBr1 = new G4LogicalVolume(solidAlBr1,G4Material::GetMaterial("Aluminium"),"logicalAlBr1");
    G4Trap* solidPVFBr1 = new G4Trap("solidPVFBr1",scinTrapdz+0.002,0*deg,0*deg,
                scinTrapdy1+AlThickness+PVFThickness,scinTrapdx1+delta13P, scinTrapdx2+delta24P,0*deg,
                scinTrapdy2+AlThickness+PVFThickness,scinTrapdx3+delta13P, scinTrapdx4+delta24P,0*deg);

    logicalPVFBr1 = new  G4LogicalVolume(solidPVFBr1,PVF,"logicalPVFBr1");
    //put daughter in mother logical volume
    physicalAlBr1 = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalAlBr1, "physicalAlBr1",logicalPVFBr1, false,0); 
    physicalScinBr1 = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalScinBr1, "physicalScinBr1",logicalAlBr1, false, 0); 

    //construct barrel PMT  bucket  (bucket=Behaelter)
    G4Tubs* solidBucketBr1= new G4Tubs("solidBucketBr1",0,bucketDBr/2.,
                bucketLBr/2., 0, 360.*deg);
    logicalBucketBr1=new G4LogicalVolume(solidBucketBr1,PMTmaterial,"logicalBucketBr1");


    //Rotate the crystalls:
    G4RotationMatrix trapRot;
    trapRot.rotateZ(0.5*angleBr-90.*deg);
    //                       /|                                    y
    //-------               / |                                   |
    //\     /              |  | rotate->(0.5*angleBr-90.*deg):    |
    // \   /       ->>>>   |  |                                    -----x
    //  ---                 \ |
    //                       \|
    G4ThreeVector pos(0,0,0);
    G4double x,y;
    for (int i=0;i<nScinBr;i++)
    {
        ostringstream osnamePVFBr1;
        osnamePVFBr1 << "physicalPVFBr1_"<<i;
        ostringstream osnameBucket1Br1;
        osnameBucket1Br1 << "physicalBucket1Br1_"<<i;
        ostringstream osnameBucket2Br1;
        osnameBucket2Br1 << "physicalBucket2Br1_"<<i;

        //scintillator and casing
        x=scinPosR*cos((i+0.5)*angleBr); //+0.5: start from phi=0
        y=scinPosR*sin((i+0.5)*angleBr);
        pos.setX(x);
        pos.setY(y);
        pos.setZ(0);

        //to make code consistent with gdml
        double a, b, c;
        getXYZ(trapRot, a, b ,c);   //Function is defined below
        G4RotationMatrix trapRotTemp;
        trapRotTemp.rotateZ(c);
        G4Transform3D transform(trapRotTemp,pos);

        //G4PVPlacement: Class representing a single volume positioned within and relative to a mother volume.
        //G4Transform3D transform(trapRot,pos);
        physicalPVFBr1 = new G4PVPlacement(transform,logicalPVFBr1,osnamePVFBr1.str(), logicalBrTof,false,i);
        //bucket --> Two PMT, one on each side
        pos.setZ(-(bucketLBr+br1L)/2.-0.01);
        physicalBucket1Br1 = new G4PVPlacement(0,pos,logicalBucketBr1,osnameBucket1Br1.str(),logicalBrTof,false,0);
        pos.setZ((bucketLBr+br1L)/2.+0.01);
        physicalBucket2Br1 = new G4PVPlacement(0,pos,logicalBucketBr1,osnameBucket2Br1.str(),logicalBrTof,false,0);

        //rotate for next scintillator
        trapRot.rotateZ(angleBr);  
    }
}

void BesTofConstruction::ConstructBr2Tof()
{
    BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

    //barrel tof layer2 geometry data
    G4int nScinBr=tofPara->GetnScinBr();           //number of barrel scintillators
    G4double br2L=tofPara->GetBr2L();
    G4double br2TrapW1=tofPara->GetBr2TrapW1();
    G4double br2TrapW2=tofPara->GetBr2TrapW2();
    G4double br2TrapH=tofPara->GetBr2TrapH();
    G4double br2R1=tofPara->GetBr2R1();
    G4double AlThickness=tofPara->GetAlThickness();
    G4double PVFThickness=tofPara->GetPVFThickness();
    //barrel PMT bucket geometry data
    G4double bucketDBr=tofPara->GetBucketDBr();    //diameter of barrel PMT bucket
    G4double bucketLBr=tofPara->GetBucketLBr();    //length of barrel PMT bucket

    //computer from the original data
    G4double angleBr=360./nScinBr*deg;
    G4double scinTrapdx1,scinTrapdx2,scinTrapdx3,scinTrapdx4;
    scinTrapdx1=scinTrapdx3=br2TrapW1/2.;
    scinTrapdx2=scinTrapdx4=br2TrapW2/2.;
    G4double scinTrapdy1,scinTrapdy2;
    scinTrapdy1=scinTrapdy2=0.5*br2TrapH;
    G4double scinTrapdz=br2L/2.;
    G4double scinPosR=br2R1+scinTrapdy1;
    G4double theta = atan((br2TrapW2/2.-br2TrapW1/2.)/br2TrapH);
    G4double delta13 = AlThickness*(1/cos(theta)-tan(theta));
    G4double delta24 = AlThickness*(1/cos(theta)+tan(theta));
    G4double delta13P = (PVFThickness+AlThickness)*(1/cos(theta)-tan(theta));
    G4double delta24P = (PVFThickness+AlThickness)*(1/cos(theta)+tan(theta));

    //construct barrel tof scintillator
    G4Trap* solidScinBr2=new G4Trap("solidScinBr2",scinTrapdz,0*deg,0*deg,
                scinTrapdy1,scinTrapdx1,scinTrapdx2,0*deg,
                scinTrapdy2,scinTrapdx3,scinTrapdx4,0*deg);
    logicalScinBr2 = new G4LogicalVolume(solidScinBr2,BC408,"logicalScinBr2");

    //construct barrel Al film and PVF film
    G4Trap* solidAlBr2 = new  G4Trap("solidAlBr2", scinTrapdz+0.001,0*deg,0*deg,
                scinTrapdy1+AlThickness, scinTrapdx1+delta13, scinTrapdx2+delta24 ,0*deg,
                scinTrapdy2+AlThickness, scinTrapdx3+delta13, scinTrapdx4+delta24 ,0*deg);

    logicalAlBr2 = new G4LogicalVolume(solidAlBr2,G4Material::GetMaterial("Aluminium"),"logicalAlBr2");
    G4Trap* solidPVFBr2 = new G4Trap("solidPVFBr2",scinTrapdz+0.002,0*deg,0*deg,
                scinTrapdy1+AlThickness+PVFThickness,scinTrapdx1+delta13P, scinTrapdx2+delta24P,0*deg,
                scinTrapdy2+AlThickness+PVFThickness,scinTrapdx3+delta13P, scinTrapdx4+delta24P,0*deg);

    logicalPVFBr2 = new  G4LogicalVolume(solidPVFBr2,PVF,"logicalPVFBr2");
    //put daughter in mother logical volume
    physicalAlBr2 = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalAlBr2, "physicalAlBr2",logicalPVFBr2, false,0);
    physicalScinBr2 = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalScinBr2, "physicalScinBr2",logicalAlBr2, false, 0);

    //construct barrel PMT  bucket
    G4Tubs* solidBucketBr2= new G4Tubs("solidBucketBr2",0,bucketDBr/2.,
                bucketLBr/2., 0, 360.*deg);
    logicalBucketBr2=new G4LogicalVolume(solidBucketBr2,PMTmaterial,"logicalBucketBr2");

    G4RotationMatrix trapRot;
    trapRot.rotateZ(-90.*deg);
    //                       /|                                    y
    //-------               / |                                   |
    //\     /              |  | rotate->(-90.*deg):    |
    // \   /       ->>>>   |  |                                    -----x
    //  ---                 \ |
    //                       \|
    G4ThreeVector pos(0,0,0);
    G4double x,y;
    for (int i=0;i<nScinBr;i++)
    {
        ostringstream osnamePVFBr2;
        osnamePVFBr2 << "physicalPVFBr2_"<<i;
        ostringstream osnameBucket1Br2;
        osnameBucket1Br2 << "physicalBucket1Br2_"<<i;
        ostringstream osnameBucket2Br2;
        osnameBucket2Br2 << "physicalBucket2Br2_"<<i;

        //scintillator and casing
        x=scinPosR*cos(i*angleBr); //+0.5: start from phi=0
        y=scinPosR*sin(i*angleBr);
        pos.setX(x);
        pos.setY(y);
        pos.setZ(0);
        //to make code consistent with gdml
        double a, b, c;
        getXYZ(trapRot, a, b ,c);
        G4RotationMatrix trapRotTemp;
        trapRotTemp.rotateZ(c);
        G4Transform3D transform(trapRotTemp,pos);
        //G4Transform3D transform(trapRot,pos);
        physicalPVFBr2 = new G4PVPlacement(transform,logicalPVFBr2,osnamePVFBr2.str(), logicalBrTof,false,i);
        //bucket
        pos.setZ(-(bucketLBr+br2L)/2.-0.01);
        physicalBucket1Br2 = new G4PVPlacement(0,pos,logicalBucketBr2,osnameBucket1Br2.str(),logicalBrTof,false,0);
        pos.setZ((bucketLBr+br2L)/2.+0.01);
        physicalBucket2Br2 = new G4PVPlacement(0,pos,logicalBucketBr2,osnameBucket2Br2.str(),logicalBrTof,false,0);

        //rotate for next scintillator
        trapRot.rotateZ(angleBr);
    }

}

void BesTofConstruction::ConstructEcTof()
{
    BesTofGeoParameter* tofPara = BesTofGeoParameter::GetInstance();

    //Get all interesting parameters for the TOF construction
    //endcap geometry data
    G4int nScinEc = tofPara->GetnScinEc();  //number of endcap scintillators
    G4double ecL = tofPara->GetEcL();
    G4double ecTrapW1 = tofPara->GetEcTrapW1();
    G4double ecTrapW2 = tofPara->GetEcTrapW2();
    G4double ecTrapH = tofPara->GetEcTrapH();
    G4double ecTrapH1 = tofPara->GetEcTrapH1();
    G4double zPosEastEc = tofPara->GetzPosEastEc();  //z position of east endcap
    G4double zPosWestEc = tofPara->GetzPosWestEc();  //z position of west endcap
    G4double ecR1 = tofPara->GetEcR1();
    //G4double ecR2 = tofPara.GetEcR2();

    //endcap PMT bucket geometry data
    G4double bucketDEc = tofPara->GetBucketDEc(); //diameter of endcap PMT bucket
    G4double bucketLEc = tofPara->GetBucketLEc(); //length of endcap PMT bucket
    G4double bucketPosR = tofPara->GetBucketPosR(); //R  of bucket center

    G4double AlThickness=tofPara->GetAlThickness();
    G4double PVFThickness=tofPara->GetPVFThickness();

    G4double angleEc=360./nScinEc*deg;
    G4double ecTrapW3 = ecTrapW1 + (ecTrapW2-ecTrapW1)*ecTrapH1/ecTrapH;
    G4double ecTrapH2 = ecTrapH-ecTrapH1;
    G4double pdz = ecL/2;
    G4double ptheta = atan(ecTrapH1/(2*ecL));
    G4double pdy1 = ecTrapH2/2;
    G4double pdx1 = ecTrapW3/2;
    G4double pdx2 = ecTrapW2/2;
    G4double pdy2 = ecTrapH/2;
    G4double pdx3 = ecTrapW1/2;
    G4double pdx4 = ecTrapW2/2;

    //because of removing a heighth of H1,
    //plus initial center position ecR=ecR1+ecTrapH/2 with ecTrapH1/4
    G4double ecR = ecR1+ecTrapH/2+ecTrapH1/4;

    //construct endcap scintillator
    G4Trap* solidScinEc=new G4Trap("solidScinEc",pdz,ptheta, 270*deg,
                pdy1,pdx1,pdx2,0*deg,pdy2,pdx3,pdx4,0*deg);

    logicalScinEcWest=new G4LogicalVolume(solidScinEc,BC404,"logicalScinEcWest");
    logicalScinEcEast=new G4LogicalVolume(solidScinEc,BC404,"logicalScinEcEast");

    //construct endcap PMT bucket
    G4Tubs* solidBucketEc = new G4Tubs("solidBucketEc", 0, bucketDEc/2.,
                bucketLEc/2., 0, 360.*deg);
    logicalBucketEc = new G4LogicalVolume(solidBucketEc, PMTmaterial, "logicalBucketEc");

    //construct Al and PVF film
    G4double pthetaAl = atan(ecTrapH1/(2*(ecL+AlThickness*2)));
    G4double theta1 = atan((ecTrapW2/2.-ecTrapW3/2.)/ecTrapH2);
    G4double theta2 = atan((ecTrapW2/2.-ecTrapW1/2.)/ecTrapH);

    G4double delta1 = AlThickness*(1/cos(theta1)-tan(theta1));
    G4double delta2 = AlThickness*(1/cos(theta1)+tan(theta1));
    G4double delta3 = AlThickness*(1/cos(theta2)-tan(theta2));
    G4double delta4 = AlThickness*(1/cos(theta2)+tan(theta2));

    G4double thick = AlThickness+PVFThickness;
    G4double pthetaPVF = atan(ecTrapH1/(2*(ecL+thick*2)));
    G4double delta1P = thick*(1/cos(theta1)-tan(theta1));
    G4double delta2P = thick*(1/cos(theta1)+tan(theta1));
    G4double delta3P = thick*(1/cos(theta2)-tan(theta2));
    G4double delta4P = thick*(1/cos(theta2)+tan(theta2));

    G4Trap* solidAlEc= new G4Trap("solidAlEc",pdz+AlThickness,pthetaAl,270*deg,pdy1+AlThickness,pdx1+delta1,pdx2+delta2,0*deg,pdy2+AlThickness,pdx3+delta3,pdx4+delta4,0*deg);

    logicalAlEcWest = new G4LogicalVolume(solidAlEc,G4Material::GetMaterial("Aluminium"),"logicalAlEcWest");
    logicalAlEcEast = new G4LogicalVolume(solidAlEc,G4Material::GetMaterial("Aluminium"),"logicalAlEcEast");

    G4Trap* solidPVFEc = new G4Trap("solidPVFEc",pdz+thick,pthetaPVF,270*deg,
                pdy1+thick,pdx1+delta1P,pdx2+delta2P,0*deg,
                pdy2+thick,pdx3+delta3P,pdx4+delta4P,0*deg);

    logicalPVFEcWest = new G4LogicalVolume(solidPVFEc,PVF,"logicalPVFEcWest");
    logicalPVFEcEast = new G4LogicalVolume(solidPVFEc,PVF,"logicalPVFEcEast");

    //put scintilator in Al, then put Al in PVF
    physicalAlEcWest = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalAlEcWest, "physicalAlEcWest",logicalPVFEcWest, false,0);
    physicalScinEcWest = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalScinEcWest, "physicalScinEcWest",logicalAlEcWest, false, 0);


    //put scintilator in Al, then put Al in PVF
    physicalAlEcEast = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalAlEcEast, "physicalAlEcEast",logicalPVFEcEast, false,0);
    physicalScinEcEast = new G4PVPlacement(0,G4ThreeVector(0,0,0),logicalScinEcEast, "physicalScinEcEast",logicalAlEcEast, false, 0);

    //zPosWestEc<0, zPosEastEc>0, direction of axis z: west--->east
    //construct west endcap first
    //when no rotation, the first tub is start from phi=90
    //after rotation, the first tub is start from 0, rotate angle = 0.5*angleEc
    G4ThreeVector posW(0,0,0);
    G4ThreeVector posE(0,0,0);
    G4RotationMatrix trapRot;
    trapRot.rotateY(180.*deg);
    trapRot.rotateZ(0.5*angleEc-90*deg);
    for (int i=0;i<nScinEc;i++)
    {
        ostringstream osnamePVFEc;
        osnamePVFEc << "physicalPVFEcWest_"<<i;
        ostringstream osnameBucketEc;
        osnameBucketEc << "physicalBucketEcWest_"<<i;

        posW.setX( ecR*cos((0.5+i)*angleEc) );
        posW.setY( ecR*sin((0.5+i)*angleEc) );
        //posW.setZ( zPosWestEc );
        posW.setZ(0);

        //to make code consistent with gdml
        double a, b, c;
        getXYZ(trapRot, a, b ,c);
        G4RotationMatrix trapRotTemp;
        trapRotTemp.rotateX(a);
        trapRotTemp.rotateY(b);
        trapRotTemp.rotateZ(c);
        G4Transform3D transform(trapRotTemp,posW);
        //G4Transform3D transform(trapRot,posW);
        physicalPVFEcWest = new G4PVPlacement(transform,logicalPVFEcWest,osnamePVFEc.str(), logicalEcTofWest,false,i);
        posW.setX( bucketPosR*cos((0.5+i)*angleEc) );
        posW.setY( bucketPosR*sin((0.5+i)*angleEc) );
        //posW.setZ( zPosWestEc-ecL/2-thick-bucketLEc/2-0.01 );
        posW.setZ(-ecL/2-thick-bucketLEc/2-0.01 );
        physicalBucketEc = new  G4PVPlacement(0,posW,logicalBucketEc,osnameBucketEc.str(),logicalEcTofWest,false,0);
        trapRot.rotateZ(angleEc);  //pay attention: not i*angleEc
    }
    //to make east scintillator start from phi=0
    //        _                               _
    //    _ -                             _ -
    //  - - - -  ====>  - - - -  ====>  - - - -
    //                    ~ - _
    //
    trapRot.rotateZ(-angleEc);
    trapRot.rotateX(180.*deg);  //make east bucket point to the center
    for (int i=0;i<nScinEc;i++)
    {
        ostringstream osnamePVFEc;
        osnamePVFEc << "physicalPVFEcEast_"<<i;
        ostringstream osnameBucketEc;
        osnameBucketEc << "physicalBucketEcEast_"<<i;

        posE.setX( ecR*cos((0.5+i)*angleEc) );
        posE.setY( ecR*sin((0.5+i)*angleEc) );
        //posE.setZ( zPosEastEc );
        posE.setZ(0);

        //to make code consistent with gdml
        double a, b, c;
        getXYZ(trapRot, a, b ,c);
        G4RotationMatrix trapRotTemp;
        trapRotTemp.rotateX(a);
        trapRotTemp.rotateY(b);
        trapRotTemp.rotateZ(c);
        G4Transform3D transform(trapRotTemp,posE);
        //G4Transform3D transform(trapRot,posE);
        physicalPVFEcEast = new G4PVPlacement(transform,logicalPVFEcEast,osnamePVFEc.str(), logicalEcTofEast,false,i);
        posE.setX( bucketPosR*cos((0.5+i)*angleEc) );
        posE.setY( bucketPosR*sin((0.5+i)*angleEc) );
        //posE.setZ( zPosEastEc+ecL/2+thick+bucketLEc/2+0.01 );
        posE.setZ(ecL/2+thick+bucketLEc/2+0.01 );
        physicalBucketEc = new  G4PVPlacement(0,posE,logicalBucketEc,osnameBucketEc.str(),logicalEcTofEast,false,0);
        trapRot.rotateZ(angleEc);  //pay attention: not i*angleEc
    }
}


//This function construct the TOF_Endcap_MRPC
void BesTofConstruction::ConstructEcTof_mrpc()
{
    //I construct my detector in the Geant4 coordinate system. It will be rotated into the BES coordinatesystem later!
    //                  |z              ___________
    //                  |  /y           \         /
    //                  | /              \       /
    //                  |/______x         \_____/         


    G4double smallL = 0.01*mm;
    G4RotationMatrix rot_dummy(0*deg,0*deg,0*deg);
    IniParam_mrpc();
    PartProduce* partProduce = new PartProduce();
    partProduce->IniParam();
    bool checkOverlap = false;

    //Get the parts and place them in the container
    //container
    std::ostringstream name;
    //Create 4 containers and gasContainers
    //0: top module, closer to IP, longer bottom cover;
    //1: same to [0], with gas sensitive detector overturned;
    //2: bottom module, next to EMC, shorter bottom cover;
    //3: same to [2], with gas sensitive detector overturned
    G4LogicalVolume* logical_container[4];
    G4LogicalVolume* logical_gasContainer[4];
    for(int kk=0; kk<4; kk++)
    {
        name.str("");
        name<<"logical_container_m"<<kk;
        logical_container[kk] = partProduce->lg_container((int)kk/2, name.str());

        name.str("");
        name<<"logical_gasContainer_m"<<kk;
        logical_gasContainer[kk] = partProduce->lg_gasContainer(name.str());
    }


    //The parts that compose the container
    G4LogicalVolume* logical_containerFrame = partProduce->lg_containerFrame("logical_containerFrame");
    G4LogicalVolume* logical_upCover = partProduce->lg_upCover("logical_upCover");
    G4LogicalVolume* logical_lowCover1 = partProduce->lg_lowCover1("logical_lowCover1");
    G4LogicalVolume* logical_lowCover2 = partProduce->lg_lowCover2("logical_lowCover2");
    G4LogicalVolume* logical_upFEE = partProduce->lg_upFEE("logical_upFEE");
    G4LogicalVolume* logical_sideFEE = partProduce->lg_sideFEE("logical_sideFEE");
    G4LogicalVolume* logical_castingDie = partProduce->lg_castingDie("logical_castingDie");
    G4LogicalVolume* logical_bareChamber = partProduce->lg_bareChamber("logical_bareChamber");
    G4LogicalVolume* logical_bracket = partProduce->lg_bracket("logical_bracket");
    G4LogicalVolume* logical_sideStopBlock = partProduce->lg_sideStopBlock("logical_sideStopBlock");
    G4LogicalVolume* logical_upStopBlock = partProduce->lg_upStopBlock("logical_upStopBlock");


    //Place the parts in the container
    for(int kk=0; kk<4; kk++)
    {
        //container frame
        name.str("");
        name<<"physical_containerFrame_m"<<kk;
        new G4PVPlacement(0, G4ThreeVector(0,0,0), logical_containerFrame, name.str(), logical_container[kk], false, 0, checkOverlap);

        //top cover
        G4ThreeVector translation_upCover(0, (containerOut_y+upCover_y)/2+smallL, (upCover_z-containerOut_z)/2-upCover_posz);
        name.str("");
        name<<"physical_upCover_m"<<kk;
        new G4PVPlacement(0, translation_upCover, logical_upCover, name.str(), logical_container[kk], false, 0, checkOverlap);

        //bottom cover
        name.str("");
        name<<"physical_lowCover_m"<<kk;
        if(kk<2)
        {
            G4ThreeVector translation_lowCover(0, -1*(containerOut_y+upCover_y)/2, (lowCover1_z-containerOut_z)/2-lowCover1_posz);
            new G4PVPlacement(0, translation_lowCover, logical_lowCover1, name.str(), logical_container[kk], false, 0, checkOverlap);
        }
        else
        {
            G4ThreeVector translation_lowCover(0, -1*(containerOut_y+upCover_y)/2, (lowCover2_z-containerOut_z)/2-lowCover2_posz);
            new G4PVPlacement(0, translation_lowCover, logical_lowCover2, name.str(), logical_container[kk], false, 0, checkOverlap);
        }

        //FEE on the top cover
        G4ThreeVector translation_upFEE(0, containerOut_y/2+upCover_y+upFEE_y/2+smallL, upCover_z-upFEE_posz-upFEE_z/2-upCover_posz-containerOut_z/2);
        name.str("");
        name<<"physical_upFEE_m"<<kk;
        new G4PVPlacement(0, translation_upFEE, logical_upFEE, name.str(), logical_container[kk], false, 0, checkOverlap);

        //FEE at the side
        double angle_containerFrameSide = atan((containerOut_xup-containerOut_xdown)/2/containerOut_z) *rad;
        double l_containerFrameSide = containerOut_z/cos(angle_containerFrameSide);
        double x_sideFEE = (l_containerFrameSide/2-sideFEE_posl)*sin(angle_containerFrameSide)+(containerOut_xup+containerOut_xdown)/4+sideFEE_x/2*cos(angle_containerFrameSide);
        double z_sideFEE = (l_containerFrameSide/2-sideFEE_posl)*cos(angle_containerFrameSide)-sideFEE_x/2*sin(angle_containerFrameSide);
        G4ThreeVector translation_sideFEE_left(-x_sideFEE, 0, z_sideFEE);
        G4RotationMatrix rot_sideFEE_left;
        rot_sideFEE_left.rotateY(-angle_containerFrameSide);
        G4Transform3D transform_sideFEE_left(rot_sideFEE_left, translation_sideFEE_left);
        name.str("");
        name<<"physical_sideFEE_left_m"<<kk;
        new G4PVPlacement(transform_sideFEE_left, logical_sideFEE, name.str(), logical_container[kk], false, 0, checkOverlap);

        G4ThreeVector translation_sideFEE_right(x_sideFEE, 0, z_sideFEE);
        G4RotationMatrix rot_sideFEE_right;
        rot_sideFEE_right.rotateZ(180*deg);
        rot_sideFEE_right.rotateY(angle_containerFrameSide);
        G4Transform3D transform_sideFEE_right(rot_sideFEE_right, translation_sideFEE_right);
        name.str("");
        name<<"physical_sideFEE_right_m"<<kk;
        new G4PVPlacement(transform_sideFEE_right, logical_sideFEE, name.str(), logical_container[kk], false, 0, checkOverlap);

        //casting die
        G4RotationMatrix rot_castingDie;
        rot_castingDie.rotateX(-90*deg);
        G4ThreeVector translation_castingDie_1(0, containerOut_y/2+upCover_y+castingDie_z/2+smallL, -containerOut_z/2+upCover_posz+castingDie_posz1);
        G4Transform3D transform_castingDie_1(rot_castingDie, translation_castingDie_1);
        name.str("");
        name<<"physical_castingDie_1_m"<<kk;
        new G4PVPlacement(transform_castingDie_1, logical_castingDie, name.str(), logical_container[kk], false, 0, checkOverlap);

        G4ThreeVector translation_castingDie_2(0, containerOut_y/2+upCover_y+castingDie_z/2+smallL, -containerOut_z/2+upCover_posz+castingDie_posz2);
        G4Transform3D transform_castingDie_2(rot_castingDie, translation_castingDie_2);
        name.str("");
        name<<"physical_castingDie_2_m"<<kk;
        new G4PVPlacement(transform_castingDie_2, logical_castingDie, name.str(), logical_container[kk], false, 0, checkOverlap);

        //gas container
        G4ThreeVector translation_gasContainer(0, 0, (container_lowBorder-container_thickness)/2);
        name.str("");
        name<<"physical_gasContainer_m"<<kk;
        new G4PVPlacement(0, translation_gasContainer, logical_gasContainer[kk], name.str(), logical_container[kk], false, 0, checkOverlap);
    }


    //Fill in the inside of the container
    //1mm from the bottom cover in design. Considering the modules's position are not real, change this number to make the bare chamber in the right place
    double l_chamber = honeyComb_y*2+tape_mrpc_y*2+pcbBoard_y0*2+pcbBoard_y1+mylar_y*4+glass0_y*4+glass1_y*10+gasLayer_y*12;
    double y_chamber;
    double z_chamber = (containerIn_z-pcbBoard_z)/2-smallL;
    for(int kk=0; kk<4; kk++)
    {
        if(kk<2) y_chamber = -(containerIn_y-l_chamber)/2+chamber_in_posy;
        else y_chamber = -(containerIn_y-l_chamber)/2+chamber_out_posy;
        G4Transform3D transform_bareChamber;
        if(kk==0 || kk==2)
        {
            transform_bareChamber = G4Transform3D(rot_dummy, G4ThreeVector(0,y_chamber,z_chamber));
        }
        else
        {
            G4RotationMatrix rot_bareChamber;
            rot_bareChamber.rotateZ(180*deg);
            transform_bareChamber = G4Transform3D(rot_bareChamber, G4ThreeVector(0,y_chamber,z_chamber));
        }
        new G4PVPlacement(transform_bareChamber, logical_bareChamber, "physical_bareChamber", logical_gasContainer[kk], false, 0, checkOverlap);

        //lower bracket
        G4double z_bracket = -(containerIn_z-bracket_z)/2+smallL;
        new G4PVPlacement(0, G4ThreeVector(-bracket_posx, 0, z_bracket), logical_bracket, "physical_bracket_0", logical_gasContainer[kk], false, 0, checkOverlap);
        new G4PVPlacement(0, G4ThreeVector(bracket_posx, 0, z_bracket), logical_bracket, "physical_bracket_1", logical_gasContainer[kk], false, 0, checkOverlap);

        //side stop block
        G4Transform3D transform_sideStopBlock;
        G4double angle_gasContainerSide = (atan((containerIn_xup-containerIn_xdown)/2/(containerIn_z))) *rad;
        G4double x_sideStopBlock = (containerIn_xup+containerIn_xdown)/4+sideStopBlock_posz0*tan(angle_gasContainerSide)-sideStopBlock_x/2*cos(angle_gasContainerSide);
        G4double z_sideStopBlock = sideStopBlock_posz0+sideStopBlock_x/2*sin(angle_gasContainerSide);

        G4RotationMatrix rot_sideStopBlock;
        rot_sideStopBlock.rotateY(angle_gasContainerSide);
        rot_sideStopBlock.rotateZ(180*deg);
        transform_sideStopBlock = G4Transform3D(rot_sideStopBlock, G4ThreeVector(-x_sideStopBlock, 0, z_sideStopBlock));
        new G4PVPlacement(transform_sideStopBlock, logical_sideStopBlock, "physical_sideStopBlock_0", logical_gasContainer[kk], false, 0, checkOverlap);

        rot_sideStopBlock = G4RotationMatrix(0,0,0);
        rot_sideStopBlock.rotateY(angle_gasContainerSide);
        transform_sideStopBlock = G4Transform3D(rot_sideStopBlock, G4ThreeVector(x_sideStopBlock, 0, z_sideStopBlock));
        new G4PVPlacement(transform_sideStopBlock, logical_sideStopBlock, "physical_sideStopBlock_1", logical_gasContainer[kk], false, 0, checkOverlap);

        x_sideStopBlock =  (containerIn_xup+containerIn_xdown)/4+sideStopBlock_posz1*tan(angle_gasContainerSide)-sideStopBlock_x/2*cos(angle_gasContainerSide);
        z_sideStopBlock = sideStopBlock_posz1+sideStopBlock_x/2*sin(angle_gasContainerSide);
        rot_sideStopBlock = G4RotationMatrix(0,0,0);
        rot_sideStopBlock.rotateY(angle_gasContainerSide);
        rot_sideStopBlock.rotateZ(180*deg);
        transform_sideStopBlock = G4Transform3D(rot_sideStopBlock, G4ThreeVector(-x_sideStopBlock, 0, z_sideStopBlock));
        new G4PVPlacement(transform_sideStopBlock, logical_sideStopBlock, "physical_sideStopBlock_2", logical_gasContainer[kk], false, 0, checkOverlap);

        rot_sideStopBlock = G4RotationMatrix(0,0,0);
        rot_sideStopBlock.rotateY(angle_gasContainerSide);
        transform_sideStopBlock = G4Transform3D(rot_sideStopBlock, G4ThreeVector(x_sideStopBlock, 0, z_sideStopBlock));
        new G4PVPlacement(transform_sideStopBlock, logical_sideStopBlock, "physical_sideStopBlock_3", logical_gasContainer[kk], false, 0, checkOverlap);
    }

    //stop block
    //upper stop block
    G4double x_upStopBlock = containerIn_xup/4;
    G4double y_upStopBlock = pcbBoard_y1/2.+mylar_y+glass0_y+glass1_y*2.5+gasLayer_y*3;
    G4double z_upStopBlock = (pcbBoard_z-upStopBlock_z)/2-smallL;
    new G4PVPlacement(0, G4ThreeVector(-x_upStopBlock, -y_upStopBlock, z_upStopBlock ), logical_upStopBlock, "physical_upStopBlock_0", logical_bareChamber, false,0, checkOverlap);
    new G4PVPlacement(0, G4ThreeVector(-x_upStopBlock, y_upStopBlock, z_upStopBlock ), logical_upStopBlock, "physical_upStopBlock_1", logical_bareChamber, false,0, checkOverlap);
    new G4PVPlacement(0, G4ThreeVector(x_upStopBlock, -y_upStopBlock, z_upStopBlock ), logical_upStopBlock, "physical_upStopBlock_2", logical_bareChamber, false,0, checkOverlap);
    new G4PVPlacement(0, G4ThreeVector(x_upStopBlock, y_upStopBlock, z_upStopBlock ), logical_upStopBlock, "physical_upStopBlock_3", logical_bareChamber, false,0, checkOverlap);


    //honeyComb
    G4LogicalVolume* logical_honeyComb = partProduce->lg_honeyComb("logical_honeyComb");
    G4double y_honeyComb = (honeyComb_y+pcbBoard_y1)/2+tape_mrpc_y+pcbBoard_y0+mylar_y*2+glass0_y*2+glass1_y*5+gasLayer_y*6;
    G4double z_honeyComb = (pcbBoard_z-honeyComb_z)/2-honeyComb_posz;
    new G4PVPlacement(0,G4ThreeVector(0,y_honeyComb,z_honeyComb), logical_honeyComb,"physical_honeyComb_0",logical_bareChamber,false,0, checkOverlap);
    new G4PVPlacement(0,G4ThreeVector(0,-1*y_honeyComb,z_honeyComb), logical_honeyComb,"physical_honeyComb_1",logical_bareChamber,false,1, checkOverlap);

    //tape_mrpc
    G4LogicalVolume* logical_tape_mrpc = partProduce->lg_tape_mrpc("logical_tape_mrpc");
    G4double y_tape_mrpc = (tape_mrpc_y+pcbBoard_y1)/2+pcbBoard_y0+mylar_y*2+glass0_y*2+glass1_y*5+gasLayer_y*6;
    G4double z_tape_mrpc = (pcbBoard_z-tape_mrpc_z)/2-tape_mrpc_posz;
    new G4PVPlacement(0,G4ThreeVector(0,y_tape_mrpc,z_tape_mrpc), logical_tape_mrpc,"physical_tape_mrpc_0",logical_bareChamber,false,0, checkOverlap);
    new G4PVPlacement(0,G4ThreeVector(0,-1*y_tape_mrpc,z_tape_mrpc), logical_tape_mrpc,"physical_tape_mrpc_1",logical_bareChamber,false,1, checkOverlap);

    //pcbBoard
    G4LogicalVolume* logical_pcbBoard0 = partProduce->lg_pcbBoard(0, "logical_pcbBoard0");
    G4double y_pcbBoard = (pcbBoard_y0+pcbBoard_y1)/2+mylar_y*2+glass0_y*2+glass1_y*5+gasLayer_y*6;
    new G4PVPlacement(0,G4ThreeVector(0,y_pcbBoard,0),logical_pcbBoard0,"physical_pcbBoard0_0",logical_bareChamber,false,0, checkOverlap);
    //Notice!!! I rotate 180 deg in order to make the strips near the PCB surface, 
    //but keep in mind that this will make the readout strips not mirror symmetry, but Z axial symmetry
    G4RotationMatrix* rot_pcbBoard = new G4RotationMatrix();
    rot_pcbBoard->rotateZ(180*degree);
    new G4PVPlacement(rot_pcbBoard,G4ThreeVector(0,-1*y_pcbBoard,0),logical_pcbBoard0,"physical_pcbBoard0_1",logical_bareChamber,false,1, checkOverlap);

    G4LogicalVolume* logical_pcbBoard1 = partProduce->lg_pcbBoard(1, "logical_pcbBoard1");
    new G4PVPlacement(0,G4ThreeVector(0,0,0),logical_pcbBoard1,"physical_pcbBoard1",logical_bareChamber,false,0, checkOverlap);

    //readout strip
    G4AssemblyVolume* logical_strip = partProduce->lg_strip("logical_strip");
    G4double z_strip = (pcbBoard_z-12*strip_z-11*strip_gap)/2-strip_posz;
    G4Transform3D transform_strip(rot_dummy, G4ThreeVector(0, pcbBoard_y0/2.-strip_y/2-smallL, z_strip));
    logical_strip->MakeImprint(logical_pcbBoard0, transform_strip);
    transform_strip = G4Transform3D(rot_dummy, G4ThreeVector(0,0,z_strip));
    logical_strip->MakeImprint(logical_pcbBoard1, transform_strip);

    //mylar
    G4LogicalVolume* logical_mylar = partProduce->lg_mylar("logical_mylar");
    G4double y_mylar = (mylar_y+pcbBoard_y1)/2+mylar_y+glass0_y*2+glass1_y*5+gasLayer_y*6;
    G4double z_mylar = (pcbBoard_z-mylar_z)/2-mylar_posz;
    new G4PVPlacement(0,G4ThreeVector(0,y_mylar,z_mylar),logical_mylar,"physical_mylar_0",logical_bareChamber,false,0, checkOverlap);
    new G4PVPlacement(0,G4ThreeVector(0,-y_mylar,z_mylar),logical_mylar,"physical_mylar_3",logical_bareChamber,false,3, checkOverlap);

    y_mylar = (mylar_y+pcbBoard_y1)/2;
    new G4PVPlacement(0,G4ThreeVector(0,y_mylar,z_mylar),logical_mylar,"physical_mylar_1",logical_bareChamber,false,1, checkOverlap);
    new G4PVPlacement(0,G4ThreeVector(0,-y_mylar,z_mylar),logical_mylar,"physical_mylar_2",logical_bareChamber,false,2, checkOverlap);

    //glass
    G4LogicalVolume* logical_glass0 = partProduce->lg_glass(0, "logical_glass0");
    G4double y_glass = (glass0_y+pcbBoard_y1)/2.+mylar_y+glass0_y+glass1_y*5+gasLayer_y*6;
    G4double z_glass = (pcbBoard_z-glass0_z)/2.-glass0_posz;
    new G4PVPlacement(0, G4ThreeVector(0, y_glass, z_glass), logical_glass0, "physical_glass0_0", logical_bareChamber, false, 0, checkOverlap);
    new G4PVPlacement(0, G4ThreeVector(0, -y_glass, z_glass), logical_glass0, "physical_glass0_3", logical_bareChamber, false, 3, checkOverlap);
    y_glass = pcbBoard_y1/2.+mylar_y+glass0_y/2.;
    new G4PVPlacement(0, G4ThreeVector(0, y_glass, z_glass), logical_glass0, "physical_glass0_1", logical_bareChamber, false, 1, checkOverlap);
    new G4PVPlacement(0, G4ThreeVector(0, -y_glass, z_glass), logical_glass0, "physical_glass0_2", logical_bareChamber, false, 2, checkOverlap);

    G4LogicalVolume* logical_glass1 = partProduce->lg_glass(1, "logical_glass1");
    z_glass = (pcbBoard_z-glass1_z)/2.-glass1_posz;
    for(G4int i=0; i<5; i++)
    {
        y_glass = pcbBoard_y1/2.+mylar_y+glass0_y+glass1_y*(4.5-i)+gasLayer_y*(5-i);
        name.str("");
        name<<"physical_glass1_"<<i;
        new G4PVPlacement(0, G4ThreeVector(0, y_glass, z_glass), logical_glass1, name.str(), logical_bareChamber, false, i, checkOverlap);
        name.str("");
        name<<"physical_glass1_"<<9-i;
        new G4PVPlacement(0, G4ThreeVector(0, -y_glass, z_glass), logical_glass1, name.str(), logical_bareChamber, false, 9-i, checkOverlap);
    }

    //gas
    logical_gasLayer = partProduce->lg_gasLayer("logical_gasLayer");
    G4double y_gasLayer;
    G4double z_gasLayer = (pcbBoard_z-gasLayer_z)/2.-gasLayer_posz;
    G4VPhysicalVolume* physical_gasLayer[12];
    for(G4int i=0; i<6; i++)  //y->larger, gasNp->larger
    {
        y_gasLayer = pcbBoard_y1/2.+mylar_y+glass0_y+glass1_y*(5-i)+gasLayer_y*(5.5-i);
        name.str("");
        name<<"physical_gasLayer_"<<11-i;
        physical_gasLayer[i] = new G4PVPlacement(0, G4ThreeVector(0, -y_gasLayer, z_gasLayer), logical_gasLayer, name.str(), logical_bareChamber, false, 11-i, checkOverlap);
    }
    for(G4int i=6; i<12; i++)
    {
        y_gasLayer = pcbBoard_y1/2.+mylar_y+glass0_y+glass1_y*(i-6)+gasLayer_y*(-5.5+i);
        name.str("");
        name<<"physical_gasLayer_"<<11-i;
        physical_gasLayer[i] = new G4PVPlacement(0, G4ThreeVector(0, y_gasLayer, z_gasLayer), logical_gasLayer, name.str(), logical_bareChamber, false, 11-i, checkOverlap);
    }


    //arrange the 72 modules in the endcap, it's assumed to form two semicircles with a gap of about 2mm

    // Z: East; y: Up; x: North
    //      /y                                            |z  y                   y|/|   
    //   |z/              ___________    rotateX          |  /        rotateZ     /| | 
    //   |/_____x         \         /                  ___|_/_                    ||_|____x
    //                     \       /     ->>>>        /   |/__\___x    ->>>>      |  |
    //            small end \_____/  bigger end      /_________\                   \ |
    //                                                                              \|   

    const G4int n_module = 36;
    G4double angle = 360.0*deg/n_module;
    G4double z_layerIn = endcap_length/2-containerOut_y/2-lowCover1_y-layer_posz;
    G4double z_layerOut = endcap_length/2-containerOut_y/2-lowCover2_y-layer_posz;
    //Adjusting parameters
    //rOffset_east: along R direction
    //angle_east: along phi direction
    //angleOffset_east: along self asymmetric axis
    rOffset_east = tofPara->GetVec("rOffset_east");
    angle_east = tofPara->GetVec("angle_east");
    angleOffset_east = tofPara->GetVec("angleOffset_east");
    rOffset_west = tofPara->GetVec("rOffset_west");
    angle_west = tofPara->GetVec("angle_west");
    angleOffset_west = tofPara->GetVec("angleOffset_west");


    //arrange east endcap
    for(int i=0; i<n_module; i++)
    {
        G4double angle_module = startAngle_east+angle_east[i]+i*angle;
        G4double r_module = endcap_r+rOffset_east[i];

        G4RotationMatrix rot_layerIn_east;
        rot_layerIn_east.rotateX(90.*deg);
        rot_layerIn_east.rotateZ(90.*deg);
        rot_layerIn_east.rotateZ(angle_module+angleOffset_east[i]);

        G4ThreeVector translation_layerIn_east = G4ThreeVector(r_module*cos(angle_module), r_module*sin(angle_module), -z_layerIn);
        G4Transform3D transform_layerIn_east = G4Transform3D(rot_layerIn_east, translation_layerIn_east);


        G4RotationMatrix rot_layerOut_east;
        rot_layerOut_east.rotateZ(180.*deg);
        rot_layerOut_east.rotateX(90.*deg);
        rot_layerOut_east.rotateZ(90.*deg);
        rot_layerOut_east.rotateZ(angle_module+angleOffset_east[i]);

        G4ThreeVector translation_layerOut_east = G4ThreeVector(r_module*cos(angle_module), r_module*sin(angle_module), z_layerOut);
        G4Transform3D transform_layerOut_east = G4Transform3D(rot_layerOut_east, translation_layerOut_east);

        name.str("");
        name<<"physical_mrpc_east_"<<i;
        if(i%2==0)
        {
            new G4PVPlacement(transform_layerOut_east, logical_container[3], name.str(), logicalEcTofEast, false, i, checkOverlap);
        }
        else
        {
            new G4PVPlacement(transform_layerIn_east, logical_container[0], name.str(), logicalEcTofEast, false, i, checkOverlap);
        }
    }

    //arrange west endcap
    for(int i=0; i<n_module; i++)
    {
        G4double angle_module = startAngle_west+angle_west[i]+i*angle;
        G4double r_module = endcap_r+rOffset_west[i];

        G4RotationMatrix rot_layerIn_west;
        rot_layerIn_west.rotateZ(180.*deg);
        rot_layerIn_west.rotateX(90.*deg);
        rot_layerIn_west.rotateZ(90.*deg);
        rot_layerIn_west.rotateZ(angle_module+angleOffset_west[i]);

        G4ThreeVector translation_layerIn_west = G4ThreeVector(r_module*cos(angle_module), r_module*sin(angle_module), z_layerIn);
        G4Transform3D transform_layerIn_west = G4Transform3D(rot_layerIn_west, translation_layerIn_west);


        G4RotationMatrix rot_layerOut_west;
        rot_layerOut_west.rotateX(90.*deg);
        rot_layerOut_west.rotateZ(90.*deg);
        rot_layerOut_west.rotateZ(angle_module+angleOffset_west[i]);

        G4ThreeVector translation_layerOut_west = G4ThreeVector(r_module*cos(angle_module), r_module*sin(angle_module), -z_layerOut);
        G4Transform3D transform_layerOut_west = G4Transform3D(rot_layerOut_west, translation_layerOut_west);

        name.str("");
        name<<"physical_mrpc_west_"<<i;
        if(i%2==0)
        {
            new G4PVPlacement(transform_layerOut_west, logical_container[2], name.str(), logicalEcTofWest, false, i, checkOverlap);
        }
        else
        {
            new G4PVPlacement(transform_layerIn_west, logical_container[1], name.str(), logicalEcTofWest, false, i, checkOverlap);
        }
    }
} 



void BesTofConstruction::DefineMaterial()   //This fucntion just "build" my materials 
{
    G4double a,z,density,fraction;
    G4int nel, natoms, ncomponents;
    G4String name, symbol;


    G4Element* H=G4Element::GetElement("Hydrogen");
    if (!H)
      H = new G4Element(name="Hydrogen",symbol="H" , z= 1., a=1.01*g/mole);

    G4Element* C=G4Element::GetElement("Carbon");
    if (!C)
      C = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a=12.01*g/mole);

    G4Element* F=G4Element::GetElement("Fluorin");
    if (!F)
      F = new G4Element(name="Fluorin", symbol="F", z=9., a=18.01*g/mole);

    G4Element* O=G4Element::GetElement("Oxygen");
    if (!O)
      O = new G4Element(name="Oxygen",symbol="O",z= 8., a= 16.00*g/mole);

    G4Element* N=G4Element::GetElement("Nitrogen");
    if (!N)
      N = new G4Element(name="Nitrogen",symbol="N",z= 7., a= 14.01*g/mole);

    G4Element* S=G4Element::GetElement("Sulfur");
    if(!S)
      S = new G4Element(name="Sulfur",symbol="S",z= 16., a= 32.06*g/mole);


    BC404 = new G4Material("BC404",density=1.032*g/cm3,nel=2);
    BC404->AddElement(C,10);
    BC404->AddElement(H,11);

    BC408 = new G4Material("BC408",density=1.032*g/cm3,nel=2);
    BC408->AddElement(C,1000);
    BC408->AddElement(H,1104);

    PVF = new G4Material("PVF",density=1.45*g/cm3,nel=3);
    PVF->AddElement(C,2);
    PVF->AddElement(H,3);
    PVF->AddElement(F,1);

    //PMT mixed material
    G4Material* Cu = G4Material::GetMaterial("Copper");
    G4Material* Al = G4Material::GetMaterial("Aluminium");

    //vacuum
    //G4Material* Vacuum = new G4Material("Galactic", z=1., a=1.01*g/mole,
    //density= universe_mean_density, kStateGas, 3.e-18*pascal, 2.73*kelvin);

    density = 1.4618815*g/cm3;
    PMTmaterial =  new G4Material(name="PMTmaterial", density,  4);
    PMTmaterial->AddMaterial(Al, 0.4495);
    PMTmaterial->AddMaterial(Cu,0.35);
    PMTmaterial->AddMaterial(G4Material::GetMaterial("SiO2"),0.2);
    PMTmaterial->AddMaterial(G4Material::GetMaterial("Air"),0.0005);
    //PMTmaterial->AddMaterial(Vacuum,0.1);
    G4cout<<PMTmaterial;

    density = 1.002*g/cm3;
    G4Material* tape = new G4Material(name="tape",density,nel=2);
    tape->AddElement(C,2);
    tape->AddElement(H,5);


    //MRPC material
    //The stop block in the gas container
    G4Material* Nylon = new G4Material("Nylon", density=1.15*g/cm3, ncomponents=4);
    Nylon->AddElement(C, natoms=2);
    Nylon->AddElement(H, natoms=3);
    Nylon->AddElement(O, natoms=1);
    Nylon->AddElement(N, natoms=1);
    G4cout<<Nylon<<G4endl;

    //The core of honeyComb is thought to be cellulose
    G4Material* honeycombCore = new G4Material("honeycombCore", density=0.024*g/cm3, ncomponents=3);
    honeycombCore->AddElement(C,natoms=6);
    honeycombCore->AddElement(H,natoms=10);
    honeycombCore->AddElement(O,natoms=5);
    G4cout<<honeycombCore<<G4endl;

    //The honeyComb surface and the PCB board are thought to be composed of SiO2(60%) and epoxy resins(40%)
    G4Material* epoxy = new G4Material("epoxy", density=1.2*g/cm3, ncomponents=3);
    epoxy->AddElement(C,natoms=11);
    epoxy->AddElement(H,natoms=12);
    epoxy->AddElement(O,natoms=3);
    G4cout<<epoxy<<G4endl;

    G4Material* insulationBoard = new G4Material("insulationBoard", density=1.85*g/cm3, 2);
    insulationBoard->AddMaterial(G4Material::GetMaterial("SiO2"), 0.6);
    insulationBoard->AddMaterial(epoxy, 0.4);
    G4cout<<insulationBoard<<G4endl;

    //Mylar and tape are thought to be PET
    G4Material* PET = new G4Material("PET", density= 1.39*g/cm3, ncomponents=3);
    PET->AddElement(C,natoms=10);
    PET->AddElement(H,natoms=8);
    PET->AddElement(O,natoms=4);
    G4cout<<PET<<G4endl;

    //MRPC gas: 90% r134a, 5% isobutan, 5% SF6 
    G4Material* FreonR134A = new G4Material("FreonR134A", density =4.241*mg/cm3,ncomponents=3);
    FreonR134A->AddElement(C,natoms=2);
    FreonR134A->AddElement(H,natoms=2);
    FreonR134A->AddElement(F,natoms=4);
    G4cout<<FreonR134A<<G4endl;

    G4Material* SF6 = new G4Material("SF6", density =6.14*mg/cm3,ncomponents=2);
    SF6->AddElement(S,natoms=1);
    SF6->AddElement(F,natoms=6);
    G4cout<<SF6<<G4endl;

    G4Material* Isobutan = new G4Material("Isobutan", density =2.487*mg/cm3,ncomponents=2);
    Isobutan->AddElement(C, natoms=4);
    Isobutan->AddElement(H, natoms=10);
    G4cout<<Isobutan<<G4endl;

    G4Material* MRPCGas = new G4Material(name="MRPCGas", density=4.17*mg/cm3,ncomponents=3);
    MRPCGas->AddMaterial(FreonR134A, fraction= 89.69*perCent);
    MRPCGas->AddMaterial(SF6, fraction=7.34*perCent);
    MRPCGas->AddMaterial(Isobutan, fraction=2.97*perCent);

    //This setting is to produce ion pairs, default is 0 
    if(0.0 == MRPCGas->GetIonisation()->GetMeanEnergyPerIonPair())
    {
        //MRPCGas->GetIonisation()->SetMeanEnergyPerIonPair(20*eV);
        MRPCGas->GetIonisation()->SetMeanEnergyPerIonPair(m_ionE*eV);
    }
    G4cout<<MRPCGas<<G4endl;
}

void  BesTofConstruction::getXYZ( const G4RotationMatrix r, double& a, double& b, double& c) const
{
    double cosb = sqrt( r.xx()*r.xx() + r.yx()*r.yx() );

    if (cosb > 16*FLT_EPSILON)
    {
        a = atan2(  r.zy(), r.zz() );
        b = atan2( -r.zx(), cosb    );
        c = atan2(  r.yx(), r.xx() );
    }
    else
    {
        a = atan2( -r.yz(), r.yy() );
        b = atan2( -r.zx(), cosb    );
        c = 0.;
    }
    //std::cout.precision(20);
    //std::cout<<"in    getXYZ   :( "<<r.xx()<<" "<<r.xy()<<" "<<r.xz()<<std::endl;
    //std::cout<<"                  "<<r.yx()<<" "<<r.yy()<<" "<<r.yz()<<std::endl;
    //std::cout<<"                  "<<r.zx()<<" "<<r.zy()<<" "<<r.zz()<<std::endl;
    //std::cout<<"details: cosb="<<cosb<<" a="<<a<<" b="<<b<<" c="<<c<<" min="<<16*FLT_EPSILON<<std::endl;


    G4RotationMatrix temp;
    temp.rotateZ((c/deg)*deg);
    //std::cout.precision(20);
    //std::cout<<"in detail temp2:( "<<temp.xx()<<" "<<temp.xy()<<" "<<temp.xz()<<std::endl;
    //std::cout<<"                  "<<temp.yx()<<" "<<temp.yy()<<" "<<temp.yz()<<std::endl;
    //std::cout<<"                  "<<temp.zx()<<" "<<temp.zy()<<" "<<temp.zz()<<std::endl;
}


void BesTofConstruction::IniParam_mrpc()
{
    tofPara = BesTofGeoParameter::GetInstance();

    //All the values are in units of mm or deg
    containerOut_xdown = tofPara->Get("containerOut_xdown");
    containerOut_xup = tofPara->Get("containerOut_xup");
    containerOut_y = tofPara->Get("containerOut_y");
    containerOut_z = tofPara->Get("containerOut_z");
    container_thickness = tofPara->Get("container_thickness");
    container_lowBorder = tofPara->Get("container_lowBorder");

    containerIn_xdown = tofPara->Get("containerIn_xdown");
    containerIn_xup = tofPara->Get("containerIn_xup");
    containerIn_y = tofPara->Get("containerIn_y");
    containerIn_z = tofPara->Get("containerIn_z");

    upCover_xdown = tofPara->Get("upCover_xdown");
    upCover_xup = tofPara->Get("upCover_xup");
    upCover_y = tofPara->Get("upCover_y");
    upCover_z = tofPara->Get("upCover_z");
    upCover_posz = tofPara->Get("upCover_posz");

    lowCover1_xdown = tofPara->Get("lowCover1_xdown");
    lowCover1_xup = tofPara->Get("lowCover1_xup");
    lowCover1_y = tofPara->Get("lowCover1_y");
    lowCover1_z = tofPara->Get("lowCover1_z");
    lowCover1_addx = tofPara->Get("lowCover1_addx");
    lowCover1_addz = tofPara->Get("lowCover1_addz");
    lowCover1_posz = tofPara->Get("lowCover1_posz");

    lowCover2_xdown = tofPara->Get("lowCover2_xdown");
    lowCover2_xup = tofPara->Get("lowCover2_xup");
    lowCover2_y = tofPara->Get("lowCover2_y");
    lowCover2_z = tofPara->Get("lowCover2_z");
    lowCover2_posz = tofPara->Get("lowCover2_posz");

    upFEE_xdown = tofPara->Get("upFEE_xdown");
    upFEE_xup = tofPara->Get("upFEE_xup");
    upFEE_y = tofPara->Get("upFEE_y");
    upFEE_z = tofPara->Get("upFEE_z");
    FEE_thickness = tofPara->Get("FEE_thickness");
    upFEE_posz = tofPara->Get("upFEE_posz");

    upFEE_pcb_xdown = tofPara->Get("upFEE_pcb_xdown");
    upFEE_pcb_xup = tofPara->Get("upFEE_pcb_xup");
    upFEE_pcb_y = tofPara->Get("upFEE_pcb_y");
    upFEE_pcb_z = tofPara->Get("upFEE_pcb_z");

    sideFEE_x = tofPara->Get("sideFEE_x");
    sideFEE_y = tofPara->Get("sideFEE_y");
    sideFEE_z = tofPara->Get("sideFEE_z");
    sideFEE_posl = tofPara->Get("sideFEE_posl");

    sideFEE_pcb_x = tofPara->Get("sideFEE_pcb_x");
    sideFEE_pcb_y = tofPara->Get("sideFEE_pcb_y");
    sideFEE_pcb_z = tofPara->Get("sideFEE_pcb_z");

    castingDie_x = tofPara->Get("castingDie_x");
    castingDie_y = tofPara->Get("castingDie_y");
    castingDie_z = tofPara->Get("castingDie_z");
    castingDie_addr = tofPara->Get("castingDie_addr");
    castingDie_subtractx = tofPara->Get("castingDie_subtractx");
    castingDie_subtract_posz = tofPara->Get("castingDie_subtract_posz");
    castingDie_posz1 = tofPara->Get("castingDie_posz1");
    castingDie_posz2 = tofPara->Get("castingDie_posz2");

    chamber_in_posy = tofPara->Get("chamber_in_posy");
    chamber_out_posy = tofPara->Get("chamber_out_posy");

    bracket_x = tofPara->Get("bracket_x");
    bracket_y = tofPara->Get("bracket_y");
    bracket_z = tofPara->Get("bracket_z");
    bracket_r = tofPara->Get("bracket_r");
    bracket_posx = tofPara->Get("bracket_posx");

    upStopBlock_x = tofPara->Get("upStopBlock_x");
    upStopBlock_y = tofPara->Get("upStopBlock_y");
    upStopBlock_z = tofPara->Get("upStopBlock_z");
    upStopBlock_subtractr = tofPara->Get("upStopBlock_subtractr");
    upStopBlock_subtractposz = tofPara->Get("upStopBlock_subtractposz");

    sideStopBlock_x = tofPara->Get("sideStopBlock_x");
    sideStopBlock_y = tofPara->Get("sideStopBlock_y");
    sideStopBlock_z = tofPara->Get("sideStopBlock_z");
    sideStopBlock_thickness = tofPara->Get("sideStopBlock_thickness");
    sideStopBlock_posz0 = tofPara->Get("sideStopBlock_posz0");
    sideStopBlock_posz1 = tofPara->Get("sideStopBlock_posz1");


    honeyComb_xdown = tofPara->Get("honeyComb_xdown");
    honeyComb_xup = tofPara->Get("honeyComb_xup");
    honeyComb_y = tofPara->Get("honeyComb_y");
    honeyComb_z = tofPara->Get("honeyComb_z");
    honeyComb_surface_y = tofPara->Get("honeyComb_surface_y");
    honeyComb_posz = tofPara->Get("honeyComb_posz");

    tape_mrpc_xdown = tofPara->Get("tape_mrpc_xdown");
    tape_mrpc_xup = tofPara->Get("tape_mrpc_xup");
    tape_mrpc_y = tofPara->Get("tape_mrpc_y");
    tape_mrpc_z = tofPara->Get("tape_mrpc_z");
    tape_mrpc_posz = tofPara->Get("tape_mrpc_posz");

    pcbBoard_xdown = tofPara->Get("pcbBoard_xdown");
    pcbBoard_xup = tofPara->Get("pcbBoard_xup");
    pcbBoard_y0 = tofPara->Get("pcbBoard_y0");
    pcbBoard_y1 = tofPara->Get("pcbBoard_y1");
    pcbBoard_z = tofPara->Get("pcbBoard_z");

    strip_x[0] = tofPara->Get("strip_x[0]");
    strip_x[1] = tofPara->Get("strip_x[1]");
    strip_x[2] = tofPara->Get("strip_x[2]");
    strip_x[3] = tofPara->Get("strip_x[3]");
    strip_x[4] = tofPara->Get("strip_x[4]");
    strip_x[5] = tofPara->Get("strip_x[5]");
    strip_x[6] = tofPara->Get("strip_x[6]");
    strip_x[7] = tofPara->Get("strip_x[7]");
    strip_x[8] = tofPara->Get("strip_x[8]");
    strip_x[9] = tofPara->Get("strip_x[9]");
    strip_x[10] = tofPara->Get("strip_x[10]");
    strip_x[11] = tofPara->Get("strip_x[11]");
    strip_y = tofPara->Get("strip_y");
    strip_z = tofPara->Get("strip_z");
    strip_gap = tofPara->Get("strip_gap");
    strip_posz = tofPara->Get("strip_posz");

    mylar_xdown = tofPara->Get("mylar_xdown");
    mylar_xup = tofPara->Get("mylar_xup");
    mylar_y = tofPara->Get("mylar_y");
    mylar_z = tofPara->Get("mylar_z");
    mylar_posz = tofPara->Get("mylar_posz");

    glass0_xdown = tofPara->Get("glass0_xdown");
    glass0_xup = tofPara->Get("glass0_xup");
    glass0_y = tofPara->Get("glass0_y");
    glass0_z = tofPara->Get("glass0_z");
    glass0_posz = tofPara->Get("glass0_posz");

    glass1_xdown = tofPara->Get("glass1_xdown");
    glass1_xup = tofPara->Get("glass1_xup");
    glass1_y = tofPara->Get("glass1_y");
    glass1_z = tofPara->Get("glass1_z");
    glass1_posz = tofPara->Get("glass1_posz");

    gasLayer_xdown = tofPara->Get("gasLayer_xdown");
    gasLayer_xup = tofPara->Get("gasLayer_xup");
    gasLayer_y = tofPara->Get("gasLayer_y");
    gasLayer_z = tofPara->Get("gasLayer_z");
    gasLayer_posz = tofPara->Get("gasLayer_posz");

    endcap_r = tofPara->Get("endcap_r");
    endcap_length = tofPara->Get("endcap_length");
    layer_posz = tofPara->Get("layer_posz");
    startAngle_east = tofPara->Get("startAngle_east");
    startAngle_west = tofPara->Get("startAngle_west");
}
