//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: AstAbsAssociation.h,v 1.1.1.1 2005/04/21 01:15:12 zhangy Exp $
//
// Description:
//	Class AstAbsAssociation
//      Abstract interface for simple one to one associations
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Phil Strother           LBNL
//
//------------------------------------------------------------------------

#ifndef ASTABSASSOCIATION_HH
#define ASTABSASSOCIATION_HH

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
//DEL #include "AbsEvent/AbsEvtObj.hh"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------


//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

template <class T1, class T2> class AstAbsAssociation;

//		---------------------
// 		-- Class Interface --
//              ---------------------

template <class T1, class T2>
class AstAbsAssociation{ //DEL : public AbsEvtObj{

//--------------------
// Declarations     --
//--------------------

    // Typedefs, consts, and enums

//--------------------
// Instance Members --
//--------------------

public:

  // Constructors
  // Should be protected?
  AstAbsAssociation();

  // Copy Constructor

  // Destructor
  virtual ~AstAbsAssociation( );

  // Operators
  
  bool operator==( const AstAbsAssociation<T1, T2> & ) const;

  bool operator!=( const AstAbsAssociation<T1, T2> & ) const;

  // Selectors (const)

  virtual const T1 *firstArgument() const=0;
  virtual const T2 *secondArgument() const=0;

private:

  AstAbsAssociation( const AstAbsAssociation<T1, T2> &);

};

//SKIP #ifdef    BES_COMP_INST
//CHANGE #include  "MdcTrkRecon/Tools/AstAbsAssociation.cxx"
#include  "AstAbsAssociation.icc"
//SKIP #endif // BES_COMP_INST

#endif







