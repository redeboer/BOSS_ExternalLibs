#ifndef BesMagneticFieldMessenger_h
#define BesMagneticFieldMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class BesMagneticField;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

class BesMagneticFieldMessenger:public G4UImessenger
{
	public:
		BesMagneticFieldMessenger(BesMagneticField*);
		~BesMagneticFieldMessenger();
		void SetNewValue(G4UIcommand*,G4String);
		
	private:
		BesMagneticField* fEMfieldSetup;
		G4UIdirectory* F01detDir;
		G4UIcmdWithAnInteger* StepperCmd;
		G4UIcmdWithADouble* MaximumEpsilonStepCmd;
		G4UIcmdWithADouble* MinimumEpsilonStepCmd;
		G4UIcmdWithADoubleAndUnit* MinStepCmd;
		G4UIcmdWithADoubleAndUnit* DeltaIntersectionCmd;
		G4UIcmdWithADoubleAndUnit* DeltaOneStepCmd;
};
#endif
