#ifndef DCHAIN_DCCHARGEDVISIBLELIST_H
#define DCHAIN_DCCHARGEDVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ChargedVisibleList
// 
// Description: List to hold Candidate subclasses generated from VisibleEvidence
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DCChargedVisibleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCChargedVisibleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.5  2006/01/11 20:28:23  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.4  2003/08/01 13:48:40  mrs43
// changed plus and minus methods of DCChargedVisibleList to const
//
// Revision 1.3  2003/05/15 19:56:06  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
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
#include "DecayChain/List/ChargedVisibleList.h" // superclass declaration
#include "DecayChain/List/ChosenChargeList.h" // for return-by-value
#include "DecayChain/Element/MutableReferenceHolder.h"
#include "DecayChain/Function/DCSelectionFunction.h"
#include "DecayChain/Function/DCUnaryAdapter.h"

// forward declarations
template < class DecayClass , class CandidateClass > class LabeledParticleList;
template < class Arg > class DCSelectionFunction ;
template < class Arg > class DCAnalysisFunction ;

//
//  Note: To avoid a myriad of compiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class LabeledDecayList< A , A::CandidateClass > ;

template < class Charged , class CandidateClass >
class DCChargedVisibleList : public dchain::ChargedVisibleList< Charged, CandidateClass >
{
   // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeItr< Charged > iterator ;
      typedef dchain::WholeItr< Charged > const_iterator ;
      typedef dchain::PartialItr< Charged > const_partial_iterator ;

      typedef Charged value_type ;

      // Constructors and destructor
      DCChargedVisibleList( bool ( * pFunction )( Charged& ) = 0 ) ;
      DCChargedVisibleList( const DCChargedVisibleList< Charged , CandidateClass >& aOtherList ,
	    bool ( * pFunction )( Charged& ) = 0 ) ;
      DCChargedVisibleList( DCSelectionFunction< Charged >& aFunctionalObject ) ;
      DCChargedVisibleList( const DCChargedVisibleList< Charged , CandidateClass >& aOtherList ,
	    DCSelectionFunction< Charged >& aFunctionalObject ) ;
      virtual ~DCChargedVisibleList() ;

      // assignment operator(s)
      const DCChargedVisibleList< Charged , CandidateClass >& operator=( const DCChargedVisibleList< Charged , CandidateClass >& aOtherList ) ;

      // member functions
      // static member functions

   protected:
      // protected member functions
      using dchain::ChargedVisibleList<Charged,CandidateClass>::fill;
      void fill( const dchain::LabeledParticleList< Charged , CandidateClass >& aList ) ;

      template<class Iterator>
	 void fill(Iterator first, Iterator last) {   
	    if ( 0 != m_function ) {
	       fill(first,last, dchain::NewMaker<Charged>(), m_function);
	    } else if (m_functionalObject) {
	       fill(first,last,dchain::NewMaker<Charged>(),m_functionalObject);
	    } else {
	       fill(first,last, dchain::NewMaker<Charged>(), dchain::SelectAll<Charged>());
	    }
	 }


      // protected const member functions

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      bool ( * m_function )( Charged& ) ;
      dchain::DCUnaryAdapter<DCSelectionFunction< Charged > > m_functionalObject ;

      // static data members

} ;

// inline function definitions

// function definitions
#include "DecayChain/List/Template/DCChargedVisibleList.cc"

#endif /* DCHAIN_DCCHARGEDVISIBLELIST_H */

