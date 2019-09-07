#ifndef DCHAIN_DCDECAYLIST_H
#define DCHAIN_DCDECAYLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      DCDecayList
// 
// Description: container of subclasses of Decay, used for iteration
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DCDecayList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCDecayList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.16  2006/04/09 13:32:09  cdj
// added back missing operator+
//
// Revision 1.15  2006/02/03 18:29:33  cdj
// fixed bug in copy constructor and operator=
//
// Revision 1.14  2006/01/14 15:29:43  cdj
// fixed missing symbol problem
//
// Revision 1.13  2006/01/13 18:10:15  cdj
// improved backwards compatibility
//
// Revision 1.12  2006/01/11 20:28:24  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.11  2004/02/19 21:30:26  cdj
// added operator+= to DecayList
//
// Revision 1.10  2003/10/23 18:43:39  cdj
// can now use DCDecayList with other Decay objects
//
// Revision 1.9  2003/05/15 19:56:08  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.8  2002/06/19 20:36:03  cleo3
// now compiles under Solaris 8
//
// Revision 1.7  2001/05/24 19:46:20  urner
// first tag
//
// Revision 1.6  2001/04/12 13:43:58  urner
// explicitely instantiated operator+ to make Solaris happy
//
// Revision 1.5  2001/04/10 13:56:45  urner
// bug fixes
//
// Revision 1.4  2001/04/03 11:43:10  cdj
// made operator+ a friend
//
// Revision 1.3  2001/03/30 16:34:53  cdj
// added simplistic operator+
//
// Revision 1.2  2001/03/23 23:07:43  urner
// modification needed to add pi0 eta and Ks decay lists in CleoDChain
//
// Revision 1.1.1.1  2000/12/18 22:16:52  cdj
// imported DChain
//
// Revision 1.12  1998/08/27 04:56:07  sjp
// Added value_type typedef
//
// Revision 1.11  1998/08/20 19:58:58  sjp
// Modified to use DChainBoolean
//
// Revision 1.10  1998/08/19 20:38:03  sjp
// Fixed include path for defintions
//
// Revision 1.9  1998/05/01 20:20:01  sjp
// Removed unnecessary functions which were causing infinate loop
//
// Revision 1.8  1998/04/17 19:10:58  sjp
// Modified to use latest types
//
// Revision 1.7  1997/08/28 06:59:42  sjp
// Modified files to handle complete templating
//
// Revision 1.6  1997/08/15 21:32:05  sjp
// Updated to use <package>/<file>.h include structure.
// Updated to use bug flags specified in Experiement.h
//
// Revision 1.5  1997/01/31 20:16:58  sjp
// Modified to use the new `bug' include files
//
// Revision 1.4  1997/01/22 16:27:06  sjp
// Fixed error in pre-processor labels
//
// Revision 1.3  1997/01/21 20:34:07  sjp
// Changed CPP flags and include because of library reorganization
//
// Revision 1.2  1996/12/20 21:19:25  sjp
// Extended pathnames for Include files
//
// Revision 1.1  1996/11/04 19:36:12  sjp
// New file for new `List' module
//

// system include files
#include <assert.h>
#include <iostream>

// user include files
#include "DecayChain/List/DecayList.h"
#include "DecayChain/List/DCFillableNeutralList.h"
#include "DecayChain/Function/DCSelectionFunction.h"

template < class DecayClass , class CandidateClass >
class DCDecayList;

template < class DecayClass , class CandidateClass >
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCDecayList< DecayClass, CandidateClass >& iList1,
   const DCDecayList< DecayClass, CandidateClass >& iList2 );
template < class DecayClass , class CandidateClass, class NeutralClass >
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCDecayList< DecayClass, CandidateClass >& iList1,
   const DCFillableNeutralList< NeutralClass >& iList2 );
template < class DecayClass , class CandidateClass, class NeutralClass >
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCFillableNeutralList< NeutralClass >& iList1 ,
   const DCDecayList< DecayClass, CandidateClass >& iList2);

template < class DecayClass , class CandidateClass >
class DCDecayList : public dchain::DecayList<DecayClass, CandidateClass>
{
   // friend classses and functions

   friend 
      DCDecayList< DecayClass, CandidateClass > operator+ <DecayClass, CandidateClass> (
	    const DCDecayList< DecayClass, CandidateClass >& iList1,
	    const DCDecayList< DecayClass, CandidateClass >& iList2 );

   public:
   // constants, enums and typedefs
   typedef DecayClass value_type ;

   // Constructors and destructor
   DCDecayList( bool ( * pFunction )( DecayClass& ) = 0 ) ;
   // need this function to stop default copy constructor
   DCDecayList( const DCDecayList< DecayClass , CandidateClass >& aOtherList ,
	 bool ( * pFunction )( DecayClass& ) ) ;
   DCDecayList( DCSelectionFunction< DecayClass >& aFunctionalObject ) ;

   DCDecayList(const DCDecayList<DecayClass,CandidateClass>& iList) :
      dchain::DecayList<DecayClass,CandidateClass>(
	    static_cast<dchain::DecayList<DecayClass,CandidateClass> >(iList)),
      m_function(0), m_functionalObject(0) {}

