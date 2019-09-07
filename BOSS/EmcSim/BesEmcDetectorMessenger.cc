//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesEmcDetectorMessenger.hh"

#include "G4ThreeVector.hh"
#include "BesEmcConstruction.hh"
#include "BesEmcGeometry.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4Material.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcDetectorMessenger::BesEmcDetectorMessenger(
				    BesEmcConstruction* BesDet,
				    BesEmcGeometry*     besEMCGeometry)
:BesEmc(BesDet)
{ 
  fBesEmcGeometry=besEMCGeometry;

  BesdetDir = new G4UIdirectory("/calor/");
  BesdetDir->SetGuidance("Bes calorimeter detector control.");

  verboseCmd = new G4UIcmdWithAnInteger("/calor/verbose",this);
  verboseCmd->SetGuidance("Set Verbose level of calor management category.");
  verboseCmd->SetGuidance(" 0 : Important information");
  verboseCmd->SetGuidance(" 1 : Base information");
  verboseCmd->SetGuidance(" 2 : More...");
  verboseCmd->SetParameterName("level",true);
  verboseCmd->SetRange("level>=0");
  verboseCmd->SetDefaultValue(0);
      
  CryMaterCmd = new G4UIcmdWithAString("/calor/setCryMat",this);
  CryMaterCmd->SetGuidance("Select Material of the Crystal.");
  CryMaterCmd->SetParameterName("choice",true,true);
  //  CryMaterCmd->AvailableForStates(Idle);
  
  CasingMaterCmd = new G4UIcmdWithAString("/calor/setCasMat",this);
  CasingMaterCmd->SetGuidance("Select Material of the Casing.");
  CasingMaterCmd->SetParameterName("choice",true,true);
  //  CasingMaterCmd->AvailableForStates(Idle);
  
  CasingThickCmd = new G4UIcmdWith3VectorAndUnit("/calor/setCasThick",this);
  CasingThickCmd->SetGuidance("Set Thickness of the Casing");
  CasingThickCmd->SetParameterName("TyvekThk","AlThk","MylarThk",true);
  CasingThickCmd->SetRange("Size>=0.");
  CasingThickCmd->SetUnitCategory("Length"); 
  CasingThickCmd->SetDefaultValue(fBesEmcGeometry->GetCasingThickness()/mm);
  CasingThickCmd->SetDefaultUnit("mm");
  //  CasingThickCmd->AvailableForStates(Idle);
  
  SizeRminCmd = new G4UIcmdWithADoubleAndUnit("/calor/setSizeRmin",this);
  SizeRminCmd->SetGuidance("Set Rmin size of the barrel calorimeter");
  SizeRminCmd->SetParameterName("Size",true);
  SizeRminCmd->SetRange("Size>0.");
  SizeRminCmd->SetUnitCategory("Length");
  SizeRminCmd->SetDefaultValue(fBesEmcGeometry->GetBSCRmin()/cm);
  SizeRminCmd->SetDefaultUnit("cm");
  //  SizeRminCmd->AvailableForStates(Idle);

  NbPhiCmd = new G4UIcmdWithAnInteger("/calor/setNbPhi",this);
  NbPhiCmd->SetGuidance("Set number of crystals at phi direction.");
  NbPhiCmd->SetParameterName("NbCrystals",false);
  NbPhiCmd->SetRange("NbCrystals>0 && NbCrystals<=150");
  //  NbPhiCmd->AvailableForStates(Idle);

  NbThetaCmd = new G4UIcmdWithAnInteger("/calor/setNbTheta",this);
  NbThetaCmd->SetGuidance("Set number of crystals at theta direction.");
  NbThetaCmd->SetParameterName("NbCrystals",false);
  NbThetaCmd->SetRange("NbCrystals>0 && NbCrystals<=22");
  //  NbThetaCmd->AvailableForStates(Idle);
  
  StartIDThetaCmd = new G4UIcmdWithAnInteger("/calor/setStartTheta",this);
  StartIDThetaCmd->SetGuidance("Set ID of starting crystals at theta direction.");
  StartIDThetaCmd->SetParameterName("IDCrystal",false);
  StartIDThetaCmd->SetRange("IDCrystal>=0 && IDCrystal<22");
  //  StartIDThetaCmd->AvailableForStates(Idle);

  LengthCmd = new G4UIcmdWithADoubleAndUnit("/calor/setCryLength",this);
  LengthCmd->SetGuidance("Set Length of crystals of barrel calorimeter");
  LengthCmd->SetParameterName("Size",true);
  LengthCmd->SetRange("Size>0.");
  LengthCmd->SetUnitCategory("Length");
  LengthCmd->SetDefaultValue(fBesEmcGeometry->GetBSCCryLength()/cm);
  LengthCmd->SetDefaultUnit("cm");
  //  LengthCmd->AvailableForStates(Idle);

  UpdateCmd = new G4UIcmdWithoutParameter("/calor/update",this);
  UpdateCmd->SetGuidance("Update calorimeter geometry.");
  UpdateCmd->SetGuidance("This command MUST be applied before \"beamOn\" ");
  UpdateCmd->SetGuidance("if you changed geometrical value(s).");
  //  UpdateCmd->AvailableForStates(Idle);
      
  MagFieldCmd = new G4UIcmdWithADoubleAndUnit("/calor/setField",this);  
  MagFieldCmd->SetGuidance("Define magnetic field.");
  MagFieldCmd->SetGuidance("Magnetic field will be in Z direction.");
  MagFieldCmd->SetParameterName("Bz",true);
  MagFieldCmd->SetRange("Bz>=0.");
  MagFieldCmd->SetUnitCategory("Magnetic flux density");
  MagFieldCmd->SetDefaultValue(BesEmc->GetMagField()/tesla);
  MagFieldCmd->SetDefaultUnit("tesla");
  //  MagFieldCmd->AvailableForStates(Idle);  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BesEmcDetectorMessenger::~BesEmcDetectorMessenger()
{
  delete verboseCmd;
  delete LengthCmd; 
  delete NbPhiCmd; delete NbThetaCmd;
  delete StartIDThetaCmd;
  delete CryMaterCmd; delete CasingMaterCmd;
  delete CasingThickCmd;
  delete SizeRminCmd;
  delete UpdateCmd;
  delete MagFieldCmd;
  delete BesdetDir;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BesEmcDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == verboseCmd )
    { 
      BesEmc->SetVerboseLevel(verboseCmd->GetNewIntValue(newValue));
    }
  
  if( command == CryMaterCmd )
    { BesEmc->SetCrystalMaterial(newValue);}
   
  if( command == CasingMaterCmd )
    { BesEmc->SetCasingMaterial(newValue);}
  
  if( command == CasingThickCmd )
    { BesEmc->SetCasingThickness(CasingThickCmd->GetNew3VectorValue(newValue));}
  
  if( command == SizeRminCmd )
    { BesEmc->SetBSCRmin(SizeRminCmd->GetNewDoubleValue(newValue));}

  if( command == NbPhiCmd )
    { BesEmc->SetBSCNbPhi(NbPhiCmd->GetNewIntValue(newValue));}
  
  if( command == NbThetaCmd )
    { BesEmc->SetBSCNbTheta(NbThetaCmd->GetNewIntValue(newValue));}

  if( command == StartIDThetaCmd )
    { BesEmc->SetStartIDTheta(StartIDThetaCmd->GetNewIntValue(newValue));}

  if( command == LengthCmd )
    { BesEmc->SetBSCCrystalLength(LengthCmd->GetNewDoubleValue(newValue));}

  if( command == UpdateCmd )
    { BesEmc->UpdateGeometry(); }

  if( command == MagFieldCmd )
    { BesEmc->SetMagField(MagFieldCmd->GetNewDoubleValue(newValue));}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String BesEmcDetectorMessenger::GetCurrentValue(G4UIcommand * command)
{
  G4String cv;

  if( command == verboseCmd )
    { cv = verboseCmd->ConvertToString(BesEmc->GetVerboseLevel()); }

  if( command == CryMaterCmd )
    { cv = BesEmc->GetCrystalMaterial()->GetName(); }

  if( command == CasingMaterCmd )
    { cv = BesEmc->GetCasingMaterial()->GetName(); }
  
  return cv;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
