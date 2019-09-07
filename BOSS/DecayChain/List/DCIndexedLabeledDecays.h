#ifndef DCHAIN_INDEXEDLABELEDDECAYS_H
#define DCHAIN_INDEXEDLABELEDDECAYS_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      IndexedLabeledDecays
// 
// Description: Interface used by Stone iterators
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Sep 13 17:58:11 EDT 1996
// $Id: DCIndexedLabeledDecays.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCIndexedLabeledDecays.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:25  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:53  cdj
// imported DChain
//
// Revision 1.1  1998/08/28 05:17:28  sjp
// New class for iterator access
//

// system include files

// user include files
#include "DecayChain/List/LabeledCandidateList.h" // superclass

namespace dchain {

// forward declarations
template < class DecayClass > class LabeledDecay ;
template < class DecayClass > class MuteWholeItr ;
template < class DecayClass > class WholeItr ;
template < class DecayClass > class PartialItr ;

template < class DecayClass , class CandidateClass >
class IndexedLabeledDecays : public LabeledCandidateList< CandidateClass >
{
   // friend classses and functions
   friend class MuteWholeItr< DecayClass > ;
   friend class WholeItr< DecayClass > ;
   friend class PartialItr< DecayClass > ;

   public:
   // constants, enums and typedefs

   // Constructors and destructor
   IndexedLabeledDecays() ;
   virtual ~IndexedLabeledDecays() ;

   // member functions

   // const member functions

   // static member functions

   protected:
   // protected member functions
   virtual LabeledDecay< DecayClass >& labeledDecayClass( const int aIndex ) = 0 ;

   // protected const member functions
   virtual const LabeledDecay< DecayClass >& labeledDecayClass( const int aIndex ) const = 0 ;

   private:
   // Constructors and destructor
   IndexedLabeledDecays( const IndexedLabeledDecays< DecayClass , CandidateClass >& ) ; // stop default

   // assignment operator(s)
   const IndexedLabeledDecays& operator=( const IndexedLabeledDecays< DecayClass , CandidateClass>& ) ; // stop default

   // private member functions

   // private const member functions

   // data members

   // static data members

} ;

}

// inline function definitions

// function definitions
//#include "DecayChain/List/Template/IndexedLabeledDecays.cc"

#endif // DCHAIN_INDEXEDLABELEDDECAYS_H

