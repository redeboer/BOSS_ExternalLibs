#ifndef DCHAIN_COMBINATORICLIST_CC
#define DCHAIN_COMBINATORICLIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      CombinatoricList
// 
// Description: container used to build Decay subclass objects
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Sat Oct  5 17:18:02 EDT 1996
// $Id: CombinatoricList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CombinatoricList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:35  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.10  2003/05/15 19:56:12  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.9  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.8  2002/06/21 14:20:51  cdj
// previous changes now compile under OSF1
//
// Revision 1.7  2002/06/19 20:37:10  cleo3
// added missing typename and create a copy constructor
//
// Revision 1.6  2002/06/18 20:52:04  cdj
// fixed bug that occurred when mulitplying 3 or more of the same list and conjugation where one of the list was the first list of the combination
//
// Revision 1.5  2001/04/20 14:02:09  ajm36
// plug memory leak associated with makeDecay
//
// Revision 1.4  2001/04/10 13:56:48  urner
// bug fixes
//
// Revision 1.3  2001/03/30 19:41:12  cdj
// improved memory usage when doing combinatorics
//
// Revision 1.2  2001/03/29 21:33:38  cdj
// initial try at bug fix for selfCongugateList*nonSelfCongugateList
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.13  1998/08/21 00:51:54  sjp
// Modifier to no longer use typedefs
//
// Revision 1.12  1998/08/20 20:00:09  sjp
// Modified to use DChainBoolean
//
// Revision 1.11  1998/08/19 20:46:21  sjp
// Fixed comments and removed report references
//
// Revision 1.10  1998/04/17 19:14:49  sjp
// Modified to use latest type
//
// Revision 1.9  1997/09/03 14:58:22  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.8  1997/08/28 07:00:35  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 23:03:14  sjp
// Restructured package to be independent of Rock
//
// Revision 1.6  1997/08/19 16:19:03  sjp
// Improved handling of STL containers
//
// Revision 1.5  1997/08/15 21:33:35  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/31 20:16:24  sjp
// Modified to use the new `bug' include files
//
// Revision 1.3  1997/01/21 20:31:34  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:14:01  sjp
// Changed aPossible to Possible and added dropLink where nec.
//
// Revision 1.1  1996/11/04 17:17:12  sjp
// New file for new `List' module
//

// system include files
#include <stdlib.h>  // For 'exit'

// user include files
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeCandidateItr.h"
#include "DecayChain/Iterator/WholeCandidateItr.h"
#include "DecayChain/Iterator/PartialCandidateItr.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Iterator/PartialItr.h"
#include "DecayChain/List/CombinatoricLoop.h"

#include "DecayChain/List/CombinatoricList.h"

//
// constants, enums and typedefs
//
namespace dchain {
static const conjugation::Label kBaseLabel = conjugation::kHeads ;

//
// local functions
//

// The follwing function is local rather than a member of CombinatoricList
//   as that would require the file <vector> to be included in
//   CombinatoricList.h, and thus in many other parts of the code.  That
//   inclusion can sieverly slow down compliation and also cause many
//   local copies of the code to be generated.  It is best to avoid this.
//
// Now, you may ask why make this a static function in a class.  The simple
//   answer is it works!  I tried this as a simple, normal static
//   (i.e. file-scope) function and my compilers barfed.  Why do it work
//   in a class? I don't know, but it does so lets just get one with it!

template < class DecayClass, class CandidateClass >
class FillDecayList {
public:
   class ChildList  {
      public:

	 ChildList() {
	    m_list.reserve(4);
	 }

	 ChildList(const DecayClass* decay) {
	    //fill the m_list with decay
	    uint32_t n = decay->numberChildren();
	    for ( uint32_t i = 0; i < n; i++) m_list.push_back(&(decay->child(i)));
	 }

