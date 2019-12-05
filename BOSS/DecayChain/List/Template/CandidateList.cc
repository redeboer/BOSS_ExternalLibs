#ifndef DCHAIN_CANDIDATELIST_CC
#define DCHAIN_CANDIDATELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      CandidateList
// 
// Description: container of subclasses of Candidate, used for iteration
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 20:56:18 EDT 1996
// $Id: CandidateList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CandidateList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:34  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:54  cdj
// imported DChain
//
// Revision 1.11  1998/08/21 00:51:56  sjp
// Modifier to no longer use typedefs
//
// Revision 1.10  1998/08/20 20:00:15  sjp
// Modified to use DChainBoolean
//
// Revision 1.9  1998/08/19 20:46:27  sjp
// Fixed comments and removed report references
//
// Revision 1.8  1998/04/17 19:14:54  sjp
// Modified to use latest type
//
// Revision 1.7  1997/08/28 07:00:24  sjp
// Modified files to handle complete templating
//
// Revision 1.6  1997/08/26 04:14:34  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.5  1997/08/19 23:03:12  sjp
// Restructured package to be independent of Rock
//
// Revision 1.4  1997/08/15 21:33:33  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.3  1997/01/21 20:31:32  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1997/01/19 20:28:42  sjp
// Changed LargeCount to Count
//
// Revision 1.1  1996/11/04 17:17:10  sjp
// New file for new `List' module
//

// system include files
//#include <string>

// user include files
#include "DecayChain/Element/LabeledCandidate.h"
#include "DecayChain/Iterator/MuteWholeCandidateItr.h"
#include "DecayChain/Iterator/WholeCandidateItr.h"
#include "DecayChain/List/LabeledCandidateList.h"

#include "DecayChain/List/CandidateList.h"

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
CandidateList< CandidateClass >::CandidateList()
{
}

// CandidateList< CandidateClass >::CandidateList( const CandidateList< CandidateClass >& )
// {
// }

template < class CandidateClass >
CandidateList< CandidateClass >::~CandidateList()
{
}

//
// assignment operators
//
// const CandidateList< CandidateClass >& CandidateList< CandidateClass >::operator=( const CandidateList< CandidateClass >& )
// {
// }

//
// member functions
//

//------ begin ------
//
template < class CandidateClass >
typename CandidateList< CandidateClass >::iterator CandidateList< CandidateClass >::begin()
{
   return ( labeledCandidateList().begin() ) ;
}

//------ end ------
//
template < class CandidateClass >
typename CandidateList< CandidateClass >::iterator CandidateList< CandidateClass >::end()
{
   return ( labeledCandidateList().end() ) ;
}

//
// const member functions
//

//------ empty ------
//
template < class CandidateClass >
bool CandidateList< CandidateClass >::empty() const
{
   return ( labeledCandidateList().empty() ) ;
}

//------ size ------
//
template < class CandidateClass >
int CandidateList< CandidateClass >::size() const
{
   return ( labeledCandidateList().size() ) ;
}

//------ begin ------
//
template < class CandidateClass >
typename CandidateList< CandidateClass >::const_iterator CandidateList< CandidateClass >::begin() const
{
// do this casting to make sure const 'begin' function is called
   return ( static_cast<const dchain::LabeledCandidateList< CandidateClass >*>(&labeledCandidateList())->begin() );
}

//------ end ------
//
template < class CandidateClass >
typename CandidateList< CandidateClass >::const_iterator CandidateList< CandidateClass >::end() const
{
// do this casting to make sure const 'end' function is called
   return ( static_cast<const dchain::LabeledCandidateList< CandidateClass >*>(&labeledCandidateList())->end() );
}

//
// static member functions
//
}

#endif /* DCHAIN_DCCANDIDATELIST_CC */
