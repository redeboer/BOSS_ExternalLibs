//
//  Bes EMC Rec Cluster
//
//  Created by Zhe Wang 2003, 10, 1
//
#ifndef REC_EMC_CLUSTER_H
#define REC_EMC_CLUSTER_H

#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/ObjectVector.h"
#include "EventModel/EventModel.h"
#include "EmcRecEventModel/RecEmcHit.h"
#include "EmcRecEventModel/RecEmcDataType.h"

using namespace std;

extern const CLID& CLID_RecEmcCluster;

//class RecEmcShower;

class RecEmcCluster :  virtual public ContainedObject
{
 public:
   
    virtual const CLID& clID()  const { return  RecEmcCluster::classID();}
    static  const CLID& classID() { return CLID_RecEmcCluster; }

   //Constructors and destructors
   RecEmcCluster();
   ~RecEmcCluster();
   
   //Copy and assignment
   //RecEmcCluster(const RecEmcCluster& aCluster);
   //RecEmcCluster& operator=(const RecEmcCluster& aCluster);

   //Other methods
   //Reset all data members to default value
   void Clear();
   
   //Access a cluster
   RecEmcID getClusterId() const { return fClusterId; }
   void ClusterId(const RecEmcID id);
   //Cluster energy
   RecEmcEnergy getEnergy() const;
   //Cluster position
   HepPoint3D getPosition() const;
   //Second moment
   double getSecondMoment() const;

   //Read hit map of a cluster
   //Only const_iterator is supplied.
   //Use Insert and Erase to add and delete hit.
   RecEmcHitMap::const_iterator Begin() const;
   RecEmcHitMap::const_iterator End() const;
   RecEmcHitMap::const_iterator Find(const RecEmcID& CellId) const;
      
   //Append and remove a hit
   void Insert(const RecEmcHit& aHit);
   void Erase(const RecEmcHit& aHit);
   
   //Use Insert and Erase to add and delete seed.
   RecEmcHitMap::const_iterator BeginSeed() const;
   RecEmcHitMap::const_iterator EndSeed() const;
   RecEmcHitMap::const_iterator FindSeed(const RecEmcID& CellId) const;
   int getSeedSize() const;
      
   //Append and remove a seed
   void InsertSeed(const RecEmcHit& aSeed);

   int getShowerSize() const;
   void InsertShowerId(const RecEmcID id);
   vector<RecEmcID> getShowerIdVec() const { return fShowerIdVec; }
   void ShowerIdVec(const vector<RecEmcID>& showerIdVec) { fShowerIdVec=showerIdVec; }
      
   //Dump out
   void Dump() const;
   
 protected:
   //fClusterId: the minimum CellId.
   RecEmcID fClusterId;
   //fClusterHitMap: hit members of a cluster.
   RecEmcHitMap fHitMap;
   //fSeedMap: seed members of a cluster.
   RecEmcHitMap fSeedMap;
   //fShowerMap: shower members of a cluster.
   //map<RecEmcID,RecEmcShower*,less<RecEmcID> > fShowerMap;
   vector<RecEmcID> fShowerIdVec;
};

ostream& operator<<(ostream & os, const RecEmcCluster& aCluster);

typedef map<RecEmcID,RecEmcCluster,less<RecEmcID> > RecEmcClusterMap;
typedef ObjectVector<RecEmcCluster> RecEmcClusterCol;

#endif  // REC_EMC_CLUSTER_H
