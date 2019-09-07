//
//    Linear weighted position attribute reconstruction
//
//    Created by Zhe Wang 2004, 5, 16
//
#include "EmcRec/EmcRecShowerPosLin.h"

#include <iostream>
#include <fstream>

#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EmcRec/EmcRecParameter.h"

void EmcRecShowerPosLin::Position(RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator cit;
  HepPoint3D pos(0,0,0);
  HepPoint3D possum(0,0,0);
  double w,wsum;
  double etot;
  //cout<<"EmcRecShowerPosLin::Position Begin"<<endl; 
  
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
//    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  }

  EmcRecParameter& Para=EmcRecParameter::GetInstance();

  double e5x5 = aShower.e5x5();
  
  etot=0;

  for(cit=aShower.Begin();cit!=aShower.End();cit++){      
    etot+=(cit->second.getEnergy()*cit->second.getFraction());
  }
  wsum=0;
  for(cit=aShower.Begin();cit!=aShower.End();cit++){
    w=(cit->second.getEnergy()*cit->second.getFraction());
    wsum+=w;
    pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
    possum+=pos*w;
  }

  if(wsum<=0) {
    cout<<"[[[[[[[[[[[[[[["<<wsum;
  }

  pos=possum/wsum;
//  aShower.setPosition(pos);

  //PosCorr=0 without position correction
  //PosCorr=1 with position correction
  if(Para.PosCorr()==0){	aShower.setPosition(pos);}

  
  //----------------------------------------------------------------------
  //position correction
  RecEmcID id = aShower.getShowerId();
  const unsigned int module = EmcID::barrel_ec(id);;
  const unsigned int thetaModule = EmcID::theta_module(id);
  const unsigned int phiModule = EmcID::phi_module(id);
  HepPoint3D posCorr(pos);

  if(module==1) {   //barrel
    unsigned int theModule;
    if(thetaModule>21) {
      theModule = 43 - thetaModule;
      id = EmcID::crystal_id(module,theModule,phiModule);
      posCorr.setZ(-posCorr.z());
    } else {
      theModule = thetaModule;
    }
    //cout<<"EmcID: "<<id<<" theta: "<<theModule<<endl;

    EmcRecParameter& Para=EmcRecParameter::GetInstance();
    double IRShift, parTheta[5],parPhi[5];


    if(theModule==21) {
      IRShift = 0;
    } else if(theModule==20) {
      IRShift = 2.5;
    } else {
      IRShift = 5.;
    }
	
    
    if(Para.MethodMode()==0){

            
      for(int i=0;i<5;i++){
	if(theModule==21) {
	  double par[5] = { 1.698, -1.553, 0.9384, 0.1193, -0.01916 };
	  parTheta[i] = par[i];
	}else if(theModule==20){
	  double par[5] = { 1.593, -1.582, 0.8881, 0.3298, -0.08856 };
	  parTheta[i] = par[i];
	}else{
	  double par[5] = { 1.605, -1.702, 0.8433, 0.3072, -0.1809 };
	  parTheta[i] = par[i];
	}
      }

    } else if(Para.MethodMode()==1){

      for(int i=0;i<5;i++){
      
	if(e5x5>1.0)                      parTheta[i] = Para.BarrLinThetaPara(theModule,i);
	else if(e5x5<=1.0 && e5x5>0.5)    parTheta[i] = Para.BarrLinThetaPara(theModule+22,i);
	else if(e5x5<=0.5)	          parTheta[i] = Para.BarrLinThetaPara(theModule+44,i);
	    
       	if(e5x5>1.0)	                  parPhi[i] = Para.BarrLinPhiPara(0,i);
	else if(e5x5<=1.0 && e5x5>0.5)    parPhi[i] = Para.BarrLinPhiPara(1,i);
	else if(e5x5<=0.5)	          parPhi[i] = Para.BarrLinPhiPara(2,i);
	    
      }
    }
  


    HepPoint3D IR(0,0,IRShift);
    HepPoint3D center01, center23;  //center of point0,1 and point2,3 in crystal
    center01 = (iGeoSvc->GetCrystalPoint(id,0)+iGeoSvc->GetCrystalPoint(id,1))/2;
    center23 = (iGeoSvc->GetCrystalPoint(id,2)+iGeoSvc->GetCrystalPoint(id,3))/2;
    //cout<<"before correction: "<<(posCorr-IR).theta()<<"\t"<<posCorr.phi()<<endl;

    double theta01,theta23,length,d;
    theta01 = (center01-IR).theta();
    theta23 = (center23-IR).theta();
    length = (center01-IR).mag();
    d = length*tan(theta01-theta23);  //length in x direction
    //cout<<"theta01: "<<theta01<<"\t"<<" theta23: "<<theta23<<"\t"
    //<<" length: "<<length<<" d: "<<d<<endl;

    double xIn,xOut,deltaTheta;
    xIn = length*tan(theta01-(posCorr-IR).theta())-d/2;
    xOut = xIn-(parTheta[0]*atan(xIn*parTheta[1]-parTheta[4])+parTheta[2]*xIn+parTheta[3]);
    deltaTheta = atan((xOut+d/2)/length)-atan((xIn+d/2)/length);
    //cout<<"xIn: "<<xIn<<" xOut: "<<xOut<<" deltaTheta: "<<deltaTheta<<endl;

    double parPhi1[5] = { 0.7185, -2.539, 0.6759, 0.3472, -0.3108 };  //e-
    double parPhi2[5] = { 0.5781, -2.917, 0.5516, -0.5745, 0.3777 };  //e+

//    double parPhi[5];// = { 0.7723, -3.1, 0.6992, -0.1441, -0.01012 }; //by photon, not used

//    for(int i=0;i<5;i++) {
//      parPhi[i] = (parPhi1[i]+parPhi2[i])/2;  //average of e- and e+
//    }

    double yIn,yOut,deltaPhi;
//    yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.1537
//      : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.1537;
    if(phiModule==0&&posCorr.phi()<0){

      yIn = posCorr.phi()*180./CLHEP::pi+360.-119*3-1.843;

    }else if (phiModule==119&&posCorr.phi()>0){

      yIn = posCorr.phi()*180./CLHEP::pi-1.843;

    }else {

      yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
      : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.843;

    }

    yOut = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4])+parPhi[2]*yIn+parPhi[3]; 
    deltaPhi = yOut*CLHEP::pi/180.;
    //cout<<"yIn="<<yIn<<"\tyOut="<<yOut<<"\tdeltaPhi="<<deltaPhi<<endl;

    HepPoint3D rotateVector(0,1,0);   //y axis
    rotateVector.rotateZ(posCorr.phi());
    posCorr.setZ(posCorr.z()-IRShift);
    posCorr.rotate(-deltaTheta,rotateVector);
    posCorr.setZ(posCorr.z()+IRShift);
    if(Para.MethodMode()==0){
    	posCorr.rotateZ(-0.002994);
    }else if(Para.MethodMode()==1){
    	posCorr.rotateZ(-deltaPhi);    
    }

    if(thetaModule>21) {
      posCorr.setZ(-posCorr.z());
    }
  } else {  //endcap

        EmcRecParameter& Para=EmcRecParameter::GetInstance();

    if(Para.MethodMode()==1){

      double IRShift = 0;
      HepPoint3D IR(0,0,IRShift);
      double theta12=-9999,theta03=-9999,theta23=-9999,theta14=-9999,delta=-9999;
      double phi=-9999,phi01=-9999,phi23=-9999,phi12=-9999,phi34=-9999,deltaphi=-9999; 
      double xIn,xOut,deltaTheta,yIn,yOut,deltaPhi;        
      double posphi=posCorr.phi();
      
      double parTheta[5],parPhi[5];     

      HepPoint3D point0 ,point1 ,point2 ,point3 ,point4 ;
      point0 = iGeoSvc->GetCrystalPoint(id,0);
      point1 = iGeoSvc->GetCrystalPoint(id,1); 
      point2 = iGeoSvc->GetCrystalPoint(id,2);
      point3 = iGeoSvc->GetCrystalPoint(id,3);
      point4 = iGeoSvc->GetCrystalPoint(id,4);

      HepPoint3D center,center01, center23, center12,center03,center34,center14;
      center01 = (point0+point1)/2;
      center23 = (point2+point3)/2;
      center12 = (point1+point2)/2;
      center34 = (point3+point4)/2;
      center03 = (point0+point3)/2;
      center14 = (point1+point4)/2;

      phi01 = center01.phi();
      phi23 = center23.phi();
      phi12 = center12.phi();
      phi34 = center34.phi();


      if(  (thetaModule==1&&((phiModule+3)%4 == 0||(phiModule+2)%4 == 0))
	   ||(thetaModule==3&&((phiModule+4)%5 == 0||(phiModule+3)%5 == 0||(phiModule+2)%5 == 0)))
	{
	  center12 = center23;
	  center03 = center14;
	  
	  center01 = center12; 
	  center23 = center34;

	  phi01 = phi12;
	  phi23 = phi34;
	}

      if(phiModule==0){
	posphi = posphi;
      }else if(  ((thetaModule==0||thetaModule==1)&&phiModule==63)
		 ||((thetaModule==2||thetaModule==3)&&phiModule==79)
		 ||((thetaModule==4||thetaModule==5)&&phiModule==95)
		 ){
	
	posphi = posphi+2*CLHEP::pi;
	
      }else {
	posphi = posphi < 0 ? posphi+2*CLHEP::pi : posphi;	
      }

      phi01 = phi01 <0 ? phi01+2*CLHEP::pi : phi01;
      phi23 = phi23 <0 ? phi23+2*CLHEP::pi : phi23;
      
      if(module==0){  //east endcap	

	IRShift = 10;
	center = center23;
	phi = phi23;
	
	for(int i=0;i<5;i++) {
	  parTheta[i] = Para.EastLinThetaPara(thetaModule,i);
	  parPhi[i] = Para.EastLinPhiPara(0,i);
	}

      }else if (module==2){  //west endcap          

	IRShift = -10;
	center = center01;   
	phi = phi01;

	for(int i=0;i<5;i++){
	  parTheta[i] = Para.WestLinThetaPara(thetaModule,i);
	  parPhi[i] = Para.WestLinPhiPara(0,i);
	}

      }

     theta12 = (center12 - IR).theta();
     theta03	= (center03 - IR).theta();
     delta = theta03 - theta12;
     xIn = (((posCorr-IR).theta() - theta12) - delta*0.5)/delta;
     xOut = parTheta[0]*atan(xIn*parTheta[1]-parTheta[4]) + parTheta[2]*xIn + parTheta[3] ;
     deltaTheta = xOut*delta;
      
     deltaphi = fabs(phi23 - phi01);
     yIn = ((posphi - phi) - deltaphi*0.5)/deltaphi;
     yOut = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4]) + parPhi[2]*yIn + parPhi[3] ;
     deltaPhi = yOut*deltaphi;
     
     HepPoint3D rotateVector(0,1,0);   //y axis
     rotateVector.rotateZ(center.phi());
     posCorr.setZ(posCorr.z()-IRShift);
     posCorr.rotate(-deltaTheta,rotateVector);
     posCorr.setZ(posCorr.z()+IRShift);
     posCorr.rotateZ(-deltaPhi);
	
    }
    }  

  //  aShower.setPosition(posCorr);
  if(Para.PosCorr()==1){  aShower.setPosition(posCorr);}

  //----------------------------------------------------------------------
  if(aShower.energy()<1e-5) return;

  double r,theta,phi;
  double dtheta,dphi,dx,dy,dz;

  r = posCorr.mag();
  theta = posCorr.theta();
  phi = posCorr.phi();
  //cout<<"theta: "<<theta<<" phi: "<<phi<<endl;

