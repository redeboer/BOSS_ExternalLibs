#include "BaBar.hh"
#include "BgsGenocide.hh"

//
// Constructor
//
BgsGenocide::BgsGenocide( const G4String &name, G4ProcessType type )
	: G4VProcess( name, type )
{;}


//
// Destructor
//
BgsGenocide::~BgsGenocide() {;}


//
// Kill Kill Kill!!!
//
G4double BgsGenocide::PostStepGetPhysicalInteractionLength( 
                               const G4Track& track,
                               G4double   previousStepSize,
                               G4ForceCondition* condition )
{
	*condition = NotForced;
	return 0;
}


//
// Our victims vanish without a trace
//
G4VParticleChange *BgsGenocide::PostStepDoIt( const G4Track &track,
                                              const G4Step &step ) 
{
	pParticleChange->Initialize(track);

	pParticleChange->ProposeTrackStatus( fStopAndKill );
	pParticleChange->SetNumberOfSecondaries( 0 );
	pParticleChange->ProposeLocalEnergyDeposit( 0 );
	ClearNumberOfInteractionLengthLeft();

	return pParticleChange;
}
