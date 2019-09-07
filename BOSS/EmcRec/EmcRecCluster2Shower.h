//
//  Convert from Cluster Map to Shower Map
//
//  Wang.Zhe 2004, 3, 30
//  Miao He 2006, 3, 13
//
#ifndef EMC_REC_CLUSTER_2_SHOWER_H
#define EMC_REC_CLUSTER_2_SHOWER_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecCluster2ShowerAbs.h"
#include "EmcRec/EmcRecNeighbor.h"
#include "EmcRec/EmcRecSeedAbs.h"
#include "EmcRec/EmcRecSplitAbs.h"

class EmcRecCluster2Shower:public EmcRecCluster2ShowerAbs
{
 public:
   // Constructors and destructors
   EmcRecCluster2Shower();
   ~EmcRecCluster2Shower();
   
   void Convert(RecEmcClusterMap& aClusterMap,
		RecEmcShowerMap& aShowerMap);

 private:
   EmcRecSeedAbs* fSeedFinder;
   EmcRecSplitAbs* fSplitter;
};

#endif  //EMC_REC_CLUSTER_2_SHOWER_H

