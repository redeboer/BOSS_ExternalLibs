//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Description: Persistent store of objects in BOOST
////Author : Dengzy

////Created:  Mar, 2004
////Modified:
////Comment:
////---------------------------------------------------------------------------//
//// $Id:BesAsciiIO.hh

#ifndef BesAsciiIO_h
#define BesAsciiIO_h 1

#include "G4Event.hh"
#include "AsciiDmp/AsciiData.hh"

class BesEventAction;
class G4DigiManager;

class BesAsciiIO
{
  public:
    BesAsciiIO(G4int, G4int, G4int, G4int,
               G4int, G4int ,G4int, G4int, G4String) ;
    ~BesAsciiIO();

  public:
    void SetMdcTruFlag(G4int flag) {m_mdcTruFlag=flag;}
    void SetTofTruFlag(G4int flag) {m_tofTruFlag=flag;}
    void SetEmcTruFlag(G4int flag) {m_emcTruFlag=flag;}
    void SetMucTruFlag(G4int flag) {m_mucTruFlag=flag;}

    void SetMdcDigiFlag(G4int flag) {m_mdcDigiFlag=flag;}
    void SetTofDigiFlag(G4int flag) {m_tofDigiFlag=flag;}
    void SetEmcDigiFlag(G4int flag) {m_emcDigiFlag=flag;}
    void SetMucDigiFlag(G4int flag) {m_mucDigiFlag=flag;}
    void SetAsciiFile(G4String name) {m_asciiFile=name;}

    void SaveAsciiEvents(G4int, const G4Event*);
    
    void SaveDecayMode(EVENT& );
    void SaveTrackTruth( EVENT& );
    void SaveVertexTruth( EVENT& );
    void SaveMdcTruth( EVENT& );
    void SaveTofTruth( EVENT& );
    void SaveEmcTruth( EVENT& );
    void SaveMucTruth( EVENT& );
    
    void SaveMdcDigits( EVENT& );
    void SaveTofDigits( EVENT& );
    void SaveEmcDigits( EVENT& );
    void SaveMucDigits( EVENT& );

  //Below used when do MC tuning. Now only Mdc is implemented. 
  //yuany 2007-10-31
    void SaveHitAsciiEvents(G4int, const G4Event*);

    void SaveDecayMode(HitEVENT& );
    void SaveTrackTruth( HitEVENT& );
    void SaveVertexTruth( HitEVENT& );
    void SaveMdcTruth( HitEVENT& );
    void SaveTofTruth( HitEVENT& ) {};
    void SaveEmcTruth( HitEVENT& ) {};
    void SaveMucTruth( HitEVENT& ) {};
    
    void SaveMdcHits( HitEVENT& );
    void SaveTofHits( HitEVENT& ) {};
    void SaveEmcHits( HitEVENT& ) {};
    void SaveMucHits( HitEVENT& ) {};
    
  private:
    
    G4int m_mdcTruFlag;
    G4int m_mdcDigiFlag;
    G4int m_tofTruFlag;
    G4int m_tofDigiFlag;
    G4int m_emcTruFlag;
    G4int m_emcDigiFlag;
    G4int m_mucTruFlag;
    G4int m_mucDigiFlag;
    G4String m_asciiFile;

    G4DigiManager* m_DigiMan;
};

#endif
