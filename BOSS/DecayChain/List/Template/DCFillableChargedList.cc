#ifndef DCHAIN_FILLABLECHARGEDLIST_CC
#define DCHAIN_FILLABLECHARGEDLIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableChargedList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Sep  3 13:47:97 EDT 1997
// $Id: DCFillableChargedList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableChargedList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.3  2006/01/11 20:28:38  cdj
// massive class renaming, addition of [] for selection and unit tests
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
#include "DecayChain/List/DCFillableChargedList.h"

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

template < class Charged >
DCFillableChargedList< Charged >::
DCFillableChargedList( bool ( * pFunction )( Charged& ) ) :
   DCChargedVisibleList< Charged , typename Charged::CandidateClass >( pFunction )
{
}

template < class Charged >
DCFillableChargedList< Charged >::
DCFillableChargedList( const DCFillableChargedList< Charged >&  aOtherList ,
		     bool ( * pFunction )( Charged& ) ) :
   DCChargedVisibleList< Charged , typename Charged::CandidateClass >( aOtherList ,
							pFunction )
{
}

template < class Charged >
DCFillableChargedList< Charged >::
DCFillableChargedList( DCSelectionFunction< Charged >& aFunctionalObject ) :
   DCChargedVisibleList< Charged , typename Charged::CandidateClass >( aFunctionalObject )
{
}

template < class Charged >
DCFillableChargedList< Charged >::
DCFillableChargedList( const DCFillableChargedList< Charged >& aOtherList ,
		     DCSelectionFunction< Charged >& aFunctionalObject ) :
   DCChargedVisibleList< Charged , typename Charged::CandidateClass >( aOtherList ,
					       aFunctionalObject )
{
}

template < class Charged >
DCFillableChargedList< Charged >::
~DCFillableChargedList()
{
}

//
// assignment operators
//

template < class Charged >
const DCFillableChargedList< Charged >& DCFillableChargedList< Charged >::
operator=( const DCFillableChargedList< Charged >& aOtherList )
{
   DCChargedVisibleList< Charged , typename Charged::CandidateClass >::operator=( aOtherList ) ;
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

#endif /* DCHAIN_FILLABLECHARGEDLIST_CC */
