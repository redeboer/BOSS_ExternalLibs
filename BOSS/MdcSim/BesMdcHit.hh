//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Track type hit class for BESIII MDC
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: 4 Dec, 2003
//Modified:
//Comment: 
//---------------------------------------------------------------------------//

#ifndef BesMdcHit_h
#define BesMdcHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class BesMdcHit : public G4VHit
{
public:
  
  BesMdcHit();
  ~BesMdcHit();
  BesMdcHit(const BesMdcHit&);
  const BesMdcHit& operator=(const BesMdcHit&);
  int operator==(const BesMdcHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
public:
  void SetTrackID    (G4int track)      { trackID = track; };
  void SetLayerNo    (G4int layer)      { layerNo = layer; };  
  void SetCellNo     (G4int cell)       { cellNo =cell;};
  void SetEdep       (G4double de)      { edep = de; };
  void SetPos        (G4ThreeVector xyz){ pos = xyz; };
  void SetDriftD     (G4double distance){ driftD = distance;}; 
  void SetDriftT     (G4double time)    { driftT = time;}; 
  void SetGlobalT    (G4double time)    { globalT = time;};
  void SetTheta      (G4double angle)   { theta = angle;};
  void SetEnterAngle (G4double angle)   { enterAngle = angle;};
  void SetPosFlag    (G4int flag)       { posFlag = flag;};

  G4int GetTrackID()       { return trackID; };
  G4int GetLayerNo()       { return layerNo; };
  G4int GetCellNo()        { return cellNo;};
  G4double GetEdep()       { return edep; };      
  G4ThreeVector GetPos()   { return pos; };
  G4double GetDriftD()     { return driftD;}; 
  G4double GetDriftT()     { return driftT;};      
  G4double GetGlobalT()    { return globalT;};
  G4double GetTheta()      { return theta;};
  G4double GetEnterAngle() { return enterAngle;};
  G4int GetPosFlag()       { return posFlag;};

private:
  G4int         trackID;
  G4int         layerNo;
  G4int         cellNo;
  G4double      edep;
  G4ThreeVector pos;
  G4double      driftD;
  G4double      driftT;
  G4double      globalT;
  G4double      theta;
  G4double      enterAngle;
  G4int         posFlag;      
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<BesMdcHit> BesMdcHitsCollection;

extern G4Allocator<BesMdcHit> BesMdcHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* BesMdcHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) BesMdcHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void BesMdcHit::operator delete(void *aHit)
{
  BesMdcHitAllocator.FreeSingle((BesMdcHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


