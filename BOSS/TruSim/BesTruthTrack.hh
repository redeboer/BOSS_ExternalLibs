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
//// //// $Id:BesTruthTrack.hh

#ifndef BesTruthTrack_h
#define BesTruthTrack_h 1

#include <iostream>
#include <vector>
using namespace std;
#include "CLHEP/Vector/LorentzVector.h"
#include "BesTruthVertex.hh"

using namespace CLHEP;

class BesTruthTrack
{
  public:
    BesTruthTrack();
    ~BesTruthTrack() {};

    friend ostream &operator<<(ostream &, const BesTruthTrack&);
    friend ostream &operator<<(ostream &, const BesTruthTrack*);
    
    enum { unassigned = -1 };

    //return four momentum at origin
    HepLorentzVector GetP4() const { return m_fourMomentum; }
    void SetP4( const HepLorentzVector &p4 ) { m_fourMomentum = p4; }
    
    //return particle ID (PDG)
    G4int  GetPDGCode() const {return m_PDGCode;}
    void SetPDGCode(G4int code) {m_PDGCode=code;}

    //return particle charge
    G4double GetPDGCharge() const {return m_PDGCharge;}
    void SetPDGCharge(G4double charge) {m_PDGCharge = charge;}
    
    //return particle name of this track
    G4String GetParticleName() const {return m_particleName;}
    void SetParticleName(G4String name) {m_particleName=name;}
    
    // Access vertex associated with the beginning of the track
    BesTruthVertex* GetVertex() const {return m_vertex;}
    void SetVertex(BesTruthVertex* vertex) {m_vertex=vertex;}

    G4int GetBarcodeEndVtx() {return m_barcodeEndVtx;}
    void SetBarcodeEndVtx(G4int vtx) {m_barcodeEndVtx = vtx;}

    //Access vertex associated with the death of the track,
    //or return zero if the track exited the detector.
    BesTruthVertex* GetTerminalVertex() const {return m_terminalVertex;}
    void SetTerminalVertex(BesTruthVertex* vertex) {m_terminalVertex=vertex;}

    // Returns the index of this track
    G4int GetIndex() const {return m_index;}
    void SetIndex(G4int index) {m_index=index;}

    //return the Geant4 trackId of this track
    G4int GetG4TrackId() const {return m_g4TrackId;}
    void SetG4TrackId(G4int trackId) {m_g4TrackId = trackId;}
    
    // Return the youngest parent saved to GTrack. This may or
    // may not be the immediate parent of the track.
    BesTruthTrack* GetParentTrack()
    {
      if (m_vertex == 0) return 0; 
      return m_vertex->GetParentTrack();
    }

    //add an index of one daughter of this track
    void AddDaughterIndex(G4int index) {m_daughterIndexes.push_back(index);}

    //return indexes of daughters of this track
    vector<int> GetDaughterIndexes() const {return m_daughterIndexes;}
   
    void Found() {m_found = true;}
    G4bool NotFound() {return !m_found;} 

    G4String GetSource() {return m_source;}
    void SetSource(G4String source) {m_source=source;}

  private:
    
    //the four-momentum of the particle in the
    //lab frame at the point of origin (as stored in GVertex)
    HepLorentzVector m_fourMomentum; 
    
    //vertex representing the origin of this particle
    BesTruthVertex* m_vertex;

    //vertex representing the end of this particle
    BesTruthVertex* m_terminalVertex;
 
    G4int m_barcodeEndVtx;

    //particle ID
    G4int m_PDGCode;

    //particle charge;
    G4double m_PDGCharge;

    //particle name
    G4String m_particleName;

    //index in trackList
    G4int m_index;   
   
    //Geant4 track ID
    G4int m_g4TrackId;

    //indexes of daughter tracks
    vector<int>  m_daughterIndexes; 

    G4bool m_found;
    G4String m_source;
};

#endif
