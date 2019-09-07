//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: Trajectory.cxx,v 1.2 2009/12/17 00:38:40 zhangy Exp $
// 
// Description:
//	Trajectory Virtual Base Class 
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      David N Brown                - Lawrence Berkeley Lab
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//      Steve Schaffner              - SLAC
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Berkeley Laboratory
// 
// History:
//	Migration for BESIII MDC
//	Copyright (C) 1996	CEA - Centre d'Etude de Saclay
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------
//#include "BaBar/BaBar.hh"
#include "MdcGeom/Trajectory.h"
//#include "ErrLogger/ErrLog.hh"
#include <math.h>
#include <iostream>
using std::endl;
using std::ostream;

Trajectory::Trajectory(double lowlim,double hilim)
{
  if (hilim < lowlim) {
    std::cout<<"ErrMsg(error) " << "Invalid Trajectory range requested." << std::endl;
    flightrange[0] = hilim;
    flightrange[1] = lowlim;
  }
  flightrange[0] = lowlim;
  flightrange[1] = hilim;
}

Trajectory::~Trajectory( )
{
}

double
Trajectory::distTo0thError(double , double tolerance, int) const
{
  return fabs(tolerance);
}

void
Trajectory::setFlightRange(double newrange[2])
{
  if (newrange[1] >= newrange[0]) {
    flightrange[0] = newrange[0];
    flightrange[1] = newrange[1];
  } else {
    std::cout<<"ErrMsg(error) "<< "Invalid Trajectory range requested." << std::endl;
    flightrange[0] = newrange[1];
    flightrange[1] = newrange[0];
  }
}

void
Trajectory::print(ostream& os) const
{
  os << "Trajectory range from " 
     << flightrange[0] << " to "
     << flightrange[1] << endl;
}

void
Trajectory::printAll(ostream& os) const
{
  os << "Trajectory range from "
     << flightrange[0] << " to "
     << flightrange[1] << endl;
}

Trajectory&
Trajectory::operator=(const Trajectory& other)
{
 flightrange[0] = other.flightrange[0];
 flightrange[1] = other.flightrange[1];
  return *this;
}
