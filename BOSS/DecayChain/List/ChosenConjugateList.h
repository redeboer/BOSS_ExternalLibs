#ifndef DCHAIN_CHOSENCONJUGATELIST_H
#define DCHAIN_CHOSENCONJUGATELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChosenConjugateList
// 
// Description: List to hold a particular conjugation of a list
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: ChosenConjugateList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChosenConjugateList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:21  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/List/ConjugateList.h" // superclass declaration
#include "DecayChain/Element/MutableReferenceHolder.h"

// forward declarations
template <class Conjugate, class CandidateClass> class DCNeutralNCVisibleList;

namespace dchain {
template < class DecayClass > class MuteWholeItr ;
template < class DecayClass > class WholeItr ;
template < class DecayClass > class PartialItr ;
template < class DecayClass , class CandidateClass > class LabeledParticleList ;
}

template < class Arg > class DCAnalysisFunction ;

namespace dchain {
template <class Conjugate, class CandidateClass> class NeutralNCVisibleList;

template < class Conjugate , class CandidateClass >
class ChosenConjugateList : public ConjugateList< CandidateClass >
{
   // friend classses and functions
   friend class DCNeutralNCVisibleList< Conjugate , CandidateClass > ;
   friend class NeutralNCVisibleList< Conjugate , CandidateClass > ;

   public:
   // constants, enums and typedefs
   typedef MuteWholeItr< Conjugate > iterator ;
   typedef WholeItr< Conjugate > const_iterator ;
   typedef PartialItr< Conjugate > const_partial_iterator ;

   // Constructors and destructor
   virtual ~ChosenConjugateList() ;

   // member functions
   iterator particle_begin() ;
   iterator particle_end() ;

   // const member functions
   const_iterator particle_begin() const ;
   const_iterator particle_end() const ;
   const_partial_iterator partial_particle_begin() const ;
   const_partial_iterator partial_particle_end() const ;
   // The for following redeclarations are required because of function hidding
   //   ( See ARM p310 )

   template < class TAnalyzer >
      void iterate( const TAnalyzer& analyzer) const
      {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() ) ;
	 }
      }
   template < class TAnalyzer>
      void conjugation_iterate( const TAnalyzer& analyzer) const 
      {
	 const_iterator finished ( particle_end() ) ;
	 for ( const_iterator entry ( particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() , (*entry).label() == this->label() ) ;
	 }
      }

   template < class TAnalyzer>
      void partial_iterate( const TAnalyzer& analyzer ) const {
	 const_partial_iterator finished ( partial_particle_end() ) ;
	 for ( const_partial_iterator entry ( partial_particle_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() ) ;
	 }
      }
   // static member functions

   protected:
   // Constructors and destructor
   ChosenConjugateList( LabeledParticleList< Conjugate , CandidateClass >&
	 aLabeledParticleList ,
	 const conjugation::Label aLabel ) ;

   // protected member functions
   virtual LabeledCandidateList<CandidateClass>& labeledCandidateList();

   // protected const member functions
   virtual const LabeledCandidateList< CandidateClass >& labeledCandidateList() const ;

   private:
   // Constructors and destructor
   ChosenConjugateList() ; // stop default
   ChosenConjugateList( const ChosenConjugateList< Conjugate , CandidateClass >& aOtherList ) ; // stop default

   // assignment operator(s)
   const ChosenConjugateList< Conjugate , CandidateClass >& operator=( const ChosenConjugateList< Conjugate , CandidateClass >& aOtherList ) ; // stop default

   // private member functions

   // private const member functions

   // data members
   MutableReferenceHolder<LabeledParticleList< Conjugate , CandidateClass > > m_list ;

   // static data members

};

// inline function definitions

}

// function definitions
#include "DecayChain/List/Template/ChosenConjugateList.cc"

#endif /* DCHAIN_CHOSENCONJUGATELIST_H */
