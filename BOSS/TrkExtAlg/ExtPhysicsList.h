//
//File: ExtPhysicsList.hh
//Author: Wangll
//Date: 2005.2.22
//

#ifndef ExtPhysicsList_h
#define ExtPhysicsList_h 1

#include "G4VUserPhysicsList.hh"
#include "globals.hh"

class ExtPhysicsList: public G4VUserPhysicsList
{
  public:
      ExtPhysicsList();
      ~ExtPhysicsList();
	  
  protected:
 // Construct particle and physics process
   void ConstructParticle();
   void ConstructProcess();
   void SetCuts();
};
			    
#endif
