//
//  Bes EMC Rec Hit
//
//  Created by Wang.Zhe 2003, 12, 15
//

#include <iostream>

#include "EmcRecEventModel/RecEmcDigit.h"

//Constructors and destructors 
RecEmcDigit::RecEmcDigit()
{
   fCellId.clear();
   fADC=-1;
   fTDC=-1;
}

RecEmcDigit::RecEmcDigit(const RecEmcID& CellId,
			 const RecEmcADC& ADC,
			 const RecEmcTDC& TDC)
{
   fCellId=CellId;
   fADC=ADC;
   fTDC=TDC;
}

RecEmcDigit::~RecEmcDigit()
{
   //nothing to do
}
  
//Copy and assignment
RecEmcDigit::RecEmcDigit(const RecEmcDigit& aDigit) 
{
   fCellId=aDigit.CellId();
   fADC=aDigit.ADC();
   fTDC=aDigit.TDC();
}
  
RecEmcDigit& RecEmcDigit::operator=(const RecEmcDigit& aDigit)
{
   if(this!=&aDigit)
     {
	fCellId=aDigit.CellId();
	fADC=aDigit.ADC();
	fTDC=aDigit.TDC();
     }
   return *this;
}

//Access
//Read
RecEmcID RecEmcDigit::CellId() const
{
   return fCellId;
}

RecEmcADC RecEmcDigit::ADC() const
{
   return fADC;   
}

RecEmcTDC RecEmcDigit::TDC() const
{
   return fTDC;
}

//Write
RecEmcID RecEmcDigit::CellId(const RecEmcID& CellId)
{
   fCellId=CellId; 
   return fCellId;
}

RecEmcADC RecEmcDigit::ADC(const RecEmcADC& ADC)
{
   fADC=ADC;
   return fADC;
}

RecEmcTDC RecEmcDigit::TDC(const RecEmcTDC& TDC)
{
   fTDC=TDC;
   return fTDC;
}

void RecEmcDigit::Assign(const RecEmcID& CellId,
			 const RecEmcADC& ADC,
			 const RecEmcTDC& TDC)
{
   fCellId=CellId;
   fADC=ADC;
   fTDC=TDC;
}

void RecEmcDigit::Assign(const RecEmcDigit& aDigit)
{
   fCellId=aDigit.CellId();
   fADC=aDigit.ADC();
   fTDC=aDigit.TDC();
}

//others 
void RecEmcDigit::Dump() const
{
   cout<<"Digit: ";
   
   cout<<fCellId<<", ";
   
   cout.width(12);
   cout.setf(ios::right);
   cout<<fADC<<", ";
   
   cout.width(12);
   cout.setf(ios::right);
   cout<<fTDC<<endl;
}

ostream& operator<<(ostream & os, const RecEmcDigit& aDigit)
{
   os<<"Digit: ";

   os<<aDigit.CellId()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aDigit.ADC()<<", ";

   os.width(12);
   os.setf(ios::right);
   os<<aDigit.TDC()<<endl;
   
   return os;
}
