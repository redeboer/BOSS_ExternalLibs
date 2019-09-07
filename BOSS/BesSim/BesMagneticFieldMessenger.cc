#include "BesMagneticFieldMessenger.hh"
#include "BesMagneticField.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"

//The standard constructor
BesMagneticFieldMessenger::BesMagneticFieldMessenger(BesMagneticField* pEMfieldSetup):fEMfieldSetup(pEMfieldSetup)
{
	F01detDir= new G4UIdirectory("/field/");
	F01detDir->SetGuidance("Bes MagneticField tracking control.");

	StepperCmd= new G4UIcmdWithAnInteger("/field/SetStepperType",this);
	StepperCmd->SetGuidance("Select stepper type for magnetic field");
	StepperCmd->SetParameterName("choice",true);
	StepperCmd->SetDefaultValue(4);
	StepperCmd->SetRange("choice>=0&&choice<=9");
	
	MinStepCmd= new G4UIcmdWithADoubleAndUnit("/field/SetMinStep",this);
	MinStepCmd->SetGuidance("Please change it carefully,it is better keep it const");
	MinStepCmd->SetParameterName("MinStep",true);
	MinStepCmd->SetDefaultUnit("mm");
	MinStepCmd->SetDefaultValue(0.01);
	MinStepCmd->SetRange("MinStep>=0&&MinStep<=1");
	
	DeltaOneStepCmd= new G4UIcmdWithADoubleAndUnit("/field/SetDeltaOneStep",this);
	DeltaOneStepCmd->SetGuidance("Set DeltaOneStep for tracking");
	DeltaOneStepCmd->SetParameterName("DeltaOneStep",true);
	DeltaOneStepCmd->SetDefaultUnit("mm");
	DeltaOneStepCmd->SetDefaultValue(0.01);
        DeltaOneStepCmd->SetRange("DeltaOneStep>=0");

	DeltaIntersectionCmd=new G4UIcmdWithADoubleAndUnit("/field/SetDeltaIntersection",this);
	DeltaIntersectionCmd->SetGuidance("Set DeltaIntersection for tracking");
	DeltaIntersectionCmd->SetParameterName("DeltaIntersection",true);
	DeltaIntersectionCmd->SetDefaultUnit("mm");
	DeltaIntersectionCmd->SetDefaultValue(0.001);
        DeltaIntersectionCmd->SetRange("DeltaIntersection>=0");

	MinimumEpsilonStepCmd=new G4UIcmdWithADouble("/field/SetMinimumEpsilonStep",this);
	MinimumEpsilonStepCmd->SetGuidance("Set Minimum Epsilon Step for tracking");
	MinimumEpsilonStepCmd->SetParameterName("MinimumEpsilonStep",true);
	MinimumEpsilonStepCmd->SetDefaultValue(5.0e-5);
	MinimumEpsilonStepCmd->SetRange("MinimumEpsilonStep>=0");

	MaximumEpsilonStepCmd=new G4UIcmdWithADouble("/field/SetMaximumEpsilonStep",this);
	MaximumEpsilonStepCmd->SetGuidance("Set Maximum Epsilon Step for tracking");
	MaximumEpsilonStepCmd->SetParameterName("MaximumEpsilonStep",true);
	MaximumEpsilonStepCmd->SetDefaultValue(1.0e-3);
	MaximumEpsilonStepCmd->SetRange("MaximumEpsilonStep>=0");
}

BesMagneticFieldMessenger::~BesMagneticFieldMessenger()
{
	delete MaximumEpsilonStepCmd;
	delete MinimumEpsilonStepCmd;
	delete DeltaIntersectionCmd;
	delete DeltaOneStepCmd;
	delete MinStepCmd;
	delete StepperCmd;
	delete F01detDir;
}
		
void BesMagneticFieldMessenger::SetNewValue( G4UIcommand* command, G4String newvalue)	
{
//Please don't change the order of code ,it will make some parameters useless if you do that.
	if(command==StepperCmd)
	{
		fEMfieldSetup->SetStepperType(StepperCmd->GetNewIntValue(newvalue));
		fEMfieldSetup->CreateStepperAndChordFinder();
	}
	if(command==MinStepCmd)
	{
	        fEMfieldSetup->SetMinStep(MinStepCmd->GetNewDoubleValue(newvalue));
	        fEMfieldSetup->CreateStepperAndChordFinder();
	}
	if(command==DeltaOneStepCmd)
	{
		fEMfieldSetup->SetDeltaOneStep(DeltaOneStepCmd->GetNewDoubleValue(newvalue));
	}
	if(command==DeltaIntersectionCmd)
	{
		fEMfieldSetup->SetDeltaIntersection(DeltaIntersectionCmd->GetNewDoubleValue(newvalue));
	}
	if(command==MinimumEpsilonStepCmd)
	{
		fEMfieldSetup->SetMinimumEpsilonStep(MinimumEpsilonStepCmd->GetNewDoubleValue(newvalue));
	}
	if(command==MaximumEpsilonStepCmd)
	{
		fEMfieldSetup->SetMaximumEpsilonStep(MaximumEpsilonStepCmd->GetNewDoubleValue(newvalue));
	}
	
}
