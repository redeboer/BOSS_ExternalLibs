//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: HTRange.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Template of HTRange class - class to hold lower/upper limits
//      for tuple columns
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Salnikov
//
// Copyright Information:
//	Copyright (C) 1994	<Institution>
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef HTRange_H
#define HTRange_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------

template<class T> 
class HTRange {

  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:

  // Constructors
  HTRange() : 
    _defined(false), _lower(0), _upper(0) 
  {
  } 
  HTRange(T lower, T upper) : 
    _defined(true), _lower(lower), _upper(upper) 
  {
  }

  // Copy Constructor
  HTRange( const HTRange<T>& o) :
    _defined(o._defined), _lower(o._lower), _upper(o._upper)
  {
  }

  // Destructor
  ~HTRange( ) {}

  // Operators
    
  HTRange<T>& operator= ( const HTRange<T>& o) 
  {
    if (&o == this) return *this;
    _defined = o._defined ;
    _lower = o._lower ;
    _upper = o._upper ;
    return *this ;
  }

  // Selectors (const)
  bool operator()() const { return _defined ; }

  // get upper/lower limits
  T lower() const { return _lower ; }
  T upper() const { return _upper ; }

  // Modifiers

protected:

  // Helper functions

private:

  // Friends

  // Data members
  bool _defined ;
  T _lower, _upper ;

};

//
// Some compilers require definitions of template methods to reside
// in a separate .cc file, and some require them to reside in the .hh
// file (this file).  The following construct allows us to control
// this from the makefile.

// 1. Implement your methods in HTRange.cc.
// 2. Exclude the HTRange.cc file from the list of compilations.
 
//SKIP #ifdef    BES_COMP_INST
//CHANGE #include  "MdcTrkRecon/Tools/HTRange.cxx"
#include  "HTRange.icc"

//SKIP #endif // BES_COMP_INST

#endif  // TEMPLATE_H
