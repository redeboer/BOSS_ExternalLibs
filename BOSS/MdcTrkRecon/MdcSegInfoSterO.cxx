//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegInfoSterO.cxx,v 1.18 2012/04/25 05:32:59 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors:
//
// Copyright (C)  1996  The Board of Trustees of  
// 
// History:
// 	Zhang Yao(zhangyao@ihep.ac.cn)	Migrate to BESIII
//
// The Leland Stanford Junior University.  All Rights Reserved.
//------------------------------------------------------------------------

#include "MdcTrkRecon/MdcSegInfoSterO.h"
#include "MdcGeom/MdcSuperLayer.h"
#include "TrkBase/TrkRecoTrk.h"
#include "MdcGeom/BesAngle.h"
#include "MdcTrkRecon/MdcSegWorks.h"
#include "MdcTrkRecon/MdcSeg.h"
#include "CLHEP/Alist/AList.h" 
#include "MdcTrkRecon/mdcTwoInv.h"
#include "MdcGeom/MdcDetector.h"
#include "TrkBase/TrkExchangePar.h"
#include "TrkBase/TrkHelixUtils.h"
#include "TrkBase/TrkFit.h"
#include "MdcTrkRecon/MdcLine.h"
#include "MdcData/MdcHit.h"
#include "MdcData/MdcHitUse.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include <math.h>
#include "BField/BField.h"
#include "MdcGeom/Constants.h"
using std::endl;
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

