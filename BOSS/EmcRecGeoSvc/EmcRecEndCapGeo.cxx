//
//   EmcRecEndCapGeo
//
//   Dec 18, 2003, Created by Wang.Zhe
//
//   unit: mm, radian
//
#include "EmcRecGeoSvc/EmcRecEndCapGeo.h"

EmcRecEndCapGeo::EmcRecEndCapGeo()
{
   ParameterInitialize();
   CalculateStandardSector1();
   CalculateStandardSector2();
   FillCCenterVector(); 
}

EmcRecEndCapGeo::~EmcRecEndCapGeo()
{
}
  
void EmcRecEndCapGeo::ParameterInitialize()
{
   // EndCap is too complex. More careful and detailed initialization
   // will be done to make the later calculation easier.
   int i;
   
   // Read constant for Database
   // general
   flength=280;
   fzshift=100;
   fz=1285;
   fr[0]=880;
   fr[1]=813.60278843;
   fr[2]=748.39248406;
   fr[3]=684.54540341;
   fr[4]=621.94216056;
   fr[5]=560.46548641;
   fr[6]=500;
   
   // for Ring5 and 4
   fphi5[0]=0;
   fphi5[1]=4.4;
   fphi5[2]=8.02;
   fphi5[3]=11.64;
   fphi5[4]=15.26;
   fphi5[5]=18.88;
   fphi5[6]=22.50;
   for(i=0;i<7;++i) {
      fphi5[i]=fphi5[i]*3.14159265358979/180;
   }
   
   // for Ring3 and 2
   fphi3[0]=0;
   fphi3[1]=5.124;
   fphi3[2]=9.468;
   fphi3[3]=13.812;
   fphi3[4]=18.156;
   fphi3[5]=22.50;
   for(i=0;i<6;++i) {
      fphi3[i]=fphi3[i]*3.14159265358979/180;
   }

   /// for Ring1 and 0
   fphi1[0]=0;
   fphi1[1]=6.21;
   fphi1[2]=11.64;
   fphi1[3]=17.07;
   fphi1[4]=22.50;
   for(i=0;i<5;++i) {
      fphi1[i]=fphi1[i]*3.14159265358979/180;
   }
}

