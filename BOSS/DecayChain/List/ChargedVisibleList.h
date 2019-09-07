#ifndef DCHAIN_CHARGEDVISIBLELIST_H
#define DCHAIN_CHARGEDVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChargedVisibleList
// 
// Description: List to hold Candidate subclasses generated from VisibleEvidence
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: ChargedVisibleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChargedVisibleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:20  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.4  2003/08/01 13:48:40  mrs43
// changed plus and minus methods of ChargedVisibleList to const
//
// Revision 1.3  2003/05/15 19:56:06  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.2  2000/12/30 19:43:29  cdj
// no more missing symbols under OSF1
//
// Revision 1.1.1.1  2000/12/18 22:16:51  cdj
// imported DChain
//
// Revision 1.11  1998/08/20 19:58:59  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/04/17 19:10:59  sjp
// Modified to use latest types
//
// Revision 1.9  1997/09/17 20:05:27  sjp
// Removed filling to superclass
//
// Revision 1.8  1997/08/28 06:59:44  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 15:55:06  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:32:09  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:00  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:09  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:33:37  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:00  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:35:48  sjp
// New file for new `List' module
//

// system include files
#include <memory>
#include <iostream>
//#include "boost/bind.hpp"
//#include <boost/iterator/transform_iterator.hpp>

// user include files
#include "DecayChain/List/CandidateList.h" // superclass declaration
#include "DecayChain/List/ChosenChargeList.h" // for return-by-value
#include "DecayChain/Element/MutableReferenceHolder.h"
//#include "DecayChain/Element/ReferenceHolder.h"

//#include "DecayChain/Iterator/WholeCandidateItr.h"
#include "DecayChain/Iterator/PartialItr.h"
#include "DecayChain/Iterator/WholeItr.h"

#include "DecayChain/Element/MakerTrait.h"
#include "DecayChain/Iterator/SelectiveInserter.h"
#include "DecayChain/Function/SelectAll.h"
#include "DecayChain/List/FillListWithSelection.h"

// forward declarations
#include "DecayChain/List/LabeledParticleList.h"

//
//  Note: To avoid a myriad of compiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class LabeledDecayList< A , A::CandidateClass > ;

namespace dchain {
template < class T > class MuteWholeItr ;


   template<class T>
   int charge_sign(const T& iCharged) {
      return static_cast<int>(iCharged.charge());
   }

   template<class T>
   int charge_sign(const T* iCharged) {
      return static_cast<int>(iCharged->charge());
   }

   template<class T>
   int charge_sign(T* iCharged) {
      return static_cast<int>(iCharged->charge());
   }

   template<class T>
   int charge_sign(const std::auto_ptr<T>& iCharged) {
      return static_cast<int>(iCharged->charge());
   }

   template<class TPtr,class TRef>
   int charge_sign(const ReferenceHolderBase<TPtr,TRef>& iHolder) {
      return charge_sign(*iHolder);
   }


template < class Charged , class CandidateClass = typename Charged::CandidateClass >
class ChargedVisibleList : public CandidateList< CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef MuteWholeItr< Charged > iterator ;
      typedef WholeItr< Charged > const_iterator ;
      typedef PartialItr< Charged > const_partial_iterator ;

      typedef Charged value_type ;

      typedef ChargedVisibleList<Charged,CandidateClass> self_type;

      /**
	 inserts into the list only if the the selector returns true
       */
      // Constructors and destructor
      ChargedVisibleList();
      ChargedVisibleList( const self_type& aOtherList);
      template<class TSelector>
      ChargedVisibleList( const self_type& aOtherList,
			  const TSelector& aSel) :
	 m_list( new LabeledParticleList<Charged,CandidateClass> ),
	 m_plusList(0),
	 m_minusList(0)
      {
	 fill(*(aOtherList.m_list), 
	      typename MakerTrait<Charged>::maker_type(), 
	      aSel);
      }

      template <class WitnessIterator>
      ChargedVisibleList(WitnessIterator first, WitnessIterator last) : 
	 m_list( new LabeledParticleList<Charged,CandidateClass> ),
	 m_plusList(0),
	 m_minusList(0)
      {
	 fill(first, last, typename MakerTrait<Charged>::maker_type() ); }

