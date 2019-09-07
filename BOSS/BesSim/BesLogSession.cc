//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Save the g4cout and g4cere to logfiles
//Author: Liuhm 
//Created: Jun. 16, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
#include "BesLogSession.hh"
#include "G4UImanager.hh"
#include "fstream"
#include "G4ios.hh"
     
BesLogSession::BesLogSession()
{
  logFile.open("boost.log");
  errFile.open("boost.err");
//  G4UImanager::GetUIpointer()->SetCoutDestination(this);
}

BesLogSession::~BesLogSession()
{
  logFile.close();
  errFile.close(); 
}

G4int BesLogSession::ReceiveG4cout(G4String coutString)
{
  logFile << coutString << std::flush;
  return 0;
}

G4int BesLogSession::ReceiveG4cerr(G4String cerrString)
{
  errFile << cerrString << std::flush;
  return 0;
}
 








