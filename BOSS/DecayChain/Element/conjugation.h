#ifndef DCHAIN_CONJUGATION_H
#define DCHAIN_CONJUGATION_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      Conjugation
// 
// Description: enumerator for conjugation
//
// Usage:
//    Used by the list hierarchy to determine the conjugation of a
//      Candidate with respect to the list it is in.
//    This has been designed as a name-space rather than a class
//
// Author:      Simon Patton
// Created:     Fri Sep 13 18:06:37 EDT 1996
// $Id: conjugation.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: conjugation.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:10  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files

// forward declarations
namespace dchain {
   namespace conjugation
   {
      // constants, enums and typedefs
      enum Label { kNone = 0 ,
	 kHeads = 1 ,
	 kTails = -1
      } ;
   }

   inline
      conjugation::Label otherLabel( const conjugation::Label& aLabel ) ;

   // inline function definitions

   inline
      conjugation::Label otherLabel( const conjugation::Label& aLabel )
      {
	 return ( conjugation::Label( conjugation::kNone - aLabel ) ) ;
      }
}
#endif /* DCHAIN_CONJUGATION_H */