	 void push_back( const CandidateClass* iCandidate ) {
	    m_list.push_back( iCandidate );
	 }
	 void pop_back() {
	    m_list.pop_back();
	 }
	 void eraseAll() {
	    m_list.erase(m_list.begin(), m_list.end());
	 }

	 bool overlap( const CandidateClass& iCandidate ) const {
	    for( typename std::vector<const CandidateClass*>::const_iterator itChild =
		  m_list.begin();
		  itChild != m_list.end();
		  ++itChild ) {
	       if( iCandidate.overlap( *(*itChild) ) ) {
		  return true;
	       }
	    }
	    return false;
	 }

	 DecayClass* makeDecay() {
	    DecayClass* returnValue = new DecayClass( *(m_list.front()) );
	    for( typename std::vector<const CandidateClass*>::iterator itChild = m_list.begin()+1;
		  itChild != m_list.end();
		  ++itChild ) {
	       returnValue->addChild( *(*itChild) );
	    }
	    return returnValue;
	 }
      private:
	 std::vector<const CandidateClass*> m_list;
   };

   static void fill( CombinatoricLoop<CandidateClass >& iLoop,
	 const typename _combinatoricloop_vector_::iterator& aBegin ,
	 const typename _combinatoricloop_vector_::iterator& aEnd ,
	 const conjugation::Label aLabel ,
	 dchain::LabeledParticleList< DecayClass , CandidateClass >& aDecayList )
   {
      ChildList childList;

      typename dchain::CandidateList<CandidateClass >::const_partial_iterator itEnd = iLoop.partial_end();
      for( typename dchain::CandidateList<CandidateClass >::const_partial_iterator entry = iLoop.partial_begin();
	    entry != itEnd;
	    ++entry ) {

	 iLoop.setCurrentIterator( entry ) ;

	 childList.push_back( & (*entry).labeledClass() );

	 //
	 // Note: need to cast way `const' to make sure a non-const LabeledParticleList
	 //         is returned 
	 //
	 doIt( aBegin,
	       aEnd,
	       childList ,
	       aLabel,
	       aDecayList );
	 childList.eraseAll();
      }
   }

