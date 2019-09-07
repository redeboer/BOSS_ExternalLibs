#ifndef DCHAIN_REFERENCEHOLDER_H
#define DCHAIN_REFERENCEHOLDER_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ReferenceHolder
// 
// Description: Smart pointer for classes that inherit from ReferenceCount and who should not be modified
//
// Usage:
//   ReferenceHolder()  - default constructor
//   ~ReferenceHolder() - destructor
//
//   This class wraps a pointer to a class inheriting from ReferenceCount
//     and deals with proper handling of the reference counting used to
//     manage the objects lifetime
//
//   ReferenceHolder<Foo> pFoo = new Foo;
//   pFoo->memberFunctionOfFoo();
//   takeReferenceToFoo( *pFoo);
//   takePointerToFoo( pFoo.pointer() );
//
// Author:      Chris D. Jones
// Created:     Wed May 14 08:01:51 EDT 2003
// $Id: ReferenceHolder.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ReferenceHolder.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:10  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1  2003/05/15 19:56:01  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
//

// system include files
#include <memory>

// user include files
#include "DecayChain/Element/ReferenceHolderBase.h"

// forward declarations
namespace dchain {
template< class T>
class ReferenceHolder : public ReferenceHolderBase<const T*, const T&>
{
   public:
      // Constructors and destructor
      ReferenceHolder() {}
      ReferenceHolder(const T* iRef) : ReferenceHolderBase<const T*, const T&>(iRef) {}
      template<class THolder>
      explicit ReferenceHolder(THolder iHolder):
	 ReferenceHolderBase<const T*, const T&>( &(*iHolder)) {}

      explicit ReferenceHolder(std::auto_ptr<T> iHolder):
	 ReferenceHolderBase<const T*, const T&>(iHolder.release()) {}

};
}
#endif // DCHAIN_REFERENCEHOLDER_H
