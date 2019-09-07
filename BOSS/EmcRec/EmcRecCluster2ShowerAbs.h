//
//  Convert from Cluster Map to Shower Map
//
//  Miao He 2006, 3, 9
//
#ifndef EMC_REC_CLUSTER_2_SHOWER_ABS_H
#define EMC_REC_CLUSTER_2_SHOWER_ABS_H

#include "EmcRecEventModel/RecEmcEventModel.h"

class EmcRecCluster2ShowerAbs
{
  public:
    // Constructors and destructors
    EmcRecCluster2ShowerAbs() {}
    virtual ~EmcRecCluster2ShowerAbs() {}
    
    virtual void Convert(RecEmcClusterMap& aClusterMap,
        RecEmcShowerMap& aShowerMap) = 0;
};

#endif  //EMC_REC_CLUSTER_2_SHOWER_ABS_H
