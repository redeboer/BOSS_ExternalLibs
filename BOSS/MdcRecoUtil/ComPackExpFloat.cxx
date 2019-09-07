//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackExpFloat.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      class ComPackExpFloat
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Dave Brown               10/12/00
//
// Copyright Information:
//      Copyright (C) 2000 LBNL
// 
// History:
//	Migration for BESIII MDC
//
//
//--------------------------------------------------------------------------

//#include "BaBar/BaBar.hh"
#include "MdcRecoUtil/ComPackExpFloat.h"
//#include "ErrLogger/ErrLog.hh"
#include <math.h>
#include <iostream>
using std::endl;
using std::ostream;

ComPackExpFloat::ComPackExpFloat(unsigned nbits,
				 unsigned maxexponent,
				 double start,
				 double stop,
				 bool center ) :
  ComPackBase<double>(start,stop,nbits),
  _maxexp(maxexponent) {
  if(_maxexp > 62 ) {
    std::cout<<"ErrMsg(fatal) "<< "Can't pack exponents larger than 62" << endl;
    abort();
  }
  static double invln2(1.0/log(2.0));
  long long one(1);
// compute how many bits used by exponent
  _mansft = _maxexp>0?(unsigned)(log(double(_maxexp))*invln2+1.01):0;
  _expmsk = (1<<_mansft)-1;
  _maxman = (1<<(_bitRange-_mansft));
  _manmsk = _maxman-1;
  double invmaxman = 1.0/_maxman;
  long long maxnorm = (one<<(maxexponent+1))-1;
// correct the range if we're centering the bins
  if(center) {
    long long norm = one<<maxexponent;
    double alpha = 1.0/((double)maxnorm*_maxman*2.0 - (norm + 1.0));
    _minVal -= _valRange*alpha;
    _maxVal += _valRange*norm*alpha;
    _valRange = _maxVal - _minVal;
//    cout << "alpha = " << alpha << " norm = " << norm << endl;
  }
// finish the calculations
  _invrange = maxnorm/_valRange;
  double invmaxnorm = _valRange/maxnorm;
  double manoff = (0.5 * invmaxman+1.0) * invmaxnorm;
  double valoff = _minVal -  invmaxnorm;
  double manfac = invmaxnorm * invmaxman;
// pre-compute some factors and store them in an array
  _expfac = new double[_maxexp+1];
  _expoff = new double[_maxexp+1];
  for(unsigned iexp=0;iexp<=_maxexp;iexp++){
    double expf = (one<<iexp);
    _expoff[iexp] = valoff + manoff*expf;
    _expfac[iexp] = manfac*expf;
  }
}

ComPackExpFloat::~ComPackExpFloat()
{
  delete [] _expfac;
  delete [] _expoff;
}

ComPackBase<double>::StatusCode
ComPackExpFloat::pack (const double value, d_ULong & packdata) const {
  static double invln2(1.0/log(2.0));
  static long long one(1);
  ComPackBase<double>::StatusCode retval(TAG_OK);
// first, figure out the exponent
  double renorm = 1.0+(value-_minVal)*_invrange;
  if(renorm<1.0){
    renorm=1.0;
    retval = TAG_VAL_ROUND_UP;
  }
  unsigned iexp = unsigned(log(renorm)*invln2);
  unsigned iman(0);
  if(iexp<=_maxexp){
    long long expon = one<<iexp;
    iman = (unsigned)( _maxman*(renorm/expon - 1.0) );
// deal with floating-point rounding, which could give a 'non-physical' result
    if(iman==_maxman){
      if(iexp==_maxexp)
	iman=_maxman-1;
      else {
	iman=0;
	iexp++;
      }
    }
  } else {
    iexp=_maxexp;
    iman = _maxman-1;
    retval = TAG_VAL_ROUND_DOWN;
  }
  packdata = (iexp&_expmsk) | (iman&_manmsk)<<_mansft;
  return retval;
}

ComPackBase<double>::StatusCode 
ComPackExpFloat::unpack (const d_ULong packdata, double & value) const {
  size_t iman = (packdata>>_mansft)&_manmsk;
  size_t iexp = packdata&_expmsk;
  value = _expoff[iexp] + iman * _expfac[iexp];
  return TAG_OK;
}

void
ComPackExpFloat::print(ostream& os) const {
  os << "Exponential packer for range " << _minVal << " to " << _maxVal << endl;
  os << "Maximum exponent = " << _maxexp << endl;
  os << "Maximum mantissa = " << _maxman << endl;
  os << "Mantissa storage shift, mask = " << _mansft << " , " << _manmsk << endl;
}

