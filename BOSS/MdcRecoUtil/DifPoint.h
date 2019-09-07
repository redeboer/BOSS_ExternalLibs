//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifPoint.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |DifPoint|
//      What i do ?
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A. Snyder
//
// Copyright Information:
//	Copyright (C) 1996	SLAC
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef DifPoint_H
#define DifPoint_H

#include <assert.h>
#include "MdcRecoUtil/DifVector.h"
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif


class DifPoint:public DifVector {

public:
  DifPoint(const DifPoint& p)
    :DifVector(p) {} 
  DifPoint()
    :DifVector() {}
  DifPoint(const HepPoint3D& p);

  DifPoint(const DifNumber& X,const DifNumber& Y,const DifNumber& Z)
    :DifVector(X,Y,Z) {}
  DifPoint(double X,double Y,double Z)
    :DifVector(X,Y,Z) {}

  //operators - sensible only
  inline DifPoint& operator=(const DifPoint& p);
  inline DifPoint& operator+=(const DifVector& v);
  inline DifPoint& operator-=(const DifVector& v);
  inline DifPoint& operator*=(const DifNumber& n);
  inline DifPoint& operator/=(const DifNumber& n);

  //binary operators

  inline friend DifPoint operator+
  (const DifPoint& p,const DifVector& v);
  inline friend DifPoint operator+
  (const DifVector& v,const DifPoint& p);
  inline friend DifPoint operator-
  (const DifPoint& p,const DifVector& v);
  inline friend DifVector operator-
  (const DifPoint& a,const DifPoint& b);

  //change of scale is ok (e.g. m->cm)
  inline friend DifPoint operator*
  (const DifPoint& p,const DifNumber& n);
  inline friend DifPoint operator*
  (const DifNumber& n,const DifPoint& p);
  inline friend DifPoint operator/
  (const DifPoint& v,const DifNumber& n);


private:
  //disable senseless operations
  DifPoint& operator=(const DifVector& v); 
  DifPoint& operator+=(const DifPoint& p);
  DifPoint& operator-=(const DifPoint& p);

public:
  //bes interface routines
  HepPoint3D hepPoint()const;
};

#include "MdcRecoUtil/DifPoint.icc"


#endif
 
