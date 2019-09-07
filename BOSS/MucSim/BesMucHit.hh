//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Hit in MUC
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created:  Nov, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

//
// $Id: BesMucHit.hh,v 1.3 2009/08/25 13:33:54 xieyg Exp $
// GEANT4 tag $Name: MucSim-00-01-03 $

#ifndef BesMucHit_h
#define BesMucHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
//#include "BesDetectorConstruction.hh"

class BesMucHit : public G4VHit
{
public:
  BesMucHit();
  BesMucHit(int prt, int seg, int lay, int str, int trId, int trIndex);
  //BesMucHit(BesDetectorConstruction*);
  ~BesMucHit();
  BesMucHit(const BesMucHit&);
  const BesMucHit& operator=(const BesMucHit&);
  int operator==(const BesMucHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
public:
  
  void SetTrackID(G4int track)             { m_TrackID  = track; };
  void SetTrackIndex(G4int index)          { m_TrackIndex = index;};
  void SetPDGCode(G4int pdg)               { m_PDGCode  = pdg; };
  void SetEdep(G4double de)                { m_Edep     = de; };
  void SetEnergy(G4double energy)          { m_Energy   = energy; };
  void SetTime(G4double t)                 { m_Time     = t; };  
  void SetPos(G4ThreeVector xyz)           { m_Pos      = xyz; };
  void SetPosLocal(G4ThreeVector xyzLocal) { m_PosLocal = xyzLocal; };
  void SetDir(G4ThreeVector dir)           { m_Dir      = dir; };
  void SetMomentum(G4ThreeVector momentum) { m_Momentum = momentum; };
  void SetVolume(G4VPhysicalVolume* pv);
  void SetPart(G4int part)                 { m_Part  = part; };
  void SetSeg(G4int seg)                   { m_Seg   = seg; };
  void SetGap(G4int gap)                   { m_Gap   = gap; };
  void SetStrip(G4int strip)               { m_Strip = strip; };

  
  G4int              GetTrackID()    { return m_TrackID; };
  G4int              GetTrackIndex() { return m_TrackIndex;};
  G4int              GetPDGCode()    { return m_PDGCode; };
  G4double           GetEdep()       { return m_Edep; };      
  G4double           GetEnergy()     { return m_Energy; };
  G4double           GetTime()       { return m_Time; };
  G4ThreeVector      GetPos()        { return m_Pos; };
  G4ThreeVector      GetPosLocal()   { return m_PosLocal; };
  G4ThreeVector      GetDir()        { return m_Dir; };
  G4ThreeVector      GetMomentum()   { return m_Momentum; };
  G4VPhysicalVolume* GetVolume()     { return m_Volume; };
  G4String           GetVolumeName() { return m_VolumeName; };
  G4int              GetPart()       { return m_Part; };
  G4int              GetSeg()        { return m_Seg; };
  G4int              GetGap()        { return m_Gap; };
  G4int              GetStrip()      { return m_Strip; };
  G4int              GetPanel()      { return m_Panel; };
  G4int              GetGasChamber() { return m_GasChamber; };

private:
  
  G4int              m_TrackID;     // g4 track id
  G4int              m_TrackIndex;  // MC truth id
  G4int              m_PDGCode;
  G4double           m_Edep;
  G4double           m_Energy;
  G4double           m_Time;
  G4ThreeVector      m_Pos;
  G4ThreeVector      m_PosLocal;
  G4ThreeVector      m_Dir;
  G4ThreeVector      m_Momentum;
  G4VPhysicalVolume* m_Volume;
  G4String           m_VolumeName;
  G4int              m_Part;
  G4int              m_Seg;
  G4int              m_Gap;
  G4int              m_Strip;
  G4int              m_Panel;
  G4int              m_GasChamber;
  //    BesDetectorConstruction* detector;
};

typedef G4THitsCollection<BesMucHit> BesMucHitsCollection;

extern G4Allocator<BesMucHit> BesMucHitAllocator;

inline void* BesMucHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) BesMucHitAllocator.MallocSingle();
  return aHit;
}

inline void BesMucHit::operator delete(void *aHit)
{
  BesMucHitAllocator.FreeSingle((BesMucHit*) aHit);
}

#endif


