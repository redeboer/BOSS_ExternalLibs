#ifndef DCHAIN_NEUTRALNCVISIBLELIST_CC
#define DCHAIN_NEUTRALNCVISIBLELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralNCVisibleList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: NeutralNCVisibleList.cc,v 1.2 2009/06/22 08:38:08 zoujh Exp $
//
// Revision history
//
// $Log: NeutralNCVisibleList.cc,v $
// Revision 1.2  2009/06/22 08:38:08  zoujh
// See ChangeLog
//
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:40  cdj
// massive class renaming, addition of [] for selection and unit tests
//
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
#include "DecayChain/List/ChosenConjugateList.h"

#include "DecayChain/List/NeutralNCVisibleList.h"

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

template < class NeutralNC , class CandidateClass >
NeutralNCVisibleList< NeutralNC , CandidateClass >::
NeutralNCVisibleList() :
   m_list( (new dchain::LabeledParticleList< NeutralNC , CandidateClass >) ),
   m_ntBarList(0),
   m_isBarList(0)
{
}

template < class NeutralNC , class CandidateClass >
NeutralNCVisibleList< NeutralNC , CandidateClass >::
NeutralNCVisibleList(const NeutralNCVisibleList<NeutralNC, CandidateClass>&
		     aOtherList ) :
   m_list( (new dchain::LabeledParticleList< NeutralNC , CandidateClass >) ),
   m_ntBarList(0),
   m_isBarList(0)
{
   typedef typename MakerTrait<NeutralNC>::maker_type Maker;
  fill( *(aOtherList.m_list), Maker(), SelectAll<NeutralNC>() ) ;
}


template < class NeutralNC , class CandidateClass >
NeutralNCVisibleList< NeutralNC , CandidateClass >::~NeutralNCVisibleList()
{
   delete m_ntBarList;
   delete m_isBarList;
}

//
// assignment operators
//

/*
template < class NeutralNC , class CandidateClass >
const NeutralNCVisibleList< NeutralNC , CandidateClass >& NeutralNCVisibleList< NeutralNC , CandidateClass >::operator=( const NeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList )
{
   eraseContents() ;
   typedef typename MakerTrait<NeutralNC>::maker_type Maker;
   fill( *(aOtherList.m_list), Maker(), SelectAll<NeutralNC>() ) ;
   return ( *this ) ;
}
*/

//
// member functions
//

template < class NeutralNC , class CandidateClass >
typename NeutralNCVisibleList< NeutralNC , CandidateClass >::iterator
NeutralNCVisibleList< NeutralNC , CandidateClass >::particle_begin()
{
   return ( m_list->begin() ) ;
}

template < class NeutralNC , class CandidateClass >
typename NeutralNCVisibleList< NeutralNC , CandidateClass >::iterator
NeutralNCVisibleList< NeutralNC , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template < class NeutralNC , class CandidateClass >
dchain::ChosenConjugateList< NeutralNC , CandidateClass >&
NeutralNCVisibleList< NeutralNC , CandidateClass >::operator()()
{
   if ( m_ntBarList == 0 ) {
      m_ntBarList = new ChosenConjugateList< NeutralNC, CandidateClass >(*m_list, kIsntBar);
   }

   return (*m_ntBarList);
}

template < class NeutralNC , class CandidateClass >
dchain::ChosenConjugateList< NeutralNC , CandidateClass >&
NeutralNCVisibleList< NeutralNC , CandidateClass >::bar()
{
   if ( m_isBarList == 0 ) {
      m_isBarList = new ChosenConjugateList< NeutralNC, CandidateClass >(*m_list, kIsBar);
   }

   return (*m_isBarList);
}

template < class NeutralNC , class CandidateClass >
dchain::LabeledParticleList< NeutralNC, CandidateClass >&
NeutralNCVisibleList< NeutralNC , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}


template < class NeutralNC , class CandidateClass >
void NeutralNCVisibleList< NeutralNC , CandidateClass >::eraseContents()
{
   m_list->erase( m_list->begin() ,
		  m_list->end() ) ;
}

//
// const member functions
//

template < class NeutralNC , class CandidateClass >
typename NeutralNCVisibleList< NeutralNC , CandidateClass >::const_iterator
NeutralNCVisibleList< NeutralNC , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< NeutralNC , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template < class NeutralNC , class CandidateClass >
typename NeutralNCVisibleList< NeutralNC , CandidateClass >::const_iterator
NeutralNCVisibleList< NeutralNC , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< NeutralNC , CandidateClass >*)(m_list.pointer())).end() ) ;
}


template < class NeutralNC , class CandidateClass >
const dchain::LabeledParticleList< NeutralNC, CandidateClass >&
NeutralNCVisibleList< NeutralNC , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_NEUTRALNCVISIBLELIST_CC */
