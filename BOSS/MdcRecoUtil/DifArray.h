//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifArray.h,v 1.4 2010/03/25 09:55:57 zhangy Exp $
//
// Description:
//	Class Header for |DifArray|
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
//	Zhang Yao(zhangyao@ihep.ac.cn)  Upgrade to CLHEP2.0.4.5 2010-03-25
//
//------------------------------------------------------------------------

#ifndef DIFARRAY_H
#define DIFARRAY_H

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"
#include <assert.h>
#include <stdlib.h>

using namespace CLHEP;

class DifNumber; 

class DifArray {

public:
  DifArray(int n,int npar=0);	// make an array of n elements
  DifArray			// copy from a vector
  (const HepVector& v,int npar=0);
  DifArray(const DifArray& a);	// make a copy
  ~DifArray();			// destroy array
  DifNumber& operator[](int i);	// subscripting (elem=i)
  DifNumber& operator()(int i);	// subscripting (elem=i-1)
  DifNumber fetch(int i)const;	// fetch elem=i
  int nElem()const {return _nElem;} // number of elements
  DifArray& operator=(const DifArray&); // assignment operator
  HepMatrix jacobian()const;	// return matrix of derivatives
  void zero(int npar=0);	// zero this array
  void print()const;	// print this array

private:

  //data
  int _nElem;			// number of elements
  DifNumber* _pointer;		// pointer to data

  //private functions
  void copy(const DifArray& a);	// copy a to *this
  void copy(const HepVector& a,int npar=0); //copy a clhep vector
  

};

#endif
