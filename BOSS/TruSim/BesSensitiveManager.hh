//---------------------------------------------------------------------------//
////////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////////---------------------------------------------------------------------------//
////////Description:
////////Author : Dengzy
////
//// ////Created:  Aug, 2004
//// ////Modified:
//// ////Comment:
//// ////---------------------------------------------------------------------------//
//// //// $Id: BesSensitiveManager.hh

#ifndef BesSensitiveManager_hh
#define BesSensitiveManager_hh

#include "BesSensitiveDetector.hh"
#include "G4TrackingManager.hh"
#include "globals.hh"
#include "HepMC/GenEvent.h"

class G4Svc;

class G4VProcess;
class G4Track;
class G4Event;


class BesTruthVertex;
class BesTruthTrack;
// Since G4 lacks a "permanent stack" feature like GEANT3,
// we need to keep track of the complete parentage of particles 
	
class BesTStats {
		
friend class BesSensitiveManager;

public:
  BesTStats() : 
    G4index(-1),
    p4(0),
    time(0),
    trackIndex(-1),
    originVertex(-1),
    savedByDefault(false),
    vertices(0)  {;}
  BesTStats( const G4int index, const CLHEP::HepLorentzVector theP4, G4double theTime )
    : G4index(index),
      p4(theP4),
      time(theTime),
      trackIndex(-1),
      originVertex(-1),
      savedByDefault(false),
      vertices(0)  {;}
  ~BesTStats() {;}
		
  bool operator==( const BesTStats &other ) const { return G4index == other.G4index; }
	
private:	
  G4int G4index;		// Index as in G4Track::fTrackID
  CLHEP::HepLorentzVector p4;		// 4-Momentum at origin
  G4double time;		// Global time at origin
  G4int trackIndex;	// Index into trackList, or -1 if no such
  G4int originVertex;	// Index into vertexList of origin vertex
  bool savedByDefault;  // True if saved to BesTruthTrack by default
		
  // A list of vertices (index into vertexList)
  // associated with an interaction
  // of this track.
  std::vector<int> vertices;
  
};

class BesSensitiveManager {
public:
  BesSensitiveManager():m_trackIndex(-1),m_trackFlag(0),m_count(0) 
  {
    m_hepmcevt = 0; 
    if(m_sensitiveManager) 
      G4Exception("BesSensitiveManager constructed twice.");
    m_sensitiveManager = this ;
  }

  ~BesSensitiveManager() {;}

  static BesSensitiveManager* GetSensitiveManager() {return m_sensitiveManager; }
  
  // Add a new client
  void AddSensitiveDetector( BesSensitiveDetector *detector )
  {
    clients.push_back( detector );
  }
	
  G4int GetCurrentTrackIndex() const { return m_trackIndex; }
  std::vector<BesTruthTrack*>* GetTrackList() 	{return m_trackList;}
  std::vector<BesTruthVertex*>*  GetVertexList() {return m_vertexList;}
  
  // Take care of things at the beginning and end
  // of event, including dumping all required stuff
  // at the end of the event
  void BeginOfTruthEvent(const G4Event*);
  void EndOfTruthEvent(const G4Event*);

  G4int CheckType(const HepMC::GenEvent* hepmcevt);
  void SaveParticlesFromGenerator();
  void SetVertex0(const G4Event*); 
  void UpdatePrimaryTrack(const G4Track*);
  G4bool CheckDecayTrack(const G4Track*);
  void UpdateVertex(BesTStats, const G4Track*);
  G4bool MatchDaughterTrack(const G4Track*);
  void GetDaughterVertexes(BesTruthTrack* pTrack, std::vector<int>* vDau);
  G4bool MatchVertex(G4int vIndex, std::vector<int>* vDau);

  // Take care of things at the beginning and end
  // of tracks
  void BeginOfTrack( const G4Track *track );
  void EndOfTrack( const G4Track *track , G4TrackingManager* );

  // Find out how many tracks and vertices we have
  G4int GetNumberTracks() const { return m_trackList ? m_trackList->size() : 0; }
  G4int GetNumberVertices() const { return m_vertexList ? m_vertexList->size() : 0; }
 
  void ClearEvent();

  void SetLogLevel(G4int level) {m_logLevel = level;}

protected:
  
  static BesSensitiveManager* m_sensitiveManager;
  
  std::vector<BesSensitiveDetector*>::iterator iter;
  std::vector<BesSensitiveDetector*>::iterator iter_end;
  
  // Current track indices
  G4int m_trackIndex;
  G4int m_trackFlag;

  // The following are the BesTruthTrack, BesTruthVertex
  // lists. Don't forget to delete them at the end of the event.
  std::vector<BesTruthTrack*>      *m_trackList;
  std::vector<BesTruthVertex*>     *m_vertexList;
	
  // Here is a list of clients. Note that
  // these objects are decendents of G4VSensitiveDetector
  // and are thus owned by GEANT4. We must not delete
  // them ourselves!
  std::vector<BesSensitiveDetector*> clients;

  std::vector<BesTStats> chain;	// Stats on the decay chain
  
  // Make a new TruthTrack (and TruthVertex if necessary)
  void MakeNewTrack( BesTStats &stat, const G4Track *track );
	
  // Update trackList, vertexList, for a new track, and return the new track's stats
  BesTStats FollowTrack( const G4Track *track );


  // this data member is set for recording trackIndex
  G4int m_count;  

  G4ThreeVector m_pos0;
  G4double  m_t0;

  G4int m_logLevel;
  HepMC::GenEvent* m_hepmcevt;
};

#endif
