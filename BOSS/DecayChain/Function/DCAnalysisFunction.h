#ifndef DCHAIN_ANALYSISFUNCTION_H
#define DCHAIN_ANALYSISFUNCTION_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      AnalysisFunction
// 
// Description: Base class for analysis functional objects
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 15:46:35 EDT 1996
// $Id: DCAnalysisFunction.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCAnalysisFunction.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:12  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:49  cdj
// imported DChain
//
// Revision 1.5  1998/08/27 04:55:16  sjp
// added include of defn when required
//
// Revision 1.4  1997/08/15 21:31:47  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.3  1997/01/21 20:31:18  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:08:13  sjp
// Modified to support FILENAME_ONLY
//
// Revision 1.1  1996/11/04 16:49:40  sjp
// New function for use in conjunction with new `Lists'
//

// system include files

// user include files

// forward declarations

template < class Arg >
class DCAnalysisFunction
{
      // friend classses and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCAnalysisFunction();
      virtual ~DCAnalysisFunction();

      // member functions
      virtual void operator()( const Arg& ) = 0 ;

      // const member functions
      void operator()( const Arg& iArg ) const {
	 const_cast<DCAnalysisFunction<Arg>*>(this)->operator()(iArg);
      }

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      DCAnalysisFunction( const DCAnalysisFunction< Arg >& ) ; // stop default

      // assignment operator(s)
      const DCAnalysisFunction& operator=( const DCAnalysisFunction< Arg >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

};

// inline function definitions

#endif /* DCHAIN_ANALYSISFUNCTION_H */
