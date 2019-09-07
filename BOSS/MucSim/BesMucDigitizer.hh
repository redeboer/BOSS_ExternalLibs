//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Youzy
//Created: Mar, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesMucDigitizer.hh

#ifndef BesMucDigitizer_h
#define BesMucDigitizer_h 1

#include "GaudiKernel/NTuple.h"

#include "G4VDigitizerModule.hh"
#include "BesMucDigi.hh"
#include "globals.hh"

class G4Svc;

class BesMucDigitizer : public G4VDigitizerModule
{
public:
  BesMucDigitizer(G4String modName);
  ~BesMucDigitizer();
  
  //necessary digi collection object must be constructed and set to 
  //G4DCofThisEvent by StoreDigiCollection protected method.
  virtual void Digitize();
  
private:
  void Initialize();
  void NewDigi(G4int trackIndex, G4int part, G4int seg, G4int layer, G4int strip);
  void OldDigi();
  void Smear();

private:
  BesMucDigisCollection* m_besMucDigisCollection;    

  static const G4int m_nPartMax  = 3;
  static const G4int m_nSegMax   = 8;
  static const G4int m_nGapMax   = 9;
  static const G4int m_nStripMax = 112;
  G4int m_stripID[m_nPartMax][m_nSegMax][m_nGapMax][m_nStripMax];

  G4Svc* m_G4Svc;

  NTuple::Tuple* m_tupleMuc;
  NTuple::Item<double> m_part;
  NTuple::Item<double> m_seg; 
  NTuple::Item<double> m_gap;
  NTuple::Item<double> m_strip;

};


#endif

