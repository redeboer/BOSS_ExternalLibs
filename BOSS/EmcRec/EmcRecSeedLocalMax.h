//
//  Seed finder
//  Use local max to determine a seed
//
//  Zhe Wang 2004, 8, 31
//
#ifndef EMC_REC_SEED_LOCAL_MAX_H
#define EMC_REC_SEED_LOCAL_MAX_H

#include "EmcRec/EmcRecSeedAbs.h"

class EmcRecSeedLocalMax:public EmcRecSeedAbs
{
 public:
   // Constructors and destructors
   EmcRecSeedLocalMax();
   ~EmcRecSeedLocalMax();
   
 public:
   virtual void Seed(const RecEmcCluster& aCluster,
		     RecEmcIDVector& aMaxVec);
};

#endif  // EMC_REC_SEED_LOCAL_MAX_H

