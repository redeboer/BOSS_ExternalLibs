#ifndef DCHAIN_PARTIALITR_H
#define DCHAIN_PARTIALITR_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      PartialItr
// 
// Description: a Iterator through one label of a LabeledList.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:51:25 EDT 1996
// $Id: PartialItr.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: PartialItr.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:18  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:50  cdj
// imported DChain
//
// Revision 1.12  1998/08/28 05:16:08  sjp
// Updated to use IndexedLabeled classes
//
// Revision 1.11  1998/08/21 00:53:59  sjp
// fixed bad forward declaration
//
// Revision 1.10  1997/08/28 06:59:27  sjp
// Modified files to handle complete templating
//
// Revision 1.9  1997/08/26 04:13:26  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.8  1997/08/19 23:01:59  sjp
// Restructured package to be independent of Rock
//
// Revision 1.7  1997/08/15 21:32:36  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.6  1997/01/31 20:17:12  sjp
// Modified to use the new `bug' include files
//
// Revision 1.5  1997/01/22 16:27:31  sjp
// Fixed error in pre-processor labels
//
// Revision 1.4  1997/01/21 20:34:04  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1996/12/31 17:46:42  sjp
// Replaced ptrdiff with member type distance
//
// Revision 1.2  1996/12/20 21:19:21  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:10  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h" // enumarator
#include "DecayChain/Iterator/PartialCandidateItr.h" // superclass
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/List/IndexedLabeledParticles.h"


// forward declarations
namespace dchain {
   template < class T > class LabeledParticle ;
   template < class T , class CandidateClass > class LabeledParticleList ;
   //template < class T , class CandidateClass > class IndexedLabeledParticles ;
//
// Note : some functions are inlines in the declaration becuase too many
//	    compiler bugs make it impractical to have definition outside
//	    the delcaration!  This should be reviewed in the future
//

template < class T >
class PartialItr : public PartialCandidateItr< typename T::CandidateClass >
{
   // friend classses and functions
   friend class LabeledParticleList< T , typename T::CandidateClass > ;

   public:
   // constants, enums and typedefs

   // Constructors and destructor
   PartialItr( const PartialItr< T >& aOtherItr ) ;
   //virtual ~PartialItr() ;

   // assignment operator(s)
   const PartialItr< T >& operator=( const PartialItr< T >& aOtherItr ) ;

   // member functions
   PartialItr< T >& operator++() ;
   PartialItr< T > operator++( int ) ;
   PartialItr< T >& operator--() ;
   PartialItr< T > operator--( int ) ;

   // const member functions
   const LabeledParticle< T >& operator*() const ;

   // static member functions

   protected:
   // Constructors and destructor
   PartialItr( const IndexedLabeledParticles< T , typename T::CandidateClass >* aList ,
	 const typename candidateitr::size_type aIndex ,
	 const typename conjugation::Label aLabel ) :
      PartialCandidateItr< typename T::CandidateClass >( aList ,
	    aIndex ,
	    aLabel ) {}

   // protected member functions

   // protected const member functions

   private:
   // Constructors and destructor
   PartialItr() ; // stop default

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
PartialItr< T >::PartialItr( const PartialItr< T >& aOtherItr ) :
   PartialCandidateItr< typename T::CandidateClass >( aOtherItr )
{
}

//
// assignment operators
//

template < class T >
inline
const PartialItr< T >& PartialItr< T >::operator=( const PartialItr< T >& aOtherItr )
{
   PartialCandidateItr< typename T::CandidateClass >::operator=( aOtherItr ) ;
   return ( *this ) ;
}

//
// member functions
//

template < class T >
inline
PartialItr< T >& PartialItr< T >::operator++()
{
   PartialCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( *this ) ;
}

template < class T >
inline
PartialItr< T > PartialItr< T >::operator++( int )
{
   PartialItr< T > tmp( *this ) ;
   PartialCandidateItr< typename T::CandidateClass >::operator++() ;
   return ( tmp ) ;
}

template < class T >
inline
PartialItr< T >& PartialItr< T >::operator--()
{
   PartialCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( *this ) ;
}

template < class T >
inline
PartialItr< T > PartialItr< T >::operator--( int )
{
   PartialItr< T > tmp( *this ) ;
   PartialCandidateItr< typename T::CandidateClass >::operator--() ;
   return ( tmp ) ;
}

//
// const member functions
//

template < class T >
inline
const LabeledParticle< T >& PartialItr< T >::operator*() const
{
   return ( ( *(IndexedLabeledParticles< T , typename T::CandidateClass >*)(this->indexedCandidates()) ).labeledParticleClass( this->index() ) ) ;
}
}

#endif /* DCHAIN_PARTIALITR_H */
