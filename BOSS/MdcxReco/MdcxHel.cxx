// -------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHel.cxx,v 1.7 2011/12/08 06:52:29 zhangy Exp $
//
// Description:
//	Class Implementation for |MdcxHel|
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	S. Wagner
//
// Copyright Information:
//	Copyright (C) 1996	BEPCII
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
#include "MdcGeom/Constants.h"
#include "MdcGeom/BesAngle.h"
#include <math.h>
#include "MdcxReco/MdcxHel.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/MdcxParameters.h"
using std::cout;
using std::endl;

//constructors

MdcxHel::MdcxHel( ){ }

MdcxHel::MdcxHel
(double D0, double Phi0, double Omega, double Z0, double Tanl,
 double T0, int Code, int Mode, double X, double Y) : 
d0(D0), phi0(Phi0), omega(Omega), z0(Z0), tanl(Tanl), t0(T0),
xref(X), yref(Y), code(Code), mode(Mode), omin(0.000005) { 

  ominfl = (fabs(omega) < omin) ? 0 : 1;
  double m_2pi = 2.0*M_PI;
  if (phi0 > M_PI)  phi0 -= m_2pi;
  if (phi0 < -M_PI) phi0 += m_2pi;
  cphi0 = cos(phi0);
  sphi0 = sin(phi0);
  x0 = X0();
  y0 = Y0();
  xc = Xc();
  yc = Yc();
  decode(code, qd0, qphi0, qomega, qz0, qtanl, qt0, nfree);
  turnflag = 1;  ///FIXME
  //std::cout << "MdcxHel::MdcxHel()  -> (x0, y0) (" << x0 << ", " << y0 << ")" << std::endl;
}//endof MdcxHel

MdcxHel::~MdcxHel( ){ }

//accessors

double MdcxHel::Xc()const {
  if(ominfl) {
    //return (X0() - sphi0/omega);
    return (x0 - sphi0/omega);
  } else {
    return 999999999.9;
  }//(ominfl)
}//endof Xc

double MdcxHel::Yc()const {
  if(ominfl) {
    //return (Y0()+cphi0/omega);
    return (y0 + cphi0/omega);
  } else {
    return 999999999.9;
  }//(ominfl)
}//endof Yc

double MdcxHel::X0()const{
  return (xref - sphi0*d0);
}//endof X0

double MdcxHel::Y0()const{
  return (yref + cphi0*d0);
}//endof Y0

double MdcxHel::Xh(double l)const{
  if(ominfl){
    double phit=phi0+omega*l;
    return (xc+sin(phit)/omega);
  }else{
    return (x0+cphi0*l-0.5*l*l*omega*sphi0);
  }//ominfl
}//endof Xh

double MdcxHel::Yh(double l)const{
  if(ominfl){
    double phit=phi0+omega*l;
    return (yc-cos(phit)/omega);
  }else{
    return (y0+sphi0*l+0.5*l*l*omega*cphi0);
  }//ominfl
}//endof Yh

double MdcxHel::Zh(double l)const{
  return (z0+tanl*l);
}//endof Zh

double MdcxHel::Px(double l) const {
  if(ominfl) {
    double phit = phi0 + omega*l;
    return 0.003*cos(phit)/fabs(omega);  /// pt=0.003*r (0.003 -> q*B)
  } else {
    return 1000.0*cphi0;
  }//ominfl
}//endof Px

double MdcxHel::Py(double l) const {
  if(ominfl) {
    double phit = phi0+omega*l;
    return 0.003*sin(phit)/fabs(omega);
  } else {
    return 1000.0*sphi0;
  }//ominfl
}//endof Py

double MdcxHel::Pz(double l) const {
  if(ominfl) {
    return 0.003*tanl/fabs(omega);
  }
  else{
    return 1000.0*tanl;
  }//ominfl
}//endof Pz

double MdcxHel::Ptot(double l)const{
  if(ominfl) {
    return 0.003*sqrt(1.0+tanl*tanl)/fabs(omega);
  } else {
    return 1000.0*sqrt(1.0+tanl*tanl);
  }//ominfl
}//endof Ptot

