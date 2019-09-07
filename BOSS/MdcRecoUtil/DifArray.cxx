//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifArray.cxx,v 1.3 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Implementation for |DifArray| 
//      What do i do ?
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

#include "MdcRecoUtil/DifArray.h"
#include "MdcRecoUtil/DifNumber.h"
#include <iostream>
using std::endl;
//CHAGNE using std::ostream;
using std::cout;

DifArray::DifArray(int n,int npar):
  _nElem(n),
  _pointer(new DifNumber[n])
{zero(npar);}

DifArray::DifArray(const HepVector & a,int npar):
  _nElem(a.num_row()),
  _pointer(new DifNumber[nElem()])
{copy(a,npar);}


DifArray::DifArray(const DifArray& a):
  _nElem(a.nElem()),
  _pointer(new DifNumber[nElem()])
{copy(a);}

DifArray::~DifArray() {delete[] _pointer;}

DifNumber& DifArray::operator[](int i){
  assert(i>=0);
  assert(i<nElem());
  return _pointer[i];
}

DifNumber& DifArray::operator()(int i){
  i=i-1;
  assert(i>=0);
  assert(i<nElem());
  return _pointer[i];
}

DifNumber DifArray::fetch(int i)const{
  i=i-1;
  assert(i>=0);
  assert(i<nElem());
  return _pointer[i];
}

DifArray& DifArray::operator=(const DifArray& rhs){
  copy(rhs);
  return *this;
}

HepMatrix DifArray::jacobian()const {
  int npar=_pointer[0].nPar();
  HepMatrix temp(nElem(),npar,0);
  for(int i=1; i<=nElem(); i++){
    for(int j=1; j<=npar; j++) {
      temp(i,j)=_pointer[i-1].derivative(j);
    }
  } //(int i=1; i<=nElem(); i++) 
  return temp;
}

void DifArray::copy(const HepVector& a,int npar){
  assert(nElem()==a.num_row());
  for(int i=0; i<nElem(); i++) 
    {
      _pointer[i].setNumber(a(i+1));
      _pointer[i].setNPar(npar);
    } 
}

void DifArray::copy(const DifArray& a){
  assert(nElem()==a.nElem());
  for(int i=0; i<nElem(); i++) {_pointer[i]=a._pointer[i];}
}

void DifArray::zero(int npar){
  for(int i=0; i<nElem(); i++) {
    _pointer[i]=0.0;
    _pointer[i].setNPar(npar);
  }
}

void DifArray::print(/*ostream& o*/)const{
  cout << "nElem=" << nElem() << endl;
  for(int i=1; i<=nElem(); i++){
    //SKIP cout << "element(" << i << ")=" << _pointer[i-1];
  }

}
