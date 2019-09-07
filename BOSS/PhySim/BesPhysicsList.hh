
#ifndef BesPhysicsList_h
#define BesPhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class BesPhysicsList: public G4VModularPhysicsList
{
public:
  BesPhysicsList();
  virtual ~BesPhysicsList();
  
public:
  // SetCuts() 
  virtual void SetCuts();


};


#endif



