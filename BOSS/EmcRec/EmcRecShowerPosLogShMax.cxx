//
//    Logarithmic weighted position attribute reconstruction
//
//    Created by Zhe Wang 2004, 5, 16
//
#include "EmcRec/EmcRecShowerPosLogShMax.h"

#include <iostream>
#include <fstream>

#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EmcRec/EmcRecParameter.h"


void EmcRecShowerPosLogShMax::Position(RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator cit;
  HepPoint3D pos(0,0,0);
  HepPoint3D possum(0,0,0);
  double w,wsum=0;

  //  cout<<"EmcRecShowerPosLog::Position Begin"<<endl; 

  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    //    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  } 

  EmcRecParameter& Para=EmcRecParameter::GetInstance();   

  double offset=Para.LogPosOffset(); 
  //int NX0=Para.DepthOfShowerMax(); //unit X_0=1.86
  //cout<<offset<<endl;

  double e5x5 = -99999;
  if(Para.PositionMode2()=="all") {
    double etot=aShower.getEAll();
    for(cit=aShower.Begin();cit!=aShower.End();cit++){
      w=offset+log((cit->second.getEnergy()/etot)*cit->second.getFraction());
      if(w>0){
	wsum+=w;
	pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
	possum+=pos*w;
      }
    }
  } else if(Para.PositionMode2()=="3x3") {
    double e3x3=aShower.e3x3();
    RecEmcFractionMap fracMap3x3=aShower.getFractionMap3x3();
    for(cit=fracMap3x3.begin();cit!=fracMap3x3.end();cit++) {
      w=offset+log((cit->second.getEnergy()/e3x3)*cit->second.getFraction());
      if(w>0){
	wsum+=w;
	pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
	possum+=pos*w;
      }
    }
  } else {
    e5x5=aShower.e5x5();
    RecEmcFractionMap fracMap5x5=aShower.getFractionMap5x5();
    for(cit=fracMap5x5.begin();cit!=fracMap5x5.end();cit++) {
      w=offset+log((cit->second.getEnergy()/e5x5)*cit->second.getFraction());
      if(w>0){
	wsum+=w;
	//pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());

	HepPoint3D Rc,R;
	R=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
	Rc=iGeoSvc->GetCCenter(cit->second.getCellId());
	int NX0=6;
	pos = R + (Rc-R)/(Rc-R).mag()*NX0*1.86; //X_CsI=1.86cm

	possum+=pos*w;
      }
    }
  }
  if(wsum<=0) {
    //    cout<<"[[[[[[[[[[[[[[["<<wsum<<endl;
  }
  pos=possum/wsum;


    int NX0=6;
    HepPoint3D posFront=pos-pos/pos.mag()*NX0*1.86; //X_CsI=1.86cm


  // aShower.setPositionNoCor(pos);
  //----------------------------------------------------------------------
  //position correction
  RecEmcID id = aShower.getShowerId();
  const unsigned int module = EmcID::barrel_ec(id);
  const unsigned int thetaModule = EmcID::theta_module(id);
  const unsigned int phiModule = EmcID::phi_module(id);
  //HepPoint3D posCorr(pos);
 
  HepPoint3D posCorr(posFront);

  if(module==1) {   //barrel


    double IRShift, parTheta[5],parPhi[5]; 
    
     
    for(int i=0;i<5;i++){
		
      if(e5x5>1.0)                    parTheta[i] = Para.BarrLogShMaxThetaPara(thetaModule,i);
      else if(e5x5<=1.0 && e5x5>0.5)  parTheta[i] = Para.BarrLogShMaxThetaPara(thetaModule+44,i);
      else if(e5x5<=0.5)              parTheta[i] = Para.BarrLogShMaxThetaPara(thetaModule+88,i);
      
      if(e5x5>1.0)                    parPhi[i] = Para.BarrLogShMaxPhiPara(thetaModule,i);
      else if(e5x5<=1.0 && e5x5>0.5)  parPhi[i] = Para.BarrLogShMaxPhiPara(thetaModule+44,i);
      else if(e5x5<=0.5)              parPhi[i] = Para.BarrLogShMaxPhiPara(thetaModule+88,i);
      
      
    }
    
 
    HepPoint3D center01, center23, center12,center03;  //center of point0,1 and point2,3 in crystal
    center01 = (iGeoSvc->GetCrystalPoint(id,0)+ iGeoSvc->GetCrystalPoint(id,1))/2;
    center23 = (iGeoSvc->GetCrystalPoint(id,2)+ iGeoSvc->GetCrystalPoint(id,3))/2;
    center03 = (iGeoSvc->GetCrystalPoint(id,0)+ iGeoSvc->GetCrystalPoint(id,3))/2.0;
    center12 = (iGeoSvc->GetCrystalPoint(id,1)+ iGeoSvc->GetCrystalPoint(id,2))/2.0; 

    double theta01,theta23,dtheta;

    theta01 = (center01).theta();
    theta23 = (center23).theta();

    dtheta = theta01-theta23;  //length in x direction
    double xIn,xOut,deltaTheta,deltaX;
    xIn = (((posCorr).theta()-theta23)-dtheta/2)/dtheta;
    //emcX-extX=deltaX ==> extX=emcX-deltaX,Xout=xin-deltaX
    deltaX = parTheta[0]*atan(xIn*parTheta[1]-parTheta[4]) + parTheta[2]*xIn + parTheta[3] ;
    deltaTheta = deltaX*dtheta;
 
    //obtained by photon, not used, just for comparison
    //    double parPhi[5] = { 51.1, -0.1499, 7.62, 0.1301, 0.005581 };
   
    double phi12=-9999,phi03=-9999,deltaphi=-9999;  
  
    phi03 = center03.phi();
    phi12 = center12.phi();


    double posPhi;
    posPhi=posCorr.phi();
   

    if( phiModule==0){
      posPhi = posPhi;
      phi03 = phi03;
      phi12 = phi12;

    }else if( phiModule==119){	
      posPhi = posPhi+2*CLHEP::pi;
      phi03 = phi03+2*CLHEP::pi;
      phi12 = phi12+2*CLHEP::pi;

    }else {
      posPhi = posPhi <0 ? posPhi+2*CLHEP::pi : posPhi;
      phi03 = phi03 <0 ? phi03+2*CLHEP::pi : phi03;
      phi12 = phi12 <0 ? phi12+2*CLHEP::pi : phi12;
    }

    deltaphi = phi12 - phi03;
    

    double yIn,deltaY,deltaPhi;
    yIn = ((posPhi - phi03) - deltaphi*0.5)/deltaphi; 
    //deltaY=yIn-Ymc => Ymc=yIn-deltaY
    deltaY = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4])+parPhi[2]*yIn+parPhi[3];
    deltaPhi = deltaY*deltaphi;

    // deltaPhi = deltaY*CLHEP::pi/180.;
    /*   
    HepPoint3D rotateVector(0,1,0);   //y axis
    rotateVector.rotateZ(center01.phi());
    posCorr.setZ(posCorr.z()-IRShift);
    posCorr.rotate(-deltaTheta,rotateVector);
    posCorr.setZ(posCorr.z()+IRShift);
    */
    /*
    HepPoint3D possh;
    possh=posCorr;//-IRShift;
    //possh.setTheta(possh.theta()-deltaTheta);
    //  posCorr=possh+IRShift;
    posCorr.setTheta(possh.theta()-deltaTheta);
    */
    
    HepPoint3D rotateVector(0,1,0);   //y axis
    //rotateVector.rotateZ(center01.phi());
    rotateVector.rotateZ(center23.phi());

    posCorr.rotate(-deltaTheta,rotateVector);
    posCorr.rotateZ(-deltaPhi);
  
    /*
    double lengthemc;
    lengthemc = abs(posCorr.z()/cos(posCorr.theta()));
    // for Data
    double posDataCorPar;
    if(Para.DataMode()==1) {
      posDataCorPar=Para.BarrPosDataCor(thetaModule,phiModule);	
      posCorr.setTheta(posCorr.theta()-posDataCorPar/lengthemc);
    }
    */
    /*  //for MC
    double posMCCorPar;
    if(Para.DataMode()==0) {  
      posMCCorPar=Para.BarrPosMCCor(thetaModule,phiModule);	
      posCorr.setTheta(posCorr.theta()-posMCCorPar/lengthemc);
    }
    */

  }   else {  //endcap position corretion
    // if(Para.MethodMode()==1){

      double IRShift=0, parTheta[5],parPhi[5];  
      
      if(module==0){  //east endcap
	
	IRShift = 10;
	
	for(int i=0;i<5;i++) {
	  
	  
	  if(e5x5>1.0)                   parTheta[i] = Para.EastLogShMaxThetaPara(thetaModule,i);
	  else if(e5x5<=1.0 && e5x5>0.5) parTheta[i] = Para.EastLogShMaxThetaPara(thetaModule+6,i);
	  else if (e5x5<=0.5)            parTheta[i] = Para.EastLogShMaxThetaPara(thetaModule+6,i);
	  
	  if(e5x5>1.0)                    parPhi[i] = Para.EastLogShMaxPhiPara(0,i);
	  else if(e5x5<=1.0 && e5x5>0.5)  parPhi[i] = Para.EastLogShMaxPhiPara(1,i);
	  else if (e5x5<=0.5)             parPhi[i] = Para.EastLogShMaxPhiPara(2,i);
	}
	
      }else if (module==2){  //west endcap          
	IRShift = -10;
	for(int i=0;i<5;i++){
	  
	  if(e5x5>1.0)                   parTheta[i] = Para.WestLogShMaxThetaPara(thetaModule,i);
	  else if(e5x5<=1.0 && e5x5>0.5) parTheta[i] = Para.WestLogShMaxThetaPara(thetaModule+6,i);
	  else if (e5x5<=0.5)            parTheta[i] = Para.WestLogShMaxThetaPara(thetaModule+6,i);
	  
	  if(e5x5>1.0)                    parPhi[i] = Para.WestLogShMaxPhiPara(0,i);
	  else if(e5x5<=1.0 && e5x5>0.5)  parPhi[i] = Para.WestLogShMaxPhiPara(1,i);
	  else if (e5x5<=0.5)             parPhi[i] = Para.WestLogShMaxPhiPara(2,i);
	  
	}
       
      }
     
      HepPoint3D IR(0,0,IRShift);
     
      double theta12=-9999,theta03=-9999,theta23=-9999,theta14=-9999,delta=-9999;
      double phi01=-9999,phi23=-9999,phi12=-9999,phi34=-9999,deltaphi=-9999; 
      double xIn,deltaX,deltaTheta,yIn,deltaY,deltaPhi;  
      
      double posphi=posCorr.phi();
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
      
      HepPoint3D center,center01, center23, center12,center03,center34,center14;
      
      
      if(  (thetaModule==1&&((phiModule+3)%4 == 0||(phiModule+2)%4 == 0))
	   ||(thetaModule==3&&((phiModule+4)%5 == 0||(phiModule+3)%5 == 0||(phiModule+2)%5 == 0))) //pentagon
	{
	  
	  center23=(iGeoSvc->GetCrystalPoint(id,2)+ iGeoSvc->GetCrystalPoint(id,3))/2.0;
	  center14=(iGeoSvc->GetCrystalPoint(id,1)+ iGeoSvc->GetCrystalPoint(id,4))/2.0;
	  center=center23;
	  theta23 = (center23 - IR).theta();
	  theta14 = (center14 - IR).theta();
	  delta = theta14 - theta23;
	  xIn = (((posCorr-IR).theta() - theta23) - delta*0.5)/delta;
	  
	  
	  center12=(iGeoSvc->GetCrystalPoint(id,1)+ iGeoSvc->GetCrystalPoint(id,2))/2.0;
	  center34=(iGeoSvc->GetCrystalPoint(id,3)+ iGeoSvc->GetCrystalPoint(id,4))/2.0;
	  phi12 = center12.phi();
	  phi34 = center34.phi();

	  if(phiModule==0){
	    phi12 = phi12;
	    phi34 = phi34;
	  }else if(  ((thetaModule==0||thetaModule==1)&&phiModule==63)
		     ||((thetaModule==2||thetaModule==3)&&phiModule==79)
		     ||((thetaModule==4||thetaModule==5)&&phiModule==95)
		     ){	
	    phi12 = phi12+2*CLHEP::pi;
	    phi34 = phi34+2*CLHEP::pi;	
	  }else { 
	    phi12 = phi12 <0 ? phi12+2*CLHEP::pi : phi12;
	    phi34 = phi34 <0 ? phi34+2*CLHEP::pi : phi34;
	  }

	  deltaphi = phi34 - phi12;
	  yIn = ((posphi - phi12) - deltaphi*0.5)/deltaphi;    
	  
	} else {
	
	center12=(iGeoSvc->GetCrystalPoint(id,1)+ iGeoSvc->GetCrystalPoint(id,2))/2.0;
	center03=(iGeoSvc->GetCrystalPoint(id,0)+ iGeoSvc->GetCrystalPoint(id,3))/2.0;
	center=center12;
	
	theta12 = (center12 - IR).theta();
	theta03 = (center03 - IR).theta();
	delta = theta03 - theta12;
	xIn = (((posCorr-IR).theta() - theta12) - delta*0.5)/delta;
	
	
	center01=(iGeoSvc->GetCrystalPoint(id,0)+ iGeoSvc->GetCrystalPoint(id,1))/2.0;
	center23=(iGeoSvc->GetCrystalPoint(id,2)+ iGeoSvc->GetCrystalPoint(id,3))/2.0;
	phi01 = center01.phi();
	phi23 = center23.phi();


	if(phiModule==0){
	  phi01 = phi01;
	  phi23 = phi23;
	}else if(  ((thetaModule==0||thetaModule==1)&&phiModule==63)
		   ||((thetaModule==2||thetaModule==3)&&phiModule==79)
		   ||((thetaModule==4||thetaModule==5)&&phiModule==95)
		   ){	
	  phi01 = phi01+2*CLHEP::pi;
	  phi23 = phi23+2*CLHEP::pi;	
	}else { 
	  phi01 = phi01 <0 ? phi01+2*CLHEP::pi : phi01;
	  phi23 = phi23 <0 ? phi23+2*CLHEP::pi : phi23;
	}

	deltaphi = phi23 - phi01;
	yIn = ((posphi - phi01) - deltaphi*0.5)/deltaphi;    
	
      }
      //deltaX=xIn-Xext => Xext=xIn-deltaX
      deltaX = parTheta[0]*atan(xIn*parTheta[1]-parTheta[4]) + parTheta[2]*xIn + parTheta[3] ;
      deltaTheta = deltaX*delta;   
      //deltaY=yIn-Ymc => Ymc=yIn-deltaY
      deltaY = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4]) + parPhi[2]*yIn + parPhi[3] ;
      deltaPhi = deltaY*deltaphi;
      
      HepPoint3D rotateVector(0,1,0);   //y axis
      rotateVector.rotateZ(center.phi());
      posCorr.setZ(posCorr.z()-IRShift);
      posCorr.rotate(-deltaTheta,rotateVector);
      posCorr.setZ(posCorr.z()+IRShift);
      posCorr.rotateZ(-deltaPhi);
      /*     
      double lengthemc;
      lengthemc = abs(posCorr.z()/cos(posCorr.theta()));
      double posDataCorPar;
      if(Para.DataMode()==1) {
	if(module==0) posDataCorPar=Para.EastPosDataCor(thetaModule,phiModule);
	if(module==2) posDataCorPar=Para.WestPosDataCor(thetaModule,phiModule);	
	posCorr.setTheta(posCorr.theta()-posDataCorPar/lengthemc);
      }
      */ 
      //}

  } 


  //PosCorr=0 without position correction
  //PosCorr=1 with position correction
  if(Para.PosCorr()==0){
    //int NX0=6;
    //HepPoint3D posFront=pos-pos/pos.mag()*NX0*1.86; //X_CsI=1.86cm
    aShower.setPosition(posFront);
    // aShower.setPosition(pos);
    
  }

  if(Para.PosCorr()==1){

    //pos from the shmax to the front endface of crystal 
    // int NX0=6;
    //HepPoint3D posCorrFront=posCorr-posCorr/posCorr.mag()*NX0*1.86; //X_CsI=1.86cm
    //aShower.setPosition(posCorrFront);
    ////////////

     aShower.setPosition(posCorr);
   }
  ////////////////////////////
  if(aShower.energy()<1e-5) return;

  double r,theta,phi;
  double dtheta,dphi,dx,dy,dz;

  r = posCorr.mag();
  theta = posCorr.theta();
  phi = posCorr.phi();
  //cout<<"energy: "<<aShower.energy()<<" theta: "<<theta<<" phi: "<<phi<<endl;
  
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
  //cout<<"matrix:\n"<<matrix<<endl;

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
  //cout<<"matrix1:\n"<<matrix1<<endl;

  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      matrix2[i][j]=matrix1[j][i];
    }
  }
  //cout<<"matrix2:\n"<<matrix2<<endl;
  
  HepMatrix matrix4(3,3);
  matrix4=matrix1*matrix*matrix2;
  //cout<<"matrix4:\n"<<matrix4<<endl;
  
  HepSymMatrix matrix5(3); //error matrix of x, y, z
  for(int i=0;i<3;i++) {
    for(int j=0;j<=i;j++) {
      matrix5[i][j]=matrix4[i][j];
    }
  }
  //cout<<"matrix5:\n"<<matrix5<<endl;

  aShower.setErrorMatrix(matrix5);

  if(matrix5[0][0]>0) dx=sqrt(matrix5[0][0]);
  if(matrix5[1][1]>0) dy=sqrt(matrix5[1][1]);
  if(matrix5[2][2]>0) dz=sqrt(matrix5[2][2]);
}

