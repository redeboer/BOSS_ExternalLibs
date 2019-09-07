//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: ComPackBase.h,v 1.4 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//      Class ComPackBase
//      - Templated base class for utility classes that 
//        take a concrete numerical type and convert it for use in micro 
//        and nano databases.
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
// Quote:
//      "It ain't what you do, it's the way that you do it"
//
//--------------------------------------------------------------------------

#ifndef COMPACKBASE_HH
#define COMPACKBASE_HH

//
// C includes
//
#include <stddef.h> 
#include <stdlib.h> 
#include "MdcRecoUtil/ComPackBaseBase.h"
#include "MdcRecoUtil/BesODMGTypes.h"

template <class T>
class ComPackBase : public ComPackBaseBase{
public:

  ///////////////////
  // Constructors: //
  ///////////////////
  ComPackBase(){}; 

  ComPackBase(T minval, T maxval,d_ULong bitrange) :
    _minVal(minval),_maxVal(maxval),_valRange(maxval-minval),
    _bitRange(bitrange),_bitMask((1<<bitrange)-1){;}

  ///////////////////
  // Destructor:   //
  ///////////////////
  virtual ~ComPackBase(){};

  ///////////////////
  // Packers:      //
  ///////////////////
  virtual StatusCode pack (const T ,d_ULong&) const =0;

  ///////////////////
  // Unpackers:    //
  ///////////////////
  virtual StatusCode unpack (const d_ULong, T &) const =0;

  ///////////////////
  // Accessors:    //
  ///////////////////
  virtual const T& getMinVal() const { return _minVal;};
  virtual const T& getMaxVal() const { return _maxVal;};
  virtual const T& getRange() const { return _valRange;};
// bitrange is defined as the number of bits used; bitmask is the hex map
  d_ULong bitRange() const { return _bitRange; }
  d_ULong bitMask() const { return _bitMask; }

  // Don't allow copies, assignment
  ComPackBase( const ComPackBase& ) { ::abort(); }
  ComPackBase& operator= ( const ComPackBase& ) {::abort(); return *this;}

protected:

  enum { _maxlongbits = 32 };
  T _minVal;
  T _maxVal;
  T _valRange;
  d_ULong _bitRange;
  d_ULong _bitMask;
};

#endif

