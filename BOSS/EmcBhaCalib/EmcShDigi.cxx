//EmcShDigi.cxx file

#include "EmcBhaCalib/EmcShDigi.h"

//----------------
// Constructors --
//----------------
EmcShDigi::EmcShDigi():
  m_energy(0.0),
  m_theta(999.),
  m_phi(999.),
  m_module(999),
  m_thetaIndex(9999),
  m_phiIndex(9999),
  m_time(0.0),
  m_fraction(0.),
  m_where(0,0,0),
  m_x(0),
  m_y(0),
  m_z(0)
{

}

//--------------
// Destructor --
//--------------
EmcShDigi::~EmcShDigi()
{
}

//Copy and assignment
EmcShDigi::EmcShDigi(const EmcShDigi& aDigi) 
{
  m_energy=aDigi.energy();
  m_theta=aDigi.theta();
  m_phi=aDigi.phi();
  m_module=aDigi.module();
  m_thetaIndex=aDigi.thetaIndex();
  m_phiIndex=aDigi.phiIndex();
  m_time=aDigi.time();
  m_fraction=aDigi.fraction();
  m_where = aDigi.where();
  m_x=aDigi.x();
  m_y=aDigi.y();
  m_z=aDigi.z();
}
  
EmcShDigi& EmcShDigi::operator=(const EmcShDigi& aDigi)
{
   if(this!=&aDigi)
     {
       m_energy=aDigi.energy();
       m_theta=aDigi.theta();
       m_phi=aDigi.phi();
       m_module=aDigi.module();
       m_thetaIndex=aDigi.thetaIndex();
       m_phiIndex=aDigi.phiIndex();
       m_time=aDigi.time();
       m_fraction=aDigi.fraction();
       m_where = aDigi.where();
       m_x=aDigi.x();
       m_y=aDigi.y();
       m_z=aDigi.z();
     }

   return *this;
}

bool
EmcShDigi::operator==( const EmcShDigi& otherDigi ) const
{
  bool equal=false;

  if ( this->energy() == otherDigi.energy() 
       && this->theta() == otherDigi.theta() 
       && this->phi() == otherDigi.phi() 
       && this->time() == otherDigi.time() 
       && this->fraction() == otherDigi.fraction() ) 
    {
      equal=true;
    }

  return equal;
}

bool
EmcShDigi::operator>=( const EmcShDigi& otherDigi ) const
{
  bool Gequal=false;

  if ( this->energy() >= otherDigi.energy()) 
    {
      Gequal=true;
    }
  
  return Gequal;
}

bool
EmcShDigi::operator<=( const EmcShDigi& otherDigi ) const
{
  bool Lequal=false;

  if ( this->energy() <= otherDigi.energy()) 
    {
      Lequal=true;
    }
  return Lequal;
}

bool
EmcShDigi::operator>( const EmcShDigi& otherDigi ) const
{
  bool Large=false;

  if ( this->energy() > otherDigi.energy()) 
    {
      Large=true;
    }

  return Large;
}

bool
EmcShDigi::operator<( const EmcShDigi& otherDigi ) const
{
  bool Less=false;

  if ( this->energy() < otherDigi.energy()) 
    {
      Less=true;
    }

  return Less;
}
