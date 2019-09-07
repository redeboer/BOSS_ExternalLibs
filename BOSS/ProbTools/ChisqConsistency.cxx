//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: ChisqConsistency.cxx,v 1.1.1.1 2005/04/21 01:17:17 zhangy Exp $
//
// Description:
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Bob Jacobsen, Ed Iskander
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Berkeley Laboratory
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
//#include "BaBar/BaBar.hh"
#include "ProbTools/ChisqConsistency.h"

#include <iostream>
#include <math.h> 
#include <float.h>

#include "ProbTools/NumRecipes.h"

//#include "ErrLogger/ErrLog.hh"

// prototype the cernlib function
extern "C" {
  float chisin_(const float&, const int&);
  double dgausn_(double& arg);
}

ChisqConsistency::ChisqConsistency() :
  _chisq(-1.0), _nDof(0)
{}

ChisqConsistency::ChisqConsistency(double chisq, double nDof) :
   _chisq(chisq), _nDof(nDof)
{
  double z2 = 0.5*_chisq;
  double n2 = 0.5*_nDof;
  
  if (n2<=0 || z2<0) { 
    std::cout << "ErrMsg(warning)" << " Got unphysical values: chisq = " << chisq 
		    << " #dof = " << nDof << std::endl;
    _value=0;
    _likelihood=0;
    setStatus(Consistency::unPhysical);
    return;
  }
  setStatus(Consistency::OK);
  
// given that n2>0 && z2>=0, gammq will NOT abort
  _value = NumRecipes::gammq(n2,z2);
  
  if (_chisq==0) { 
    _likelihood=1; 
  } else {
    double loglike=(n2-1)*log(z2)-z2-NumRecipes::gammln(n2);
    if ( loglike < DBL_MIN_EXP ) {
      _likelihood = 0; 
      setStatus(Consistency::underFlow);
    } else {
      _likelihood = 0.5*exp(loglike);
    }
  }
}



ChisqConsistency::ChisqConsistency(unsigned nDof, double prob) :
  _nDof(nDof)
{
  if(prob >= 0.0|| prob <= 1.0 || nDof < 0)
    _value = prob;
  else {
    std::cout << "ErrMsg(warning)" << " Got unphysical values: prob = " << prob 
		    << " #dof = " << nDof << std::endl;
    _value=0;
    _likelihood=0;
    setStatus(Consistency::unPhysical);
    return;
  }
  setStatus(Consistency::OK);
  if(prob != 1.0){
// use the cernlib function to get chisq.  Note the funny convention on prob!!
    float value = 1.0-float(_value);
    int ndof = nDof;
    if(value < 1.0)
      _chisq = chisin_(value,ndof);
    else
      _chisq = log(double(FLT_MAX));
// use the same algorithm as above to get loglikelihood
    double z2 = 0.5*_chisq;
    double n2 = 0.5*_nDof;
    if (_chisq==0) { 
      _likelihood=1; 
    } else {
      double loglike=(n2-1)*log(z2)-z2-NumRecipes::gammln(n2);
      if ( loglike < DBL_MIN_EXP ) {
        _likelihood = 0; 
        setStatus(Consistency::underFlow);
      } else {
	_likelihood = 0.5*exp(loglike);
      }
    }
  }
}


ChisqConsistency::ChisqConsistency(const ChisqConsistency& other) :
  Consistency(other), _chisq(other._chisq), _nDof(other._nDof)
{}

ChisqConsistency&
ChisqConsistency::operator =(const ChisqConsistency& other) {
  if(this != &other){
    Consistency::operator =(other);
    _chisq = other._chisq;
    _nDof = other._nDof;
  }
  return *this;
}
