//
//  Spliter
//  Simple weighted method
//
//  Zhe Wang 2004, 8, 31
//
#include <iostream>

#include "EmcRec/EmcRecSplitWeighted.h"
#include "EmcRec/EmcRecShowerEnergy.h"
#include "EmcRec/EmcRecShowerPosLin.h"
#include "EmcRec/EmcRecShowerPosLog.h"
#include "EmcRec/EmcRecShowerPosLoglin.h"
#include "EmcRec/EmcRecShowerPosLinShMax.h"
#include "EmcRec/EmcRecShowerPosLogShMax.h"
#include "EmcRec/EmcRecShowerShape.h"
#include "EmcRec/EmcRecParameter.h"
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

// Constructors and destructors
EmcRecSplitWeighted::EmcRecSplitWeighted()
{
  fShowerE=new EmcRecShowerEnergy;
  fShowerShape=new EmcRecShowerShape;
  
  EmcRecParameter& Para=EmcRecParameter::GetInstance();
//  if(Para.PositionMode1()=="lin") {
//    fShowerPos=new EmcRecShowerPosLin;
//  } else {
//    fShowerPos=new EmcRecShowerPosLog;
//  }

  if(Para.PositionMode1()=="lin") {
    fShowerPos=new EmcRecShowerPosLin;
  } else if(Para.PositionMode1()=="log"){
    fShowerPos=new EmcRecShowerPosLog;
  } else if(Para.PositionMode1()=="loglin"){
    fShowerPos=new EmcRecShowerPosLoglin;
  }   else if(Para.PositionMode1()=="linsh"){
    fShowerPos=new EmcRecShowerPosLinShMax;
  } else if(Para.PositionMode1()=="logsh"){
    fShowerPos=new EmcRecShowerPosLogShMax;
  }

}


EmcRecSplitWeighted::~EmcRecSplitWeighted()
{
   delete fShowerE;
   delete fShowerPos;
   delete fShowerShape;
}

