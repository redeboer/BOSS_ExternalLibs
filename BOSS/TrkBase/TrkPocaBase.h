//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkPocaBase.h,v 1.2 2006/03/28 01:02:36 zhangy Exp $
//
// Description:
//   Base class for various Poca classes; holds infrastructure, and one 
//   common algorithm.  Ctor and dtor protected, to prevent instantiation.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//------------------------------------------------------------------------
#ifndef TRKPOCABASE_H
#define TRKPOCABASE_H

#include "TrkBase/TrkErrCode.h"
#include "CLHEP/Geometry/Point3D.h" 
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
class Trajectory;

// Class interface
class TrkPocaBase {

protected:
  TrkPocaBase(double flt1, double flt2, double precision);
  TrkPocaBase(double flt1, double precision);
  virtual ~TrkPocaBase();

public:
  inline const TrkErrCode& status() const;     // did the calculation succeed?
  inline double flt1() const;                  // path length on traj 1 @ poca
  inline double flt2() const;
  inline double precision();                   // In case anyone wants to know

protected:
  double _precision;
  double _flt1;
  double _flt2;
  TrkErrCode _status;

  void minimize(const Trajectory& traj1, double f1,
                const Trajectory& traj2, double f2);
  void minimize(const Trajectory& traj1, double f1,
                const HepPoint3D& pt);

  void stepTowardPoca(const Trajectory& traj1, const Trajectory& traj2);
  void stepToPointPoca(const Trajectory& traj, const HepPoint3D& pt);

  static double _maxDist;                   // step > maxDist => parallel
  static int _maxTry;
  static double _extrapToler;            // error allowed in picking step size
};

// Inlined functions
double
TrkPocaBase::precision()                 {return _precision;}

const TrkErrCode&
TrkPocaBase::status() const              {return _status;}

double
TrkPocaBase::flt1() const                {return _flt1;}

double
TrkPocaBase::flt2() const                {return _flt2;}

#endif
