//lin       emcShowerPosLinShMax.cxx
//    Lin weighted position attribute reconstruction
//
//    Created by Zhe Wang 2004, 5, 16
//
#include "EmcRec/EmcRecShowerPosLinShMax.h"

#include <iostream>
#include <fstream>
#include "EmcRecGeoSvc/EmcRecGeoSvc.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "EmcRec/EmcRecParameter.h"
#include "CLHEP/Vector/LorentzVector.h"

using namespace std;

void EmcRecShowerPosLinShMax::Position(RecEmcShower& aShower)
{
  RecEmcFractionMap::const_iterator cit;
  HepPoint3D pos(0,0,0);
  HepPoint3D posMax(0,0,0);
  HepPoint3D possum(0,0,0);
  double w,wsum=0;
  double etot;
 
//cout<<"EmcRecShowerPosLinShMax::Position Begin"<<endl;
 
  IEmcRecGeoSvc* iGeoSvc;
  ISvcLocator* svcLocator = Gaudi::svcLocator();
  StatusCode sc = svcLocator->service("EmcRecGeoSvc",iGeoSvc);
  if(sc!=StatusCode::SUCCESS) {
    //cout<<"Error: Can't get EmcRecGeoSvc"<<endl;
  } 

  EmcRecParameter& Para=EmcRecParameter::GetInstance();   

  for(cit=aShower.Begin();cit!=aShower.End();cit++){
    etot+=(cit->second.getEnergy()*cit->second.getFraction());
    //cout<<etot<<endl;
  }
  wsum=0;

    double etota=aShower.getEAll();
    double ltot=(log(etota/0.0145)+0.5)*1.86;
    double e55=aShower.e5x5();
    double len55=(log(etota/0.0145)+0.5)*1.86;
    Identifier   seedID = aShower.getShowerId();
    HepPoint3D seedPoint(iGeoSvc->GetCFrontCenter(seedID));
    //Rs is the distance from orgin to showerCentroid, unit cm
//    HepDouble Rs=seedPoint.mag();   
  for(cit=aShower.Begin();cit!=aShower.End();cit++){
    w=(cit->second.getEnergy()*cit->second.getFraction());

          HepPoint3D hitPoint(iGeoSvc->GetCFrontCenter(cit->second.getCellId()));
//          HepDouble Ang=seedPoint.angle(hitPoint);
          // theDistance is the distance from orgin to showerMax
          double theDistance=0;
            if(cit->second.getCellId()==seedID) {
//              theDistance=(Rs+ltot);
		theDistance=(seedPoint.mag()+ltot);
            } else {
              theDistance=(seedPoint.mag()+ltot)/cos(seedPoint.angle(hitPoint));
//		theDistance=(Rs+ltot)/cos(Ang);
            }  

  //cout<<w<<endl;
    wsum+=w;
    pos=iGeoSvc->GetCFrontCenter(cit->second.getCellId());

    posMax = (theDistance/pos.mag())*pos;

    possum+=posMax*w;
  }

  if(wsum<=0) {
    cout<<"[[[[[[[[[[[[[[["<<wsum;
  }
  pos=possum/wsum;
//  pos=((pos.mag()-len55)/pos.mag())*pos;
//  aShower.setPosition(pos);
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

    double IRShift, parTheta[5],parPhi[5];
    if(theModule==21) {
      IRShift = 0;
    } else if(theModule==20) {
      IRShift = 2.5;
    } else {
      IRShift = 5.;
    }

    EmcRecParameter& Para=EmcRecParameter::GetInstance();

    for(int i=0;i<5;i++){

      if(e55>1.0)                     parTheta[i] = Para.BarrShLinThetaPara(theModule,i);
      else if(e55<=1.0 && e55>0.5)    parTheta[i] = Para.BarrShLinThetaPara(theModule+22,i);
      else if(e55<=0.5)               parTheta[i] = Para.BarrShLinThetaPara(theModule+44,i);


      if(e55>1.0)                      parPhi[i] = Para.BarrShLinPhiPara(0,i);
      else if(e55<=1.0 && e55>0.5)     parPhi[i] = Para.BarrShLinPhiPara(1,i);
      else if(e55<=0.5)                parPhi[i] = Para.BarrShLinPhiPara(2,i);
      
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
    xOut = xIn-( parTheta[0]*atan(xIn*parTheta[1]-parTheta[4])
        + parTheta[2]*xIn + parTheta[3] );
    deltaTheta = atan((xOut+d/2)/length)-atan((xIn+d/2)/length);
    //cout<<"xIn: "<<xIn<<" xOut: "<<xOut<<" deltaTheta: "<<deltaTheta<<endl;
    //cout<<"dx: "<<parTheta[0]*atan(xIn*parTheta[1]-parTheta[4])
    //      + parTheta[2]*xIn + parTheta[3] <<endl;
/////////////////////////////////////////////////////////////////////////////////////////////////
    double yIn,yOut,deltaPhi;

    if(phiModule==0&&posCorr.phi()<0){
    
      yIn = posCorr.phi()*180./CLHEP::pi+360.-119*3-1.843;
  
    }else if (phiModule==119&&posCorr.phi()>0){
    
      yIn = posCorr.phi()*180./CLHEP::pi-1.843;

    }else {

      yIn = posCorr.phi() < 0 ? posCorr.phi()*180./CLHEP::pi+360.-phiModule*3.-1.843
      : posCorr.phi()*180./CLHEP::pi-phiModule*3.-1.843;

    }
    yOut = parPhi[0]*atan(yIn*parPhi[1]-parPhi[4])+parPhi[2]*yIn+parPhi[3];

//    yOut = parPhi[0]+parPhi[1]*yIn+parPhi[2]*yIn*yIn+parPhi[3]*yIn*yIn*yIn+parPhi[4]*yIn*yIn*yIn*yIn+parPhi[5]*yIn*yIn*yIn*yIn*yIn;

    deltaPhi = yOut*CLHEP::pi/180.;

    HepPoint3D rotateVector(0,1,0);   //y axis
    rotateVector.rotateZ(center01.phi());
    posCorr.setZ(posCorr.z()-IRShift);
    posCorr.rotate(-deltaTheta,rotateVector);
    posCorr.setZ(posCorr.z()+IRShift);
    //phi parameter is gotten by the average of e+ e- peak
//    posCorr.rotateZ(-0.002994);
    posCorr.rotateZ(-deltaPhi);

    if(thetaModule>21) {
      posCorr.setZ(-posCorr.z());
    }
  }
  posCorr=((posCorr.mag()-len55)/posCorr.mag())*posCorr;
  //  aShower.setPosition(posCorr);
  if(Para.PosCorr()==1){  aShower.setPosition(posCorr);}
  
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

