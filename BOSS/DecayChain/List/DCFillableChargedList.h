#ifndef DCHAIN_DCFILLABLECHARGEDLIST_H
#define DCHAIN_DCFILLABLECHARGEDLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableChargedList
// 
// Description: <one line class summary>
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep  3 13:47:79 EDT 1997
// $Id: DCFillableChargedList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableChargedList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.6  2006/01/11 20:28:25  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.5  2004/09/09 15:48:22  emilie
// made the fill function public
//
// Revision 1.4  2002/07/23 20:19:13  cdj
// Can now use dc_fill even when compiler has templated member functions
//
// Revision 1.3  2000/12/30 19:43:30  cdj
// no more missing symbols under OSF1
//
// Revision 1.2  2000/12/19 23:40:09  cdj
// dc_fill now works correctly
//
// Revision 1.1.1.1  2000/12/18 22:16:52  cdj
// imported DChain
//
// Revision 1.4  1998/08/20 19:59:01  sjp
// Modified to use DChainBoolean
//
// Revision 1.3  1998/08/19 20:39:52  sjp
// Fixed include path for defintions
//
// Revision 1.2  1998/04/17 19:11:02  sjp
// Modified to use latest types
//
// Revision 1.1  1997/09/17 20:06:17  sjp
// New class to separate filling from behaviour
//

// system include files

// user include files
#include "DecayChain/List/DCChargedVisibleList.h" // superclass declaration

// forward declarations

template < class Charged >
class DCFillableChargedList : public DCChargedVisibleList< Charged , typename Charged::CandidateClass >
{
      // friend classes and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCFillableChargedList( bool ( * pFunction )( Charged& ) = 0 ) ;
      template <class WitnessIterator>
      DCFillableChargedList(WitnessIterator first, WitnessIterator last, 
			    bool ( * pFunction )( Charged& ) = 0 ) :
	    DCChargedVisibleList<Charged, typename Charged::CandidateClass>(pFunction){
	 fill(first, last); }

      //DCFillableChargedList( const TruthWitnesses& aTruthList ,
	//		   bool ( * pFunction )( Charged& ) = 0 ) ;
      //DCFillableChargedList( const VisibleWitnesses& aVisibleList ,
	//		   bool ( * pFunction )( Charged& ) = 0 ) ;
      template<class WitnessIterator>
      DCFillableChargedList(WitnessIterator first, WitnessIterator last, 
			    DCSelectionFunction< Charged >& aFunctionalObject )
	 : DCChargedVisibleList<Charged, typename Charged::CandidateClass>(aFunctionalObject) {
	 fill(first, last); }

      //DCFillableChargedList( const TruthWitnesses& aTruthList ,
	//		   DCSelectionFunction< Charged >& aFunctionalObject );
      //DCFillableChargedList( const VisibleWitnesses& aVisibleList ,
	//		   DCSelectionFunction< Charged >& aFunctionalObject );
//
      DCFillableChargedList( const DCFillableChargedList< Charged >& aOtherList
			, bool ( * pFunction )( Charged& ) = 0 ) ;
      DCFillableChargedList( DCSelectionFunction< Charged >& aFunctionalObject ) ;
      DCFillableChargedList( const DCFillableChargedList< Charged >& aOtherList
		        , DCSelectionFunction< Charged >& aFunctionalObject ) ;
      virtual ~DCFillableChargedList() ;

      // assignment operator(s)
      const DCFillableChargedList< Charged >& operator=( const DCFillableChargedList< Charged >& aOtherList ) ;
      
      template<class Witness>
      const DCFillableChargedList<Charged>& operator=(const Witness& aWitnessList) {
	 this->eraseContents();
	 fill(aWitnessList.begin(), aWitnessList.end());
	 return ( *this ) ;
      }

      // member functions
      template<class WitnessIterator>
	 void fill(WitnessIterator first, WitnessIterator last) {
	    DCChargedVisibleList<Charged, typename Charged::CandidateClass>::fill(first,
		  last);
	 }

      // const member functions

      // static member functions

   protected:
      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      //DCFillableChargedList( const DCFillableChargedList< Charged >& ); // stop default

      // private member functions
      void fill( 
	 const dchain::LabeledParticleList< Charged , 
	                          typename Charged::CandidateClass >& aList ) {
	 DCChargedVisibleList< Charged , 
                         typename Charged::CandidateClass >::fill( aList ) ;
      }

      // private const member functions

      // data members

      // static data members

};

// inline function definitions

// templated member function definitions
# include "DecayChain/List/Template/DCFillableChargedList.cc"

template<class Charged, class WitnessIterator>
inline
void dc_fill(DCFillableChargedList<Charged>& aFillableList, 
	     WitnessIterator first, 
	     WitnessIterator last)
{
  aFillableList.fill(first,last);
}

#endif /* DCHAIN_DCFILLABLECHARGEDLIST_H */
