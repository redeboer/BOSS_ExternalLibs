#ifndef DCHAIN_NEUTRALNCVISIBLELIST_H
#define DCHAIN_NEUTRALNCVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralNCVisibleList
// 
// Description: List to hold Candidate subclasses generated from VisibleEvidence
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: NeutralNCVisibleList.h,v 1.2 2009/06/22 08:38:08 zoujh Exp $
//
// Revision history
//
// $Log: NeutralNCVisibleList.h,v $
// Revision 1.2  2009/06/22 08:38:08  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:29  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/List/CandidateList.h" // superclass declaration
#include "DecayChain/List/ChosenConjugateList.h" // for return-by-value
#include "DecayChain/Element/MutableReferenceHolder.h"

#include "DecayChain/List/LabeledParticleList.h"

#include "DecayChain/Element/MakerTrait.h"
#include "DecayChain/Iterator/SelectiveInserter.h"
#include "DecayChain/Function/SelectAll.h"
#include "DecayChain/List/FillListWithSelection.h"

namespace dchain {
template < class DecayClass > class MuteWholeItr ;
template < class DecayClass > class WholeItr ;
template < class DecayClass > class PartialItr ;


   template<class T>
   int is_bar(const T& iNeutralNC) {
      return static_cast<int>(iNeutralNC.isBar());
   }

   template<class T>
   int is_bar(const T* iNeutralNC) {
      return static_cast<int>(iNeutralNC->isBar());
   }

   template<class T>
   int is_bar(T* iNeutralNC) {
      return static_cast<int>(iNeutralNC->isBar());
   }

   template<class TPtr,class TRef>
   int is_bar(const ReferenceHolderBase<TPtr,TRef>& iHolder) {
      return is_bar(*iHolder);
   }

//
//  Note: To avoid a myriad of compiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class dchain::LabeledParticleList< A , A::CandidateClass > ;

template < class NeutralNC , class CandidateClass = typename NeutralNC::CandidateClass >
class NeutralNCVisibleList : public dchain::CandidateList< CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeItr< NeutralNC > iterator ;
      typedef dchain::WholeItr< NeutralNC > const_iterator ;
      typedef dchain::PartialItr< NeutralNC > const_partial_iterator ;

      typedef NeutralNC value_type ;

      typedef NeutralNCVisibleList<NeutralNC, CandidateClass> self_type;

      // Constructors and destructor
      NeutralNCVisibleList();
      NeutralNCVisibleList( const self_type& aOtherList);
      template< class TSelector>
      NeutralNCVisibleList( const self_type& aOtherList,
			    const TSelector& aSel):
	 m_list( new LabeledParticleList<NeutralNC,CandidateClass> ),
	 m_ntBarList(0),
	 m_isBarList(0)
      {
	 fill(*(aOtherList.m_list),
	      typename MakerTrait<NeutralNC>::maker_type(), 
	      aSel);
      }

      virtual ~NeutralNCVisibleList() ;
      
      // assignment operator(s)
      //const NeutralNCVisibleList< NeutralNC , CandidateClass >& operator=( const NeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList ) ;

      template<class Witness>
      const self_type& operator=(const Witness& aWitnessList) {
	 eraseContents();
	 fill(aWitnessList.begin(), aWitnessList.end(),
	      typename MakerTrait<NeutralNC>::maker_type(),
	      SelectAll<NeutralNC>() );
	 return ( *this ) ;
      }

      // member functions
      template<class TSelector>
      FillListWithSelection<self_type, TSelector> operator[](const TSelector& iSel) {
	 return FillListWithSelection<self_type,TSelector>(*this, iSel);
      }

      iterator particle_begin() ;
      iterator particle_end() ;
      ChosenConjugateList< NeutralNC , CandidateClass >& bar() ;
      ChosenConjugateList< NeutralNC , CandidateClass >& operator()() ;

