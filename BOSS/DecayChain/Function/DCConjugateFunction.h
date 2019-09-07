#ifndef DCHAIN_CONJUGATEFUNCTION_H
#define DCHAIN_CONJUGATEFUNCTION_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ConjugateFunction
// 
// Description: Base class for analysis functional objects
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 15:46:35 EDT 1996
// $Id: DCConjugateFunction.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCConjugateFunction.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:13  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:49  cdj
// imported DChain
//
// Revision 1.5  1998/08/20 19:57:28  sjp
// Modified to use DChainBoolean
//
// Revision 1.4  1998/04/17 19:12:09  sjp
// Modified to use latest type
//
// Revision 1.3  1997/08/15 21:31:51  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.2  1997/01/21 20:31:20  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.1  1996/11/04 16:49:43  sjp
// New function for use in conjunction with new `Lists'
//

// system include files
#include <functional>

// user include files

// forward declarations

template < class Arg >
class DCConjugateFunction : public std::binary_function<Arg, bool, void>
{
      // friend classses and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCConjugateFunction();
      virtual ~DCConjugateFunction();

      // member functions

      // const member functions
      virtual void operator()( const Arg& ,
			       bool ) = 0 ;

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      DCConjugateFunction( const DCConjugateFunction< Arg >& ) ; // stop default

      // assignment operator(s)
      const DCConjugateFunction& operator=( const DCConjugateFunction< Arg >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

};

// inline function definitions

#endif /* DCHAIN_CONJUGATEFUNCTION_H */
