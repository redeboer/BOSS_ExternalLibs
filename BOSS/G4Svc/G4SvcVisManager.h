//------------------------------------------------------------------
//
// ClassName:   G4SvcVisManager
//  
// Description: Visalization manager
//
// Author:      Charles Leggett
// 
// Date:        3-8-2001
// 
// $Id: G4SvcVisManager.h,v 1.2 2005/08/17 06:45:46 dengzy Exp $
// Simulation/G4Sim/G4Svc tag $Name: G4Svc-00-01-52 $
//
//------------------------------------------------------------------

#ifndef G4SvcManager_h
#define G4SvcManager_h 1

#ifdef G4VIS_USE

#include "G4VisManager.hh"

class G4SvcVisManager: public G4VisManager {

public:

  G4SvcVisManager ();

private:

  void RegisterGraphicsSystems ();

};

#endif

#endif
