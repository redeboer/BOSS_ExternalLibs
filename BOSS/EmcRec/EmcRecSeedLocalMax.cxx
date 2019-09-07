//
//  Seed finder abstract base class
//  Use local max to determine a seed
//
//  Zhe Wang 2004, 8, 31
//
#include <iostream>

#include "EmcRec/EmcRecSeedLocalMax.h"

EmcRecSeedLocalMax::EmcRecSeedLocalMax()
{}

EmcRecSeedLocalMax::~EmcRecSeedLocalMax()
{}

void EmcRecSeedLocalMax::Seed(const RecEmcCluster& aCluster,
			      RecEmcIDVector& aMaxVec)
{
   RecEmcHitMap::const_iterator ciHitMap;
   RecEmcHitMap::const_iterator ciNeigh;
   RecEmcHit aHit;
   
   bool max;
   
   RecEmcIDVector NeighVec;
   RecEmcIDVector::const_iterator ciNeighVec;
      
   aMaxVec.clear();
   
   EmcRecNeighbor nhb;

   for(ciHitMap=aCluster.Begin();
       ciHitMap!=aCluster.End();
       ++ciHitMap) {
      //++++++++++++++++++++++++++++++++++++++++++++++++++
      //prepare a hit
      max=true;
      aHit=ciHitMap->second;
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
	    if(ciNeigh->second.getEnergy()>=aHit.getEnergy()) {
	       max=false;
	    }
	 }
      }
	 
      //once max hit is found
      if(max==true) {
        aMaxVec.push_back(aHit.getCellId());
      }
      //++++++++++++++++++++++++++++++++++++++++++++++++++
   }
}

