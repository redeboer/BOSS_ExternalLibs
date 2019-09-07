//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: TrkVolume.h,v 1.2 2006/03/28 01:02:36 zhangy Exp $
//
// Description:
//	TrkVolume Class -  
//      Abstract interface for the description
//      of the tracking volume of any subdetector
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	Gautier Hamel de Monchenault - CEN Saclay & Lawrence Berkeley Lab
//
// History (add to end):
//      Gautier   May 6, 1997  - creation
//      Dave Brown 5/11/97  Add a name member
//
// Copyright Information:
//	Copyright (C) 1997		Lawrence Berkeley Laboratory
//	Copyright (C) 1997	       CEA - Centre d'Etude de Saclay
//
//------------------------------------------------------------------------

#ifndef TRKVOLUME_HH
#define TRKVOLUME_HH

#include "TrkBase/TrkDirection.h"
#include <string>
#include "CLHEP/Geometry/Point3D.h"

#ifndef ENABLE_BACKWARDS_COMPATIBILITY
//  backwards compatibility will be enabled ONLY in CLHEP 1.9
typedef HepGeom::Point3D<double> HepPoint3D;
#endif

class Trajectory;

class TrkVolume {

public:
// Default ctor
  TrkVolume();
  TrkVolume(const char*);
// Destructor
  virtual ~TrkVolume();
//
  virtual bool extendThrough( const Trajectory* theTraj,
			      double& theFlightDist,
			      trkDirection theDirection=trkOut,
			      double* theStartingFlightDist=0) const = 0;
// returns false if the point is outside the volume
  virtual bool isInside( const HepPoint3D& ) const = 0;
// Access to the name
  const std::string& name() const {return _tvname;}
private:
  std::string _tvname; // define the volume name
};

#endif
