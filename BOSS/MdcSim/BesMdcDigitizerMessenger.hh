//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to BesMdcDigitizer 
//Author: Yuan Ye
//Created: May, 2005
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesMdcDigitizerMessenger_h
#define BesMdcDigitizerMessenger_h 1

class BesMdcDigitizer;

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcommand;

#include "G4UImessenger.hh"
#include "globals.hh"

class BesMdcDigitizerMessenger: public G4UImessenger
{
  public:
    BesMdcDigitizerMessenger(BesMdcDigitizer* );
    ~BesMdcDigitizerMessenger();
    
  public:
    void SetNewValue(G4UIcommand * command,G4String newValues);

  private:
    BesMdcDigitizer *           mdcDigitizer;
    G4UIdirectory *             noiseDirectory;
    G4UIcmdWithAnInteger *      noiseFlag;
    G4UIcmdWithAnInteger *      noiseType;
    G4UIcmdWithADouble *        noiseLevel;

    G4UIdirectory *             digiDirectory;
    G4UIcmdWithAnInteger *      smearFlag;
    G4UIcmdWithADouble *        mdcDRes;

    G4UIdirectory *             effDirectory;
    G4UIcmdWithAnInteger *      effFlag;
    G4UIcommand *               layerEff;
};

#endif