   DCDecayList(const dchain::DecayList<DecayClass,CandidateClass>& iList) :
      dchain::DecayList<DecayClass,CandidateClass>(iList),
      m_function(0), m_functionalObject(0) {}

   // assignment operator(s)
   // need this function to stop default assignment operator
   template< class TDecay>
      const DCDecayList< DecayClass , CandidateClass >& operator=( const DCDecayList< TDecay , CandidateClass >& aOtherList ) {
	 this->erase();
	 fill(aOtherList);
	 return ( *this ) ;
      }


   template< class TDecay>
      const DCDecayList< DecayClass , CandidateClass >& operator=( const dchain::DecayList< TDecay , CandidateClass >& aOtherList ) {
	 this->erase();
	 fill(aOtherList);
	 return ( *this ) ;
      }

   const DCDecayList< DecayClass , CandidateClass >& operator=( const dchain::CombinatoricList< CandidateClass >& aOtherList );

   const DCDecayList< DecayClass , CandidateClass >& operator=( const DCDecayList< DecayClass , CandidateClass >& aOtherList ){
      this->erase();
      fill(aOtherList);
      return ( *this ) ;
   }
   //
   template<class TList>
      void operator+=( const TList& aOtherList) {

	 if( this->size() != 0 ) {
	    //only check compatable conjugation if something already in
	    // this list, otherwise could have problem with default value
	    bool can_only_add_lists_with_same_conjugation =
	       this->isSelfConjugate() == aOtherList.isSelfConjugate();
	    assert(can_only_add_lists_with_same_conjugation);
	 }
	 fill(aOtherList);
      }

   // member functions
   using dchain::DecayList<DecayClass,CandidateClass>::attempt_insert;
   void attempt_insert(DecayClass* pPossible,
	 const dchain::conjugation::Label aLabel) {

      if(0!= m_function) {
	 this->attempt_insert(pPossible, aLabel, m_function);
      } else if(0!= m_functionalObject) {
	 this->attempt_insert(pPossible, aLabel, *m_functionalObject);
      } else {
	 dchain::SelectAll<DecayClass> selectAll;
	 this->attempt_insert(pPossible, aLabel,selectAll );
      }
   }
   protected:
   // Constructors and destructor

   public:
   DCDecayList(
	 dchain::LabeledParticleList< DecayClass , CandidateClass >& aLabeledDecayList ,
	 dchain::conjugation::Label aLabel ) ;

   private:
   // Constructors and destructor

   // private member functions
   using dchain::DecayList<DecayClass,CandidateClass>::fill;
   template<class TDecayList>
      void fill( const TDecayList& aList ) {
	 //std::cout << "@DCDecayList::fill()" << std::endl;
	 if(0!= m_function) {
	    this->fill(aList, m_function);
	 } else if(0!= m_functionalObject) {
	    this->fill(aList, *m_functionalObject);
	 } else {
	    dchain::SelectAll<DecayClass> selectAll;
	    this->fill(aList,selectAll );
	 }
      }	 

   private:

   // private const member functions

   // data members
   bool ( * m_function )( DecayClass& ) ;
   DCSelectionFunction< DecayClass >* m_functionalObject ;


   // static data members

} ;

// inline function definitions

#include <assert.h>

template < class DecayClass , class CandidateClass >
inline
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCDecayList< DecayClass, CandidateClass >& iList1,
   const DCDecayList< DecayClass, CandidateClass >& iList2 )
{
   bool can_only_add_lists_with_same_conjugation =
      iList1.isSelfConjugate()== iList2.isSelfConjugate();
   assert(can_only_add_lists_with_same_conjugation);

   DCDecayList< DecayClass, CandidateClass> returnValue( iList1 );
   if( &iList1 != &iList2 ) {
      returnValue.fill( iList2 );
   }
   return returnValue;
}

template < class DecayClass , class CandidateClass, class NeutralClass >
inline
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCDecayList< DecayClass, CandidateClass >& iList1,
   const DCFillableNeutralList< NeutralClass >& iList2 )
{
   bool can_only_add_lists_with_same_conjugation =
      iList1.isSelfConjugate()== iList2.isSelfConjugate();
   assert(can_only_add_lists_with_same_conjugation);

   
   DCDecayList< DecayClass, CandidateClass> returnValue( iList1 );
   typename dchain::LabeledParticleList< NeutralClass , CandidateClass >::const_iterator finished( iList2.particle_end() ) ;
   for( typename dchain::LabeledParticleList< NeutralClass , CandidateClass >::const_iterator entry(iList2.particle_begin()) ;
	entry != finished ;
	++entry ){
     NeutralClass* localCopy = new NeutralClass( (*entry).particle() ) ;
     returnValue.attempt_insert( localCopy, (*entry).label() ) ;
   }
   return returnValue;
}


template < class DecayClass , class CandidateClass, class NeutralClass >
inline
DCDecayList< DecayClass, CandidateClass > operator+(
   const DCFillableNeutralList< NeutralClass >& iList1 ,
   const DCDecayList< DecayClass, CandidateClass >& iList2)
{
     DCDecayList< DecayClass, CandidateClass> returnValue;
     returnValue = iList2 + iList1;
     return returnValue;
}

   
// function definitions
#include "DecayChain/List/Template/DCDecayList.cc"

#endif /* DCHAIN_DCDECAYLIST_H */




