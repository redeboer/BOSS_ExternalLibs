#ifndef DCHAIN_FILLABLENEUTRALNCLIST_H
#define DCHAIN_FILLABLENEUTRALNCLIST_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      FillableNeutralNCList
// 
// Description: <one line class summary>
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep  3 13:47:79 EDT 1997
// $Id: DCFillableNeutralNCList.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: DCFillableNeutralNCList.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.3  2006/01/11 20:28:25  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.2  2002/07/23 20:19:14  cdj
// Can now use dc_fill even when compiler has templated member functions
//
// Revision 1.1  2001/09/11 15:02:26  ajm36
// add files to do neutral non-conjugating particles
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
#include "DecayChain/List/DCNeutralNCVisibleList.h" // superclass declaration

// forward declarations

template < class NeutralNC >
class DCFillableNeutralNCList : public DCNeutralNCVisibleList< NeutralNC , typename NeutralNC::CandidateClass >
{
      // friend classes and functions

   public:
      // constants, enums and typedefs

      // Constructors and destructor
      DCFillableNeutralNCList( bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      template <class WitnessIterator>
      DCFillableNeutralNCList(WitnessIterator first, WitnessIterator last, 
			    bool ( * pFunction )( NeutralNC& ) = 0 ) :
	    DCNeutralNCVisibleList<NeutralNC, typename NeutralNC::CandidateClass>(pFunction){
	 fill(first, last); }

      //DCFillableNeutralNCList( const TruthWitnesses& aTruthList ,
	//		   bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      //DCFillableNeutralNCList( const VisibleWitnesses& aVisibleList ,
	//		   bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      template<class WitnessIterator>
      DCFillableNeutralNCList(WitnessIterator first, WitnessIterator last, 
			    DCSelectionFunction< NeutralNC >& aFunctionalObject )
	 : DCNeutralNCVisibleList<NeutralNC, typename NeutralNC::CandidateClass>(aFunctionalObject) {
	 fill(first, last); }

      //DCFillableNeutralNCList( const TruthWitnesses& aTruthList ,
	//		   DCSelectionFunction< NeutralNC >& aFunctionalObject );
      //DCFillableNeutralNCList( const VisibleWitnesses& aVisibleList ,
	//		   DCSelectionFunction< NeutralNC >& aFunctionalObject );
//
      DCFillableNeutralNCList( const DCFillableNeutralNCList< NeutralNC >& aOtherList
			, bool ( * pFunction )( NeutralNC& ) = 0 ) ;
      DCFillableNeutralNCList( DCSelectionFunction< NeutralNC >& aFunctionalObject ) ;
      DCFillableNeutralNCList( const DCFillableNeutralNCList< NeutralNC >& aOtherList
		        , DCSelectionFunction< NeutralNC >& aFunctionalObject ) ;
      virtual ~DCFillableNeutralNCList() ;

      // assignment operator(s)
      const DCFillableNeutralNCList< NeutralNC >& operator=( const DCFillableNeutralNCList< NeutralNC >& aOtherList ) ;
      
      template<class Witness>
      const DCFillableNeutralNCList<NeutralNC>& operator=(const Witness& aWitnessList) {
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
      //DCFillableNeutralNCList( const DCFillableNeutralNCList< NeutralNC >& ); // stop default

      // private member functions
      void fill( 
	 const dchain::LabeledParticleList< NeutralNC , 
	                          typename NeutralNC::CandidateClass >& aList ) {
	 DCNeutralNCVisibleList< NeutralNC , 
                         typename NeutralNC::CandidateClass >::fill( aList ) ;
      }
      template<class WitnessIterator>
      void fill(WitnessIterator first, WitnessIterator last) {
  DCNeutralNCVisibleList<NeutralNC, typename NeutralNC::CandidateClass>::fill(first,
									last);
      }

      // private const member functions

      // data members

      // static data members

};

// inline function definitions

// templated member function definitions
# include "DecayChain/List/Template/DCFillableNeutralNCList.cc"

template<class NeutralNC, class WitnessIterator>
inline
void dc_fill(DCFillableNeutralNCList<NeutralNC>& aFillableList, 
	     WitnessIterator first, 
	     WitnessIterator last)
{
  aFillableList.fill(first,last);
}

#endif /* DCHAIN_FILLABLENEUTRALNCLIST_H */
