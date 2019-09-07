//---------------------------------------------------------------------------//
////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
////---------------------------------------------------------------------------//
////Author : Dengzy
////Created:  Aug, 2004
////Modified:
////Comment:
////$Id: BesTruthEvent.hh

#ifndef BesTruthEvent_h 
#define BesTruthEvent_h 1

class BesTruthTrack;
class BesTruthVertex;
#include <vector>
using namespace std;

class BesTruthEvent
{
  public:
    BesTruthEvent();
    ~BesTruthEvent();
  
    vector<BesTruthTrack*>* GetTrackList() {return m_trackList;}
    vector<BesTruthVertex*>* GetVertexList() {return m_vertexList;}

    void SetTrackList(vector<BesTruthTrack*>* trackList) {m_trackList = trackList;}
    void SetVertexList(vector<BesTruthVertex*>* vertexList) {m_vertexList = vertexList;}
    
  private:
   vector<BesTruthTrack*>   *m_trackList;
   vector<BesTruthVertex*>  *m_vertexList;
   
};

#endif
