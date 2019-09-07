#ifndef DCHAIN_MUTABLEREFERENCEHOLDER_H
#define DCHAIN_MUTABLEREFERENCEHOLDER_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      MutableReferenceHolder
// 
// Description: Smart pointer for classes that inherit from ReferenceCount and who can be modified
//
// Usage:
//   This class wraps a pointer to a class inheriting from ReferenceCount
//     and deals with proper handling of the reference counting used to
//     manage the objects lifetime
//
//   MutableReferenceHolder<Foo> pFoo = new Foo;
//   pFoo->bar();
//   takeReferenceToFoo( *pFoo);
//   takePointerToFoo( pFoo.pointer() );
//
// Author:      Chris Jones
// Created:     Wed May 14 08:01:51 EDT 2003
// $Id: MutableReferenceHolder.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: MutableReferenceHolder.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:09  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1  2003/05/15 19:56:01  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
//

// system include files

// user include files
#include "DecayChain/Element/ReferenceHolderBase.h"

// forward declarations
namespace dchain {
template< class T>
class MutableReferenceHolder : public ReferenceHolderBase<T*, T&>
{
   public:
      // Constructors and destructor
      MutableReferenceHolder() {}
      MutableReferenceHolder(T* iRef) : ReferenceHolderBase<T*, T&>(iRef) {}

};
}
#endif // DCHAIN_MUTABLEREFERENCEHOLDER_H
