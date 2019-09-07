#ifndef DCHAIN_DECAYLIST_H
#define DCHAIN_DECAYLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      DecayList
// 
// Description: container of subclasses of Decay, used for iteration
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DecayList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DecayList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/04/09 13:32:10  cdj
// added back missing operator+
//
// Revision 1.1  2006/01/11 20:28:27  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files
#include <assert.h>
#include <iostream>

// user include files
#include "DecayChain/List/ConjugateList.h" // superclass declaration
#include "DecayChain/List/NeutralVisibleList.h"
#include "DecayChain/List/LabeledParticleList.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Element/MutableReferenceHolder.h"
#include "DecayChain/Function/SelectAll.h"
#include "DecayChain/List/FillListWithSelection.h"

namespace dchain {
   template < class DecayClass , class CandidateClass > class LabeledParticleList ;template < class DecayClass > class MuteWholeItr ;
   template < class DecayClass > class WholeItr ;
   template < class DecayClass > class PartialItr ;
}

namespace dchain {
   template < class CandidateClass > class CombinatoricList ;
   template < class DecayClass , class CandidateClass > class DecayList;

template < class DecayClass , class CandidateClass >
DecayList< DecayClass, CandidateClass > operator+(
   const DecayList< DecayClass, CandidateClass >& iList1,
   const DecayList< DecayClass, CandidateClass >& iList2 );
template < class DecayClass , class CandidateClass, class NeutralClass >
DecayList< DecayClass, CandidateClass > operator+(
   const DecayList< DecayClass, CandidateClass >& iList1,
   const NeutralVisibleList< NeutralClass >& iList2 );
template < class DecayClass , class CandidateClass, class NeutralClass >
DecayList< DecayClass, CandidateClass > operator+(
   const NeutralVisibleList< NeutralClass >& iList1 ,
   const DecayList< DecayClass, CandidateClass >& iList2);
//
//  Note: To avoid a myriad of conpiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class DCDecayList< A , A::CandidateClass > ;

template < class DecayClass , class CandidateClass = typename DecayClass::CandidateClass >
class DecayList : public ConjugateList< CandidateClass >
{
   // friend classses and functions
   friend DecayList< DecayClass, CandidateClass > operator+ <DecayClass, CandidateClass> (
	 const DecayList< DecayClass, CandidateClass >& iList1,
	 const DecayList< DecayClass, CandidateClass >& iList2 );

   public:
   // constants, enums and typedefs
   typedef MuteWholeItr< DecayClass > iterator ;
   typedef WholeItr< DecayClass > const_iterator  ;
   typedef PartialItr< DecayClass > const_partial_iterator  ;

   typedef DecayClass value_type ;

   typedef DecayList< DecayClass , CandidateClass > self_type;

   // Constructors and destructor
   DecayList();
   // need this function to stop default copy constructor
   DecayList( const self_type& aOtherList);
   virtual ~DecayList() ;

   // assignment operator(s)
   // need this function to stop default assignment operator
   template< class TDecay>
      const self_type& operator=( const DecayList< TDecay , CandidateClass >& aOtherList ) {
	 m_list->erase( m_list->begin() ,
	       m_list->end() ) ;
	 fill( aOtherList, SelectAll<DecayClass>() ) ;
	 return ( *this ) ;
      }

   const self_type& operator=( const CombinatoricList< CandidateClass >& aOtherList );

   const self_type& operator=( const DecayList< DecayClass , CandidateClass >& aOtherList );
   //
   template<class TList>
      void operator+=( const TList& aOtherList) {

	 if(this->size() != 0 ) {
	    //only check compatable conjugation if something already in
	    // this list, otherwise could have problem with default value
	    bool can_only_add_lists_with_same_conjugation =
	       this->isSelfConjugate()== aOtherList.isSelfConjugate();
	    assert(can_only_add_lists_with_same_conjugation);
	 }
	 fill(aOtherList);
      }

   template<class TSelector>
      FillListWithSelection<self_type, TSelector> operator[](const TSelector& iSel) {
	 return FillListWithSelection<self_type,TSelector>(*this, iSel);
      }

   // member functions
   virtual iterator particle_begin() ;
   virtual iterator particle_end() ;

