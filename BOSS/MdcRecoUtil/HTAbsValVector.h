//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: HTAbsValVector.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	This is an abstract class specifing the interface which any 
//	vector-like class must provide if it needs to be used as the parameter
//	to ntuple's column() method taking arrays.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	A.Salnikov
//
// Copyright Information:
//	Copyright (C) 1997	Saclay
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef HTAbsValVector_H
#define HTAbsValVector_H

#include <sys/types.h>
#include <stddef.h>

//		---------------------
// 		-- Class Interface --
//		---------------------

template<class T> 
class HTAbsValVector {
  
public:
  virtual ~HTAbsValVector() {}

  // operators
  virtual const T& operator()(size_t i) const = 0;
  virtual T& operator()(size_t i) = 0;
  virtual const T& operator[](size_t i) const = 0;
  virtual T& operator[](size_t i) = 0;

  // methods
  virtual size_t length() const = 0;

};

#endif  // TEMPLATE_H
