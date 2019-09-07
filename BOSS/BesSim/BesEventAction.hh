
#ifndef BesEventAction_h
#define BesEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"

class BesRunAction;
class G4DigiManager;
//class IDataProviderSvc;

class BesEventAction : public G4UserEventAction
{
  public:
    BesEventAction(BesRunAction*);
    virtual ~BesEventAction();

  public:
    virtual void   BeginOfEventAction(const G4Event*);
    virtual void   EndOfEventAction(const G4Event*);
    
  private:
    
    BesRunAction* m_runAction;
            
    G4DigiManager* m_DM;    
    // Reference to RealizationSvc
    RealizationSvc* m_RealizationSvc;
};


#endif

    