double MdcxHel::Lmax() const {
  double lmax = MdcxParameters::maxTrkLength;
  if (ominfl) {
    double rmax = 1.0/fabs(omega);
    double dmax = fabs(d0) + 2.0*rmax;
    if (dmax > MdcxParameters::maxMdcRadius) lmax = M_PI*rmax;  ///FIXME
  }
  return lmax;
}//endof Lmax

//controls
//control fitting mode
void MdcxHel::SetMode(int n) { mode = n; }

void MdcxHel::SetRef(double x, double y) {
  xref = x;
  yref = y;
}
//control free variables
void MdcxHel::SetOmega(int Qomega) {
  nfree = nfree + deltaq(qomega, Qomega);
  code  = code + deltaq(qomega, Qomega)*100;
  qomega = Qomega;
}
void MdcxHel::SetPhi0(int Qphi0) {
  nfree = nfree + deltaq(qphi0, Qphi0);
  code  = code + deltaq(qphi0, Qphi0)*10;
  qphi0 = Qphi0;
}
void MdcxHel::SetD0(int Qd0) {
  nfree = nfree + deltaq(qd0, Qd0);
  code  = code + deltaq(qd0, Qd0);
  qd0 = Qd0;
}
void MdcxHel::SetTanl(int Qtanl) {
  nfree = nfree + deltaq(qtanl, Qtanl);
  code  = code + deltaq(qtanl, Qtanl)*10000;
  qtanl = Qtanl;
}
void MdcxHel::SetZ0(int Qz0) {
  nfree = nfree + deltaq(qz0, Qz0);
  code = code + deltaq(qz0, Qz0)*1000;
  qz0 = Qz0;
}
void MdcxHel::SetT0(int Qt0) {
  nfree = nfree + deltaq(qt0, Qt0);
  code = code + deltaq(qt0, Qt0)*100000;
  qt0 = Qt0;
}

//operators
MdcxHel& MdcxHel::operator=(const MdcxHel& rhs) {
  copy(rhs);
  return *this;
}

//decode free parameter code
void MdcxHel::decode(const int code,int& i1,int& i2,
    int& i3,int& i4,int& i5,int& i6,int& n)
{  ///FIXME use bit code ?
  int temp = code;
  temp=temp/1000000; temp=code-1000000*temp;
  i6=temp/100000;    temp=temp-100000*i6;
  i5=temp/10000;     temp=temp-10000*i5;
  i4=temp/1000;      temp=temp-1000*i4;
  i3=temp/100;       temp=temp-100*i3;
  i2=temp/10;        i1=temp-10*i2;
  n = 0;
  if(i6 == 1) n++; else i6 = 0;
  if(i5 == 1) n++; else i5 = 0;
  if(i4 == 1) n++; else i4 = 0;
  if(i3 == 1) n++; else i3 = 0;
  if(i2 == 1) n++; else i2 = 0;
  if(i1 == 1) n++; else i1 = 0;
}//endof decode

//copy |MdcxHel| to |MdcxHel|
void MdcxHel::copy(const MdcxHel& rhs)
{
  //FIXME
  omega=rhs.Omega(); phi0=rhs.Phi0(); d0=rhs.D0(); t0=rhs.T0();
  tanl=rhs.Tanl(); z0=rhs.Z0();
  cphi0=rhs.CosPhi0(); sphi0=rhs.SinPhi0();
  x0=rhs.X0(); y0=rhs.Y0(); xc=rhs.Xc(); yc=rhs.Yc();
  xref=rhs.Xref(); yref=rhs.Yref();
  qomega=rhs.Qomega(); qphi0=rhs.Qphi0(); qd0=rhs.Qd0(); qt0=rhs.Qt0();
  qtanl=rhs.Qtanl(); qz0=rhs.Qz0();
  mode=rhs.Mode(); nfree=rhs.Nfree();
  code=rhs.Code(); ominfl=rhs.Ominfl(); omin=rhs.Omin();
  turnflag=rhs.GetTurnFlag();
}//endof copy

