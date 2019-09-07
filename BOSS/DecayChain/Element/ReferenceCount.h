#ifndef DCHAIN_REFERENCECOUNT_H
#define DCHAIN_REFERENCECOUNT_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      ReferenceCount
// 
// Description: Base class for classes that need reference counting
//
// Usage:
//   ReferenceCount()  - default constructor
//   ~ReferenceCount() - destructor
//   addLink()  - record that another object points to this object
//   dropLink() - record that an object no longer points to this object
//
// Author:      Simon Patton
// Created:     Fri May 17 08:01:51 EDT 1996
// $Id: ReferenceCount.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: ReferenceCount.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:09  cdj
// massive class renaming, addition of [] for selection and unit tests
//
// Revision 1.4  2004/05/12 20:12:52  ryd
// Added missing #include
//
// Revision 1.3  2004/05/11 02:39:17  ryd
// Change reference count to UInt32 and throw exception if count exceeds maximum
//
// Revision 1.2  2003/05/15 19:56:01  cdj
// revamped memory handling so always use a ReferenceHolder to deal with the reference counting
//
// Revision 1.1.1.1  2000/12/18 22:16:49  cdj
// imported DChain
//
// Revision 1.2  1998/08/21 00:46:09  sjp
// removed unnecessary include
//
// Revision 1.1  1998/08/19 20:56:46  sjp
// New heder for reference counting class
//
// Revision 1.1  1998/08/17 23:59:22  sjp
// New file to make libraries standalone
//

// system include files
#include <stdint.h>

// user include files
//#include "DecayChain/util/DAException.h"

// forward declarations
namespace dchain {

   /*
   class TooManyReferencesException : public DAException
   {
      public:
	 TooManyReferencesException():DAException("Too many references to object in a DChain list.\n check sizes of list that you multiply."){}
   };
   */


class ReferenceCount
{
   public:
      // Constructors and destructor
      ReferenceCount();
      virtual ~ReferenceCount() {}

      // const member functions
      void addLink() const ;
      void dropLink() const ;

   private:
      // Constructors and destructor
      ReferenceCount( const ReferenceCount& ) ; // stop default

      // assignment operator(s)
      const ReferenceCount& operator=( const ReferenceCount& ) ; // stop default

      // data members
      mutable uint32_t m_linkCount ;

};

// inline function definitions

//
// constructors and destructor
//

inline ReferenceCount::ReferenceCount() :
   m_linkCount( 0 )
{
}

//
// member functions
//

inline void ReferenceCount::addLink() const
{
   //if (m_linkCount==0xFFFFFFFF) {
   //   throw TooManyReferencesException();
   //}
   ++m_linkCount ;
}

inline void ReferenceCount::dropLink() const
{
   --m_linkCount ;
   if ( 0 == m_linkCount ) {
// It is necessary to throw away the 'const' so that the object
//   can be deleted
      delete const_cast<ReferenceCount*>(this) ;
   }
}
}
#endif // DCHAIN_REFERENCECOUNT_H
