#ifndef DCHAIN_LABELEDPARTICLELIST_H
#define DCHAIN_LABELEDPARTICLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledDecayList
// 
// Description: Container of `primary'/`conjugate' particle subclasses of Candidate
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Sep 13 17:58:11 EDT 1996
// $Id: LabeledParticleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: LabeledParticleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:29  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:53  cdj
// imported DChain
//
// Revision 1.17  1998/08/28 05:19:16  sjp
// Updated to subclass from IndexedLabeled class
//
// Revision 1.16  1998/08/21 00:52:42  sjp
// Modifier to no longer use typedefs
//
// Revision 1.15  1998/08/20 19:59:04  sjp
// Modified to use DChainBoolean
//
// Revision 1.14  1998/08/19 20:39:51  sjp
// Fixed include path for defintions
//
// Revision 1.13  1998/05/04 19:08:00  sjp
// Move forward declaration of vector, so alway made
//
// Revision 1.12  1998/04/17 19:11:03  sjp
// Modified to use latest types
//
// Revision 1.11  1997/08/28 06:59:52  sjp
// Modified files to handle complete templating
//
// Revision 1.10  1997/08/26 04:14:13  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.9  1997/08/19 23:02:22  sjp
// Restructured package to be independent of Rock
//
// Revision 1.8  1997/08/19 16:17:43  sjp
// Improved handling of STL containers
//
// Revision 1.7  1997/08/15 21:32:26  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.6  1997/01/31 20:17:07  sjp
// Modified to use the new `bug' include files
//
// Revision 1.5  1997/01/22 16:27:23  sjp
// Fixed error in pre-processor labels
//
// Revision 1.4  1997/01/21 20:33:54  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1997/01/19 20:28:38  sjp
// Changed LargeCount to Count
//
// Revision 1.2  1996/12/20 21:19:14  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:02  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h" // enumarator
#include "DecayChain/List/IndexedLabeledParticles.h" // superclass
#include "DecayChain/Iterator/MuteWholeItr.h"
#include "DecayChain/Iterator/WholeItr.h"
#include "DecayChain/Iterator/PartialItr.h"
#include "DecayChain/Element/LabeledParticle.h"

#include <vector>

#ifndef _labeledparticlelist_vector_
// The following CPP macro is used to avoid having to typedef
// a map in the class.  Why? Because things can get messy, at
// least in the g++ compiler as the typedef can cause instantiation
// when not all the necessary information is available!
#define _labeledparticlelist_vector_ std::vector< LabeledParticle< ParticleClass > >
#endif

//
//  Note: To avoid a myriad of conpiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class LabeledDecayList< A , A::CandidateClass > ;

namespace dchain {
template < class ParticleClass , class CandidateClass >
class LabeledParticleList
   : public IndexedLabeledParticles< ParticleClass , CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef MuteWholeItr< ParticleClass > iterator ;
      typedef WholeItr< ParticleClass > const_iterator ;
      typedef PartialItr< ParticleClass > const_partial_iterator ;

      // Constructors and destructor
      LabeledParticleList() ;
      virtual ~LabeledParticleList() ;

      // member functions
      LabeledParticle< ParticleClass >& operator[]( const int aIndex ) ;
      iterator begin() ;
      iterator end() ;
      void push_back( const LabeledParticle< ParticleClass >& aEntry ) ;
      virtual void erase_elements( int aStart ,
				   int aFinish ) ;

      // const member functions
      const LabeledParticle< ParticleClass >& operator[]( const int aIndex ) const ;
      virtual bool empty() const ;
      virtual bool empty( const typename conjugation::Label aLabel ) const ;
      virtual int size() const ;
      virtual int size( const typename conjugation::Label aLabel ) const ;
      const_iterator begin() const ;
      const_partial_iterator begin( const typename conjugation::Label aLabel ) const ;
      const_iterator end() const ;
      const_partial_iterator end( const typename conjugation::Label aLabel ) const ;

      // static member functions

   protected:
      // protected member functions
      virtual LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) ;
      virtual LabeledParticle< ParticleClass >& labeledParticleClass( const int aIndex ) ;

      // protected const member functions
      virtual const LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) const ;
      virtual const LabeledParticle< ParticleClass >& labeledParticleClass( const int aIndex ) const ;

   private:
      // Constructors and destructor
      LabeledParticleList( const LabeledParticleList< ParticleClass , CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const LabeledParticleList< ParticleClass , CandidateClass >& operator=( const LabeledParticleList< ParticleClass , typename ParticleClass::CandidateClass >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members
      _labeledparticlelist_vector_& m_list ;

      // static data members

} ;
}
// inline function definitions

// function definitions
#include "DecayChain/List/Template/LabeledParticleList.cc"

#endif /* DCHAIN_LABELEDPARTICLELIST_H */
