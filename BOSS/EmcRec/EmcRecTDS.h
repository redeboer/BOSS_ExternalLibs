#include "EmcRecEventModel/RecEmcEventModel.h"

class StatusCode;

class EmcRecTDS
{
  
  public:
    
    StatusCode RegisterReconEvent();
    StatusCode RegisterHit(RecEmcHitMap& aHitMap);
    StatusCode RegisterCluster(RecEmcClusterMap& aClusterMap);
    StatusCode RegisterShower(RecEmcShowerMap& aShowerMap);
    StatusCode RegisterToTDS(RecEmcHitMap& aHitMap, 
        RecEmcClusterMap& aClusterMap, RecEmcShowerMap& aShowerMap);
    StatusCode CheckRegister();

};
