//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: BesPointErr.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      Class BbrPointErr
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Forest Rouse            February 1996
//      Victoria Novotny        August   1996
//
// Copyright Information:
//      Copyright (C) 1996      U.C. Davis
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
// File BbrPointErr.cc
// Source file for class BbrPointErr
//
// For advice, input, or any questions then please contact either
// Bob Jacobsen <Bob_Jacobsen@lbl.gov> or
// Forest Rouse <rouse@ucdhep.ucdavis.edu>
//
// =====================================================================
// Name           Change description
// Date
// Version
// =====================================================================
//#include "BaBar/BaBar.h"

#include <stdio.h>
#include <float.h>
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifArray.h"
#include "MdcRecoUtil/BesPointErr.h"
using std::istream;
using std::ostream;

BesPointErr operator + (const BesPointErr& v, const BesVectorErr& w){
    BesPointErr pe(HepPoint3D(v.x()+w.x(),v.y()+w.y(),v.z()+w.z()),
  	   (v.covMatrix()+w.covMatrix()));
    return pe;
  }

BesPointErr operator + (const BesVectorErr& w, const BesPointErr& v){
    BesPointErr pe(HepPoint3D(v.x()+w.x(),v.y()+w.y(),v.z()+w.z()), 
                   (v.covMatrix()+w.covMatrix()));
    return pe;
}

BesPointErr operator - (const BesPointErr& v, const BesVectorErr& w){
    BesPointErr pe(HepPoint3D(v.x()-w.x(),v.y()-w.y(),v.z()-w.z()), 
                   (v.covMatrix()+w.covMatrix()));
    return pe;
}

BesVectorErr operator - (const BesPointErr& v, const BesPointErr& w){
    BesVectorErr ve(Hep3Vector(v.x()-w.x(),v.y()-w.y(),v.z()-w.z()),
		    (v.covMatrix()+w.covMatrix()));
    return ve;
}

ostream & operator<<(ostream & stream, const BesPointErr & verr) {
  stream << (const HepPoint3D&)verr
	 << ", " << verr.covMatrix();
  
  return stream;
}

istream & operator>>(istream & stream, BesPointErr & verr) {
  BesError mat(verr.SIZE);
  stream >> (HepPoint3D&)verr >> mat;
  verr.setCovMatrix(mat);
  
  return stream;
}

BesError BesPointErr::covRTPMatrix() const{
  // protect against 0's
  double xv = x()==0 ?  FLT_MIN : x();
  double yv = y()==0 ?  FLT_MIN : y();
  double zv = z()==0 ?  FLT_MIN : z();
  DifNumber xDF(xv,X+1,3), yDF(yv,Y+1,3), zDF(zv,Z+1,3);
  DifArray pars(3,NUM_PCOORDINATES);
  pars[Rho]   =  sqrt(xDF*xDF + yDF*yDF + zDF*zDF);
  pars[Phi]   = atan2(yDF,xDF);
  pars[Theta] = acos(zDF/pars[Rho]);
  return covMatrix().similarity(pars.jacobian());
}

BesError BesPointErr::covRZPMatrix() const{
  // protect against 0's
  double xv = x()==0 ?  FLT_MIN : x();
  double yv = y()==0 ?  FLT_MIN : y();
  double zv = z()==0 ?  FLT_MIN : z();
  DifNumber xDF(xv,X+1,3), yDF(yv,Y+1,3), zDF(zv,Z+1,3);
  DifArray pars(3,NUM_CCOORDINATES);
  pars[C_Rho]   =  sqrt(xDF*xDF + yDF*yDF );
  pars[C_Phi]   = atan2(yDF,xDF);
  pars[C_Zeta] =  zDF;
  return covMatrix().similarity(pars.jacobian());
}

