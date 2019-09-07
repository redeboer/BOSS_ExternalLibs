#ifndef DCHAIN_INDEXEDLABELEDCANDIDATES_H
#define DCHAIN_INDEXEDLABELEDCANDIDATES_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      IndexedLabeledCandidates
// 
// Description: Interface used by Candidate iterators
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Sep 13 17:58:11 EDT 1996
// $Id: IndexedLabeledCandidates.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: IndexedLabeledCandidates.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:27  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/Element/ReferenceCount.h" // superclass

// forward declarations
namespace dchain {
template < class CandidateClass > class LabeledCandidate ;
template < class CandidateClass > class MuteWholeCandidateItr ;
template < class CandidateClass > class WholeCandidateItr ;
template < class CandidateClass > class PartialCandidateItr ;

template < class CandidateClass >
class IndexedLabeledCandidates
   : public ReferenceCount
{
      // friend classses and functions
      friend class dchain::MuteWholeCandidateItr< CandidateClass > ;
      friend class dchain::WholeCandidateItr< CandidateClass > ;
      friend class dchain::PartialCandidateItr< CandidateClass > ;

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      IndexedLabeledCandidates() ;
      virtual ~IndexedLabeledCandidates() ;

      // member functions

      // const member functions
      virtual int size() const = 0 ;

      // static member functions

   protected:
      // protected member functions
      virtual dchain::LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) = 0 ;

      // protected const member functions
      virtual const dchain::LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) const = 0 ;

   private:
      // Constructors and destructor
      IndexedLabeledCandidates( const IndexedLabeledCandidates< CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const IndexedLabeledCandidates& operator=( const IndexedLabeledCandidates< CandidateClass >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

} ;

}
// inline function definitions

// function definitions
# include "DecayChain/List/Template/IndexedLabeledCandidates.cc"

#endif // DCHAIN_INDEXEDLABELEDCANDIDATES_H
