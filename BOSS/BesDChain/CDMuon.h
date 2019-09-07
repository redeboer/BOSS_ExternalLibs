#ifndef CLEODCHAIN_CDMUON_H
#define CLEODCHAIN_CDMUON_H
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
// $Id: CDMuon.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDMuon.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.1  2001/04/11 13:19:19  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1.1.1  2000/12/18 22:17:26  cdj
// imported CleoDChain
//
// Revision 1.2  1998/04/17 18:45:24  sjp
// Changed Package Name
//
// Revision 1.1  1997/08/29 16:57:07  sjp
// New header to specialize CDChargedVisible class
//

// system include files

// user include files
#include "BesDChain/CDChargedVisible.h"

// forward declarations
typedef CDChargedVisible< DBCandidate::kMuon > CDMuon ;
typedef const CDMuon& FixedCDMuon ;
typedef CDMuon& PossibleCDMuon ;

// inline function definitions

#endif /* CLEODCHAIN_CDMUON_H */
