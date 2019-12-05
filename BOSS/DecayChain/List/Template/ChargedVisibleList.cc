#ifndef DCHAIN_CHARGEDVISIBLELIST_CC
#define DCHAIN_CHARGEDVISIBLELIST_CC
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
// $Id: ChargedVisibleList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChargedVisibleList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:34  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.5  2003/08/01 13:48:43  mrs43
// changed plus and minus methods of ChargedVisibleList to const
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
#include "DecayChain/Element/conjugation.h"
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/List/LabeledParticleList.h"
#include "DecayChain/List/ChosenChargeList.h"

#include "DecayChain/List/ChargedVisibleList.h"

// STL include files
//#include <vector>

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

template < class Charged , class CandidateClass >
ChargedVisibleList< Charged , CandidateClass >::
ChargedVisibleList():
   m_list( (new LabeledParticleList< Charged , CandidateClass >) ),
   m_plusList(0),
   m_minusList(0)
{
}
template < class Charged , class CandidateClass >
ChargedVisibleList< Charged , CandidateClass >::
ChargedVisibleList( const ChargedVisibleList< Charged , CandidateClass >&  aOtherList ):
   m_list( (new LabeledParticleList< Charged , CandidateClass >) ),
   m_plusList(0),
   m_minusList(0)
{
   typedef typename MakerTrait<Charged>::maker_type Maker;
  fill( *(aOtherList.m_list), Maker(), SelectAll<Charged>() ) ;
}

template < class Charged , class CandidateClass >
ChargedVisibleList< Charged , CandidateClass >::~ChargedVisibleList()
{
   delete m_plusList;
   delete m_minusList;
}

//
// assignment operators
//

template < class Charged , class CandidateClass >
const ChargedVisibleList< Charged , CandidateClass >& ChargedVisibleList< Charged , CandidateClass >::operator=( const ChargedVisibleList< Charged , CandidateClass >& aOtherList )
{
   eraseContents() ;
   typedef typename MakerTrait<Charged>::maker_type Maker;
   fill( *(aOtherList.m_list), Maker(), SelectAll<Charged>() ) ;
   return ( *this ) ;
}

//
// member functions
//

template < class Charged , class CandidateClass >
typename ChargedVisibleList< Charged , CandidateClass >::iterator 
ChargedVisibleList< Charged , CandidateClass >::particle_begin()
{
   return ( m_list->begin() ) ;
}

template < class Charged , class CandidateClass >
typename ChargedVisibleList< Charged , CandidateClass >::iterator 
ChargedVisibleList< Charged , CandidateClass >::particle_end()
{
   return ( m_list->end() ) ;
}

template < class Charged , class CandidateClass >
ChosenChargeList< Charged , CandidateClass >&
ChargedVisibleList< Charged , CandidateClass >::plus() const
{
   if ( m_plusList == 0 ) {
      m_plusList = new ChosenChargeList< Charged, CandidateClass >( *m_list, kPlus );
   }
   return (*m_plusList);
}

template < class Charged , class CandidateClass >
ChosenChargeList< Charged , CandidateClass >&
ChargedVisibleList< Charged , CandidateClass >::minus() const
{
   if ( m_minusList == 0 ) {
      m_minusList = new ChosenChargeList< Charged , CandidateClass >( *m_list , kMinus );
   }
   return (*m_minusList);
}

template < class Charged , class CandidateClass >
LabeledParticleList< Charged , CandidateClass >& 
ChargedVisibleList< Charged , CandidateClass >::labeledCandidateList()
{
   return ( *m_list ) ;
}


template < class Charged , class CandidateClass >
void ChargedVisibleList< Charged , CandidateClass >::eraseContents()
{
   m_list->erase( m_list->begin() ,
		  m_list->end() ) ;
}

//
// const member functions
//

template < class Charged , class CandidateClass >
typename ChargedVisibleList< Charged , CandidateClass >::const_iterator ChargedVisibleList< Charged , CandidateClass >::particle_begin() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).begin() ) ;
}

template < class Charged , class CandidateClass >
typename ChargedVisibleList< Charged , CandidateClass >::const_iterator ChargedVisibleList< Charged , CandidateClass >::particle_end() const
{
// Need to cast to 'const' to get the right function call
  return ( (*(const LabeledParticleList< Charged , CandidateClass >*)(m_list.pointer())).end() ) ;
}


template < class Charged , class CandidateClass >
const LabeledParticleList< Charged , CandidateClass >& 
ChargedVisibleList< Charged , CandidateClass >::labeledCandidateList() const
{
   return ( *m_list ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_CHARGEDVISIBLELIST_CC */
