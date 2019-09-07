//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Youzy
//Created: July, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesMucDigi.cc 

#include "BesMucDigi.hh"

G4Allocator<BesMucDigi> BesMucDigiAllocator;

BesMucDigi::BesMucDigi()
{
  m_trackIndex = -99;
  m_partId     = -99;
  m_segId      = -99;
  m_gapId      = -99;
  m_stripId    = -99;
}

BesMucDigi::~BesMucDigi() {}

BesMucDigi::BesMucDigi(const BesMucDigi& right)
:G4VDigi()
{
  m_trackIndex = right.m_trackIndex;
  m_partId     = right.m_partId;
  m_segId      = right.m_segId;
  m_gapId      = right.m_gapId;
  m_stripId    = right.m_stripId;
}


const BesMucDigi& BesMucDigi::operator=(const BesMucDigi& right)
{
  m_trackIndex = right.m_trackIndex;
  m_partId     = right.m_partId;
  m_segId      = right.m_segId;
  m_gapId      = right.m_gapId;
  m_stripId    = right.m_stripId;

  return *this;
}


int BesMucDigi::operator==(const BesMucDigi& right) const
{
  return (this==&right) ? 1 : 0;
}

void BesMucDigi::Draw()
{ }

void BesMucDigi::Print()
{ }


