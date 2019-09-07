//-----------------------------------------------------------------------------
// File and Version Information:
// $Id: BgsGentleGenocide.cc,v 1.1 2007/10/12 07:30:20 caogf Exp $
//
// Description:
//	Class BgsGentleGenocide
//
//      A process that prevents the associated particle from being tracked
//      if it has traveled more than the specified number of steps and
//      falls below the specified kinetic energy.
//
//      To fix an apparent "loophole" in hadronic ionization for tritium
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	David Williams
//
// Copyright Information:
//	Copyright (C) 2001         SLAC
//
//-----------------------------------------------------------------------------

#include "BaBar.hh"
#include "BgsGentleGenocide.hh"


//
// Constructor
//
BgsGentleGenocide::BgsGentleGenocide( G4double theMaxKineticEnergy,
			              G4int    theMinNumberSteps,
				      const G4String &name, 
				      G4ProcessType type     )
	: BgsGenocide( name, type ),
	  maxKineticEnergy(theMaxKineticEnergy),
	  minNumberSteps(theMinNumberSteps)
{;}


//
// Destructor
//
BgsGentleGenocide::~BgsGentleGenocide() {;}


//
// Decide what to do at each step
//
G4double BgsGentleGenocide::PostStepGetPhysicalInteractionLength( 
                               const G4Track& track,
                               G4double   previousStepSize,
                               G4ForceCondition* condition )
{
	*condition = NotForced;
	
	//
	// Do nothing if we haven't reached the minimum number of steps
	//	
	if (track.GetCurrentStepNumber() < minNumberSteps) return DBL_MAX;

	//
	// Do nothing if we are above the threshold kinetic energy
	//
	const G4DynamicParticle *particle = track.GetDynamicParticle();
	
	if (particle->GetKineticEnergy() > maxKineticEnergy) return DBL_MAX;
		
	//
	// Die!
	//
	//	const G4ThreeVector &pos = track.GetPosition();
	
	const G4VProcess *creator = track.GetCreatorProcess();
	G4String creatorProcessName = "(none?)";
	if (creator) creatorProcessName = creator->GetProcessName();

	/*
	G4cout << "BgsGentleGenocide: rogue "
	       << particle->GetDefinition()->GetParticleName() 
               << " terminated, created by " 
               << creatorProcessName << G4endl; 

        G4cout << " KE = "
	       << particle->GetKineticEnergy()/keV << " keV, "
               << " global coords (cm): ("
	       << pos.x()/cm << "," 
	       << pos.y()/cm << "," 
	       << pos.z()/cm << ")" << G4endl;
	*/

	*condition = NotForced;
	return 0;
}

