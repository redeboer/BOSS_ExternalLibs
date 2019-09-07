// ----------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkLineTraj.h,v 1.2 2005/07/18 02:56:07 zhangy Exp $
//
//  Description:
//  Single line segment trajectory class
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Berkeley Laboratory
//
//  Authors: Dave Brown, 11/15/96
//-----------------------------------------------------------------------------
#ifndef TrkLineTraj_H
#define TrkLineTraj_H
#include "MdcGeom/TrkGeomTraj.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/ThreeVector.h"

class TrkLineTraj : public TrkGeomTraj
{
public:
//
//  construct from a point, direction and length, or 2 points
//
  TrkLineTraj( const HepPoint3D& point, const Hep3Vector& direction, double length);
  TrkLineTraj(const HepPoint3D& point, const Hep3Vector& direction, double lorange, double hirange);
  TrkLineTraj( const HepPoint3D& point1, const HepPoint3D& point2);
  TrkLineTraj( const TrkLineTraj&  );   // copy ctor
//
  TrkLineTraj* clone() const;

// destructor
  ~TrkLineTraj();

// operators
  TrkLineTraj& operator=(const TrkLineTraj&);

// needed implementations for intersection with a Surface
  HepPoint3D position( double ) const;
  Hep3Vector direction( double ) const;
  double curvature( double f = 0. ) const;
  Hep3Vector delDirect( double ) const;
  void  getInfo(double fltLen, HepPoint3D& , Hep3Vector& direction) const;
  void  getInfo(double fltLen, HepPoint3D& , Hep3Vector& direction, 
			Hep3Vector& delDirect) const;

  virtual double distTo1stError(double s, double tol, int pathDir) const;
  virtual double distTo2ndError(double s, double tol, int pathDir) const;

  // Support Visitor pattern (see TrkGeomTraj.hh)
  void accept(TrkGeomTrajVisitor& visitor) const;

// Data Members
private:
  HepPoint3D _start; // where the trajectory starts
  Hep3Vector _direction; // direction (unit) vector
};
#endif
