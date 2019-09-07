//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to run action 
//Author: Deng Ziyan
//Created: Sep, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesRunActionMessenger_h
#define BesRunActionMessenger_h 1

class BesRunAction;

class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcommand;

#include "G4UImessenger.hh"
#include "globals.hh"

class BesRunActionMessenger: public G4UImessenger
{
  public:
    BesRunActionMessenger(BesRunAction* runAction);
    ~BesRunActionMessenger();
    
  public:
    void SetNewValue(G4UIcommand * command,G4String newValues);

  private:
    BesRunAction * m_runAction;
    G4UIdirectory *             m_runActionDirectory;
    G4UIcmdWithAnInteger *      m_MCTruthCmd;
    G4UIcommand *               m_asciiCmd;
};

#endif





