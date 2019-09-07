//---------------------------------------------------------------------------//
////////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Author : Dengzy
////Created:  Aug, 2004
////Modified:
////Comment:
////$Id: BesSensitiveDetector.cc

// A pure virtual class that represents a generic Bes sensitive
// Detector. Bes subsystems should inherit from this class when
// implementing their hit scoring.

#include "BesSensitiveDetector.hh"
#include "BesSensitiveManager.hh"
#include "BesTruthTrack.hh"

//
// Constructor
//
BesSensitiveDetector::BesSensitiveDetector( const G4String name)
	: G4VSensitiveDetector(name)
{
	BesSensitiveManager* manager = BesSensitiveManager::GetSensitiveManager();
  manager->AddSensitiveDetector(this);
}

// Ask the sensitive detector manager
//
void BesSensitiveDetector::GetCurrentTrackIndex(G4int& trackIndex, G4int& g4TrackId) const
{
  BesSensitiveManager* manager = BesSensitiveManager::GetSensitiveManager();
  if(manager)
  {
    trackIndex = manager->GetCurrentTrackIndex();
    std::vector<BesTruthTrack*>* trackList = manager->GetTrackList();
    if(trackList)
    {
      G4int size = trackList->size();
      if(size>0)
      {
        for(G4int i=0;i<size;i++)
        {
          if( (*trackList)[i]->GetIndex() == trackIndex )
          {
            g4TrackId = (*trackList)[i]->GetG4TrackId();
            break;
          }
        }
      } 
    }
  }
}








