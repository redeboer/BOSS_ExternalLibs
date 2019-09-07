#ifndef DCHAIN_CONJUGATELIST_H
#define DCHAIN_CONJUGATELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ConjugateList
// 
// Description: container of subclasses of Candidate, used for partial iteration
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 15:59:57 EDT 1996
// $Id: ConjugateList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ConjugateList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:22  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2001/04/04 15:01:00  cdj
// made label public (needed for storage)
//
// Revision 1.1.1.1  2000/12/18 22:16:52  cdj
// imported DChain
//
// Revision 1.12  1998/08/21 00:52:41  sjp
// Modifier to no longer use typedefs
//
// Revision 1.11  1998/08/20 19:59:01  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/04/17 19:11:02  sjp
// Modified to use latest types
//
// Revision 1.9  1997/08/28 06:59:50  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/26 04:14:12  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.7  1997/08/19 23:02:18  sjp
// Restructured package to be independent of Rock
//
// Revision 1.6  1997/08/15 21:32:16  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/22 16:27:16  sjp
// Fixed error in pre-processor labels
//
// Revision 1.4  1997/01/21 20:33:45  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1997/01/19 20:28:35  sjp
// Changed LargeCount to Count
//
// Revision 1.2  1996/12/20 21:19:08  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:35:54  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h" // enum
#include "DecayChain/List/CandidateList.h" // superclass declaration

// forward declarations
template < class Arg > class DCAnalysisFunction ;
template < class Arg > class DCConjugateFunction ;
template < class Arg > class DCCombinatoricQuery ;

namespace dchain {
template < class CandidateClass >
class ConjugateList : public dchain::CandidateList< CandidateClass >
{
      // friend classses and functions
// DCCombinatoricQuery is a friend so that CombinatoricList can check the
//   LabeledCandidateList and Label to determine whether a set of
//   ConjugateLists are self-conjugate, and can use
//   conjugate_partial_begin() and conjugate_partial_end() to build up
//   the `conjugate' half of the DCCombinatoricQuery
      friend class DCCombinatoricQuery< CandidateClass > ;

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeCandidateItr< CandidateClass > iterator ;
      typedef dchain::WholeCandidateItr< CandidateClass > const_iterator ;
      typedef dchain::PartialCandidateItr< CandidateClass > const_partial_iterator ;
      // Constructors and destructor
      ConjugateList( const conjugation::Label aLabel ) ;
      virtual ~ConjugateList() ;

      // member functions

      // const member functions
      virtual bool isSelfConjugate() const ;
      bool isConjugateOf( const ConjugateList< CandidateClass >& aList ) const ;
      int partial_size() const ;

      virtual const_partial_iterator partial_begin() const ;
      virtual const_partial_iterator partial_end() const ;

      /** TAnalyzer takes two arguments
	  1) const CandidateClass&
          2) a bool which is 'true' if the candiate matches the present conjugation
       */
      template <class TAnalyzer>
      void conjugation_iterate( const TAnalyzer& analyzer) const
      {
	 const_iterator finished ( this->end() ) ;
	 for ( const_iterator entry ( this->begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() , (*entry).label() == m_label ) ;
	 }
      }

      template<class TAnalyzer>
      void partial_iterate( const TAnalyzer& analyze ) const {
	 const_partial_iterator finished ( partial_end() ) ;
	 for ( const_partial_iterator entry ( partial_begin() ) ;
	       entry != finished ;
	       ++entry ) {
	    analyze( (*entry)() ) ;
	 }
      }


      // static member functions

      // special member functions
//- Note: following two function are used by DCCombinatoricQuery only!
      const_partial_iterator conjugate_partial_begin() const ;
      const_partial_iterator conjugate_partial_end() const ;

      virtual conjugation::Label label() const ;

   protected:

      // protected member functions
      void setLabel( const conjugation::Label aLabel) ;

      // protected const member functions

   private:
      // Constructors and destructor
      ConjugateList() ; // stop default
      ConjugateList( const ConjugateList& ) ; // stop default

      // assignment operator(s)
      const ConjugateList& operator=( const ConjugateList& ) ; // stop default

      // private member functions

      // private const member functions

      // data members
      conjugation::Label m_label ;

      // static data members

};

// inline function definitions
}

// function definitions
#include "DecayChain/List/Template/ConjugateList.cc"

#endif /* DCHAIN_CONJUGATELIST_H */