void EmcRecEndCapGeo::CalculateStandardSector1()
{
   int i,j;
   EmcRecGeoPlane pl1,pl2,pl3;
   HepPoint3D po0,po1,po2,po3,po4,po5,po6,po7,po8,po9;
   
   double tantheta1,costheta1,sintheta1;

   HepPoint3D O,n1;               // very important
   O.setX(0); O.setY(0); O.setZ(0); 
   n1.setX(0); n1.setY(0); n1.setZ(fzshift);
   
   // ******** Ring 5 ********
   HepPoint3D t,n2,n;
   HepPoint3D w,m;
   double dphi1=fphi5[1]-fphi5[0];
   double dphi2=fphi5[2]-fphi5[1];   
   double dphi;
   double l,ll,lp;
   
   t.setX(fr[0]);
   t.setY(0);
   t.setZ(fzshift+fz);

   tantheta1=fr[0]/fz;
   costheta1=1/sqrt(1+tantheta1*tantheta1);
   sintheta1=costheta1*tantheta1;
   
   for(i=0;i<6;++i){
      fRing5[i].Type(EmcRecCrystal::SixPlane());
   }
   
   // No. 0,1'
   for(i=0;i<2;++i) {
      if(i==0) { 
	 dphi=dphi1; 
      } else {
	 dphi=dphi2;
      }
      // first, point 3
      po3=t;
      fRing5[i].Set(3,po3);
      // then, point 0
      po0=t;		    
      fRing5[i].Set(0,po0.rotateZ(dphi));
      // point 7 and point 4
      l=fRing5[i].Get(3).distance(fRing5[i].Get(0))/2;
      ll=sqrt(fz*fz+fr[0]*fr[0]);
      lp=ll*flength/sqrt(ll*ll-l*l);
      po7=t;
      po7.setX(po7.x()+lp*sintheta1);
      po7.setZ(po7.z()+lp*costheta1);
      fRing5[i].Set(7,po7);
      po4=po7;
      fRing5[i].Set(4,po4.rotateZ(dphi));
      // point 1,2
      pl1.Build(0,1,0,0);
      n2=(fRing5[i].Get(0)+fRing5[i].Get(3))/2;
      n=n2-n1;
      pl2.Build(n,fRing5[i].Get(3));
      m.setX(fr[1]);
      m.setY(0);
      m.setZ(fzshift+fz);
      w=m;
      w.rotateZ(dphi);
      pl3.Build(n1,m,w);
      FindInt(pl1,pl2,pl3,po2);
      
      fRing5[i].Set(2,po2);
      po1=po2;
      fRing5[i].Set(1,po1.rotateZ(dphi));
      // point 6,5
      pl2.Build(n,fRing5[i].Get(7));
      FindInt(pl1,pl2,pl3,po6);
      fRing5[i].Set(6,po6);
      po5=po6;
      fRing5[i].Set(5,po5.rotateZ(dphi));
   }
   
   // No. 1
   for(i=0;i<8;++i) {
      fRing5[1].Set(i,fRing5[1].Get(i).rotateZ(dphi1));
   }
   
   //===== No. 2--5 =====
   for(i=2;i<6;++i) {
      for(j=0;j<8;++j) {
	 fRing5[i].Set(j,fRing5[1].Get(j).rotateZ(fphi5[i]-fphi5[1]));
      }
   }
   // Finally, ring 5 is done. 
   // Check for ring 5
//   for(i=0;i<6;++i) {      
//      cout<<fRing5[i]<<endl;
//      fRing5[i].EndCapCheckout();
//   }

   // ******** Ring 4 ********
   EmcRecGeoPlane pl4,pl5,pl6;
   double dphip;
   HepPoint3D ttt;
   
   t.setX(fr[1]);
   t.setY(0);
   t.setZ(fzshift+fz);
   
   dphi1=fphi5[1]-fphi5[0];
   dphi2=fphi5[2]-fphi5[1];

   tantheta1=fr[1]/fz;
   costheta1=1/sqrt(1+tantheta1*tantheta1);
   sintheta1=costheta1*tantheta1;
   
   for(i=0;i<6;++i){
      fRing4[i].Type(EmcRecCrystal::SixPlane());
   }
   
   // It's too complicated. Boring!
   // up and down for crystal 0,1 and 4,5
   EmcRecCrystal up[2],down[2];
   for(i=0;i<2;++i) {
      if(i==0) {
	 dphi=dphi1;
      } else {
	 dphi=dphi2;
      }
      // first point 3,0, up still needs a rotation
      po3=t;
      down[i].Set(3,po3);
      po0=t;
      down[i].Set(0,po0.rotateZ(dphi));
      //
      po3=t;
      up[i].Set(3,po3);
      po0=t;
      up[i].Set(0,po0.rotateZ(dphi2));
      // then point 7,4, up still needs a rotation
      l=down[i].Get(3).distance(down[i].Get(0))/2;
      ll=sqrt(fz*fz+fr[1]*fr[1]);
      lp=ll*flength/sqrt(ll*ll-l*l);
      po7=t;
      po7.setX(po7.x()+lp*sintheta1);
      po7.setZ(po7.z()+lp*costheta1);
      down[i].Set(7,po7);
      po4=po7;
      down[i].Set(4,po4.rotateZ(dphi));
      
      l=up[i].Get(3).distance(up[i].Get(0))/2;
      ll=sqrt(fz*fz+fr[1]*fr[1]);
      lp=ll*flength/sqrt(ll*ll-l*l);
      po7=t;
      po7.setX(po7.x()+lp*sintheta1);
      po7.setZ(po7.z()+lp*costheta1);
      up[i].Set(7,po7);
      po4=po7;
      up[i].Set(4,po4.rotateZ(dphi2));
      
      up[i].Set(0,up[i].Get(0).rotateZ(dphi));
      up[i].Set(3,up[i].Get(3).rotateZ(dphi));
      up[i].Set(4,up[i].Get(4).rotateZ(dphi));
      up[i].Set(7,up[i].Get(7).rotateZ(dphi));
      // 0,3,4,7 is done.
      // switch to 1,2,5,6
      // for down
      pl1.Build(0,1,0,0);
      n2=(down[i].Get(0)+down[i].Get(3))/2;
      n=n2-n1;
      pl2.Build(n,down[i].Get(3));
      m.setX(fr[2]);
      m.setY(0);
      m.setZ(fzshift+fz);
      w=m;
      if(i==0) {
	 dphip=fphi5[2]-fphi5[0];
      } else {
	 dphip=fphi5[6]-fphi5[4];
      }
      w.rotateZ(dphip);      
      pl3.Build(n1,m,w);  // very important
      FindInt(pl1,pl2,pl3,po2);
      down[i].Set(2,po2);
      //
      pl4.Build(O,n1,down[i].Get(0));
      FindInt(pl4,pl2,pl3,po1);
      down[i].Set(1,po1);
      //
      // point 6,5
      pl2.Build(n,down[i].Get(7));
      FindInt(pl1,pl2,pl3,po6);
      down[i].Set(6,po6);
      //
      FindInt(pl4,pl2,pl3,po5);
      down[i].Set(5,po5);
      // for up
      // point 2, 1
      n2=(up[i].Get(0)+up[i].Get(3))/2;
      n=n2-n1;
      pl1.Build(O,n1,up[i].Get(3));
      pl2.Build(n,up[i].Get(3));
      FindInt(pl1,pl2,pl3,po2);
      up[i].Set(2,po2);
      //
      pl4.Build(O,n1,up[i].Get(0));
      FindInt(pl4,pl2,pl3,po1);
      up[i].Set(1,po1);
      // point 5, 6
      pl2.Build(n,up[i].Get(7));
      FindInt(pl1,pl2,pl3,po6);
      up[i].Set(6,po6);
      //
      FindInt(pl4,pl2,pl3,po5);
      up[i].Set(5,po5);
   
   }

   for(i=0;i<8;++i) {
      fRing4[0].Set(i,down[0].Get(i));
      fRing4[1].Set(i,up[0].Get(i));
      fRing4[4].Set(i,down[1].Get(i).rotateZ(fphi5[4]));
      fRing4[5].Set(i,up[1].Get(i).rotateZ(fphi5[4]));
   }

   // crystal 2 and 3
   dphi=fphi5[3]-fphi5[2];  
   // first, point 3
   po3=t;
   fRing4[2].Set(3,po3);
   // then, point 0
   po0=t;
   fRing4[2].Set(0,po0.rotateZ(dphi));
   // point 7 and point 4
   l=fRing4[2].Get(3).distance(fRing4[2].Get(0))/2;
   ll=sqrt(fz*fz+fr[1]*fr[1]);
   lp=ll*flength/sqrt(ll*ll-l*l);
   po7=t;
   po7.setX(po7.x()+lp*sintheta1);
   po7.setZ(po7.z()+lp*costheta1);
   fRing4[2].Set(7,po7);
   po4=po7;
   fRing4[2].Set(4,po4.rotateZ(dphi));
   //point 2, 1
   pl1.Build(0,1,0,0);
   n2=(fRing4[2].Get(0)+fRing4[2].Get(3))/2;
   n=n2-n1;
   pl2.Build(n,fRing4[2].Get(3));
   m.setX(fr[2]);
   m.setY(0);
   m.setZ(fzshift+fz);
   w=m;
   w.rotateZ(dphi);
   pl3.Build(n1,m,w);
   FindInt(pl1,pl2,pl3,po2);   
   fRing4[2].Set(2,po2);
   po1=po2;
   fRing4[2].Set(1,po1.rotateZ(dphi));
   // point 6,5
   pl2.Build(n,fRing4[2].Get(7));
   FindInt(pl1,pl2,pl3,po6);
   fRing4[2].Set(6,po6);
   po5=po6;
   fRing4[2].Set(5,po5.rotateZ(dphi));
   
   // Crystal 2, 3 still need a rotation.
   for(i=0;i<8;++i) {
      fRing4[2].Set(i,fRing4[2].Get(i).rotateZ(fphi5[2]));
   }
   for(i=0;i<8;++i) {
      fRing4[3].Set(i,fRing4[2].Get(i).rotateZ(fphi5[3]-fphi5[2]));
   }   
   
   // Finally done. Check it out.
//   for(i=0;i<6;++i) {
//      cout<<fRing4[i]<<endl;
//      fRing4[i].EndCapCheckout();
//   }

   // ***************** Ring3 ********************
   // Here I changed my way of calculation. 
   // Don't care for it. Still BORING!
   // do some preparation
   HepPoint3D base3[5];
   base3[0].setX(fr[2]); base3[0].setY(0); base3[0].setZ(fz+fzshift);
   for(i=1;i<5;++i) {
      base3[i]=base3[0];
   }
   base3[1].rotateZ(fphi5[2]);
   base3[2].rotateZ(fphi5[3]);
   base3[3].rotateZ(fphi5[4]);
   base3[4].rotateZ(fphi5[6]);
//   for(i=0;i<5;++i) {
//      cout<<base3[i]<<endl;
//   }
   
   HepPoint3D base2[6];
   base2[0].setX(fr[3]); base2[0].setY(0); base2[0].setZ(fz+fzshift); 
   for(i=1;i<6;++i) {
      base2[i]=base2[0];
      base2[i].rotateZ(fphi3[i]);
   }
//   for(i=0;i<6;++i) {
//      cout<<base2[i]<<endl;
//   }
//   cout<<endl;
   
   EmcRecGeoPlane pphi[6];
   for(i=0;i<6;++i) {
      pphi[i].Build(O,n1,base2[i]);
   }
   EmcRecGeoPlane ptheta[4];
   for(i=0;i<4;++i) {
      ptheta[i].Build(n1,base3[i],base3[i+1]);
   }
   EmcRecGeoPlane pthetap[5];
   for(i=0;i<5;++i) {
      pthetap[i].Build(n1,base2[i],base2[i+1]);
   }   

   // Once an error occor here. I just declare HepPoint3D nn[4];
   // Finally, the operation of nn[4] gets out of range. 
   // It has overlaped another varible.
   HepPoint3D nn[5];
   nn[0]=(base3[0]+base3[1])/2-n1;
   nn[1]=base3[1]-n1;
   nn[2]=base3[2]-n1;
   nn[3]=base3[3]-n1;
   nn[4]=(base3[3]+base3[4])/2-n1;
   
   EmcRecGeoPlane psection[5];
   for(i=0;i<5;++i) {
      psection[i].Build(nn[i],base3[i]);
   }

   EmcRecGeoPlane psection2[5];
   HepPoint3D bp[5];
   for(i=0;i<5;++i) {
      bp[i]=base3[i];
      bp[i].setX(bp[i].x()+flength*nn[i].x()/nn[i].mag());
      bp[i].setY(bp[i].y()+flength*nn[i].y()/nn[i].mag());
      bp[i].setZ(bp[i].z()+flength*nn[i].z()/nn[i].mag());
      psection2[i].Build(nn[i],bp[i]);
   }
   
   EmcRecGeoPlane pthetatmp;
   for(i=0;i<5;++i) {
      if(i==0||i==4) {
	 fRing3[i].Type(EmcRecCrystal::SixPlane());
	 if(i==0) { 
	    pthetatmp=ptheta[0]; 
	 }
	 if(i==4) { 
	    pthetatmp=ptheta[3];
	 }
	 FindInt(pphi[i],pthetatmp,psection[i],po3);
	 FindInt(pphi[i],pthetap[i],psection[i],po2);
	 FindInt(pphi[i+1],pthetatmp,psection[i],po0);
	 FindInt(pphi[i+1],pthetap[i],psection[i],po1);
	 fRing3[i].Set(0,po0);
	 fRing3[i].Set(1,po1); 
	 fRing3[i].Set(2,po2); 
	 fRing3[i].Set(3,po3);
	 FindInt(pphi[i],pthetatmp,psection2[i],po7);
	 FindInt(pphi[i],pthetap[i],psection2[i],po6);
	 FindInt(pphi[i+1],pthetatmp,psection2[i],po4);
	 FindInt(pphi[i+1],pthetap[i],psection2[i],po5);
	 fRing3[i].Set(4,po4);
	 fRing3[i].Set(5,po5);
	 fRing3[i].Set(6,po6);
	 fRing3[i].Set(7,po7);
      } else { 
	 fRing3[i].Type(EmcRecCrystal::SevenPlane());
	 po0=base3[i];
	 FindInt(pphi[i],ptheta[i-1],psection[i],po4);
	 FindInt(pphi[i],pthetap[i],psection[i],po3);
	 FindInt(pphi[i+1],ptheta[i],psection[i],po1);
	 FindInt(pphi[i+1],pthetap[i],psection[i],po2);
	 fRing3[i].Set(0,po0);
	 fRing3[i].Set(1,po1);
	 fRing3[i].Set(2,po2);
	 fRing3[i].Set(3,po3);
	 fRing3[i].Set(4,po4);
	 po5=bp[i];
	 FindInt(pphi[i],ptheta[i-1],psection2[i],po9);
	 FindInt(pphi[i],pthetap[i],psection2[i],po8);
	 FindInt(pphi[i+1],ptheta[i],psection2[i],po6);
	 FindInt(pphi[i+1],pthetap[i],psection2[i],po7);
	 fRing3[i].Set(5,po5);
	 fRing3[i].Set(6,po6);
	 fRing3[i].Set(7,po7);
	 fRing3[i].Set(8,po8);
	 fRing3[i].Set(9,po9);
      }
   }
   /// Finally, Ring 3 is done.
   /// The maximum descripancy is less than 1mm.
   /// However, it's hard to know where they from.
   /// There are too many approximation in structure design.
   /// It's already less than the tolerance of production.
   /// Then, omit them!
//   for(i=0;i<5;++i) {
//      cout<<fRing3[i]<<endl;
//      fRing3[i].EndCapCheckout();
//   }

   /// Get into Ring2
   HepPoint3D base1[4];
   base1[0].setX(fr[4]); base1[0].setY(0); base1[0].setZ(fz+fzshift);
   for(i=1;i<4;++i) {
      base1[i]=base1[0];
   }
   base1[1].rotateZ(fphi3[2]);
   base1[2].rotateZ(fphi3[3]);
   base1[3].rotateZ(fphi3[5]);
   EmcRecGeoPlane ptheta1[3];
   for(i=0;i<3;++i) {
      ptheta1[i].Build(n1,base1[i],base1[i+1]);
   }
      
   HepPoint3D nn2[5];
   for(i=0;i<5;++i) {
      nn2[i]=(base2[i]+base2[i+1])/2-n1;
   }
   EmcRecGeoPlane psec2[5];
   for(i=0;i<5;++i) {
      psec2[i].Build(nn2[i],base2[i]);
   }

   EmcRecGeoPlane psec2p[5];
   HepPoint3D bpp[5];
   for(i=0;i<5;++i) {
      bpp[i]=base2[i];
      bpp[i].setX(bpp[i].x()+flength*nn2[i].x()/nn2[i].mag());
      bpp[i].setY(bpp[i].y()+flength*nn2[i].y()/nn2[i].mag());
      bpp[i].setZ(bpp[i].z()+flength*nn2[i].z()/nn2[i].mag());
      psec2p[i].Build(nn2[i],bpp[i]);
   }
   
   EmcRecGeoPlane ptheta1tmp;
   for(i=0;i<5;++i) {
      fRing2[i].Type(EmcRecCrystal::SixPlane());
      if(i<2) { 
	 ptheta1tmp=ptheta1[0]; 
      }
      if(i==2) {
	 ptheta1tmp=ptheta1[1];
      }
      if(i>2) {
	 ptheta1tmp=ptheta1[2];
      }      
      po3=base2[i];
      FindInt(pphi[i],ptheta1tmp,psec2[i],po2); 
      po0=base2[i+1]; 
      FindInt(pphi[i+1],ptheta1tmp,psec2[i],po1);
      FindInt(pphi[i],pthetap[i],psec2p[i],po7);
      FindInt(pphi[i],ptheta1tmp,psec2p[i],po6);
      FindInt(pphi[i+1],pthetap[i],psec2p[i],po4);
      FindInt(pphi[i+1],ptheta1tmp,psec2p[i],po5);
      fRing2[i].Set(0,po0);
      fRing2[i].Set(1,po1);
      fRing2[i].Set(2,po2);
      fRing2[i].Set(3,po3);
      fRing2[i].Set(4,po4);
      fRing2[i].Set(5,po5);
      fRing2[i].Set(6,po6);
      fRing2[i].Set(7,po7);
   }
   
   /// Ring2 is done. It seems a little easier.
//   for(i=0;i<5;++i) {
//      cout<<fRing2[i]<<endl;
//      fRing2[i].EndCapCheckout();
//   }
  
   /// ************* Ring1 *************
   HepPoint3D base0[5];
   base0[0].setX(fr[5]); base0[0].setY(0); base0[0].setZ(fz+fzshift);
   for(i=1;i<5;++i) {
      base0[i]=base0[0];
      base0[i].rotateZ(fphi1[i]);
   }
   
   EmcRecGeoPlane pphi1[5];
   for(i=0;i<5;++i) {
      pphi1[i].Build(O,n1,base0[i]);
   }
   
   EmcRecGeoPlane ptheta0[4];
   for(i=0;i<4;++i) {
      ptheta0[i].Build(n1,base0[i],base0[i+1]);
   }
   
   HepPoint3D nn1[4];
   nn1[0]=(base1[0]+base1[1])/2-n1;
   nn1[1]=base1[1]-n1;
   nn1[2]=base1[2]-n1;
   nn1[3]=(base1[2]+base1[3])/2-n1;
   
   EmcRecGeoPlane psec1[4];
   for(i=0;i<4;++i) {
      psec1[i].Build(nn1[i],base1[i]);
   }
   
   EmcRecGeoPlane psec1p[4];
   HepPoint3D qq[4];
   for(i=0;i<4;++i) {
      qq[i]=base1[i];
      qq[i].setX(qq[i].x()+flength*nn1[i].x()/nn1[i].mag());
      qq[i].setY(qq[i].y()+flength*nn1[i].y()/nn1[i].mag());
      qq[i].setZ(qq[i].z()+flength*nn1[i].z()/nn1[i].mag());
      psec1p[i].Build(nn1[i],qq[i]);
   }   
   
   EmcRecGeoPlane pt1tmp;
   for(i=0;i<4;++i) {
      if(i==0||i==3) {
	 fRing1[i].Type(EmcRecCrystal::SixPlane());
	 if(i==0) {
	    pt1tmp=ptheta1[0];
	 } else {
	    pt1tmp=ptheta1[2];
	 }
	 FindInt(pphi1[i],pt1tmp,psec1[i],po3);
	 FindInt(pphi1[i],ptheta0[i],psec1[i],po2);
	 FindInt(pphi1[i+1],pt1tmp,psec1[i],po0);
	 FindInt(pphi1[i+1],ptheta0[i],psec1[i],po1);
	 FindInt(pphi1[i],pt1tmp,psec1p[i],po7);
	 FindInt(pphi1[i],ptheta0[i],psec1p[i],po6);
	 FindInt(pphi1[i+1],pt1tmp,psec1p[i],po4);
	 FindInt(pphi1[i+1],ptheta0[i],psec1p[i],po5);
	 fRing1[i].Set(0,po0);
	 fRing1[i].Set(1,po1);
	 fRing1[i].Set(2,po2);
	 fRing1[i].Set(3,po3);
	 fRing1[i].Set(4,po4);
	 fRing1[i].Set(5,po5);
	 fRing1[i].Set(6,po6);
	 fRing1[i].Set(7,po7);
      } else {
	 fRing1[i].Type(EmcRecCrystal::SevenPlane());
         po0=base1[i];
	 FindInt(pphi1[i],ptheta1[i-1],psec1[i],po4);
	 FindInt(pphi1[i],ptheta0[i],psec1[i],po3);
	 FindInt(pphi1[i+1],ptheta1[i],psec1[i],po1);
	 FindInt(pphi1[i+1],ptheta0[i],psec1[i],po2);	 
	 fRing1[i].Set(0,po0);
	 fRing1[i].Set(1,po1);
	 fRing1[i].Set(2,po2);
	 fRing1[i].Set(3,po3);
	 fRing1[i].Set(4,po4);
	 po5=qq[i];
	 FindInt(pphi1[i],ptheta1[i-1],psec1p[i],po9);
	 FindInt(pphi1[i],ptheta0[i],psec1p[i],po8);
	 FindInt(pphi1[i+1],ptheta1[i],psec1p[i],po6);
	 FindInt(pphi1[i+1],ptheta0[i],psec1p[i],po7);
	 fRing1[i].Set(5,po5);
	 fRing1[i].Set(6,po6);
	 fRing1[i].Set(7,po7);
	 fRing1[i].Set(8,po8);
	 fRing1[i].Set(9,po9);	 
      }
   }

   /// Check it out!
//   for(i=0;i<4;++i) {
//      cout<<fRing1[i]<<endl;
//      fRing1[i].EndCapCheckout();
//   }
   /// Last ring! Ring0
   HepPoint3D basem1[5];
   basem1[0].setX(fr[6]); basem1[0].setY(0); basem1[0].setZ(fz+fzshift);
   for(i=1;i<5;++i) {
      basem1[i]=basem1[0];
      basem1[i].rotateZ(fphi1[i]);
   }

   EmcRecGeoPlane pthetam1[4];
   for(i=0;i<4;++i) {
      pthetam1[i].Build(n1,basem1[i],basem1[i+1]);
   }
   
   HepPoint3D nn0[4];
   for(i=0;i<4;++i) {
      nn0[i]=(base0[i]+base0[i+1])/2-n1;
   }
   
   EmcRecGeoPlane psec0[4];
   for(i=0;i<4;++i) {
      psec0[i].Build(nn0[i],base0[i]);
   }
   
   EmcRecGeoPlane psec0p[4];
   HepPoint3D qq0[4];
   for(i=0;i<4;++i) {
      qq0[i]=base0[i];
      qq0[i].setX(qq0[i].x()+flength*nn0[i].x()/nn0[i].mag());
      qq0[i].setY(qq0[i].y()+flength*nn0[i].y()/nn0[i].mag());
      qq0[i].setZ(qq0[i].z()+flength*nn0[i].z()/nn0[i].mag());
      psec0p[i].Build(nn0[i],qq0[i]);
   }

   for(i=0;i<4;++i) {
      fRing0[i].Type(EmcRecCrystal::SixPlane());
      po3=base0[i];
      FindInt(pphi1[i],pthetam1[i],psec0[i],po2);
      po0=base0[i+1];
      FindInt(pphi1[i+1],pthetam1[i],psec0[i],po1);
      FindInt(pphi1[i],ptheta0[i],psec0p[i],po7);
      FindInt(pphi1[i],pthetam1[i],psec0p[i],po6);
      FindInt(pphi1[i+1],ptheta0[i],psec0p[i],po4);
      FindInt(pphi1[i+1],pthetam1[i],psec0p[i],po5);
      fRing0[i].Set(0,po0);
      fRing0[i].Set(1,po1);
      fRing0[i].Set(2,po2);
      fRing0[i].Set(3,po3);
      fRing0[i].Set(4,po4);
      fRing0[i].Set(5,po5);
      fRing0[i].Set(6,po6);
      fRing0[i].Set(7,po7);
   }
   
   /// Sector1 is finished. !!!
//   for(i=0;i<4;++i) {
//      cout<<fRing0[i]<<endl;
//      fRing0[i].EndCapCheckout();
//   }
   for(i=0;i<6;++i) {
      if(i<4) {
	 fCrystal[0][0][i]=fRing0[i];
	 fCrystal[0][1][i]=fRing1[i];
      }
      if(i<5) {
	 fCrystal[0][2][i]=fRing2[i];
	 fCrystal[0][3][i]=fRing3[i];
      }
      fCrystal[0][4][i]=fRing4[i];
      fCrystal[0][5][i]=fRing5[i];
   }
}

