//------------------------------------------------------------------
//
// ClassName:   G4SvcRunManager
//  
// Description: replacement G4RunManager with extra features
//
// $Id: G4SvcRunManager.h,v 1.4 2007/11/10 02:19:15 dengzy Exp $
// Simulation/G4Sim/G4Svc tag $Name: G4Svc-00-01-52 $

#ifndef G4SvcRunManager_h
#define G4SvcRunManager_h 1

#include "G4RunManager.hh"

class G4SvcRunManager: public G4RunManager {

  friend class G4Svc;

public:

  G4SvcRunManager ();
  virtual ~G4SvcRunManager ();

	G4Event* GenerateEvent(G4int i_event);
	void SimulateEvent(int i);
  void PrintPrimary(G4Event* anEvent);

private:

  void SetLogLevel(int l) { m_logLevel = l; }
  int  GetLogLevel() { return m_logLevel; }

private:
  int m_logLevel;

};

#endif

