//-----------------------------------------------------------------------------
// File and Version Information:
// $Id: BgsGentleGenocide.hh,v 1.1 2007/10/12 07:30:20 caogf Exp $
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

#ifndef BgsGentleGenocide_hh
#define BgsGentleGenocide_hh

#include "BgsGenocide.hh"

class BgsGentleGenocide : public BgsGenocide {
	public:
	BgsGentleGenocide( G4double theMaxKineticEnergy,
			   G4int theMinNumberSteps,
	                   const G4String &name="BgsGentleGenocide",
		           G4ProcessType type=fUserDefined           );
	virtual ~BgsGentleGenocide();

	//
	// Derived methods
	//
        virtual G4double
        PostStepGetPhysicalInteractionLength( const G4Track& track,
        				      G4double   previousStepSize,
        				      G4ForceCondition* condition );

	protected:
	G4double maxKineticEnergy;
	G4int minNumberSteps;
};


#endif
