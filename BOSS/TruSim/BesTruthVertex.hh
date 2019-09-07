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
//// //// $Id: BesTruthVertex.hh

#ifndef BesTruthVertex_h
#define BesTruthVertex_h 1

#include <iostream>
using namespace std;
#include "globals.hh"
#include "G4ThreeVector.hh"

class BesTruthTrack;

class BesTruthVertex
{
  public:
    BesTruthVertex();
    ~BesTruthVertex() {;}

    friend ostream &operator<<(ostream &, const BesTruthVertex&);
    friend ostream &operator<<(ostream &, const BesTruthVertex*);    

    enum {unassigned = -1} ;
   
    G4String GetProcessName() const { return m_processName;}
    void SetProcessName(const G4String name) {m_processName=name;}

    G4ThreeVector GetPosition() const { return m_position; }
    void SetPosition( const G4ThreeVector &p ) { m_position = p; }
	
    G4double GetTime() const { return m_time; }
    void SetTime( const G4double &t ) { m_time = t; }
	
    BesTruthTrack* GetParentTrack() const { return m_parent; }
    void SetParentTrack( BesTruthTrack *newParent ) { m_parent = newParent; }
    
    bool GetTerminal() const { return m_terminal; }
    void SetTerminal( bool wasTerminal ) { m_terminal = wasTerminal; }
	
    G4int GetIndex() const { return m_index; }
    void SetIndex( signed long newIndex ) { m_index = newIndex; }
   
    G4int GetCurrentDau() const { return m_currentDau;}
    void AddCurrentDau() { m_currentDau++; }

    G4int GetMinDau() const { return m_minDau; }
    void SetMinDau(G4int dau) { m_minDau = dau; }

  private:    
    //name of the process which created this vertex
    G4String m_processName;
    
    //position of this vertex
    G4ThreeVector m_position;
	
    //relative time of this vertex
    G4double m_time;
    
    // _parent: oldest progenitor stored in a GTrack.
    BesTruthTrack* m_parent;
	
   //_terminal: true if this vertex killed its parent particle
   bool m_terminal;
   
   //index in vertexList
   signed long m_index;

   //the following data member is added for recording trackIndex
   //current processed daughter index, count from 0
   G4int m_currentDau;
   
   //minimum daughter index of this vertex
   G4int m_minDau;
};

#endif