   // const member functions
   const DecayList< DecayClass , CandidateClass > bar() const ;
   const DecayList< DecayClass , CandidateClass >& operator()() const ;
   virtual const_iterator particle_begin() const ;
   virtual const_iterator particle_end() const ;
   virtual const_partial_iterator partial_particle_begin() const ;
   virtual const_partial_iterator partial_particle_end() const ;
   // The for following redeclarations are required because of function hidding
   //   ( See ARM p310 )
   using ConjugateList< CandidateClass >::iterate;
   template<class TAnalyze>
      void iterate( const TAnalyze& analyze) const {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry).particle() ) ;
	 }
      }

   template < class TAnalyzer>
      void conjugation_iterate( const TAnalyzer& analyzer) const 
      {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() , (*entry).label() == this->label() ) ;
	 }
      }

   template<class TAnalyze>
      void iterate( TAnalyze& analyze) const {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry).particle() ) ;
	 }
      }

   using ConjugateList< CandidateClass >::iterate;
   template<class TAnalyze>
      void partial_iterate( TAnalyze& analyze ) const
      {
	 const_partial_iterator finished ( partial_particle_end() ) ;
	 for ( const_partial_iterator entry ( partial_particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry).particle() ) ;
	 }
      }
   void eraseContents() ;

   void erase() {
      eraseContents();
   }
   // static member functions

   DecayList(
	 LabeledParticleList< DecayClass , CandidateClass >& aLabeledDecayList ,
	 conjugation::Label aLabel ) ;
   protected:

   // protected member functions
   virtual LabeledCandidateList< CandidateClass >& labeledCandidateList() ;
   virtual LabeledParticleList< DecayClass , CandidateClass >& labeledParticleList() ;

   // protected const member functions
   virtual const LabeledCandidateList< CandidateClass >& labeledCandidateList() const ;
   virtual const LabeledParticleList< DecayClass , CandidateClass >& labeledParticleList() const ;
   public:
   template <class TSelect>
      bool attempt_insert( DecayClass* pPossible ,
	    const conjugation::Label aLabel,
	    const TSelect& aSelect ) {
	 ReferenceHolder<DecayClass> pHolder( pPossible );
	 bool returnValue = aSelect(*pPossible);
	 if (returnValue) {
	    this->insert(pPossible,aLabel);
	 }
	 return returnValue;
      }

   template <class THolder>
      void insert(THolder& pPossible,
	    const conjugation::Label aLabel) {
	 m_list->push_back( LabeledParticle< DecayClass >( pPossible ,
		  aLabel ) ) ;
      }
   // Constructors and destructor

   // private member functions
   template<class TDecayList, class TSelector>
      void fill( const TDecayList& aList, const TSelector& aSel = SelectAll<DecayClass>() ) {
	 //std::cout << "@DecayList::fill()" << std::endl;
	 setLabel( aList.label() ) ;
	 typename TDecayList::const_iterator finished( aList.particle_end() ) ;
	 for ( typename TDecayList::const_iterator entry( aList.particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    //std::cout << "@ DecayList::fill()" << std::endl;
	    DecayClass* localCopy = new DecayClass( (*entry).particle() ) ;
	    attempt_insert( localCopy ,
		  (*entry).label(),
		  aSel ) ;
	 }
      }

   private:

   // private const member functions

   // data members
   MutableReferenceHolder<LabeledParticleList< DecayClass , CandidateClass > > m_list ;
} ;

// inline function definitions



#include <assert.h>

template < class DecayClass , class CandidateClass >
inline
DecayList< DecayClass, CandidateClass > operator+(
   const DecayList< DecayClass, CandidateClass >& iList1,
   const DecayList< DecayClass, CandidateClass >& iList2 )
{
   bool can_only_add_lists_with_same_conjugation =
      iList1.isSelfConjugate()== iList2.isSelfConjugate();
   assert(can_only_add_lists_with_same_conjugation);

   DecayList< DecayClass, CandidateClass> returnValue( iList1 );
   if( &iList1 != &iList2 ) {
      returnValue.fill( iList2, SelectAll<DecayClass>() );
   }
   return returnValue;
}

template < class DecayClass , class CandidateClass, class NeutralClass >
inline
DecayList< DecayClass, CandidateClass > operator+(
   const DecayList< DecayClass, CandidateClass >& iList1,
   const NeutralVisibleList< NeutralClass >& iList2 )
{
   bool can_only_add_lists_with_same_conjugation =
      iList1.isSelfConjugate()== iList2.isSelfConjugate();
   assert(can_only_add_lists_with_same_conjugation);

   
   DecayList< DecayClass, CandidateClass> returnValue =iList1 ;
   typename LabeledParticleList< NeutralClass , CandidateClass >::const_iterator finished( iList2.particle_end() ) ;
   for( typename LabeledParticleList< NeutralClass , CandidateClass >::const_iterator entry(iList2.particle_begin()) ;
	entry != finished ;
	++entry ){
     NeutralClass* localCopy = new NeutralClass( (*entry).particle() ) ;
     returnValue.attempt_insert( localCopy, (*entry).label(),SelectAll<DecayClass>() ) ;
   }
   return returnValue;
}


template < class DecayClass , class CandidateClass, class NeutralClass >
inline
DecayList< DecayClass, CandidateClass > operator+(
   const NeutralVisibleList< NeutralClass >& iList1 ,
   const DecayList< DecayClass, CandidateClass >& iList2)
{
     DecayList< DecayClass, CandidateClass> returnValue;
     returnValue = iList2 + iList1;
     return returnValue;
}

}
   
// function definitions
#include "DecayChain/List/Template/DecayList.cc"

#endif /* DCHAIN_DECAYLIST_H */
