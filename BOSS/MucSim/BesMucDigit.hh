//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Digitization of Muc Hits
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created:  Dec, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

//
// $Id: BesMucDigit.hh,v 1.2 2006/11/06 06:32:56 liangyt Exp $
// GEANT4 tag $Name: MucSim-00-01-03 $

#ifndef BesMucDigit_h
#define BesMucDigit_h 1

#include "BesMucHit.hh"
#include "G4ThreeVector.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"

class BesMucDigit
{
public:
  BesMucDigit();
  ~BesMucDigit();
  //BesMucDigit(const BesMucDigit&);
  //const BesMucDigit$ operator=(const BesMucDigit&);
  //int operator==(const BesMucDigit&) const;

  //inline void* operator new(size_t);
  //inline void operator delete(void*);

  void SetHit(BesMucHit* hit);

  G4int GetTrackIndex() { return m_pHit->GetTrackIndex(); }
  G4int GetPart() { return m_pHit->GetPart(); }
  G4int GetSeg()  { return m_pHit->GetSeg(); }
  G4int GetGap()  { return m_pHit->GetGap(); }

  G4VPhysicalVolume* GetNearestStrip();
  G4int              GetNearestStripNo();
  G4VPhysicalVolume* GetStrip(G4int i);
  G4float            GetStripPos(G4VPhysicalVolume* pvStrip);
  G4int              IsNearest(G4int i);

private:
  BesMucHit* m_pHit;
  G4float    m_Pos;
  G4float    m_Pos2;  //for barrel segment 2
  G4LogicalVolume*   m_lvGap;
  G4String           m_lvGapName;
  G4VPhysicalVolume* m_pvStripDigit;
};

#endif