   static void doIt( const typename _combinatoricloop_vector_::iterator& aBegin ,
	 const typename _combinatoricloop_vector_::iterator& aEnd ,
	 ChildList& iList ,
	 const conjugation::Label aLabel ,
	 dchain::LabeledParticleList< DecayClass, CandidateClass >& aDecayList )
   {
      if ( aBegin == aEnd ) {
	 //
	 // if all ConjuagetLists have been used add Decay to the final list
	 //
	 DecayClass* ptr = iList.makeDecay();
	 ReferenceHolder<DecayClass> pHolder(ptr);
	 aDecayList.push_back(dchain::LabeledParticle< DecayClass >( 
		  ptr,
		  aLabel ) ) ;
      }
      else {
	 //
	 // make iterator for next ConjugateList loop
	 //
	 const typename _combinatoricloop_vector_::iterator newBegin( aBegin + 1 ) ;
	 //
	 // loop over each entry in the current ConjugateList
	 //
	 typename dchain::CandidateList< CandidateClass >::const_partial_iterator finished( (*(*aBegin)).partial_end() ) ;
	 for ( typename dchain::CandidateList< CandidateClass >::const_partial_iterator entry( (*(*aBegin)).partial_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    if ( !( iList.overlap( (*entry)() ) ) ) {
	       //
	       // If baseDecay does not overlap current element in the this ConjugateList,
	       //   record the current element of this Loop
	       //
	       (*(*aBegin)).setCurrentIterator( entry ) ;
	       //
	       // build new baseDecay with old baseDecay plus the current element
	       //
	       iList.push_back( & (*entry).labeledClass() );
	       //
	       // do all successive ConjugateList loops
	       //
	       doIt( newBegin ,
		     aEnd ,
		     iList ,
		     aLabel ,
		     aDecayList ) ;

	       iList.pop_back();

	    }
	 }
      }
   }
} ;

//
// static data member definitions
//

//
// constructors and destructor
//

//the copy constructor is only used for the return value of operator* so
// we can make things faster by swapping the data stored in the vector
template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const CombinatoricList<CandidateClass >& iRHS) :
   m_listOfLists( *(new _combinatoriclist_vector_() ) ),
   m_listFilled( iRHS.m_listFilled ) ,
   m_conjugationKnown( iRHS.m_conjugationKnown )
{
   m_listOfLists.swap(const_cast<CombinatoricList<CandidateClass>&>(iRHS).m_listOfLists);
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const ConjugateList< CandidateClass >& aList ) :
   m_listOfLists( *(new _combinatoriclist_vector_( 1 ,
						   &aList ) ) ) ,
   m_listFilled( false ) ,
   m_conjugationKnown( false )
{
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const ConjugateList< CandidateClass >& lhs ,
				    const ConjugateList< CandidateClass >& rhs ) :
   m_listOfLists( *(new _combinatoriclist_vector_( 1 ,
						   &lhs ) ) ) ,
   m_listFilled( false ) ,
   m_conjugationKnown( false )
{
   //std::cout << "@CombinatoricList::CombinatoricList() 11" << std::endl;
   //typedef typename ConjugateList< CandidateClass >::const_partial_iterator test_it;
   //std::cout << "lhs ...................... : " << std::endl;
   //for ( test_it it = lhs.partial_begin(); it != lhs.partial_end(); it++ ) {
   //   std::cout << (*it)().footPrint() << std::endl;
   //}
   //std::cout << "rhs ...................... : " << std::endl;
   //for ( test_it it = rhs.partial_begin(); it != rhs.partial_end(); it++ ) {
   //   std::cout << (*it)().footPrint() << std::endl;
   //}
   m_listOfLists.push_back( &rhs ) ;
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const CombinatoricList< CandidateClass >& lhs ,
				    const ConjugateList< CandidateClass >& rhs ) :
   m_listOfLists( *(new _combinatoriclist_vector_( lhs.m_listOfLists ) ) ) ,
   m_listFilled( false ) ,
   m_conjugationKnown( false )
{
   m_listOfLists.push_back( &rhs ) ;
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const ConjugateList< CandidateClass >& lhs ,
				    const CombinatoricList< CandidateClass >& rhs ) :
   m_listOfLists( *(new _combinatoriclist_vector_( 1 ,
						   &lhs ) ) ) ,
   m_listFilled( false ) ,
   m_conjugationKnown( false )
{
   m_listOfLists.insert( m_listOfLists.end() ,
			 (rhs.m_listOfLists).begin() ,
			 (rhs.m_listOfLists).end() ) ;
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::CombinatoricList( const CombinatoricList< CandidateClass >& lhs ,
				    const CombinatoricList< CandidateClass >& rhs ) :
   m_listOfLists( *(new _combinatoriclist_vector_( lhs.m_listOfLists ) ) ) ,
   m_listFilled( false ) ,
   m_conjugationKnown( false )
{
   m_listOfLists.insert( m_listOfLists.end() ,
			 (rhs.m_listOfLists).begin() ,
			 (rhs.m_listOfLists).end() ) ;
}

template < class CandidateClass >
CombinatoricList< CandidateClass >::~CombinatoricList()
{
   delete &m_listOfLists ;
}

//
// assignment operators
//
// const CombinatoricList< CandidateClass >& CombinatoricList< CandidateClass >::operator=( const CombinatoricList< CandidateClass >& )
// {
// }

//
// member functions
//

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::iterator CombinatoricList< CandidateClass >::begin()
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::begin() ) ;
}

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::iterator CombinatoricList< CandidateClass >::end()
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::end() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::iterator CombinatoricList< CandidateClass >::particle_begin()
{
   //std::cout << "@CombinatoricList::particle_begin() -- iterator" << std::endl;
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::particle_begin() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::iterator CombinatoricList< CandidateClass >::particle_end() {
   //std::cout << "@CombinatoricList::particle_end() -- iterator" << std::endl;
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::particle_end() ) ;
}

