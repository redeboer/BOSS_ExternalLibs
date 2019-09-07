#ifndef DCHAIN_CANDIDATELIST_H
#define DCHAIN_CANDIDATELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CandidateList
// 
// Description: container of subclasses of Candidate, used for iteration
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 15:59:57 EDT 1996
// $Id: CandidateList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CandidateList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:20  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:51  cdj
// imported DChain
//
// Revision 1.10  1998/08/21 00:52:43  sjp
// Modifier to no longer use typedefs
//
// Revision 1.9  1998/08/20 19:59:06  sjp
// Modified to use DChainBoolean
//
// Revision 1.8  1998/04/17 19:11:06  sjp
// Modified to use latest types
//
// Revision 1.7  1997/08/28 06:59:57  sjp
// Modified files to handle complete templating
//
// Revision 1.6  1997/08/26 04:14:17  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.5  1997/08/15 21:32:07  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.4  1997/01/22 16:27:08  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:33:36  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1997/01/19 20:28:34  sjp
// Changed LargeCount to Count
//
// Revision 1.1  1996/11/04 19:35:46  sjp
// New file for new `List' module
//

// system include files

// user include files

// forward declarations
namespace dchain {

template < class CandidateClass > class MuteWholeCandidateItr ;
template < class CandidateClass > class WholeCandidateItr ;
template < class CandidateClass > class PartialCandidateItr ;
template < class CandidateClass > class LabeledCandidate ;
template < class CandidateClass > class LabeledCandidateList ;


template < class CandidateClass >
class CandidateList
{
   // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef dchain::MuteWholeCandidateItr< CandidateClass > iterator ;
      typedef dchain::WholeCandidateItr< CandidateClass > const_iterator ;
      typedef dchain::PartialCandidateItr< CandidateClass > const_partial_iterator ;

      typedef dchain::LabeledCandidate<CandidateClass> value_type;
      // Constructors and destructor
      CandidateList() ;
      virtual ~CandidateList() ;

      // member functions
      virtual iterator begin() ;
      virtual iterator end() ;

      // const member functions
      virtual bool empty() const ;
      virtual int size() const ;
      virtual const_iterator begin() const ;
      virtual const_iterator end() const ;

      template < class TAnalyzer >
	 void iterate( const TAnalyzer& analyze ) const {
	    const_iterator finished ( end() );
	    for ( const_iterator entry ( begin() ) ;
		  entry != finished ;
		  ++entry ) {
	       analyze( (*entry)() ) ;
	    }
	 }

      // static member functions

   protected:
      // protected member functions
      virtual dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() = 0 ;

      // protected const member functions
      virtual const dchain::LabeledCandidateList< CandidateClass >& labeledCandidateList() const = 0 ;

   private:
      // Constructors and destructor
      CandidateList( const CandidateList< CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const CandidateList& operator=( const CandidateList< CandidateClass >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

};
}
// inline function definitions

// function definitions
#include "DecayChain/List/Template/CandidateList.cc"

#endif /* DCHAIN_CANDIDATELIST_H */
