//
//  Bes EMC Rec Cluster
//
//  Created by Zhe Wang 2003, 10, 1
//
#include <iostream>

#include "EmcRecEventModel/RecEmcCluster.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

//Constructors and destructors
RecEmcCluster::RecEmcCluster()
{
  Clear();
}

RecEmcCluster::~RecEmcCluster()
{
  Clear();
}

//Copy and assignment                                                     
/*RecEmcCluster::RecEmcCluster(const RecEmcCluster& aCluster)
  :ContainedObject(aCluster)
{  
  RecEmcHitMap::const_iterator pHitMap;

  Clear();

  for(pHitMap=aCluster.Begin();
      pHitMap!=aCluster.End();
      pHitMap++)
  {
    Insert(pHitMap->second);
  }

  RecEmcHitMap::const_iterator pSeedMap;
  for(pSeedMap=aCluster.BeginSeed();
      pSeedMap!=aCluster.EndSeed();
      pSeedMap++)
  {
    InsertSeed(pSeedMap->second);
  }

  map<RecEmcID,RecEmcShower*,less<RecEmcID> >::const_iterator pShowerMap;
  for(pShowerMap=aCluster.BeginShower();
      pShowerMap!=aCluster.EndShower();
      pShowerMap++)
  {
    InsertShower(pShowerMap->second);
  }

  fClusterId=aCluster.ClusterId();
}

RecEmcCluster& RecEmcCluster::operator=(const RecEmcCluster& aCluster)
{
  RecEmcHitMap::const_iterator pHitMap;

  Clear();

  if(this!=&aCluster)
  {
    for(pHitMap=aCluster.Begin();
        pHitMap!=aCluster.End();
        pHitMap++)
    {
      Insert(pHitMap->second);
    }

    RecEmcHitMap::const_iterator pSeedMap;
    for(pSeedMap=aCluster.BeginSeed();
        pSeedMap!=aCluster.EndSeed();
        pSeedMap++)
    {
      InsertSeed(pSeedMap->second);
    }

    map<RecEmcID,RecEmcShower*,less<RecEmcID> >::const_iterator pShowerMap;
    for(pShowerMap=aCluster.BeginShower();
        pShowerMap!=aCluster.EndShower();
        pShowerMap++)
    {
      InsertShower(pShowerMap->second);
    }

    fClusterId=aCluster.ClusterId();
  }

  return *this;
}*/

//Other methods
void RecEmcCluster::Clear()
{
  fClusterId.clear();
  fHitMap.clear();
  fSeedMap.clear();
  fShowerIdVec.clear();
}

//Access a cluster
//RecEmcID RecEmcCluster::ClusterId() const
//{
//  return fClusterId;
//}

void RecEmcCluster::ClusterId(const RecEmcID id)
{
  fClusterId=id;
}

RecEmcHitMap::const_iterator RecEmcCluster::Begin() const
{
  return fHitMap.begin();
}

RecEmcHitMap::const_iterator RecEmcCluster::End() const
{
  return fHitMap.end();
}

RecEmcHitMap::const_iterator RecEmcCluster::Find(const RecEmcID& CellId) const
{   
  // If failed the return vale is End().
  return fHitMap.find(CellId);
}

//Insert and Erase a hit  
void RecEmcCluster::Insert(const RecEmcHit& aHit)
{
  fHitMap[aHit.getCellId()]=aHit;
  //fClusterId=fHitMap.begin()->first;
  return;
}

void RecEmcCluster::Erase(const RecEmcHit& aHit)
{      
  RecEmcHitMap::const_iterator pHitMap;
  pHitMap=fHitMap.find(aHit.getCellId());

  //blank HitMap
  if(fHitMap.empty()) 
  {
    return;
  }

  //not find 
  if(pHitMap==End()) 
  {
    return;
  }

  //find it
  if(pHitMap!=End())
  {
    fHitMap.erase(pHitMap->first);
    //empty
    if(fHitMap.empty())
    {
      Clear();
      return;
    }
    //not empty
    else
    {
      fClusterId=fHitMap.begin()->first;
      return;
    }
  }
}

//======== Seed map
RecEmcHitMap::const_iterator RecEmcCluster::BeginSeed() const
{
  return fSeedMap.begin();
}

