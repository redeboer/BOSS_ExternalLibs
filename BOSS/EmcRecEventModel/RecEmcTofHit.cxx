//
// Bes Tof Rec Hit
//
// Created by He Miao 2006,9,15
//
#include "EmcRecEventModel/RecEmcTofHit.h"
#include "Identifier/TofID.h"
#include <iostream>

//Constructors and destructors
RecEmcTofHit::RecEmcTofHit()
{
  Clear();
}

RecEmcTofHit::RecEmcTofHit(const Identifier& id,
                           const double e,
                           const HepPoint3D pos)
{
  m_cellId = id;
  m_energy = e;
  m_pos = pos;
}

RecEmcTofHit::~RecEmcTofHit()
{}

//Copy and assignment
RecEmcTofHit::RecEmcTofHit(const RecEmcTofHit& tofHit)
{
  m_cellId = tofHit.CellId();
  m_energy = tofHit.Energy();
  m_pos = tofHit.Position();
}

RecEmcTofHit& RecEmcTofHit::operator=(const RecEmcTofHit& tofHit)
{
  if(this!=&tofHit) {
    m_cellId = tofHit.CellId();
    m_energy = tofHit.Energy();
    m_pos = tofHit.Position();
  }
  return *this;
}
 
//Other methods
int RecEmcTofHit::IntCellId() const
{
  int layer = TofID::layer(m_cellId);
  int im = TofID::phi_module(m_cellId);
  return im+layer*88;
}

void RecEmcTofHit::CellId(const Identifier& id)
{
  m_cellId = id;
}

void RecEmcTofHit::Energy(const double e)
{
  m_energy = e;
}

void RecEmcTofHit::Position(const HepPoint3D pos)
{
  m_pos = pos;
}

void RecEmcTofHit::Assign(const Identifier& id,
    const double e,
    const HepPoint3D pos)
{
  m_cellId = id;
  m_energy = e;
  m_pos = pos;
} 
  
void RecEmcTofHit::Clear()
{
  m_cellId.clear();
  m_energy = 0;
  m_pos = HepPoint3D(0,0,0);
}

double RecEmcTofHit::DistanceTo(const RecEmcTofHit &tofHit)
{
  //double dphi = m_pos.phi()-tofHit.m_pos.phi();

  //if(dphi>CLHEP::pi) dphi = CLHEP::twopi - dphi;
  //if(dphi<-CLHEP::pi) dphi = CLHEP::twopi + dphi;

  Hep3Vector pos1(m_pos);
  Hep3Vector pos2(tofHit.m_pos);

  return fabs(pos1.deltaPhi(pos2));

  //cout<<"dphi="<<dphi<<"\tdeltaPhi="<<deltaPhi<<endl;
  //return fabs(dphi);
}

ostream& operator<<(ostream & os, const RecEmcTofHit& tofHit)
{
  os<<"TofHit: ";
  
  os<<tofHit.IntCellId()<<", ";
  
  os.width(12);
  os.setf(ios::right);
  os<<tofHit.CellId()<<", ";
  
  os.width(12);
  os.setf(ios::right);
  os<<tofHit.Energy()<<", ";
  
  os.width(12);
  os.setf(ios::right);
  os<<tofHit.ZPosition()<<endl;
  
  return os;
}
