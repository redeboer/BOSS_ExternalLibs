//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackExpFloat.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      class ComPackExpFloat; pack a floating (double) into a fixed range
//      mantissa with an exponential (power of 2) representation.  With this class, the values
//      stored at the start of the range have the best precision, which
//      gradually decreases to the worst precision when going to the opposite
//      range.  This is equivalent to flat packing of the log of the value,
//      but because the exponent storage space is variable, this class allows
//      the equivalent of scaling the (log) value.
//
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

#ifndef COMPACKEXPFLOAT_HH
#define COMPACKEXPFLOAT_HH

#include "MdcRecoUtil/ComPackBase.h"
#include <iostream>

class ComPackExpFloat:public ComPackBase<double> {

public:
// specify the storage space (number of total bits) and the maximum exponent.
// The mantissa will be stored in the bits left over after storing the exponent
// Note that the exponent is -UNSIGNED-.  Note that the stop and start values do
// _not_ have to be monotonic; the order simply defines which end of the range
// has the fine precision (start) vs coarse (stop).  The 'center' command will
// use the start and stop values to define the (exact) range for unpacking data
// (ie the packing bins will be centered about those values).
  ComPackExpFloat(unsigned nbits,
		  unsigned maxexponent,
		  double start,
		  double stop,
		  bool center=false);
  virtual ~ComPackExpFloat();
// ComPackBase functions
  virtual StatusCode pack (const double, d_ULong &) const;
  virtual StatusCode unpack (const d_ULong, double &) const;
  void print(std::ostream& os) const;
private:
  unsigned _maxexp; // maximum value of the exponent (+1)
  unsigned _expmsk; // exponent storage mask
  unsigned _mansft; // mantissa storage shift
  unsigned _manmsk; // mantissa storage mask
  unsigned _maxman; // maximum integer mantissa
  double _invrange; // inverse range
// pre-computed quantities to speed unpacking
  double* _expfac;
  double* _expoff;
// disallow copy and equivalence
  ComPackExpFloat(const ComPackExpFloat&);
  ComPackExpFloat& operator = (const ComPackExpFloat&); 
};

#endif






