
#include "EmcBhaCalib/EmcShower.h"
//#include "CLHEP/Units/PhysicalConstants.h"

//----------------
// Constructors --
//----------------
EmcShower::EmcShower():
  m_energy(0.0),
  m_theta(999.),
  m_phi(999.),
  m_module(999),
  m_thetaIndex(9999),
  m_phiIndex(9999),
  m_numberOfDigis(0),
  m_where(0,0,0),
  m_x(0.),
  m_y(0.),
  m_z(0.)
{
  m_maxima=EmcShDigi();
}

//--------------
// Destructor --
//--------------
EmcShower::~EmcShower()
{
}

//Copy and assignment
EmcShower::EmcShower(const EmcShower& aShower) 
{
  m_energy=aShower.energy();
  m_theta=aShower.theta();
  m_phi=aShower.phi();
  m_module=aShower.module();
  m_thetaIndex=aShower.thetaIndex();
  m_phiIndex=aShower.phiIndex();
  m_numberOfDigis=aShower.numberOfDigis();
  m_digiList=aShower.digiList();
  m_maxima=aShower.maxima();
  m_where = aShower.where();
  m_x=aShower.x();
  m_y=aShower.y();
  m_z=aShower.z();
}
  
EmcShower& EmcShower::operator=(const EmcShower& aShower)
{
   if(this!=&aShower)
     {
       m_energy=aShower.energy();
       m_theta=aShower.theta();
       m_phi=aShower.phi();
       m_module=aShower.module();
       m_thetaIndex=aShower.thetaIndex();
       m_phiIndex=aShower.phiIndex();
       m_numberOfDigis=aShower.numberOfDigis();
       m_digiList=aShower.digiList();
       m_maxima=aShower.maxima();
       m_where = aShower.where();
       m_x=aShower.x();
       m_y=aShower.y();
       m_z=aShower.z();
     }

   return *this;
}

bool
EmcShower::operator==( const EmcShower& otherShower ) const
{
  bool equal=false;

  if ( this->theta() == otherShower.theta() 
       && this->phi() == otherShower.phi() 
       && this->energy() == otherShower.energy() ) 
    {
      equal=true;
    }

  return equal;
}

bool
EmcShower::operator>=( const EmcShower& otherShower ) const
{
  bool Gequal=false;

  if ( this->energy() >= otherShower.energy() ) 
    {
      Gequal=true;
    }
  
  return Gequal;
}

bool
EmcShower::operator<=( const EmcShower& otherShower ) const
{
  bool Lequal=false;

  if ( this->energy() <= otherShower.energy() ) 
    {
      Lequal=true;
    }
  
  return Lequal;
}

bool
EmcShower::operator>( const EmcShower& otherShower ) const
{
  bool Large=false;

  if ( this->energy() > otherShower.energy() ) 
    {
      Large=true;
    }

  return Large;
}

bool
EmcShower::operator<( const EmcShower& otherShower ) const
{
  bool Less=false;

  if ( this->energy() < otherShower.energy() ) 
    {
      Less=true;
    }

  return Less;
}

//-------------
// Modifiers --
//-------------
void
EmcShower::addDigi( EmcShDigi theDigi )
{ 
  m_digiList.push_back(theDigi); 

}
