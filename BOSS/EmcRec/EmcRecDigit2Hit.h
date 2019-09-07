//
//  Convert from Digit Map to Hit Map
//
//  Zhe Wang  2004, 8, 27
//
#ifndef EMC_REC_DIGIT_2_HIT_H
#define EMC_REC_DIGIT_2_HIT_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include <string>

class EmcRecDigit2Hit
{
 public:
   // Constructors and destructors
   EmcRecDigit2Hit();
   ~EmcRecDigit2Hit();
   
   void Convert(const RecEmcDigitMap& aDigitMap,
		RecEmcHitMap& aHitMap);

   void Output(const RecEmcHitMap& aHitMap) const;
   void OutputEndcap(const RecEmcHitMap& aHitMap, const unsigned int module_ew) const;
   inline void SetAlgName(const string &name) { m_algName=name; }

 private:
   std::string m_algName;
};
#endif // EMC_REC_DIGIT_2_HIT_H

