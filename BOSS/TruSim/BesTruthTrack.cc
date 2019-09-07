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
//// //// $Id: BesTruthTrack.cc

#include "BesTruthTrack.hh"
#include <iomanip>
using namespace std;

BesTruthTrack::BesTruthTrack()
:m_fourMomentum(0),m_daughterIndexes(0)
{
  //m_fourMomentum;
  m_vertex=0;
  m_terminalVertex=0;
  m_PDGCode=0;
  m_PDGCharge=-99;
  m_particleName="";
  m_index=unassigned;
  m_g4TrackId=-99;
  //m_daughterIndexes=0;
  m_found=false;
  m_source="";
}

ostream &operator<<(ostream &stream, const BesTruthTrack &track)
{
  //track index
  if(track.GetIndex() == BesTruthTrack::unassigned)
    stream << "  --";
  else
    stream << setw(4) << track.GetIndex() << " ";
  
  //PDGcode
  if(track.GetPDGCode())
    stream << setw(4) << track.GetPDGCode()<<" ";
  else
    stream <<"  --";
  
  //particle name
  if(track.GetParticleName())
    stream << setw(10) << track.GetParticleName()<<" ";
  else
    stream <<"      --";
  
  //original vertex index
  if(track.GetVertex())
    stream << setw(4) << track.GetVertex()->GetIndex();
  else
    stream << "  --";

  //terminal vertex index
  if(track.GetTerminalVertex())
    stream << setw(4) << track.GetTerminalVertex()->GetIndex();
  else
    stream << "  --";

  
  //four momentum
  stream << " " << setiosflags(ios::fixed)
         <<setprecision(4) << setw(11) << track.GetP4().x()
	 <<setprecision(4) << setw(11) << track.GetP4().y()
	 <<setprecision(4) << setw(11) << track.GetP4().z()
	 <<setprecision(4) << setw(11) << track.GetP4().e();
  
  //daughter indexes
  if(track.GetDaughterIndexes().size()>0)
  {
    G4int nDaughter = track.GetDaughterIndexes().size();
    for(G4int i=0;i<nDaughter;i++)
     stream <<"  "<<( track.GetDaughterIndexes() )[i]<<"  ";
  }
  return stream;
}

ostream &operator<<(ostream &stream, const BesTruthTrack *track)
{
  stream << *track;
  return stream;
}


  
	
