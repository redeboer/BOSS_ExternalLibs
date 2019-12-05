#ifndef DCHAIN_DCNEUTRALVISIBLELIST_CC
#define DCHAIN_DCNEUTRALVISIBLELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralVisibleList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: DCNeutralVisibleList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCNeutralVisibleList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.3  2006/01/11 20:28:39  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2003/05/15 19:56:13  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.1.1.1  2000/12/18 22:16:56  cdj
// imported DChain
//
// Revision 1.14  1998/08/20 20:00:14  sjp
// Modified to use DChainBoolean
//
// Revision 1.13  1998/08/19 20:46:26  sjp
// Fixed comments and removed report references
//
// Revision 1.12  1998/04/17 19:14:54  sjp
// Modified to use latest type
//
// Revision 1.11  1997/09/17 20:05:36  sjp
// Removed filling to superclass
//
// Revision 1.10  1997/09/03 14:58:28  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.9  1997/08/28 07:00:23  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/19 23:02:39  sjp
// Restructured package to be independent of Rock
//
// Revision 1.7  1997/08/19 15:55:29  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:33:55  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:16:29  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/21 20:31:50  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1996/12/20 21:14:03  sjp
// Changed aPossible to Possible and added dropLink where nec.
//
// Revision 1.2  1996/11/05 19:31:31  sjp
// Added definition for filling for TruthTable.
//
// Revision 1.1  1996/11/04 17:17:22  sjp
// New file for new `List' module
//

// system include files
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/Function/DCSelectionFunction.h"
#include "DecayChain/Function/DCAnalysisFunction.h"
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/List/LabeledParticleList.h"

#include "DecayChain/List/DCNeutralVisibleList.h"

// STL include files
//#include <vector>

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//

template< class Neutral , class CandidateClass >
DCNeutralVisibleList< Neutral , CandidateClass >::
DCNeutralVisibleList( bool ( * pFunction )( Neutral& ) ) :
   m_function( pFunction ) ,
   m_functionalObject( 0 )
{
}

template< class Neutral , class CandidateClass >
DCNeutralVisibleList< Neutral , CandidateClass >::
DCNeutralVisibleList( const DCNeutralVisibleList< Neutral , CandidateClass >&  aOtherList ,
		    bool ( * pFunction )( Neutral& ) ) :
   dchain::NeutralVisibleList< Neutral, CandidateClass >( aOtherList, pFunction ) ,
   m_function( pFunction ) ,
   m_functionalObject( 0 )
{
}

template< class Neutral , class CandidateClass >
DCNeutralVisibleList< Neutral , CandidateClass >::
DCNeutralVisibleList( DCSelectionFunction< Neutral >& aFunctionalObject ) :
   m_function( 0 ) ,
   m_functionalObject( &aFunctionalObject )
{
}

template< class Neutral , class CandidateClass >
DCNeutralVisibleList< Neutral , CandidateClass >::
DCNeutralVisibleList( const DCNeutralVisibleList< Neutral , CandidateClass >& aOtherList ,
		    DCSelectionFunction< Neutral >& aFunctionalObject ) :
   dchain::NeutralVisibleList< Neutral,CandidateClass >( aOtherList, aFunctionalObject),
   m_function( 0 ) ,
   m_functionalObject( &aFunctionalObject )
{
}

template< class Neutral , class CandidateClass >
DCNeutralVisibleList< Neutral , CandidateClass >::
~DCNeutralVisibleList()
{
}

//
// assignment operators
//

template< class Neutral , class CandidateClass >
const DCNeutralVisibleList< Neutral , CandidateClass >& DCNeutralVisibleList< Neutral , CandidateClass >::operator=( const DCNeutralVisibleList< Neutral , CandidateClass >& aOtherList )
{
   this->eraseContents() ;
   fill( aOtherList.labeledCandidateList() ) ;
   return ( *this ) ;
}

template < class Neutral , class CandidateClass >
void DCNeutralVisibleList< Neutral , CandidateClass >::fill( const dchain::LabeledParticleList< Neutral , CandidateClass >& aList )
{
   if ( 0 != m_function ) {
      fill(aList, dchain::NewMaker<Neutral>(), m_function);
   } else if (m_functionalObject) {
      fill(aList,dchain::NewMaker<Neutral>(), m_functionalObject);
   } else {
      fill(aList, dchain::NewMaker<Neutral>(), dchain::SelectAll<Neutral>());
   }
}

//
// member functions
//

//
// static member functions
//

#endif /* DCHAIN_DCNEUTRALVISIBLELIST_CC */
