#ifndef GAUDIKERNEL_IG4SVC_H
#define GAUDIKERNEL_IG4SVC_H

// Include Files
#include "GaudiKernel/IService.h"

class G4RunManager;
class G4UImanager;
class G4Event;
class G4Run;
class G4VUserDetectorConstruction;
class G4VUserPhysicsList;
class G4UserRunAction;
class G4UserEventAction;
class G4VUserPrimaryGeneratorAction;
class G4UserStackingAction;
class G4UserTrackingAction;
class G4UserSteppingAction;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IG4Svc("IG4Svc", 1 , 0); 

/* @class IG4Svc IG4Svc.h GaudiKernel/IG4Svc.h

    This class is an interface to the G4Svc.

    @author Charles Leggett

*/
class IG4Svc : virtual public IService {

public:
  // Typedefs for container type

  virtual   G4RunManager* runMgr() = 0;
  virtual   G4UImanager*  uiMgr() = 0;
  virtual   void          StartUISession() = 0;

  virtual const G4Event*  GetCurrentEvent() const = 0;
  virtual const G4Run*    GetCurrentRun() const = 0;

  virtual void SetUserInitialization(G4VUserDetectorConstruction*) = 0;
  virtual void SetUserInitialization(G4VUserPhysicsList*) = 0;

  virtual void SetUserAction(G4UserRunAction *) = 0;
  virtual void SetUserAction(G4UserEventAction *) = 0;
  virtual void SetUserAction(G4VUserPrimaryGeneratorAction*) = 0;
  virtual void SetUserAction(G4UserStackingAction*) = 0;
  virtual void SetUserAction(G4UserTrackingAction*) = 0;
  virtual void SetUserAction(G4UserSteppingAction*) = 0;

  //virtual void NoG4GeomInit() = 0;
  //virtual void SimulateG4Event() = 0;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IG4Svc; }

  virtual void G4Init() = 0;
private:
};

#endif