void EmcRecEndCapGeo::CalculateStandardSector2()
{
   EmcRecCrystal edge[6];
   
   int i,j;

   for(i=1;i<6;++i) {
      fRing5p[i]=fRing5[i];
      fRing4p[i]=fRing4[i];
   }
   for(i=1;i<5;++i) {
      fRing3p[i]=fRing3[i];
      fRing2p[i]=fRing2[i];
   }
   for(i=1;i<4;++i) {
      fRing1p[i]=fRing1[i];
      fRing0p[i]=fRing0[i];
   }
   
   edge[5]=fRing5[0];
   edge[4]=fRing4[0]; 
   edge[3]=fRing3[0]; 
   edge[2]=fRing2[0]; 
   edge[1]=fRing1[0]; 
   edge[0]=fRing0[0]; 
   
   EmcRecGeoPlane p10mm;
   p10mm.Build(0,1,0,-10);

   EmcRecGeoPlane sec1,sec2;
   EmcRecGeoPlane theta1,theta2;
   
   HepPoint3D po3,po2,po7,po6;
   
   for(i=0;i<6;++i) {
      sec1.Build(edge[i].Get(0),edge[i].Get(1),edge[i].Get(2));
      sec2.Build(edge[i].Get(4),edge[i].Get(5),edge[i].Get(6));
      theta1.Build(edge[i].Get(2),edge[i].Get(5),edge[i].Get(6));
      theta2.Build(edge[i].Get(3),edge[i].Get(4),edge[i].Get(7));
      
      FindInt(sec1,theta1,p10mm,po2);
      FindInt(sec1,theta2,p10mm,po3); 
      FindInt(sec2,theta1,p10mm,po6); 
      FindInt(sec2,theta2,p10mm,po7);
      
      edge[i].Set(2,po2);
      edge[i].Set(3,po3);
      edge[i].Set(6,po6);
      edge[i].Set(7,po7);
   }
   
   fRing5p[0]=edge[5];
   fRing4p[0]=edge[4];
   fRing3p[0]=edge[3];
   fRing2p[0]=edge[2];
   fRing1p[0]=edge[1];
   fRing0p[0]=edge[0];   
   
   double pio2=3.14159265358979/2;

   /// rotate 90 degrees
   for(i=0;i<6;++i) {
      for(j=0;j<10;++j) {
	 fRing5p[i].Set(j,fRing5p[i].Get(j).rotateZ(pio2));
	 fRing4p[i].Set(j,fRing4p[i].Get(j).rotateZ(pio2));
      }
   }
   for(i=0;i<5;++i) {
      for(j=0;j<10;++j) { 
	 fRing3p[i].Set(j,fRing3p[i].Get(j).rotateZ(pio2));
	 fRing2p[i].Set(j,fRing2p[i].Get(j).rotateZ(pio2));
      }
   }
   for(i=0;i<4;++i) {
      for(j=0;j<10;++j) { 
	 fRing1p[i].Set(j,fRing1p[i].Get(j).rotateZ(pio2));
	 fRing0p[i].Set(j,fRing0p[i].Get(j).rotateZ(pio2));
      }
   }
   
   // check it!!!
//   for(i=0;i<4;++i) {
//      cout<<fRing1p[i]<<endl;
//   }
//   for(i=0;i<5;++i) {   
//      cout<<fRing3p[i]<<endl;
//   }
//   for(i=0;i<6;++i) {
//      cout<<fRing5p[i]<<endl;
//   }
   for(i=0;i<6;++i) {
      if(i<4) {
	 fCrystal[1][0][i]=fRing0p[i];
	 fCrystal[1][1][i]=fRing1p[i];
      }
      if(i<5) {
	 fCrystal[1][2][i]=fRing2p[i];
	 fCrystal[1][3][i]=fRing3p[i];
      }
      fCrystal[1][4][i]=fRing4p[i];
      fCrystal[1][5][i]=fRing5p[i];
   }
}

