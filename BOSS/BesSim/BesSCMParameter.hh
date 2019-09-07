//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Caogf
//Created: June, 2007
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesSCMParameter.hh

#ifndef BesSCMParameter_h
#define BesSCMParameter_h 1

#include "globals.hh"

class BesSCMParameter
{
public:
  BesSCMParameter();
  ~BesSCMParameter();
           
public:
  void  ReadData();
  G4double getInnerCryo(G4int i) { return innerCryo[i]; }
  G4double getL1Adia(G4int i) { return l1Adia[i]; }
  G4double getInnerShield(G4int i) { return innerShield[i]; }
  G4double getL2Adia(G4int i) { return l2Adia[i]; }
  G4double getOuterShield(G4int i) { return outerShield[i]; }
  G4double getL3Adia(G4int i) { return l3Adia[i]; }
  G4double getEndShield(G4int i) { return endShield[i]; }
  G4double getL1insu(G4int i) { return l1insu[i]; }
  G4double getCoil(G4int i) { return coil[i]; }
  G4double getL2insu(G4int i) { return l2insu[i]; }
  G4double getSupp(G4int i) { return supp[i]; }
  G4double getL4Adia(G4int i) { return l4Adia[i]; }
  G4double getEndCoil(G4int i) { return endCoil[i]; }
  G4double getOuterCryo(G4int i) { return outerCryo[i]; }
  G4double getEndCryo(G4int i) { return endCryo[i]; }
  G4double getRein(G4int i) { return rein[i]; }
  G4double getPipe1(G4int i) { return pipe1[i]; }
  G4double getPipe2(G4int i) { return pipe2[i]; }
  G4double getPipe3(G4int i) { return pipe3[i]; }
  G4double getHole(G4int i) { return hole[i]; }

private:
  G4double innerCryo[3];
  G4double l1Adia[3];
  G4double innerShield[3];
  G4double l2Adia[3];
  G4double outerShield[3];
  G4double l3Adia[3];
  G4double endShield[5];
  G4double l1insu[3];
  G4double coil[3];
  G4double l2insu[3];
  G4double supp[3];
  G4double l4Adia[3];
  G4double endCoil[5];
  G4double outerCryo[3];
  G4double endCryo[5];
  G4double rein[5];
  G4double pipe1[6];
  G4double pipe2[6];
  G4double pipe3[6];
  G4double hole[13];
};
#endif
