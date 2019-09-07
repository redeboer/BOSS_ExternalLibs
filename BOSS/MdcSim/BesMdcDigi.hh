//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created: Oct.26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesMdcDigi_h
#define BesMdcDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class BesMdcDigi : public G4VDigi
{
  public:
    BesMdcDigi();
    virtual ~BesMdcDigi();

    BesMdcDigi(const BesMdcDigi&);
    const BesMdcDigi& operator=(const BesMdcDigi&);

    virtual G4int operator==(const BesMdcDigi&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

  //    virtual void Draw();
    virtual void Print();

public:
  
  void SetTrackID  (G4int track)      { trackID = track; };
  void SetLayerNo  (G4int layer)      { layerNo = layer; };  
  void SetCellNo   (G4int cell)       { cellNo =cell;};
  void SetEdep     (G4double de)      { edep = de; };
  void SetDriftT   (G4double time)    { driftT = time;}; 
  
  G4int GetTrackID()    { return trackID; };
  G4int GetLayerNo()    { return layerNo; };
  G4int GetCellNo()     { return cellNo;};
  G4double GetEdep()    { return edep; };      
  G4double GetDriftT()  { return driftT;};      
  
private:
  
  G4int         trackID;
  G4int         layerNo;
  G4int         cellNo;
  G4double      edep;
  G4double      driftT;

};

typedef G4TDigiCollection<BesMdcDigi> BesMdcDigisCollection;

extern G4Allocator<BesMdcDigi> BesMdcDigiAllocator;

inline void* BesMdcDigi::operator new(size_t)
{
  void *aDigi;
  aDigi = (void *) BesMdcDigiAllocator.MallocSingle();
  return aDigi;
}

inline void BesMdcDigi::operator delete(void *aDigi)
{
  BesMdcDigiAllocator.FreeSingle((BesMdcDigi*) aDigi);
}

#endif

