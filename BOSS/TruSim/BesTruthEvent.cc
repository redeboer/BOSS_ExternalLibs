//---------------------------------------------------------------------------//
//////      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//////---------------------------------------------------------------------------//
//////Author : Dengzy
//////Created:  Aug, 2004
//////Modified:
//////Comment:
//////$Id: BesTruthEvent.cc
//////Description:

#include "BesTruthEvent.hh"
#include "BesTruthTrack.hh"
#include "BesTruthVertex.hh"

BesTruthEvent::BesTruthEvent()
:m_trackList(0), m_vertexList(0)
{;}

BesTruthEvent::~BesTruthEvent()
{
/*  for(size_t i=0;i<m_trackList->size();i++)
    delete (*m_trackList)[i];
  m_trackList->clear();
  delete m_trackList;

  for(size_t i=0;i<m_vertexList->size();i++)
    delete (*m_vertexList)[i];
  m_vertexList->clear();
  delete m_vertexList;
*/
}
