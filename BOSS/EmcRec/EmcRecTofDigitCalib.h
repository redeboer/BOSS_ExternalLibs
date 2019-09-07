//
//  Tof digit to hit
//
//  He Miao 2006, 9, 26
//
#ifndef EMC_REC_TOF_DIGIT_CALIB_H
#define EMC_REC_TOF_DIGIT_CALIB_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "TofRawEvent/TofDigi.h"

class EmcRecTofDigitCalib
{
 public:
   // Constructors and destructors
   EmcRecTofDigitCalib();
   ~EmcRecTofDigitCalib();
   
   void Convert(TofDigiCol* tofDigiCol,
       RecEmcTofHitMap& tofHitMap);
   
};
#endif
