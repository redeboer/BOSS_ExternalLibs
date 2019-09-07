#ifndef DCHAIN_DCNEUTRALVISIBLELIST_H
#define DCHAIN_DCNEUTRALVISIBLELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      NeutralVisibleList
// 
// Description: List to hold Candidates generated from observed Tracks.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DCNeutralVisibleList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCNeutralVisibleList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.5  2006/01/11 20:28:27  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.4  2003/05/15 19:56:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.3  2001/03/28 14:27:58  urner
// added same workaround as for charged because of NonTemplatedMemberFunctionBug
//
// Revision 1.2  2001/03/23 23:07:44  urner
// modification needed to add pi0 eta and Ks decay lists in CleoDChain
//
// Revision 1.1.1.1  2000/12/18 22:16:53  cdj
// imported DChain
//
// Revision 1.11  1998/08/20 19:59:05  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/04/17 19:11:05  sjp
// Modified to use latest types
//
// Revision 1.9  1997/09/17 20:05:29  sjp
// Removed filling to superclass
//
// Revision 1.8  1997/08/28 06:59:56  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 15:55:10  sjp
// Modified list to have no dependence on any other
//   structures except its contents, which must have
//   certain `typedef' available to use.
//
// Revision 1.6  1997/08/15 21:32:39  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:14  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:32  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:34:06  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:23  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:11  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/List/NeutralVisibleList.h" // superclass declaration
#include "DecayChain/Function/DCUnaryAdapter.h"
#include "DecayChain/Function/DCSelectionFunction.h"
#include "DecayChain/Function/DCAnalysisFunction.h"

// forward declarations

//
//  Note: To avoid a myriad of conpiler flags it is easier to
//        have a two parameter template class.  The instantiation
//        of this class must look like
//
//        template class LabeledDecayList< A , A::CandidateClass > ;

template< class Neutral , class CandidateClass >
class DCNeutralVisibleList : public dchain::NeutralVisibleList< Neutral, CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      /*
      typedef dchain::MuteWholeItr< Neutral > iterator ;
      typedef dchain::WholeItr< Neutral > const_iterator ;
      //typedef dchain::PartialItr< Neutral > const_partial_iterator ;
      */
      typedef Neutral value_type ;

      // Constructors and destructor
      DCNeutralVisibleList( bool ( * pFunction )( Neutral& ) = 0 ) ;
      DCNeutralVisibleList( const DCNeutralVisibleList< Neutral , CandidateClass >& aOtherList ,
			  bool ( * pFunction )( Neutral& ) = 0 ) ;
      DCNeutralVisibleList( DCSelectionFunction< Neutral >& aFunctionalObject ) ;
      DCNeutralVisibleList( const DCNeutralVisibleList< Neutral , CandidateClass >& aOtherList ,
			  DCSelectionFunction< Neutral >& aFunctionalObject ) ;
      virtual ~DCNeutralVisibleList() ;
      
      // assignment operator(s)
      const DCNeutralVisibleList< Neutral , CandidateClass >& operator=( const DCNeutralVisibleList< Neutral , CandidateClass >& aOtherList );
      
      // member functions

      // const member functions

      // static member functions

   protected:
      // protected member functions
      using dchain::NeutralVisibleList<Neutral,CandidateClass>::fill;
      void fill( const dchain::LabeledParticleList< Neutral , CandidateClass >& aList ) ;

      template<class Iterator>
      void fill(Iterator first, Iterator last) {   
	 if ( 0 != m_function ) {
	    fill(first,last, dchain::NewMaker<Neutral>(), m_function);
	 } else if (m_functionalObject) {
	    fill(first,last,dchain::NewMaker<Neutral>(),m_functionalObject);
	 } else {
	    fill(first,last, dchain::NewMaker<Neutral>(), dchain::SelectAll<Neutral>());
	 }
      }


      // protected const member functions

   private:
      // Constructors and destructor

      // private member functions

      // private const member functions

      // data members
      bool ( * m_function )( Neutral& ) ;
      dchain::DCUnaryAdapter<DCSelectionFunction< Neutral > > m_functionalObject ;

      // static data members

} ;

// inline function definitions

// function definitions
#include "DecayChain/List/Template/DCNeutralVisibleList.cc"

#endif /* DCHAIN_DCNEUTRALVISIBLELIST_H */
