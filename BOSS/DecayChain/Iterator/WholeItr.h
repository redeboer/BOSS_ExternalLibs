#ifndef DCHAIN_WHOLEITR_H
#define DCHAIN_WHOLEITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      WholeItr
// 
// Description: a Iterator though whole of LabeledDecayList
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: WholeItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: WholeItr.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:19  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:51  cdj
// imported DChain
//
// Revision 1.11  1998/08/28 05:16:09  sjp
// Updated to use IndexedLabeled classes
//
// Revision 1.10  1997/08/28 06:59:30  sjp
// Modified files to handle complete templating
//
// Revision 1.9  1997/08/26 04:13:34  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.8  1997/08/19 23:02:03  sjp
// Restructured package to be independent of Rock
//
// Revision 1.7  1997/08/15 21:32:43  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.6  1997/01/31 20:17:19  sjp
// Modified to use the new `bug' include files
//
// Revision 1.5  1997/01/22 16:27:35  sjp
// Fixed error in pre-processor labels
//
// Revision 1.4  1997/01/21 20:34:11  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1996/12/31 17:46:45  sjp
// Replaced ptrdiff with member type distance
//
// Revision 1.2  1996/12/20 21:19:28  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:16  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Iterator/WholeCandidateItr.h" // superclass
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/List/IndexedLabeledParticles.h"

// forward declarations
namespace dchain {
//template < class T > class LabeledParticle ;
//template < class T , class CandidateClass > class IndexedLabeledParticles ;
template < class T , class CandidateClass > class LabeledParticleList ;

//
// Note : some functions are inlines in the declaration becuase too many
//	    compiler bugs make it impractical to have definition outside
//	    the delcaration!  This should be reviewed in the future
//

template < class T >
class WholeItr : public WholeCandidateItr< typename T::CandidateClass >
{
   // friend classses and functions
   friend class LabeledParticleList< T , typename T::CandidateClass > ;

   public:
   // constants, enums and typedefs
   typedef WholeCandidateItr<typename T::CandidateClass> ParentType;
   typedef typename ParentType::difference_type difference_type;
   typedef typename ParentType::size_type size_type;

   // Constructors and destructor
   WholeItr( const WholeItr< T >& aOtherItr ) ;
   //virtual ~WholeItr() ;

   // assignment operator(s)
   const WholeItr< T >& operator=( const WholeItr< T >& aOtherItr ) ;

   // member functions
   WholeItr< T >& operator++() ;
   WholeItr< T > operator++( int ) ;
   WholeItr< T >& operator--() ;
   WholeItr< T > operator--( int ) ;
   WholeItr< T >& operator+=( difference_type n )
   {
      WholeCandidateItr< typename T::CandidateClass >::operator+=( n ) ;
      return( *this ) ;
   }
   WholeItr< T > operator+( difference_type n )
   {
      WholeItr< T > tmp( *this ) ;
      return ( tmp += n ) ;
   }
   WholeItr< T >& operator-=( difference_type n )
   {
      WholeCandidateItr< typename T::CandidateClass >::operator-=( n ) ;
      return( *this ) ;
   }
   WholeItr< T > operator-( difference_type n )
   {
      WholeItr< T > tmp( *this ) ;
      return ( tmp -= n ) ;
   }

   // const member functions
   const LabeledParticle< T >& operator*() const ;
   const LabeledParticle< T >& operator[]( size_type n ) const
   {
      return ( ( *(IndexedLabeledParticles< T , typename T::CandidateClass >*)(this->indexedCandidates()) ).labeledParticleClass( this->index() + n ) ) ;
   }
   difference_type operator-( const WholeItr< T >& aOtherItr ) const
   {
      return ( this->index() - aOtherItr.index() ) ;
   }

   // static member functions

   protected:
   // Constructors and destructor
   WholeItr( const IndexedLabeledParticles< T , typename T::CandidateClass >* aList ,
	 const size_type aIndex ) :
      WholeCandidateItr< typename T::CandidateClass >( aList ,
	    aIndex ) {}

   // protected member functions

   // protected const member functions

   private:
   // Constructors and destructor
   WholeItr() ; // stop default

   // private member functions

   // private const member functions

   // data members

   // static data members

} ;

// inline function definitions

// user include files

//
// constructors and destructor
//

template < class T >
inline
WholeItr< T >::WholeItr( const WholeItr< T >& aOtherItr ) :
   WholeCandidateItr< typename T::CandidateClass >( aOtherItr )
{
}

//
// assignment operators
//

template < class T >
inline
const WholeItr< T >& WholeItr< T >::operator=( const WholeItr< T >& aOtherItr )
{
   WholeCandidateItr< typename T::CandidateClass >::operator=( aOtherItr ) ;
   return ( *this ) ;
}

//
// member functions
//

template < class T >
inline
WholeItr< T >& WholeItr< T >::operator++()
{
   WholeCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( *this ) ;
}

template < class T >
inline
WholeItr< T > WholeItr< T >::operator++( int )
{
   WholeItr< T > tmp( *this ) ;
   WholeCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( tmp ) ;
}

template < class T >
inline
WholeItr< T >& WholeItr< T >::operator--()
{
   WholeCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( *this ) ;
}

template < class T >
inline
WholeItr< T > WholeItr< T >::operator--( int )
{
   WholeItr< T > tmp( *this ) ;
   WholeCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( tmp ) ;
}

//
// const member functions
//

template < class T >
inline
const LabeledParticle< T >& WholeItr< T >::operator*() const
{
   return ( ( *(IndexedLabeledParticles< T , typename T::CandidateClass >*)(this->indexedCandidates()) ).labeledParticleClass( this->index() ) ) ;
}
}


#endif /* DCHAIN_WHOLEITR_H */
