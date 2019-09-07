//
//  Convert from Cluster Map to Shower Map in online mode.
//
//  Miao He 2006, 3, 9
//
#ifndef EMC_REC_FAST_CLUSTER_2_SHOWER_H
#define EMC_REC_FAST_CLUSTER_2_SHOWER_H

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecCluster2ShowerAbs.h"

class EmcRecFastCluster2Shower:public EmcRecCluster2ShowerAbs
{
  public:
    // Constructors and destructors
    EmcRecFastCluster2Shower();
    virtual ~EmcRecFastCluster2Shower();
    
    virtual void Convert(RecEmcClusterMap& aClusterMap,
        RecEmcShowerMap& aShowerMap);
};

#endif  //EMC_REC_FAST_CLUSTER_2_SHOWER_H