template < class CandidateClass >
dchain::LabeledCandidateList< CandidateClass >& CombinatoricList< CandidateClass >::labeledCandidateList()
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::labeledCandidateList() ) ;
}

//
// const member functions
//

template < class CandidateClass >
const dchain::DecayList< typename CombinatoricList<CandidateClass>::DecayClass , CandidateClass >
CombinatoricList< CandidateClass >::bar() const
{
   fill() ;
// Need to cast away 'const' to get the right type of LabelList returned
   return ( dchain::DecayList< DecayClass , CandidateClass >(
      (*(CombinatoricList< CandidateClass >*)this).labeledParticleList() ,
      otherLabel( label() ) ) ) ;
}

template < class CandidateClass >
const dchain::DecayList< typename CombinatoricList<CandidateClass>::DecayClass , CandidateClass >& 
CombinatoricList< CandidateClass >::operator()() const
{
   fill() ;
// // Need to cast away 'const' to get the right type of LabelList returned
//    return ( dchain::DecayList< DecayClass , CandidateClass >( (*(CombinatoricList< CandidateClass >*)this).labeledDecayList() ,
// 				   label() ) ) ;
   return ( *this ) ;
}

template < class CandidateClass >
bool 
CombinatoricList< CandidateClass >::isSelfConjugate() const
{
   determineConjugation() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::isSelfConjugate() ) ;
}

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::const_iterator CombinatoricList< CandidateClass >::begin() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::begin() ) ;
}

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::const_iterator CombinatoricList< CandidateClass >::end() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::end() ) ;
}

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::const_partial_iterator CombinatoricList< CandidateClass >::partial_begin() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::partial_begin() ) ;
}

template < class CandidateClass >
typename dchain::CandidateList< CandidateClass >::const_partial_iterator CombinatoricList< CandidateClass >::partial_end() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::partial_end() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::const_iterator 
CombinatoricList< CandidateClass >::particle_begin() const
{
   //std::cout << "@CombinatoricList::particle_begin() -- iterator" << std::endl;
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::particle_begin() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::const_iterator 
CombinatoricList< CandidateClass >::particle_end() const
{
   //std::cout << "@CombinatoricList::particle_end() -- const_iterator" << std::endl;
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::particle_end() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::const_partial_iterator 
CombinatoricList< CandidateClass >::partial_particle_begin() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::partial_particle_begin() ) ;
}

template < class CandidateClass >
typename dchain::DecayList< typename DCCandidateTraits<CandidateClass>::DecayClass , CandidateClass >::const_partial_iterator 
CombinatoricList< CandidateClass >::partial_particle_end() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::partial_particle_end() ) ;
}


template < class CandidateClass >
conjugation::Label CombinatoricList< CandidateClass >::label() const
{
   determineConjugation() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::label() ) ;
}

template < class CandidateClass >
const dchain::LabeledCandidateList< CandidateClass >& CombinatoricList< CandidateClass >::labeledCandidateList() const
{
   fill() ;
   return ( dchain::DecayList< DecayClass , CandidateClass >::labeledCandidateList() ) ;
}