void EmcRecSplitWeighted::Split(RecEmcCluster& aCluster,
				const RecEmcIDVector& aMaxVec,
				RecEmcShowerMap& aShowerMap)
{
   //A exponential function will be used to describe the shape of a shower.
   //Energy fall off with distance from centre of shower is exp(-a*dist/r)
   //dist: distance from center
   //r: moliere radius
   //a: a parameter, needed to be optimised
  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  //Calculate the cut of second moment
  double eCluster=aCluster.getEnergy();
  double cut=0;
  if(eCluster>Para.SmCut(3)) {
    cut=Para.SmCut(0)*exp(-(eCluster)/Para.SmCut(1))+Para.SmCut(2);
  } else {
    cut=Para.SmCut(0)*exp(-(Para.SmCut(3))/Para.SmCut(1))+Para.SmCut(2);  //constant
  }
  cut/=10;
 
  RecEmcShower aShower;
  RecEmcHit aHit;
  RecEmcFraction aFraction;
  RecEmcHitMap::const_iterator ciHitMap;
  if(aMaxVec.size()==0) {
    // the cluster is abandoned
  }
   
  // only one seed or second moment belows cut
  else if(aMaxVec.size()==1||aMaxVec.size()>20||aCluster.getSecondMoment()<cut) {
    aShower.Clear();
    aShower.setStatus(1);
    // ID
    aShower.ShowerId(aMaxVec[0]);
    aShower.ClusterId(aCluster.getClusterId());
    aCluster.InsertShowerId(aMaxVec[0]);
    // each fraction
    double time=aCluster.Find(aMaxVec[0])->second.getTime();
    if(time>=Para.TimeMin()&&time<=Para.TimeMax()) {
      for(ciHitMap=aCluster.Begin();
          ciHitMap!=aCluster.End();
	        ++ciHitMap) {
	      aHit=ciHitMap->second;
        //if((aHit.time()>=Para.TimeMin()) &&
           //(aHit.time()<=Para.TimeMax())){
	        aFraction=aHit;
	        aFraction.Fraction(1);
	        aShower.Insert(aFraction);
        //}
      }
      
      aShower.setTime(time);
      fShowerE->Energy(aShower);
      fShowerPos->Position(aShower);
      fShowerShape->CalculateMoment(aShower);
      aShower.ThetaGap(9);
      aShower.PhiGap(9);
      // push it into map
      aShowerMap[aMaxVec[0]]=aShower;
    }
  }


  // two or more seeds and second moment beyonds cut
  else if(aMaxVec.size()>1&&aMaxVec.size()<=20&&aCluster.getSecondMoment()>cut) {
    //cout<<"In EmcRecSplitWeighted: ";
    //cout<<aMaxVec.size();
    //cout<<" seeds in a cluster"<<endl;      

    RecEmcCluster tmpCluster=aCluster;
    RecEmcHitMap::const_iterator ci_HitMap;
    ci_RecEmcIDVector ciMax, ciMax1;;
    RecEmcShower aShower;
    RecEmcShowerMap tmpShowerMap;
    RecEmcShowerMap::iterator i_ShowerMap,i2_ShowerMap;      
    
    RecEmcFraction aFrac;
    unsigned int iterations=0;  //iteration time
    double centroidShift;   //center shift between 2 iterations
    map<RecEmcID,HepPoint3D,less<RecEmcID> > showerCentroid;  //shower center
    map<RecEmcID,HepPoint3D,less<RecEmcID> >::const_iterator ci_showerCentroid;
    
    IEmcRecGeoSvc* iGeoSvc;
    ISvcLocator* svcLocator = Gaudi::svcLocator();
    StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
    if(sc!=StatusCode::SUCCESS) {
      //cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
    }

    //Preparation
    for(ciMax=aMaxVec.begin();
        ciMax!=aMaxVec.end();
        ++ciMax) {
      //initialize: seed's front center
      showerCentroid[*ciMax]=iGeoSvc->GetCFrontCenter(*ciMax);
    }
    do {
      centroidShift=0.;
      tmpShowerMap.clear();
      for(ciMax=aMaxVec.begin();
          ciMax!=aMaxVec.end();
          ++ciMax) {
        double weightTotal=0.,weight=0.;

        aShower.Clear();
        aShower.ShowerId(*ciMax);
        
        // figure out the weight for each cell for each seed
        for(ci_HitMap=tmpCluster.Begin();
            ci_HitMap!=tmpCluster.End();
            ++ci_HitMap) {

          aFrac=ci_HitMap->second;
          double aDistance=0;
          RecEmcEnergy aESeed=0;

          bool isSeed=false;
          for(ciMax1=aMaxVec.begin();
              ciMax1!=aMaxVec.end();
              ++ciMax1) {
            HepPoint3D seedPoint(showerCentroid[*ciMax1]);
            HepPoint3D hitPoint(iGeoSvc->GetCFrontCenter(aFrac.getCellId()));

            RecEmcEnergy theESeed=tmpCluster.Find(*ciMax1)->second.getEnergy();
            double theDistance;
            if(*ciMax1==aFrac.getCellId()) {
              isSeed=true;
              theDistance=0.;
            } else {
              theDistance=(hitPoint-seedPoint).mag();
            }
          
            if(*ciMax1==*ciMax) {
              aDistance=theDistance;
              aESeed=theESeed;
            }
            
            weightTotal+=theESeed*exp(-Para.LateralProfile()*theDistance/Para.MoliereRadius());
          }

          weight=aESeed*exp(-Para.LateralProfile()*aDistance/Para.MoliereRadius())/weightTotal;
          aFrac.Fraction(weight);
          //if((aFrac.time()>=Para.TimeMin() &&
              //aFrac.time()<=Para.TimeMax()) ||
              //isSeed) {
            if(aFrac.getEnergy()*aFrac.getFraction()>Para.ElectronicsNoiseLevel()) {
              aShower.Insert(aFrac);
            }
          //}
          weightTotal=0;
        }

        fShowerE->Energy(aShower);
        fShowerPos->Position(aShower);
        HepPoint3D newCentroid(aShower.position());
        HepPoint3D oldCentroid(showerCentroid[*ciMax]);
        centroidShift+=(newCentroid-oldCentroid).mag();
        tmpShowerMap[aShower.getShowerId()]=aShower;
        showerCentroid[*ciMax]=newCentroid;
      }

      centroidShift/=(double)aMaxVec.size();
      for(ci_showerCentroid=showerCentroid.begin();
          ci_showerCentroid!=showerCentroid.end();
          ci_showerCentroid++){
        showerCentroid[ci_showerCentroid->first]
          =tmpShowerMap[ci_showerCentroid->first].position();
      }
      iterations++;
      //cout<<"iterations: "<<iterations<<"\tcentroidShift: "<<centroidShift<<endl;
    }     
    while((iterations<8)&&(centroidShift>0.01));
      
    unsigned int tht,phi;
    unsigned int tht2,phi2;
    unsigned int dtht,dphi;
    unsigned int thetagap=0,phigap=0;
    double distmin,dist;
    RecEmcID id,id2,nearest;
    // shower attributes
    for(i_ShowerMap=tmpShowerMap.begin();
    	  i_ShowerMap!=tmpShowerMap.end();
	      ++i_ShowerMap) {
      aCluster.InsertShowerId(i_ShowerMap->second.getShowerId());
      i_ShowerMap->second.ClusterId(aCluster.getClusterId());
      i_ShowerMap->second.setStatus(2);
      fShowerE->Energy(i_ShowerMap->second);
      fShowerPos->Position(i_ShowerMap->second);
      fShowerShape->CalculateMoment(i_ShowerMap->second);
	    //
	    // min theta, phi gap
	    id=(i_ShowerMap->second).getShowerId();
	    tht=EmcID::theta_module(id);
	    phi=EmcID::phi_module(id);
	    distmin=999999;
	    for(i2_ShowerMap=tmpShowerMap.begin();
	        i2_ShowerMap!=tmpShowerMap.end();
	        ++i2_ShowerMap) {
	      id2=(i2_ShowerMap->second).getShowerId();
	      tht2=EmcID::theta_module(id2);
	      phi2=EmcID::phi_module(id2);
	      if(id!=id2) {
	        dtht=tht>tht2 ? tht-tht2 : tht2-tht;
	        dphi=phi>phi2 ? phi-phi2 : phi2-phi;
	        if(dphi>(EmcID::getPHI_BARREL_MAX()+1)/2) dphi=EmcID::getPHI_BARREL_MAX()+1-dphi;
	        dist=sqrt(double(dtht*dtht+dphi*dphi));
	        if(dist<distmin) {
		        distmin=dist;
		        nearest=id2;
		        if(dtht>6) dtht=6;
		        if(dphi>6) dphi=6;
		        thetagap=dtht;
		        phigap=dphi;
	        }
	      }
      }
	    //cout<<"+++++"<<id<<" "<<thetagap<<" "<<phigap<<endl;
	    i_ShowerMap->second.NearestSeed(nearest);
	    i_ShowerMap->second.ThetaGap(thetagap);
	    i_ShowerMap->second.PhiGap(phigap);
	 
	    // save it
      double time=
        i_ShowerMap->second.Find(i_ShowerMap->second.getShowerId())->second.getTime();
      if(time>=Para.TimeMin() && time<=Para.TimeMax() &&
         (i_ShowerMap->second.getEAll()>Para.EThresholdCluster())) {
        i_ShowerMap->second.setTime(time);
        aShowerMap[i_ShowerMap->first]=i_ShowerMap->second;
      }
    }
    tmpShowerMap.clear();
  }
  
}
