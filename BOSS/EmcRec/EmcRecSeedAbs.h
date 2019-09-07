//
//  Seed finder abstract base class
//
//  Zhe Wang 2004, 8, 31
//
#ifndef EMC_REC_SEED_ABS_H
#define EMC_REC_SEED_ABS_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecNeighbor.h"

class EmcRecSeedAbs
{
 public:
   // Constructors and destructors
   EmcRecSeedAbs() {}
   virtual ~EmcRecSeedAbs() {}
   
 public:
   virtual void Seed(const RecEmcCluster& aCluster,
		     RecEmcIDVector& aMaxVec) =0;
};

#endif  // EMC_REC_SEED_ABS_H