template < class CandidateClass >
void CombinatoricList< CandidateClass >::determineConjugation() const
{
   if ( m_conjugationKnown ) {
      return ;
   }
   _combinatoriclist_vector_ unConjugatedLists ;
   typename _combinatoriclist_vector_::const_iterator finishedTesting ( m_listOfLists.end() ) ;
   for ( typename _combinatoriclist_vector_::const_iterator list( m_listOfLists.begin() ) ;
	 list != finishedTesting ;
	 ++list ) {
      if ( ! (*(*list)).isSelfConjugate() ) {
         bool noMatchFound( !false ) ;
	 typename _combinatoriclist_vector_::iterator matchToCheck( unConjugatedLists.begin() ) ;
	 typename _combinatoriclist_vector_::iterator finishedMatching ( unConjugatedLists.end() ) ;
	 while ( noMatchFound &&
		 ( matchToCheck != finishedMatching ) ) {
	    if ( (*(*list)).isConjugateOf( (*(*matchToCheck)) ) ) {
	       unConjugatedLists.erase( matchToCheck ) ;
	       noMatchFound = false ;
	    }
	    ++matchToCheck ;
	 }
	 if ( noMatchFound ) {
	    unConjugatedLists.push_back( *list ) ;
	 }
      }
   }
   if ( 0 == unConjugatedLists.size() ) {
// have to cast away 'const' to set the real value of the label
      (*(CombinatoricList< CandidateClass >*)this).setLabel( conjugation::kNone ) ;
   }
   else {
// have to cast away 'const' to set the real value of the label
      (*(CombinatoricList< CandidateClass >*)this).setLabel( kBaseLabel ) ;
   }
// have to cast away 'const' to set the real value of the flag
   (*(CombinatoricList< CandidateClass >*)this).m_conjugationKnown = !false ;
}

