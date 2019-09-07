#ifndef DCHAIN_WHOLECANDIDATEITR_H
#define DCHAIN_WHOLECANDIDATEITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      WholeCandidateItr
// 
// Description: a const Iterator though whole of LabeledCandidateList
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: WholeCandidateItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: WholeCandidateItr.h,v $
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
#include "DecayChain/Iterator/candidateitr.h" // distance
#include "DecayChain/Element/LabeledCandidate.h"
#include "DecayChain/List/IndexedLabeledCandidates.h"


// forward declarations
//template < class CandidateClass > class IndexedLabeledCandidates ;
namespace dchain {

//template < class CandidateClass > class LabeledCandidate ;
template < class CandidateClass > class LabeledCandidateList ;

template < class CandidateClass >
class WholeCandidateItr 
{
   // friend classses and functions
   friend class LabeledCandidateList< CandidateClass > ;

   public:
   // constants, enums and typedefs
   typedef const LabeledCandidate<CandidateClass> value_type;
   typedef dchain::candidateitr::size_type distance_type;
   typedef dchain::candidateitr::size_type size_type;
   typedef dchain::candidateitr::difference_type difference_type;
   typedef std::random_access_iterator_tag iterator_category;
   typedef const LabeledCandidate<CandidateClass>* pointer;
   typedef const LabeledCandidate<CandidateClass>& reference;


   // Constructors and destructor
   WholeCandidateItr( const WholeCandidateItr< CandidateClass >& aOtherItr ) ;
   virtual ~WholeCandidateItr() {}

   // assignment operator(s)
   const WholeCandidateItr& operator=( const WholeCandidateItr< CandidateClass >& aOtherItr ) ;

   // member functions
   WholeCandidateItr< CandidateClass >& operator++() ;
   WholeCandidateItr< CandidateClass > operator++( int ) ;
   WholeCandidateItr< CandidateClass >& operator--() ;
   WholeCandidateItr< CandidateClass > operator--( int ) ;
   WholeCandidateItr< CandidateClass >& operator+=( difference_type n ) ;
   WholeCandidateItr< CandidateClass > operator+( difference_type n ) ;
   WholeCandidateItr< CandidateClass >& operator-=( difference_type n ) ;
   WholeCandidateItr< CandidateClass > operator-( difference_type n ) ;

   // const member functions
   const LabeledCandidate< CandidateClass >& operator*() const ;
   const LabeledCandidate< CandidateClass >& operator[]( size_type n ) const ;
   difference_type operator-( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;

   bool operator==( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator!=( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator<( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator>( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator>=( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;
   bool operator<=( const WholeCandidateItr< CandidateClass >& aOtherItr ) const ;

   // static member functions

   protected:
   // Constructors and destructor
   WholeCandidateItr( const IndexedLabeledCandidates< CandidateClass >* aList ,
	 const size_type aIndex ) ;

   // protected member functions

   // protected const member functions
   IndexedLabeledCandidates< CandidateClass >* indexedCandidates() const ;
   size_type index() const ;

   private:
   // Constructors and destructor
   WholeCandidateItr() ; // stop default

   // private member functions

   // private const member functions

   // data members
   IndexedLabeledCandidates< CandidateClass >* m_indexedCandidates ;
   size_type m_index ;

   // static data members

} ;

// inline function definitions

// user include files
//
// forward definitions of inline functions
//

template < class CandidateClass >
inline
IndexedLabeledCandidates< CandidateClass >* WholeCandidateItr< CandidateClass >::indexedCandidates() const
{
   return ( m_indexedCandidates ) ;
}

template < class CandidateClass >
inline
typename WholeCandidateItr<CandidateClass>::size_type WholeCandidateItr< CandidateClass >::index() const
{
   return ( m_index ) ;
}

//
// constructors and destructor
//

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >::WholeCandidateItr( const WholeCandidateItr< CandidateClass >& aOtherItr ) :
   m_indexedCandidates( aOtherItr.indexedCandidates() ) ,
   m_index( aOtherItr.index() )
{
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >::WholeCandidateItr( const IndexedLabeledCandidates< CandidateClass >* aList ,
					    const size_type aIndex ) :
// cast away const as const_itr can be assocciated with a non-const list,
//   but only const lists use this constructor
   m_indexedCandidates( (IndexedLabeledCandidates< CandidateClass >*)aList ) ,
   m_index( aIndex )
{   
}

//
// assignment operators
//

template < class CandidateClass >
inline
const WholeCandidateItr< CandidateClass >& WholeCandidateItr< CandidateClass >::operator=( const WholeCandidateItr< CandidateClass >& aOtherItr )
{
   m_indexedCandidates = aOtherItr.indexedCandidates() ;
   m_index = aOtherItr.index() ;
   return ( *this ) ;
}

//
// member functions
//

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >& WholeCandidateItr< CandidateClass >::operator++()
{
   ++m_index ;
   return ( *this ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass > WholeCandidateItr< CandidateClass >::operator++( int )
{
   WholeCandidateItr tmp( *this ) ;
   ++m_index ;
   return ( tmp ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >& WholeCandidateItr< CandidateClass >::operator--()
{
   --m_index ;
   return ( *this ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass > WholeCandidateItr< CandidateClass >::operator--( int )
{
   WholeCandidateItr tmp( *this ) ;
   --m_index ;
   return ( tmp ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >& WholeCandidateItr< CandidateClass >::operator+=( difference_type n )
{
   m_index += n ;
   return( *this ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass > WholeCandidateItr< CandidateClass >::operator+( difference_type n )
{
   WholeCandidateItr tmp( *this ) ;
   return ( tmp += n ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass >& WholeCandidateItr< CandidateClass >::operator-=( difference_type n )
{
   m_index -= n ;
   return( *this ) ;
}

template < class CandidateClass >
inline
WholeCandidateItr< CandidateClass > WholeCandidateItr< CandidateClass >::operator-( difference_type n )
{
   WholeCandidateItr tmp( *this ) ;
   return ( tmp -= n ) ;
}

//
// const member functions
//

template < class CandidateClass >
inline
const LabeledCandidate< CandidateClass >& WholeCandidateItr< CandidateClass >::operator*() const
{
   return ( (*m_indexedCandidates).labeledCandidateClass( m_index ) ) ;
}

template < class CandidateClass >
inline
const LabeledCandidate< CandidateClass >& WholeCandidateItr< CandidateClass >::operator[]( size_type n ) const
{
   return ( (*m_indexedCandidates).labeledCandidateClass( m_index + n ) ) ;
}

template < class CandidateClass >
inline
typename WholeCandidateItr<CandidateClass>::difference_type 
WholeCandidateItr< CandidateClass >::operator-( const WholeCandidateItr& aOtherItr ) const
{
   return ( m_index - aOtherItr.index() ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator==( const WholeCandidateItr& aOtherItr ) const
{
   return ( ( m_index == aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator!=( const WholeCandidateItr& aOtherItr ) const
{
   return ( ( m_index != aOtherItr.index() ) ||
	    ( m_indexedCandidates != aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator<( const WholeCandidateItr& aOtherItr ) const
{
   return ( ( m_index < aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator>( const WholeCandidateItr& aOtherItr ) const 
{
   return ( ( m_index > aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator>=( const WholeCandidateItr& aOtherItr ) const
{
   return ( ( m_index >= aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool WholeCandidateItr< CandidateClass >::operator<=( const WholeCandidateItr& aOtherItr ) const
{
   return ( ( m_index <= aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}
}

#endif /* DCHAIN_WHOLECANDIDATEITR_H */
