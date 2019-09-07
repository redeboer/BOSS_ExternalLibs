//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Using gdml in G4
//Author: Liuhm
//Created: May 25, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesDetectorConstruction.hh"
#include "BesMagneticField.hh"

#include "BesMdcConstruction.hh"
#include "BesTofConstruction.hh"
#include "BesEmcConstruction.hh"
#include "BesMucConstruction.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "globals.hh"
#include <cstdlib>
#include <fstream>
#include "ReadBoostRoot.hh"
#include "GDMLProcessor.hh"
#include "GDMLExpressionEvaluator.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
// Added here just to help resolve properly dependencies
#include "G4BooleanSolid.hh"
#include "G4Geo/BesG4Geo.h"

BesDetectorConstruction::BesDetectorConstruction()
{
  //create commands for interactive definition of the calorimeter  
  //detectorMessenger = new BesDetectorMessenger(this);

  if(ReadBoostRoot::GetField())
    besField = new BesMagneticField(); 
  
  mdc = new BesMdcConstruction();
  tof = new BesTofConstruction();
  emc = new BesEmcConstruction();
  muc = new BesMucConstruction();
  m_SCM = new BesSCM();
  m_Pip = new BesPip();
  m_pipeSCM = 2; 
  lWorld=0;
  fWorld=0; 
}

BesDetectorConstruction::~BesDetectorConstruction() 
{
  G4cout<<"BesDetectorConstruction::~BesDetectorConstruction()"<<G4endl;
  if(ReadBoostRoot::GetField() && besField)
    delete besField; 
  if(mdc) delete mdc;
  if(tof) delete tof;
  if(emc) delete emc;
  if(muc) delete muc;
  if(m_SCM) delete m_SCM;
  if(m_Pip) delete m_Pip;
}

