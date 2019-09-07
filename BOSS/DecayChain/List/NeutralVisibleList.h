#ifndef DCHAIN_NEUTRALVISIBLELIST_H
#define DCHAIN_NEUTRALVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralVisibleList
// 
// Description: List to hold Candidates generated from observed Tracks.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: NeutralVisibleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: NeutralVisibleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:29  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.4  2003/05/15 19:56:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2001/03/28 14:27:58  urner
// added same workaround as for charged because of NonTemplatedMemberFunctionBug
//
// Revision 1.2  2001/03/23 23:07:44  urner
// modification needed to add pi0 eta and Ks decay lists in CleoDChain
//
// Revision 1.1.1.1  2000/12/18 22:16:53  cdj
// imported DChain
//
// Revision 1.11  1998/08/20 19:59:05  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/04/17 19:11:05  sjp
// Modified to use latest types
//
// Revision 1.9  1997/09/17 20:05:29  sjp
// Removed filling to superclass
//
// Revision 1.8  1997/08/28 06:59:56  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 15:55:10  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:32:39  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:14  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:32  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:34:06  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:23  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:11  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/List/ConjugateList.h" // superclass declaration
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

//
//  Note: To avoid a myriad of conpiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class LabeledDecayList< A , A::CandidateClass > ;

template< class Neutral , class CandidateClass = typename Neutral::CandidateClass >
class NeutralVisibleList : public dchain::ConjugateList< CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeItr< Neutral > iterator ;
      typedef dchain::WholeItr< Neutral > const_iterator ;
      //typedef dchain::PartialItr< Neutral > const_partial_iterator ;

      typedef Neutral value_type ;

      typedef NeutralVisibleList<Neutral,CandidateClass> self_type;
      // Constructors and destructor
      NeutralVisibleList() ;
      NeutralVisibleList( const self_type& aOtherList);
      template<class TSelector>
      NeutralVisibleList( const self_type& aOtherList ,
			    const TSelector& aSel) :
	 ConjugateList< CandidateClass >( dchain::conjugation::kNone ),
	 m_list(new LabeledParticleList<Neutral,CandidateClass>) {
	 fill(*(aOtherList.m_list),
	      typename MakerTrait<Neutral>::maker_type(),
	      aSel);
      }

      template <class WitnessIterator>
      NeutralVisibleList(WitnessIterator first, WitnessIterator last) : 
	 ConjugateList< CandidateClass >( dchain::conjugation::kNone ),
	 m_list( new LabeledParticleList<Neutral,CandidateClass> ) {
	 fill(first, last, typename MakerTrait<Neutral>::maker_type() ); }

      template<class WitnessIterator, class TSelector>
      NeutralVisibleList(WitnessIterator first, WitnessIterator last, 
			    const TSelector& aFunctionalObject )
	 : 
	 ConjugateList< CandidateClass >( dchain::conjugation::kNone ),
	 m_list( new LabeledParticleList<Neutral,CandidateClass> )
      {
	 fill(first, last, 
	      typename MakerTrait<Neutral>::maker_type(), aFunctionalObject); }
      virtual ~NeutralVisibleList() ;
      
      // assignment operator(s)
      const self_type& operator=(const self_type& aOtherList );
      
      template<class Witness>
      const self_type& operator=(const Witness& aWitnessList) {
	 eraseContents();
	 fill(aWitnessList.begin(), aWitnessList.end(),
	      typename MakerTrait<Neutral>::maker_type(),
	      SelectAll<Neutral>() );
	 return ( *this ) ;
      }

      // member functions
      template<class TSelector>
      FillListWithSelection<self_type, TSelector> operator[](const TSelector& iSel) {
	 return FillListWithSelection<self_type,TSelector>(*this, iSel);
      }

      iterator particle_begin() ;
      iterator particle_end() ;

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
	 bool returnValue = (aSelect)(*pPossible);
	 if (returnValue) {
	    this->insert(pPossible);
	 }
	 return returnValue;
      }
      template <class THolder>
      void insert(THolder& pPossible) {
	 m_list->push_back( LabeledParticle< Neutral >( pPossible ,
							conjugation::kNone ) ) ;
      }
      void eraseContents() ;

      void erase() {
	 eraseContents();
      }
      // static member functions

      template<class TMaker, class TSelector>
      void fill( const LabeledParticleList< Neutral , CandidateClass >& aList,
		 const TMaker& aMaker,
		 const TSelector& aSel) {

	 for( const_iterator entry(aList.begin()); entry != aList.end(); ++entry){
	    //NOTE: even though this is really just calling the copy constructor
	    //it is best to use the maker since it allows further control
	    const Neutral& from((*entry)());
	    typename ResultTypeTrait<TMaker>::type localCopy( aMaker(from) );
	    
	    attempt_insert(localCopy,aSel);
	 }
 
      }
      template<class Iterator, class TMaker, class TSelector>
      void fill(Iterator first, Iterator last,
		const TMaker& aMaker,
		const TSelector& aSel = SelectAll<Neutral>() ) {
	 std::transform(first,last,
			selectiveInserter(aSel),
			aMaker);
      }
   protected:
      // protected member functions
      virtual dchain::LabeledParticleList< Neutral, CandidateClass >& labeledCandidateList() ;

      // protected const member functions
      virtual const dchain::LabeledParticleList< Neutral, CandidateClass >& labeledCandidateList() const ;

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      MutableReferenceHolder< dchain::LabeledParticleList< Neutral , CandidateClass > > m_list ;

      // static data members

} ;

// inline function definitions
   template<class TIList, class TSelector, class T, class U>
   void fill_using_selector(const TIList& iList,
			    const TSelector& iSel,
			    NeutralVisibleList<T,U>& oList) {
      oList.fill(iList.begin(), iList.end(),
		 typename MakerTrait<T>::maker_type(),
		 iSel);
   }

}

// function definitions
#include "DecayChain/List/Template/NeutralVisibleList.cc"

#endif /* DCHAIN_NEUTRALVISIBLELIST_H */
