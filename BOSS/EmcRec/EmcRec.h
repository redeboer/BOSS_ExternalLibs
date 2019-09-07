//
//  Emc Rec
//  
//  Created by Zhe Wang, May 31, 2004
//
#ifndef EMC_REC_A_H
#define EMC_REC_A_H

#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/PropertyMgr.h"

#include "EmcRecEventModel/RecEmcEventModel.h"
#include "EmcRec/EmcRecDigit2Hit.h"
#include "EmcRec/EmcRecHit2Cluster.h"
#include "EmcRec/EmcRecCluster2ShowerAbs.h"

//Ntuple
#include "GaudiKernel/NTuple.h"

class IRawDataProviderSvc;

class EmcRec:public Algorithm {
 public:
   EmcRec(const std::string& name, ISvcLocator* pSvcLocator);
   StatusCode initialize();
   StatusCode execute();
   StatusCode finalize();
   
 private:
   // functionality members
   EmcRecDigit2Hit      fDigit2Hit;
   EmcRecHit2Cluster    fHit2Cluster;
   EmcRecCluster2ShowerAbs *fCluster2Shower;
   
   // data members
   RecEmcDigitMap   fDigitMap;
   RecEmcHitMap     fHitMap;
   RecEmcClusterMap fClusterMap;
   RecEmcShowerMap  fShowerMap;   
   
   // some control parameters
   IRawDataProviderSvc *m_rawDataProviderSvc;
   PropertyMgr   m_propMgr;
   int  m_event;
   int  fOutput;
   int  fEventNb;
   bool fDigiCalib;
   bool fTofEnergy;
   bool fOnlineMode;
   double fTimeMin;
   double fTimeMax;
   double fMethodMode;
   double fPosCorr;

   //MC reconstruction for electronics saturation
   // 0: using Bhabha calibration constants
   // 1: using Emaxdata from data
   int fElecSaturation;

   std::vector<std::string> fPositionMode;

#ifndef OnlineMode
   // others 
   NTuple::Tuple*  m_tuple;
   // mc truth
   NTuple::Item<long>  pid;
   NTuple::Item<float> tp;
   NTuple::Item<float> ttheta;
   NTuple::Item<float> tphi;
   //
   NTuple::Item<long> nrun;
   NTuple::Item<long> nrec;
   //
   NTuple::Item<long> nneu;
   NTuple::Item<long> npart;
   NTuple::Item<long> ntheta;
   NTuple::Item<long> nphi;
   NTuple::Item<long> ndigi;
   NTuple::Item<long> nhit;
   NTuple::Item<long> ncluster;
   // track 1
   NTuple::Array<float> pp1;
   NTuple::Item<float> theta1;
   NTuple::Item<float> phi1;
   NTuple::Item<float> dphi1;
   NTuple::Item<float> eseed;
   NTuple::Item<float> e3x3;
   NTuple::Item<float> e5x5;
   NTuple::Item<float> enseed;
   NTuple::Item<float> etof2x1;
   NTuple::Item<float> etof2x3;
   NTuple::Item<float> cluster2ndMoment;
   NTuple::Item<float> secondMoment;
   NTuple::Item<float> latMoment;
   NTuple::Item<float> a20Moment;
   NTuple::Item<float> a42Moment;
   NTuple::Item<float> mpi0;
   NTuple::Item<long> thtgap1;
   NTuple::Item<long> phigap1;   
   
   // track 2
   NTuple::Array<float> pp2; 
#endif
};

#endif /// EMC_REC_A_H

