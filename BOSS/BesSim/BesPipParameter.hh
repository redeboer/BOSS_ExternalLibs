//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Caogf
//Created: June, 2007
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//$ID: BesPipParameter.hh

#ifndef BesPipParameter_h
#define BesPipParameter_h 1

#include "globals.hh"

class BesPipParameter
{
public:
  BesPipParameter();
  ~BesPipParameter();

public:
  void  ReadData();
  G4double getGoldLayer(G4int i) { return goldLayer[i]; }
  G4double getInnerBe(G4int i) { return innerBe[i]; }
  G4double getOilLayer(G4int i) { return oilLayer[i]; }
  G4double getOuterBe(G4int i) { return outerBe[i]; }

  G4double getInnerBeSide(G4int i) { return innerBeSide[i]; }
  G4double getInnerAl(G4int i) { return innerAl[i]; }
  G4double getAlRing(G4int i) { return AlRing[i]; }
  G4double getOuterAl(G4int i) { return outerAl[i]; }
  G4double getAgLayer(G4int i) { return AgLayer[i]; }
  G4double getCuLayer(G4int i) { return CuLayer[i]; }
  G4double getAlCover(G4int i) { return AlCover[i]; }

private:
  G4double goldLayer[3];
  G4double innerBe[3];
  G4double oilLayer[3];
  G4double outerBe[3];

  G4double innerBeSide[5];
  G4double innerAl[5];
  G4double AlRing[5];
  G4double outerAl[5];
  G4double AgLayer[5];
  G4double CuLayer[5];
  G4double AlCover[5];
};
#endif
