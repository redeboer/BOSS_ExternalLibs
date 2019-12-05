#ifndef DCHAIN_LABELEDPARTICLELIST_CC
#define DCHAIN_LABELEDPARTICLELIST_CC
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledParticleList
// 
// Description: Container of `primary'/`conjugate' particle subclasses of Candidate
//
// Implimentation:
//     <Notes on implimentation>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 20:56:18 EDT 1996
// $Id: LabeledParticleList.cc,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: LabeledParticleList.cc,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:40  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2003/03/13 20:19:16  cleo3
// now compiles under Linux
//
// Revision 1.1.1.1  2000/12/18 22:16:56  cdj
// imported DChain
//
// Revision 1.16  1998/08/28 05:19:17  sjp
// Updated to subclass from IndexedLabeled class
//
// Revision 1.15  1998/08/21 00:51:55  sjp
// Modifier to no longer use typedefs
//
// Revision 1.14  1998/08/20 20:00:13  sjp
// Modified to use DChainBoolean
//
// Revision 1.13  1998/08/19 20:46:24  sjp
// Fixed comments and removed report references
//
// Revision 1.12  1998/07/27 22:20:49  sjp
// Did explicit \'count\' to avoid compiler problems
//
// Revision 1.11  1998/04/17 19:14:52  sjp
// Modified to use latest type
//
// Revision 1.10  1997/09/03 14:58:25  sjp
// Use new report.h and TBParticlePoint
//
// Revision 1.9  1997/08/28 07:00:20  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/26 04:14:26  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.7  1997/08/19 23:02:36  sjp
// Restructured package to be independent of Rock
//
// Revision 1.6  1997/08/19 16:17:47  sjp
// Improved handling of STL containers
//
// Revision 1.5  1997/08/15 21:33:09  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/31 20:17:31  sjp
// Modified to use the new `bug' include files
//
// Revision 1.3  1997/01/21 20:34:33  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1997/01/19 20:28:49  sjp
// Changed LargeCount to Count
//
// Revision 1.1  1996/11/04 19:36:45  sjp
// New file for new `List' module
//

// system include files
#include <algorithm>
#include <stdlib.h>  // required for 'exit'

// user include files
#include "DecayChain/Element/LabeledParticle.h"
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Iterator/PartialItr.h"

#include "DecayChain/List/LabeledParticleList.h"

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
template < class ParticleClass , class CandidateClass >
LabeledParticleList< ParticleClass , CandidateClass >::LabeledParticleList() :
   m_list( *(new _labeledparticlelist_vector_ ) )
{
}

// template < class ParticleClass , class CandidateClass >
// LabeledParticleList< ParticleClass , CandidateClass >::LabeledParticleList( const LabeledParticleList< ParticleClass , CandidateClass >& )
// {
// }

template < class ParticleClass , class CandidateClass >
LabeledParticleList< ParticleClass , CandidateClass >::~LabeledParticleList()
{
   delete( &m_list ) ;
}

//
// assignment operators
//
// template < class ParticleClass , class CandidateClass >
// const LabeledParticleList< ParticleClass , CandidateClass >& LabeledParticleList< ParticleClass , CandidateClass >::operator=( const LabeledParticleList< ParticleClass , CandidateClass >& )
// {
// }

//
// member functions
//

//------ operator[] ------
//
template < class ParticleClass , class CandidateClass >
LabeledParticle< ParticleClass >& LabeledParticleList< ParticleClass , CandidateClass >::operator[]( const int aIndex ) {
   return ( labeledParticleClass( aIndex ) ) ;
}

//------ begin ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::iterator LabeledParticleList< ParticleClass , CandidateClass >::begin()
{
   return ( iterator( this , 0 ) ) ;
}

//------ end ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::iterator LabeledParticleList< ParticleClass , CandidateClass >::end()
{
   return ( iterator( this , m_list.size() ) ) ;
}

//------ push_back ------
//
template < class ParticleClass , class CandidateClass >
void LabeledParticleList< ParticleClass , CandidateClass >::push_back( const LabeledParticle< ParticleClass >& aEntry )
{
   m_list.push_back( aEntry ) ;
}

//------ erase_elements ------
//
template < class ParticleClass , class CandidateClass >
void LabeledParticleList< ParticleClass , CandidateClass >::erase_elements( int aStart ,
					      int aFinish )
{
   m_list.erase( m_list.begin() + aStart ,
		 m_list.begin() + aFinish ) ;
}