template < class CandidateClass >
void CombinatoricList< CandidateClass >::fill() const
{
   //std::cout << "@CombinatoricList::fill(), filled = " << m_listFilled << std::endl;
   if ( m_listFilled ) {
      return ;
   }
//
// Create CombinatoricLoop lists for `primary' half of the list.
// If list is not self-conjugate, fill CombinatoricLoop lists for `conjugate'
//   half of the list as well.
//
   const short kPrimary = 0 ;
   const short kConjugate = 1 ;
   const short kEndLoopType = 2 ;
   _combinatoricloop_vector_ loopList[ 2 ] ;
   typename _combinatoricloop_vector_::iterator initialLoop[ 2 ] ;
   typename _combinatoricloop_vector_::iterator secondLoop[ 2 ] ;
   typename _combinatoricloop_vector_::iterator endLoop[ 2 ] ;
   for ( short loopType( kPrimary ) ;
	 loopType != kEndLoopType ;
	 ++loopType ) {
      if ( ( kConjugate != loopType ) ||
           ( ! isSelfConjugate() ) ) {
	 loopList[ loopType ].reserve( m_listOfLists.size() ) ;
	 typename _combinatoriclist_vector_::const_iterator finishedBuilding( m_listOfLists.end() ) ;
	 for ( typename _combinatoriclist_vector_::const_iterator listForLoop( m_listOfLists.begin() ) ;
	       listForLoop != finishedBuilding ;
	       ++listForLoop ) {
	    CombinatoricLoop< CandidateClass >* newLoop ;
	    if ( kPrimary == loopType ) {
	       newLoop = new CombinatoricLoop< CandidateClass >( (*(*listForLoop)).partial_begin() ,
							     (*(*listForLoop)).partial_end() ,
							     loopList[ loopType ] ) ;
	    }
	    else {
	       newLoop = new CombinatoricLoop< CandidateClass >( (*(*listForLoop)).conjugate_partial_begin() ,
							     (*(*listForLoop)).conjugate_partial_end() ,
							     loopList[ loopType ] ) ;
	    }
	    loopList[ loopType ].push_back( newLoop ) ;
	 }
	 initialLoop[ loopType ] = loopList[ loopType ].begin() ;
	 secondLoop[ loopType ] = initialLoop[ loopType ] + 1 ;
	 endLoop[ loopType ] = loopList[ loopType ].end() ;
      }
   }
//
// do loop through initial ConjugateList
//
   conjugation::Label conjugationLabel = kBaseLabel;
   if( isSelfConjugate() ) {
      conjugationLabel = conjugation::kNone;
   } 
      
   FillDecayList< DecayClass , CandidateClass >::fill(
      *(*initialLoop[ kPrimary ]),
      secondLoop[ kPrimary ] ,
      endLoop[ kPrimary ] ,
      conjugationLabel,
      const_cast<CombinatoricList< CandidateClass >*>(this)->labeledParticleList() );

   if( !isSelfConjugate() ) {
      FillDecayList< DecayClass , CandidateClass >::fill(
	 *(*initialLoop[ kConjugate ]),
	 secondLoop[ kConjugate ] ,
	 endLoop[ kConjugate ] ,
	 otherLabel(conjugationLabel),
	 const_cast<CombinatoricList< CandidateClass >*>(this)->labeledParticleList() );
   }
   /*
#if defined(THIS_IS_NEVER_TRUE)
   conjugation::Label entryLabel[ 2 ] ;
   if ( ! isSelfConjugate() ) {
      entryLabel[ 0 ] = kBaseLabel ;
      entryLabel[ 1 ] = otherLabel( kBaseLabel ) ;
   }
   else {
      entryLabel[ 0 ] = conjugation::kNone ;
   }
   const ConjugateList< CandidateClass >& initialList( (*(*(m_listOfLists.begin()))) ) ;
   typename dchain::CandidateList< CandidateClass >::const_partial_iterator initialListEntry[ 2 ] ;
   initialListEntry[ kPrimary ] = initialList.partial_begin() ;
   if ( ! isSelfConjugate() ) {
      initialListEntry[ kConjugate ] = initialList.conjugate_partial_begin() ;
   }
   typename dchain::CandidateList< CandidateClass >::const_iterator finished( initialList.end() ) ;
   for ( typename dchain::CandidateList< CandidateClass >::const_iterator entry( initialList.begin() ) ;
	 entry != finished ;
	 ++entry ) {
//
// if entry is in `primary' half of list build all decays for this
//   entry, otherwise, if list is not self-conjugate build all decays
//   for this `conjugate' entry
//
      short entryType( kConjugate ) ;
      if ( &(*entry) == &(*initialListEntry[ kPrimary ]) ) {
         entryType = kPrimary ;
      }
      if ( ( kConjugate != entryType ) ||
           ( ! isSelfConjugate() ) ) {
	 (*(*initialLoop[ entryType ])).setCurrentIterator( initialListEntry[ entryType ] ) ;
	 ReferenceHolder<DecayClass> initialDecay( new DecayClass( (*initialListEntry[ entryType ]).labeledClass() ) );
//
// Note: need to cast way `const' to make sure a non-const LabeledParticleList
//         is returned 
//
	 typename FillDecayList<DecayClass, CandidateClass>::ChildList childList(initialDecay.pointer());

	 FillDecayList< DecayClass , CandidateClass >::
	    doIt( secondLoop[ entryType ] ,
		  endLoop[ entryType ] ,
		  childList,
		  entryLabel[ entryType ] ,
		  (*(CombinatoricList< CandidateClass >*)this).labeledParticleList()
		  //this->labeledParticleList()
		  ) ;
	 ++initialListEntry[ entryType ] ;
      }
   }
#endif
   */
//
// delete contents of CombinatoricLoop lists.
//
   for ( short halfType( kPrimary ) ;
	 halfType != kEndLoopType ;
	 ++halfType ) {
      if ( ( kConjugate != halfType ) ||
           ( ! isSelfConjugate() ) ) {
	 typename _combinatoricloop_vector_::iterator finishedDeletion( loopList[ halfType ].end() ) ;
	 for ( typename _combinatoricloop_vector_::iterator loopInList( loopList[ halfType ].begin() ) ;
	       loopInList != finishedDeletion ;
	       ++loopInList ) {
	    delete *loopInList ;
	 }
      }
   }
   m_listFilled = !false ;
}

//
// static member functions
//
}
#endif /* DCHAIN_COMBINATORICLIST_CC */




