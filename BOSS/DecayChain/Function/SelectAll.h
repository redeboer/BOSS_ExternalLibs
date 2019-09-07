#ifndef DCHAIN_SELECTALL_H
#define DCHAIN_SELECTALL_H
// -*- C++ -*-
//
// Package:     <DChain>
// Module:      SelectAll
// 
/**\class SelectAll SelectAll.h DChain/SelectAll.h

 Description: Returns 'true' for all candidates passed to it

 Usage:
    <usage>

*/
//
// Author:      Chris D Jones
// Created:     Sat Dec 24 12:36:16 EST 2005
// $Id: SelectAll.h,v 1.1.1.1 2009/03/03 06:06:56 maqm Exp $
//
// Revision history
//
// $Log: SelectAll.h,v $
// Revision 1.1.1.1  2009/03/03 06:06:56  maqm
// first import of DecayChain
//
// Revision 1.1  2006/01/11 20:28:14  cdj
// massive class renaming, addition of [] for selection and unit tests
//

// system include files

// user include files

// forward declarations

namespace dchain {
   template<class T>
   struct SelectAll {
	 bool operator()(const T&) const {
	    return true;
	 }
   };
}
#endif /* DCHAIN_SELECTALL_H */
