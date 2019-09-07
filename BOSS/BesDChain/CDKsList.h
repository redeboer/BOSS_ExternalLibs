#ifndef PACKAGE_CDKSLIST_H
#define PACKAGE_CDKSLIST_H
// -*- C++ -*-
//
// Package:     <package>
// Module:      CDKsList
// 
/**\class CDKsList CDKsList.h package/CDKsList.h

 Description: <one line class summary>

 Usage:
    <usage>

*/
//
// Author:      David Urner
// Created:     Fri Mar 16 14:38:59 EST 2001
// $Id: CDKsList.h,v 1.1.1.1 2009/03/03 06:05:56 maqm Exp $
//
// Revision history
//
// $Log: CDKsList.h,v $
// Revision 1.1.1.1  2009/03/03 06:05:56  maqm
// first import of BesDChain
//
// Revision 1.2  2006/01/13 14:41:19  cdj
// removed unnecessary includes of DChainBoolean.h
//
// Revision 1.1  2001/04/11 13:19:18  urner
// transition to files with CD prefix. Addition of new files
//
// Revision 1.1  2001/03/23 23:05:36  urner
// added pi0 eta and CDKs decay lists
//

// system include files

// user include files
#include "DecayChain/List/DCFillableNeutralList.h"
#include "BesDChain/CDKs.h"

// forward declarations
typedef DCFillableNeutralList<CDKs> CDKsList;

#endif /* PACKAGE_CDKSLIST_H */
