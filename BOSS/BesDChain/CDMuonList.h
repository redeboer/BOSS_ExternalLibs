#ifndef CLEODCHAIN_CDMUONLIST_H
#define CLEODCHAIN_CDMUONLIST_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDChargedVisible
// 
// Description: declare specialized CDChargedVisibleList
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDMuonList.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDMuonList.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/09/11 15:08:31  ajm36
// add files for lambdas
//
// Revision 1.1  2001/04/11 13:19:13  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.2  2000/12/19 15:10:52  cdj
// updated to new DChain interface
//
// Revision 1.1  2000/12/18 22:54:15  cdj
// added CDChargedKaon and CDMuon lists
//
// Revision 1.2  1998/04/17 18:52:22  sjp
// Modifed to use latest CLEO types
//
// Revision 1.1  1997/12/23 21:52:33  sjp
// new typedef to define List
//
// Revision 1.1  1997/08/29 16:57:05  sjp
// New header to specialize CDChargedVisible class
//

// system include files

// user include files
#include "BesDChain/CDMuon.h"
#include "DecayChain/List/DCFillableChargedList.h"

// forward declarations
typedef DCFillableChargedList< CDMuon > CDMuonList ;

// inline function definitions

#endif /* CLEODCHAIN_CDMUONLIST_H */
