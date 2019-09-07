//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkDifTraj.cxx,v 1.2 2005/07/18 02:56:08 zhangy Exp $
//
// Description:
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#include "TrkBase/TrkDifTraj.h"
#include "MdcRecoUtil/DifVector.h"
#include "MdcGeom/Trajectory.h"
//Constructor
TrkDifTraj::TrkDifTraj(const double lowlim,const double hilim) :
  Trajectory(lowlim, hilim) {
}
 
TrkDifTraj::~TrkDifTraj() {}

void  
TrkDifTraj::getDFInfo2(double fltLen, DifPoint& pos, DifVector& direction) const {
  // Slow default implementation.  Override in subclasses where speed matters
  DifVector dummy;
  getDFInfo(fltLen, pos, direction, dummy);
}




