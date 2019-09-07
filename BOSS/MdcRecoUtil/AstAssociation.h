//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAssociation.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class AstAssociation
//      Very simple class to provide an association between two
//      pointer types which don't obviously belong together in a class,
//      but need to go together for some reason.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           Original author
//
// Copyright Information:
//	Copyright (C) 1997      Imperial College
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef ASTASSOCIATION_H
#define ASTASSOCIATION_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------
#include "MdcTrkRecon/Tools/AstAbsAssociation.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

template <class T1, class T2> class AstAssociation;

//		---------------------
// 		-- Class Interface --
//              ---------------------

template <class T1, class T2>
class AstAssociation : public AstAbsAssociation<T1, T2> {

//--------------------
// Declarations     --
//--------------------

    // Typedefs, consts, and enums

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  AstAssociation(T1 *, T2 *);

  // Copy Constructor
  AstAssociation( const AstAssociation<T1, T2> &);

  AstAssociation<T1, T2> &operator=(const AstAssociation<T1, T2> &);
  // Destructor
  virtual ~AstAssociation( );

  // Selectors (const)

  virtual const T1 *firstArgument() const;
  virtual const T2 *secondArgument() const;

  virtual T1 *firstArgument();
  virtual T2 *secondArgument();
  // Modifiers

  virtual void deleteMembers();

protected:
  
    // Helper functions

private:

  // Friends
  
  // Data members
  
  T1 *_itsT1;
  T2 *_itsT2;

  AstAssociation();
  
//------------------
// Static Members --
//------------------

public:

    // Selectors (const)

    // Modifiers

private:

    // Data members

};

//#ifdef    BABAR_COMP_INST
//CHANGE #include  "AssocTools/AstAssociation.cxx"
#include  "AstAssociation.icc"
//#endif // BABAR_COMP_INST

#endif







