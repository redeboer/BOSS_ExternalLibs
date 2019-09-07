#ifndef DCHAIN_REFERENCEHOLDERBASE_H
#define DCHAIN_REFERENCEHOLDERBASE_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ReferenceHolderBase
// 
// Description: Base implementation of smart pointer for classes that inherit from ReferenceCount
//
// Usage:
//   ReferenceHolderBase()  - default constructor
//   ~ReferenceHolderBase() - destructor
//
// Author:      Simon Patton
// Created:     Fri May 17 08:01:51 EDT 1996
// $Id: ReferenceHolderBase.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ReferenceHolderBase.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:10  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.1  2003/05/15 19:56:02  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
//

// system include files

// user include files


// forward declarations
namespace dchain {
template< class TPtr, class TRef>
class ReferenceHolderBase
{
   public:
      // Constructors and destructor
      ReferenceHolderBase() : m_ref(0) {}
      ReferenceHolderBase(TPtr iRef) : m_ref(iRef) {
         if(0 != m_ref) { m_ref->addLink(); } }
      ~ReferenceHolderBase() {
	 if (0 != m_ref ) {m_ref->dropLink();}
      }

      ReferenceHolderBase( const ReferenceHolderBase<TPtr,TRef>& iOther) :
      m_ref( iOther.m_ref ) {
	if(0 != m_ref ) { 
	  m_ref->addLink();
	}
      }

      // assignment operator(s)
      const ReferenceHolderBase& operator=( const ReferenceHolderBase<TPtr,TRef>& iRHS) {
	if( 0 != iRHS.m_ref ) {
	  iRHS.m_ref->addLink();
	}
	if( 0 != m_ref ) {
	  m_ref->dropLink();
	}
	m_ref = iRHS.m_ref;
	return *this;
      }

      // const member functions
      TPtr operator->() const {
	return m_ref;
      }

      TRef operator*() const {
	return *m_ref;
      }

      TPtr pointer() const {
	return m_ref;
      }

   private:

      // data members
      TPtr m_ref;

};

// inline function definitions

//
// constructors and destructor
//

//
// member functions
//

}
#endif // DCHAIN_REFERENCEHOLDERBASE_H
