//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: DifIndepPar.h,v 1.3 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class Header for |DifIndepPar|
//      Container class for a set of independ parameters
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

#ifndef DifIndepPar_HH
#define DifIndepPar_HH

//#include "BaBar/BaBar.hh"
#include <assert.h>
#include <stdlib.h>

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
using CLHEP::HepVector;
using CLHEP::HepSymMatrix;

#include <iosfwd>
class DifArray;
class DifNumber;

class DifIndepPar {

public:

  DifIndepPar(int n);		// create slots for n parameters
  DifIndepPar(const HepVector& a);// from a vector of doubles
  DifIndepPar(const HepVector& pvec, const HepSymMatrix& pcov);// from a vector and an error matrix
  DifIndepPar(const DifIndepPar& rhs);// new set via copy of existing set

  DifArray difParameters()const; // return (by copy) array of indep parameters
  DifNumber difPar(int i)const; // fetch parameter i

  HepVector& parameter() { return parvec;}
  const HepVector& parameter() const { return parvec;}
  HepSymMatrix& covariance() { return parcov;}
  const HepSymMatrix& covariance() const { return parcov;}
  int nPar()const {return parvec.num_row();}

  //io
  void print(std::ostream& os = std::cout) const;
  void printAll(std::ostream& os = std::cout) const;

private:
  //data
  HepVector parvec;                 //  Vector of parameters
  HepSymMatrix parcov;                 //  Covariance matrix on the parameters

  //private functions
  //  void setDerives();		// make numbers point at this

};

//io
inline std::ostream& operator<<(std::ostream& o,const DifIndepPar& i){ i.print(o); return o; }

#endif
