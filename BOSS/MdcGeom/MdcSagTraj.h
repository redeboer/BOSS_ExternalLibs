//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: MdcSagTraj.h,v 1.3 2009/12/17 00:38:40 zhangy Exp $
//
// Description:
//	Class MdcSagTraj.
//      the flight length at the moment is the flight length of a linear 
//      trajectory, the sag is approximated with a parabola (for the
//      moment, maybe at a later time we can replace with a more accurate
//      description, cosh for example)
//      
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//	R. Stroili		originator
//	
//
// Copyright Information:
//	Copyright (C) 1998	INFN & Padova University
// 
// History:
//	Migration for BESIII MDC
//
//------------------------------------------------------------------------

#ifndef DCHSAGTRAJ_H
#define DCHSAGTRAJ_H

//-------------
// C Headers --
//-------------
extern "C" {
}

//---------------
// C++ Headers --
//---------------

//----------------------
// Base Class Headers --
//----------------------
#include "MdcGeom/TrkGeomTraj.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "CLHEP/Geometry/Point3D.h"
#ifndef ENABLE_BACKWARDS_COMPATIBILITY
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#include "CLHEP/Vector/ThreeVector.h"
using CLHEP::Hep3Vector;

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
class Trajectory;

//		---------------------
// 		-- Class Interface --
//		---------------------

class MdcSagTraj : public TrkGeomTraj {

public:

  // Constructors
  MdcSagTraj(const double sag, const HepPoint3D& point1, const HepPoint3D& point2);
  MdcSagTraj(const MdcSagTraj& traj);
 
  MdcSagTraj* clone() const;
  // Destructor
  virtual ~MdcSagTraj( );

  // Operators
  MdcSagTraj& operator=(const MdcSagTraj&);

  double sag( void ) const { return _sag; }

// needed implementations for intersection with a Surface
  HepPoint3D position( double ) const;
  Hep3Vector direction( double ) const;
  double curvature( double f = 0. ) const;
  Hep3Vector delDirect( double ) const;
  const Hep3Vector& rawDirection( void ) const { return _direction; }
  void  getInfo(double fltLen, HepPoint3D& , Hep3Vector& direction) const;
  void  getInfo(double fltLen, HepPoint3D& , Hep3Vector& direction, 
			Hep3Vector& delDirect) const;

  virtual double distTo1stError(double s, double tol, int pathDir) const;
  virtual double distTo2ndError(double s, double tol, int pathDir) const;
  double deltaY(double fltLen) const { return (_a*fltLen+_b)*fltLen; }
  // Support Visitor pattern (see TrkGeomTraj.h)
  void accept(TrkGeomTrajVisitor& visitor) const;

//    virtual int operator==( const MdcSagTraj& ) const;
//            int operator!=( const MdcSagTraj& ) const;

private:

  // Data members
  double _sag;
  double _a;
  double _b;
  double _length;
//   double _a_H;
//   double _b_H;
  HepPoint3D _start; // where the trajectory starts
  HepPoint3D _stop; // where the trajectory stops
  Hep3Vector _direction; // direction (unit) vector for 
                         // null sag (straight line)

  Hep3Vector deviation(double) const; // displacement from the line trajectory
                                      // at a given flightlength
  //Note: if your class needs a copy constructor or an assignment operator, 
  //  make one of the following public and implement it.
//   MdcSagTraj( const MdcSagTraj& );       // Copy Constructor
//   MdcSagTraj&       operator= ( const MdcSagTraj& );  // Assignment op

};

#endif // DCHSAGTRAJ_H
