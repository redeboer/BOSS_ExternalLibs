//
//  Bes EMC Rec Shower Fraction
//  (a fraction of a hit)
//
//  Created by Wang.Zhe  2003, 10, 1
//
#ifndef REC_EMC_FRACTION_H
#define REC_EMC_FRACTION_H

#include <map>

#include "EmcRecEventModel/RecEmcDataType.h"
#include "EmcRecEventModel/RecEmcHit.h"

using namespace std;

class RecEmcFraction: public RecEmcHit
{
 public:
   //Constructors and destructors
   RecEmcFraction();
   RecEmcFraction(const RecEmcHit& aHit);
   ~RecEmcFraction();
   
   //Copy and assignment
   //RecEmcFraction(const RecEmcFraction& aFraction);
   //RecEmcFraction& operator=(const RecEmcFraction& aFraction);
   
   bool operator < (const RecEmcFraction& aFraction) const {
     return fEnergy*fFraction < aFraction.fEnergy*aFraction.fFraction;
   }
   
   bool operator > (const RecEmcFraction& aFraction) const {
     return fEnergy*fFraction > aFraction.fEnergy*aFraction.fFraction;
   }
   
   //Other methods
   //Read
   RecEmcFrac getFraction() const;
   
   //Write
   RecEmcFrac Fraction(const RecEmcFrac& Fraction);

   //Distance to another frac
   double Distance(const RecEmcID id) const;

   //others
   void Dump() const;

 protected:
   RecEmcFrac fFraction;
};

ostream& operator<<(ostream & os, const RecEmcFraction& aFraction);

typedef map<RecEmcID,RecEmcFraction,less<RecEmcID> > RecEmcFractionMap;

#endif	/* REC_EMC_FRACTION_H */
