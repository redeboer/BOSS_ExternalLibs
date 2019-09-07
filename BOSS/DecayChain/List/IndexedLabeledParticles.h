#ifndef DCHAIN_INDEXEDLABELEDPARTICLES_H
#define DCHAIN_INDEXEDLABELEDPARTICLES_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      IndexedLabeledParticles
// 
// Description: Interface used by Stone iterators
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Sep 13 17:58:11 EDT 1996
// $Id: IndexedLabeledParticles.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: IndexedLabeledParticles.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:28  cdj
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

// forward declarations

namespace dchain {
   template < class T > class LabeledParticle ;
   template < class T > class MuteWholeItr ;
   template < class T > class WholeItr ;
   template < class T > class PartialItr ;

template < class T , class CandidateClass >
class IndexedLabeledParticles
   : public LabeledCandidateList< CandidateClass >
{
      // friend classses and functions
      friend class MuteWholeItr< T > ;
      friend class WholeItr< T > ;
      friend class PartialItr< T > ;

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      IndexedLabeledParticles() {}
      virtual ~IndexedLabeledParticles() {}

      // member functions

      // const member functions

      // static member functions

   protected:
      // protected member functions
      virtual LabeledParticle< T >& labeledParticleClass( const int aIndex ) = 0 ;

      // protected const member functions
      virtual const LabeledParticle< T >& labeledParticleClass( const int aIndex ) const = 0 ;

   private:
      // Constructors and destructor
      IndexedLabeledParticles( const IndexedLabeledParticles< T , CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const IndexedLabeledParticles& operator=( const IndexedLabeledParticles< T , CandidateClass>& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

} ;
}
// inline function definitions

#endif // DCHAIN_INDEXEDLABELEDPARTICLES_H

