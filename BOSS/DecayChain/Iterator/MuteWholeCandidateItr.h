#ifndef DCHAIN_MUTEWHOLECANDIDATEITR_H
#define DCHAIN_MUTEWHOLECANDIDATEITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      MuteWholeCandidateItr
// 
// Description: Iterator through a entire LabeledList.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: MuteWholeCandidateItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: MuteWholeCandidateItr.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:17  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/Iterator/candidateitr.h" // distance
#include "DecayChain/Element/LabeledCandidate.h"
//#include "DecayChain/List/IndexedLabeledCandidates.h"

// forward declarations
namespace dchain {

template < class CandidateClass > class IndexedLabeledCandidates ;
template < class CandidateClass> class LabeledCandidate ;
template < class CandidateClass > class LabeledCandidateList ;

template < class CandidateClass >
class MuteWholeCandidateItr
{
      // friend classses and functions
      friend class LabeledCandidateList< CandidateClass > ;

   public:
      // constants, enums and typedefs
      typedef const LabeledCandidate<CandidateClass> value_type;
      typedef candidateitr::size_type distance_type;
      typedef candidateitr::size_type size_type;
      typedef candidateitr::difference_type difference_type;
      typedef std::random_access_iterator_tag iterator_category;
      typedef const LabeledCandidate<CandidateClass>* pointer;
      typedef const LabeledCandidate<CandidateClass>& reference;

      // Constructors and destructor
      MuteWholeCandidateItr( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) ;
      virtual ~MuteWholeCandidateItr() {}

      // assignment operator(s)
      const MuteWholeCandidateItr< CandidateClass >& operator=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) ;

      // member functions
      MuteWholeCandidateItr< CandidateClass >& operator++() ;
      MuteWholeCandidateItr< CandidateClass > operator++( int ) ;
      MuteWholeCandidateItr< CandidateClass >& operator--() ;
      MuteWholeCandidateItr< CandidateClass > operator--( int ) ;
      MuteWholeCandidateItr< CandidateClass >& operator+=( difference_type n ) ;
      MuteWholeCandidateItr< CandidateClass > operator+( difference_type n ) ;
      MuteWholeCandidateItr< CandidateClass >& operator-=( difference_type n ) ;
      MuteWholeCandidateItr< CandidateClass > operator-( difference_type n ) ;

      // const member functions
      LabeledCandidate< CandidateClass >& operator*() const ;
      LabeledCandidate< CandidateClass >& operator[]( size_type n ) const ;
      difference_type operator-( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator==( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator!=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator<( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator>( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator>=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;
      bool operator<=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const ;

      // static member functions

   protected:
      // Constructors and destructor
      MuteWholeCandidateItr( IndexedLabeledCandidates< CandidateClass >* aList ,
			 const size_type aIndex ) ;

      // protected member functions

      // protected const member functions
      IndexedLabeledCandidates< CandidateClass >* indexedCandidates() const ;
      size_type index() const ;

   private:
      // Constructors and destructor
      MuteWholeCandidateItr() ; // stop default

      // private member functions

      // private const member functions

      // data members
      IndexedLabeledCandidates< CandidateClass >* m_indexedCandidates ;
      size_type m_index ;

      // static data members

} ;

template < class CandidateClass >
LabeledCandidate< CandidateClass >* value_type( const MuteWholeCandidateItr< CandidateClass >& ) ;

// inline function definitions

// user include files

//
// forward definitions of inline functions
//

template < class CandidateClass >
inline
IndexedLabeledCandidates< CandidateClass >* MuteWholeCandidateItr< CandidateClass >::indexedCandidates() const
{
   return ( m_indexedCandidates ) ;
}

template < class CandidateClass >
inline
typename MuteWholeCandidateItr<CandidateClass>::size_type MuteWholeCandidateItr< CandidateClass >::index() const
{
   return ( m_index ) ;
}

//
// constructors and destructor
//

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass >::MuteWholeCandidateItr( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) :
   m_indexedCandidates( aOtherItr.indexedCandidates() ) ,
   m_index( aOtherItr.index() )
{
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass >::MuteWholeCandidateItr( IndexedLabeledCandidates< CandidateClass >* aList ,
						    const size_type aIndex ) :
   m_indexedCandidates( (IndexedLabeledCandidates< CandidateClass >*)aList ) ,
   m_index( aIndex )
{   
}

//
// assignment operators
//

template < class CandidateClass >
inline
const MuteWholeCandidateItr< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr )
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
MuteWholeCandidateItr< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator++()
{
   ++m_index ;
   return ( *this ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass > MuteWholeCandidateItr< CandidateClass >::operator++( int )
{
   MuteWholeCandidateItr tmp( *this ) ;
   ++m_index ;
   return ( tmp ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator--()
{
   --m_index ;
   return ( *this ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass > MuteWholeCandidateItr< CandidateClass >::operator--( int )
{
   MuteWholeCandidateItr tmp( *this ) ;
   --m_index ;
   return ( tmp ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator+=( difference_type n )
{
   m_index += n ;
   return( *this ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass > MuteWholeCandidateItr< CandidateClass >::operator+( difference_type n )
{
   MuteWholeCandidateItr tmp( *this ) ;
   return ( tmp += n ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator-=( difference_type n )
{
   m_index -= n ;
   return( *this ) ;
}

template < class CandidateClass >
inline
MuteWholeCandidateItr< CandidateClass > MuteWholeCandidateItr< CandidateClass >::operator-( difference_type n )
{
   MuteWholeCandidateItr tmp( *this ) ;
   return ( tmp -= n ) ;
}

//
// const member functions
//

template < class CandidateClass >
inline
LabeledCandidate< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator*() const
{
   return ( (*m_indexedCandidates).labeledCandidateClass( m_index ) ) ;
}

template < class CandidateClass >
inline
LabeledCandidate< CandidateClass >& MuteWholeCandidateItr< CandidateClass >::operator[]( size_type n ) const
{
   return ( (*m_indexedCandidates).labeledCandidateClass( m_index + n ) ) ;
}

template < class CandidateClass >
inline
typename MuteWholeCandidateItr<CandidateClass>::difference_type MuteWholeCandidateItr< CandidateClass >::operator-( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( m_index - aOtherItr.index() ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator==( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( ( m_index == aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator!=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( ( m_index != aOtherItr.index() ) ||
	    ( m_indexedCandidates != aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator<( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( ( m_index < aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator>( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const 
{
   return ( ( m_index > aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator>=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( ( m_index >= aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
bool MuteWholeCandidateItr< CandidateClass >::operator<=( const MuteWholeCandidateItr< CandidateClass >& aOtherItr ) const
{
   return ( ( m_index <= aOtherItr.index() ) &&
	    ( m_indexedCandidates == aOtherItr.indexedCandidates() ) ) ;
}

template < class CandidateClass >
inline
LabeledCandidate< CandidateClass >* value_type( const MuteWholeCandidateItr< CandidateClass >& )
{
   return ( (CandidateClass*)0 ) ;
}

}

#endif /* DCHAIN_MUTEWHOLECANDIDATEITR_H */