      // const member functions
      const_iterator particle_begin() const ;
      const_iterator particle_end() const ;

// The two following redeclarations are required because of function hidding
//   ( See ARM p310 )


      using CandidateList< CandidateClass >::iterate;
      template<class TAnalyze>
      void iterate( const TAnalyze& analyze) const {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() ) ;
	 }
      }

      template<class TAnalyze>
      void iterate( TAnalyze& analyze) const {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() ) ;
	 }
      }

      ///creates an insert iterator that filters based on iSelect
      template<class TSelect>
      SelectiveInserter<self_type,TSelect> selectiveInserter(const TSelect& iSelect) {
	 return SelectiveInserter<self_type,TSelect>(*this,iSelect); }

      template <class THolder, class TSelect>
      bool attempt_insert( THolder& pPossible,
			   const TSelect& aSelect) {
	 bool returnValue = (aSelect)(*pPossible);
	 if (returnValue) {
	    this->insert(pPossible);
	 }
	 return returnValue;
      }

      template <class THolder>
      void insert(THolder& pPossible) {
	 if (is_bar(pPossible)) {
	    m_list->push_back( LabeledParticle< NeutralNC >( pPossible ,
							     kIsBar ) ) ;
	 }
	 else {
	    m_list->push_back( LabeledParticle< NeutralNC >( pPossible ,
							     kIsntBar ) ) ;
	 }
      }

      void eraseContents() ;
      void erase() {
	 eraseContents();
      }

      template<class TMaker, class TSelector>
      void fill( const LabeledParticleList< NeutralNC , CandidateClass >& aList,
		 const TMaker& aMaker,
		 const TSelector& aSel) {

	 for( const_iterator entry(aList.begin()); entry != aList.end(); ++entry){
	    //NOTE: even though this is really just calling the copy constructor
	    //it is best to use the maker since it allows further control
	    const NeutralNC& from((*entry)());
	    typename ResultTypeTrait<TMaker>::type localCopy( aMaker(from) );
	    
	    attempt_insert(localCopy,aSel);
	 }
 
      }
      template<class Iterator, class TMaker, class TSelector>
      void fill(Iterator first, Iterator last,
		const TMaker& aMaker,
		const TSelector& aSel = SelectAll<NeutralNC>() ) {
	 std::transform(first,last,
			selectiveInserter(aSel),
			aMaker);
      }

      // static member functions

   protected:
      // protected member functions
      virtual dchain::LabeledParticleList< NeutralNC, CandidateClass >& labeledCandidateList() ;

      // protected const member functions
      virtual const dchain::LabeledParticleList< NeutralNC, CandidateClass >& labeledCandidateList() const ;

   private:
      // Constructors and destructor

      // private member functions
      const NeutralNCVisibleList< NeutralNC , CandidateClass >& operator=( const NeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList ) ; //stop default

      // private const member functions

      // data members
      MutableReferenceHolder<dchain::LabeledParticleList< NeutralNC , CandidateClass > > m_list ;

      mutable ChosenConjugateList< NeutralNC , CandidateClass >* m_ntBarList;
      mutable ChosenConjugateList< NeutralNC , CandidateClass >* m_isBarList;

      // static data members
      static const dchain::conjugation::Label kIsntBar = dchain::conjugation::kHeads ;
      static const dchain::conjugation::Label kIsBar = dchain::conjugation::kTails ;

} ;

// inline function definitions
   template<class TIList, class TSelector, class T, class U>
   void fill_using_selector(const TIList& iList,
			    const TSelector& iSel,
			    NeutralNCVisibleList<T,U>& oList) {
      oList.fill(iList.begin(), iList.end(),
		 typename MakerTrait<T>::maker_type(),
		 iSel);
   }

}

// function definitions
#include "DecayChain/List/Template/NeutralNCVisibleList.cc"

#endif /* DCHAIN_NEUTRALNCVISIBLELIST_H */