      template<class WitnessIterator, class TSelector>
      ChargedVisibleList(WitnessIterator first, WitnessIterator last, 
			    const TSelector& aFunctionalObject ) :
	 m_list( new LabeledParticleList<Charged,CandidateClass> ),
	 m_plusList(0),
	 m_minusList(0)
      {
	 fill(first, last, 
	      typename MakerTrait<Charged>::maker_type(), aFunctionalObject); }

      virtual ~ChargedVisibleList() ;
      
      // assignment operator(s)
      const self_type& operator=( const self_type& aOtherList ) ;
      template<class Witness>
      const self_type& operator=(const Witness& aWitnessList) {
	 eraseContents();
	 fill(aWitnessList.begin(), aWitnessList.end(),
	      typename MakerTrait<Charged>::maker_type(),
	      SelectAll<Charged>() );
	 return ( *this ) ;
      }

      // member functions
      template<class TSelector>
      FillListWithSelection<self_type, TSelector> operator[](const TSelector& iSel) {
	 return FillListWithSelection<self_type,TSelector>(*this, iSel);
      }

      iterator particle_begin() ;
      iterator particle_end() ;
      ChosenChargeList< Charged , CandidateClass >& plus() const;
      ChosenChargeList< Charged , CandidateClass >& minus() const;

      // const member functions
      const_iterator particle_begin() const ;
      const_iterator particle_end() const ;


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
	 //std::cout << " @ ChargedVisibleList::attempt_insert() " << __LINE__ << std::endl;
	 bool returnValue = (aSelect)(*pPossible);
	 if (returnValue) {
	    this->insert(pPossible);
	 }
	 return returnValue;
      }

      template <class THolder>
      void insert(THolder& pPossible) {
	 if (0 < charge_sign(pPossible)) {
	    m_list->push_back( LabeledParticle< Charged >( pPossible ,
							  kPlus ) ) ;
	 }
	 else {
	    m_list->push_back( LabeledParticle< Charged >( pPossible ,
							  kMinus ) ) ;
	 }
      }

      void eraseContents() ;
      void erase() {
	 eraseContents();
      }

      template<class TMaker, class TSelector>
      void fill( const LabeledParticleList< Charged , CandidateClass >& aList,
		 const TMaker& aMaker,
		 const TSelector& aSel) {

	 for( const_iterator entry(aList.begin()); entry != aList.end(); ++entry){
	    //NOTE: even though this is really just calling the copy constructor
	    //it is best to use the maker since it allows further control
	    const Charged& from((*entry)());
	    typename ResultTypeTrait<TMaker>::type localCopy( aMaker(from) );
	    
	    attempt_insert(localCopy,aSel);
	 }
 
      }
      template<class Iterator, class TMaker, class TSelector>
      void fill(Iterator first, Iterator last,
		const TMaker& aMaker,
		const TSelector& aSel = SelectAll<Charged>() ) {
	 //std::cout << " @ ChargedVisibleList::fill() " << __LINE__ << std::endl;
	 //std::cout << "size before transform: " << m_list->size() << std::endl;
	 std::transform(first,last,
			selectiveInserter(aSel),
			aMaker);
	 //std::cout << "size after transform: " << m_list->size() << std::endl;
      }
      // static member functions

   protected:
      // protected member functions
      virtual LabeledParticleList< Charged , CandidateClass >& labeledCandidateList();
      

      // protected const member functions
      virtual const LabeledParticleList< Charged , CandidateClass >& labeledCandidateList() const ;

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      MutableReferenceHolder<LabeledParticleList< Charged , CandidateClass > > m_list ;

      mutable ChosenChargeList< Charged, CandidateClass >* m_plusList;
      mutable ChosenChargeList< Charged, CandidateClass >* m_minusList;

      // static data members
      static const conjugation::Label kPlus = conjugation::kHeads;
      static const conjugation::Label kMinus = conjugation::kTails;

} ;

// inline function definitions
   
   template<class TIList, class TSelector, class T, class U>
   void fill_using_selector(const TIList& iList,
			    const TSelector& iSel,
			    ChargedVisibleList<T,U>& oList) {
      oList.fill(iList.begin(), iList.end(),
		 typename MakerTrait<T>::maker_type(),
		 iSel);
   }
}

// function definitions
#include "DecayChain/List/Template/ChargedVisibleList.cc"

#endif /* DCHAIN_CHARGEDVISIBLELIST_H */

