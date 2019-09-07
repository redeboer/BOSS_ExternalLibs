//
//   EmcRecBarrelGeo
//
//   Dec 18, 2003, Created by Wang.Zhe
//
//   unit: mm, radian
//
//#include "BesGeoEMC/DB2BesGeoEMC.h"
#include "EmcRecGeoSvc/EmcRecBarrelGeo.h"

EmcRecBarrelGeo::EmcRecBarrelGeo()
{
   ParameterInitialize();
   CalculateStandardCrystal();
   Transform2Column1();
   FillCCenterVector();
}

EmcRecBarrelGeo::~EmcRecBarrelGeo()
{
   fStandard.clear();
   fCCenter.clear();
   fCFrontCenter.clear();
}
  
void EmcRecBarrelGeo::ParameterInitialize()
{
   /*
   DB2BesGeoEMC obj1;
   vector<BesGeoEMC> VecA;
   
   obj1.Get_BesGeoEMC(VecA);
   fBarrelR=VecA[0].Get_BarrelR();
   fBarrelOffset1=VecA[0].Get_BarrelOffset1();
   fBarrelOffset2=VecA[0].Get_BarrelOffset2();
   fBarrelh1=VecA[0].Get_Barrelh1();
   fBarrelh2=VecA[0].Get_Barrelh2();
   fBarrelh3=VecA[0].Get_Barrelh3();
   fBarrelL=VecA[0].Get_BarrelL();
   fBarrelL2=VecA[0].Get_BarrelL2();
   fBarrelNPhiMax=(int)VecA[0].Get_BarrelNPhiMax();
   fBarrelNThetaMax=(int)VecA[0].Get_BarrelNThetaMax();
   */
    
   fBarrelR=942;
   fBarrelOffset1=25;
   fBarrelOffset2=50;
   fBarrelh1=51;
   fBarrelh2=52;
   fBarrelh3=52.466;
   fBarrelL=280;
   fBarrelL2=240; 
   fBarrelNPhiMax=120;
   fBarrelNThetaMax=22;
   
   fBarrelAlpha=twopi/fBarrelNPhiMax;

   fStandard.clear();
   fCCenter.clear();
   fCFrontCenter.clear();
}

