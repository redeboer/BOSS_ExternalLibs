////////////////////////////////////////////////////////////
///  Obtains TOF hit IDs that match MDC tracks found by  ///
///  Belle Fast Tracker, fzisan.                         ///
///                                                      ///
///  How:                                                ///
///     Extrapolates fzisan tracks (helix) to the inner  ///
///     surface of TOF counters to obtain corresponding  ///
///     IDs.                                             ///
///                                                      ///
///  Function:  TofFz_get(double rtof, int id)            ///
///               rtof: Input TOF counter radius         ///
///               id:   Input fzisan track Id            ///
///  Return IDs:                                         ///
///             1:  OK                                   ///
///            -1:  wrong fzisan ID                      ///
///            -3:  track multiplicity <= 0              ///
///            -5:  inclomplete fitting in fzisan        ///
///            -7:  bad path length or Z_tof             ///
///                                                      ///
///  Based on:  Tof_helix class                          ///
///                                                      ///
///  Author:  H.Kichimi (for TRASAN tracks)              ///
///                                                      ///
///  Modifications:                                      ///
///    Modified for fzisan tracks S.Behari 15/01/2000    ///
/////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"
#include "TofRawEvent/TofDigi.h"
#include "McTruth/McParticle.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include <vector>
#include <iostream>
#include <cstdio>
#include <math.h>

#include "EsTimeAlg/Toffz_helix.h"
#include "CLHEP/Vector/ThreeVector.h"   
#include "MdcFastTrkAlg/FTFinder.h"
#include "TrackUtil/Helix.h"
#include "CLHEP/Geometry/Point3D.h"
 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
  typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using CLHEP::HepVector; 
using CLHEP::Hep3Vector;


TofFz_helix::TofFz_helix(void) {
   piby1=3.141593;
   pi2=2.0*piby1;
   piby44=piby1/44.0;
   piby24=piby1/24.0;
   piby18=piby1/18.0;
   _debug=1;
   _pathl_cut= 500.0;
   _ztof_cutm=-140.0;
   _ztof_cutx= 140.0;

 
}