//--------------------------------------------------------------------------
bool MdcSegInfoSterO::parIsAngle(int i) const {
  //---------------------------------------------------------------------------
  assert (i >= 0 && i < 2);
  return false;
}
//----------------------------------------------------------------------------
int MdcSegInfoSterO::calcStereo(MdcSeg *parentSeg, 
    const TrkRecoTrk &track, MdcSegWorks &segStuff) {
  //----------------------------------------------------------------------------
  const TrkFit* tkFit = track.fitResult();
  if (tkFit == 0) return 1;
  TrkExchangePar par = tkFit->helix(0.0);
  double Bz = track.bField().bFieldZ();//??
  return calcStereo(parentSeg, par, segStuff,Bz);
}
//----------------------------------------------------------------------------
int MdcSegInfoSterO::calcStereo(MdcSeg *parentSeg, 
    const TrkExchangePar &par, MdcSegWorks &segStuff,double Bz) {
  //----------------------------------------------------------------------------
  _debug = 0;//yzhang debug
  // phiAx = approximate phi of axial track @ stereo slayer
  // return 1 if calculation failed
  const MdcSuperLayer *slayer = parentSeg->superlayer();

  double kap = 0.5 * par.omega();
  double d0 = par.d0();
  BesAngle phi0(par.phi0());
  BesAngle phiSeg(parentSeg->phi());
  double radius = slayer->rEnd();
  double dPhiZ = (-1)*slayer->delPhiinv() * slayer->zEnd();
  if (_debug >0){
    parentSeg->plotSeg();
    std::cout << "delPhi "<<slayer->delPhi()
      <<" delPhiInv "<<slayer->delPhiinv()
      <<" dPhiZ "<<dPhiZ 
      <<" zEnd() "<<slayer->zEnd() 
      <<" phiAx "<<segStuff.phiAx
      <<" phiSeg "<<phiSeg
      <<" phiDiff "<<phiSeg - segStuff.phiAx<< std::endl;//yzhang debug
  }
  bool lStraight = false;
  if (fabs(kap)<1.e-9) lStraight = true;

  BesAngle phiDiff = phiSeg - segStuff.phiAx;
  double zApprox = phiDiff.rad() * -dPhiZ;
  if (_debug >0){
    std::cout<<"zApp  "<<zApprox<<std::endl;//yzhang debug
  }
  double delRad = slayer->stDip() *
    (1. - zApprox * zApprox * segStuff.wirLen2inv);
  radius -= delRad;
  double delPhiArg;
  if (lStraight) {
    delPhiArg = delRad * d0 / (radius*radius);
  }else {
    delPhiArg = -delRad * kap;
  }


  //---------------------yzhang fix-------------------
  MdcLine span(12);
  int hitFit=0;
  bool szFaild = false;
  double arcTemp=0;

  for (int ihit = 0; ihit < parentSeg->nHit(); ihit++){
    //*** Calc. z and correct for stereo dip
    //int szCode = zPosition(*(parentSeg->hit(ihit)), track, &span,hitFit,parentSeg->bunchTime());
    int szCode = zPosition(*(parentSeg->hit(ihit)), par, &span,hitFit,parentSeg->bunchTime(),Bz);//yzhang delete 2011-05-11 
    //int szCode = -1;

    //std::cout<< __FILE__ << "   " << __LINE__ << " calcStereo szCode  "<<szCode<<std::endl;
    if (szCode > 0){
      span.sigma[hitFit] = 1;
      arcTemp += span.x[hitFit];
      hitFit ++;
    }else{
      szFaild = true;
      if (_debug >0){
	parentSeg->hit(ihit)->mdcHit()->print(std::cout);
	std::cout<<"MdcSegInfoSterO hit "<<ihit<<" z calc faild"<<std::endl;//yzhang debug 
      }
    }
  }
  if (hitFit >0) span.fit(hitFit);
  //zhangy 

  // series expand for asin, dropping (delphiarg)**2 and higher terms
  segStuff.phiAx += delPhiArg + 
    0.5 * delPhiArg * segStuff.phiArg * segStuff.phiArg;  
  phiDiff = phiSeg - segStuff.phiAx;  
  double z = phiDiff.rad() * -dPhiZ;
  if (_debug >0){
    std::cout<<"z  "<<z<<std::endl;//yzhang debug 
  }
  double ct, z0, arc;
  if (lStraight) {  //straight track
    //*** Calculate parameters
    double arg = radius*radius - d0*d0;
    if (arg <= 0.0) return 1;
    double rD0Root = sqrt(arg);
    double rD0Rinv = 1. / rD0Root;
    double rinv = 1./radius;
    // ct 
    double slope = parentSeg->slope();
    ct = dPhiZ * (rD0Root * slope + d0 * rinv) * rinv;
    // z0
    arc = TrkHelixUtils::fltToRad(par, radius);
    if (arc == 0.0) return 1;
    z0 = z - ct * arc;

    if (_debug>0){
      std::cout << "in MdcSegInfoSterO : z0 "<<z0 <<" "<<_errmat[0]
	<<" ct "<<ct<<" "<<_errmat[1]
	<<" arc "<<arc<<" "<<_errmat[2]
	<<endl;//yzhang debug
    }
    // calculate errors
    double dctdm = dPhiZ * rD0Root * rinv;
    double dctdD = -dPhiZ * rinv * (slope * d0 * rD0Rinv - rinv);
    double dzdm = -arc * dPhiZ * rD0Root * rinv;
    double dzdphi = dPhiZ;
    double dzdphi0 = -dPhiZ;
    double dzdD = -dPhiZ + ct * d0 * rD0Rinv - arc * dctdD;

    const double *inErr = parentSeg->errmat(); 
    //z0
    const HepMatrix trackErr = par.covariance();
    _errmat[0] = dzdm * dzdm * inErr[2] +
      2 * dzdm    * dzdphi  * inErr[1] + 
      dzdphi  * dzdphi  * inErr[0] + 
      dzdD  * dzdD  * trackErr(1,1) +
      dzdphi0 * dzdphi0 * trackErr(2,2) +
      2 * dzdD  * dzdphi0 * trackErr(1,2);
    if (_errmat[0] < 0.) _errmat[0] = 0.;

    //ct
    _errmat[2] = dctdm * dctdm * inErr[2] +
      dctdD * dctdD * trackErr(1,1);
    if (_errmat[2] < 0.) _errmat[2] = 0.;

    // off-diag
    _errmat[1] = dzdm * dctdm * inErr[2] +
      dzdphi  * dctdm   * inErr[1] +
      dzdD  * dctdD * trackErr(1,1) +
      dzdphi0 * dctdD * trackErr(1,2);
  }  
  else { // curved track (treated as from origin)
    //*** Calculate parameters
    // ct
    double arg = 1. - kap*kap * radius*radius;
    if (arg < 0.0) return 1;
    double rKapRoot = sqrt(arg);
    double rKapRinv = 1. / rKapRoot;
    double ctFactor = -rKapRoot * -dPhiZ;
    double slopeAx = kap * rKapRinv;
    double slope = parentSeg->slope();

    //std::cout<<"slopeAx="<<slopeAx<<" slopeSeg="<<slope
    //	    <<"\n diff "<<slopeAx - slope
    //	    <<" fac "<<ctFactor<<" rKapRoot "<<rKapRoot
    //	    <<" dPhiZ "<<dPhiZ<<std::endl;//yzhang debug	  
    //inner use szPozision	    
    //if (slayer->index()>1 || szFaild)//inner chamber use sz algorithm	   
    if (szFaild){ //use orinal algorithm
      //original 
      ct = (slopeAx - slope) * ctFactor;
      arc = TrkHelixUtils::fltToRad(par, radius);
      if (arc == 0.0) return 1;
      z0 = z - ct * arc;
    }else{
      //yzhang use belle s/z
      if (!szFaild){ 
	arc = arcTemp/hitFit;
	ct = span.slope;    
	z0 = span.intercept;
      }else{
	ct = 999;
	z0 = 999;
	arc = 999;
      }
    }
    if (_debug >0){//yzhang debug
      std::cout<<"--slayer NO. "<<slayer->index()<<std::endl;//yzhang debug
      std::cout<<"ori ct "<<(slopeAx - slope) * ctFactor
	<<" z0 "<<z - ct * TrkHelixUtils::fltToRad(par, radius)
	<<" arc "<<TrkHelixUtils::fltToRad(par, radius)<<std::endl;
      std::cout<<"fix ct "<<span.slope<<" z0 "<<span.intercept
	<<" arc "<<arcTemp/hitFit<<std::endl;
      std::cout<<"--------  "<<std::endl;//yzhang debug
    }
    // Calculate errors -- eliminate a bunch of terms when I get around to it
    double dctdm = dPhiZ * rKapRoot;
    double dctdkap = -dPhiZ * ( 1 + radius * radius * kap * rKapRinv * slope);
    double dzdm = arc * -dPhiZ * rKapRoot;
    double dzdphi = dPhiZ;
    double dzdkap = dPhiZ * radius * rKapRinv - arc * dctdkap -
      ct * ( radius * rKapRinv / kap - arc / kap);
    double dzdphi0 = -dPhiZ ;

    const double *inErr = parentSeg->errmat(); 
    //z0
    const HepMatrix trackErr = par.covariance();
    _errmat[0] = dzdm * dzdm * inErr[2] +
      2 * dzdm    * dzdphi  * inErr[1] + 
      dzdphi  * dzdphi  * inErr[0] + 
      dzdkap  * dzdkap  * trackErr(3,3) +
      dzdphi0 * dzdphi0 * trackErr(2,2) +
      2 * dzdkap  * dzdphi0 * trackErr(2,3);
    /*
       std::cout<<"dzdm  "<<dzdm
       <<" inErr[2] "<<inErr[2]
       <<" inErr[1] "<<inErr[1]
       <<" inErr[0] "<<inErr[0]
       <<" dzdphi "<<dzdphi
       <<" dzdkap "<<dzdkap
       <<" dzdphi0 "<<dzdphi0
       <<" trackErr3,3 "<<trackErr(3,3)
       <<" trackErr2,2 "<<trackErr(2,2)
       <<" trackErr2,3 "<<trackErr(2,3)				
       <<std::endl;
       */
    if (_errmat[0] < 0.) _errmat[0] = 0.;

    //ct
    _errmat[2] = dctdm * dctdm * inErr[2] +
      dctdkap * dctdkap * trackErr(3,3);
    if (_errmat[2] < 0.) _errmat[2] = 0.;

    //off-diag
    _errmat[1] = dzdm * dctdm * inErr[2] +
      dzdphi  * dctdm   * inErr[1] +
      dzdkap  * dctdkap * trackErr(3,3) +
      dzdphi0 * dctdkap * trackErr(2,3);

  }  // end branch on straight/curved


  // Load parameters
  _par0 = z0;
  _par1 = ct;
  _arc = arc;

  long error = mdcTwoInv(_errmat, _inverr);
  if (error) {
    std::cout << " ErrMsg(warning)" << "Failed to invert matrix -- MdcSegInfo::calcStereo" << 
      endl << _errmat[0] << " " << _errmat[1] << " " << _errmat[2]<< std::endl;
  }
  return 0;

}

