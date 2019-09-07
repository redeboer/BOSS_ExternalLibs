//
//  Bes Rec EMC Shower
//
//  Created by Zhe Wang 2003, 10, 1
//
#include <iostream>
#include <complex>

#include "EmcRecEventModel/RecEmcShower.h"

//Constructors and destructors
RecEmcShower::RecEmcShower()
{
  Clear();
}

RecEmcShower::~RecEmcShower()
{
  //Clear();
}

//Copy and assignment                                                        
RecEmcShower::RecEmcShower(const DstEmcShower& aShower)
  :DstEmcShower(aShower)
{
  Clear();
  fShowerId=RecEmcID(aShower.cellId());
}

RecEmcShower& RecEmcShower::operator=(const DstEmcShower& aShower)
{ 
  Clear();
  DstEmcShower::operator=(aShower);
  fShowerId=RecEmcID(aShower.cellId());
  return *this;
}

/*RecEmcShower::RecEmcShower(const RecEmcShower& aShower)
  :ContainedObject(aShower),DstEmcShower(aShower)
{  
  *this = aShower;
}

RecEmcShower& RecEmcShower::operator=(const RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator pFractionMap;

  Clear();

  if(this!=&aShower)
  {
    DstEmcShower::operator=(aShower);

    for(pFractionMap=aShower.Begin();
        pFractionMap!=aShower.End();
        pFractionMap++){
      Insert(pFractionMap->second);
    }
    fCellId3x3=aShower.CellId3x3();
    fCellId5x5=aShower.CellId5x5();
    fTofShower=aShower.TofShower();
    fShowerId=aShower.ShowerId();
    fClusterId=aShower.ClusterId();
    fCluster=aShower.Cluster();
    fEAll=aShower.EAll();
    fELepton=aShower.ELepton();
    fETof2x1=aShower.ETof2x1();
    fETof2x3=aShower.ETof2x3();
    //fPosition=aShower.Position();
    fNearestSeed=aShower.NearestSeed();
    fThetaGap=aShower.ThetaGap();
    fPhiGap=aShower.PhiGap();
  }

  return *this;
}*/

//Other methods
void RecEmcShower::Clear()
{
  fShowerId.clear();
  fClusterId.clear();
  fCellId3x3.clear();
  fCellId5x5.clear();
  fFractionMap.clear();
  //
  fCluster=NULL;
  fTofShower=false;
  fEAll=0;
  fELepton=0;
  fETof2x1=0;
  fETof2x3=0;
  //
  //HepPoint3D o(0,0,0);
  //fPosition=o;
  //   
  fNearestSeed.clear();
  fThetaGap=0;
  fPhiGap=0;
}

//Access a Shower
RecEmcID RecEmcShower::ShowerId(RecEmcID id)
{
  return fShowerId=id;
}

//Access a Cluster
void RecEmcShower::ClusterId(const RecEmcID id)
{
  fClusterId=id;
}

void RecEmcShower::Cluster(RecEmcCluster* pCluster)
{
  fCluster=pCluster;
}

void RecEmcShower::CellId3x3(RecEmcIDVector& id3x3)
{
  fCellId3x3.clear();
  fCellId3x3=id3x3;
}

void RecEmcShower::CellId5x5(RecEmcIDVector& id5x5)
{
  fCellId5x5.clear();
  fCellId5x5=id5x5;
}

RecEmcFractionMap::const_iterator RecEmcShower::Begin() const
{
  return fFractionMap.begin();
}

RecEmcFractionMap::const_iterator RecEmcShower::End() const
{
  return fFractionMap.end();
}

RecEmcFractionMap::const_iterator RecEmcShower::Find(const RecEmcID& CellId) const
{   
  // If failed the return vale is End().
  return fFractionMap.find(CellId);
}

//Insert and Erase a Fraction  
void RecEmcShower::Insert(const RecEmcFraction& aFraction)
{
  fFractionMap[aFraction.getCellId()]=aFraction;
  return;
}

void RecEmcShower::Erase(const RecEmcFraction& aFraction)
{      
  RecEmcFractionMap::const_iterator pFractionMap;
  pFractionMap=fFractionMap.find(aFraction.getCellId());

  //blank FractionMap
  if(fFractionMap.empty()) 
  {
    return;
  }

  //not find 
  if(pFractionMap==End()) 
  {
    return;
  }

  //find it
  if(pFractionMap!=End())
  {
    fFractionMap.erase(pFractionMap->first);
    //empty
    if(fFractionMap.empty())
    {
      Clear();
      return;
    }
    //not empty
    else
    {
      return;
    }
  }
}

//Dump out.
void RecEmcShower::Dump() const
{
  RecEmcFractionMap::const_iterator pFractionMap;

  cout<<"EMC Shower: ";

  cout<<"Shower Id= ";
  cout<<fShowerId<<endl;

  for(pFractionMap=fFractionMap.begin();
      pFractionMap!=fFractionMap.end();
      pFractionMap++)
  {
    pFractionMap->second.Dump();
  }
}


//physics attribute
unsigned int RecEmcShower::getSize() const
{
  return fFractionMap.size();
}

RecEmcFractionMap RecEmcShower::getFractionMap() const
{
  RecEmcFractionMap tmpFractionMap;
  RecEmcFractionMap::const_iterator pFractionMap;

  for(pFractionMap=fFractionMap.begin();
      pFractionMap!=fFractionMap.end();
      pFractionMap++){
    RecEmcFraction aFraction=pFractionMap->second;
    tmpFractionMap[aFraction.getCellId()]=aFraction;
  }

  return tmpFractionMap;
}

