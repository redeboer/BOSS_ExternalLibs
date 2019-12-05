#ifndef DCHAIN_DECAYLIST_CC
#define DCHAIN_DECAYLIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      DecayList
// 
// Description: container of subclasses of Decay, used for iteration
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: DecayList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DecayList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:39  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.6  2003/10/23 18:43:41  cdj
// can now use DecayList with other Decay objects
//
// Revision 1.5  2003/05/16 18:33:39  cdj
// previous change now works properly on OSF
//
// Revision 1.4  2003/05/15 19:56:12  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.2  2001/03/23 23:07:48  urner
// modification needed to add pi0 eta and Ks decay lists in CleoDChain
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.13  1998/08/20 20:00:07  sjp
// Modified to use DChainBoolean
//
// Revision 1.12  1998/08/19 20:46:18  sjp
// Fixed comments and removed report references
//
// Revision 1.11  1998/06/30 23:42:06  sjp
// Cleaned up return object from labeledCandidateList
//
// Revision 1.10  1998/05/01 20:20:03  sjp
// Removed unnecessary functions which were causing infinate loop
//
// Revision 1.9  1998/04/17 19:14:45  sjp
// Modified to use latest type
//
// Revision 1.8  1997/09/03 14:58:17  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.7  1997/08/28 07:00:15  sjp
// Modified files to handle complete templating
//
// Revision 1.6  1997/08/19 23:02:31  sjp
// Restructured package to be independent of Rock
//
// Revision 1.5  1997/08/15 21:32:58  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/31 20:17:26  sjp
// Modified to use the new `bug' include files
//
// Revision 1.3  1997/01/21 20:34:38  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:16:01  sjp
// Ammended to allow for LabaledLists being reference counted.
// Added dropLink() for all created entries.
//
// Revision 1.1  1996/11/04 19:36:49  sjp
// New file for new `List' module
//

// system include files
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/Function/DCSelectionFunction.h"
#include "DecayChain/Function/DCAnalysisFunction.h"
#include "DecayChain/Function/DCConjugateFunction.h"
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Iterator/PartialItr.h"
#include "DecayChain/List/LabeledParticleList.h"
#include "DecayChain/List/CombinatoricList.h"

#include "DecayChain/List/DecayList.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
namespace dchain {
template < class DecayClass , class CandidateClass >
DecayList< DecayClass , CandidateClass >::DecayList() :
   ConjugateList< CandidateClass >( conjugation::kNone ) ,
   m_list( (new LabeledParticleList< DecayClass , CandidateClass >) )
{
}

template < class DecayClass , class CandidateClass >
DecayList< DecayClass , CandidateClass >::DecayList( const DecayList< DecayClass , CandidateClass >& aOtherList ):
   ConjugateList< CandidateClass >( conjugation::kNone ) ,
   m_list( (new LabeledParticleList< DecayClass , CandidateClass >) )
{
   fill( aOtherList, SelectAll<DecayClass>() ) ;
}


template < class DecayClass , class CandidateClass >
DecayList< DecayClass , CandidateClass >::DecayList( LabeledParticleList< DecayClass , CandidateClass >& aLabeledList , conjugation::Label aLabel ) :
   ConjugateList< CandidateClass >( aLabel ) ,
   m_list( &aLabeledList ) 
{
}

template < class DecayClass , class CandidateClass >
DecayList< DecayClass , CandidateClass >::~DecayList()
{
}

//
// assignment operators
//

template < class DecayClass , class CandidateClass >
const DecayList< DecayClass , CandidateClass >& DecayList< DecayClass , CandidateClass >::operator=( const DecayList< DecayClass , CandidateClass >& aOtherList )
{
   //std::cout << "@DecayList::operator=() ... (DecayList)" << std::endl;
   m_list->erase( m_list->begin() ,
		  m_list->end() ) ;
   fill( aOtherList, SelectAll<DecayClass>() ) ;
   return ( *this ) ;
}

template < class DecayClass , class CandidateClass >
const DecayList< DecayClass , CandidateClass >& DecayList< DecayClass , CandidateClass >::operator=( const CombinatoricList< CandidateClass >& aOtherList )
{
   //std::cout << "@DecayList::operator=() ... (CombinatoricList)" << std::endl;
   return ( *this = static_cast<const DecayList<typename DCCandidateTraits<CandidateClass>::DecayClass, CandidateClass>& >(aOtherList) ) ;
}

//
// member functions
//

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::iterator DecayList< DecayClass , CandidateClass >::particle_begin()
{
   //std::cout << "@DecayList::particle_begin() -- iterator" << std::endl;
   return ( m_list->begin() ) ;
}

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::iterator DecayList< DecayClass , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template < class DecayClass , class CandidateClass >
dchain::LabeledCandidateList< CandidateClass >& DecayList< DecayClass , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}

template < class DecayClass , class CandidateClass >
dchain::LabeledParticleList< DecayClass , CandidateClass >& DecayList< DecayClass , CandidateClass >::labeledParticleList()
{
   return ( *m_list ) ;
}


//
// const member functions
//

template < class DecayClass , class CandidateClass >
const DecayList< DecayClass , CandidateClass > DecayList< DecayClass , CandidateClass >::bar() const
{
   return ( DecayList< DecayClass , CandidateClass >( *m_list ,
				      otherLabel( this->label() ) ) ) ;
}

template < class DecayClass , class CandidateClass >
const DecayList< DecayClass , CandidateClass >& DecayList< DecayClass , CandidateClass >::operator()() const
{
   return ( *this ) ;
}

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::const_iterator DecayList< DecayClass , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
   //std::cout << "@DecayList::particle_begin() -- const_iterator" << std::endl;
   return ( (*(const LabeledParticleList< DecayClass , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::const_iterator DecayList< DecayClass , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
	 return ( (*(const LabeledParticleList< DecayClass , CandidateClass >*)(m_list.pointer())).end() ) ;
}

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::const_partial_iterator DecayList< DecayClass , CandidateClass >::partial_particle_begin() const
{
// Need to cast to 'const' to get the right function call
	 return ( (*(const LabeledParticleList< DecayClass , CandidateClass >*)(m_list.pointer())).begin( this->label() ) ) ;
}

template < class DecayClass , class CandidateClass >
typename DecayList< DecayClass , CandidateClass >::const_partial_iterator DecayList< DecayClass , CandidateClass >::partial_particle_end() const
{
// Need to cast to 'const' to get the right function call
	 return ( (*(const LabeledParticleList< DecayClass , CandidateClass >*)(m_list.pointer())).end( this->label() ) ) ;
}



template < class DecayClass , class CandidateClass >
void DecayList< DecayClass , CandidateClass >::eraseContents()
{
   m_list->erase( m_list->begin() ,
		 m_list->end() ) ;
}


template < class DecayClass , class CandidateClass >
const LabeledCandidateList< CandidateClass >& DecayList< DecayClass , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}


template < class DecayClass , class CandidateClass >
const LabeledParticleList< DecayClass , CandidateClass >& DecayList< DecayClass , CandidateClass >::labeledParticleList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_DECAYLIST_CC */
