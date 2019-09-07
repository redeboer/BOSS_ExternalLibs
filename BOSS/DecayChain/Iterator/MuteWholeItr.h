#ifndef DCHAIN_MUTEWHOLEITR_H
#define DCHAIN_MUTEWHOLEITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      MuteWholeItr
// 
// Description: Itr through a entire LabeledList.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: MuteWholeItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: MuteWholeItr.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:17  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/Iterator/MuteWholeCandidateItr.h" // superclass
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/List/IndexedLabeledParticles.h"


// forward declarations
namespace dchain {
   template <class T> class LabeledParticle;
   template <class T, class CandidateClass> class LabeledParticleList;

template < class T , class CandidateClass > class IndexedLabeledParticles ;

//
// Note : some functions are inlines in the declaration becuase too many
//	    compiler bugs make it impractical to have definition outside
//	    the delcaration!  This should be reviewed in the future
//

template < class T >
class MuteWholeItr : public MuteWholeCandidateItr< typename T::CandidateClass >
{
      // friend classses and functions
      friend class LabeledParticleList< T , typename T::CandidateClass > ;

   public:
      // constants, enums and typedefs
      typedef MuteWholeCandidateItr<typename T::CandidateClass> ParentType;
      typedef typename ParentType::difference_type difference_type;
      typedef typename ParentType::size_type size_type;

      // Constructors and destructor
      MuteWholeItr( const MuteWholeItr< T >& aOtherItr ) ;
      //virtual ~MuteWholeItr() ;

      // assignment operator(s)
      const MuteWholeItr< T >& operator=( const MuteWholeItr< T >& aOtherItr ) ;

      // member functions
      MuteWholeItr< T >& operator++() ;
      MuteWholeItr< T > operator++( int ) ;
      MuteWholeItr< T >& operator--() ;
      MuteWholeItr< T > operator--( int ) ;
      MuteWholeItr< T >& operator+=( difference_type n )
{
   MuteWholeCandidateItr< typename T::CandidateClass >::operator+=( n ) ;
   return( *this ) ;
}
      MuteWholeItr< T > operator+( difference_type n )
{
   MuteWholeItr< T > tmp( *this ) ;
   return ( tmp += n ) ;
}
      MuteWholeItr< T >& operator-=( difference_type n )
{
   MuteWholeCandidateItr< typename T::CandidateClass >::operator-=( n ) ;
   return( *this ) ;
}
      MuteWholeItr< T > operator-( difference_type n )
{
   MuteWholeItr< T > tmp( *this ) ;
   return ( tmp -= n ) ;
}

      // const member functions
      LabeledParticle< T >& operator*() const ;
      LabeledParticle< T >& operator[]( size_type n ) const
{
   return ( ( *(IndexedLabeledParticles< T , typename T::CandidateClass >*)(this->indexedCandidates()) ).labeledParticleClass( this->index() + n ) ) ;
}
      difference_type operator-( const MuteWholeItr< T >& aOtherItr ) const
{
   return ( this->index() - aOtherItr.index() ) ;
}

      // static member functions

   protected:
      // Constructors and destructor
      MuteWholeItr( IndexedLabeledParticles< T , typename T::CandidateClass >* aList ,
		    const size_type aIndex ) :
	 MuteWholeCandidateItr< typename T::CandidateClass >( aList ,
						    aIndex ) {}

      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      MuteWholeItr() ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

} ;

// inline function definitions

//
// constructors and destructor
//

template < class T >
inline
MuteWholeItr< T >::MuteWholeItr( const MuteWholeItr< T >& aOtherItr ) :
   MuteWholeCandidateItr< typename T::CandidateClass >( aOtherItr )
{
}

//
// assignment operators
//

template < class T >
inline
const MuteWholeItr< T >& MuteWholeItr< T >::operator=( const MuteWholeItr< T >& aOtherItr )
{
   MuteWholeCandidateItr< typename T::CandidateClass >::operator=( aOtherItr ) ;
   return ( *this ) ;
}

//
// member functions
//

template < class T >
inline
MuteWholeItr< T >& MuteWholeItr< T >::operator++()
{
   MuteWholeCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( *this ) ;
}

template < class T >
inline
MuteWholeItr< T > MuteWholeItr< T >::operator++( int )
{
   MuteWholeItr< T > tmp( *this ) ;
   MuteWholeCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( tmp ) ;
}

template < class T >
inline
MuteWholeItr< T >& MuteWholeItr< T >::operator--()
{
   MuteWholeCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( *this ) ;
}

template < class T >
inline
MuteWholeItr< T > MuteWholeItr< T >::operator--( int )
{
   MuteWholeItr< T > tmp( *this ) ;
   MuteWholeCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( tmp ) ;
}

//
// const member functions
//

template < class T >
inline
LabeledParticle< T >& MuteWholeItr< T >::operator*() const
{
   return ( ( *(IndexedLabeledParticles< T , typename T::CandidateClass >*)(this->indexedCandidates()) ).labeledParticleClass( this->index() ) ) ;
}
}

#endif /* DCHAIN_WHOLEITR_H */
