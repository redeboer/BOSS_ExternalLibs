#ifndef CLEODCHAIN_CDDECAYLIST_H
#define CLEODCHAIN_CDDECAYLIST_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDDecay
// 
// Description: declare specialized ConjugateList< CDDecay >
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDDecayList.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDDecayList.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:14  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.2  1998/04/17 18:51:30  sjp
// Changed Package Name
//
// Revision 1.1  1997/12/23 21:52:35  sjp
// new typedef to define List
//
// Revision 1.1  1997/08/29 16:57:05  sjp
// New header to specialize CDChargedVisible class
//

// system include files

// user include files
#include "BesDChain/CDDecay.h"
#include "DecayChain/List/DCDecayList.h"

// forward declarations
typedef DCDecayList< CDDecay , CDDecay::CandidateClass > CDDecayList ;

// inline function definitions

#endif /* CLEODCHAIN_CDDECAYLIST_H */
