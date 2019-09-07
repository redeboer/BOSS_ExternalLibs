//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description: The class for Tof digi info
//Author: Dengzy
//Created: Mar, 2004
//Modified: Liu Yong @ Dec, 2008
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesTofDigi.cc

#include "BesTofDigi.hh"

G4Allocator<BesTofDigi> BesTofDigiAllocator;

BesTofDigi::BesTofDigi()
{
    m_trackIndex = 0;
    m_partId = -9;
    m_scinNb = -9;
    m_forwADC = -999;
    m_backADC = -999;
    m_forwTDC = -999;
    m_backTDC = -999;


}

BesTofDigi::~BesTofDigi() {}

BesTofDigi::BesTofDigi(const BesTofDigi& right)
        :G4VDigi()
{
    m_trackIndex = right.m_trackIndex;
    m_partId=right.m_partId;
    m_scinNb=right.m_scinNb;
    m_forwADC=right.m_forwADC;
    m_backADC=right.m_backADC;
    m_forwTDC=right.m_forwTDC;
    m_backTDC=right.m_backTDC;
    
      

}


const BesTofDigi& BesTofDigi::operator=(const BesTofDigi& right)
{
    m_trackIndex=right.m_trackIndex;
    m_partId=right.m_partId;
    m_scinNb=right.m_scinNb;
    m_forwADC=right.m_forwADC;
    m_backADC=right.m_backADC;
    m_forwTDC=right.m_forwTDC;
    m_backTDC=right.m_backTDC;


    return *this;
}


int BesTofDigi::operator==(const BesTofDigi& right) const
{
    return (this==&right) ? 1 : 0;
}


void BesTofDigi::Draw()
{
}


void BesTofDigi::Print()
{
}


