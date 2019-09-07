//---------------------------------------------------------------------------//
//      BOOST --- BESIII Object_Oriented Simulation Tool                     //
//---------------------------------------------------------------------------//
//Description:
//Author: Yuan Ye(yuany@mail.ihep.ac.cn)
//Created:  Oct.26, 2004
//Modified:
//Comment:
//---------------------------------------------------------------------------//

#include "BesMdcCalTransfer.hh"
#include "G4ios.hh"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/ISvcLocator.h"

BesMdcCalTransfer::BesMdcCalTransfer(){
  StatusCode sc= Gaudi::svcLocator()->service("MdcCalibFunSvc",mdcCalFunSvc);
  if (!sc.isSuccess())
    std::cout <<" MdcCal::Could not open Cal Service"<<std::endl;
}

BesMdcCalTransfer::~BesMdcCalTransfer(){}

void BesMdcCalTransfer::SetHitPointer(BesMdcHit* hit){
      layerId = hit->GetLayerNo();
      cellId = hit->GetCellNo();
      driftD = hit->GetDriftD();
      edep = hit->GetEdep();
      theta = hit->GetTheta();
      enterAngle = hit->GetEnterAngle();
      posFlag = hit->GetPosFlag();
      z = hit->GetPos().z();
}

void BesMdcCalTransfer::GetSigma(double& sigma1, double& sigma2,double& f){
  sigma1 = mdcCalFunSvc->getSigma1(layerId, posFlag, driftD,
				   enterAngle, theta, z, edep);
  sigma2 = mdcCalFunSvc->getSigma2(layerId, posFlag, driftD,
				   enterAngle, theta, z, edep);
  f = mdcCalFunSvc->getF(layerId, posFlag, driftD,
			 enterAngle, theta, z, edep);

 /*
  if(layerId<8){
    if(driftD<0.5){
      sigma1=0.112784;      sigma2=0.229274;      f=0.666;
    }else if(driftD<1.){
      sigma1=0.103123;      sigma2=0.269797;      f=0.934;
    }else if(driftD<1.5){
      sigma1=0.08276;        sigma2=0.17493;      f=0.89;
    }else if(driftD<2.){
      sigma1=0.070109;      sigma2=0.149859;      f=0.89;
    }else if(driftD<2.5){
      sigma1=0.064453;      sigma2=0.130149;      f=0.886;
    }else if(driftD<3.){
      sigma1=0.062383;      sigma2=0.138806;      f=0.942;
    }else if(driftD<3.5){
      sigma1=0.061873;      sigma2=0.145696;      f=0.946;
    }else if(driftD<4.){
      sigma1=0.061236;      sigma2=0.119584;      f=0.891;
    }else if(driftD<4.5){
      sigma1=0.066292;      sigma2=0.148426;      f=0.917;
    }else if(driftD<5.){
      sigma1=0.078074;      sigma2=0.188148;      f=0.911;
    }else if(driftD<5.5){
      sigma1=0.088657;      sigma2=0.27548;      f=0.838;
    }else{
      sigma1=0.093089;      sigma2=0.115556;      f=0.367;
    }
  }else{
   if(driftD<0.5){
      sigma1=0.112433;      sigma2=0.327548;      f=0.645;
    }else if(driftD<1.){
      sigma1=0.096703;      sigma2=0.305206;      f=0.897;
    }else if(driftD<1.5){
      sigma1=0.082518;      sigma2=0.248913;      f= 0.934;
    }else if(driftD<2.){
      sigma1=0.072501;      sigma2=0.153868;      f= 0.899;
    }else if(driftD<2.5){
      sigma1= 0.065535;     sigma2=0.14246;       f=0.914;
    }else if(driftD<3.){
      sigma1=0.060497;      sigma2=0.126489;      f=0.918;
    }else if(driftD<3.5){
      sigma1=0.057643;      sigma2= 0.112927;     f=0.892;
    }else if(driftD<4.){
      sigma1=0.055266;      sigma2=0.094833;      f=0.887;
    }else if(driftD<4.5){
      sigma1=0.056263;      sigma2=0.124419;      f= 0.932;
    }else if(driftD<5.){
      sigma1=0.056599;      sigma2=0.124248;      f=0.923;
    }else if(driftD<5.5){
      sigma1= 0.061377;     sigma2=0.146147;      f=0.964;
    }else if(driftD<6.){
      sigma1=0.063978;      sigma2=0.150591;      f=0.942;
    }else if(driftD<6.5){
      sigma1=0.072951;      sigma2=0.15685;       f=0.913;
    }else if(driftD<7.){
      sigma1=0.085438;      sigma2=0.255109;      f=0.931;
    }else if(driftD<7.5){
      sigma1=0.101635;      sigma2=0.315529;      f=0.878;
    }else{
      sigma1=0.149529;      sigma2=0.374697;      f=0.89;
    }
  }
  */
}

double BesMdcCalTransfer::D2T(double driftDNew){
  double driftT;
  driftT= mdcCalFunSvc->distToDriftTime(driftDNew, layerId, cellId, posFlag, enterAngle);
    //  driftT  = driftD/40.*1000;
  /*
  G4double c[8],x1;
  c[0]=0.; c[1]=1.96061432e01.;c[2]=2.31378106.; c[3]=3.59120632e-1; c[0]=0.;c[0]=0.;c[0]=0.;c[0]=0.;x1=5.924711;
  driftT=0.;
  if(driftD<x1){
    for(int i=0;i<7;i++){
      driftT+=c[i]*driftD;
    }
  }else{
    for(int i=0;i<8;i++){
      driftT+=c[i]*driftD;
    }
  }
  */
  return driftT;
}

double BesMdcCalTransfer::GetT0(){
  return mdcCalFunSvc->getT0(layerId, cellId);
}

double BesMdcCalTransfer::GetTimeWalk(){
  return mdcCalFunSvc->getTimeWalk(layerId, edep);
}

double BesMdcCalTransfer::GetEff(){
  //  return mdcCalFunSvc->getEff(layerId, cellId);
  return 1.;
}


