//
//  Seed finder
//  1. local max (the same with EmcRecSeedLocalMax)
//  2. energy of hit > EThresholdSeed
//
//  Zhe Wang 2004, 8, 31
//
#include <iostream>

#include "EmcRec/EmcRecSeedEThreshold.h"

EmcRecSeedEThreshold::EmcRecSeedEThreshold()
{
   fEThresholdSeed=EmcRecParameter::GetInstance().EThresholdSeed();
}

EmcRecSeedEThreshold::~EmcRecSeedEThreshold()
{}

void EmcRecSeedEThreshold::Seed(const RecEmcCluster& aCluster,
				RecEmcIDVector& aMaxVec)
{
  RecEmcHitMap::const_iterator ciHitMap;
  RecEmcHitMap::const_iterator ciNeigh;
  RecEmcHit aHit;
  
  bool max;
  
  RecEmcIDVector NeighVec;
  RecEmcIDVector::const_iterator ciNeighVec;
  
  aMaxVec.clear();
  
  vector<RecEmcHit> aHitVec,tmpHitVec;
  vector<RecEmcHit>::const_iterator ciHitVec;
  
  EmcRecNeighbor nhb;
  
  for(ciHitMap=aCluster.Begin();
      ciHitMap!=aCluster.End();
      ++ciHitMap) {
    //++++++++++++++++++++++++++++++++++++++++++++++++++
	  //prepare a hit
    max=true;
    aHit=ciHitMap->second;
    
    if(aHit.getEnergy()>fEThresholdSeed) {
      //prepare its neighbours
      NeighVec.clear();
      NeighVec=nhb.GetNeighbors(aHit.getCellId());
      
      //to see if it is energy max
      for(ciNeighVec=NeighVec.begin();
          ciNeighVec!=NeighVec.end();
          ++ciNeighVec) {
        //find it
        ciNeigh=aCluster.Find(*ciNeighVec);
		    if(ciNeigh!=aCluster.End()) {
		      //largest!!!
		      if(ciNeigh->second.getEnergy()>aHit.getEnergy()) {
		        max=false;
		      }
		    }
      }
    } else {
      max=false;
    }

    //avoid 2 equal neighbor seeds
    for(ciHitVec=aHitVec.begin();
        ciHitVec!=aHitVec.end();
        ciHitVec++) {
      if(nhb.IsNeighbor(ciHitVec->getCellId(),aHit.getCellId())) {
        max=false;
        break;
      }
    }
      
    //once max hit is found
    if(max==true) {
      aHitVec.push_back(aHit);
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++
  }
  
  sort(aHitVec.begin(),aHitVec.end(),greater<RecEmcHit>());
  
  for(ciHitVec=aHitVec.begin();
      ciHitVec!=aHitVec.end();
      ciHitVec++) {
    aMaxVec.push_back(ciHitVec->getCellId());
  }
}

