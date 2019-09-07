//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Hemiao
//Created: Sep, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesEmcDigi.hh

#ifndef BesEmcDigi_h
#define BesEmcDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "BesEmcWaveform.hh"

class BesEmcDigi : public G4VDigi
{
  public:
    BesEmcDigi();
    virtual ~BesEmcDigi();

    BesEmcDigi(const BesEmcDigi&);
    const BesEmcDigi& operator=(const BesEmcDigi&);

    virtual BesEmcDigi& operator+=(const BesEmcDigi&);
    virtual G4int operator==(const BesEmcDigi&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    virtual void Draw();
    virtual void Print();

  public:
    void MakeWaveform();

  public:
    void SetPartId(G4int id) { m_partId = id; };
    void SetThetaNb(G4int nTheta) { m_nTheta = nTheta; };
    void SetPhiNb(G4int nPhi) { m_nPhi = nPhi; };
    void SetEnergy(G4double energy) { m_energy = energy; };
    void SetTime(G4double time) { m_time = time;};
    void SetTrackIndex(G4int index){ m_trackIndex = index; };
    void SetWaveform(BesEmcWaveform* wave) { digiWaveform = wave; };
 
    G4int GetPartId() {return m_partId;};
    G4int GetThetaNb() { return m_nTheta;};
    G4int GetPhiNb() { return m_nPhi; };
    G4double GetEnergy() { return m_energy; };
    G4double GetTime(){ return m_time; };
    G4int GetTrackIndex(){ return m_trackIndex; };
    BesEmcWaveform* GetWaveform(){ return digiWaveform; };

  private:
    BesEmcWaveform* digiWaveform;
    
  private:
    G4int  m_partId;
    G4int  m_nTheta;
    G4int  m_nPhi;
    G4double  m_energy;
    G4double  m_time;
    G4int m_trackIndex;
};

typedef G4TDigiCollection<BesEmcDigi> BesEmcDigitsCollection;

extern G4Allocator<BesEmcDigi> BesEmcDigiAllocator;

inline void* BesEmcDigi::operator new(size_t)
{
  void *aDigi;
  aDigi = (void *) BesEmcDigiAllocator.MallocSingle();
  return aDigi;
}

inline void BesEmcDigi::operator delete(void *aDigi)
{
  BesEmcDigiAllocator.FreeSingle((BesEmcDigi*) aDigi);
}

#endif