RecEmcFractionMap RecEmcShower::getFractionMap3x3() const
{
  RecEmcFractionMap tmpFractionMap;
  if(fFractionMap.empty()) return tmpFractionMap;

  RecEmcFractionMap::const_iterator pFractionMap;
  ci_RecEmcIDVector pNearCell;

  for(pFractionMap=fFractionMap.begin();
      pFractionMap!=fFractionMap.end();
      pFractionMap++){
    for(pNearCell=fCellId3x3.begin();
        pNearCell!=fCellId3x3.end();
        pNearCell++){
      if(pFractionMap->second.getCellId()==*pNearCell){
        RecEmcFraction aFraction=pFractionMap->second;
        tmpFractionMap[aFraction.getCellId()]=aFraction;
      }
    }
  }

  return tmpFractionMap;
}

RecEmcFractionMap RecEmcShower::getFractionMap5x5() const
{
  RecEmcFractionMap tmpFractionMap;
  if(fFractionMap.empty()) return tmpFractionMap;
  
  RecEmcFractionMap::const_iterator pFractionMap;
  ci_RecEmcIDVector pNearCell;

  for(pFractionMap=fFractionMap.begin();
      pFractionMap!=fFractionMap.end();
      pFractionMap++){
    for(pNearCell=fCellId5x5.begin();
        pNearCell!=fCellId5x5.end();
        pNearCell++){
      if(pFractionMap->second.getCellId()==*pNearCell){
        RecEmcFraction aFraction=pFractionMap->second;
        tmpFractionMap[aFraction.getCellId()]=aFraction;
      }
    }
  }

  return tmpFractionMap;
}

//////////////////////////////////////////////////
RecEmcEnergy RecEmcShower::EAll(RecEmcEnergy e)
{
  return fEAll=e;
}
RecEmcEnergy RecEmcShower::ELepton(RecEmcEnergy e)
{
  return fELepton=e;
}
RecEmcEnergy RecEmcShower::ETof2x1(RecEmcEnergy e)
{
  return fETof2x1=e;
}
RecEmcEnergy RecEmcShower::ETof2x3(RecEmcEnergy e)
{
  return fETof2x3=e;
}
///////////////////////////////////////////////

RecEmcID RecEmcShower::NearestSeed() const
{
  return fNearestSeed;
}

RecEmcID RecEmcShower::NearestSeed(const RecEmcID& ns)
{
  return fNearestSeed=ns;
}

int RecEmcShower::ThetaGap() const
{
  return fThetaGap;
}

int RecEmcShower::ThetaGap(int ThetaGap)
{
  return fThetaGap=ThetaGap;
}

int RecEmcShower::PhiGap() const
{
  return fPhiGap;
}

int RecEmcShower::PhiGap(int PhiGap)
{
  return fPhiGap=PhiGap;
}

ostream& operator<<(ostream & os, const RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator pFractionMap;
  RecEmcFractionMap::const_iterator ciFractionMap3x3;
  RecEmcFractionMap::const_iterator ciFractionMap5x5;

  os<<"------------------RecEmcShower:"<<endl;
  os<<"Track id: "<<aShower.trackId()<<", number of hits: "<<aShower.getSize()
    <<", status: "<<aShower.status()<<", shower id: "<<aShower.getShowerId()
    <<", cluster id: "<<aShower.getClusterId()<<", module: "<<aShower.module()
    <<", time: "<<aShower.time()<<endl;
  os<<"Energy: "<<aShower.energy()<<", de: "<<aShower.dE()
    <<", eseed: "<<aShower.eSeed()
    <<", e3x3: "<<aShower.e3x3()<<", e5x5: "<<aShower.e5x5()
    <<", eall: "<<aShower.getEAll()<<", elepton: "<<aShower.getELepton()<<endl;
  os<<"Position: "<<aShower.position()
    <<", theta: "<<aShower.theta()<<", phi: "<<aShower.phi()
    <<"\ndx: "<<aShower.dx()<<", dy: "<<aShower.dy()<<", dz: "<<aShower.dz()
    <<", dtheta: "<<aShower.dtheta()<<", dphi: "<<aShower.dphi()<<endl;
  os<<"Second moment: "<<aShower.secondMoment()
    <<", lat moment: "<<aShower.latMoment()
    <<", a20 moment: "<<aShower.a20Moment()
    <<", a42 moment: "<<aShower.a42Moment()<<endl;
  os<<"Error matrix: \n"<<aShower.errorMatrix()<<endl;

  if(aShower.getSize()>0) {
    os<<"Fraction Map: "<<endl;
    for(pFractionMap=aShower.Begin();
        pFractionMap!=aShower.End();
        pFractionMap++){
      os<< (pFractionMap->second);
    }
  }

  RecEmcFractionMap fracMap = aShower.getFractionMap3x3();
  if(fracMap.size()>0) {
    os<<"Fraction Map 3x3: "<<endl; 
    for(ciFractionMap3x3=fracMap.begin();
        ciFractionMap3x3!=fracMap.end();
        ciFractionMap3x3++){
      os<< (ciFractionMap3x3->second);
    }
  }

  fracMap = aShower.getFractionMap5x5();
  if(fracMap.size()>0) {
    os<<"Fraction Map 5x5: "<<endl; 
    for(ciFractionMap5x5=fracMap.begin();
        ciFractionMap5x5!=fracMap.end();
        ciFractionMap5x5++){
      os<< (ciFractionMap5x5->second);
    }
  }

  os<<endl;

  return os;
}