//------ labeledCandidateClass ------
//
template < class ParticleClass , class CandidateClass >
LabeledCandidate< CandidateClass >& LabeledParticleList< ParticleClass , CandidateClass >::labeledCandidateClass( const int aIndex )
{
   return ( labeledParticleClass( aIndex ) ) ;
}

//------ labeledParticleClass ------
//
template < class ParticleClass , class CandidateClass >
LabeledParticle< ParticleClass >& LabeledParticleList< ParticleClass , CandidateClass >::labeledParticleClass( const int aIndex )
{
   return ( m_list[ aIndex ] ) ;
}

//
// const member functions
//

//------ operator[] ------
//
template < class ParticleClass , class CandidateClass >
const LabeledParticle< ParticleClass >& LabeledParticleList< ParticleClass , CandidateClass >::operator[]( const int aIndex ) const
{
   return ( labeledParticleClass( aIndex ) ) ;
}

//------ empty ------
//
template < class ParticleClass , class CandidateClass >
bool LabeledParticleList< ParticleClass , CandidateClass >::empty() const
{
   return ( m_list.empty() ) ;
}

//------ empty ------
//
template < class ParticleClass , class CandidateClass >
bool LabeledParticleList< ParticleClass , CandidateClass >::empty( const conjugation::Label aLabel ) const
{
// for efficiency, check to see if the whole list is empty first
   if ( m_list.empty() ) {
      return ( !false ) ;
   }
   return ( const_partial_iterator( this , 0 , aLabel ) ==
			const_partial_iterator( this , m_list.size() , aLabel ) ) ;
}

//------ size ------
//
template < class ParticleClass , class CandidateClass >
int LabeledParticleList< ParticleClass , CandidateClass >::size() const
{
   return ( m_list.size() ) ;
}

//------ size ------
//
template < class ParticleClass , class CandidateClass >
int LabeledParticleList< ParticleClass , CandidateClass >::size( const conjugation::Label aLabel ) const
{
// for efficiency, check to see if the whole list is empty first
   if ( m_list.empty() ) {
      return ( 0 ) ;
   }
   int result( 0 ) ;
/* Currently the "count" is done explicilty because of deficiencies
  in the SunOS5 compiler!
   count( _labeledparticlelist_vector_::const_iterator( m_list.begin() ) ,
	  _labeledparticlelist_vector_::const_iterator( m_list.end() ) ,
	  aLabel ,
	  result ) ;
	  */
   typename _labeledparticlelist_vector_::const_iterator first( m_list.begin() )  ;
   typename _labeledparticlelist_vector_::const_iterator last( m_list.end() ) ;
   while (first != last) {
      if ( *first == aLabel ) {
	 first++ ;
	 result++ ;
      }
   }

   return ( result ) ;
}

//------ begin ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::const_iterator LabeledParticleList< ParticleClass , CandidateClass >::begin() const
{
   return ( const_iterator( this , 0 ) ) ;
}

//------ begin ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::const_partial_iterator LabeledParticleList< ParticleClass , CandidateClass >::begin( const conjugation::Label aLabel ) const
{
   return ( const_partial_iterator( this , 0 , aLabel ) ) ;
}

//------ end ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::const_iterator LabeledParticleList< ParticleClass , CandidateClass >::end() const
{
   return ( const_iterator( this , m_list.size() ) ) ;
}

//------ end ------
//
template < class ParticleClass , class CandidateClass >
typename LabeledParticleList< ParticleClass , CandidateClass >::const_partial_iterator LabeledParticleList< ParticleClass , CandidateClass >::end( const conjugation::Label aLabel ) const
{
   return ( const_partial_iterator( this , m_list.size() , aLabel ) ) ;
}

//------ labeledCandidateClass ------
//
template < class ParticleClass , class CandidateClass >
const LabeledCandidate< CandidateClass >& LabeledParticleList< ParticleClass , CandidateClass >::labeledCandidateClass( const int aIndex ) const
{
   return ( labeledParticleClass( aIndex ) ) ;
}

//------ labeledParticleClass ------
//
template < class ParticleClass , class CandidateClass >
const LabeledParticle< ParticleClass >& LabeledParticleList< ParticleClass , CandidateClass >::labeledParticleClass( const int aIndex ) const 
{
   return ( m_list[ aIndex ] ) ;
}

//
// static member functions
//
}
#endif /* DCHAIN_LABELEDPARTICLELIST_CC */
