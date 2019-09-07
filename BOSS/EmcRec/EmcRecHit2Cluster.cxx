//
//  Convert from Hit Map to Cluster Map
//
//  Wang.Zhe 2004, 3, 29
//
#include <iostream>

#include "EmcRec/EmcRecHit2Cluster.h" 
#include "EmcRec/EmcRecParameter.h"

// Constructors and destructors
EmcRecHit2Cluster::EmcRecHit2Cluster()
{}

EmcRecHit2Cluster:: ~EmcRecHit2Cluster()
{}

void EmcRecHit2Cluster::Convert(const RecEmcHitMap& aHitMap,
				   RecEmcClusterMap& aClusterMap)
{
   RecEmcHitMap tHitMap,aBufferHitMap;
   RecEmcHitMap::iterator pHitMap,pBufferHitMap;
   tHitMap=aHitMap;
   
   RecEmcID CellId;
   RecEmcIDVector NearCell;
   i_RecEmcIDVector pNearCell;
   
   RecEmcCluster aCluster;

   EmcRecParameter& Para=EmcRecParameter::GetInstance();
   EmcRecNeighbor nhb;
   
   while(!tHitMap.empty())
   {
     double eCluster=0;   //total energy of a cluster
     double emax=0;   //seed's energy of a cluster
     RecEmcID idmax;    //seed's id of a cluster
     
	   // Search a cluster.
	   pHitMap=tHitMap.begin();
	   aBufferHitMap[pHitMap->first]=pHitMap->second;
	   tHitMap.erase(pHitMap);
	   aCluster.Clear();
	   while(!aBufferHitMap.empty())
	   {
	     pBufferHitMap=aBufferHitMap.begin();
	     CellId=pBufferHitMap->first;
	     NearCell=nhb.GetNeighbors(CellId);
	     for(pNearCell=NearCell.begin();
		       pNearCell!=NearCell.end();
		       pNearCell++)
	     {
		     pHitMap=tHitMap.find(*pNearCell);
		     if(pHitMap!=tHitMap.end())
		     {
		       aBufferHitMap[pHitMap->first]=pHitMap->second;
		       tHitMap.erase(pHitMap);
		     }
	     }

       // Find seed
       if(pBufferHitMap->second.getEnergy()>emax) {
         emax=pBufferHitMap->second.getEnergy();
         idmax=pBufferHitMap->first;
       }
         
       eCluster+=pBufferHitMap->second.getEnergy();
	     aCluster.Insert(pBufferHitMap->second);
	     aBufferHitMap.erase(pBufferHitMap);
	   }
	 // Now, done and save it.
     if(eCluster>Para.EThresholdCluster())
     {
       aCluster.ClusterId(idmax);
	     aClusterMap[idmax]=aCluster;
	     //aClusterMap[aCluster.ClusterId()]=aCluster;
     }
   }

}

