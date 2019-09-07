#ifndef CLEODCHAIN_CDMUD_H
#define CLEODCHAIN_CDMUD_H
// -*- C++ -*-
//
// Package:     DChain
// Module:      CDMud
// 
// Description: Manage the footprints for a given Evidence Class
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Mon Apr 20 20:47:30 PDT 1998
// $Id: CDMud.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDMud.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:19  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.1  1998/04/21 05:16:16  sjp
// New File
//

// system include files
#include <vector>

// user include files

// forward declarations
class CDFootPrint ;

//class DummyFriend;

template < class Evidence >
class CDMud
{
      // friend classses and functions
      //friend class DummyFriend;

   public:

      // constants, enums and typedefs

      // Constructors and destructor
      virtual ~CDMud() {}

      // member functions

      // const member functions

      // static member functions
      static const CDFootPrint& get( const Evidence* aEvidence ) ;
      static void clear() ;
//
      static std::vector< const CDFootPrint* >& footprints() ;

   protected:
      // Constructors and destructor

      // protected member functions

      // protected const member functions

   private:
      // Constructors and destructor
      CDMud() ; // stop default
      CDMud( const CDMud& aOtherCDMud ) ; // stop default

      // assignment operator(s)
      const CDMud& operator=( const CDMud& aOtherCDMud ) ; // stop default

      // private member functions

      // private const member functions

      // data members

      // static data members

} ;

# include "BesDChain/Template/CDMud.cc"

#endif /* CLEODCHAIN_CDMUD_H */
