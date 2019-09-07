//-----------------------------------------------------------------------------
// File and Version Information:
// $Id: BgsGenocide.hh,v 1.1 2007/10/12 07:30:20 caogf Exp $
//
// Description:
//	Class BgsGenocide
//
// A process that prevents the associated particle from being tracked.
// Appropriate, for example, for neutrinos
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
// Created:
// Modification history:
//
//-----------------------------------------------------------------------------

#ifndef BgsGenocide_hh
#define BgsGenocide_hh

#include "globals.hh"
#include "G4VProcess.hh"

class BgsGenocide : public G4VProcess {
	public:
	BgsGenocide( const G4String &name="BgsGenocide",
		     G4ProcessType type=fUserDefined     );
	virtual ~BgsGenocide();
	
	//
	// Derived methods
	//
        virtual G4double
        PostStepGetPhysicalInteractionLength( const G4Track& track,
        				      G4double   previousStepSize,
        				      G4ForceCondition* condition );

        virtual G4VParticleChange* PostStepDoIt( const G4Track &track,
        					 const G4Step &step );

        virtual G4double
        AlongStepGetPhysicalInteractionLength( const G4Track&,
        				       G4double  previousStepSize,
        				       G4double  currentMinimumStep,
        				       G4double& currentSafety,
        				       G4GPILSelection* selection )
          { return -1.0; }

        virtual G4VParticleChange* AlongStepDoIt( const G4Track &track,
        					  const G4Step &step )
          { return 0; }
 
        virtual G4double
        AtRestGetPhysicalInteractionLength( const G4Track &track,
        					  G4ForceCondition *force )
          { return -1.0; }

        virtual G4VParticleChange*
        AtRestDoIt( const G4Track &track, const G4Step &step )
          { return 0; }

};


#endif
