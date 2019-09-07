//
//  Convert from Hit Map to Cluster Map
//
//  Wang.Zhe 2004, 3, 29
//
#ifndef EMC_REC_HIT_2_CLUSTER_H
#define EMC_REC_HIT_2_CLUSTER_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecNeighbor.h"

class EmcRecHit2Cluster
{
 public:
   // Constructors and destructors
   EmcRecHit2Cluster();
   ~EmcRecHit2Cluster();
   
   void Convert(const RecEmcHitMap& aHitMap,
		RecEmcClusterMap& aClusterMap);
   
};
#endif
