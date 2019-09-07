//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Sep 4, 2003
//Modified: He Miao, Jul 18th, 2006
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesEmcHit_h
#define BesEmcHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "Identifier/Identifier.h"
#include <map>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class BesEmcHit : public G4VHit
{
 public:

   BesEmcHit();
  ~BesEmcHit();
   BesEmcHit(const BesEmcHit&);
   const BesEmcHit& operator=(const BesEmcHit&);
   int operator==(const BesEmcHit&) const;

   inline void* operator new(size_t);
   inline void  operator delete(void*);

   void Draw();
   void Print(G4int);
   void Print();
      
 public:
  
  void AddCrystal(G4double de, G4double dl) {TotalEdepCrystal += de; TotalTrackLengthCrystal += dl;};
  
  void SetEdepCrystal(G4double de) {EdepCrystal=de;};
  void SetEdepCasing(G4double de) {EdepCasing=de;};
  void SetTrakCrystal(G4double dl){TrackLengthCrystal=dl;};
  void SetPosCrystal(G4ThreeVector position){PositionCrystal=position;};
  void SetTimeCrystal(G4double t){TimeCrystal=t;};
  void SetNumCrystal(G4int id,G4int numTheta,G4int numPhi){PartId=id;NumTheta=numTheta;NumPhi=numPhi;};
  void SetTrackIndex(G4int index) { trackIndex = index;}
  void SetG4Index(G4int index) { g4Index = index;}
  void SetMomentum(G4ThreeVector momen) {momentum = momen;}
  
  G4double GetTotalEdepCrystal()     { return TotalEdepCrystal; };
  G4double GetTotalTrakCrystal()     { return TotalTrackLengthCrystal; };
  G4double GetEdepCrystal()     { return EdepCrystal; };
  G4double GetEdepCasing()     { return EdepCasing; };
  G4double GetTrakCrystal()     { return TrackLengthCrystal; };
  G4ThreeVector GetPosCrystal() { return PositionCrystal;};
  G4double GetTimeCrystal() {return TimeCrystal;};
  G4int GetPartId() {return PartId;};
  G4int GetNumThetaCrystal() {return NumTheta;};
  G4int GetNumPhiCrystal() {return NumPhi;};
  G4int GetTrackIndex() {return trackIndex;}
  G4int GetG4Index() {return g4Index;}
  G4ThreeVector GetMomentum() {return momentum;}
  
 private:

  G4double TotalEdepCrystal, TotalTrackLengthCrystal;
  G4double EdepCrystal,TrackLengthCrystal;
  G4double EdepCasing;
  G4ThreeVector PositionCrystal;
  G4double TimeCrystal;
  G4int PartId,NumTheta,NumPhi;
  G4int trackIndex;
  G4int g4Index;
  G4ThreeVector momentum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<BesEmcHit> BesEmcHitsCollection;

extern G4Allocator<BesEmcHit> BesEmcHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* BesEmcHit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) BesEmcHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void BesEmcHit::operator delete(void* aHit)
{
  BesEmcHitAllocator.FreeSingle((BesEmcHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class BesEmcTruthHit : public G4VHit
{
  public:
    
    BesEmcTruthHit();
    virtual ~BesEmcTruthHit();
    
    BesEmcTruthHit(const BesEmcTruthHit&);
    const BesEmcTruthHit& operator=(const BesEmcTruthHit&);

    G4int operator==(const BesEmcTruthHit&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    void Print();

  public:
    
    inline void SetIdentify(Identifier id) { m_identify=id; }
    inline void SetTrackIndex(G4int index) { m_trackIndex=index; }
    inline void SetG4TrackId(G4int trackId) { m_g4TrackId=trackId; }
    inline void SetHitEmc(G4int is) { m_hitEmc=is; }
    inline void SetPDGCode(G4int code) {m_PDGCode=code;}
    inline void SetPDGCharge(G4double charge) {m_PDGCharge = charge;}
    inline void SetParticleName(G4String name) {m_particleName=name;}
    inline void SetEDep(G4double de) { m_edep=de; }
    inline void SetTime(G4double time) { m_time=time; }
    inline void SetMomentum(G4ThreeVector p) { m_momentum=p; }
    inline void SetPosition(G4ThreeVector pos) { m_position=pos; }

    Identifier GetIdentify() const { return m_identify; }
    G4int GetTrackIndex() const { return m_trackIndex; }
    G4int GetG4TrackId() const { return m_g4TrackId; }
    G4int GetHitEmc() const { return m_hitEmc; }
    G4int  GetPDGCode() const {return m_PDGCode;}
    G4double GetPDGCharge() const {return m_PDGCharge;}
    G4String GetParticleName() const {return m_particleName;}
    G4double GetEDep() const { return m_edep; }
    G4double GetTime() const { return m_time; }
    G4ThreeVector GetMomentum() const { return m_momentum; }
    G4ThreeVector GetPosition() const { return m_position; }

    std::map<Identifier,G4double>::const_iterator Begin() const;
    std::map<Identifier,G4double>::const_iterator End() const;
    std::map<Identifier,G4double>::const_iterator Find(Identifier) const;

    G4double GetEHit(Identifier);
    void AddEHit(Identifier,G4double);
    void Insert(Identifier,G4double);
    G4int Size() const;
    
  private:
   
    //hit crystal id
    Identifier m_identify;
    //track index
    G4int m_trackIndex;
    //G4 Track Id
    G4int m_g4TrackId;
    //whether hit Emc
    G4int m_hitEmc;
    //particle ID
    G4int m_PDGCode;
    //particle charge;
    G4double m_PDGCharge;
    //particle name
    G4String m_particleName;        
    //total energy deposits
    G4double m_edep;
    G4double m_time;
    G4ThreeVector m_momentum;
    G4ThreeVector m_position;
    std::map<Identifier,G4double> m_hitMap;
        
};

typedef G4THitsCollection<BesEmcTruthHit> BesEmcTruthHitsCollection;

extern G4Allocator<BesEmcTruthHit> BesEmcTruthHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* BesEmcTruthHit::operator new(size_t)
{
  void* aHit;
  aHit = (void*) BesEmcTruthHitAllocator.MallocSingle();
  return aHit;
}

inline void BesEmcTruthHit::operator delete(void* aHit)
{
  BesEmcTruthHitAllocator.FreeSingle((BesEmcTruthHit*) aHit);
}

#endif
