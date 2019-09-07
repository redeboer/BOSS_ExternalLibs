//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: HTColumn.h,v 1.2 2009/12/23 02:59:56 zhangy Exp $
//
// Description:
//	Class HTColumn.
//      Base class for the ntuple columns. This is abstract one, all
//      real things will be implemented it the classes for specific types.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Salnikov
//
// Copyright Information:
//	Copyright (C) 1997	Saclay
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef HTColumn_H
#define HTColumn_H


//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <string>

//----------------------
// Base Class Headers --
//----------------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class HTColumnAcceptor ;

//		---------------------
// 		-- Class Interface --
//		---------------------

class HTColumn {

  //--------------------
  // Declarations     --
  //--------------------

  // Typedefs, consts, and enums

  //--------------------
  // Instance Members --
  //--------------------

public:

  // Constructors
  HTColumn( const std::string& label, 
	    const std::string& block=std::string(), 
	    const std::string& indexLabel=std::string() );
  
  // Copy Constructor
  HTColumn( const HTColumn& );

  // Destructor
  virtual ~HTColumn( );

  // Operators
  HTColumn& operator=(const HTColumn&) ;
    
  // Selectors (const)
  virtual const std::string& label() const ;
  virtual const std::string& block() const ;
  virtual const std::string& indexLabel() const ;

  // typeinfo
  virtual  const std::string& typeName() const = 0 ;

  // modifiers
  virtual void clearData() = 0 ;

  virtual void print(/*std::ostream& out*/) const ;

protected:

  // Helper functions

private:

  // Friends

  // Data members
  std::string _label ;
  std::string _block ;
  std::string _indexLabel ;

};

#endif // HTColumn_H
