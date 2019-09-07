//------------------------------------------------------------------
//
// ClassName:   G4SvcRunManager
//  
// Description: replacement G4RunManager with extra features
//
// $Id: G4SvcRunManager.cpp,v 1.13 2011/02/15 01:05:25 dengzy Exp $
// Simulation/G4Sim/G4Svc tag $Name: G4Svc-00-01-52 $

#include "G4Svc/G4SvcRunManager.h"
#include "G4Svc/BesHepMCInterface.h"
#include "G4ios.hh"
#include "G4Event.hh"
#include "G4VHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4THitsCollection.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VUserPhysicsList.hh"
#include "G4SDManager.hh"
#include "G4PrimaryParticle.hh"

#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"

#include "GaudiKernel/System.h"
#include "GaudiKernel/IMessageSvc.h"

//#include "FadsKinematics/GeneratorCenter.hh"
#include "G4VUserPrimaryGeneratorAction.hh"

#include <iostream>
#include <string>
#include <vector>

//*************************************************************************//

G4SvcRunManager::G4SvcRunManager(): 
  G4RunManager(),
  m_logLevel(6) {
}

//*************************************************************************//

G4SvcRunManager::~G4SvcRunManager() {
}

G4Event* G4SvcRunManager::GenerateEvent(G4int i_event)
{
  G4Event* anEvent = new G4Event(i_event);
  
  if(!userPrimaryGeneratorAction)
  {
    if(m_logLevel<=4)
	    std::cout<<"calling the hepmc interface"<<std::endl;
    BesHepMCInterface* hepmcInterface = new BesHepMCInterface;
    hepmcInterface->SetLogLevel(m_logLevel);
    HepMC::GenEvent* hepmcEvent = hepmcInterface->GenerateHepMCEvent();
    if(m_logLevel<=4)
      std::cout<<"got the hepmcEvent"<<std::endl;
    hepmcInterface->HepMC2G4(hepmcEvent, anEvent);
    delete hepmcInterface;
    if(m_logLevel<=4)
      PrintPrimary(anEvent);

  }
  else{
    if(storeRandomNumberStatus) {
      G4String fileN = randomNumberStatusDir + "currentEvent.rndm"; 
      CLHEP::HepRandom::saveEngineStatus(fileN);
    }  
    userPrimaryGeneratorAction->GeneratePrimaries(anEvent);
  }
  return anEvent;
}

void G4SvcRunManager::PrintPrimary(G4Event* anEvent)
{
  G4cout<<G4endl<<"primary particles of this event: "<<G4endl;
  G4int n_vertex = anEvent->GetNumberOfPrimaryVertex();
  for( G4int i=0; i<n_vertex; i++ )
  {
    G4cout<<"vertex:"<<i<<G4endl;
    G4PrimaryVertex* primaryVertex = anEvent->GetPrimaryVertex(i);
    G4PrimaryParticle* primaryParticle = primaryVertex->GetPrimary();
    G4cout<<"PDGCode:";
    while( primaryParticle != 0 )
    {
      G4int PDGcode = primaryParticle->GetPDGcode();
      G4cout<<PDGcode<<" ";
      primaryParticle = primaryParticle->GetNext();
    }
    G4cout<<G4endl;
  }
  G4cout<<G4endl;
}
void G4SvcRunManager::SimulateEvent(int i)
{
	//G4StateManager* stateManager = G4StateManager::GetStateManager();
  //stateManager->SetNewState(G4State_EventProc);
  currentEvent = GenerateEvent(i);
  eventManager->ProcessOneEvent(currentEvent);
  AnalyzeEvent(currentEvent);
  //stateManager->SetNewState(G4State_GeomClosed);
  StackPreviousEvent(currentEvent);
  currentEvent = 0;
  if(m_logLevel<=4)
	  std::cout<<"SimulateEvent : done simulating one event "<<std::endl<<std::endl;
}



