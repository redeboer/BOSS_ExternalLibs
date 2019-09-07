#ifndef DCHAIN_LABELEDDECAY_H
#define DCHAIN_LABELEDDECAY_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledDecay
// 
// Description: pairing of subclass of Decay<> and Conjugation::Label
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: LabeledDecay.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: LabeledDecay.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:09  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:49  cdj
// imported DChain
//
// Revision 1.9  1998/08/19 20:56:00  sjp
// Added include of definitions
//
// Revision 1.8  1997/08/28 06:59:19  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/26 04:12:58  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.6  1997/08/19 23:01:43  sjp
// Restructured package to be independent of Rock
//
// Revision 1.5  1997/08/15 21:32:20  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/22 16:27:18  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:33:48  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:09  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:35:57  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h"  // enumerator
#include "DecayChain/Element/LabeledCandidate.h"  // superclass

using namespace dchain;
// forward declarations

template < class DecayClass >
class LabeledDecay : public LabeledCandidate< typename DecayClass::CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      LabeledDecay() ;
      LabeledDecay( const LabeledDecay< DecayClass >& aOtherLabeled ) ;
      template<class THolder>
      LabeledDecay( THolder aDecayClass ,
		      const typename conjugation::Label aLabel ) :
	 LabeledCandidate<typename DecayClass::CandidateClass>(aDecayClass,aLabel) {}

      // assignment operator(s)
      const LabeledDecay< DecayClass >& operator=( const LabeledDecay< DecayClass >& aOtherLabeled ) ;
      
      // member functions
      
      // const member functions
// note: The following function can NOT be virtual as its return type changes
//       for DecayClasses
      const DecayClass& operator()() const ;
//
      const DecayClass& particle() const ;

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members

      // static data members

};

// function definitions
//# include "DecayChain/Element/Template/LabeledDecay.cc"

#endif /* DCHAIN_LABELEDDECAY_H */
