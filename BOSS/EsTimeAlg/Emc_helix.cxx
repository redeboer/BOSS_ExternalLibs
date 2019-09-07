////////////////////////////////////////////////////////////
///  Obtains EMC hit postion that match MDC tracks found ///
///  by  BESIII  Fast Tracker, fzisan.                   ///
///                                                      ///
///  How:                                                ///
///     Extrapolates fzisan tracks (helix) to the inner  ///
///     surface of EMC  to obtain corresponding          ///
///      position.                                       ///
///                                                      ///
///  Function:    Emc_get(double remc, int id)            ///
///               rEmc: Input EMC counter radius         ///
///               id:   Input fzisan track Id            ///
///  Return IDs:                                         ///
///             1:  OK                                   ///
///            -1:  wrong fzisan ID                      ///
///            -3:  track multiplicity <= 0              ///
///            -5:  inclomplete fitting in fzisan        ///
///            -7:  bad path length or Z_emc             ///
///                                                      ///
///  Based on:  helix class                              ///
///                                                      ///
///                                                      ///
/////////////////////////////////////////////////////////////

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/PropertyMgr.h"
#include "EventModel/Event.h"
#include "MdcRawEvent/MdcDigi.h"
#include "EmcRawEvent/EmcDigi.h"
#include "McTruth/McParticle.h"

#include "MdcGeomSvc/IMdcGeomSvc.h"
#include "MdcGeomSvc/MdcGeoWire.h"
#include "MdcGeomSvc/MdcGeoLayer.h"
#include <vector>
#include <iostream>
#include <cstdio>


#include "EsTimeAlg/Emc_helix.h"
#include "CLHEP/Vector/ThreeVector.h"   
#include "MdcFastTrkAlg/FTFinder.h"

#include "CLHEP/Geometry/Point3D.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
    typedef HepGeom::Point3D<double> HepPoint3D;
#endif

using CLHEP::HepVector; 
using CLHEP::Hep3Vector;


Emc_helix::Emc_helix(void) {
   piby1=3.141593;
   pi2=2.0*piby1;
   _debug=0;
   _pathl_cut= 1000.0;
  
 }


int Emc_helix::Emc_Get(double rEmc, int id, double fzisan[]) {
   // crossing points of the helix on the Emc cylinder
   double xc,yc;            // center of the circle
   double rho;              // radius of emc  cyclinder
   double  xx[2],yy[2];      // coordinates of hits
   double  emcx,emcy;        // (x,y) on the radius of R_emc
   double  nx,ny;            // direction cosines of mom vector at vertex
   double  cosx,sinx;
   double aa,ca,cb,cc,detm;
  
   
   // fzisan track ID and EMC radius
   int Id_cdfztrk = id; R_emc = rEmc;

   if( Id_cdfztrk<0 ) {
     if (_debug) std::cout << " Emc_helix *** wrong id_cdfztrk ="
			   << Id_cdfztrk << std::endl;
                                                  return(-1);
   }
   
   // fzisan track
   /*    FTFinder * ftrk = FTFinder:: GetPointer();
   FTList<FTTrack *> & FsTrks = ftrk->tracks();
   NTrk = FsTrks.length();
     
   if (NTrk<=0)                                   return (-3);

   // get helix params for Id_cdfztrk 
   FTTrack & trk = * FsTrks[Id_cdfztrk];
   const Helix hel = * trk.helix();

   const HepPoint3D pivot1(0.,0.,0.);
   HepVector  a(5,0);
   a[0] = hel.a()[0];
   a[1] = hel.a()[1];
   a[2] = hel.a()[2];
   a[3] = hel.a()[3];
   a[4] = hel.a()[4];
   */
   const HepPoint3D pivot1(0.,0.,0.);
   HepVector  a(5,0);
   a[0] = fzisan[0];
   a[1] = fzisan[1];
   a[2] = fzisan[2];
   a[3] = fzisan[3];
   a[4] = fzisan[4];

   // Ill-fitted track in fzisan (dz=tanl=-9999.)
   if (abs(a[3])>50.0 || abs(a[4])>500.0)          return (-5);

   // D e f i n e   h e l i x
   Helix helix1(pivot1,a);
   Dr    = helix1.a()[0];   
   Phi0  = helix1.a()[1];   
   Kappa = helix1.a()[2];    
   Dz    = helix1.a()[3];   
   Tanl  = helix1.a()[4];

   /* double detaphi=asin(0.5*0.81*0.3*fabs(Kappa));
      double phi=Phi0+detaphi+ piby1/2;
      */
    
   // check cdfztrk hit on Emc cyclinder
   rho  = helix1.radius(); 
   // cout<<" Emc_helix:: rho  ="<<rho<<endl;  // radius of the circle in cm
   HepPoint3D    xyz    = helix1.center();
   xc   = xyz(0);
   yc   = xyz(1);

   Hep3Vector vxyz   = helix1.direction();
   nx   = vxyz(0);              // direction of track at the vertex
   ny   = vxyz(1); 
   
   // get hit on Emc cylinder at radius R_Emc;
   if( xc==0.0 && yc==0.0 ) return(-3);
   
   // coefficients of quadratic equation
   ca = xc*xc + yc*yc ;
   aa = 0.5 * ( ca - rho*rho + R_emc*R_emc );
   
   if ( xc != 0.0 ) {
      cb = aa * yc;
      cc = aa*aa - R_emc*R_emc*xc*xc; 
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
      cc = aa*aa - R_emc*R_emc*yc*yc;
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
  
  if( xx[0]*nx + yy[0]*ny > 0.0 ) { emcx = xx[0]; emcy = yy[0]; }
   else                            { emcx = xx[1]; emcy = yy[1]; }
    
   double fi = atan2(emcy ,emcx );    // atna2 from -pi to pi

    if( fi < 0.0 ) fi=pi2+fi;
   Fi_emc = fi;         
     
   // get phi and z on the emc counter
   const HepPoint3D pivot2( emcx, emcy, 0.0);
   helix1.pivot(pivot2);
   
   // corrected by Ozaki san to avoid negative path length on Aug.10,99   
   Phi1=helix1.a()[1];
   Z_emc=helix1.a()[3];

   //get(thepa,phi)from the intersection (x,y,z)
   Hep3Vector intersec(emcx, emcy, Z_emc);
    theta_emc = intersec.getTheta();
    phi_emc = intersec.getPhi();
  
   return(1);

}
           

