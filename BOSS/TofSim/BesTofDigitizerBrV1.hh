//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerBrV1.hh

#ifndef BesTofDigitizerBrV1_h
#define BesTofDigitizerBrV1_h 1

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "globals.hh"
#include "ScintSingle.hh"


class BesTofDigitizerBrV1 : public BesTofDigitizerV
{
  public:
    BesTofDigitizerBrV1();
    ~BesTofDigitizerBrV1();
    
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection* );

    void TofPmtInit();
    void TofPmtAccum(BesTofHit*);
    void Smear(G4int);

  private:
    G4double m_t1st;
    G4double m_z;

    G4double m_scinLength;
};


#endif

