#ifndef DCHAIN_LABELEDCANDIDATE_H
#define DCHAIN_LABELEDCANDIDATE_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledCandidate
// 
// Description: pairing of subclass of Candidate and Conjugation::Label
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: LabeledCandidate.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $: LabeledCandidate.h,v 1.6 1997/08/19 23:01:45 sjp Exp $
//
// Revision history
//
// $Log: LabeledCandidate.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:08  cdj
// massive class renaming, addition of [] for selection and unit tests
//
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h"  // enumerator
#include "DecayChain/Element/ReferenceHolder.h"

// forward declarations
namespace dchain {
template < class CandidateClass >
class LabeledCandidate
{
      // friend classses and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      LabeledCandidate() {}
      LabeledCandidate( const LabeledCandidate< CandidateClass >& aOtherLabeled ) :
	 m_pointer( aOtherLabeled.m_pointer ) ,
	 m_label( aOtherLabeled.m_label )
      {}
      virtual ~LabeledCandidate() {}

      // assignment operator(s)
      const LabeledCandidate& operator=( const LabeledCandidate< CandidateClass >& aOtherLabeled ) 
      {
	 m_pointer = aOtherLabeled.m_pointer ;
	 m_label = aOtherLabeled.m_label ;
	 return ( *this ) ;
      }


      // member functions

      // const member functions
// note: The following function can NOT be virtual as its return type changes
//       for CandidateClasses
      const CandidateClass& operator()() const {
	 return ( *m_pointer ) ;
      }

//
      const CandidateClass& labeledClass() const {
	 return ( *m_pointer ) ;
      }

      conjugation::Label label() const {
	 return ( m_label ) ;
      }

      bool operator==( const conjugation::Label& aLabel ) const {
	 return ( aLabel == m_label ) ;
      }

      bool operator!=( const conjugation::Label& aLabel ) const {
	 return ( aLabel != m_label ) ;
      }


      // static member functions

   protected:
      // Constructors and destructor
      template<class THolder>
      LabeledCandidate( THolder aCandidateClass ,
			  const conjugation::Label aLabel ) :
	 m_pointer(aCandidateClass),
	 m_label(aLabel) {}

      // protected member functions
      CandidateClass* pointer() {
	 return m_pointer.pointer();
      }

      // protected const member functions
      const CandidateClass* pointer() const {
	 return ( m_pointer.pointer() ) ;
      }


   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      ReferenceHolder<CandidateClass> m_pointer;
      conjugation::Label m_label ;

      // static data members

} ;
}

#endif /* DCHAIN_LABELEDCANDIDATE_H */
