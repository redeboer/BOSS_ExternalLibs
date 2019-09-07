//
//  Spliter
//  Simple weighted method
//  This is only used for testing code.
//
//  Zhe Wang 2004, 8, 31
//
#ifndef EMC_REC_SPLIT_WEIGHTED_H
#define EMC_REC_SPLIT_WEIGHTED_H

#include "EmcRec/EmcRecSplitAbs.h"

class EmcRecShowerEnergy;
class EmcRecShowerPosAbs;
class EmcRecShowerShape;

class EmcRecSplitWeighted:public EmcRecSplitAbs
{
 public:
   // Constructors and destructors
   EmcRecSplitWeighted();
   ~EmcRecSplitWeighted();
   
 public:
   virtual void Split(RecEmcCluster& aCluster,
		      const RecEmcIDVector& aMaxVec,
		      RecEmcShowerMap& aShowerMap);

   EmcRecShowerEnergy* fShowerE;
   EmcRecShowerPosAbs* fShowerPos;
   EmcRecShowerShape* fShowerShape;
};

#endif  // EMC_REC_SPLIT_WEIGHTED_H

