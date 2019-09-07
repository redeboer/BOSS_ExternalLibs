//---------------------------------------------------------------------------//
//////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//////---------------------------------------------------------------------------//
//////Description: operate on every track in simulation
//                 PreUserTrackingAction: at the beginning of a track
//                 PostUserTrackingAction: at the end of a track
//////Author : Dengzy
// 
// ////Created:  Aug, 2004
// ////Modified:
// ////Comment:
// ////---------------------------------------------------------------------------//
// //// $Id:BesTrackingAction.cc

#include "BesTrackingAction.hh"
#include "BesSensitiveManager.hh"
#include "G4VProcess.hh"
#include "G4RunManager.hh"
#include "BesRunAction.hh"

BesTrackingAction::BesTrackingAction(BesRunAction* runAction)
:m_runAction(runAction)
{;}

BesTrackingAction::~BesTrackingAction()
{;}

void BesTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  if(m_runAction->GetMCTruthFlag()!=0)
  {
    BesSensitiveManager* sensitiveManager =  BesSensitiveManager::GetSensitiveManager();
    sensitiveManager->BeginOfTrack(track);
  }
}

void BesTrackingAction::PostUserTrackingAction(const G4Track* track)
{
  if(m_runAction->GetMCTruthFlag()!=0)
  {
    BesSensitiveManager* sensitiveManager =  BesSensitiveManager::GetSensitiveManager();
    sensitiveManager->EndOfTrack(track, fpTrackingManager);
  }
}

