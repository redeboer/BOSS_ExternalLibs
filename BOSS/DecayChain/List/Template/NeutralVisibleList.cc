#ifndef DCHAIN_NEUTRALVISIBLELIST_CC
#define DCHAIN_NEUTRALVISIBLELIST_CC
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
// $Id: NeutralVisibleList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: NeutralVisibleList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:40  cdj
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
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/List/LabeledParticleList.h"

#include "DecayChain/List/NeutralVisibleList.h"

namespace dchain {
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
NeutralVisibleList< Neutral , CandidateClass >::
NeutralVisibleList() :
   dchain::ConjugateList< CandidateClass >( dchain::conjugation::kNone ) ,
   m_list( (new dchain::LabeledParticleList< Neutral , CandidateClass >) )
{
}

template< class Neutral , class CandidateClass >
NeutralVisibleList< Neutral , CandidateClass >::
NeutralVisibleList( const NeutralVisibleList< Neutral , CandidateClass >&  aOtherList ) :
   dchain::ConjugateList< CandidateClass >( dchain::conjugation::kNone ) ,
   m_list( (new dchain::LabeledParticleList< Neutral , CandidateClass >) )
{
   typedef typename MakerTrait<Neutral>::maker_type Maker;
  fill( *(aOtherList.m_list), Maker(), SelectAll<Neutral>() ) ;

}


template< class Neutral , class CandidateClass >
NeutralVisibleList< Neutral , CandidateClass >::
~NeutralVisibleList()
{
}

//
// assignment operators
//

template< class Neutral , class CandidateClass >
const NeutralVisibleList< Neutral , CandidateClass >& NeutralVisibleList< Neutral , CandidateClass >::operator=( const NeutralVisibleList< Neutral , CandidateClass >& aOtherList )
{
   eraseContents() ;
   typedef typename MakerTrait<Neutral>::maker_type Maker;
   fill( *(aOtherList.m_list), Maker(), SelectAll<Neutral>() ) ;
   return ( *this ) ;
}

//
// member functions
//

template< class Neutral , class CandidateClass >
typename NeutralVisibleList< Neutral , CandidateClass >::iterator NeutralVisibleList< Neutral , CandidateClass >::particle_begin()
{
   return ( m_list->begin() ) ;
}

template< class Neutral , class CandidateClass >
typename NeutralVisibleList< Neutral , CandidateClass >::iterator NeutralVisibleList< Neutral , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template< class Neutral , class CandidateClass >
dchain::LabeledParticleList< Neutral, CandidateClass >& NeutralVisibleList< Neutral , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}


template < class Neutral , class CandidateClass >
void NeutralVisibleList< Neutral , CandidateClass >::eraseContents()
{
   m_list->erase( m_list->begin() ,
		  m_list->end() ) ;
}

//
// const member functions
//

template< class Neutral , class CandidateClass >
typename NeutralVisibleList< Neutral , CandidateClass >::const_iterator NeutralVisibleList< Neutral , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Neutral , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template< class Neutral , class CandidateClass >
typename NeutralVisibleList< Neutral , CandidateClass >::const_iterator NeutralVisibleList< Neutral , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Neutral , CandidateClass >*)(m_list.pointer())).end() ) ;
}


template< class Neutral , class CandidateClass >
const dchain::LabeledParticleList< Neutral, CandidateClass >& NeutralVisibleList< Neutral , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_NEUTRALVISIBLELIST_CC */
