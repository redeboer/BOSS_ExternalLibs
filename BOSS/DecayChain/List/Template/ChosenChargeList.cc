#ifndef DCHAIN_CHOSENCHARGELIST_CC
#define DCHAIN_CHOSENCHARGELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChosenChargeList
// 
// Description: <one line class summary>
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:56 EDT 1996
// $Id: ChosenChargeList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChosenChargeList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:34  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.3  2003/05/15 19:56:11  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.2  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.1.1.1  2000/12/18 22:16:55  cdj
// imported DChain
//
// Revision 1.12  1998/08/20 20:00:09  sjp
// Modified to use DChainBoolean
//
// Revision 1.11  1998/08/19 20:46:20  sjp
// Fixed comments and removed report references
//
// Revision 1.10  1998/04/17 19:14:47  sjp
// Modified to use latest type
//
// Revision 1.9  1997/09/03 14:58:20  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.8  1997/08/28 07:00:19  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 23:02:34  sjp
// Restructured package to be independent of Rock
//
// Revision 1.6  1997/08/19 16:16:48  sjp
// Changed Template's Parameter for symmetry with Neutrals
//
// Revision 1.5  1997/08/15 21:33:02  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/31 20:17:30  sjp
// Modified to use the new `bug' include files
//
// Revision 1.3  1997/01/21 20:34:29  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:15:59  sjp
// Ammended to allow for LabaledLists being reference counted.
// Added dropLink() for all created entries.
//
// Revision 1.1  1996/11/04 19:36:41  sjp
// New file for new `List' module
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

#include "DecayChain/List/ChosenChargeList.h"

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
template < class Charged , class CandidateClass >
ChosenChargeList< Charged , CandidateClass >::ChosenChargeList( dchain::LabeledParticleList< Charged , CandidateClass >& aLabeledDecayList ,
							    const conjugation::Label aLabel ) :
   ConjugateList< CandidateClass >( aLabel ) ,
   m_list( &aLabeledDecayList )
{
}

template < class Charged , class CandidateClass >
ChosenChargeList< Charged , CandidateClass >::~ChosenChargeList()
{
}

//
// assignment operators
//
// template < class Charged , class CandidateClass >
// const ChosenChargeList< Charged , CandidateClass >& ChosenChargeList< Charged , CandidateClass >::operator=( const ChosenChargeList< Charged , CandidateClass >& aOtherList )
// {
// }

//
// member functions
//
template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::iterator ChosenChargeList< Charged , CandidateClass >::particle_begin()
{
   return ( m_list->begin() ) ;
}

template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::iterator ChosenChargeList< Charged , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template < class Charged , class CandidateClass >
dchain::LabeledCandidateList< CandidateClass >& ChosenChargeList< Charged , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}

//
// const member functions
//
template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::const_iterator ChosenChargeList< Charged , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::const_iterator ChosenChargeList< Charged , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).end() ) ;
}

template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::const_partial_iterator ChosenChargeList< Charged , CandidateClass >::partial_particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).begin( this->label() ) ) ;
}

template < class Charged , class CandidateClass >
typename ChosenChargeList< Charged , CandidateClass >::const_partial_iterator ChosenChargeList< Charged , CandidateClass >::partial_particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const dchain::LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).end( this->label() ) ) ;
}

template < class Charged , class CandidateClass >
const dchain::LabeledCandidateList< CandidateClass >& ChosenChargeList< Charged , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}

#endif /* DCHAIN_CHOSENCHARGELIST_CC */
