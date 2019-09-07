#ifndef DCHAIN_COMBINATORICLIST_H
#define DCHAIN_COMBINATORICLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CombinatoricList
// 
// Description: List to hold a ConjugateList of Candidates built
//		  from DecayClass Class.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: CombinatoricList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CombinatoricList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/02/04 17:31:10  wsun
// Made CombinatoricList::label() public instead of protected.
//
// Revision 1.1  2006/01/11 20:28:21  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

//...
template< class CandidateClass >
class DCCandidateTraits {
   public:
      typedef typename CandidateClass::DecayEvidence DecayClass;
};

// user include files

#include "DecayChain/List/DecayList.h" // superclass declaration
#include "DecayChain/List/LabeledCandidateList.h"

// forward declarations
//template< class DecayClass , class CandidateClass > class DCDecayList ; //for backwards compatibility

template < class Arg > class DCAnalysisFunction ;

#include <vector>

#ifndef _combinatoriclist_vector_
// The following CPP macro is used to avoid having to typedef
// a map in the class.  Why? Because things can get messy, at
// least in the g++ compiler as the typedef can cause instantiation
// when not all the necessary information is available!
#define _combinatoriclist_vector_ std::vector<const ConjugateList< CandidateClass >* >
#endif

namespace dchain {
template< class CandidateClass> 
class CombinatoricList;

template< class CandidateClass> 
class ConjugateList;

template<  class CandidateClass>
CombinatoricList< CandidateClass > operator*(
   const ConjugateList< CandidateClass >& lhs ,
   const ConjugateList< CandidateClass >& rhs ) ;

template<  class CandidateClass>
CombinatoricList< CandidateClass > operator*(
   const CombinatoricList< CandidateClass >& lhs ,
   const ConjugateList< CandidateClass >& rhs ) ;

template<  class CandidateClass>
CombinatoricList< CandidateClass > operator*(
   const ConjugateList< CandidateClass >& lhs ,
   const CombinatoricList< CandidateClass >& rhs ) ;

template<  class CandidateClass>
CombinatoricList< CandidateClass > operator*(
   const CombinatoricList< CandidateClass >& lhs ,
   const CombinatoricList< CandidateClass >& rhs ) ;

//
//  Note: To avoid a myriad of conpiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class CombinatoricList< A , A::CandidateClass > ;

template < class CandidateClass >
class CombinatoricList : public dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >
{
   public:
      typedef typename DCCandidateTraits<CandidateClass>::DecayClass DecayClass;
   private:
      // friend classses and functions
// This class is a friend so that it can use the protected constructor
//   that takes a ConjugateList as its argument
      friend class dchain::DecayList< DecayClass , CandidateClass > ;
      //friend class DCDecayList< DecayClass , CandidateClass > ; //for backwards compatibility

// These are `friends' as these are the only way a CombinatoricList
//   should be created, i.e. it need access to the private constructors
      friend CombinatoricList< CandidateClass > operator* <CandidateClass> (
         const ConjugateList< CandidateClass >& lhs ,
         const ConjugateList< CandidateClass >& rhs ) ;
      friend CombinatoricList< CandidateClass > operator* <CandidateClass> (
         const CombinatoricList< CandidateClass >& lhs ,
         const ConjugateList< CandidateClass >& rhs ) ;
      friend CombinatoricList< CandidateClass > operator* <CandidateClass> (
         const ConjugateList< CandidateClass >& lhs ,
         const CombinatoricList< CandidateClass >& rhs ) ;
      friend CombinatoricList< CandidateClass > operator* <CandidateClass> (
         const CombinatoricList< CandidateClass >& lhs ,
         const CombinatoricList< CandidateClass >& rhs ) ;

   public:
      // constants, enums and typedefs
      typedef typename dchain::DecayList< DecayClass , CandidateClass >::iterator iterator ;
      typedef typename dchain::DecayList< DecayClass , CandidateClass >::const_iterator const_iterator ;
      typedef typename dchain::DecayList< DecayClass , CandidateClass >::const_partial_iterator const_partial_iterator ;

      // Constructors and destructor
      virtual ~CombinatoricList() ;

      // used by Decay<> for single product DecayClass trees
      CombinatoricList( const ConjugateList< CandidateClass >& aList ) ;

      virtual conjugation::Label label() const ;

      // member functions
      virtual typename dchain::CandidateList< CandidateClass >::iterator begin() ;
      virtual typename dchain::CandidateList< CandidateClass >::iterator end() ;
      virtual typename dchain::DecayList< DecayClass , CandidateClass >::iterator particle_begin() ;
      virtual typename dchain::DecayList< DecayClass , CandidateClass >::iterator particle_end() ;