G4VPhysicalVolume* BesDetectorConstruction::Construct() 
{ 
  if(m_pipeSCM != 2)
  {
    //define materials
    G4double a, z, density,abundance,fractionmass;
    G4double temperature, pressure;
    G4int iz,n,nel,ncomponents,natoms;
    G4String name,symbol;
    G4Isotope* U5 = new G4Isotope(name="U235", iz=92, n=235, a=235.01*g/mole);
    G4Isotope* U8 = new G4Isotope(name="U238", iz=92, n=238, a=238.03*g/mole);

    G4Element* U  = new G4Element(name="enriched Uranium",symbol="U",ncomponents=2);
    U->AddIsotope(U5, abundance= 90.*perCent);
    U->AddIsotope(U8, abundance= 10.*perCent);
    
    a = 1.01*g/mole;
    G4Element* H  = new G4Element(name="Hydrogen",symbol="H" , z= 1., a);
    
    a = 12.01*g/mole;
    G4Element* C  = new G4Element(name="Carbon"  ,symbol="C" , z= 6., a);

    a = 14.01*g/mole;
    G4Element* N  = new G4Element(name="Nitrogen",symbol="N" , z= 7., a);

    a = 16.00*g/mole;
    G4Element* O  = new G4Element(name="Oxygen"  ,symbol="O" , z= 8., a);

    a=28.09*g/mole;
    G4Element* Si = new G4Element(name="Silicon",symbol="Si",z=14.,a);
    
    a=126.90447*g/mole;
    G4Element* I = new G4Element(name="Iodine",symbol="I",z=53.,a);

    a=132.90545*g/mole;
    G4Element* Cs = new G4Element(name="Cesium",symbol="Cs",z=55.,a);

    density = 1.848*g/cm3;
    a = 9.012182*g/mole;
    G4Material* Beryllium = new G4Material(name="Beryllium",z=4.0,a,density);

    density = 2.70*g/cm3;
    a = 26.98*g/mole;
    G4Material* Aluminium = new G4Material(name="Aluminium",z=13.,a,density);

    density = 7.84*g/cm3;
    a = 55.845*g/mole;
    G4Material* Iron = new G4Material(name="Iron",z=26.0,a,density);

    density = 8.96*g/cm3;
    a = 63.546*g/mole;
    G4Material* Copper = new G4Material(name="Copper",z=29.0,a,density);

    density = 0.0001664*g/cm3;
    a = 4.0026*g/mole;
    G4Material* Hegas = new G4Material(name="Hegas",z=2.0,a,density);

    density = 0.001879*g/cm3;
    G4Material* Propane = new G4Material(name="Propane",density,nel=2);
    Propane->AddElement(C, natoms=3);
    Propane->AddElement(H, natoms=8);

    density = 4.53*g/cm3;
    G4Material* Cesiumiodide = new G4Material(name="Cesiumiodide",density,nel=2);
    Cesiumiodide->AddElement(I, natoms=1);
    Cesiumiodide->AddElement(Cs, natoms=1);

    density = 0.00085144*g/cm3;
    G4Material* Mdcgas = new G4Material(name="Mdcgas",density,nel=2);
    Mdcgas->AddMaterial(Hegas, fractionmass = 0.1173);
    Mdcgas->AddMaterial(Propane, fractionmass = 0.8827);

    density = 1.390*g/cm3;
    a = 39.95*g/mole;
    G4Material* liquidArgon = new G4Material(name="liquidArgon",z=18.0,a,density);

    density = 11.35*g/cm3; 
    a = 207.19*g/mole; 
    G4Material* Lead = new G4Material(name="Lead",z=82.,a,density);

    density = 1.0*g/cm3;
    G4Material* Water = new G4Material(name="Water", density, nel=2);
    Water->AddElement(H, natoms=2);
    Water->AddElement(O, natoms=1);

    density = 1.032*g/cm3;
    G4Material* Scintillator = new G4Material(name="Scintillator", density, nel=2);
    Scintillator->AddElement(C, natoms=9);
    Scintillator->AddElement(H, natoms=10);  

    density = 2.200*g/cm3;
    G4Material* SiO2 = new G4Material(name="SiO2", density, nel=2);
    SiO2->AddElement(Si, natoms=1);
    SiO2->AddElement(O, natoms=2);

    density = 1.290*mg/cm3;
    G4Material* Air = new G4Material(name="Air",density,nel=2);
    Air->AddElement(N, fractionmass = 0.7);
    Air->AddElement(O, fractionmass = 0.3);
    
    density = 0.200*g/cm3;
    G4Material* Aerogel = new G4Material(name="Aerogel",density,nel=3);
    Aerogel->AddMaterial(SiO2, fractionmass = 0.625);
    Aerogel->AddMaterial(Water, fractionmass = 0.374);
    Aerogel->AddElement(C, fractionmass = 0.001);
    
    density     = 27.0*mg/cm3;
    pressure    = 50.0*atmosphere;
    temperature = 325.0*kelvin;
    G4Material* CarbonicGas = new G4Material(name="CarbonicGas",density,nel=2,kStateGas,temperature,pressure);
    CarbonicGas->AddElement(C,natoms=1);
    CarbonicGas->AddElement(O,natoms=2);

    density     = 0.3*mg/cm3;
    pressure    = 2.0*atmosphere;
    temperature = 500.0*kelvin;
    G4Material* WaterSteam = new G4Material(name="WaterSteam",density,nel=1,kStateGas,temperature,pressure);
    WaterSteam->AddMaterial(Water,fractionmass=1);

    density     = universe_mean_density;
    pressure    = 3.e-18*pascal;
    temperature = 2.73*kelvin;
    G4Material* Galactic = new G4Material(name="Galactic", z=1., a=1.01*g/mole,
                                       density,kStateGas,temperature,pressure);

    density     = 1.0e-5*g/cm3;
    pressure    = 2.e-2*bar;
    G4Material* Beam = new G4Material(name="Beam",density,nel=1,kStateGas,STP_Temperature,pressure);
    Beam->AddMaterial(Air,fractionmass=1.0);

    //construct world volume
    G4Box* sWorld = new G4Box("solidWorld",263.5*cm,263.5*cm,287.5*cm);
    lWorld = new G4LogicalVolume(sWorld,Air,"logicalWorld",0,0,0);
    fWorld = new G4PVPlacement(0,G4ThreeVector(0,0,0),lWorld,"physicWorld",0,false,0); 
    //set visual attributes 
    lWorld -> SetVisAttributes(G4VisAttributes::Invisible);

    if( m_pipeSCM == 1 )
    {
      G4cout<<"construct beampipe and SCM with Geant4 classes"<<G4endl;
      //construct beam pipe
/*
      G4Tubs* sPipe = new G4Tubs("solidPipe",0.00*cm,3.58*cm,15.0*cm,0.*deg,360*deg);
      G4LogicalVolume* lPipe = new G4LogicalVolume(sPipe,Air,"logicalPipe",0,0,0);
      G4VPhysicalVolume* fPipe = new G4PVPlacement(0,G4ThreeVector(0,0,0),lPipe,"physicPipe",lWorld,false,0);
   
      G4Tubs* sPip1 = new G4Tubs("solidPip1",3.15*cm,3.23*cm,15.0*cm,0.*deg,360*deg);
      G4LogicalVolume* lPip1 = new G4LogicalVolume(sPip1,Beryllium,"logicalPip1",0,0,0);
      G4VPhysicalVolume* fPip1 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lPip1,"physicPip1",lPipe,false,0);
      
      G4Tubs* sPip2 = new G4Tubs("solidPip2",3.23*cm,3.53*cm,15.0*cm,0.*deg,360*deg);
      G4LogicalVolume* lPip2 = new G4LogicalVolume(sPip2,Hegas,"logicalPip2",0,0,0);
      G4VPhysicalVolume*  fPip2 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lPip2,"physicPip2",lPipe,false,0);

      G4Tubs* sPip3 = new G4Tubs("solidPip3",3.53*cm,3.58*cm,15.0*cm,0.*deg,360*deg);
      G4LogicalVolume* lPip3 = new G4LogicalVolume(sPip3,Beryllium,"logicalPip3",0,0,0);
      G4VPhysicalVolume*  fPip3 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lPip3,"physicPip3",lPipe,false,0);
      //set visual attributes
      lPipe -> SetVisAttributes(G4VisAttributes::Invisible);
      lPip1 -> SetVisAttributes(G4VisAttributes::Invisible);
      lPip2 -> SetVisAttributes(G4VisAttributes::Invisible);
      lPip3 -> SetVisAttributes(G4VisAttributes::Invisible);
*/
      m_Pip->Construct(lWorld);  
      m_SCM->Construct(lWorld);
/*      
      //construct SCM
      G4Tubs* sCoil = new G4Tubs("solidCoil",137.5*cm,170.0*cm,195.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil = new G4LogicalVolume(sCoil,Air,"logicalCoil",0,0,0);
      G4VPhysicalVolume*  fCoil = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil,"physicCoil",lWorld,false,0);

      G4Tubs* sCoilin = new G4Tubs("solidCoilin",137.5*cm,138.1*cm,195.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoilin = new G4LogicalVolume(sCoilin,Iron,"logicalCoilin",0,0,0);
      G4VPhysicalVolume*  fCoilin = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoilin,"physicCoilin",lCoil,false,0);

      G4Tubs* sCoilout = new G4Tubs("solidCoilout",168.4*cm,170.0*cm,195.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoilout = new G4LogicalVolume(sCoilout,Iron,"logicalCoilout",0,0,0);
      G4VPhysicalVolume*  fCoilout = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoilout,"physicCoilout",lCoil,false,0);

      G4Tubs* sCoil1 = new G4Tubs("solidCoil1",144.5*cm,145.0*cm,179.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil1 = new G4LogicalVolume(sCoil1,Aluminium,"logicalCoil1",0,0,0);
      G4VPhysicalVolume*  fCoil1 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil1,"physicCoil1",lCoil,false,0);

      G4Tubs* sCoil2 = new G4Tubs("solidCoil2",157.5*cm,158.0*cm,179.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil2 = new G4LogicalVolume(sCoil2,Aluminium,"logicalCoil2",0,0,0);
      G4VPhysicalVolume*  fCoil2 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil2,"physicCoil2",lCoil,false,0);

      G4Tubs* sCoil3 = new G4Tubs("solidCoil3",148.0*cm,150.0*cm,179.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil3 = new G4LogicalVolume(sCoil3,Aluminium,"logicalCoil3",0,0,0);
      G4VPhysicalVolume*  fCoil3 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil3,"physicCoil3",lCoil,false,0);

      G4Tubs* sCoil4 = new G4Tubs("solidCoil4",148.8*cm,149.2*cm,179.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil4 = new G4LogicalVolume(sCoil4,Copper,"logicalCoil4",0,0,0);
      G4VPhysicalVolume*  fCoil4 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil4,"physicCoil4",lCoil3,false,0);

      G4Tubs* sCoil5 = new G4Tubs("solidCoil5",150.5*cm,152.7*cm,179.5*cm,0.*deg,360*deg);
      G4LogicalVolume* lCoil5 = new G4LogicalVolume(sCoil5,Aluminium,"logicalCoil5",0,0,0);
      G4VPhysicalVolume*  fCoil5 = new G4PVPlacement(0,G4ThreeVector(0,0,0),lCoil5,"physicCoil5",lCoil,false,0);
      //set visual attributes
      lCoil -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoilin -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoilout -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoil1 -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoil2 -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoil3 -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoil4 -> SetVisAttributes(G4VisAttributes::Invisible);
      lCoil5 -> SetVisAttributes(G4VisAttributes::Invisible);  
*/
    }
  }
 
  else if(m_pipeSCM == 2)
  { 
/*
    BesG4Geo* aBesG4Geo = new BesG4Geo();
    
    //construct with gdml file
    //G4String GeometryPath = ReadBoostRoot::GetBoostRoot(); 
    //G4String GeometryPath = getenv("GDMLMANAGEMENTROOT");
    if(!GeometryPath){ 
      G4Exception("BOOST environment not set!");
    }
    GeometryPath += "/dat/Bes.gdml";
    
    G4cout<<"construc beampipe and SCM from: "<<GeometryPath<<G4endl;

    config.SetURI(GeometryPath);
    config.SetSetupName( "Default" );
    config.SetSetupVersion( "1.0" ); 
    config.SetType( "G4" );
    sxp.Configure( &config );
    sxp.Initialize();
    sxp.Run();
    

    fWorld=(G4VPhysicalVolume *)GDMLProcessor::GetInstance()->GetWorldVolume();  
    if( fWorld == 0 ) {
      G4Exception(
      "World volume not set properly check your setup selection criteria or GDML input!");
    }
    
    //make the world volume invisible   
    lWorld = FindLogicalVolume("logicalWorld");  
    lWorld -> SetVisAttributes(G4VisAttributes::Invisible); 

    //make the material inside magnet coil invisible

    G4LogicalVolume* lCoil = FindLogicalVolume("logicalCoil" );
    lCoil -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoilin = FindLogicalVolume("logicalCoilin" ); 
    lCoilin -> SetVisAttributes(G4VisAttributes::Invisible);
 
    G4LogicalVolume* lCoilout = FindLogicalVolume("logicalCoilout" );
    lCoilout -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoil1 = FindLogicalVolume("logicalCoil1");
    lCoil1 -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoil2 = FindLogicalVolume("logicalCoil2");
    lCoil2 -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoil3 = FindLogicalVolume("logicalCoil3");
    lCoil3 -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoil4 = FindLogicalVolume("logicalCoil4");
    lCoil4 -> SetVisAttributes(G4VisAttributes::Invisible);

    G4LogicalVolume* lCoil5 = FindLogicalVolume("logicalCoil5");
    lCoil5 -> SetVisAttributes(G4VisAttributes::Invisible);
   
    delete aBesG4Geo;
*/    
    SetVisAttributes("Pip_SCM"); 
    
  } 

  //construct subdetectors
  if(ReadBoostRoot::GetMdc())
    mdc->Construct(lWorld);
  if(ReadBoostRoot::GetTof())
    tof->Construct(lWorld);
  if(ReadBoostRoot::GetEmc())
    emc->Construct(lWorld);
  if(ReadBoostRoot::GetMuc())  
    muc->Construct(lWorld); 

 G4cout<<"get world "<<fWorld->GetLogicalVolume()->GetNoDaughters()<<endl;
    for(int nd = 0; nd < fWorld->GetLogicalVolume()->GetNoDaughters(); nd++)
      {
  G4cout<<"daughter n: name :"<<nd<<" "<<fWorld->GetLogicalVolume()->GetDaughter(nd)->GetName()<<G4endl;

      } 

  return fWorld;
}