void EmcRecBarrelGeo::CalculateStandardCrystal()
{
   double R1,R2,a;
   EmcRecCrystal pre,now;
   double dx,dy,dz;
   HepPoint3D t1,t2;
   double L,h;
   
   HepPoint3D O1(0,0,0);
   HepPoint3D O2(0,0,fBarrelOffset1);
   HepPoint3D O3(0,0,fBarrelOffset2);

   R1=2*fBarrelR*sin(fBarrelAlpha/2)/sin(fBarrelAlpha);
   R2=2*fBarrelR*sin(fBarrelAlpha/2)*(tan(fBarrelAlpha)+1/tan(fBarrelAlpha));
   a=2*fBarrelR*sin(fBarrelAlpha/2)/cos(fBarrelAlpha);
   
   HepPoint3D M4(0,R1,0);
   HepPoint3D A1(a,R1,0);
   
//   cout<<O1<<", "<<O2<<", "<<O3<<endl;
//   cout<<M4<<", "<<A1<<endl;
//   cout<<R1<<", "<<R2<<endl;
   
   // No. 1  theta: 21; phi: 30
   EmcRecCrystal Crystal1;

   Crystal1.Set(0,M4);
   Crystal1.Set(1,A1);
   Crystal1.Set(2,A1);   Crystal1.SetZ(2,A1.z()+fBarrelh1);
   Crystal1.Set(3,M4);   Crystal1.SetZ(3,M4.z()+fBarrelh1);
   
   Crystal1.Set(4,M4);   Crystal1.SetY(4,M4.y()+fBarrelL);
   dx=a*(R1+fBarrelL)/R1;
   Crystal1.Set(5,Crystal1.Get(4));   Crystal1.SetX(5,dx);
   dz=fBarrelOffset1+(fBarrelh1-fBarrelOffset1)*(R1+fBarrelL)/R1;
   Crystal1.Set(6,Crystal1.Get(5));   Crystal1.SetZ(6,Crystal1.Get(5).z()+dz);
   Crystal1.Set(7,Crystal1.Get(4));   Crystal1.SetZ(7,Crystal1.Get(4).z()+dz);
   
   fStandard.push_back(Crystal1);
   pre=fStandard[0];
   
   // No. 2  theta: 20; phi: 30
   EmcRecCrystal Crystal2; 
   double sin_gamma,cos_gamma,tan_gamma;
   
   tan_gamma=R1/(fBarrelh1-fBarrelOffset1);
   sin_gamma=tan_gamma/(sqrt(1+tan_gamma*tan_gamma));
   cos_gamma=1/(sqrt(1+tan_gamma*tan_gamma));
   
   double aa,bb,Rp;
   aa=fBarrelh1/sin_gamma;
   bb=fBarrelh1/tan_gamma; 
   
   Crystal2.Set(0,pre.Get(3));   Crystal2.SetZ(0,pre.Get(3).z()+bb*cos_gamma);
                                 Crystal2.SetY(0,pre.Get(3).y()+bb*sin_gamma);
   Rp=R1/sin_gamma;
   dx=a*(Rp+bb)/Rp;
   Crystal2.Set(1,Crystal2.Get(0));   Crystal2.SetX(1,dx);   
   Crystal2.Set(3,pre.Get(3));        Crystal2.SetZ(3,pre.Get(3).z()+aa);
   Crystal2.Set(2,Crystal2.Get(3));   Crystal2.SetX(2,a);
   
   dz=(fBarrelL+bb)*cos_gamma;
   dy=(fBarrelL+bb)*sin_gamma;
   Crystal2.Set(4,pre.Get(3));   Crystal2.SetZ(4,pre.Get(3).z()+dz);
                                 Crystal2.SetY(4,pre.Get(3).y()+dy); 
   dx=a*(Rp+bb+fBarrelL)/Rp; 
   Crystal2.Set(5,Crystal2.Get(4));     Crystal2.SetX(5,dx);
   double gam2,gam3,Lp,Rpp;
   t1=pre.Get(3)-Crystal2.Get(3);
   t2=O3-Crystal2.Get(3);
   gam2=acos(t1*t2/(t1.mag()*t2.mag()));
   gam3=acos(cos_gamma)-gam2;
   Lp=fBarrelL/cos(gam3);
   dz=Lp*cos(gam2);
   dy=Lp*sin(gam2);
   Crystal2.Set(7,Crystal2.Get(3));     Crystal2.SetZ(7,Crystal2.Get(3).z()+dz);
                                        Crystal2.SetY(7,Crystal2.Get(3).y()+dy);
   Rpp=R1/sin(gam2);
   dx=a*(Rpp+Lp)/Rpp;
   Crystal2.Set(6,Crystal2.Get(7));     Crystal2.SetX(6,dx);
   
   fStandard.push_back(Crystal2);

   // No. 3 -- No. 22  theta: 19-0; phi: 30
   for(int i=3; i<=fBarrelNThetaMax; i++) {
      pre=fStandard[i-2];
      
      if(i<fBarrelNThetaMax) {
	 L=fBarrelL;
	 h=fBarrelh2;
      }
      else {
	 L=fBarrelL2;
	 h=fBarrelh3;
      }
      
      tan_gamma=R1/(pre.Get(3).z()-fBarrelOffset2);
      sin_gamma=tan_gamma/(sqrt(1+tan_gamma*tan_gamma));
      cos_gamma=1/(sqrt(1+tan_gamma*tan_gamma));

      aa=h/sin_gamma;
      bb=h/tan_gamma;
      
      now.Set(3,pre.Get(3));   now.SetZ(3,pre.Get(3).z()+aa);
      now.Set(0,pre.Get(3));   now.SetZ(0,pre.Get(3).z()+bb*cos_gamma);
                               now.SetY(0,pre.Get(3).y()+bb*sin_gamma);
      
      now.Set(4,pre.Get(3));   now.SetZ(4,pre.Get(3).z()+(L+bb)*cos_gamma);
                               now.SetY(4,pre.Get(3).y()+(L+bb)*sin_gamma);
      
      gam2=atan(R1/(now.Get(3).z()-fBarrelOffset2));
      gam3=acos(cos_gamma)-gam2;
      
      Lp=L/cos(gam3);
      dz=Lp*cos(gam2);
      dy=Lp*sin(gam2);
   
      now.Set(7,now.Get(3));   now.SetZ(7,now.Get(3).z()+dz);
                               now.SetY(7,now.Get(3).y()+dy);

      Rp=R1/sin_gamma;
      dx=a*(Rp+bb)/Rp;
      now.Set(1,now.Get(0));   now.SetX(1,dx);
      dx=a*(Rp+bb+L)/Rp;
      now.Set(5,now.Get(4));   now.SetX(5,dx);
      
      now.Set(2,now.Get(3));   now.SetX(2,a);
      
      Rpp=R1/sin(gam2);
      dx=a*(Rpp+Lp)/Rpp;
      now.Set(6,now.Get(7));   now.SetX(6,dx);
      
      fStandard.push_back(now);
   }
   
//   for(int i=1; i<=fBarrelNThetaMax; i++) {
//      cout<<endl;
//      cout<<"No. "<<i<<endl;
//      now=fStandard[i-1];
//      now.BarrelCheckout();
//      now.Dump();
//   }  
}