void EmcRecEndCapGeo::FillCCenterVector()
{
   unsigned int module;
   unsigned int phi,phimax,theta;
   Identifier id;
   EmcRecCrystal aCry;
   HepPoint3D aCenter;
   HepPoint3D aFrontCenter;
   
   module=EmcID::getENDCAP_EAST();
   for(theta=0;theta<=5;++theta) {
      phimax=EmcID::getPHI_ENDCAP_MAX(theta);
      for(phi=0;phi<=phimax;++phi) {
	 id=EmcID::crystal_id(module,theta,phi);
	 aCry=GetCrystal(id);
	 aCenter=aCry.Center();
   aFrontCenter=aCry.FrontCenter();
	 fCCenter.push_back(aCenter);
   fCFrontCenter.push_back(aFrontCenter);
      }
   }

   module=EmcID::getENDCAP_WEST();
   for(theta=0;theta<=5;++theta) {
      phimax=EmcID::getPHI_ENDCAP_MAX(theta);
      for(phi=0;phi<=phimax;++phi) {
	 id=EmcID::crystal_id(module,theta,phi);
	 aCry=GetCrystal(id);
	 aCenter=aCry.Center();
   aFrontCenter=aCry.FrontCenter();
	 fCCenter.push_back(aCenter);
   fCFrontCenter.push_back(aFrontCenter);
      }
   }
}
   
