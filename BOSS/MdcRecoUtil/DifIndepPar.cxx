//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifIndepPar.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Implementation for |DifIndepPar| 
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
//#include "BaBar/BaBar.h"

#include "MdcRecoUtil/DifIndepPar.h"
#include "MdcRecoUtil/DifNumber.h"
#include "MdcRecoUtil/DifArray.h"
using std::endl;
using std::ostream;

DifIndepPar::DifIndepPar(int ndim):
  parvec(ndim,0), 
  parcov(ndim,1)
{}

DifIndepPar::DifIndepPar(const HepVector& a):
  parvec(a),
  parcov(nPar(),1)
{}

DifIndepPar::DifIndepPar(const DifIndepPar& rhs):
  parvec(rhs.parameter()),
  parcov(rhs.covariance())
{}

DifArray DifIndepPar::difParameters()const{
  DifArray pars(nPar(),nPar());
  for(int i=1; i<=nPar(); i++){
    pars(i)=difPar(i);
  }
  return pars;
}

DifIndepPar::DifIndepPar(const HepVector& pvec, const HepSymMatrix& pcov) :
  parvec(pvec), parcov(pcov) {
}
//void DifIndepPar::setDerives(){
//  for(int i=0; i<nPar(); i++){
//    _parameters[i].zeroDerivatives();
//    _parameters[i].setDerivative(i+1,1.0);
//    _parameters[i].setIndepPar(this);  
//  }
//}

DifNumber DifIndepPar::difPar(int i)const{
  return DifNumber(parvec(i),i,this);
}

//--------------------------------------
void 
DifIndepPar::print(ostream& os) const {
//--------------------------------------
  os << "DifIndepPar Parameter Vector = " << parvec;
}

//--------------------------------------
void 
DifIndepPar::printAll(ostream& os) const {
//--------------------------------------
  os << "DifIndepPar Parameter Vector = " << parvec << endl;
  os << "DifIndepPar Covariance Matrix = " << parcov << endl;
}
