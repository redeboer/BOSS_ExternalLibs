#ifndef DCHAIN_DCNEUTRALNCVISIBLELIST_H
#define DCHAIN_DCNEUTRALNCVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralNCVisibleList
// 
// Description: List to hold Candidate subclasses generated from VisibleEvidence
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DCNeutralNCVisibleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCNeutralNCVisibleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.3  2006/01/11 20:28:27  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2003/05/15 19:56:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.1  2001/09/11 15:02:27  ajm36
// add files to do neutral non-conjugating particles
//
// Revision 1.2  2000/12/30 19:43:29  cdj
// no more missing symbols under OSF1
//
// Revision 1.1.1.1  2000/12/18 22:16:51  cdj
// imported DChain
//
// Revision 1.11  1998/08/20 19:58:59  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/04/17 19:10:59  sjp
// Modified to use latest types
//
// Revision 1.9  1997/09/17 20:05:27  sjp
// Removed filling to superclass
//
// Revision 1.8  1997/08/28 06:59:44  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 15:55:06  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:32:09  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:00  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:09  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:33:37  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:00  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:35:48  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/List/NeutralNCVisibleList.h" // superclass declaration
#include "DecayChain/List/ChosenConjugateList.h" // for return-by-value
#include "DecayChain/Element/MutableReferenceHolder.h"
#include "DecayChain/List/LabeledParticleList.h"
#include "DecayChain/Function/DCUnaryAdapter.h"

template < class Arg > class DCSelectionFunction ;
template < class Arg > class DCAnalysisFunction ;
namespace dchain {
template < class DecayClass > class MuteWholeItr ;
template < class DecayClass > class WholeItr ;
template < class DecayClass > class PartialItr ;
}
//
//  Note: To avoid a myriad of compiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class dchain::LabeledParticleList< A , A::CandidateClass > ;

template < class NeutralNC , class CandidateClass >
class DCNeutralNCVisibleList : public dchain::NeutralNCVisibleList<NeutralNC, CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeItr< NeutralNC > iterator ;
      typedef dchain::WholeItr< NeutralNC > const_iterator ;
      typedef dchain::PartialItr< NeutralNC > const_partial_iterator ;

      typedef NeutralNC value_type ;

      // Constructors and destructor
      DCNeutralNCVisibleList( bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      DCNeutralNCVisibleList( const DCNeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList ,
			  bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      DCNeutralNCVisibleList( DCSelectionFunction< NeutralNC >& aFunctionalObject ) ;
      DCNeutralNCVisibleList( const DCNeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList ,
			  DCSelectionFunction< NeutralNC >& aFunctionalObject ) ;
      virtual ~DCNeutralNCVisibleList() ;
      
      // assignment operator(s)
      const DCNeutralNCVisibleList< NeutralNC , CandidateClass >& operator=( const DCNeutralNCVisibleList< NeutralNC , CandidateClass >& aOtherList ) ;

      // member functions

   protected:
      // protected member functions
      using dchain::NeutralNCVisibleList<NeutralNC,CandidateClass>::fill;
      void fill( const dchain::LabeledParticleList< NeutralNC , CandidateClass >& aList ) ;

      template<class Iterator>
      void fill(Iterator first, Iterator last) {
	 if ( 0 != m_function ) {
	    fill(first,last, dchain::NewMaker<NeutralNC>(), m_function);
	 } else if (m_functionalObject) {
	    fill(first,last,dchain::NewMaker<NeutralNC>(),m_functionalObject);
	 } else {
	    fill(first,last, dchain::NewMaker<NeutralNC>(), dchain::SelectAll<NeutralNC>());
	 }
      }

      // protected const member functions

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      bool ( * m_function )( NeutralNC& ) ;
      dchain::DCUnaryAdapter<DCSelectionFunction< NeutralNC > > m_functionalObject ;

      // static data members

} ;

// inline function definitions

// function definitions
#include "DecayChain/List/Template/DCNeutralNCVisibleList.cc"

#endif /* DCHAIN_DCNEUTRALNCVISIBLELIST_H */

