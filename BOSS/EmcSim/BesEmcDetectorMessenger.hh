//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesEmcDetectorMessenger_h
#define BesEmcDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class BesEmcConstruction;
class BesEmcGeometry;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class BesEmcDetectorMessenger: public G4UImessenger
{
public:
  BesEmcDetectorMessenger(BesEmcConstruction*,BesEmcGeometry* );
  ~BesEmcDetectorMessenger();
  
  void SetNewValue(G4UIcommand*, G4String);
  G4String GetCurrentValue(G4UIcommand *);
private:
  BesEmcConstruction*        BesEmc;
  BesEmcGeometry*            fBesEmcGeometry;

  G4UIdirectory*             BesdetDir;
  G4UIcmdWithAnInteger*      verboseCmd;      
  G4UIcmdWithAString*        CryMaterCmd;
  G4UIcmdWithAString*        CasingMaterCmd;
  G4UIcmdWith3VectorAndUnit* CasingThickCmd;
  G4UIcmdWithADoubleAndUnit* SizeRminCmd;
  G4UIcmdWithAnInteger*      NbPhiCmd;
  G4UIcmdWithAnInteger*      NbThetaCmd;
  G4UIcmdWithAnInteger*      StartIDThetaCmd;
  G4UIcmdWithADoubleAndUnit* LengthCmd;
  G4UIcmdWithADoubleAndUnit* MagFieldCmd;
  G4UIcmdWithoutParameter*   UpdateCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
