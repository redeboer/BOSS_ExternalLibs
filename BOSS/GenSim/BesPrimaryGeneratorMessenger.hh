//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to Generator TESTER and GENBES
//Author: Liuhm
//Created: Jun. 5, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesPrimaryGeneratorMessenger_h
#define BesPrimaryGeneratorMessenger_h 1

class BesPrimaryGeneratorAction;

class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4ParticleTable;

#include "G4UImessenger.hh"
#include "globals.hh"

class BesPrimaryGeneratorMessenger: public G4UImessenger
{
  public:
    BesPrimaryGeneratorMessenger(BesPrimaryGeneratorAction* mpga);
    ~BesPrimaryGeneratorMessenger();
    
  public:
    void SetNewValue(G4UIcommand * command,G4String newValues);
    G4String GetCurrentValue(G4UIcommand * command);

  private:
    BesPrimaryGeneratorAction * myAction;
    G4ParticleTable * particleTable;    
  private: //commands
    G4UIdirectory *             generatorDirectory;
    G4UIcmdWithAString *        gennamCmd;
    G4UIcmdWithAString *        genbesCmd;
    G4UIcommand*                testerCmd;
    G4UIcommand*                cosmicCmd;
};

#endif