void EmcRecBarrelGeo::Transform2Column1()  // to theta: 21-0; phi: 0
{
   HepPoint3D O1(fBarrelR*sin(fBarrelAlpha/2),
		 fBarrelR*cos(fBarrelAlpha/2)-2*fBarrelR*sin(fBarrelAlpha/2)/tan(fBarrelAlpha),
		 0);
   
   HepPoint3D R=fStandard[0].Get(5);
   double phi=(R.rotateZ(fBarrelAlpha)+O1).phi();

   for(int i=1; i<=fBarrelNThetaMax; i++) {
      for(int m=0;m<8;m++)
	{
	   fStandard[i-1].Type(EmcRecCrystal::SixPlane());
	   fStandard[i-1].Set(m,fStandard[i-1].Get(m).rotateZ(fBarrelAlpha));
	   fStandard[i-1].Set(m,fStandard[i-1].Get(m)+O1);
	   fStandard[i-1].Set(m,fStandard[i-1].Get(m).rotateZ(-phi));
	}

//      cout<<endl;
//      cout<<"No. "<<i<<endl;
//      fStandard[i-1].BarrelCheckout();
//      fStandard[i-1].Dump();
   }
}

void EmcRecBarrelGeo::FillCCenterVector()
{
   int phi,theta;
   EmcRecCrystal aCry;
   HepPoint3D aCenter;
   HepPoint3D aFrontCenter;
   
   for(theta=0;theta<2*fBarrelNThetaMax;++theta) {
      for(phi=0;phi<fBarrelNPhiMax;++phi) {
	 aCry=GetCrystal(EmcID::crystal_id(EmcID::getBARREL(),theta,phi));
	 aCenter=aCry.Center();
   aFrontCenter=aCry.FrontCenter();
	 fCCenter.push_back(aCenter);
   fCFrontCenter.push_back(aFrontCenter);
//	 aCry.Dump();
//	 cout<<"("<<theta<<","<<phi<<")  ";
//	 cout<<aCenter<<endl;
      }
   }
}


// Access
EmcRecCrystal EmcRecBarrelGeo::GetCrystal(const Identifier& id) const
{
   EmcRecCrystal cry;
   unsigned int theta=EmcID::theta_module(id);
   unsigned int phi=EmcID::phi_module(id);
   
   double dphi=phi*fBarrelAlpha;
   
   if((int)theta>=fBarrelNThetaMax) {
      cry=fStandard[theta-fBarrelNThetaMax];
      for(int m=0;m<8;++m)
	{
	   cry.SetZ(m,-cry.Get(m).z());
	}
   }
   else {
      cry=fStandard[fBarrelNThetaMax-theta-1];
   }
   
   for(int m=0;m<8;++m)
     {
	cry.Set(m,cry.Get(m).rotateZ(dphi));
     }
   
   return cry;
}

HepPoint3D EmcRecBarrelGeo::GetCCenter(const Identifier& id) const
{
   unsigned int theta=EmcID::theta_module(id);
   unsigned int phi=EmcID::phi_module(id);
   
   return fCCenter[theta*fBarrelNPhiMax+phi];
}

HepPoint3D EmcRecBarrelGeo::GetCFrontCenter(const Identifier& id) const
{
  unsigned int theta=EmcID::theta_module(id);
  unsigned int phi=EmcID::phi_module(id);

  return fCFrontCenter[theta*fBarrelNPhiMax+phi];
}
