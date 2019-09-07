#ifndef DCHAIN_LABELEDCANDIDATELIST_H
#define DCHAIN_LABELEDCANDIDATELIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      LabeledCandidateList
// 
// Description: Container of `primary'/`conjugate' subclasses of Candidate
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Sep 13 17:58:11 EDT 1996
// $Id: LabeledCandidateList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: LabeledCandidateList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:28  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1.1.1  2000/12/18 22:16:53  cdj
// imported DChain
//
// Revision 1.14  1998/08/28 05:19:15  sjp
// Updated to subclass from IndexedLabeled class
//
// Revision 1.13  1998/08/21 00:52:42  sjp
// Modifier to no longer use typedefs
//
// Revision 1.12  1998/08/20 19:59:05  sjp
// Modified to use DChainBoolean
//
// Revision 1.11  1998/08/19 20:39:50  sjp
// Fixed include path for defintions
//
// Revision 1.10  1998/04/17 19:11:04  sjp
// Modified to use latest types
//
// Revision 1.9  1997/08/28 06:59:53  sjp
// Modified files to handle complete templating
//
// Revision 1.8  1997/08/26 04:14:15  sjp
// Made Candidate handling objects into `CandidateClass' Templates
//
// Revision 1.7  1997/08/19 23:02:20  sjp
// Restructured package to be independent of Rock
//
// Revision 1.6  1997/08/15 21:32:24  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/22 16:27:21  sjp
// Fixed error in pre-processor labels
//
// Revision 1.4  1997/01/21 20:33:52  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.3  1997/01/19 20:28:37  sjp
// Changed LargeCount to Count
//
// Revision 1.2  1996/12/20 21:19:13  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:00  sjp
// New file for new `List' module
//

// system include files

// user include files
#include "DecayChain/Element/conjugation.h" // enumarator
#include "DecayChain/List/IndexedLabeledCandidates.h" // superclass

#include <vector>

namespace dchain {
   template < class CandidateClass > class MuteWholeCandidateItr ;
   template < class CandidateClass > class WholeCandidateItr ;
   template < class CandidateClass > class DPartialCandidateItr ;
}

namespace dchain {

template < class CandidateClass > class LabeledCandidate ;

template < class CandidateClass >
class LabeledCandidateList
   : public IndexedLabeledCandidates< CandidateClass >
{
      // friend classses and functions

   public:
      // constants, enums and typedefs
      typedef MuteWholeCandidateItr< CandidateClass > iterator ;
      typedef WholeCandidateItr< CandidateClass > const_iterator ;
      typedef PartialCandidateItr< CandidateClass > const_partial_iterator ;

      typedef LabeledCandidate<CandidateClass> value_type;

      // Constructors and destructor
      LabeledCandidateList() ;
      virtual ~LabeledCandidateList() ;

      // member functions
      LabeledCandidate< CandidateClass >& operator[]( const int aIndex ) ;
      iterator begin() ;
      iterator end() ;
      void erase( iterator aStart, iterator aFinish ) ;

      // const member functions
      const LabeledCandidate< CandidateClass >& operator[]( const int aIndex ) const ;
      virtual bool empty() const = 0 ;
      virtual bool empty( const conjugation::Label aLabel ) const = 0 ;
      virtual int size() const = 0 ;
      virtual int size( const conjugation::Label aLabel ) const = 0 ;

      const_iterator begin() const ;
      const_partial_iterator begin( const conjugation::Label aLabel ) const ;
      const_iterator end() const ;
      const_partial_iterator end( const conjugation::Label aLabel ) const ;

      // static member functions

   protected:
      // protected member functions
      virtual LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) = 0 ;
      virtual void erase_elements( int aStart ,
				   int aFinish ) = 0 ;

      // protected const member functions
      virtual const LabeledCandidate< CandidateClass >& labeledCandidateClass( const int aIndex ) const = 0 ;

   private:
      // Constructors and destructor
      LabeledCandidateList( const LabeledCandidateList< CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const LabeledCandidateList& operator=( const LabeledCandidateList< CandidateClass >& ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

} ;
}
// inline function definitions

// function definitions
#include "DecayChain/List/Template/LabeledCandidateList.cc"

#endif /* DCHAIN_LABELEDCANDIDATELIST_H */
