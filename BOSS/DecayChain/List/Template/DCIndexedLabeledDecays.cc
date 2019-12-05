#ifndef DCHAIN_INDEXEDLABELEDDECAYS_CC
#define DCHAIN_INDEXEDLABELEDDECAYS_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      IndexedLabeledDecays
// 
// Description: Interface used by Decay iterators
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Mon Nov  4 12:19:24 EST 1996
// $Id: DCIndexedLabeledDecays.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCIndexedLabeledDecays.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:38  cdj
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
#include "DecayChain/List/IndexedLabeledDecays.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

template < class DecayClass , class CandidateClass >
IndexedLabeledDecays< DecayClass , CandidateClass >::IndexedLabeledDecays()
{
}

// IndexedLabeledDecays::IndexedLabeledDecays( const IndexedLabeledDecays& )
// {
// }

template < class DecayClass , class CandidateClass >
IndexedLabeledDecays< DecayClass , CandidateClass >::~IndexedLabeledDecays()
{
}

//
// assignment operators
//
// const IndexedLabeledDecays& IndexedLabeledDecays::operator=( const IndexedLabeledDecays& )
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

#endif /* DCHAIN_INDEXEDLABELEDDECAYS_CC */
