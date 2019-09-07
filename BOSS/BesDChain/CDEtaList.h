#ifndef PACKAGE_CDETALIST_H
#define PACKAGE_CDETALIST_H
// -*- C++ -*-
//
// Package:     <package>
// Module:      CDPi0List
// 
/**\class CDEtaList CDEtaList.h package/CDEtaList.h

 Description: <one line class summary>

 Usage:
    <usage>

//petez (2009.2.25): This is a modified copy of BesDChain CDPi0List.h
*/

// system include files

// user include files
#include "DecayChain/List/DCFillableNeutralList.h"
#include "BesDChain/CDEta.h"

// forward declarations
typedef DCFillableNeutralList<CDEta> CDEtaList;

#endif /* PACKAGE_CDETALIST_H */
