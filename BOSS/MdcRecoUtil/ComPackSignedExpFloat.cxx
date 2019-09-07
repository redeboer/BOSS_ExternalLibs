//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackSignedExpFloat.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
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
#include "MdcRecoUtil/ComPackSignedExpFloat.h"
#include <assert.h>
#include <math.h>
using std::endl;
using std::ostream;

ComPackSignedExpFloat::ComPackSignedExpFloat(unsigned nbits,
					     unsigned maxexponent,
					     double maxval,
					     bool invert,
                                             bool center) :
  ComPackBase<double>(-maxval,maxval,nbits),
  _packer(nbits-1,maxexponent,invert?maxval:0.0,
	  invert?0.0:maxval, center),
  _signbit(1<<nbits-1),_signmask(~(1<<nbits-1))
{
  assert(maxval>0.0);
}

ComPackSignedExpFloat::~ComPackSignedExpFloat()
{;}

ComPackBase<double>::StatusCode
ComPackSignedExpFloat::pack (const double value, d_ULong & packdata) const {
// first, pack the absolute value
  ComPackBase<double>::StatusCode retval =
    _packer.pack(fabs(value),packdata);
  if(retval != TAG_BAD){
    if(value>0)
      packdata &= _signmask;
    else
      packdata |= _signbit;
  }
  return retval;
}

ComPackBase<double>::StatusCode 
ComPackSignedExpFloat::unpack (const d_ULong packdata, double & value) const {
  ComPackBase<double>::StatusCode retval =
    _packer.unpack(packdata,value);
  if (value < 0)
    value = 0;
  else if(packdata&_signbit)
    value *= -1.0;
  return retval;
}

void
ComPackSignedExpFloat::print(ostream& os) const {
  os << "Signed exponential packer using exponential packer as follows:"<<endl;
  _packer.print(os);
}



    


