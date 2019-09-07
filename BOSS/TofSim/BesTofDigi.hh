//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigi.hh

#ifndef BesTofDigi_h
#define BesTofDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"

class BesTofDigi : public G4VDigi
{
  public:
    BesTofDigi();
    virtual ~BesTofDigi();

    BesTofDigi(const BesTofDigi&);
    const BesTofDigi& operator=(const BesTofDigi&);

    virtual G4int operator==(const BesTofDigi&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    virtual void Draw();
    virtual void Print();

  public:
    void SetTrackIndex(G4int index) { m_trackIndex = index; }
    void SetPartId(G4int partId)    { m_partId  = partId;   }
  // Scintillator
    void SetScinNb(G4int scinNb)    { m_scinNb  = scinNb;   }
    void SetForwADC(G4double ADC)   { m_forwADC = ADC;      }
    void SetBackADC(G4double ADC)   { m_backADC = ADC;      }
    void SetForwTDC(G4double TDC)   { m_forwTDC = TDC;      }
    void SetBackTDC(G4double TDC)   { m_backTDC = TDC;      }
  // MRPC
    void SetModule(G4int module)    { m_scinNb  = module;   }
    void SetStrip(G4int strip)      { m_strip   = strip;    }
    void SetForwT1(G4double t1)     { m_forwTDC = t1;       }
    void SetBackT1(G4double t1)     { m_backTDC = t1;       }
    void SetForwT2(G4double t2)     { m_forwADC = t2;       }
    void SetBackT2(G4double t2)     { m_backADC = t2;       }

    G4int GetTrackIndex()           { return m_trackIndex;  }
    G4int GetPartId()               { return m_partId;      }
  // Scintillator
    G4int GetScinNb()               { return m_scinNb;      }
    G4double GetForwADC()           { return m_forwADC;     }
    G4double GetBackADC()           { return m_backADC;     }
    G4double GetForwTDC()           { return m_forwTDC;     }
    G4double GetBackTDC()           { return m_backTDC;     }
  // MRPC
    G4int GetModule()               { return m_scinNb;      }
    G4int GetStrip()                { return m_strip;       }
    G4double GetForwT1()            { return m_forwTDC;     }
    G4double GetBackT1()            { return m_backTDC;     }
    G4double GetForwT2()            { return m_forwADC;     }
    G4double GetBackT2()            { return m_backADC;     }

  private:
    G4int  m_trackIndex;
    G4int  m_partId;
    G4int  m_scinNb;
    G4int  m_strip;
    G4double  m_forwADC;
    G4double  m_backADC;
    G4double  m_forwTDC;
    G4double  m_backTDC;
};

typedef G4TDigiCollection<BesTofDigi> BesTofDigitsCollection;

extern G4Allocator<BesTofDigi> BesTofDigiAllocator;

inline void* BesTofDigi::operator new(size_t)
{
  void *aDigi;
  aDigi = (void *) BesTofDigiAllocator.MallocSingle();
  return aDigi;
}

inline void BesTofDigi::operator delete(void *aDigi)
{
  BesTofDigiAllocator.FreeSingle((BesTofDigi*) aDigi);
}

#endif

