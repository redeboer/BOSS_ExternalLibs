//---------------------------------------------------------------------------//
//////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//////---------------------------------------------------------------------------//
//////Description: operation at the beginning and the end of a track
//////Author : Dengzy
// 
// ////Created:  Aug, 2004
// ////Modified:
// ////Comment:
// ////---------------------------------------------------------------------------//
// //// $Id:BesTrackingAction.hh

#ifndef BesTrackingAction_h
#define BesTrackingAction_h 1

#include "G4UserTrackingAction.hh"
class BesRunAction;

class BesTrackingAction : public G4UserTrackingAction
{
  public:
    BesTrackingAction(BesRunAction*);
    ~BesTrackingAction() ;

    void PreUserTrackingAction(const G4Track *track);
    void PostUserTrackingAction(const G4Track *track);

  private:
    BesRunAction* m_runAction;    
};

#endif

