//
//    Logarithmic weighted position attribute reconstruction
//
//    Created by Zhe Wang 2004, 5, 16
//
#include "EmcRec/EmcRecShowerPosLoglin.h"

#include <iostream>
#include <fstream>


#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"

#include "EmcRec/EmcRecParameter.h"


void EmcRecShowerPosLoglin::Position(RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator cit;
  HepPoint3D pos(0,0,0);
  HepPoint3D possum(0,0,0);
  double w,w1,w2,wsum=0;
  double num,num0;

  // cout<<"EmcRecShowerPosLoglin::Position Begin"<<endl; 
   
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    //    cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  } 

  EmcRecParameter& Para=EmcRecParameter::GetInstance();   


  //  double offset=Para.LogPosOffset();
  double a0=4;
  double offset;
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
    offset=a0-1.594*exp(-2.543*e5x5);
    RecEmcFractionMap fracMap5x5=aShower.getFractionMap5x5();

    num0=0;
    num=0;
    for(cit=fracMap5x5.begin();cit!=fracMap5x5.end();cit++) {
      w1=offset+log((cit->second.getEnergy()/e5x5)*cit->second.getFraction());      
      w2=(cit->second.getEnergy()/e5x5)*cit->second.getFraction();
      
      num0++;
      
      if(w1>0){
	num++;
      }
      
    }

    // cout<<"num0=="<<num0<<endl;
    //    cout<<"num==="<<num<<endl;
    for(cit=fracMap5x5.begin();cit!=fracMap5x5.end();cit++) {
      w1=offset+log((cit->second.getEnergy()/e5x5)*cit->second.getFraction());      
      w2=(cit->second.getEnergy()/e5x5)*cit->second.getFraction();
      
      num0++;
     
      if(w1>0){
	if(num>1){
	  //	  cout<<"w1 be used"<<endl;
	  wsum+=w1;
	  pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
	  possum+=pos*w1;
	} else{
	  //	  cout<<"w2 be used"<<endl;
	  wsum+=w2;
	  pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());
	  possum+=pos*w2;
	}
      }           
    }

    
  }
  if(wsum<=0) {
    //    cout<<"[[[[[[[[[[[[[[["<<wsum<<endl;
  }
  pos=possum/wsum;

  //PosCorr=0 without position correction
  //PosCorr=1 with position correction
  if(Para.PosCorr()==0){ 	

    aShower.setPosition(pos);
  }
  
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
    
    double IRShift=0, parTheta[5],parPhi[5];
    if(theModule==21) {
      IRShift = 0;
    } else if(theModule==20) {
      IRShift = 2.5;
    } else {
      IRShift = 5.;
    }  

 
      EmcRecParameter& Para=EmcRecParameter::GetInstance();
      for(int i=0;i<5;i++){
	
	parTheta[i] = Para.BarrLoglinThetaPara(theModule,i);
	 
	parPhi[i] = Para.BarrLoglinPhiPara(0,i);

//	cout<<"Para.BarrLoglinThetaPara"<<i<<"===="<<Para.BarrLoglinThetaPara(theModule,i)<<endl;
//	cout<<"Para.BarrLoglinPhiPara"<<i<<"===="<<Para.BarrLoglinPhiPara(0,i)<<endl;

//	cout<<"theModule===="<<theModule<<endl;
//	cout<<"partheta"<<i<<"===="<<parTheta[i]<<endl;
//	cout<<"parphi"<<i<<"===="<<parPhi[i]<<endl;

	
      }
    
    HepPoint3D IR(0,0,IRShift);
    HepPoint3D center01, center23;  //center of point0,1 and point2,3 in crystal
    center01 = (iGeoSvc->GetCrystalPoint(id,0)+iGeoSvc->GetCrystalPoint(id,1))/2;
    center23 = (iGeoSvc->GetCrystalPoint(id,2)+iGeoSvc->GetCrystalPoint(id,3))/2;

    double theta01,theta23,length,d;
    theta01 = (center01-IR).theta();
    theta23 = (center23-IR).theta();
    length = (center01-IR).mag();
    d = length*tan(theta01-theta23);  //length in x direction
    
    double xIn,xOut,deltaTheta;
    xIn = length*tan(theta01-(posCorr-IR).theta())-d/2;
    xOut = xIn-( parTheta[0]*atan(xIn*parTheta[1]-parTheta[4])+ parTheta[2]*xIn + parTheta[3] );
    deltaTheta = atan((xOut+d/2)/length)-atan((xIn+d/2)/length);
    //cout<<"xIn: "<<xIn<<" xOut: "<<xOut<<" deltaTheta: "<<deltaTheta<<endl;
    //cout<<"dx: "<<parTheta[0]*atan(xIn*parTheta[1]-parTheta[4])
    //      + parTheta[2]*xIn + parTheta[3] <<endl;
    
    //obtained by photon, not used, just for comparison
    //    double parPhi[5] = { 51.1, -0.1499, 7.62, 0.1301, 0.005581 };
    double yIn,yOut,deltaPhi;
    //    yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.1537
    //      : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.1537;
    //    yOut = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4])+parPhi[2]*yIn+parPhi[3];
    
    yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
      : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.843;

    if(phiModule==0&&posCorr.phi()<0){
      yIn = posCorr.phi()*180./CLHEP::pi+360.-119*3-1.843;
    }else if (phiModule==119&&posCorr.phi()>0){
      yIn = posCorr.phi()*180./CLHEP::pi-1.843;
    }else {
      yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843 : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.843;
    }
    yOut = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4])+parPhi[2]*yIn+parPhi[3];
    deltaPhi = yOut*CLHEP::pi/180.;
   
    HepPoint3D rotateVector(0,1,0);   //y axis
    rotateVector.rotateZ(center01.phi());
    posCorr.setZ(posCorr.z()-IRShift);
    posCorr.rotate(-deltaTheta,rotateVector);
    posCorr.setZ(posCorr.z()+IRShift);
    //phi parameter is gotten by the average of e+ e- peak
   
      posCorr.rotateZ(-deltaPhi);
    
    if(thetaModule>21) {
      posCorr.setZ(-posCorr.z());
    }
  }  
  if(Para.PosCorr()==1){  
	
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

