//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: HTColumn.cxx,v 1.3 2010/03/25 09:55:57 zhangy Exp $
//
// Description:
//	Class HTColumn
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

//-----------------------
// This Class's Header --
//-----------------------
#include "MdcRecoUtil/HTColumn.h"

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------
#include <iostream>
#include <iomanip>
using std::endl;
using std::ends;
using std::ios;
using std::cout;
using std::resetiosflags;
using std::setiosflags;
using std::setw;
using std::string;

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

static const char rscid[] = "$Id: HTColumn.cxx,v 1.3 2010/03/25 09:55:57 zhangy Exp $";

//		----------------------------------------
// 		-- Public Function Member Definitions --
//		----------------------------------------

//----------------
// Constructors --
//----------------
HTColumn::HTColumn( const string& label, 
		    const string& block, 
		    const string& indexLabel ) :
  _label(label),
  _block(block),
  _indexLabel(indexLabel)
{
}

HTColumn::HTColumn( const HTColumn& o) :
  _label(o._label),
  _block(o._block),
  _indexLabel(o._indexLabel)
{
}

//--------------
// Destructor --
//--------------
HTColumn::~HTColumn()
{
}

//-------------
// Methods   --
//-------------
    
//-------------
// Operators --
//-------------
HTColumn&
HTColumn::operator=( const HTColumn& o)
{
  if (&o == this) return *this;
  _label = o._label ;
  _block = o._block ;
  _indexLabel = o._indexLabel ;

  return *this ;
}


//-------------
// Selectors --
//-------------
const string&
HTColumn::label() const
{
  return _label ;
}

const string&
HTColumn::block() const
{
  return _block ;
}

const string&
HTColumn::indexLabel() const
{
  return _indexLabel ;
}

void 
HTColumn::print(/*ostream& out*/) const
{
  cout << label() ;
  if ( indexLabel().length() > 0 ) cout << "(" << indexLabel() << ")" ;
  cout<< ends ;

  cout << " @ " << setw(10) << block() ;
  cout << " : " << typeName() ;
  cout << endl ;
  cout << resetiosflags(ios::adjustfield) ;
}

//-------------
// Modifiers --
//-------------


