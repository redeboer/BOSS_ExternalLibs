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
//// //// $Id: BesTruthVertex.cc

#include <iomanip>
using namespace std;
#include "BesTruthVertex.hh"
#include "BesTruthTrack.hh"

BesTruthVertex::BesTruthVertex()
:m_processName(""),
m_position(0), 
m_time(0), 
m_parent(0),
m_terminal(false),
m_index(unassigned),
m_currentDau(0),
m_minDau(0)
{}

ostream &operator<<(ostream &stream, const BesTruthVertex &vert)
{
  if (vert.GetIndex() == BesTruthVertex::unassigned)
    stream << "  --";
  else
    stream << setw(4) << vert.GetIndex();

  if (vert.GetParentTrack())
    stream << setw(4) << vert.GetParentTrack()->GetIndex();
  else
    stream << "  --";

  stream << " "  << setiosflags(ios::fixed)
	 << setprecision(4) << setw(11) << vert.GetPosition().x()
	 << setprecision(4) << setw(11) << vert.GetPosition().y()
	 << setprecision(4) << setw(11) << vert.GetPosition().z()
	 << " "  << setiosflags(ios::scientific)
	 << setprecision(4) << setw(11) << vert.GetTime()
	 << resetiosflags(ios::scientific);

  return stream;
}

ostream &operator<<(ostream &stream, const BesTruthVertex *vert)
{
  stream << *vert;
  return stream;
}


