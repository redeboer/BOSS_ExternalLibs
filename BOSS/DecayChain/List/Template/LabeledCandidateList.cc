#ifndef DCHAIN_LABELEDCANDIDATELIST_CC
#define DCHAIN_LABELEDCANDIDATELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledCandidateList
// 
// Description: Container of `primary'/`conjugate' subclasses of Candidate
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 20:56:18 EDT 1996
// $Id: LabeledCandidateList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: LabeledCandidateList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:40  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:56  cdj
// imported DChain
//
// Revision 1.10  1998/08/21 00:51:56  sjp
// Modifier to no longer use typedefs
//
// Revision 1.9  1998/08/19 20:46:24  sjp
// Fixed comments and removed report references
//
// Revision 1.8  1997/08/28 07:00:21  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/26 04:14:27  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.6  1997/08/19 23:03:20  sjp
// Restructured package to be independent of Rock
//
// Revision 1.5  1997/08/15 21:33:47  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/31 20:16:27  sjp
// Modified to use the new `bug' include files
//
// Revision 1.3  1997/01/21 20:31:43  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1997/01/19 20:28:45  sjp
// Changed LargeCount to Count
//
// Revision 1.1  1996/11/04 17:17:19  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/LabeledCandidate.h"
#include "DecayChain/Iterator/MuteWholeCandidateItr.h"
#include "DecayChain/Iterator/WholeCandidateItr.h"
#include "DecayChain/Iterator/PartialCandidateItr.h"

#include "DecayChain/List/LabeledCandidateList.h"

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
template < class CandidateClass >
LabeledCandidateList< CandidateClass >::LabeledCandidateList()
{
}

// LabeledCandidateList< CandidateClass >::LabeledCandidateList( const LabeledCandidateList< CandidateClass >& )
// {
// }

template < class CandidateClass >
LabeledCandidateList< CandidateClass >::~LabeledCandidateList()
{
}

//
// assignment operators
//
// const LabeledCandidateList< CandidateClass >& LabeledCandidateList< CandidateClass >::operator=( const LabeledCandidateList< CandidateClass >& )
// {
// }

//
// member functions
//

//------ operator[] ------
//
template < class CandidateClass >
LabeledCandidate< CandidateClass >& LabeledCandidateList< CandidateClass >::operator[]( const int aIndex )
{
   return ( labeledCandidateClass( aIndex ) ) ;
}

//------ begin ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::iterator LabeledCandidateList< CandidateClass >::begin()
{
   return ( iterator( this , 0 ) ) ;
}

//------ end ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::iterator LabeledCandidateList< CandidateClass >::end()
{
   return ( iterator( this , size() ) ) ;
}

//------ erase ------
//
template < class CandidateClass >
void LabeledCandidateList< CandidateClass >::erase( iterator aStart ,
					    iterator aFinish )
{
   erase_elements( aStart.index() ,
		   aFinish.index() ) ;
}

//
// const member functions
//

//------ operator[] ------
//
template < class CandidateClass >
const LabeledCandidate< CandidateClass >& LabeledCandidateList< CandidateClass >::operator[]( const int aIndex ) const
{
   return ( labeledCandidateClass( aIndex ) ) ;
}

//------ begin ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::const_iterator LabeledCandidateList< CandidateClass >::begin() const
{
   return ( const_iterator( this , 0 ) ) ;
}

//------ begin ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::const_partial_iterator LabeledCandidateList< CandidateClass >::begin( const conjugation::Label aLabel ) const
{
   return ( const_partial_iterator( this , 0 , aLabel ) ) ;
}

//------ end ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::const_iterator LabeledCandidateList< CandidateClass >::end() const
{
   return ( const_iterator( this , size() ) ) ;
}

//------ end ------
//
template < class CandidateClass >
typename LabeledCandidateList< CandidateClass >::const_partial_iterator LabeledCandidateList< CandidateClass >::end( const conjugation::Label aLabel ) const
{
   return ( const_partial_iterator( this , size() , aLabel ) ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_LABELEDCANDIDATELIST_CC */
