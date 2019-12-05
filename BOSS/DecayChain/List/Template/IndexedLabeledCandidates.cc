#ifndef DCHAIN_INDEXEDLABELEDCANDIDATES_CC
#define DCHAIN_INDEXEDLABELEDCANDIDATES_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      IndexedLabeledCandidates
// 
// Description: Interface used by Candidate iterators
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Mon Nov  4 12:19:24 EST 1996
// $Id: IndexedLabeledCandidates.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: IndexedLabeledCandidates.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:39  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:56  cdj
// imported DChain
//
// Revision 1.1  1998/08/28 05:17:29  sjp
// New class for iterator access
//

// system include files

// user include files
#include "DecayChain/List/IndexedLabeledCandidates.h"

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
IndexedLabeledCandidates< CandidateClass >::IndexedLabeledCandidates()
{
}

// IndexedLabeledCandidates::IndexedLabeledCandidates( const IndexedLabeledCandidates& )
// {
// }

template < class CandidateClass >
IndexedLabeledCandidates< CandidateClass >::~IndexedLabeledCandidates()
{
}

//
// assignment operators
//
// const IndexedLabeledCandidates& IndexedLabeledCandidates::operator=( const IndexedLabeledCandidates& )
// {
// }

//
// member functions
//

//
// const member functions
//

//
// static member functions
//
}

#endif /* DCHAIN_INDEXEDLABELEDCANDIDATES_CC */

