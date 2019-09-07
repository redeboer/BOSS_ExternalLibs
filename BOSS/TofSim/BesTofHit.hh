//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofHit.hh

#ifndef BesTofHit_h
#define BesTofHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"


class BesTofHit : public G4VHit
{
  public:
    BesTofHit();
    virtual ~BesTofHit();
    
    BesTofHit(const BesTofHit&);
    const BesTofHit& operator=(const BesTofHit&);
   
    void AddHit(const BesTofHit&);

    virtual G4int operator==(const BesTofHit&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    virtual void Draw();	
    virtual void Print();

  public:
    void SetEvent(G4double event) {m_event = event; };
    G4double GetEvent() {return m_event; }
    void SetTrackIndex(G4int trackIndex) { m_trackIndex = trackIndex; };
    void SetG4Index(G4int index) {m_g4Index = index;}
    void SetPartId(G4int partId) {m_partId = partId;}
    void SetScinNb(G4int scinNb) { m_scinNb = scinNb; }
    void SetEdep(G4double edep) { m_edep = edep; }
    void SetStepL(G4double stepL) { m_stepL = stepL;}
    void SetTrackL(G4double length) { m_trackL = length; }
    void SetPos(G4ThreeVector pos) { m_pos = pos; }
    void SetTime(G4double time) { m_time=time;}
    void SetDeltaT(G4double deltaT) { m_deltaT = deltaT;}
    void SetPDirection(G4ThreeVector pDirection) { m_pDirection = pDirection; }
    void SetMomentum(G4ThreeVector momentum) { m_momentum = momentum; }
    //void SetPName(G4String pName) { m_pName = pName; }
    //void SetMaterial(G4Material* myMaterial){m_scinMaterial = myMaterial;}
    void SetCharge(G4double charge) {m_charge = charge;}
    void SetPDGcode(G4int pdgcode){m_pdgcode=pdgcode;}
    // ETF(MRPC)
    void SetModule( G4int module ) { m_scinNb = module; }
    void SetStrip( G4int strip ) { m_strip = strip; }
    void SetIons( G4int ions ) { m_number_of_ions=ions; }
    void SetLocPos( G4ThreeVector locPos ) { m_locPos = locPos; }
    void SetGapNb(G4int gapNb) { m_gapNb = gapNb; }

    G4int GetTrackIndex() { return m_trackIndex; }
    G4int GetG4Index() {return m_g4Index;}
    G4int GetPartId()  { return m_partId; }
    G4int GetScinNb() { return m_scinNb; }
    G4double GetEdep() { return m_edep; }      
    G4double GetStepL() { return m_stepL; }
    G4double GetTrackL() {return m_trackL; }
    G4ThreeVector GetPos() { return m_pos; }	
    G4double GetTime() { return m_time;}
    G4double GetDeltaT() {return m_deltaT;}
    G4ThreeVector GetPDirection() {return m_pDirection;}
    G4ThreeVector GetMomentum() {return m_momentum;}
    //G4String GetPName() {return m_pName; }
    //G4Material* GetMaterial() {return m_scinMaterial; }
    G4double GetCharge() {return m_charge; }
    G4int GetPDGcode(){return m_pdgcode;}    
    // ETF(MRPC)
    G4int GetModule() {return m_scinNb; }
    G4int GetStrip() { return m_strip; }
    G4int GetIons() { return m_number_of_ions; }
    G4ThreeVector GetLocPos() { return m_locPos; }
    G4int GetGapNb() { return m_gapNb; }

    inline void AddEdep( G4double de ) { m_edep += de; } 

  private:
    G4double m_event;
    G4int  m_trackIndex;
    G4int m_g4Index;
    G4int  m_partId;
    G4int  m_scinNb;
    G4int  m_gapNb;
    G4double  m_edep;
    G4double m_stepL;
    G4double m_trackL;
    G4ThreeVector  m_pos;
    G4double  m_time;
    G4double m_deltaT;
    G4ThreeVector m_pDirection;
    G4ThreeVector m_momentum;
    //G4String m_pName;
    //G4Material* m_scinMaterial;
    G4double m_charge;
    G4int m_number_of_ions;
    G4int m_pdgcode;
    G4int m_strip;
    G4ThreeVector m_locPos;
};


typedef G4THitsCollection<BesTofHit> BesTofHitsCollection;

extern G4Allocator<BesTofHit> BesTofHitAllocator;


inline void* BesTofHit::operator new(size_t) {
  void *aHit;
  aHit = (void *) BesTofHitAllocator.MallocSingle();
  return aHit;
}


inline void BesTofHit::operator delete(void *aHit) {
  BesTofHitAllocator.FreeSingle((BesTofHit*) aHit);
}

#endif


