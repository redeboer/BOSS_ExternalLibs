//
//  Match Emc Shower and Tof Hit
//
//  He Miao 2006, 9, 15
//
#ifndef EMC_REC_TOF_MATCH_H
#define EMC_REC_TOF_MATCH_H

#include "EmcRecEventModel/RecEmcEventModel.h"

class EmcRecTofMatch
{
 public:
   // Constructors and destructors
   EmcRecTofMatch();
   ~EmcRecTofMatch();
   
   // Emc Tof Match
   void Match(RecEmcShowerMap& aShowerMap,
       RecEmcTofHitMap& tofHitMap);
   
   // Energy correction
   RecEmcEnergy ECorrection(const RecEmcEnergy eIn);
   
};
#endif