int TofFz_helix::TofFz_Get(double rtof, int id, double fzisan[]) {
   // crossing points of the helix on the tof cylinder
   double xc,yc;            // center of the circle
   double rho;              // radius of tof cyclinder
   double  xx[2],yy[2];      // coordinates of hits
   double  tofx,tofy;        // (x,y) on the radius of R_tof
   double  nx,ny;            // direction cosines of mom vector at vertex
   double  cosx,sinx;
   double aa,ca,cb,cc,detm;
  
   // fzisan track ID and TOF radius
   int Id_cdfztrk = id; R_tof = rtof;

    if( Id_cdfztrk<0 ) {
     if (_debug) std::cout << " TofFz_helix *** wrong id_cdfztrk ="
			   << Id_cdfztrk << std::endl;
                                                  return(-1);
   }
   
   const HepPoint3D pivot1(0.,0.,0.);
   HepVector  a(5,0);
   a[0] = fzisan[0];
   a[1] = fzisan[1];
   a[2] = fzisan[2];
   a[3] = fzisan[3];
   a[4] = fzisan[4];


   // Ill-fitted track in fzisan (dz=tanl=-9999.)
   if (abs(a[3])>50.0 || abs(a[4])>500.0)          return (-5);
  // if (abs(a[3])>10.0 || abs(a[4])>500.0)          return (-5);

   // D e f i n e   h e l i x
   Helix helix1(pivot1,a);
   Dr    = helix1.a()[0];   
   Phi0  = helix1.a()[1];   
   Kappa = helix1.a()[2];    
   Dz    = helix1.a()[3];   
   Tanl  = helix1.a()[4];

   // check cdfztrk hit on tof cyclinder
   rho  = helix1.radius(); 
   // cout<<" Toffz_helix:: rho  ="<<rho<<endl;  // radius of the circle in cm
   HepPoint3D    xyz    = helix1.center();
   xc   = xyz(0);
   yc   = xyz(1);
   // cout<<"Toffz_helix::helix center:xc="<<xc<<",yc= "<<yc<<endl;

   Hep3Vector vxyz   = helix1.direction();
   nx   = vxyz(0);              // direction of track at the vertex
   ny   = vxyz(1); 
   // cout<<"Toffz_helix::direction: nx= "<<nx<<", ny="<<ny<<endl;
  
   if(fabs(rho)>R_tof/2){
   
   // get hit on tof cylinder at radius R_tof;

   if( xc==0.0 && yc==0.0 ) return(-3); 
   // coefficients of quadratic equation
   ca = xc*xc + yc*yc ;
   aa = 0.5 * ( ca - rho*rho + R_tof*R_tof );
   
   if ( xc != 0.0 ) {
      cb = aa * yc;
      cc = aa*aa - R_tof*R_tof*xc*xc; 
      // determinant
      detm = cb*cb - ca*cc;
      if ( detm > 0.0 ) {
	 yy[0] = ( cb + sqrt(detm) )/ ca;
	 xx[0] = ( aa - yy[0]*yc )/xc;
	 yy[1] = ( cb - sqrt(detm) )/ ca;
	 xx[1] = ( aa - yy[1]*yc )/xc;
      } else  return(-1);
   }
   else{
      cb = aa * xc;
      cc = aa*aa - R_tof*R_tof*yc*yc;
      // determinant
      detm = cb*cb - ca*cc;
      if ( detm > 0.0 ) {
	 xx[0] = ( cb + sqrt(detm) )/ca;
	 yy[0] = ( aa - xx[0]*xc )/yc;
	 xx[1] = ( cb - sqrt(detm) )/ca;
	 yy[1] = ( aa - xx[1]*xc )/yc;
      } else return(-2);
   }
 
   // choose one of hits according to track direction at the vertex.
  
   if( xx[0]*nx + yy[0]*ny > 0.0 ) { tofx = xx[0]; tofy = yy[0]; }
   else                            { tofx = xx[1]; tofy = yy[1]; }
  
   double fi = atan2(tofy ,tofx );    // atna2 from -pi to pi

   
   if( fi < 0.0 ) fi=pi2+fi;
   Fi_tof = fi;         
   //  Tofid = (int) ( Fi_tof/piby44 + 1.0 );
   Tofid = (int) ( Fi_tof/piby44  );
   
   // get phi and z on the tof counter
   const HepPoint3D pivot2( tofx, tofy, 0.0);
   helix1.pivot(pivot2);
   
   // corrected by Ozaki san to avoid negative path length on Aug.10,99   
   Phi1=helix1.a()[1];
   Z_tof=helix1.a()[3];
   
   double dphi = (-xc*(tofx-xc)-yc*(tofy-yc))/sqrt(xc*xc+yc*yc)/fabs(rho);
   dphi = acos(dphi);
   Pathl = fabs(rho*dphi);
   double coscor=sqrt(1.0+Tanl*Tanl);
   Pathl=Pathl*coscor;

   // path length in tof scintillator
   Hep3Vector vxyz1   = helix1.direction();
   nx   = vxyz1(0);            // direction of track at the vertex
   ny   = vxyz1(1); 
   //   cout<<" Toffz_helix::track direction: nx="<<nx<<",ny="<<ny<<endl;
   double corxy=(nx*tofx+ny*tofy)/R_tof;
   Path_tof=4.0*coscor/corxy;
   // cout<<" Toffz_helix::Path_tof="<< Path_tof<<endl;
   if(abs(Z_tof)<115)  return (1);
   //  if(Z_tof>116.5){


   //std::cout << "   helix1.a()[3]    " << helix1.a()[3] <<std::endl;

   if(Z_tof>=115){
     // Scintillator East Endcap TOF
     Z_tof = 133;
     Pathl = Z_tof/sin(atan(Tanl));
     double phi0 = -(Z_tof/Tanl)/rho;
     double phi  = -(Z_tof-Dz)/Tanl/rho;
     double phi1 = (Z_tof*Kappa*0.003)/Tanl;
     double phi2 = (Z_tof-Dz)*Kappa*0.003/Tanl;
     
     double x_endtof = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi));
     double y_endtof = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi));
     r_endtof = sqrt(x_endtof * x_endtof + y_endtof * y_endtof);
     
     Helix helix1(pivot1,a); 
     
     double x1_endtof=helix1.x(phi).x();
     double y1_endtof=helix1.x(phi).y();
     double z1_endtof=helix1.x(phi).z(); 
     
     double fi_endtof = atan2(y_endtof,x_endtof );    // atna2 from -pi to pi
     if (fi_endtof<0) fi_endtof=pi2+fi_endtof;
     
     Tofid =(int)(fi_endtof/piby24);
     if(Tofid>47)Tofid=Tofid-48;
   
     // MRPC East Endcap TOF
     // using hardware provide number: 1330+1+1+4.73 / mingming 133.7
     double Z_etf_1 = 133.673;
     double Pathl_1 = Z_etf_1/sin(atan(Tanl));
     double phi0_1 = -(Z_etf_1/Tanl)/rho;
     double phi_1  = -(Z_etf_1-Dz)/Tanl/rho;
     double phi1_1 = (Z_etf_1*Kappa*0.003)/Tanl;
     double phi2_1 = (Z_etf_1-Dz)*Kappa*0.003/Tanl;
     
     double x1_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_1));
     double y1_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_1));
     double r_etf_1 = sqrt( x1_etf*x1_etf + y1_etf*y1_etf );
     
     double x_etf_1 = helix1.x(phi_1).x();
     double y_etf_1 = helix1.x(phi_1).y();
     double z_etf_1 = helix1.x(phi_1).z(); 
     
     // the sysmetric axis of east endcap has a degree shift
     double fi_etf_1 = atan2( y1_etf, x1_etf ) + 1.25*piby1/180.0;
     if( fi_etf_1<0 ) { fi_etf_1 = pi2 + fi_etf_1; }

     int Etfid_1 = (int)(fi_etf_1/piby18);
     if( Etfid_1>35 ) { Etfid_1 = Etfid_1 - 36; }
     
     // Inner Layer
     if( Etfid_1%2 == 1 ) {
       Etfid = Etfid_1;
       r_etf = r_etf_1;
       Z_etf = Z_etf_1;
       Path_etf = Pathl_1;
     }
     // Outer Layer
     else {
       // using hardware provide number 1357.5+1+2.5+4.73 / mingming 136.4
       double Z_etf_2 = 136.573;
       double Pathl_2 = Z_etf_2/sin(atan(Tanl));
       double phi0_2 = -(Z_etf_2/Tanl)/rho;
       double phi_2  = -(Z_etf_2-Dz)/Tanl/rho;
       double phi1_2 = (Z_etf_2*Kappa*0.003)/Tanl;
       double phi2_2 = (Z_etf_2-Dz)*Kappa*0.003/Tanl;
       
       double x2_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_2));
       double y2_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_2));
       double r_etf_2 = sqrt( x2_etf*x2_etf + y2_etf*y2_etf );
       
       double x_etf_2 = helix1.x(phi_2).x();
       double y_etf_2 = helix1.x(phi_2).y();
       double z_etf_2 = helix1.x(phi_2).z(); 
       
       // the sysmetric axis of east endcap has a 3.75 degree shift
       double fi_etf_2 = atan2( y2_etf, x2_etf ) + 1.25*piby1/180.0 ;
       if( fi_etf_2<0 ) { fi_etf_2 = pi2 + fi_etf_2; }
       
       int Etfid_2 = (int)(fi_etf_2/piby18);
       if( Etfid_2>35 ) { Etfid_2 = Etfid_2 - 36; }
       
       if( Etfid_2%2 == 1 ) {
	 int tmp1 = (int)((fi_etf_2+0.5*piby18)/piby18);
	 int tmp2 = (int)((fi_etf_2-0.5*piby18)/piby18);
	 if( tmp1 == Etfid_2 ) { Etfid_2 = tmp2; }
	 else { Etfid_2 = tmp1; }
       }
       
       Etfid = Etfid_2;
       r_etf = r_etf_2;
       Z_etf = Z_etf_2;
       Path_etf = Pathl_2;
     }

     return (0);

   }
   else if (Z_tof<=-115){
     // Scintillator West Endcap TOF
     Z_tof = -133;
     Pathl = Z_tof/sin(atan(Tanl));
     double phi0 = -(Z_tof/Tanl)/rho;
     double phi  = -(Z_tof-Dz)/Tanl/rho;
     double phi1 = (Z_tof*Kappa*0.003)/Tanl;
     double phi2 = (Z_tof-Dz)*Kappa*0.003/Tanl;
     
     double x_endtof = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi));
     double y_endtof = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi));
     r_endtof = sqrt(x_endtof * x_endtof + y_endtof * y_endtof);
     
     Helix helix1(pivot1,a);   
     
     double x1_endtof=helix1.x(phi).x();
     double y1_endtof=helix1.x(phi).y();
     double z1_endtof=helix1.x(phi).z(); 
     
     double fi_endtof = atan2(y_endtof,x_endtof );    // atna2 from -pi to pi
     if (fi_endtof<0) fi_endtof=pi2+fi_endtof;
     
     Tofid =(int)(fi_endtof/piby24);
     if(Tofid>47) Tofid=Tofid-48;

     // MRPC West Endcap TOF
     double Z_etf_1 = -133.673;
     double Pathl_1 = Z_etf_1/sin(atan(Tanl));
     double phi0_1 = -(Z_etf_1/Tanl)/rho;
     double phi_1  = -(Z_etf_1-Dz)/Tanl/rho;
     double phi1_1 = (Z_etf_1*Kappa*0.003)/Tanl;
     double phi2_1 = (Z_etf_1-Dz)*Kappa*0.003/Tanl;
     
     double x1_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_1));
     double y1_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_1));
     double r_etf_1 = sqrt( x1_etf*x1_etf + y1_etf*y1_etf );
     
     double x_etf_1 = helix1.x(phi_1).x();
     double y_etf_1 = helix1.x(phi_1).y();
     double z_etf_1 = helix1.x(phi_1).z(); 
     
     // the sysmetric axis of west endcap has a 16.25 degree shift
     double fi_etf_1 = atan2( y1_etf, x1_etf ) - 11.25*piby1/180.0;
     if( fi_etf_1<0 ) { fi_etf_1 = pi2 + fi_etf_1; }
     
     int Etfid_1 = (int)(fi_etf_1/piby18);
     if( Etfid_1>35 ) { Etfid_1 = Etfid_1 - 36; }
     
     // Inner Layer
     if( Etfid_1%2 == 1 ) {
       Etfid = Etfid_1;
       r_etf = r_etf_1;
       Z_etf = Z_etf_1;
       Path_etf = Pathl_1;
     }
     // Outer Layer
     else {
       double Z_etf_2 = -136.573;
       double Pathl_2 = Z_etf_2/sin(atan(Tanl));
       double phi0_2 = -(Z_etf_2/Tanl)/rho;
       double phi_2  = -(Z_etf_2-Dz)/Tanl/rho;
       double phi1_2 = (Z_etf_2*Kappa*0.003)/Tanl;
       double phi2_2 = (Z_etf_2-Dz)*Kappa*0.003/Tanl;
       
       double x2_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_2));
       double y2_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_2));
       double r_etf_2 = sqrt( x2_etf*x2_etf + y2_etf*y2_etf );
       
       double x_etf_2 = helix1.x(phi_2).x();
       double y_etf_2 = helix1.x(phi_2).y();
       double z_etf_2 = helix1.x(phi_2).z(); 
       
       // the sysmetric axis of west endcap has a 16.25 degree shift
       double fi_etf_2 = atan2( y2_etf, x2_etf ) - 11.25*piby1/180.0;
       if( fi_etf_2<0 ) { fi_etf_2 = pi2 + fi_etf_2; }
       
       int Etfid_2 = (int)(fi_etf_2/piby18);
       if( Etfid_2>35 ) { Etfid_2 = Etfid_2 - 36; }
       
       if( Etfid_2%2 == 1 ) {
	 int tmp1 = (int)((fi_etf_2+0.5*piby18)/piby18);
	 int tmp2 = (int)((fi_etf_2-0.5*piby18)/piby18);
	 if( tmp1 == Etfid_2 ) { Etfid_2 = tmp2; }
	 else { Etfid_2 = tmp1; }
       }
       
       Etfid = Etfid_2;
       r_etf = r_etf_2;
       Z_etf = Z_etf_2;
       Path_etf = Pathl_2;
     }

     return (2);

   }

   }
   else {
     if(Tanl>0){
       // Scintillator East Endcap TOF
       Z_tof = 133;
       Pathl = Z_tof/sin(atan(Tanl));
       double phi0 = -(Z_tof/Tanl)/rho;
       double phi  = -(Z_tof-Dz)/Tanl/rho;
       double phi1 = (Z_tof*Kappa*0.003)/Tanl;
       double phi2 = (Z_tof-Dz)*Kappa*0.003/Tanl;
       
       double x_endtof=Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi));
       double y_endtof=Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi));
       r_endtof=sqrt(x_endtof * x_endtof + y_endtof * y_endtof);
       
       double fi_endtof = atan2(y_endtof,x_endtof );    // atna2 from -pi to pi
       if (fi_endtof<0) fi_endtof=pi2+fi_endtof;
       Tofid =(int)(fi_endtof/piby24);
       if(Tofid>47)Tofid=Tofid-48;
     
       // MRPC East Endcap TOF
       double Z_etf_1 = 133.673;
       double Pathl_1 = Z_etf_1/sin(atan(Tanl));
       double phi0_1 = -(Z_etf_1/Tanl)/rho;
       double phi_1  = -(Z_etf_1-Dz)/Tanl/rho;
       double phi1_1 = (Z_etf_1*Kappa*0.003)/Tanl;
       double phi2_1 = (Z_etf_1-Dz)*Kappa*0.003/Tanl;
     
       double x1_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_1));
       double y1_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_1));
       double r_etf_1 = sqrt( x1_etf*x1_etf + y1_etf*y1_etf );

       double x_etf_1 = helix1.x(phi_1).x();
       double y_etf_1 = helix1.x(phi_1).y();
       double z_etf_1 = helix1.x(phi_1).z(); 
       
       // the sysmetric axis of east endcap has a 3.75 degree shift
       double fi_etf_1 = atan2( y1_etf, x1_etf ) + 1.25*piby1/180.0;
       if( fi_etf_1<0 ) { fi_etf_1 = pi2 + fi_etf_1; }
       
       int Etfid_1 = (int)(fi_etf_1/piby18);
       if( Etfid_1>35 ) { Etfid_1 = Etfid_1 - 36; }
       
       // Inner Layer
       if( Etfid_1%2 == 1 ) {
	 Etfid = Etfid_1;
	 r_etf = r_etf_1;
	 Z_etf = Z_etf_1;
	 Path_etf = Pathl_1;
       }
       // Outer Layer
       else {
	 double Z_etf_2 = 136.573;
	 double Pathl_2 = Z_etf_2/sin(atan(Tanl));
	 double phi0_2 = -(Z_etf_2/Tanl)/rho;
	 double phi_2  = -(Z_etf_2-Dz)/Tanl/rho;
	 double phi1_2 = (Z_etf_2*Kappa*0.003)/Tanl;
	 double phi2_2 = (Z_etf_2-Dz)*Kappa*0.003/Tanl;
	 
	 double x2_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_2));
	 double y2_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_2));
	 double r_etf_2 = sqrt( x2_etf*x2_etf + y2_etf*y2_etf );
	 
	 double x_etf_2 = helix1.x(phi_2).x();
	 double y_etf_2 = helix1.x(phi_2).y();
	 double z_etf_2 = helix1.x(phi_2).z(); 
	 
	 // the sysmetric axis of east endcap has a 3.75 degree shift
	 double fi_etf_2 = atan2( y2_etf, x2_etf ) + 1.25*piby1/180.0;
	 if( fi_etf_2<0 ) { fi_etf_2 = pi2 + fi_etf_2; }
	 
	 int Etfid_2 = (int)(fi_etf_2/piby18);
	 if( Etfid_2>35 ) { Etfid_2 = Etfid_2 - 36; }
	 
	 if( Etfid_2%2 == 1 ) {
	   int tmp1 = (int)((fi_etf_2+0.5*piby18)/piby18);
	   int tmp2 = (int)((fi_etf_2-0.5*piby18)/piby18);
	   if( tmp1 == Etfid_2 ) { Etfid_2 = tmp2; }
	   else { Etfid_2 = tmp1; }
	 }
	 Etfid = Etfid_2;
	 r_etf = r_etf_2;
	 Z_etf = Z_etf_2;
	 Path_etf = Pathl_2;
       }

       return (0);
     }
     else{
       // Scintillator West Endcap TOF
       Z_tof = -133;
       Pathl = Z_tof/sin(atan(Tanl));
       double phi0 = -(Z_tof/Tanl)/rho;
       double phi  = -(Z_tof-Dz)/Tanl/rho;
       double phi1 = (Z_tof*Kappa*0.003)/Tanl;
       double phi2 = (Z_tof-Dz)*Kappa*0.003/Tanl;
       
       double x_endtof=Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi));
       double y_endtof=Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi));
       r_endtof=sqrt(x_endtof * x_endtof + y_endtof * y_endtof);
       
       double fi_endtof = atan2(y_endtof,x_endtof );    // atna2 from -pi to pi
       if (fi_endtof<0) fi_endtof=pi2+fi_endtof;
       Tofid =(int)(fi_endtof/piby24);
       if(Tofid>47)Tofid=Tofid-48;
     
       // MRPC West Endcap TOF
       double Z_etf_1 = -133.673;
       double Pathl_1 = Z_etf_1/sin(atan(Tanl));
       double phi0_1 = -(Z_etf_1/Tanl)/rho;
       double phi_1  = -(Z_etf_1-Dz)/Tanl/rho;
       double phi1_1 = (Z_etf_1*Kappa*0.003)/Tanl;
       double phi2_1 = (Z_etf_1-Dz)*Kappa*0.003/Tanl;
       
       double x1_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_1));
       double y1_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_1));
       double r_etf_1 = sqrt( x1_etf*x1_etf + y1_etf*y1_etf );
       
       double x_etf_1 = helix1.x(phi_1).x();
       double y_etf_1 = helix1.x(phi_1).y();
       double z_etf_1 = helix1.x(phi_1).z(); 
       
       // the sysmetric axis of west endcap has a 16.25 degree shift
       double fi_etf_1 = atan2( y1_etf, x1_etf ) - 11.25*piby1/180.0;
       if( fi_etf_1<0 ) { fi_etf_1 = pi2 + fi_etf_1; }
       
       int Etfid_1 = (int)(fi_etf_1/piby18);
       if( Etfid_1>35 ) { Etfid_1 = Etfid_1 - 36; }
       
       if( Etfid_1%2 == 1 ) {
	 Etfid = Etfid_1;
	 r_etf = r_etf_1;
	 Z_etf = Z_etf_1;
	 Path_etf = Pathl_1;
       }
       else {
	 double Z_etf_2 = -136.573;
	 double Pathl_2 = Z_etf_2/sin(atan(Tanl));
	 double phi0_2 = -(Z_etf_2/Tanl)/rho;
	 double phi_2  = -(Z_etf_2-Dz)/Tanl/rho;
	 double phi1_2 = (Z_etf_2*Kappa*0.003)/Tanl;
	 double phi2_2 = (Z_etf_2-Dz)*Kappa*0.003/Tanl;
	 
	 double x2_etf = Dr*cos(Phi0)+(1/(Kappa*0.003))*(cos(Phi0)-cos(Phi0+phi_2));
	 double y2_etf = Dr*sin(Phi0)+(1/(Kappa*0.003))*(sin(Phi0)-sin(Phi0+phi_2));
	 int r_etf_2 = sqrt( x2_etf*x2_etf + y2_etf*y2_etf );
	 
	 double x_etf_2 = helix1.x(phi_2).x();
	 double y_etf_2 = helix1.x(phi_2).y();
	 double z_etf_2 = helix1.x(phi_2).z(); 
	 
	 // the sysmetric axis of west endcap has a 16.25 degree shift
	 double fi_etf_2 = atan2( y2_etf, x2_etf ) - 11.25*piby1/180.0;
	 if( fi_etf_2<0 ) { fi_etf_2 = pi2 + fi_etf_2; }
	 
	 int Etfid_2 = (int)(fi_etf_2/piby18);
	 if( Etfid_2>35 ) { Etfid_2 = Etfid_2 - 36; }
	 
	 if( Etfid_2%2 == 1 ) {
	   int tmp1 = (int)((fi_etf_2+0.5*piby18)/piby18);
	   int tmp2 = (int)((fi_etf_2-0.5*piby18)/piby18);
	   if( tmp1 == Etfid_2 ) { Etfid_2 = tmp2; }
	   else { Etfid_2 = tmp1; }
	 }
	 
	 Etfid = Etfid_2;
	 r_etf = r_etf_2;
	 Z_etf = Z_etf_2;
	 Path_etf = Pathl_2;
       }

       return (2);
     }
   }

   if (abs(Pathl) > _pathl_cut || 
       Z_tof < _ztof_cutm || Z_tof > _ztof_cutx) {
     // Bad path length or Z_tof
     if (_debug) {
       printf("\n TofFz_helix> Trk=%3d  params(dr,phi,kappa,dz,tanl)="
	      "(%5.1f,%6.3f,%6.4f,%6.1f,%6.3f) R_tof %5.1f\n",
	      Id_cdfztrk,Dr,Phi0,Kappa,Dz,Tanl,R_tof);
       
       printf(" TofFz_helix> rho=%8.1f, (xc,yc)=(%8.1f,%8.1f)"
	      " (nx,ny)=(%5.2f,%5.2f)\n",rho,xc,yc,nx,ny);
      
       printf(" TofFz_helix> tof (x,y)=(%5.1f,%5.1f) and (%5.1f,%5.1f)\n",
	      xx[0],yy[0],xx[1],yy[1]);
     
       printf(" TofFz_helix> tofid=%3d, fitof=%6.3f, w=%5.3f"
	      " (x,y,z)=(%5.1f,%5.1f,%5.1f)  pathl=%5.1f cm  path=%5.1f cm\n",
	      Tofid,Fi_tof,W_tof,tofx,tofy,Z_tof,Pathl,Path_tof);
     }
     return (-7);
   }
          
}
