#ifndef BesRootIO_h
#define BesRootIO_h 1


#include "RootEventData/TMcHitTof.h"    
#include "RootEventData/TMcHitMdc.h"    
#include "RootEventData/TMcHitEvent.h"  
#include "RootEventData/TDigiEvent.h"
#include "RootEventData/TEmcDigi.h"
#include "G4Event.hh"
#include "TFile.h"
#include "TTree.h"

class BesEventAction;
class G4DigiManager;

class BesRootIO
{
  public:
    BesRootIO(G4int, G4int, G4int, G4int,
	         G4int, G4int ,G4int, G4int, G4String);
    
    TFile *f;
    TTree *HitTree;
      
    ~BesRootIO();

  public:
    void SetMdcDigiFlag(G4int flag) {m_mdcDigiFlag=flag;}
    
    void SaveEmcDigiRootEvent();
    void SaveHitRootEvent(G4int, const G4Event*);
    void SaveMdcHitRoot();  
    void SaveTofHitRoot();  
  private:
    G4int m_mdcTruFlag;
    G4int m_mdcDigiFlag;
    G4int m_tofTruFlag;
    G4int m_tofDigiFlag;
    G4int m_emcTruFlag;
    G4int m_emcDigiFlag;
    G4int m_mucTruFlag;
    G4int m_mucDigiFlag;
    G4String m_rootFile;


    Int_t m_trackIndex;

    TMcHitEvent* m_TMcHitEvent;
    TMcHitTof*   m_TMcHitTof;
    TMcHitMdc*   m_TMcHitMdc;
    TEmcDigi*    m_TMmcDigi;
    
    G4DigiManager* m_DigiMan;
};

#endif