G4LogicalVolume* BesDetectorConstruction::FindLogicalVolume(const G4String& vn )
{
  return const_cast<G4LogicalVolume*>( GDMLProcessor::GetInstance()->GetLogicalVolume(vn) );
}

void BesDetectorConstruction::SetVisAttributes(G4String volumeName)
{
  if(volumeName = "Pip_SCM")
  {
    BesG4Geo* aBesG4Geo = new BesG4Geo();
    fWorld=(G4VPhysicalVolume *)GDMLProcessor::GetInstance()->GetWorldVolume();
    if( fWorld == 0 ) {
      G4Exception(
        "World volume not set properly check your setup selection criteria or GDML input!");
    }

    //make the world volume invisible   
    lWorld = FindLogicalVolume("logicalWorld");  
    lWorld -> SetVisAttributes(G4VisAttributes::Invisible);
  
    //beam pipe

    G4LogicalVolume* logicalPip = FindLogicalVolume("logicalPip");
    G4LogicalVolume* logicalgoldLayer = FindLogicalVolume("logicalgoldLayer");
    G4LogicalVolume* logicalinnerBe = FindLogicalVolume("logicalinnerBe");
    G4LogicalVolume* logicaloilLayer = FindLogicalVolume("logicaloilLayer");
    G4LogicalVolume* logicalouterBe = FindLogicalVolume("logicalouterBe");
    G4LogicalVolume* logicalinnerBeSide = FindLogicalVolume("logicalinnerBeSide");
    G4LogicalVolume* logicalinnerAl = FindLogicalVolume("logicalinnerAl");
    G4LogicalVolume* logicalAlRing = FindLogicalVolume("logicalAlRing");
    G4LogicalVolume* logicalouterAl = FindLogicalVolume("logicalouterAl");
    G4LogicalVolume* logicalAgLayer = FindLogicalVolume("logicalAgLayer");
    G4LogicalVolume* logicalCuLayer = FindLogicalVolume("logicalCuLayer");
    G4LogicalVolume* logicalAlCover = FindLogicalVolume("logicalAlCover");

    logicalPip->SetVisAttributes(G4VisAttributes::Invisible);
    logicalgoldLayer->SetVisAttributes(G4VisAttributes::Invisible);
    logicalinnerBe->SetVisAttributes(G4VisAttributes::Invisible);
    logicaloilLayer->SetVisAttributes(G4VisAttributes::Invisible);
    logicalouterBe->SetVisAttributes(G4VisAttributes::Invisible);
    logicalinnerBeSide->SetVisAttributes(G4VisAttributes::Invisible);
    logicalinnerAl->SetVisAttributes(G4VisAttributes::Invisible);
    logicalAlRing->SetVisAttributes(G4VisAttributes::Invisible);
    logicalouterAl->SetVisAttributes(G4VisAttributes::Invisible);
    logicalAgLayer->SetVisAttributes(G4VisAttributes::Invisible);
    logicalCuLayer->SetVisAttributes(G4VisAttributes::Invisible);
    logicalAlCover->SetVisAttributes(G4VisAttributes::Invisible);

    //SCM
    G4LogicalVolume* logicalSCM = FindLogicalVolume("logicalSCM");
    G4LogicalVolume* logicalinnCryo = FindLogicalVolume("logicalinnCryo");
    G4LogicalVolume* logicalouterCryo =FindLogicalVolume("logicalouterCryo");
    G4LogicalVolume* logicalendCryo = FindLogicalVolume("logicalendCryo");
    G4LogicalVolume* logicall1Adia = FindLogicalVolume("logicall1Adia");
    G4LogicalVolume* logicall2Adia = FindLogicalVolume("logicall2Adia");
    G4LogicalVolume* logicall3Adia = FindLogicalVolume("logicall3Adia");
    G4LogicalVolume* logicall4Adia = FindLogicalVolume("logicall4Adia");
    G4LogicalVolume* logicalinnShield = FindLogicalVolume("logicalinnShield");
    G4LogicalVolume* logicaloutShield = FindLogicalVolume("logicaloutShield");
    G4LogicalVolume* logicalendShield = FindLogicalVolume("logicalendShield");
    G4LogicalVolume* logicall1insu = FindLogicalVolume("logicall1insu");
    G4LogicalVolume* logicall2insu = FindLogicalVolume("logicall2insu");
    G4LogicalVolume* logicalcoil = FindLogicalVolume("logicalcoil");
    G4LogicalVolume* logicalendCoil = FindLogicalVolume("logicalendCoil");
    G4LogicalVolume* logicalsupp = FindLogicalVolume("logicalsupp");
    G4LogicalVolume* logicalpipe1 = FindLogicalVolume("logicalpipe1");
    G4LogicalVolume* logicalpipe2 = FindLogicalVolume("logicalpipe2");
    G4LogicalVolume* logicalpipe3 = FindLogicalVolume("logicalpipe3");
    G4LogicalVolume* logicaltub17 = FindLogicalVolume("logicaltub17");
    G4LogicalVolume* logicaltub19 = FindLogicalVolume("logicaltub19");
    G4LogicalVolume* logicaltub20 = FindLogicalVolume("logicaltub20");
    G4LogicalVolume* logicaltub22 = FindLogicalVolume("logicaltub22");
    G4LogicalVolume* logicaltub23 = FindLogicalVolume("logicaltub23");
    G4LogicalVolume* logicaltub25 = FindLogicalVolume("logicaltub25");
    G4LogicalVolume* logicalsub1 = FindLogicalVolume("logicalsub1");
    G4LogicalVolume* logicalsub2 = FindLogicalVolume("logicalsub2");
    G4LogicalVolume* logicalsub3 = FindLogicalVolume("logicalsub3");
    G4LogicalVolume* logicalhole1 = FindLogicalVolume("logicalhole1");
    G4LogicalVolume* logicalhole2 = FindLogicalVolume("logicalhole2");
    G4LogicalVolume* logicalhole3 = FindLogicalVolume("logicalhole3");
    G4LogicalVolume* logicalhole4 = FindLogicalVolume("logicalhole4");
    G4LogicalVolume* logicalhole5 = FindLogicalVolume("logicalhole5");

//  G4VisAttributes* visSCM1 = new G4VisAttributes(G4Colour(0.,0.,1.));
  logicalSCM->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalinnCryo->SetVisAttributes(visSCM1);
  logicalinnCryo->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalouterCryo->SetVisAttributes(visSCM1);
  logicalouterCryo->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalendCryo->SetVisAttributes(visSCM1);
  logicalendCryo->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM2 = new G4VisAttributes(G4Colour(0.,1.,0.));
//  logicall1Adia->SetVisAttributes(visSCM2);
  logicall1Adia->SetVisAttributes(G4VisAttributes::Invisible);
//  logicall2Adia->SetVisAttributes(visSCM2);
  logicall2Adia->SetVisAttributes(G4VisAttributes::Invisible);
//  logicall3Adia->SetVisAttributes(visSCM2);
  logicall3Adia->SetVisAttributes(G4VisAttributes::Invisible);
//  logicall4Adia->SetVisAttributes(visSCM2);
  logicall4Adia->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM3 = new G4VisAttributes(G4Colour(1.,0.,0.));
//  logicalinnShield->SetVisAttributes(visSCM3);
  logicalinnShield->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaloutShield->SetVisAttributes(visSCM3);
  logicaloutShield->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalendShield->SetVisAttributes(visSCM3);
  logicalendShield->SetVisAttributes(G4VisAttributes::Invisible);

//  G4VisAttributes* visSCM4 = new G4VisAttributes(G4Colour(0.,1.,1.));
//  logicall1insu->SetVisAttributes(visSCM4);
  logicall1insu->SetVisAttributes(G4VisAttributes::Invisible);
//  logicall2insu->SetVisAttributes(visSCM4);
  logicall2insu->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM5 = new G4VisAttributes(G4Colour(1.,0.,1.));
//  logicalcoil->SetVisAttributes(visSCM5);
  logicalcoil->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalendCoil->SetVisAttributes(visSCM5);
  logicalendCoil->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM6 = new G4VisAttributes(G4Colour(1.,1.,0.));
//  logicalsupp->SetVisAttributes(visSCM6);
  logicalsupp->SetVisAttributes(G4VisAttributes::Invisible);
//  G4VisAttributes* visSCM7 = new G4VisAttributes(G4Colour(0.,0.,0.));
//  logicalrein->SetVisAttributes(G4VisAttributes::Invisible);
  logicalpipe1->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalpipe2->SetVisAttributes(visSCM2);
  logicalpipe2->SetVisAttributes(G4VisAttributes::Invisible);
  logicalpipe3->SetVisAttributes(G4VisAttributes::Invisible);
//   logicaltub17->SetVisAttributes(visSCM2);
  logicaltub17->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaltub19->SetVisAttributes(visSCM5);
  logicaltub19->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaltub20->SetVisAttributes(visSCM1);
  logicaltub20->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaltub22->SetVisAttributes(visSCM1);
  logicaltub22->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaltub23->SetVisAttributes(visSCM2);
  logicaltub23->SetVisAttributes(G4VisAttributes::Invisible);
//  logicaltub25->SetVisAttributes(visSCM5);
  logicaltub25->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalsub1->SetVisAttributes(visSCM4);
  logicalsub1->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalsub2->SetVisAttributes(visSCM4);
  logicalsub2->SetVisAttributes(G4VisAttributes::Invisible);

//  logicalsub3->SetVisAttributes(visSCM4);
  logicalsub3->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalhole1->SetVisAttributes(visSCM3);
  logicalhole1->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalhole2->SetVisAttributes(visSCM3);
  logicalhole2->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalhole3->SetVisAttributes(visSCM3);
  logicalhole3->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalhole4->SetVisAttributes(visSCM3);
  logicalhole4->SetVisAttributes(G4VisAttributes::Invisible);
//  logicalhole5->SetVisAttributes(visSCM3);
  logicalhole5->SetVisAttributes(G4VisAttributes::Invisible);

  delete aBesG4Geo;
  }
}