//  EmcRecParameter& Para=EmcRecParameter::GetInstance();   
  if(aShower.energy()>0) {
    dtheta = Para.SigTheta(0)/sqrt(aShower.energy())+Para.SigTheta(1);
    dphi = Para.SigPhi(0)/sqrt(aShower.energy())+Para.SigPhi(1);
  }
  else {
    dtheta = 0;
    dphi = 0;
  }
  //cout<<"dtheta: "<<dtheta<<" dphi: "<<dphi<<endl;

  dx = fabs(iGeoSvc->GetBarrelR()*sin(phi)*dphi);
  dy = fabs(iGeoSvc->GetBarrelR()*cos(phi)*dphi);
  if(theta>0)
    dz = fabs(iGeoSvc->GetBarrelR()*dtheta/(sin(theta)*sin(theta)));
  else
    dz = 0;
  //cout<<" dx: "<<dx<<" dy: "<<dy<<" dz: "<<dz<<endl;

  aShower.setDtheta(dtheta);
  aShower.setDphi(dphi);

  //----------------------------------------------------------------------
  // Error matrix
  HepSymMatrix matrix(3); //error matrix of r, theta, phi
  matrix[0][0]=0;
  matrix[1][1]=dtheta*dtheta;
  matrix[2][2]=dphi*dphi;
  matrix[0][1]=0;
  matrix[0][2]=0;
  matrix[1][2]=0;
  //cout<<"matrix: \n"<<matrix;

  HepMatrix matrix1(3,3),matrix2(3,3);
  matrix1[0][0]=sin(theta)*cos(phi);
  matrix1[0][1]=r*cos(theta)*cos(phi);
  matrix1[0][2]=-r*sin(theta)*sin(phi);
  matrix1[1][0]=sin(theta)*sin(phi);
  matrix1[1][1]=r*cos(theta)*sin(phi);
  matrix1[1][2]=r*sin(theta)*cos(phi);
  matrix1[2][0]=cos(theta);
  matrix1[2][1]=-r*sin(theta);
  matrix1[2][2]=0;
  //cout<<"matrix1: \n"<<matrix1;

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix2[i][j]=matrix1[j][i];
    }
  }
  //cout<<"matrix2: \n"<<matrix2;

  HepMatrix matrix4(3,3);
  matrix4=matrix1*matrix*matrix2;
  //cout<<"matrix4: \n"<<matrix4;

  HepSymMatrix matrix5(3); //error matrix of x, y, z
  for(int i=0;i<3;i++) {
    for(int j=0;j<=i;j++) {
      matrix5[i][j]=matrix4[i][j];
    }
  }
  //cout<<"matrix5: \n"<<matrix5;

  aShower.setErrorMatrix(matrix5);
}
