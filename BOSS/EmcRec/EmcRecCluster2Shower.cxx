//
//  Convert from Cluster Map to Shower Map
//
//  Created by Zhe Wang 2004, 3, 29
//
#include <iostream>

#include "EmcRec/EmcRecCluster2Shower.h"
//#include "EmcRec/EmcRecSeedLocalMax.h"
#include "EmcRec/EmcRecSeedEThreshold.h"
#include "EmcRec/EmcRecSplitWeighted.h"

// Constructors and destructors
EmcRecCluster2Shower::EmcRecCluster2Shower()
{   
  //cout<<"====== EmcRec: Offline Mode ======"<<endl;
  //   fSeedFinder=new EmcRecSeedLocalMax;
  fSeedFinder=new EmcRecSeedEThreshold;
  fSplitter=new EmcRecSplitWeighted;
}

EmcRecCluster2Shower:: ~EmcRecCluster2Shower()
{
  delete fSeedFinder;
  delete fSplitter;
}

void EmcRecCluster2Shower::Convert(RecEmcClusterMap& aClusterMap,
    RecEmcShowerMap& aShowerMap)
{
  RecEmcClusterMap::iterator ciClusterMap;

  RecEmcIDVector aMaxVec;
  for(ciClusterMap=aClusterMap.begin();
      ciClusterMap!=aClusterMap.end();
      ++ciClusterMap)
  {
    //++++++++++++++++++++++++++
    //get its local maximum list
    fSeedFinder->Seed(ciClusterMap->second,aMaxVec);
    //++++++++++++++++++++++++++++++++++++++++++++++
    //put seeds to cluster
    if(!aMaxVec.empty()) {
      ci_RecEmcIDVector ciMax;
      for(ciMax=aMaxVec.begin();
          ciMax!=aMaxVec.end();
          ++ciMax) {
        ciClusterMap->second.InsertSeed(ciClusterMap->second.Find(*ciMax)->second);
      }
    }
    //++++++++++++++++++++++++++++++++++++++++++++++
    //split it into showers and push into shower map
    fSplitter->Split(ciClusterMap->second,aMaxVec,aShowerMap);
  }
}

