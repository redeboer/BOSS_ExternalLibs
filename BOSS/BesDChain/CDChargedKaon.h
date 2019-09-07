#ifndef CLEODCHAIN_CDCHARGEDKAON_H
#define CLEODCHAIN_CDCHARGEDKAON_H
// -*- C++ -*-
//
// Package:     CleoDChain
// Module:      CDChargedVisible
// 
// Description: CDCharged candidate built from charged or truth table
//
// Usage:
//    <usage>
//
// Author:      Simon Patton
// Created:     Wed Sep 18 14:47:30 EDT 1996
// $Id: CDChargedKaon.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDChargedKaon.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:12  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.2  1998/04/17 18:45:26  sjp
// Changed Package Name
//
// Revision 1.1  1997/08/29 16:57:16  sjp
// New header to specialize CDChargedVisible class
//

// system include files

// user include files
#include "BesDChain/CDChargedVisible.h"

// forward declarations
typedef CDChargedVisible< DBCandidate::kChargedKaon > CDChargedKaon ;
typedef const CDChargedKaon& FixedCDChargedKaon ;
typedef CDChargedKaon& PossibleCDChargedKaon ;

// inline function definitions

#endif /* CLEODCHAIN_CDCHARGEDKAON_H */
