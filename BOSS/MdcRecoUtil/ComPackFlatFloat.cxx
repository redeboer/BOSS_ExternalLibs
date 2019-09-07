//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackFlatFloat.cxx,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      Class ComPackFlatFloat - see .hh file for details
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      D.E.Azzopardi               Originator. 
//
// Copyright Information:
//      Copyright (C) 1999  DESVA Research & Design, for Hedgehog Concepts
// 
// History:
//	Migration for BESIII MDC
//
// Additional information:
//      This class, and related classes, are in development right now...
//      so BEWARE!!!!
//   
// Quote:
//      "I have never let my schooling get in the way of my education."
//                              -Mark Twain 
//--------------------------------------------------------------------------
//#include "BaBar/BaBar.hh"

//
// C includes
//
#include <stddef.h> 
#include <math.h>
#include <stdlib.h>

//
// C++ includes
//
#include <iostream>

//
// Bass class includes
//
#include "MdcRecoUtil/ComPackBase.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
//#include "ErrLogger/ErrLog.hh"

//
// This class
//
#include "MdcRecoUtil/ComPackFlatFloat.h"
using std::cout;
using std::endl;

ComPackFlatFloat::ComPackFlatFloat(const double val_one, const double val_two, size_t bits)
{
  if (val_one>val_two) {
    _minVal=val_two; 
    _maxVal=val_one;
  } else {
    _minVal=val_one; 
    _maxVal=val_two;
  }

// this is a stupid test.  I wanted to packa a number in nano-seconds,
// and it refused.  I've removed the test dnb 11/17/00
//  const double epsilon = 0.0001;
  const double epsilon = 0.0;
  if (( _maxVal - _minVal )<=epsilon) {
    cout <<" ErrMsg(fatal) "  << "Error : Range is zero!" << endl;
    abort();
  }
  _valRange = _maxVal - _minVal;

  if (bits > _maxlongbits ) {
    bits = _maxlongbits;
    cout << " ErrMsg(fatal) "<< "Warning : Number of bits truncated! "
    << "Reason  : Number of bits too large for " << _maxlongbits << " bit packing operations! "
    << "This is probably caused by a serious typo somewhere!" << endl;
    abort();
  }
  _bitRange = bits;
  _bitMask = (1<<_bitRange) - 1;
  if ( 0 == _bitMask) { // check for wrap around
    _bitMask--;
  }
// preset packing.
  _pacfac = (1<<_bitRange)/_valRange;
  _upacfac = 1.0/_pacfac;
}

ComPackFlatFloat::~ComPackFlatFloat(){}

ComPackBase<double>::StatusCode 
ComPackFlatFloat::pack(const double theval, d_ULong & packedval) const
{
  StatusCode retval(TAG_OK);
  double dpack = (theval-_minVal)*_pacfac;
  packedval = (d_ULong)dpack;
  if (dpack>_bitMask) { 
    packedval = _bitMask;
    retval = TAG_VAL_ROUND_DOWN;
  } else if (dpack<0) {
    packedval = 0; // clamp
    retval = TAG_VAL_ROUND_UP;
  }
  return retval;
}

ComPackBase<double>::StatusCode
ComPackFlatFloat::unpack(const d_ULong val, double & unpackedval ) const
{
  unpackedval = ((val&_bitMask)+0.5)*_upacfac + _minVal;
  return TAG_OK;
}

void
ComPackFlatFloat::testMe (size_t numsteps, double & toterror)
{
  if ( 0 == numsteps) return;
  toterror = 0.;
  const double incstep = double (_valRange) / double (numsteps);
  for ( double i = _minVal; i<= _maxVal; i+=incstep)
    {
      d_ULong tagVal;
      pack ( i, tagVal );
      double unTagVal;
      unpack ( tagVal, unTagVal );
      cout << i << " is converted to :" << tagVal << ". Upon unpacking :" << unTagVal << endl;
      toterror += fabs ( i-unTagVal );
    }
}

