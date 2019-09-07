//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: 
////Author : Dengzy

////Created:  Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
//// $Id:BesMcTruthWriter.hh

#ifndef BesMcTruthWriter_h
#define BesMcTruthWriter_h 1

#include "G4Event.hh"
#include "BesMdcGeoParameter.hh"
#include "McTruth/McParticle.h"

class G4DigiManager;
class IDataProviderSvc;

class BesMcTruthWriter
{
  public:
    BesMcTruthWriter() ;
    ~BesMcTruthWriter();

  public:

    void SaveMcTruth();
    void SaveMcParticle();
    void AddMother(Event::McParticle* , Event::McParticleCol* );
    void SaveDecayMode();
    void SaveMdcTruth();
    void SaveTofTruth();
    void SaveEmcTruth();
    void SaveMucTruth();
    
  private:
    
    G4DigiManager* m_DigiMan;
    BesMdcGeoParameter* mdcGeoPointer;
    IDataProviderSvc* m_evtSvc;
};

#endif
