#ifndef DCHAIN_PARTIALCANDIDATEITR_H
#define DCHAIN_PARTIALCANDIDATEITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      PartialCandidateItr
// 
// Description: Iterator through a part of a LabeledCandidateList.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: PartialCandidateItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: PartialCandidateItr.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:18  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files
#include <iterator>

// user include files
#include "DecayChain/Element/conjugation.h" // enumarator
#include "DecayChain/Iterator/candidateitr.h" // distance
#include "DecayChain/Element/LabeledCandidate.h" // equal function
//#include "DecayChain/List/IndexedLabeledCandidates.h" // labeledStonceClass()

// forward declarations

template < class CandidateClass > class IndexedLabeledCandidates ;

namespace dchain {

template < class CandidateClass > class LabeledCandidate ;
template < class CandidateClass > class LabeledCandidateList ;

template < class CandidateClass >
class PartialCandidateItr
{
   // friend classses and functions
   friend class LabeledCandidateList< CandidateClass > ;

   public:
   // constants, enums and typedefs
   //      typedef int distance ;
   typedef LabeledCandidate<CandidateClass>   value_type;
   typedef dchain::candidateitr::difference_type difference_type;
   typedef dchain::candidateitr::size_type size_type;
   typedef value_type&        reference;
   typedef value_type*        pointer;

   typedef std::bidirectional_iterator_tag iterator_category;


   // Constructors and destructor
   PartialCandidateItr() ; // used by CombinatoricList to build array
   PartialCandidateItr( const PartialCandidateItr< CandidateClass >& aOtherItr ) ;
   virtual ~PartialCandidateItr() {}

   // assignment operator(s)
   const PartialCandidateItr< CandidateClass >& operator=( const PartialCandidateItr< CandidateClass >& aOtherItr ) ;

   // member functions
   PartialCandidateItr< CandidateClass >& operator++() ;
   PartialCandidateItr< CandidateClass > operator++( int ) ;
   PartialCandidateItr< CandidateClass >& operator--() ;
   PartialCandidateItr< CandidateClass > operator--( int ) ;

