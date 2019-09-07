//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Dengzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigitizerEcV1.hh

#ifndef BesTofDigitizerEcV1_h
#define BesTofDigitizerEcV1_h 1

#include "BesTofDigitizerV.hh"
#include "BesTofDigi.hh"
#include "globals.hh"
#include "ScintSingle.hh"


class BesTofDigitizerEcV1 : public BesTofDigitizerV
{
  public:
    BesTofDigitizerEcV1();
    ~BesTofDigitizerEcV1();
    
    virtual void Digitize(ScintSingle*, BesTofDigitsCollection* );

    void TofPmtInit();
    void TofPmtAccum(BesTofHit*);
    void Smear(G4int);

  private:
    G4double m_t1st;
    G4double m_r;

    G4double m_bucketPosR;
};


#endif

