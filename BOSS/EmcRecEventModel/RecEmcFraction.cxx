//
//  Bes EMC Rec Shower Fraction
//  (a fraction of a hit)
//
//  Created by Wang.Zhe  2003, 10, 1
//

#include <iostream>

#include "EmcRecEventModel/RecEmcFraction.h"
#include "Identifier/EmcID.h"

//Constructors and destructors
RecEmcFraction::RecEmcFraction():RecEmcHit()
{
   fFraction=-1;
}

RecEmcFraction::RecEmcFraction(const RecEmcHit& aHit):RecEmcHit(aHit)
{
   fFraction=-1;
}

RecEmcFraction::~RecEmcFraction()
{
   //nothing to do
}

//Copy and assignment
//RecEmcFraction::RecEmcFraction(const RecEmcFraction& aFraction)
//:ContainedObject(aFraction),RecEmcHit(aFraction)
//{
//   //fCellId=aFraction.CellId();
//   //fEnergy=aFraction.Energy();
//   //fTime=aFraction.Time();
//   //
//   fFraction=aFraction.Fraction();
//}
//
//RecEmcFraction& RecEmcFraction::operator=(const RecEmcFraction& aFraction)
//{
//   if(this!=&aFraction)
//     {
//	fCellId=aFraction.CellId();
//	fEnergy=aFraction.Energy();
//	fTime=aFraction.Time();
//	//
//	fFraction=aFraction.Fraction();
//     }
//   return *this;
//}

//Other methods(read or write)
RecEmcFrac RecEmcFraction::getFraction() const
{
   return fFraction;
}

RecEmcFrac RecEmcFraction::Fraction(const RecEmcFrac& Fraction)
{
   fFraction=Fraction;
   return fFraction;
}

//Distance to another hit
double RecEmcFraction::Distance(const RecEmcID id) const
{
  unsigned int tht,phi;
  unsigned int tht2,phi2;
  unsigned int dtht,dphi;
  double dist;

  tht=EmcID::theta_module(fCellId);
  phi=EmcID::phi_module(fCellId);
  tht2=EmcID::theta_module(id);
  phi2=EmcID::phi_module(id);

  dtht=tht>tht2 ? tht-tht2 : tht2-tht;
  dphi=phi>phi2 ? phi-phi2 : phi2-phi;
  if(dphi>(EmcID::getPHI_BARREL_MAX()+1)/2)
  {
    dphi=EmcID::getPHI_BARREL_MAX()+1-dphi;
  }
  dist=sqrt(double(dtht*dtht+dphi*dphi));

  return dist;
}

//others
void RecEmcFraction::Dump() const
{
   cout<<"Fraction: ";

   cout<<fCellId<<", ";
   
   cout.width(12);
   cout.setf(ios::right);
   cout<<fEnergy<<", ";
   
   cout.width(12);
   cout.setf(ios::right);
   cout<<fTime<<", ";

   cout.width(12);
   cout.setf(ios::right);
   cout<<fFraction;

   cout<<endl;
}

ostream& operator<<(ostream & os, const RecEmcFraction& aFraction)
{
   os<<"Fraction: ";

   os<<aFraction.getCellId()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aFraction.getEnergy()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aFraction.getTime()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aFraction.getFraction()<<", ";
   
   os.width(12);
   os.setf(ios::right);
   os<<aFraction.getEnergy()*aFraction.getFraction();

   os<<endl;

   return os;
}
