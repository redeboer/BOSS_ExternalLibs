#ifndef DCHAIN_DCCHARGEDVISIBLELIST_CC
#define DCHAIN_DCCHARGEDVISIBLELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChargedVisibleList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: DCChargedVisibleList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCChargedVisibleList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.6  2006/01/11 20:28:36  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.5  2003/08/01 13:48:43  mrs43
// changed plus and minus methods of DCChargedVisibleList to const
//
// Revision 1.4  2003/05/15 19:56:11  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.2  2000/12/30 19:43:32  cdj
// no more missing symbols under OSF1
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.14  1998/08/20 20:00:08  sjp
// Modified to use DChainBoolean
//
// Revision 1.13  1998/08/19 20:46:19  sjp
// Fixed comments and removed report references
//
// Revision 1.12  1998/04/17 19:14:46  sjp
// Modified to use latest type
//
// Revision 1.11  1997/09/17 20:05:34  sjp
// Removed filling to superclass
//
// Revision 1.10  1997/09/03 14:58:18  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.9  1997/08/28 07:00:17  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/19 23:02:32  sjp
// Restructured package to be independent of Rock
//
// Revision 1.7  1997/08/19 15:55:28  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:33:00  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:28  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/21 20:34:26  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1996/12/20 21:15:57  sjp
// Ammended to allow for LabaledLists being reference counted.
// Added dropLink() for all created entries.
//
// Revision 1.2  1996/11/05 19:30:35  sjp
// Added definition for filling for TruthTable.
//
// Revision 1.1  1996/11/04 19:36:39  sjp
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
#include "DecayChain/List/ChosenChargeList.h"

#include "DecayChain/List/DCChargedVisibleList.h"

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

template < class Charged , class CandidateClass >
DCChargedVisibleList< Charged , CandidateClass >::
DCChargedVisibleList( bool ( * pFunction )( Charged& ) ) :
   m_function( pFunction ) ,
   m_functionalObject( 0 )
{
}

template < class Charged , class CandidateClass >
DCChargedVisibleList< Charged , CandidateClass >::
DCChargedVisibleList( const DCChargedVisibleList< Charged , CandidateClass >&  aOtherList ,
		    bool ( * pFunction )( Charged& ) ) :
   dchain::ChargedVisibleList<Charged, CandidateClass>(aOtherList, pFunction),
   m_function( pFunction ) ,
   m_functionalObject( 0 )
{
}

template < class Charged , class CandidateClass >
DCChargedVisibleList< Charged , CandidateClass >::
DCChargedVisibleList( DCSelectionFunction< Charged >& aFunctionalObject ) :
   m_function( 0 ) ,
   m_functionalObject( &aFunctionalObject )
{
}

template < class Charged , class CandidateClass >
DCChargedVisibleList< Charged , CandidateClass >::
DCChargedVisibleList( const DCChargedVisibleList< Charged , CandidateClass >& aOtherList ,
		    DCSelectionFunction< Charged >& aFunctionalObject ) :
   dchain::ChargedVisibleList<Charged,CandidateClass>(aOtherList,aFunctionalObject),
   m_function( 0 ) ,
   m_functionalObject( &aFunctionalObject )
{
}

template < class Charged , class CandidateClass >
DCChargedVisibleList< Charged , CandidateClass >::~DCChargedVisibleList()
{
}

//
// assignment operators
//

template < class Charged , class CandidateClass >
const DCChargedVisibleList< Charged , CandidateClass >& DCChargedVisibleList< Charged , CandidateClass >::operator=( const DCChargedVisibleList< Charged , CandidateClass >& aOtherList )
{
   this->eraseContents() ;
   fill(aOtherList.labeledCandidateList()) ;
   return ( *this ) ;
}

//
// member functions
//

template < class Charged , class CandidateClass >
void DCChargedVisibleList< Charged , CandidateClass >::fill( const dchain::LabeledParticleList< Charged , CandidateClass >& aList )
{
   if ( 0 != m_function ) {
      fill(aList, dchain::NewMaker<Charged>(), m_function);
   } else if (m_functionalObject) {
      fill(aList,dchain::NewMaker<Charged>(), m_functionalObject);
   } else {
      fill(aList, dchain::NewMaker<Charged>(), dchain::SelectAll<Charged>());
   }
}

//
// const member functions
//

//
// static member functions
//

#endif /* DCHAIN_DCCHARGEDVISIBLELIST_CC */
