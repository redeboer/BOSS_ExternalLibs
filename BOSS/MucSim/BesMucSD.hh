//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: Sensitive detector of Muc
//Author:  Youzy      Peking University      mail: youzy@hep.pku.cn
//Created: Nov, 2003
//Modified:
//Comment:
//---------------------------------------------------------------------------//
//
//
// $Id: BesMucSD.hh,v 1.5 2009/08/25 13:33:54 xieyg Exp $
// GEANT4 tag $Name: MucSim-00-01-03 $
//

#ifndef BesMucSD_h
#define BesMucSD_h 1

#include "G4Event.hh"
#include "G4TrajectoryContainer.hh"
#include "BesSensitiveDetector.hh"
#include "BesMucHit.hh"
#include "BesMucConstruction.hh"
#include <vector>
#include "BesMucEfficiency.hh"
#include "BesMucNoise.hh"

using namespace std;

class G4Step;
class G4HCofThisEvent;
class G4Svc;

class BesMucSD : public BesSensitiveDetector
{
public:
  BesMucSD(G4String, BesMucConstruction*);
  ~BesMucSD();
  
  void Initialize(G4HCofThisEvent*);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void EndOfEvent(G4HCofThisEvent*);
  void BeginOfTruthEvent(const G4Event*);
  void EndOfTruthEvent(const G4Event*);
  
private:

  G4Svc* m_G4Svc;
  
  G4VTrajectory* GetTrajFromID(G4int id);
  bool IsChildOf(G4Track *curTrack, G4int primaryG4TrackID);
  
  BesMucHitsCollection* MucHitCollection;
  BesMucHitsCollection* MucHitList;
  G4int m_trackIndex;
  vector<G4int> m_trackIndexes;
  G4int m_prePart, m_preSeg, m_preGap, m_preStrip;
  BesMucConstruction*  detector;
  G4int*   HitID;
  G4double TotEneDeposit;
  G4double DeltaEnergy; 
  
  G4int m_PreviousPrimaryTrackG4Id;
  const G4Event* m_CurEvent;
  G4TrajectoryContainer* m_TrackCon;

  BesMucEfficiency* m_effi;  
  BesMucNoise* m_noise;
  int m_noiseMode;   //0-no; 1-cnt sampling; 2-noise ratio sampling;

};

#endif












