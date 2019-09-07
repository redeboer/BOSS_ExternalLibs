#ifndef PACKAGE_CDPI0LIST_H
#define PACKAGE_CDPI0LIST_H
// -*- C++ -*-
//
// Package:     <package>
// Module:      CDPi0List
// 
/**\class CDPi0List CDPi0List.h package/CDPi0List.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      David Urner
// Created:     Fri Mar 16 14:38:59 EST 2001
// $Id: CDPi0List.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDPi0List.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2006/01/12 13:46:24  cdj
// removed a now defunct DChainBoolean.h include
//
// Revision 1.1  2001/04/11 13:19:22  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1  2001/03/23 23:05:38  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files
#include "DecayChain/List/DCFillableNeutralList.h"
#include "BesDChain/CDPi0.h"

// forward declarations
typedef DCFillableNeutralList<CDPi0> CDPi0List;

#endif /* PACKAGE_CDPI0LIST_H */
