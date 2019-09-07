//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oreiented Simulation Tool                    //
//---------------------------------------------------------------------------//
//Descpirtion: EMC detector 
//Author: Fu Chengdong
//Created: Dec 15, 2003
//Comment:
//---------------------------------------------------------------------------//
//
#ifndef BesEmcDigitization_h
#define BesEmcDigitization_h 1

#include "globals.hh"

class G4Event;
class BesEmcDigitization
{
public:
  BesEmcDigitization();
  ~BesEmcDigitization();

public:
  G4bool Digitize(const G4Event*, G4double);
  G4double GetEmcSignal(G4int, G4int);
  G4int GetNSignal(){return fNCrystalSignal;};//amount of crystals
                                              // which has signal 
  G4double* GetESignal(){return fEnergySignal;};
  G4int* GetNThetaSignal(){return fNThetaSignal;};
  G4int* GetNPhiSignal(){return fNPhiSignal;};
  void Print();

private:          //memory size needed optimizing
  G4int            calorimeterCollID;
  G4int            fNHits;
  G4double         fTotECrystal;
  G4double         fTotLCrystal;
  G4double         fSingleECrystal[50][150];
  G4int            fNCrystalSignal;
  G4double         *fEnergySignal;
  G4int            *fNThetaSignal;
  G4int            *fNPhiSignal;

};
#endif
