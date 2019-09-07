// MdcSegWorks.h -- structure to hold some pass some calculated quantities to
//  segInfo class

// Interface Dependencies ----------------------------------------------

#ifndef MDCSEGWORKS_H
#define MDCSEGWORKS_H
#include "MdcGeom/BesAngle.h"

//  End Interface Dependencies -----------------------------------------

// Class definition//
class MdcSegWorks {
public:
  double wirLen2inv;
  double phiArg;
  BesAngle phiAx;
};

#endif







