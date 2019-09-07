//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: passing parameters to BesMdcDigitizer
//Author: Yuan Ye
//Created: May , 2005
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesMdcDigitizerMessenger.hh"
#include "BesMdcDigitizer.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"
#include <sstream>

BesMdcDigitizerMessenger::BesMdcDigitizerMessenger(BesMdcDigitizer * pointer)
:mdcDigitizer(pointer)
{
  noiseDirectory = new G4UIdirectory("/mdc/noise/");
  noiseDirectory->SetGuidance("BESIII MDC noise control");
  
  //Noise On/Off command
  noiseFlag =  new G4UIcmdWithAnInteger("/mdc/noise/flag",this);
  noiseFlag->SetGuidance("whether to add noise to MdcDigi.");
  noiseFlag->SetParameterName("MdcNoiseFlag",false);
  noiseFlag->SetRange("MdcNoiseFlag>=0");
  //  noiseFlag->SetDefaultValue(0);

  //Noise type command
  noiseType = new G4UIcmdWithAnInteger("/mdc/noise/type",this);
  noiseType->SetGuidance("select noise type.");
  noiseType->SetParameterName("MdcNoiseType",false);
  noiseType->SetRange("MdcNoiseType>=0");
  //  noiseType->SetDefaultValue(3);

  //Noise Level command
  noiseLevel = new G4UIcmdWithADouble("/mdc/noise/level",this);
  noiseLevel->SetGuidance("noise level");
  noiseLevel->SetParameterName("MdcNoiseLevel",false);
  noiseLevel->SetRange("MdcNoiseLevel>=0.");
  //  noiseLevel->SetDefaultValue(0.1);

  digiDirectory = new G4UIdirectory("/mdc/digi/");
  digiDirectory->SetGuidance("BESIII MDC digitizer control");

  //T channel smear On/Off command
  smearFlag =  new G4UIcmdWithAnInteger("/mdc/digi/smearflag",this);
  smearFlag->SetGuidance("whether to do T smear to MdcDigi.");
  smearFlag->SetParameterName("MdcSmearFlag",true);
  smearFlag->SetRange("MdcSmearFlag>=0");
  smearFlag->SetDefaultValue(1);

  //Space resolution command
  mdcDRes = new G4UIcmdWithADouble("/mdc/digi/spaceres",this);
  mdcDRes->SetGuidance("value of space resolution");
  mdcDRes->SetParameterName("MdcDRes",false);
  mdcDRes->SetRange("MdcDRes>0");

  effDirectory = new G4UIdirectory("/mdc/eff/");
  effDirectory->SetGuidance("BESIII MDC wire efficiency control");

  //Eff. source command
  effFlag =  new G4UIcmdWithAnInteger("/mdc/eff/flag",this);
  effFlag->SetGuidance("Get eff. from 0: CalSvc; 1: mac file");
  effFlag->SetParameterName("MdcEffFlag",false);
  effFlag->SetRange("MdcEffFlag>=0");
  //  effFlag->SetDefaultValue(0);

  G4UIparameter* parameter;
  G4bool omittable;

  //Eff. command
  layerEff = new G4UIcommand("/mdc/eff/eff",this);
  layerEff->SetGuidance("Set eff. of MDC layers");

  parameter = new G4UIparameter("layer",'i',omittable = false);
  parameter->SetGuidance("Which layer to be modified");
  parameter->SetParameterRange("layer >= -1 && layer < 43");
  parameter->SetDefaultValue(-1);
  layerEff->SetParameter(parameter);

  parameter = new G4UIparameter("eff",'d',omittable = false);
  parameter->SetGuidance("Eff. value");
  parameter->SetParameterRange("eff >= 0. && eff <=1.");
  parameter->SetDefaultValue(1.);
  layerEff->SetParameter(parameter);
}

BesMdcDigitizerMessenger::~BesMdcDigitizerMessenger()
{
  delete noiseDirectory;
  delete noiseFlag;
  delete noiseType;
  delete noiseLevel;
  delete digiDirectory;
  delete smearFlag;
  delete mdcDRes;
  delete effDirectory;
  delete effFlag;
  delete layerEff;
}

void BesMdcDigitizerMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command == noiseFlag) {
    mdcDigitizer->SetNoiseFlag(noiseFlag->GetNewIntValue(newValue));
  }else if(command==noiseType){
    mdcDigitizer->SetNoiseType(noiseType->GetNewIntValue(newValue));
  }else if(command==noiseLevel){
    mdcDigitizer->SetNoiseLevel(noiseLevel->GetNewDoubleValue(newValue));  
  }else if(command==smearFlag){
    mdcDigitizer->SetSmearFlag(smearFlag->GetNewIntValue(newValue));
  }else if(command==mdcDRes){
    mdcDigitizer->SetMdcDRes(mdcDRes->GetNewDoubleValue(newValue));
  }else if(command==effFlag){
    mdcDigitizer->SetEffFlag(effFlag->GetNewIntValue(newValue));
  }else if(command==layerEff){
    G4int layer;
    G4double eff;
    std::istringstream is ((char*)newValue.data());
    is >> layer >> eff;
    mdcDigitizer->SetEff(layer, eff);
  }
}














