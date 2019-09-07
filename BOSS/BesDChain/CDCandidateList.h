#ifndef CLEODCHAIN_CDCANDIDATELIST_H
#define CLEODCHAIN_CDCANDIDATELIST_H
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
// $Id: CDCandidateList.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDCandidateList.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2006/01/11 20:37:28  cdj
// work with renaming done in DChain package
//
// Revision 1.1  2001/04/11 13:19:11  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1.1.1  2000/12/18 22:17:25  cdj
// imported CleoDChain
//
// Revision 1.1  1998/04/17 18:53:42  sjp
// New File
//
// Revision 1.1  1997/12/23 21:52:33  sjp
// new typedef to define List
//
// Revision 1.1  1997/08/29 16:57:05  sjp
// New header to specialize CDChargedVisible class
//

// system include files

// user include files
#include "BesDChain/CDCandidate.h"
#include "DecayChain/List/CandidateList.h"

// forward declarations
typedef dchain::CandidateList< CDCandidate > CDCandidateList ;

// inline function definitions

#endif /* CLEODCHAIN_CDCANDIDATELIST_H */
