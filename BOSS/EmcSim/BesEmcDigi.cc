//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Hemiao
//Created: Sep, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//
// $Id: BesEmcDigi.cc 

#include "BesEmcDigi.hh"

G4Allocator<BesEmcDigi> BesEmcDigiAllocator;

BesEmcDigi::BesEmcDigi()
{
  m_partId=0;
  m_nTheta=0;
  m_nPhi=0;
  m_energy=0;
  m_time=0;
  digiWaveform = new BesEmcWaveform();
}

BesEmcDigi::~BesEmcDigi() 
{
  if(digiWaveform)
    delete digiWaveform;
}

BesEmcDigi::BesEmcDigi(const BesEmcDigi& right)
:G4VDigi()
{
  m_partId=right.m_partId;
  m_nTheta=right.m_nTheta;
  m_nPhi=right.m_nPhi;
  m_energy=right.m_energy;
  m_time=right.m_time;
}


const BesEmcDigi& BesEmcDigi::operator=(const BesEmcDigi& right)
{
  m_partId=right.m_partId;
  m_nTheta=right.m_nTheta;
  m_nPhi=right.m_nPhi;
  m_energy=right.m_energy;
  m_time=right.m_time;
  return *this;
}


BesEmcDigi& BesEmcDigi::operator+=(const BesEmcDigi& right)
{
  BesEmcDigi *digi = const_cast<BesEmcDigi*>(&right);
  digiWaveform->updateWaveform(digi);
  
  G4long bin;
  m_energy=digiWaveform->max(bin);
  m_time=(G4double)bin;

  return *this;
}


int BesEmcDigi::operator==(const BesEmcDigi& right) const
{
  return (this==&right) ? 1 : 0;
}


void BesEmcDigi::Draw()
{
}


void BesEmcDigi::Print()
{
}

void BesEmcDigi::MakeWaveform()
{
  digiWaveform->updateWaveform(this);
}
