#ifndef DCHAIN_FILLABLENEUTRALNCLIST_CC
#define DCHAIN_FILLABLENEUTRALNCLIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableNeutralNCList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Sep  3 13:47:97 EDT 1997
// $Id: DCFillableNeutralNCList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableNeutralNCList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:38  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1  2001/09/11 15:02:43  ajm36
// add files to do neutral non-conjugating particles
//
// Revision 1.2  2000/12/30 19:43:33  cdj
// no more missing symbols under OSF1
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.4  1998/08/20 20:00:12  sjp
// Modified to use DChainBoolean
//
// Revision 1.3  1998/08/19 20:46:23  sjp
// Fixed comments and removed report references
//
// Revision 1.2  1998/04/17 19:14:50  sjp
// Modified to use latest type
//
// Revision 1.1  1997/09/17 20:06:41  sjp
// New class to separate filling from behaviour
//

// system include files
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/List/DCFillableNeutralNCList.h"

// STL include files

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

template < class NeutralNC >
DCFillableNeutralNCList< NeutralNC >::
DCFillableNeutralNCList( bool ( * pFunction )( NeutralNC& ) ) :
   DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >(
      pFunction )
{
}

template < class NeutralNC >
DCFillableNeutralNCList< NeutralNC >::
DCFillableNeutralNCList(const DCFillableNeutralNCList< NeutralNC >& aOtherList,
                        bool ( * pFunction )( NeutralNC& ) ) :
   DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >(
      aOtherList, pFunction )
{
}

template < class NeutralNC >
DCFillableNeutralNCList< NeutralNC >::
DCFillableNeutralNCList( DCSelectionFunction< NeutralNC >& aFunctionalObject ):
   DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >(
      aFunctionalObject )
{
}

template < class NeutralNC >
DCFillableNeutralNCList< NeutralNC >::
DCFillableNeutralNCList(const DCFillableNeutralNCList< NeutralNC >& aOtherList,
                        DCSelectionFunction< NeutralNC >& aFunctionalObject ) :
   DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >(
      aOtherList, aFunctionalObject )
{
}

template < class NeutralNC >
DCFillableNeutralNCList< NeutralNC >::
~DCFillableNeutralNCList()
{
}

//
// assignment operators
//

template < class NeutralNC >
const DCFillableNeutralNCList<NeutralNC>& DCFillableNeutralNCList<NeutralNC>::
operator=( const DCFillableNeutralNCList< NeutralNC >& aOtherList )
{
   DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >::
      operator=( aOtherList ) ;
   return ( *this ) ;
}

//
// member functions
//

//
// const member functions
//

//
// static member functions
//

#endif /* DCHAIN_FILLABLENEUTRALNCLIST_CC */
