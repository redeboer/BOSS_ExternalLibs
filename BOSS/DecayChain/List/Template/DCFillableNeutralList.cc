#ifndef DCHAIN_FILLABLENEUTRALLIST_CC
#define DCHAIN_FILLABLENEUTRALLIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableNeutralList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: DCFillableNeutralList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableNeutralList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/11 20:28:38  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:56  cdj
// imported DChain
//
// Revision 1.4  1998/08/20 20:00:12  sjp
// Modified to use DChainBoolean
//
// Revision 1.3  1998/08/19 20:46:23  sjp
// Fixed comments and removed report references
//
// Revision 1.2  1998/04/17 19:14:52  sjp
// Modified to use latest type
//
// Revision 1.1  1997/09/17 20:06:43  sjp
// New class to separate filling from behaviour
//

// system include files
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/List/DCFillableNeutralList.h"

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

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
DCFillableNeutralList< Neutral >::
DCFillableNeutralList( bool ( * pFunction )( Neutral& ) ) :
   DCNeutralVisibleList< Neutral , typename Neutral::CandidateClass >( pFunction )
{
}

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//DCFillableNeutralList( const TruthWitnesses& aTruthList ,
//		     bool ( * pFunction )( Neutral& ) ) :
//   DCNeutralVisibleList< Neutral , Neutral::CandidateClass >( pFunction )
//{
//   fill( aTruthList ) ;
//}

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//DCFillableNeutralList( const VisibleWitnesses& aVisibleList ,
//		     bool ( * pFunction )( Neutral& ) ) :
//   DCNeutralVisibleList< Neutral , Neutral::CandidateClass >( pFunction )
//{
//   fill( aVisibleList ) ;
//}

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
DCFillableNeutralList< Neutral >::
DCFillableNeutralList( const DCFillableNeutralList< Neutral >&  aOtherList ,
		     bool ( * pFunction )( Neutral& ) ) :
   DCNeutralVisibleList< Neutral , 
                         typename Neutral::CandidateClass >( aOtherList ,
							pFunction )
{
}

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
DCFillableNeutralList< Neutral >::
DCFillableNeutralList( DCSelectionFunction< Neutral >& aFunctionalObject ) :
   DCNeutralVisibleList< Neutral , 
                         typename Neutral::CandidateClass >( aFunctionalObject )
{
}

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//DCFillableNeutralList( const TruthWitnesses& aTruthList ,
//		     DCSelectionFunction< Neutral >& aFunctionalObject ) :
//   DCNeutralVisibleList< Neutral , Neutral::CandidateClass >( aFunctionalObject )
//{
//   fill( aTruthList ) ;
//}

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//DCFillableNeutralList( const VisibleWitnesses& aVisibleList ,
//		     DCSelectionFunction< Neutral >& aFunctionalObject ) :
//   DCNeutralVisibleList< Neutral , Neutral::CandidateClass >( aFunctionalObject )
//{
//   fill( aVisibleList ) ;
//}

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
DCFillableNeutralList< Neutral  >::
DCFillableNeutralList( const DCFillableNeutralList< Neutral >& aOtherList ,
		     DCSelectionFunction< Neutral >& aFunctionalObject ) :
   DCNeutralVisibleList< Neutral , 
                         typename Neutral::CandidateClass >( aOtherList ,
							aFunctionalObject )
{
}
/*
template< class Neutral >
DCFillableNeutralList< Neutral >::
DCFillableNeutralList( const vector<Neutral>& aVector )
{
   fill(aVector);
}
*/

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
DCFillableNeutralList< Neutral >::
~DCFillableNeutralList()
{
}

//
// assignment operators
//

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//const DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >& DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//operator=( const TruthWitnesses& aTruthList )
//{
//   eraseContents() ;
//   fill( aTruthList ) ;
//   return ( *this ) ;
//}

//template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//const DCFillableNeutralList< Neutral, TruthWitnesses, VisibleWitnesses >& DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::
//operator=( const VisibleWitnesses& aVisibleList )
//{
//   eraseContents() ;
//   fill( aVisibleList ) ;
//   return ( *this ) ;
//}

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
const DCFillableNeutralList< Neutral >& 
DCFillableNeutralList< Neutral >::operator=( 
   const DCFillableNeutralList< Neutral >& aOtherList )
{
   DCNeutralVisibleList< Neutral , 
                         typename Neutral::CandidateClass >::operator=( 
			    aOtherList ) ;
   return ( *this ) ;
}

//
// member functions
//

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//void DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::fill( const TruthWitnesses& aList )
//{
//   TruthWitnesses::const_iterator finished ( aList.end() ) ;
//   for ( TruthWitnesses::const_iterator entry ( aList.begin() ) ;
//	 entry != finished ;
//	 ++entry ) {
//      Neutral* localCopy = new Neutral( *entry ) ;
//      insert_possible( localCopy ) ;
//   }
//}

//template< class Neutral ,
//          class TruthWitnesses ,
//          class VisibleWitnesses >
//void DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >::fill( const VisibleWitnesses& aList )
//{
//   VisibleWitnesses::const_iterator finished ( aList.end() ) ;
//   for ( VisibleWitnesses::const_iterator entry ( aList.begin() ) ;
//	 entry != finished ;
//	 ++entry ) {
//      Neutral* localCopy = new Neutral( *entry ) ;
//      insert_possible( localCopy ) ;
//   }
//}

template< class Neutral >
void 
DCFillableNeutralList< Neutral >::fill( 
   const dchain::LabeledParticleList< Neutral , 
                             typename Neutral::CandidateClass >& aList )
{
   DCNeutralVisibleList< Neutral , 
                         typename Neutral::CandidateClass >::fill( aList ) ;
}


//
// const member functions
//

//
// static member functions
//

#endif /* DCHAIN_FILLABLENEUTRALLIST_CC */