double MdcxHel::Doca(const MdcxHit& h) { 
  //std::cout<< __FILE__ << "   " << __LINE__ << "  hit("<<h.Layer()<<","<<h.WireNo()<<")";
  return Doca( h.wx(), h.wy(), h.wz(), h.x(), h.y() ); 
}

double MdcxHel::Doca( double wx, double wy, double wz,
    double xi, double yi, double zi )
{
  double m_2pi = 2.0*M_PI;
  // describe wire
  //cout << " In Doca, xi = " << xi << " yi = " << yi << " zi = " << zi <<endl;
  Hep3Vector ivec(xi, yi, zi); 
  wvec = Hep3Vector(wx, wy, wz);
  //cout << " In Doca, wx = " << wx << " wy = " << wy << " wz = " << wz <<endl;
  //  calculate len to doca
  double zd, xd = xi, yd = yi;
  // cout << " In Doca, start xd = " << xd << " yd = " << yd << endl;
  double lnew,t1,t2,dphi,dlen=1000.0;
  len = 0.0;
  int itry = 2;
  // int segflg=0; if ((code==111)&&(z0==0.0)&&(tanl==0.0))segflg=1;
  // int superseg=0; if ((code==11111)&&(xref!=0.0)&&(yref!=0.0))superseg=1;
  double circut, circum = 10000.; 
  if (ominfl) circum = m_2pi/fabs(omega);
  circut = 0.50 * circum;
  while (itry) {
    if (ominfl) {
      ///calc the phi of point(i)
      t1 = -xc + xd; t2 = yc - yd; phi = atan2(t1, t2); 
      if (omega < 0.0) phi += M_PI;
      if (phi > M_PI)  phi -= m_2pi;
      dphi = phi - phi0;
      if (omega < 0.0){
	if (dphi > 0.0)    dphi -= m_2pi;
	if (dphi < -m_2pi) dphi += m_2pi;
      }else{
	if (dphi < 0.0)   dphi += m_2pi;
	if (dphi > m_2pi) dphi -= m_2pi;
      }
      lnew = dphi/omega; 
      //   if ((lnew>circut)&&(segflg))lnew-=circum; 
      //   if ((lnew>circut)&&(superseg))lnew-=circum; 
      if ((lnew>circut)&&(turnflag)) lnew -= circum;  //FIXME attention
      
      zh = Zh(lnew); 
      xd=xi+(zh-zi)*wx/wz; yd=yi+(zh-zi)*wy/wz; zd=zh;
      //   cout << " In Doca, xd = " << xd << " yd = " << yd << " zh = " << zh;
      //   cout << " lnew = " << lnew << endl;
      dlen=fabs(lnew-len); len=lnew; 
      //   if (segflg)break; 
      //std::cout<< __FILE__ << __LINE__<<" Doca()  dlen " << dlen<< " zh  "<<zh<<" >?"
	//<<MdcxParameters::maxMdcZLen<<std::endl;
      if (fabs(zh) > MdcxParameters::maxMdcZLen) break;  //FIXME attention
      if ( (0.0==wx) && (0.0==wy) )break; if (dlen < 0.000001)break; itry--;
    } else {
      len = (xi-xref)*cphi0 + (yi-yref)*sphi0;
      zh  = z0 + tanl*len;
      phi = phi0;
      break;
    }
  }
  //  Hep3Vector Dvec(xd,yd,zd);
  xh = Xh(len); yh = Yh(len); 
  Hep3Vector hvec(xh, yh, zh);
//   cout << " In Doca, xh = " << xh << " yh = " << yh << " zh = " << zh << " len=" << len << " om " << omega << endl;
  double lamb = atan(tanl);
  cosl = cos(lamb); sinl = sin(lamb);
  tx = cosl*cos(phi); ty = cosl*sin(phi); tz = sinl; 
  tvec = Hep3Vector(tx, ty, tz); 
  Hep3Vector vvec = wvec.cross(tvec); 
  vhat = vvec.unit(); vx = vhat.x(); vy = vhat.y(); vz = vhat.z();
  // cout << " In Doca, vx = " << vx << " vy = " << vy << " vz = " << vz << endl;
  dvec = ivec - hvec;
  double doca = dvec*vhat;
  // cout << " doca = " << doca << endl;
  double f1 = dvec*tvec; double f2 = wvec*tvec; double f3 = dvec*wvec;
  f0 = (f1 - f2*f3) / (1.0 - f2*f2); 
  samb = (doca > 0.0) ? -1 : +1;
  double wirephi = atan2(yd, xd);
  eang = BesAngle(phi-wirephi);
  wamb = (fabs(eang) < Constants::pi/2) ? samb : -samb;
  //std::cout<< __FILE__ << __LINE__<<" Doca()  dlen " << dlen<< " zh  "<<zh<<" >?"
	//<<MdcxParameters::maxMdcZLen<<std::endl;
  if (fabs(zh) > MdcxParameters::maxMdcZLen) doca = 1000.0; ///FIXME
  //if(doca == 1000.0)    cout << " In Doca, zh = " << zh << " len=" << len << " om " << omega <<" "<< ominfl<< 
   //" z0 " << z0 << "tanl " << tanl <<endl;
  //cout << "  doca = " << doca << endl;
  return doca;
}//endof Doca

