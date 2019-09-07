#ifndef DCHAIN_MAKERTRAIT_H
#define DCHAIN_MAKERTRAIT_H
// -*- C++ -*-
//
// Package:     <DChain>
// Module:      MakerTrait
// 
/**\class MakerTrait MakerTrait.h DChain/MakerTrait.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      Chris D Jones
// Created:     Thu Jan  5 15:37:46 EST 2006
// $Id: MakerTrait.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: MakerTrait.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:09  cdj
// massive class renaming, addition of [] for selection and unit tests
//

// system include files

// user include files

// forward declarations
namespace dchain {
   template<class T>
   struct NewMaker {

	 typedef ReferenceHolderBase<T*, T&> result_type;

	 template<class U>
	 result_type operator()(const U& iU) const {
	    return result_type(new T(iU));
	 }
   };

   template<class T>
   struct MakerTrait {
	 typedef NewMaker<T> maker_type;
   };

   template<class T>
   struct ResultTypeTrait {
	 typedef typename T::result_type type;
   };
   
   template<class TReturn, class TArg>
   struct ResultTypeTrait< TReturn (*)(const TArg&)> {
      typedef TReturn type;
   };

}
// inline function definitions


#endif /* DCHAIN_MAKERTRAIT_H */
