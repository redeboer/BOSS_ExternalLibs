#ifndef DCHAIN_COMBINATORICLOOP_H
#define DCHAIN_COMBINATORICLOOP_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CombinatoricLoop
// 
// Description: return 'begin' and 'end' of loop during Combinatoric generation
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Mon Oct 21 10:06:45 EDT 1996
// $Id: CombinatoricLoop.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: CombinatoricLoop.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:22  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2002/06/19 20:34:30  cleo3
// added missing typename
//
// Revision 1.1.1.1  2000/12/18 22:16:52  cdj
// imported DChain
//
// Revision 1.12  1998/08/19 20:38:01  sjp
// Fixed include path for defintions
//
// Revision 1.11  1998/06/30 23:41:29  sjp
// Moved ctor so that SunOS5 would compile
//
// Revision 1.10  1998/05/01 20:19:10  sjp
// Added necessary include file from STLUtility
//
// Revision 1.9  1998/04/17 19:11:01  sjp
// Modified to use latest types
//
// Revision 1.8  1997/08/28 06:59:49  sjp
// Modified files to handle complete templating
//
// Revision 1.7  1997/08/19 16:18:25  sjp
// Improved handling of STL containers
//
// Revision 1.6  1997/08/15 21:32:14  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:17:05  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:14  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:33:43  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:06  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:35:52  sjp
// New file for new `List' module
//

// system include files
#include <assert.h>

// user include files
#include "DecayChain/List/CandidateList.h" // for iterator declarations

// STL include files
#include <vector>

// forward declarations
namespace dchain {
template < class CandidateClass > class CombinatoricLoop ;

//
// Note: Most of the methods are included in the declarations because
//  there are too many problems with the g++ compiler to make is
//  sensible to have separate definitions

#ifndef _combinatoricloop_vector_
// The following CPP macro is used to avoid having to typedef
// a map in the class.  Why? Because things can get messy, at
// least in the g++ compiler as the typedef can cause instantiation
// when not all the necessary information is available!
#define _combinatoricloop_vector_ std::vector< CombinatoricLoop< CandidateClass >* >
#endif

template< class CandidateClass >
class CombinatoricLoop
{
   // friend classses and functions

   public:

      // constants, enums and typedefs

      // Constructors and destructor
      // Due to some `feature' of CC on SunOS5 the constructor has been
      //   moved to be the last declaration in the public area.
      virtual ~CombinatoricLoop();

      // member functions
      void setCurrentIterator( typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aIterator ) ;

      // const member functions
      const typename dchain::CandidateList< CandidateClass >::const_partial_iterator partial_begin() const ;
      const typename dchain::CandidateList< CandidateClass >::const_partial_iterator partial_end() const ;

      CombinatoricLoop( const typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aBegin ,
			const typename dchain::CandidateList< CandidateClass >::const_partial_iterator& aEnd ,
			_combinatoricloop_vector_& aLoopList ) ;

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      CombinatoricLoop() ; // stop  default
      CombinatoricLoop( const CombinatoricLoop< CandidateClass >& ) ; // stop default

      // assignment operator(s)
      const CombinatoricLoop& operator=( const CombinatoricLoop< CandidateClass >& ) ; // stop default

      // private member functions
      void adjustEnd() ;

      // private const member functions

      // data members
      typename dchain::CandidateList< CandidateClass >::const_partial_iterator m_beginIterator ;
      typename dchain::CandidateList< CandidateClass >::const_partial_iterator m_endIterator ;
      typename dchain::CandidateList< CandidateClass >::const_partial_iterator m_currentIterator ;
      typename _combinatoricloop_vector_::const_iterator* m_previousDuplicate ;

      // static data members

};

// inline function definitions
}

// function definitions
# include "DecayChain/List/Template/CombinatoricLoop.cc"

#endif /* DCHAIN_COMBINATORICLOOP_H */
