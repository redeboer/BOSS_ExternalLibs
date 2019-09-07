//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcxHel.h,v 1.7 2011/12/08 06:52:29 zhangy Exp $
//
// Description:
//	Class Header for |MdcxHel| and |MdcxFittedHel| - helix fitting classes
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
#ifndef _MDCXHEL_
#define _MDCXHEL_

//MdcxHel & MdcxFittedHel classes ...
#include <iostream>
#include <fstream>
#include "CLHEP/Alist/AList.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "MdcxReco/MdcxHit.h"
#include "MdcxReco/MdcxParameters.h"
#include <math.h>
#include <vector>
 
using CLHEP::Hep3Vector;
//class MdcxHit;//yzhang del

class MdcxHel {

public:

  //constructors
  
  MdcxHel( );
  
  MdcxHel(double D0, double Phi0, double Omega, double Z0=0.0, double Tanl=0.0,
  double T0=0.0, int Code=11111, int Mode=0, double X=0.0, double Y=0.0);
  
  //destructor
  
  virtual ~MdcxHel( );
  
  //access
  inline double D0()const{return d0;}
  inline double Phi0()const{return phi0;}
  inline double Omega()const{return omega;}
  inline double Z0()const{return z0;}
  inline double Tanl()const{return tanl;}
  inline double X()const{return xref;}
  inline double Xref()const{return xref;}
  inline double Y()const{return yref;}
  inline double Yref()const{return yref;}
  inline double T0()const{return t0;}
  inline double CosPhi0()const{return cphi0;}
  inline double SinPhi0()const{return sphi0;}
  inline double Doca_Len()const{return len;}
  inline double Doca_FLen()const{return len*sqrt(1.0+tanl*tanl);}
  inline double Doca_Tof()const{return len*sqrt(1.0+tanl*tanl)/MdcxParameters::c;}
  inline double Doca_Zh()const{return zh;}
  inline int Doca_Samb()const{return samb;}
  inline int Doca_Wamb()const{return wamb;}
  inline double Doca_Eang()const{return eang;}
  inline double Omin()const{return omin;}
  inline int  Mode()const{return mode;}
  inline int  Code()const{return code;}
  inline int  Nfree()const{return nfree;}
  inline int  Ominfl()const{return ominfl;}
  inline int  Qd0()const{return qd0;}
  inline int  Qphi0()const{return qphi0;}
  inline int  Qomega()const{return qomega;}
  inline int  Qz0()const{return qz0;}
  inline int  Qtanl()const{return qtanl;}
  inline int  Qt0()const{return qt0;}
  double Xc()const;
  double Yc()const;
  double X0()const;
  double Y0()const;
  double Xh(double l)const;
  double Yh(double l)const;
  double Zh(double l)const;
  double Px(double l=0.0)const;
  double Py(double l=0.0)const;
  double Pz(double l=0.0)const;
  double Ptot(double l=0.0)const;
  double Lmax()const;
  
  //workers
  double Doca(double WX,double WY,double WZ,double X,double Y,double Z=0.0);
  double Doca(const MdcxHit& h);
  //double Doca(const MdcxHit& h) { return Doca( h.wx(), h.wy(), h.wz(), h.x(), h.y() ); }
  void print()const; //print this helix
  void flip(); //flip this helix if moving towards origin
  
  //set values
  std::vector<float> derivatives(const MdcxHit& h);
  void SetRef(double x, double y);
  
  //controls
  void SetMode(int n);
  void SetD0(int n);
  void SetPhi0(int n);
  void SetOmega(int n);
  void SetZ0(int n);
  void SetTanl(int n);
  void SetT0(int n);
  inline void SetTurnFlag(const int &i) {turnflag=i;}
  inline int  GetTurnFlag()const {return turnflag;}
  
  //operators
  MdcxHel& operator=(const MdcxHel&);
  
  protected:
  
  double d0;     // signed distance of closest approach (of helix to z axis)
  double phi0;   // direction
  double omega;  // signed 1/r
  double z0;     // z value (at DOCA of helix to z axis)
  double tanl;   // tangent of dip angle
  double t0;     // zero of time
  double xref;   // x of ref pt of fit
  double yref;   // y of ref pt of fit
  double cphi0;  // cos(phi0)
  double sphi0;  // sin(phi0)
  double x0;
  double y0;
  double xc;
  double yc;
  int  code;   // derivative control 
               // specify which derivatives to calculate
               // digits -> t0 tanl z0 omega phi0 d0
               // 11111  is standard 5 parameter helix fit
               // 111111 includes t0
               // 11     is a 2D straight line fit (if omega=0.0)
               // 111    is the Dahl-Lynch 2D circle fit
               // 11011  is a 3D straight line fit (if omega=0.0)
  int  mode;   // 0=>unsign drift time; mode=1=>signed drift time
               // free in fit if q-variable non-zero (true)
  int  qd0;
  int  qphi0;
  int  qomega;
  int  qz0;
  int  qtanl;
  int  qt0;
  int  nfree;
  int  ominfl;
  int  turnflag;
  
  //static control parameters
    double omin;	// omega straight line <-> helix cut-off
  
  //invisible data (calculated by Doca; used by other fcns which first call
  //Doca or assume Doca is already called)
  double len,phi,xh,yh,zh,vx,vy,vz,cosl,sinl,f0,tx,ty,tz;
  Hep3Vector wvec,tvec,vhat,dvec;
  int samb,wamb; double eang;
  
  //utility functions
  double CalcPhi(double xf,double yf,double xl,double yl) const{
   double phit=atan2(yl-yf,xl-xf); return phit<0?phit+2*M_PI:phit;
  }//endof CalcPhi
  int deltaq(int i,int j) const { return i==j?0:i==0?1:-1; } //integer comparisons 
  void decode(const int i, int& i1, int& i2,
                           int& i3, int& i4, int& i5, int& i6,int& n);
  void copy(const MdcxHel& hel);
  
};
//endof MdcxHel

#endif
