//
//  Bes EMC Rec Hit
//
//  Created by Wang.Zhe  2003, 10, 1
//
#include <iostream>

#include "EmcRecEventModel/RecEmcHit.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

//Constructors and destructors
RecEmcHit::RecEmcHit()
{
   fCellId.clear();
   fEnergy=-1;
   fTime=-1;
}

RecEmcHit::RecEmcHit(const RecEmcID& CellId,
		     const RecEmcEnergy& Energy,
		     const RecEmcTime& Time)
{
   fCellId=CellId;
   fEnergy=Energy;
   fTime=Time;
}

RecEmcHit::~RecEmcHit()
{
   //Nothing to do
}

//Copy and assignment
//RecEmcHit::RecEmcHit(const RecEmcHit& aHit)
//  :ContainedObject(aHit)
//{
//   fCellId=aHit.CellId();
//   fEnergy=aHit.Energy();
//   fTime=aHit.Time();
//}
//
//RecEmcHit& RecEmcHit::operator=(const RecEmcHit& aHit)
//{
//   if(this!=&aHit)
//     {
//  ContainedObject::operator=(aHit);
//	fCellId=aHit.CellId();
//	fEnergy=aHit.Energy();
//	fTime=aHit.Time();
//     }
//   return *this;
//}

//Other methods
HepPoint3D RecEmcHit::getCenter() const
{
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  return iGeoSvc->GetCCenter(fCellId);
}

HepPoint3D RecEmcHit::getFrontCenter() const
{
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  return iGeoSvc->GetCFrontCenter(fCellId);
}

RecEmcID RecEmcHit::CellId(const RecEmcID& CellId)
{
   fCellId=CellId;
   return fCellId;
}

RecEmcEnergy RecEmcHit::Energy(const RecEmcEnergy& Energy)
{
   fEnergy=Energy;
   return fEnergy;
}
  
RecEmcTime RecEmcHit::Time(const RecEmcTime& Time)
{
   fTime=Time;
   return fTime;
}

void RecEmcHit::Assign(const RecEmcID& CellId,
		       const RecEmcEnergy& Energy,
		       const RecEmcTime& Time)
{
   fCellId=CellId;
   fEnergy=Energy;
   fTime=Time;
}

void RecEmcHit::Dump() const
{
   cout<<"Hit: ";

   cout<<fCellId<<", ";
   
   cout.width(12);
   cout.setf(ios::right);
   cout<<fEnergy<<", ";
      
   cout.width(12);
   cout.setf(ios::right);
   cout<<fTime<<endl;
}

ostream& operator<<(ostream & os, const RecEmcHit& aHit)
{
   os<<"Hit: ";

   os<<aHit.getCellId()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aHit.getEnergy()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aHit.getTime()<<endl;

   return os;
}

