#ifndef DCHAIN_FILLABLENEUTRALLIST_H
#define DCHAIN_FILLABLENEUTRALLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableNeutralList
// 
// Description: List to hold Candidates generated from observed Tracks.
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Fri Oct  4 11:04:47 EDT 1996
// $Id: DCFillableNeutralList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableNeutralList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.6  2006/01/11 20:28:25  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.5  2003/12/16 15:45:41  cdj
// added include file needed for Linux
//
// Revision 1.4  2002/07/23 20:19:13  cdj
// Can now use dc_fill even when compiler has templated member functions
//
// Revision 1.3  2001/03/28 14:27:57  urner
// added same workaround as for charged because of NonTemplatedMemberFunctionBug
//
// Revision 1.2  2001/03/23 23:07:44  urner
// modification needed to add pi0 eta and Ks decay lists in CleoDChain
//
// Revision 1.1.1.1  2000/12/18 22:16:52  cdj
// imported DChain
//
// Revision 1.3  1998/08/20 19:59:04  sjp
// Modified to use DChainBoolean
//
// Revision 1.2  1998/04/17 19:11:03  sjp
// Modified to use latest types
//
// Revision 1.1  1997/09/17 20:06:20  sjp
// New class to separate filling from behaviour
//

// system include files
#include <vector>

// user include files
#include "DecayChain/List/DCNeutralVisibleList.h" // superclass declaration

// forward declarations

template< class Neutral >
//          class TruthWitnesses ,
//          class VisibleWitnesses >
class DCFillableNeutralList : 
   public DCNeutralVisibleList< Neutral , 
                                typename Neutral::CandidateClass >
{
      // friend classes and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCFillableNeutralList( bool ( * pFunction )( Neutral& ) = 0 ) ;
      template <class WitnessIterator>
      DCFillableNeutralList(WitnessIterator first, WitnessIterator last,
	                    bool ( * pFunction )( Neutral& ) = 0 ) :
	 DCNeutralVisibleList<Neutral, typename Neutral::CandidateClass>(pFunction) {
	    fill(first, last); }
      //DCFillableNeutralList( const TruthWitnesses& aTruthList ,
	//		   bool ( * pFunction )( Neutral& ) = 0 ) ;
      //DCFillableNeutralList( const VisibleWitnesses& aVisibleList ,
	//		   bool ( * pFunction )( Neutral& ) = 0 ) ;
      //DCFillableNeutralList( const TruthWitnesses& aTruthList ,
	//		   DCSelectionFunction< Neutral >& aFunctionalObject ) ;
      //DCFillableNeutralList( const VisibleWitnesses& aVisibleList ,
	//		   DCSelectionFunction< Neutral >& aFunctionalObject ) ;
//
      template <class WitnessIterator>
      DCFillableNeutralList(WitnessIterator first, WitnessIterator last,
	                    DCSelectionFunction< Neutral >& aFunctionalObject ) :
	 DCNeutralVisibleList<Neutral, typename Neutral::CandidateClass>(aFunctionalObject) {
	    fill(first, last); }
      DCFillableNeutralList( const DCFillableNeutralList< Neutral >& aOtherList ,
			   bool ( * pFunction )( Neutral& ) = 0 ) ;
      DCFillableNeutralList( DCSelectionFunction< Neutral >& aFunctionalObject ) ;
      DCFillableNeutralList( const DCFillableNeutralList< Neutral >& aOtherList ,
			   DCSelectionFunction< Neutral >& aFunctionalObject ) ;
      DCFillableNeutralList( const std::vector<Neutral>& aVector ) ;
      virtual ~DCFillableNeutralList() ;
      
      // assignment operator(s)
      //const DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >& operator=( const TruthWitnesses& aTruthList ) ;
      //const DCFillableNeutralList< Neutral , TruthWitnesses , VisibleWitnesses >& operator=( const VisibleWitnesses& aVisibleList ) ;
//
      const DCFillableNeutralList< Neutral >& operator=( const DCFillableNeutralList< Neutral >& aOtherList );



      template<class WitnessIterator>
      void fill(WitnessIterator first, WitnessIterator last) {
  DCNeutralVisibleList< Neutral , typename Neutral::CandidateClass >::fill(first,last);
      }

      template<class Witness>
      const DCFillableNeutralList<Neutral>& operator=(const Witness& aWitnessList) {
	 this->eraseContents();
	 fill(aWitnessList.begin(), aWitnessList.end());
	 return ( *this ) ;
      }
      
      // member functions

      // const member functions

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor

      // private member functions
      //void fill( const TruthWitnesses& aList ) ;
      //void fill( const VisibleWitnesses& aList ) ;
      void fill( const dchain::LabeledParticleList< Neutral , typename Neutral::CandidateClass >& aList ) ;

      // private const member functions

      // data members

      // static data members

} ;

// inline function definitions

// function definitions
#include "DecayChain/List/Template/DCFillableNeutralList.cc"

template<class Neutral, class WitnessIterator>
inline
void dc_fill(DCFillableNeutralList<Neutral>& aFillableList, 
	     WitnessIterator first, 
	     WitnessIterator last)
{
  aFillableList.fill(first,last);
}


#endif /* DCHAIN_FILLABLENEUTRALLIST_H */
