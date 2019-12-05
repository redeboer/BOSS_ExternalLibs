#ifndef DCHAIN_CONJUGATELIST_CC
#define DCHAIN_CONJUGATELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      ConjugateList
// 
// Description: container of 'const Candidates*', used for partial iteration
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Sat Oct  5 17:18:02 EDT 1996
// $Id: ConjugateList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ConjugateList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:35  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.10  1998/08/21 00:51:55  sjp
// Modifier to no longer use typedefs
//
// Revision 1.9  1998/08/20 20:00:10  sjp
// Modified to use DChainBoolean
//
// Revision 1.8  1998/08/19 20:46:22  sjp
// Fixed comments and removed report references
//
// Revision 1.7  1998/04/17 19:14:50  sjp
// Modified to use latest type
//
// Revision 1.6  1997/08/28 07:00:38  sjp
// Modified files to handle complete templating
//
// Revision 1.5  1997/08/19 23:03:18  sjp
// Restructured package to be independent of Rock
//
// Revision 1.4  1997/08/15 21:33:42  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.3  1997/01/21 20:31:39  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1997/01/19 20:28:44  sjp
// Changed LargeCount to Count
//
// Revision 1.1  1996/11/04 17:17:15  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Function/DCAnalysisFunction.h"
#include "DecayChain/Function/DCConjugateFunction.h"
#include "DecayChain/Iterator/MuteWholeCandidateItr.h"
#include "DecayChain/Iterator/WholeCandidateItr.h"
#include "DecayChain/Iterator/PartialCandidateItr.h"

#include "DecayChain/List/ConjugateList.h"

//
// constants, enums and typedefs
//
namespace dchain {
//
// static data member definitions
//

//
// constructors and destructor
//

template < class CandidateClass >
ConjugateList< CandidateClass >::ConjugateList( const conjugation::Label aLabel ) :
   m_label( aLabel )
{
}

// ConjugateList< CandidateClass >::ConjugateList( const ConjugateList& )
// {
// }

template < class CandidateClass >
ConjugateList< CandidateClass >::~ConjugateList()
{
}

//
// assignment operators
//
// const ConjugateList& ConjugateList< CandidateClass >::operator=( const ConjugateList& )
// {
// }

//
// member functions
//

template < class CandidateClass >
void ConjugateList< CandidateClass >::setLabel( const conjugation::Label aLabel )
{
   m_label = aLabel ;
}

//
// const member functions
//

template < class CandidateClass >
bool ConjugateList< CandidateClass >::isSelfConjugate() const
{
   return ( conjugation::kNone == m_label ) ;
}

template < class CandidateClass >
bool ConjugateList< CandidateClass >::isConjugateOf( const ConjugateList< CandidateClass >& aList ) const
{
   return ( ( &(this->labeledCandidateList()) == &(aList.labeledCandidateList()) ) &&
	    ( label() == otherLabel( aList.label() ) ) ) ;
}

template < class CandidateClass >
int ConjugateList< CandidateClass >::partial_size() const
{
   return ( this->labeledCandidateList().size( m_label ) ) ;
}

template < class CandidateClass >
typename ConjugateList< CandidateClass >::const_partial_iterator ConjugateList< CandidateClass >::partial_begin() const
{
   return ( this->labeledCandidateList().begin( m_label ) ) ;
}

template < class CandidateClass >
typename ConjugateList< CandidateClass >::const_partial_iterator ConjugateList< CandidateClass >::partial_end() const
{
   return ( this->labeledCandidateList().end( m_label ) ) ;
}

template < class CandidateClass >
typename ConjugateList< CandidateClass >::const_partial_iterator ConjugateList< CandidateClass >::conjugate_partial_begin() const
{
   return ( this->labeledCandidateList().begin( otherLabel( m_label ) ) ) ;
}

template < class CandidateClass >
typename ConjugateList< CandidateClass >::const_partial_iterator ConjugateList< CandidateClass >::conjugate_partial_end() const
{
   return ( this->labeledCandidateList().end( otherLabel( m_label ) ) ) ;
}


template < class CandidateClass >
conjugation::Label ConjugateList< CandidateClass >::label() const
{
   return ( m_label ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_CONJUGATELIST_CC */
