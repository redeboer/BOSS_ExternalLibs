#ifndef DCHAIN_COMBINATORICLOOP_CC
#define DCHAIN_COMBINATORICLOOP_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      CombinatoricLoop
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Mon Oct 21 10:06:47 EDT 1996
// $Id: CombinatoricLoop.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CombinatoricLoop.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.2  2006/01/12 13:44:47  cdj
// missed a name change
//
// Revision 1.1  2006/01/11 20:28:35  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.10  1998/08/20 20:00:10  sjp
// Modified to use DChainBoolean
//
// Revision 1.9  1998/08/19 20:46:22  sjp
// Fixed comments and removed report references
//
// Revision 1.8  1997/09/03 14:58:23  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.7  1997/08/28 07:00:37  sjp
// Modified files to handle complete templating
//
// Revision 1.6  1997/08/19 23:03:16  sjp
// Restructured package to be independent of Rock
//
// Revision 1.5  1997/08/19 16:19:06  sjp
// Improved handling of STL containers
//
// Revision 1.4  1997/08/15 21:33:38  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.3  1997/01/31 20:16:26  sjp
// Modified to use the new `bug' include files
//
// Revision 1.2  1997/01/21 20:31:37  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.1  1996/11/04 17:17:14  sjp
// New file for new `List' module
//

// system include files
#include <stdlib.h>  // For 'exit'

// user include files
#include "DecayChain/Iterator/PartialCandidateItr.h"
#include "DecayChain/List/CombinatoricLoop.h"

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
// CombinatoricLoop< CandidateClass >::CombinatoricLoop()
// {
// }

// CombinatoricLoop< CandidateClass >::CombinatoricLoop( const CombinatoricLoop< CandidateClass >& )
// {
// }

template< class CandidateClass >
CombinatoricLoop< CandidateClass >::CombinatoricLoop( const typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aBegin ,
				    const typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aEnd ,
				    _combinatoricloop_vector_& aLoopList ) :
   m_beginIterator( aBegin ) ,
   m_endIterator( aEnd ) ,
   m_previousDuplicate( 0 )
{
   typename _combinatoricloop_vector_::const_iterator lastLoop( aLoopList.begin() ) ;
   typename _combinatoricloop_vector_::const_iterator loopToCheck( aLoopList.end() ) ;
   while ( loopToCheck != lastLoop ) {
      --loopToCheck ;
      if ( ( (*(*loopToCheck)).m_beginIterator ) == m_beginIterator ) {
	 m_previousDuplicate = new typename _combinatoricloop_vector_::const_iterator( loopToCheck ) ;
	 (*(*(*m_previousDuplicate))).adjustEnd() ;
//
// only require one match, so prepare to terminate search for duplicate
//
	 loopToCheck = lastLoop ;
      }
   } 
}

template< class CandidateClass >
CombinatoricLoop< CandidateClass >::~CombinatoricLoop()
{
   delete m_previousDuplicate ;
}

//
// assignment operators
//
// const CombinatoricLoop< CandidateClass >& CombinatoricLoop< CandidateClass >::operator=( const CombinatoricLoop< CandidateClass >& )
// {
// }

//
// member functions
//

template< class CandidateClass >
void CombinatoricLoop< CandidateClass >::setCurrentIterator( typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aIterator )
{
   m_currentIterator = aIterator ;
}

//
// const member functions
//

template< class CandidateClass >
const typename dchain::CandidateList< CandidateClass >::const_partial_iterator CombinatoricLoop< CandidateClass >::partial_begin() const
{
   if ( 0 == m_previousDuplicate ) {
      return( m_beginIterator ) ;
   }
   typename dchain::CandidateList< CandidateClass >::const_partial_iterator newBegin( ( (*(*(*m_previousDuplicate))).m_currentIterator ) ) ;
   return( ++newBegin ) ;
}

template< class CandidateClass >
const typename dchain::CandidateList< CandidateClass >::const_partial_iterator CombinatoricLoop< CandidateClass >::partial_end() const
{
   return( m_endIterator ) ;
}

template< class CandidateClass >
void CombinatoricLoop< CandidateClass >::adjustEnd()
{
   if ( m_beginIterator != m_endIterator ) {
      --m_endIterator ;
      if ( 0 != m_previousDuplicate ) {
	 (*(*(*m_previousDuplicate))).adjustEnd() ;
      }     
   }
}

//
// static member functions
//
}
#endif /* DCHAIN_COMBINATORICLOOP_CC */
