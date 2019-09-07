//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: Persistent store of objects in BOOST
////Author : Yuan Ye

////Created:  Dec, 2007
////Modified:
////Comment:
////---------------------------------------------------------------------------//
//// $Id:BesTuningIO.hh

#ifndef BesTuningIO_h
#define BesTuningIO_h 1

#include "AsciiDmp/AsciiData.hh"
#include <iostream> 
#include <fstream>
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>

class G4DigiManager;

class BesTuningIO
{
  public:
    BesTuningIO(std::vector<std::string>) ;
    ~BesTuningIO();

  public:

    void GetNextEvents(void);

    void GetMdcHits(void);
    void GetTofHits(void) {};
    void GetEmcDigi(void);
    void GetMucHits(void) {};
    
 
   TFile *f;
   TTree *HitTree;     
   TChain *HitChain;     
   void GetRootEvent(int evtID);
   void GetMdcRootHits();
   void GetTofRootHits();
   void GetEmcRootDigi();
    
  private:

   std::vector<std::string> m_tuningFile;
    std::ifstream* m_inputFileStream;
    FRMTVERSION m_version;		// version numbers
    HitEVENT* m_evt;
    G4DigiManager* m_DigiMan;

    const TMcHitEvent* m_TMcHitEvent;
    const TMcHitTof*   m_TMcHitTof;
    const TMcHitMdc*   m_TMcHitMdc;
    const TMcDigiEmc*  m_TMcDigiEmc;
    
  private:
     G4int   trackIndex ;
     G4int   layerNo    ;
     G4int   cellNo     ;
     G4double posX       ;
     G4double posY       ;
     G4double posZ       ;
     G4double energyDeposit ;
     G4double driftDistance ;
     G4double globalT       ;
     G4double theta         ;
     G4double enterAngle    ;
     G4double posFlag       ;

};

#endif
