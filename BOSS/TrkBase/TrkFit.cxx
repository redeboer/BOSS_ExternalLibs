//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkFit.cxx,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.hh"
#include "TrkBase/TrkFit.h"
#include <iostream>
#include "MdcRecoUtil/Pdt.h"
using std::ostream;

//------------------------------------------------------------------------
TrkFit::~TrkFit() {
//------------------------------------------------------------------------
}

//------------------------------------------------------------------------
TrkFit::TrkFit() {
//------------------------------------------------------------------------
}

void 
TrkFit::printType(ostream& ostr) const 
{
  ostr << "Particle type: " << Pdt::lookup( particleType() )->name();
}

// default implementation of validFlightLength
bool
TrkFit::validFlightLength(double fltl,double tolerance) const {
  return fltl+tolerance >= startValidRange() &&
    fltl-tolerance <= endValidRange();
}
