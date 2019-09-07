#ifndef DCHAIN_SELECTIONFUNCTION_H
#define DCHAIN_SELECTIONFUNCTION_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      SelectionFunction
// 
// Description: Base class for selection functional objects
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 15:46:35 EDT 1996
// $Id: DCSelectionFunction.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCSelectionFunction.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.3  2006/01/11 20:28:13  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2003/10/01 23:45:18  cdj
// added compound selection function
//
// Revision 1.1.1.1  2000/12/18 22:16:49  cdj
// imported DChain
//
// Revision 1.7  1998/08/27 04:55:17  sjp
// added include of defn when required
//
// Revision 1.6  1998/08/20 19:57:29  sjp
// Modified to use DChainBoolean
//
// Revision 1.5  1998/04/17 19:12:10  sjp
// Modified to use latest type
//
// Revision 1.4  1997/08/15 21:31:54  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.3  1997/01/21 20:31:22  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:08:15  sjp
// Modified to support FILENAME_ONLY
//
// Revision 1.1  1996/11/04 16:49:42  sjp
// New function for use in conjunction with new `Lists'
//

// system include files
#include <functional>

// user include files

// forward declarations

template < class Arg >
class DCSelectionFunction : public std::unary_function<Arg&, bool>
{
      // friend classses and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCSelectionFunction() {}
      virtual ~DCSelectionFunction() {}

      // member functions
      virtual bool operator()( Arg& ) =0;

      // const member functions
      bool operator()(Arg& iArg) const {
	 return const_cast<DCSelectionFunction<Arg> *>(this)->operator()(iArg);
      }

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      //DCSelectionFunction( const DCSelectionFunction< Arg >& ) ; // stop default

      // assignment operator(s)
      //const DCSelectionFunction& operator=( const DCSelectionFunction< Arg >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

};

// inline function definitions

#endif /* DCHAIN_SELECTIONFUNCTION_H */
