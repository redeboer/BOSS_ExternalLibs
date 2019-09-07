//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackSignedExpFloat.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      class ComPackSignedExpFloat; Uses ComPackExpFloat to store the
//      magnitude of a signed quantity, then adds in the sign.
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Dave Brown         10/12/00
//
// Copyright Information:
//      Copyright (C) 2000 LBNL
// 
// History:
//	Migration for BESIII MDC
//
//
//--------------------------------------------------------------------------

#ifndef COMPACKSIGNEDEXPFLOAT_HH
#define COMPACKSIGNEDEXPFLOAT_HH

#include "MdcRecoUtil/ComPackExpFloat.h"
#include <iostream>

class ComPackSignedExpFloat:public ComPackBase<double> {

public:
// See ComPackExpFloat for an explanation.  Note that in this class, the number
// of bits should include 1 for the sign.  The invert flag specifies that the best
// resolution should occur at the +-maxval end of the range instead of around 0. 
  ComPackSignedExpFloat(unsigned nbits,
			unsigned maxexponent,
			double maxval,
			bool invert=false,
                        bool center=false );
  virtual ~ComPackSignedExpFloat();
// ComPackBase functions
  virtual StatusCode pack (const double, d_ULong &) const;
  virtual StatusCode unpack (const d_ULong, double &) const;
  void print(std::ostream& s=std::cout) const;
private:
// This class uses ComPackExpFloat to do the dirty work
  ComPackExpFloat _packer;
  unsigned _signbit; // sign bits location
  unsigned _signmask; // mask for positive
};

#endif






