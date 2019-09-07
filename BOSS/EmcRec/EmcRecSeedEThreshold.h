//
//  Seed finder
//  1. local max (the same with EmcRecSeedLocalMax)
//  2. energy of hit > EThresholdSeed
//
//  Zhe Wang 2004, 8, 31
//
#ifndef EMC_REC_SEED_E_THRESHOLD_H
#define EMC_REC_SEED_E_THRESHOLD_H

#include "EmcRec/EmcRecSeedAbs.h"
#include "EmcRec/EmcRecParameter.h"

class EmcRecSeedEThreshold:public EmcRecSeedAbs
{
 public:
   // Constructors and destructors
   EmcRecSeedEThreshold();
   ~EmcRecSeedEThreshold();
   
 public:
   virtual void Seed(const RecEmcCluster& aCluster,
		     RecEmcIDVector& aMaxVec);
   
 private:
   double fEThresholdSeed;
};

#endif  // EMC_REC_SEED_E_THRESHOLD_H

