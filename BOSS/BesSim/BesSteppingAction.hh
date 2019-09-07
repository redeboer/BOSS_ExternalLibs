
#ifndef BesSteppingAction_h
#define BesSteppingAction_h 1

#include "G4UserSteppingAction.hh"

class BesSteppingAction : public G4UserSteppingAction
{
  public:
    BesSteppingAction();
   ~BesSteppingAction();

    void UserSteppingAction(const G4Step*);
};

#endif
