//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Save the g4cout and g4cere to logfiles
//Author: Liuhm 
//Created: Jun. 16, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#ifndef BesLogSession_H 
#define BesLogSession_H 1

#include "G4UIsession.hh"
#include "fstream"

class BesLogSession : public G4UIsession
{
  public:

  BesLogSession();
  ~BesLogSession();  
  G4int ReceiveG4cout(G4String coutString);
  G4int ReceiveG4cerr(G4String cerrString);

  private:

  std::ofstream logFile;
  std::ofstream errFile;
};

#endif




