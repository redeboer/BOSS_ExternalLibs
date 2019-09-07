//---------------------------------------------------------------------------//
//////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//////---------------------------------------------------------------------------//
////Author : Dengzy
////Created:  Aug, 2004
////Modified:
////Comment:
////$Id:BesSensitiveDetector.hh

// A pure virtual class that represents a generic Bes sensitive
// Detector. Bes subsystems should inherit from this class when
// implementing their hit scoring.
//
// Subsystems may optionally override the following methods:
//         BeginOfEvent
//         EndOfEvent
//         BeginOfTrack
//         EndOfTrack
// The following utility methods are provided:
//         GetMonteCarloId
//
// Chronological sequence:
//
//       For each event:
//               call BeginOfTruthEvent()
//               For each track:
//                      call BeginOfTrack()
//                      processHits()
//                      call EndOfTrack()
//               call EndOfTruthEvent()
//

#ifndef BesSensitiveDetector_hh
#define BesSensitiveDetector_hh

#include "G4VSensitiveDetector.hh"

class G4Event;
class G4Step;
class G4Track;
class G4TouchableHistory;
class BesSensitiveManager;

class BesSensitiveDetector : public G4VSensitiveDetector
{
  public:
	  
    BesSensitiveDetector( const G4String name);
    virtual ~BesSensitiveDetector() {;}
	
    //
    // These methods are invoked at the beginning and end
    // of each event. Subsystems may wish to override these
    // methods to perform actions at these times.
    //
    // For example, at the end of an event, a subsystem
    // may want to store their lists into the Event.
    //
    virtual void BeginOfTruthEvent(const G4Event*) {;}
    virtual void EndOfTruthEvent(const G4Event*) {;}
	
	
    //
    // These methods are invoked at the beginning and end
    // of each track. Subsystems may wish to override these
    // methods to perform bookkeeping actions for each track.
    //
    // Since many tracks are created in an event, it is
    // important that these methods, if overridden, be
    // very, very quick on average.
    //
    virtual void BeginOfTrack( const G4Track* ) {;}

    virtual void EndOfTrack( const G4Track* ) {;}
	
  protected:

    void GetCurrentTrackIndex(G4int &trackIndex, G4int &g4TrackId) const;
    
};

#endif