EmcRecCrystal EmcRecEndCapGeo::GetCrystal(const Identifier& id) const
{
   int i;
   EmcRecCrystal cry;
   unsigned int module=EmcID::barrel_ec(id);
   unsigned int theta=EmcID::theta_module(id);
   unsigned int phi=EmcID::phi_module(id);
   
   unsigned int phiMax,phiMax16;
   unsigned int phiQuotient,phiRemainder;
   
   phiMax=EmcID::getPHI_ENDCAP_MAX(theta);
   phiMax16=(phiMax+1)/16;   
   phiQuotient=(unsigned int)(phi/phiMax16);
   phiRemainder=phi%phiMax16;

   //cout<<phiQuotient<<" "<<phiRemainder<<endl;
   
   if(module==EmcID::getENDCAP_EAST()) {
      if(phiQuotient!=3&&phiQuotient!=4&&
	 phiQuotient!=11&&phiQuotient!=12) {
	 cry=fCrystal[0][theta][phiRemainder];
	 for(i=0;i<10;++i) {
	    cry.Set(i,cry.Get(i).rotateZ(phiQuotient*fphi5[6]));
	 }
      }	 else {
	 if(phiQuotient==4) {
	    cry=fCrystal[1][theta][phiRemainder];
	 }
	 if(phiQuotient==3) {
	    cry=fCrystal[1][theta][phiMax16-1-phiRemainder];
	    for(i=0;i<10;++i) {
	       cry.SetX(i,-cry.Get(i).x());
	    }
	 }
	 if(phiQuotient==11) {
	    cry=fCrystal[1][theta][phiMax16-1-phiRemainder];
	    for(i=0;i<10;++i) {
	       cry.SetY(i,-cry.Get(i).y());
	    }
	 }
	 if(phiQuotient==12) {
	    cry=fCrystal[1][theta][phiRemainder];
	    for(i=0;i<10;++i) {
	       cry.SetX(i,-cry.Get(i).x());
	       cry.SetY(i,-cry.Get(i).y());
	    }
	 }
      }
   }
   
   if(module==EmcID::getENDCAP_WEST()) {
      unsigned int phipp;
      unsigned int phiMax2=(phiMax+1)/2;
      if(phi<phiMax2) {
	 phipp=phiMax2-1-phi;
      } else {
	 phipp=phiMax+phiMax2-phi;
      }
      Identifier idd=EmcID::crystal_id(EmcID::getENDCAP_EAST(),theta,phipp);
      cry=GetCrystal(idd);
      for(i=0;i<10;++i) {
	 cry.SetX(i,-cry.Get(i).x());
	 cry.SetZ(i,-cry.Get(i).z());
      }
   }      
   
   return cry;
}

