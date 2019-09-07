//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment: Add a new digitizer module for endcap TOF. Liu Yong @ Dec,2008
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizer.hh

#ifndef BesTofDigitizer_h
#define BesTofDigitizer_h 1

#include "G4VDigitizerModule.hh"
#include "BesTofDigi.hh"
#include "BesTofDigitizerBrV1.hh"
#include "BesTofDigitizerEcV1.hh"
#include "BesTofDigitizerBrV2.hh"
#include "BesTofDigitizerEcV2.hh"
#include "BesTofDigitizerEcV3.hh"

//#Matthias
#include "BesTofDigitizerEcV4.hh"

#include "RealizationSvc/RealizationSvc.h"
#include "RealizationSvc/IRealizationSvc.h"

//class BesTofDigitizerMessenger;

class BesTofDigitizer : public G4VDigitizerModule
{
  public:
    BesTofDigitizer(G4String modName);
    ~BesTofDigitizer();
  
    void GroupHits();
    //#Matthias
    void GroupHits_mrpc();
    //G4int GetVersion() {return m_version;}
    //void SetVersion(G4int v) {m_version = v;}
    
    virtual void Digitize();
    
  private:
    BesTofDigitizerBrV1* tofDMBr1;
    BesTofDigitizerEcV1* tofDMEc1;
    BesTofDigitizerBrV2* tofDMBr2;
    BesTofDigitizerEcV2* tofDMEc2;
    BesTofDigitizerEcV3* tofDMEc3;
    BesTofDigitizerEcV4* tofDMEc4;

    BesTofDigitsCollection* m_besTofDigitsCollection; 
    BesTofHitsCollection* m_THC;
  

    //BesTofDigitizerMessenger* m_digiMessenger;
    vector<ScintSingle*>* m_scintGroup;
   
    G4int m_versionBr;
    G4int m_versionEc;

    RealizationSvc* m_RealizationSvc;
};

#endif