      // const member functions
      virtual const dchain::DecayList< DecayClass , CandidateClass > bar() const ;
      virtual const dchain::DecayList< DecayClass , CandidateClass >& operator()() const ;
      virtual bool isSelfConjugate() const ;

      virtual typename dchain::CandidateList< CandidateClass >::const_iterator begin() const ;
      virtual typename dchain::CandidateList< CandidateClass >::const_iterator end() const ;
      virtual typename dchain::CandidateList< CandidateClass >::const_partial_iterator
      partial_begin() const ;
      virtual typename dchain::CandidateList< CandidateClass >::const_partial_iterator
      partial_end() const ;
      virtual const_iterator particle_begin() const ;
      virtual const_iterator particle_end() const ;
      virtual const_partial_iterator partial_particle_begin() const ;
      virtual const_partial_iterator partial_particle_end() const ;

      using ConjugateList< CandidateClass >::iterate;
      template<class TAnalyze>
      void iterate( const TAnalyze& analyze) const {
	 fill();
	 dchain::DecayList<DecayClass,CandidateClass>::iterate(analyze);
      }

      template < class TAnalyzer>
      void conjugation_iterate( const TAnalyzer& analyze) const 
      {
	 fill();
	 dchain::DecayList<DecayClass,CandidateClass>::conjugation_iterate(analyze);
      }

      template<class TAnalyze>
      void partial_iterate( TAnalyze& analyze ) const
      {
	 fill();
	 dchain::DecayList<DecayClass,CandidateClass>::partial_iterate(analyze);
      }

      // static member functions

   protected:
      // Constructors and destructor
      // used by operator* functions to build up CombinatoricLists
      CombinatoricList(
         const ConjugateList< CandidateClass >& lhs ,
         const ConjugateList< CandidateClass >& rhs ) ;
      CombinatoricList(
         const CombinatoricList< CandidateClass >& lhs ,
         const ConjugateList< CandidateClass >& rhs ) ;
      CombinatoricList(
         const ConjugateList< CandidateClass >& lhs ,
         const CombinatoricList< CandidateClass >& rhs ) ;
      CombinatoricList(
         const CombinatoricList< CandidateClass >& lhs ,
         const CombinatoricList< CandidateClass >& rhs ) ;

   protected:
      // protected member functions
      virtual dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() ;

      // protected const member functions
      virtual const dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() const ;

   private:
      // Constructors and destructor
     CombinatoricList( void ) ; // stop default

   public:
     CombinatoricList(
        const CombinatoricList< CandidateClass >& aOtherList ) ;

   private:

      // stop default

      // assignment operator(s)
      const CombinatoricList< CandidateClass >& operator=(
         const CombinatoricList< CandidateClass >& aOtherList ) ;
      // stop default

      // private member functions

      // private const member functions
      void determineConjugation() const ;
      void fill() const ;

      // data members
      _combinatoriclist_vector_& m_listOfLists ;
      mutable bool m_listFilled ;
      bool m_conjugationKnown ;

      // static data members

} ;

// inline function definitions

template<  class CandidateClass> 
inline
CombinatoricList< CandidateClass >
operator*( const ConjugateList< CandidateClass >& lhs ,
	   const ConjugateList< CandidateClass >& rhs ) 
{
   return CombinatoricList<CandidateClass>( lhs, rhs); 
}

template<  class CandidateClass>
inline 
CombinatoricList< CandidateClass >
operator*( const CombinatoricList< CandidateClass >& lhs ,
	   const ConjugateList< CandidateClass >& rhs ) 
{
   return CombinatoricList<CandidateClass>( lhs, rhs); 
}

template<  class CandidateClass> 
inline
CombinatoricList< CandidateClass >
operator*( const ConjugateList< CandidateClass >& lhs ,
	   const CombinatoricList< CandidateClass >& rhs ) 
{
   return CombinatoricList<CandidateClass>( lhs, rhs);
}

template<  class CandidateClass> 
inline
CombinatoricList< CandidateClass >
operator*( const CombinatoricList< CandidateClass >& lhs ,
	   const CombinatoricList< CandidateClass >& rhs ) 
{
   return CombinatoricList<CandidateClass>( lhs, rhs);
}
}

// function definitions
# include "DecayChain/List/Template/CombinatoricList.cc"

#endif /* DCHAIN_COMBINATORICLIST_H */