RecEmcHitMap::const_iterator RecEmcCluster::EndSeed() const
{
  return fSeedMap.end();
}

RecEmcHitMap::const_iterator RecEmcCluster::FindSeed(const RecEmcID& CellId) const
{   
  // If failed the return vale is End().
  return fSeedMap.find(CellId);
}

//Insert and Erase a seed  
void RecEmcCluster::InsertSeed(const RecEmcHit& aSeed)
{
  fSeedMap[aSeed.getCellId()]=aSeed;
  return;
}

int RecEmcCluster::getSeedSize() const
{
  return fSeedMap.size();
}

int RecEmcCluster::getShowerSize() const
{
  return fShowerIdVec.size();
}

//Insert and Erase a shower  
void RecEmcCluster::InsertShowerId(const RecEmcID id)
{
  fShowerIdVec.push_back(id);
}

//Cluster energy
RecEmcEnergy RecEmcCluster::getEnergy() const
{
  RecEmcHitMap::const_iterator pHitMap;
  double etot=0;

  for(pHitMap=fHitMap.begin();
      pHitMap!=fHitMap.end();
      pHitMap++) {
    etot+=pHitMap->second.getEnergy();
  }
  return etot;
}

//Cluster position
HepPoint3D RecEmcCluster::getPosition() const
{
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  RecEmcHitMap::const_iterator pHitMap;
  HepPoint3D pos(0,0,0);
  HepPoint3D possum(0,0,0);
  double etot=0;

  for(pHitMap=fHitMap.begin();
      pHitMap!=fHitMap.end();
      pHitMap++) {
    etot+=pHitMap->second.getEnergy();
    pos=iGeoSvc->GetCFrontCenter(pHitMap->second.getCellId());
    possum+=pos*pHitMap->second.getEnergy();
  }

  if(etot>0) {
    possum/=etot;
  }
  return possum;
}

//Cluster second moment
double RecEmcCluster::getSecondMoment() const
{
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  double etot=0;
  double sum=0;
  HepPoint3D center(getPosition());
  RecEmcHitMap::const_iterator pHitMap;

  for(pHitMap=fHitMap.begin();
      pHitMap!=fHitMap.end();
      pHitMap++) {
    HepPoint3D pos(pHitMap->second.getFrontCenter());
    etot+=pHitMap->second.getEnergy();
    sum+=pHitMap->second.getEnergy()*pos.distance2(center);
  }

  if(etot>0) {
    sum/=etot;
  }
  return sum;
}

//Dump out.
void RecEmcCluster::Dump() const
{
  RecEmcHitMap::const_iterator pHitMap;

  cout<<"EMC Cluster: ";

  cout<<"Cluster Id= ";
  cout<<fClusterId<<endl;

  for(pHitMap=fHitMap.begin();
      pHitMap!=fHitMap.end();
      pHitMap++)
  {
    pHitMap->second.Dump();
  }
}

ostream& operator<<(ostream & os, const RecEmcCluster& aCluster)
{
  RecEmcHitMap::const_iterator pHitMap;

  cout<<"EMC Cluster: ";

  cout<<"Cluster Id= ";
  cout<<aCluster.getClusterId()<<endl;

  for(pHitMap=aCluster.Begin();
      pHitMap!=aCluster.End();
      pHitMap++)
  {
    os<< (pHitMap->second);
  }

  if(aCluster.getSeedSize()>0) {
    cout<<"Contains "<<aCluster.getSeedSize()<<" Seeds:"<<endl;
    RecEmcHitMap::const_iterator pSeedMap;
    for(pSeedMap=aCluster.BeginSeed();
        pSeedMap!=aCluster.EndSeed();
        pSeedMap++)
    {
      os<< (pSeedMap->second);
    }
  }

  if(aCluster.getShowerSize()>0) {
    vector<RecEmcID> aShowerIdVec=aCluster.getShowerIdVec();
    vector<RecEmcID>::iterator iShowerId;
    os<<"Contains "<<aCluster.getShowerSize()<<" Showers:"<<endl;
    for(iShowerId=aShowerIdVec.begin();
        iShowerId!=aShowerIdVec.end();
        iShowerId++) {
      os << *iShowerId <<endl;
    }
  }

  return os;
}
