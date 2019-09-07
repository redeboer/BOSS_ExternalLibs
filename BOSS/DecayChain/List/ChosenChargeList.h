#ifndef DCHAIN_CHOSENCHARGELIST_H
#define DCHAIN_CHOSENCHARGELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChosenChargeList
// 
// Description: List to hold a ConjugateList of ChargedVisible.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: ChosenChargeList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ChosenChargeList.h,v $
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
namespace dchain {
   template < class Charged , class CandidateClass > class ChargedVisibleList ;
}

namespace dchain {

template < class DecayClass > class MuteWholeItr ;
template < class DecayClass > class WholeItr ;
template < class DecayClass > class PartialItr ;
template < class DecayClass , class CandidateClass > class LabeledParticleList ;
}

//#include <vector>

template < class Arg > class DCAnalysisFunction ;

namespace dchain {

template < class Charged , class CandidateClass >
class ChosenChargeList : public ConjugateList< CandidateClass >
{
   // friend classses and functions
   friend class dchain::ChargedVisibleList< Charged , CandidateClass > ;

   public:
   // constants, enums and typedefs
   typedef dchain::MuteWholeItr< Charged > iterator ;
   typedef dchain::WholeItr< Charged > const_iterator ;
   typedef dchain::PartialItr< Charged > const_partial_iterator ;

   // Constructors and destructor
   virtual ~ChosenChargeList() ;

   // member functions
   iterator particle_begin() ;
   iterator particle_end() ;

   // const member functions
   const_iterator particle_begin() const ;
   const_iterator particle_end() const ;
   const_partial_iterator partial_particle_begin() const ;
   const_partial_iterator partial_particle_end() const ;
   //
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
   ChosenChargeList( dchain::LabeledParticleList< Charged , CandidateClass >& aLabeledDecayList ,
	 const conjugation::Label aLabel ) ;

   // protected member functions
   virtual dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() ;

   // protected const member functions
   virtual const dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() const ;

   private:
   // Constructors and destructor
   ChosenChargeList() ; // stop default
   ChosenChargeList( const ChosenChargeList< Charged , CandidateClass >& aOtherList ) ; // stop default

   // assignment operator(s)
   const ChosenChargeList< Charged , CandidateClass >& operator=( const ChosenChargeList< Charged , CandidateClass >& aOtherList ) ; // stop default

   // private member functions

   // private const member functions

   // data members
   MutableReferenceHolder< dchain::LabeledParticleList< Charged , CandidateClass > > m_list ;

   // static data members

} ;
}
// inline function definitions

// function definitions
#include "DecayChain/List/Template/ChosenChargeList.cc"

#endif /* DCHAIN_CHOSENCHARGELIST_H */