int MdcSegInfoSterO::zPosition(MdcHitUse & hitUse, const TrkExchangePar &par, MdcLine* span,int hitFit, double t0, double Bz) const{

  int debug =0;


  const MdcHit &  h = *(hitUse.mdcHit());

  HepPoint3D fp ( h.wire()->getWestPoint()->x(),h.wire()->getWestPoint()->y(),h.wire()->getWestPoint()->z());
  HepPoint3D rp ( h.wire()->getEastPoint()->x(),h.wire()->getEastPoint()->y(),h.wire()->getEastPoint()->z());


  HepVector3D X = 0.5 * (fp + rp);
  if(debug>0){
    std::cout<<"----------  "<<std::endl;//yzhang debug
    h.print(std::cout);
    //h.wire()->print(std::cout);
    std::cout<<"fp rp "<<fp<<" "<<rp<<std::endl;//yzhang debug
    std::cout<<"Xmid  "<<X<<std::endl;//yzhang debug
  }
  HepVector3D xx = HepVector3D(X.x(), X.y(), 0.);
  //center of helix/circle
  //change to belle param

  double d0 = -par.d0();
  double phi0 = (par.phi0()-pi/2);
  double _charge = -1;
  if((-1)*par.omega()/Bz > 0) _charge = 1;

  double r;
  if (fabs(par.omega())< Constants::epsilon) r = 9999.;
  else r = 1/par.omega();

  double xc = sin(par.phi0()) *(-d0+r) * -1.; //z axis verse,x_babar = -x_belle
  double yc = -1.*cos(par.phi0()) *(-d0+r) * -1;//???
  HepPoint3D center (xc, yc, 0. );

  HepVector3D yy = center - xx;
  HepVector3D ww = HepVector3D(yy.x(), yy.y(), 0.);
  double wwmag2 = ww.mag2();
  double wwmag = sqrt(wwmag2);
  int ambig = hitUse.ambig();
  //dirftDist 
  double driftdist = fabs( h.driftDist(t0,ambig) );
  HepVector3D lr(driftdist/wwmag * ww.x(),
      driftdist/wwmag * ww.y(), 0.);
  if (debug >0){
    std::cout<<"xc "<<xc << " yc "<<yc<< " drfitdist "<<driftdist<<std::endl;
    std::cout<<"r1 "<<r <<" d0 "<<d0 <<" phi0 "<<phi0<<std::endl;//
    std::cout<<"lr "<<lr<<" hit ambig "<<hitUse.ambig()<<" ambig "<<ambig
      <<" left "<<h.driftDist(0,1)
      <<" right "<<h.driftDist(0,-1)<<std::endl;
  }

  //...Check left or right...
  HepPoint3D ORIGIN = HepPoint3D(0., 0., 0.);
  //ambig
  //-1 right -phi direction driftDist-=lr 
  //, +1 left +phi directon driftDist+=lr
  if (ambig == 0) lr = ORIGIN;
  if (_charge*Bz < 0){ //yzhang
    if (ambig == -1){
      lr = - lr;//right
    }
  }else{
    if (ambig == 1){
      lr = - lr;//left
    }
  }
  X += lr;

  //...Prepare vectors...
  //    HepPoint3D center = _helix->center();
  HepPoint3D tmp(-9999., -9999., 0.);
  HepVector3D x = HepVector3D(X.x(), X.y(), 0.);
  HepVector3D w = x - center;
  // //modified the next sentence because the direction are different from belle.
  HepVector3D V = h.wire()->zAxis();
  HepVector3D v = HepVector3D(V.x(), V.y(), 0.);
  double vmag2 = v.mag2();
  //double vmag = sqrt(vmag2);

  //double r = _helix->curv();
  double wv = w.dot(v);
  //    wv = abs(wv);
  double d2 = wv * wv - vmag2 * (w.mag2() - r * r);
  if (debug >0){
    std::cout<<"X_fix "<<X<<" center "<<center<<std::endl;
    std::cout<<"V "<<V<<std::endl;//yzhang debug
    std::cout<<"w "<<w<<" v "<<v<<std::endl;      
  }
  //...No crossing in R/Phi plane... This is too tight...

  if (d2 < 0.) {
    //hitUse.position(tmp);
    if (debug>0){
      std::cout << "in MdcSegInfoSterO !!! stereo: 0. > d2 = " << d2 << " "
	<< hitUse.ambig() << std::endl;
    }
    return -1;
  }
  double d = sqrt(d2);

  //...Cal. length to crossing points...
  double l[2];
  if (debug >0){
    std::cout<<"wv "<<wv<<" d "<<d<<" vmag2 "<<vmag2<<std::endl;//yzhang debug
  }
  l[0] = (- wv + d) / vmag2;
  l[1] = (- wv - d) / vmag2;

  //...Cal. z of crossing points...
  bool ok[2];
  ok[0] = true;
  ok[1] = true;
  double z[2];
  z[0] = X.z() + l[0] * V.z();
  z[1] = X.z() + l[1] * V.z();
  if (debug >0){//yzhang debug
    std::cout << "X.z  "<<X.z()<<" V.z "<<V.z()<<std::endl;
    std::cout << "l0, l1 = " << l[0] << ", " << l[1] << std::endl;
    std::cout << "rpz " << rp.z() << " fpz " << fp.z() << std::endl;
    std::cout << "z0  " << z[0] << " z1  " << z[1] << std::endl;
  }
  //...Check z position...
  if (hitUse.ambig() == 0) {
    if(debug>0)std::cout<< " ambig = 0  " <<std::endl;
    if (z[0] > rp.z()+20.  || z[0] < fp.z()-20.) {
      ok[0] = false;
    }
    if (z[1] > rp.z()+20.  || z[1] < fp.z()-20.) {
      ok[1] = false;
    }
  } else {
    if(debug>0)std::cout<< " ambig != 0  " <<std::endl;
   // if (z[0] > rp.z() || z[0] < fp.z() ) 
    if (fabs(z[0]/rp.z())>1.05 ) { ok[0] = false; }
    //if (z[1] > rp.z() || z[1] < fp.z() ) 
    if (fabs(z[1]/rp.z())>1.05 ) { ok[1] = false; }
  }
  if ((! ok[0]) && (! ok[1])) {
      if(debug>0&&(hitUse.ambig()!=0)&&fabs(z[1]/rp.z())>1.05)std::cout<< " z[1] bad " << std::endl;
      if(debug>0&&(hitUse.ambig()!=0)&&fabs(z[0]/rp.z())>1.05)std::cout<< " z[0] bad " << std::endl;
    //hitUse.position(tmp);
    if(debug>0) {
      std::cout<< " z[1] bad " << "rpz " << rp.z() << " fpz " << fp.z() 
	      << "z0  " << z[0] << " z1  " << z[1] << std::endl;
      std::cout<< " !ok[0] && !ok[1]  return -2" <<std::endl;
    }
    return -2;
  }

  //...Cal. xy position of crossing points...
  HepVector3D p[2];
  p[0] = x + l[0] * v;
  p[1] = x + l[1] * v;

  if(debug>0){
    std::cout<<__FILE__<<" "<<__LINE__<< " p0 "<<p[0].x()<<" "<<p[0].y()<< std::endl;
    std::cout<<__FILE__<<" "<<__LINE__<< " p1 "<<p[1].x()<<" "<<p[1].y()<< std::endl;
    std::cout<<__FILE__<<" "<<__LINE__<< " c "<<center.x()<<" "<<center.y()<<" "<<_charge<< std::endl;
    std::cout<<__FILE__<<" "<<__LINE__<< " p0 centerx*y "<<center.x() * p[0].y()<<" centery*x"<<center.y() * p[0].x()<< std::endl;
    std::cout<<__FILE__<<" "<<__LINE__<< " p1 centerx*y "<<center.x() * p[1].y()<<" centery*x"<<center.y() * p[1].x()<< std::endl;
  }

  //delete for cosmic
  //if (_charge * (center.x() * p[0].y() - center.y() * p[0].x())  < 0.){
  //  ok[0] = false;
  //}
  //if (_charge * (center.x() * p[1].y() - center.y() * p[1].x())  < 0.){
  //  ok[1] = false;
  //}
  //if ((! ok[0]) && (! ok[1])){
  //  if(debug>0) std::cout<< " !ok[0] && !ok[1]  reuturn -3" <<std::endl; 
  //  return -3;
  //}

  //...Which one is the best?... Study needed...
  unsigned best = 0;
  if (ok[1]) best = 1;
  //std::cout<<"in MdcSegInfoSterO  Zbelle "<<z[best]<<std::endl;//yzhang debug

  //...Cal. arc length...
  double cosdPhi = - center.dot((p[best] - center).unit()) / center.mag();
  double dPhi;
  if(fabs(cosdPhi)<=1.0) {
    dPhi = acos(cosdPhi);
  } else if (cosdPhi>1.0) {
    dPhi = 0.0;
  } else {
    dPhi = pi;	
  }

  //...Finish...
  tmp.setX(abs(r * dPhi));
  tmp.setY(z[best]);
  //hitUse.position(tmp);

  //z
  span->y[hitFit] = z[best];
  //if (ok[0]) span->y[hitFit] = p[0].mag();//r
  //else if(ok[1]) span->y[hitFit] = p[1].mag();
  //else span->y[hitFit] = tmp.mag();
  span->x[hitFit] = abs(r * dPhi);
  if (hitUse.ambig()<0) driftdist *= -1.;
  span->sigma[hitFit] = h.sigma(driftdist,hitUse.ambig());

  if (debug >0){
    std::cout<<"("<<h.layernumber()<<","<<h.wirenumber()<<") s "<<span->x[hitFit]<<" z "<<span->y[hitFit]<<std::endl;//yzhang debug
  }
  return 1;    
}
