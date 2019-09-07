#ifndef CLEODCHAIN_CDELECTRONLIST_H
#define CLEODCHAIN_CDELECTRONLIST_H
// -*- C++ -*-
//
// Package:     Carin
// Module:      CDChargedVisibleList
// 
// Description: typedef FillableChargedList< CDElectron > to CDElectronList
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 11 21:45:29 EDT 1996
// $Id: CDElectronList.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDElectronList.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2002/03/27 19:05:45  ajm36
// do template for CDElectronList properly
//
// Revision 1.1  2001/04/11 13:19:15  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2000/12/18 22:54:55  cdj
// get FATable.h from the proper package
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.2  1998/04/17 18:49:44  sjp
// Modifed to use latest CLEO types
//
// Revision 1.1  1997/12/23 21:52:31  sjp
// new typedef to define List
//

// system include files

// user include files
#include "DecayChain/List/DCFillableChargedList.h"

#include "BesDChain/CDElectron.h"

// forward declarations

typedef DCFillableChargedList< CDElectron > CDElectronList ;

#endif /* CLEODCHAIN_CDELECTRONLIST_H */
