//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to event action
//Author: Deng Ziyan
//Created: Sep, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesRunActionMessenger.hh"
#include "BesRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"

BesRunActionMessenger::BesRunActionMessenger(BesRunAction * runAction)
:m_runAction(runAction)
{
  m_runActionDirectory = new G4UIdirectory("/runAction/");
  m_runActionDirectory->SetGuidance("BESIII run action");
  
  //MCTruth command
  m_MCTruthCmd =  new G4UIcmdWithAnInteger("/runAction/MCTruth",this);
  m_MCTruthCmd->SetGuidance("whether to save MC Truth information.");
  m_MCTruthCmd->SetParameterName("MCTruthFlag",true);
  m_MCTruthCmd->SetRange("MCTruthFlag>=0");
  m_MCTruthCmd->SetDefaultValue(0);

  //ascii I/O command
  /*G4UIparameter* parameter;
  G4bool omittable;

  m_asciiCmd = new G4UIcommand("/runAction/ascii",this);
  m_asciiCmd->SetGuidance("Ascii I/O in BES.");
  m_asciiCmd->SetGuidance("mdcTruFlag, tofTruFlag, emcTruFlag, mucTruFlag,mdcDigiFlag, tofDigiFlag, emcDigiFlag, mucDigiFlag, asciiFileName");
  
  parameter = new G4UIparameter("mdcTruFlag",'i',omittable = true);
  parameter->SetGuidance("whether output mdc Truth information to asciiFile");
  parameter->SetParameterRange("mdcTruFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);

  parameter = new G4UIparameter("tofTruFlag",'i',omittable = true);
  parameter->SetGuidance("whether output tof truth information to asciiFile");
  parameter->SetParameterRange("tofTruFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);


  parameter = new G4UIparameter("emcTruFlag",'i',omittable = true);
  parameter->SetGuidance("whether output emc truth information to asciiFile");
  parameter->SetParameterRange("emcTruFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);

  parameter = new G4UIparameter("mucTruFlag",'i',omittable = true);
  parameter->SetGuidance("whether output muc truth information to asciiFile");
  parameter->SetParameterRange("mucTruFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);


  parameter = new G4UIparameter("mdcDigiFlag",'i',omittable = true);
  parameter->SetGuidance("whether output mdc digi information to asciiFile");
  parameter->SetParameterRange("mdcDigiFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);

  parameter = new G4UIparameter("tofDigiFlag",'i',omittable = true);
  parameter->SetGuidance("whether output tof digi information to asciiFile");
  parameter->SetParameterRange("tofDigiFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);


  parameter = new G4UIparameter("emcDigiFlag",'i',omittable = true);
  parameter->SetGuidance("whether output emc digi information to asciiFile");
  parameter->SetParameterRange("emcDigiFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);

  parameter = new G4UIparameter("mucDigiFlag",'i',omittable = true);
  parameter->SetGuidance("whether output muc digi information to asciiFile");
  parameter->SetParameterRange("mucDigiFlag >= 0");
  parameter->SetDefaultValue(1);
  m_asciiCmd->SetParameter(parameter);

  parameter = new G4UIparameter("asciiFileName",'s',omittable = true);
  parameter->SetGuidance("specify the ascii file name");
  parameter->SetDefaultValue("AsciiDmp_out.dat");
  m_asciiCmd->SetParameter(parameter);
  */

}

BesRunActionMessenger::~BesRunActionMessenger()
{
  delete m_runActionDirectory;
  delete m_MCTruthCmd;
  delete m_asciiCmd;
}

void BesRunActionMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command == m_MCTruthCmd) 
    m_runAction->SetMCTruthFlag(m_MCTruthCmd->GetNewIntValue(newValue));
  
  /*else if( command == m_asciiCmd)
  {
    G4int mdcTruFlag, tofTruFlag, emcTruFlag, mucTruFlag;
    G4int mdcDigiFlag, tofDigiFlag, emcDigiFlag, mucDigiFlag;
    G4String fileName;
    
    std::istrstream is ((char*)newValue.data());
    is >> mdcTruFlag >> tofTruFlag >> emcTruFlag >> mucTruFlag 
       >> mdcDigiFlag >> tofDigiFlag >> emcDigiFlag >> mucDigiFlag
       >> fileName;
    m_runAction->SetMdcTruFlag(mdcTruFlag);
    m_runAction->SetTofTruFlag(tofTruFlag);
    m_runAction->SetEmcTruFlag(emcTruFlag);
    m_runAction->SetMucTruFlag(mucTruFlag);
    
    m_runAction->SetMdcDigiFlag(mdcDigiFlag);
    m_runAction->SetTofDigiFlag(tofDigiFlag);
    m_runAction->SetEmcDigiFlag(emcDigiFlag);
    m_runAction->SetMucDigiFlag(mucDigiFlag);

    m_runAction->SetAsciiFile(fileName);
  }*/
}














