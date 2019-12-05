#ifndef DCHAIN_CHOSENCONJUGATELIST_CC
#define DCHAIN_CHOSENCONJUGATELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChosenConjugateList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: ChosenConjugateList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChosenConjugateList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:34  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/Function/DCAnalysisFunction.h"
#include "DecayChain/Function/DCConjugateFunction.h"
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Iterator/PartialItr.h"
#include "DecayChain/List/LabeledParticleList.h"

#include "DecayChain/List/ChosenConjugateList.h"

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
template < class Conjugate , class CandidateClass >
ChosenConjugateList< Conjugate , CandidateClass >::ChosenConjugateList( LabeledParticleList< Conjugate , CandidateClass >& aLabeledParticleList ,
							    const conjugation::Label aLabel ) :
   ConjugateList< CandidateClass >( aLabel ) ,
   m_list( &aLabeledParticleList )
{
}

template < class Conjugate , class CandidateClass >
ChosenConjugateList< Conjugate , CandidateClass >::~ChosenConjugateList()
{
}

//
// member functions
//
template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::iterator ChosenConjugateList< Conjugate , CandidateClass >::particle_begin()
{
   return ( m_list->begin() ) ;
}

template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::iterator ChosenConjugateList< Conjugate , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template < class Conjugate , class CandidateClass >
LabeledCandidateList< CandidateClass >& ChosenConjugateList< Conjugate , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}

//
// const member functions
//
template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::const_iterator ChosenConjugateList< Conjugate , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Conjugate , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::const_iterator ChosenConjugateList< Conjugate , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Conjugate , CandidateClass >*)(m_list.pointer())).end() ) ;
}

template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::const_partial_iterator ChosenConjugateList< Conjugate , CandidateClass >::partial_particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Conjugate , CandidateClass >*)(m_list.pointer())).begin( this->label() ) ) ;
}

template < class Conjugate , class CandidateClass >
typename ChosenConjugateList< Conjugate , CandidateClass >::const_partial_iterator ChosenConjugateList< Conjugate , CandidateClass >::partial_particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Conjugate , CandidateClass >*)(m_list.pointer())).end( this->label() ) ) ;
}


template < class Conjugate , class CandidateClass >
const LabeledCandidateList< CandidateClass >& ChosenConjugateList< Conjugate , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_CHOSENCONJUGATELIST_CC */