HepPoint3D EmcRecEndCapGeo::GetCCenter(const Identifier& id) const
{
   unsigned int module,theta,phi;   
   unsigned int i,j;
   
   module=EmcID::barrel_ec(id);
   theta=EmcID::theta_module(id);
   phi=EmcID::phi_module(id);
   
   i=0;
   if(module==EmcID::getENDCAP_EAST()) {
      for(j=0;j<theta;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      i+=(phi+1);
   } else {
      for(j=0;j<6;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      for(j=0;j<theta;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      i+=(phi+1); 
   }
   
   return fCCenter[i-1];
}

HepPoint3D EmcRecEndCapGeo::GetCFrontCenter(const Identifier& id) const
{
   unsigned int module,theta,phi;   
   unsigned int i,j;
   
   module=EmcID::barrel_ec(id);
   theta=EmcID::theta_module(id);
   phi=EmcID::phi_module(id);
   
   i=0;
   if(module==EmcID::getENDCAP_EAST()) {
      for(j=0;j<theta;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      i+=(phi+1);
   } else {
      for(j=0;j<6;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      for(j=0;j<theta;++j) {
	 i+=(EmcID::getPHI_ENDCAP_MAX(j)+1);
      }
      i+=(phi+1); 
   }
   
   return fCFrontCenter[i-1];
}


void EmcRecEndCapGeo::FindInt(const EmcRecGeoPlane& p1,
			      const EmcRecGeoPlane& p2,
			      const EmcRecGeoPlane& p3,
			      HepPoint3D &p)
{
   // solve a system of linear equation 
   // The form is AX=B
   HepMatrix A(3,3);
   HepVector B(3);
   HepVector X(3);
   
   A(1,1)=p1.a();   A(1,2)=p1.b();   A(1,3)=p1.c();   B(1)=-p1.d();
   A(2,1)=p2.a();   A(2,2)=p2.b();   A(2,3)=p2.c();   B(2)=-p2.d();
   A(3,1)=p3.a();   A(3,2)=p3.b();   A(3,3)=p3.c();   B(3)=-p3.d();
   
   X=solve(A,B);
//   cout<<A;
//   cout<<B;
//   cout<<X;
 
   p.setX(X(1));
   p.setY(X(2));
   p.setZ(X(3));
//   cout<<p;
}
   
   
