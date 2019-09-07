//
//  Bes EMC Rec Hit
//
//  Created by Wang.Zhe 2003, 12, 15
//

#ifndef REC_EMC_DIGIT_H
#define REC_EMC_DIGIT_H

#include <map>

#include "EmcRecEventModel/RecEmcDataType.h"

using namespace std;

class RecEmcDigit
{
 public:
   //Constructors and destructors
   RecEmcDigit();
   RecEmcDigit(const RecEmcID& CellId,
	       const RecEmcADC& ADC,
	       const RecEmcTDC& TDC);
   ~RecEmcDigit();
   
   //Copy and assignment
   RecEmcDigit(const RecEmcDigit& aDigit);
   RecEmcDigit& operator=(const RecEmcDigit& aDigit);
   
   //Access
   //Read
   RecEmcID CellId() const;
   RecEmcADC ADC() const;
   RecEmcTDC TDC() const;
   
   //Write
   RecEmcID CellId(const RecEmcID& CellId);
   RecEmcADC ADC(const RecEmcADC& ADC);
   RecEmcTDC TDC(const RecEmcTDC& TDC);
   
   void Assign(const RecEmcID& CellId,
	       const RecEmcADC& ADC,
	       const RecEmcTDC& TDC);
   
   void Assign(const RecEmcDigit& aDigit);

   //others
   void Dump() const; 
   
 protected:
   //Data members
   RecEmcID fCellId;
   RecEmcADC fADC;
   RecEmcTDC fTDC;
};

ostream& operator<<(ostream & os, const RecEmcDigit& aDigit);

typedef map<RecEmcID,RecEmcDigit,less<RecEmcID> > RecEmcDigitMap;

#endif /* REC_EMC_DIGIT_H */
