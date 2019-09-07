// ----------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkLineTraj.cxx,v 1.3 2006/03/28 01:02:36 zhangy Exp $
//
//  Description:
//  Single line segment trajectory class
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Berkeley Laboratory
//
//  Authors: Dave Brown, 11/15/96
//-----------------------------------------------------------------------------
#include "CLHEP/Matrix/Matrix.h"
#include "TrkBase/TrkLineTraj.h"
#include "MdcGeom/TrkGeomTrajVisitor.h"

TrkLineTraj::TrkLineTraj(const HepPoint3D& point, const Hep3Vector& direction,
                        double length) :
  TrkGeomTraj(0.0,length) , _start(point),
  _direction(direction)
{
  _direction.setMag(1.0);
}

TrkLineTraj::TrkLineTraj
(const HepPoint3D& point,
 const Hep3Vector& direction,
 double lorange,
 double hirange):
  TrkGeomTraj(lorange,hirange),
  _start(point),
  _direction(direction)
{
  _direction.setMag(1.0);
}



TrkLineTraj::TrkLineTraj(const HepPoint3D& point1, const HepPoint3D& point2)
  : TrkGeomTraj(0.0,point1.distance2(point2)), _start(point1)
{
  //_direction = point2 - point1;
  _direction = ((CLHEP::Hep3Vector) point2) - ((CLHEP::Hep3Vector) point1);
  _direction.setMag(1.0);
}

TrkLineTraj::TrkLineTraj(const TrkLineTraj& other) :
  TrkGeomTraj(other.lowRange(),other.hiRange()), _start(other._start),
  _direction(other._direction)
{
}

TrkLineTraj::~TrkLineTraj()
{
}

TrkLineTraj& TrkLineTraj::operator = (const TrkLineTraj& other)
{
  if(&other != this){
    Trajectory::operator=(other);
    _start = other._start;
    _direction = other._direction;
  }
  return *this;
}

TrkLineTraj*
TrkLineTraj::clone() const
{
  return new TrkLineTraj(*this);
}

HepPoint3D
TrkLineTraj::position(double flightlen) const
{
  return _start + _direction*flightlen;
}

Hep3Vector
TrkLineTraj::direction(double ) const
{
  return _direction;
}

Hep3Vector
TrkLineTraj::delDirect( double ) const
{
  return Hep3Vector(0.,0.,0.);
}

double
TrkLineTraj::distTo1stError(double , double, int ) const
{
  return 999.e4;
}

double
TrkLineTraj::distTo2ndError(double, double, int ) const
{
  return 999.e4;
}

double
TrkLineTraj::curvature( double ) const
{
  return 0.0;
}

void
TrkLineTraj::getInfo(double fltLen, HepPoint3D& pos, Hep3Vector& dir) const
{
  pos = position(fltLen);
  dir = direction(fltLen);
}

void
TrkLineTraj::getInfo(double fltLen, HepPoint3D& pos, Hep3Vector& dir,
                                    Hep3Vector& delDir) const
{
  pos = position(fltLen);
  dir = direction(fltLen);
  delDir = delDirect(fltLen);
}

void
TrkLineTraj::accept(TrkGeomTrajVisitor& visitor) const
{
  visitor.visitLine(this);
}