std::vector<float> MdcxHel::derivatives(const MdcxHit& hit) 
{
  double doca = Doca(hit);
  std::vector<float> temp(nfree+1);
  temp[0] = doca;
  double fac = 1.0;
  if((mode==0) && (doca<0.0)) fac = -fac;
  if(mode == 0) temp[0] = fabs(temp[0]);

  int bump = 0;
  if (qd0)   temp[++bump] = (-vx*sphi0 + vy*cphi0) * fac;
  if (qphi0) {
    //double dddp0=-(yh-y0)*vx+(xh-x0)*vy;
    double dddp0 = -(yh-y0+f0*ty)*vx + (xh-x0+f0*tx)*vy;
    dddp0 *= (1.0 + d0*omega);
    temp[++bump] = dddp0*fac;
  }
  if (qomega) {
    double dddom;
    if (ominfl) {
      dddom = ((len*cos(phi)-xh+x0)*vx + (len*sin(phi)-yh+y0)*vy)/omega;
      dddom += f0*len*cosl*(-sin(phi)*vx+cos(phi)*vy);
    } else {
      dddom = 0.5*len*len*(-vx*sphi0+vy*cphi0);
    }
    temp[++bump] = dddom * fac;
  }
  if (qz0)   temp[++bump] = vz * fac;
  if (qtanl) temp[++bump] = (vz *len) * fac;
  if (qt0)   temp[++bump] = -hit.v();
  return temp;
}//endof derivatives

void MdcxHel::print() const {
  cout << "MdcxHel(";
  cout << d0<<",";
  cout << phi0<<",";
  cout << omega<<",";
  cout << z0<<",";
  cout << tanl<<")"<<endl; 
  cout << " t0 = " << t0 ; 
  cout << " nfree = " << nfree ;
  cout << " (x0,y0) " << x0<<","<<y0;
  cout << " (xc,yc) " << xc<<","<<yc;
  cout << " (xref,yref) " << xref<<","<<yref;
  cout << " code = " << code; 
  cout << " mode = " << mode;
  cout << " ominfl = " << ominfl;
  cout << "  " << endl;
}//endof print

void MdcxHel::flip() {
  double m_2pi = 2.0*M_PI;
  if (ominfl) {
    if ( (fabs(d0) + 2.0/fabs(omega)) > 80.0 ) return;
    double lturn = m_2pi/fabs(omega);
    double zturn = Zh(lturn);
    //  cout << "z0 " << z0 << " zturn " << zturn << endl;
    if (fabs(zturn) < fabs(z0)) {
      z0 = zturn;
      tanl = -tanl;
      omega = -omega;
      d0 = -d0; 
      phi0 = phi0 - M_PI;
      if (phi0 < -M_PI) phi0 += m_2pi;
      cphi0 = cos(phi0);
      sphi0 = sin(phi0);
      x0 = X0();
      y0 = Y0();
    } 
  }
}//endof flip

