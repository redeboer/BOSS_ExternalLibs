//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkGeomTraj.cxx,v 1.1.1.1 2005/07/18 05:51:56 codeman Exp $
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
#include "MdcGeom/TrkGeomTraj.h"
#include <iostream>
using namespace std;
TrkGeomTraj::TrkGeomTraj(double lowlim, double hilim) : 
  Trajectory(lowlim, hilim)
{
}

TrkGeomTraj::~TrkGeomTraj()
{
}

void 
TrkGeomTraj::accept(TrkGeomTrajVisitor&) const
{
  cout <<"ErrMsg(warning)" << 
    "TrkGeomTraj: accept() invoked for derived class that has\n"
		  << "not overridden it.  No action taken." << endl;
}