   // const member functions
   const LabeledCandidate< CandidateClass >& operator*() const ;
   bool operator==( const PartialCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator!=( const PartialCandidateItr< CandidateClass >& aOtherItr ) const ;

   // static member functions

   protected:
   // Constructors and destructor
   PartialCandidateItr( const IndexedLabeledCandidates< CandidateClass >* aList ,
	 const size_type aIndex ,
	 const conjugation::Label aLabel ) ;

   // protected member functions

   // protected const member functions
   IndexedLabeledCandidates< CandidateClass >* indexedCandidates() const ;
   size_type index() const ;
   conjugation::Label label() const ;

   private:
   // Constructors and destructor

   // private member functions

   // private const member functions

   // data members
   IndexedLabeledCandidates< CandidateClass >* m_indexedCandidates ;
   size_type m_index ;
   conjugation::Label m_label ;

   // static data members

} ;

// inline function definitions

// user include files

//
// forward definitions of inline functions
//

template < class CandidateClass >
inline
IndexedLabeledCandidates< CandidateClass >* PartialCandidateItr< CandidateClass >::indexedCandidates() const
{
   return ( m_indexedCandidates ) ;
}

template < class CandidateClass >
inline
typename PartialCandidateItr<CandidateClass>::size_type 
PartialCandidateItr< CandidateClass >::index() const
{
   return ( m_index ) ;
}

template < class CandidateClass >
inline
conjugation::Label PartialCandidateItr< CandidateClass >::label() const
{
   return ( m_label ) ;
}

//
// constructors and destructor
//

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass >::PartialCandidateItr()
{
}

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass >::PartialCandidateItr( const PartialCandidateItr< CandidateClass >& aOtherItr ) :
   m_indexedCandidates( aOtherItr.indexedCandidates() ) ,
   m_index( aOtherItr.index() ) ,
   m_label( aOtherItr.label() )
{
}

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass >::PartialCandidateItr( const IndexedLabeledCandidates< CandidateClass >* aList ,
      const size_type aIndex ,
      const conjugation::Label aLabel ) :
   // cast away const as const_iterator can be assocciated with a non-const list,
   //   but only const lists use this constructor
   m_indexedCandidates( (IndexedLabeledCandidates< CandidateClass >*)aList ) ,
   m_index( aIndex ) ,
   m_label( aLabel )
{
   // move to first entry with correct label
   if ( ( m_index < size_type( (*m_indexedCandidates).size() ) ) &&
	 ( (*m_indexedCandidates).labeledCandidateClass( m_index ) != m_label )
      ) {
      operator++() ;
   }
}

//
// assignment operators
//

template < class CandidateClass >
inline
const PartialCandidateItr< CandidateClass >& PartialCandidateItr< CandidateClass >::operator=( const PartialCandidateItr< CandidateClass >& aOtherItr )
{
   m_indexedCandidates = aOtherItr.indexedCandidates() ;
   m_index = aOtherItr.index() ;
   m_label = aOtherItr.label() ;
   return ( *this ) ;
}

//
// member functions
//

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass >& PartialCandidateItr< CandidateClass >::operator++()
{
   ++m_index ;
   // This while loop find next match to label.
   // The order is done for efficiency reasons.  It is more likely a label
   //   will match than the end of the list has been reached.  However this
   //   will can cause an access to an uninitiallized location, but the
   //   loop will still terminate correctly.
   // update: the efficiency doesn't matter but reading invalid memory
   //   makes our automated memory checkers have fits
   while (
	 ( m_index < size_type( (*m_indexedCandidates).size() ) &&
	   ( (*m_indexedCandidates).labeledCandidateClass( m_index ) != m_label )
	 ) ) {
      ++m_index ;
   }
   return ( *this ) ;
}

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass > PartialCandidateItr< CandidateClass >::operator++( int )
{
   PartialCandidateItr< CandidateClass > tmp( *this ) ;
   ++m_index ;
   // This while loop find next match to label.
   // The order is done for efficiency reasons.  It is more likely a label
   //   will match than the end of the list has been reached.  However this
   //   will can cause an access to an uninitiallized location, but the
   //   loop will still terminate correctly.
   while (
	 ( (*m_indexedCandidates).labeledCandidateClass( m_index ) != m_label ) &&
	 ( m_index < size_type( (*m_indexedCandidates).size() ) ) ) {
      ++m_index ;
   }
   return ( tmp ) ;
}

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass >& PartialCandidateItr< CandidateClass >::operator--()
{
   // As the index is 'unsigned' it should not go below zero.  This behavior is completely
   //   consistent with the STL reverse_iterator adaptor. (See pp254 of Musser & Saini)
   //
   if ( 0 != m_index ) {
      --m_index ;
   }
   // This while loop find next match to label.
   // The order is done for efficiency reasons.  It is more likely a label
   //   will match than the end of the list has been reached.  However this
   //   will can cause an access to an uninitiallized location, but the
   //   loop will still terminate correctly.
   while (
	 ( (*m_indexedCandidates).labeledCandidateClass( m_index ) != m_label ) &&
	 ( m_index != 0 ) ) {
      --m_index ;
   }
   return ( *this ) ;
}

template < class CandidateClass >
inline
PartialCandidateItr< CandidateClass > PartialCandidateItr< CandidateClass >::operator--( int )
{
   PartialCandidateItr< CandidateClass > tmp( *this ) ;
   // As the index is 'unsigned' it should not go below zero.  This behavior is completely
   //   consistent with the STL reverse_iterator adaptor. (See pp254 of Musser & Saini)
   //
   if ( 0 != m_index ) {
      --m_index ;
   }
   // This while loop find next match to label.
   // The order is done for efficiency reasons.  It is more likely a label
   //   will match than the end of the list has been reached.  However this
   //   will can cause an access to an uninitiallized location, but the
   //   loop will still terminate correctly.
   while (
	 ( (*m_indexedCandidates).labeledCandidateClass( m_index ) != m_label ) &&
	 ( m_index != 0 ) ) {
      --m_index ;
   }
   return ( tmp ) ;
}

//
// const member functions
//

template < class CandidateClass >
inline
const LabeledCandidate< CandidateClass >& PartialCandidateItr< CandidateClass >::operator*() const
{
   return ( (*m_indexedCandidates).labeledCandidateClass( m_index ) ) ;
}

template < class CandidateClass >
inline
bool PartialCandidateItr< CandidateClass >::operator==( const PartialCandidateItr< CandidateClass >& aOtherItr ) const
{
   // There needs to be a final test on the label as itrs with two different labels
   //   have the same values for the past-the-end value
   return ( ( m_index == aOtherItr.index() ) &&
	 ( m_indexedCandidates == aOtherItr.indexedCandidates() ) &&
	 ( m_label == aOtherItr.label() ) ) ;
}

template < class CandidateClass >
inline
bool PartialCandidateItr< CandidateClass >::operator!=( const PartialCandidateItr< CandidateClass >& aOtherItr ) const
{
   // There needs to be a final test on the label as itrs with two different labels
   //   have the same values for the past-the-end value
   return ( ( m_index != aOtherItr.index() ) ||
	 ( m_indexedCandidates != aOtherItr.indexedCandidates() ) ||
	 ( m_label != aOtherItr.label() ) ) ;
}
}

#endif /* DCHAIN_PARTIALCANDIDATEITR_H */
