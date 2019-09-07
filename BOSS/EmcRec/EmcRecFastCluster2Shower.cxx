//
//  Convert from Cluster Map to Shower Map in online mode
//
//  Created by Miao He 2006, 3, 9
//
#include <iostream>

#include "EmcRec/EmcRecFastCluster2Shower.h"

// Constructors and destructors
EmcRecFastCluster2Shower::EmcRecFastCluster2Shower()
{
  //cout<<"====== EmcRec: Online Mode ======"<<endl;
}

EmcRecFastCluster2Shower:: ~EmcRecFastCluster2Shower()
{
}

void EmcRecFastCluster2Shower::Convert(RecEmcClusterMap& aClusterMap,
				   RecEmcShowerMap& aShowerMap)
{
  RecEmcClusterMap::const_iterator ciClusterMap;
  RecEmcHitMap::const_iterator ciHitMap;
   
  for(ciClusterMap=aClusterMap.begin();
      ciClusterMap!=aClusterMap.end();
      ++ciClusterMap) {

    RecEmcShower aShower;
    aShower.Clear();
    const RecEmcCluster &aCluster=ciClusterMap->second;

    aShower.ShowerId(aCluster.getClusterId());
    RecEmcEnergy energy=aCluster.getEnergy();
    aShower.EAll(energy);
    aShower.setEnergy(energy);
    aShower.setPosition(aCluster.getPosition());
    aShowerMap[aShower.getShowerId()]=aShower;
  }

}

