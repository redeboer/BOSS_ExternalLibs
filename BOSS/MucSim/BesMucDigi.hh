//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Youzy
//Created: July, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesMucDigi.hh

#ifndef BesMucDigi_h
#define BesMucDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"

class BesMucDigi : public G4VDigi
{
public:
  BesMucDigi();
  virtual ~BesMucDigi();
  
  BesMucDigi(const BesMucDigi&);
  const BesMucDigi& operator=(const BesMucDigi&);
  
  virtual G4int operator==(const BesMucDigi&) const;
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  virtual void Draw();
  virtual void Print();
  
public:
  void SetTrackIndex(G4int index) { m_trackIndex = index; }
  void SetPartId(G4int  partId)   { m_partId  = partId;  }
  void SetSegId(G4int   segId)    { m_segId   = segId;   }
  void SetGapId(G4int   gapId)    { m_gapId   = gapId; }
  void SetStripId(G4int stripId)  { m_stripId = stripId; }
 
  G4int GetTrackIndex() { return m_trackIndex; }
  G4int GetPartId()  { return m_partId;  }
  G4int GetSegId()   { return m_segId;   }
  G4int GetGapId()   { return m_gapId; }
  G4int GetStripId() { return m_stripId; }
    
private:
  G4int m_trackIndex;
  G4int m_partId;
  G4int m_segId;
  G4int m_gapId;
  G4int m_stripId;
};

typedef G4TDigiCollection<BesMucDigi> BesMucDigisCollection;

extern G4Allocator<BesMucDigi> BesMucDigiAllocator;

inline void* BesMucDigi::operator new(size_t)
{
  void *aDigi;
  aDigi = (void *) BesMucDigiAllocator.MallocSingle();
  return aDigi;
}

inline void BesMucDigi::operator delete(void *aDigi)
{
  BesMucDigiAllocator.FreeSingle((BesMucDigi*) aDigi);
}

#endif

