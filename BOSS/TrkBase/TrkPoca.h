//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkPoca.h,v 1.1.1.1 2005/04/21 06:01:42 zhangy Exp $
//
// Description:
//     Calculate the point of closest approach between two trajectories, 
//     or between a trajectory and a point.    
//     Calculates (in ctor) the distance and the flight lengths alongs 
//     the trajectory or trajectories; calculated values are obtained 
//     through accessors.  "Precision" is maximum allowed error on distance 
//     (in cm).  The input flightlengths are used as a starting point; the 
//     code will find the point-of-closest-approach that is closest to that
//     point.  (A good starting point also reduces CPU time.)
//     Note that distance is a signed quantity for two trajectories.
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner; core algorithm stolen from Art Snyder.
//
//------------------------------------------------------------------------
#ifndef TRKPOCA_H
#define TRKPOCA_H
class TrkDifTraj;
#include "TrkBase/TrkPocaBase.h"

// Class interface //
class TrkPoca : public TrkPocaBase {
public:
  TrkPoca(const Trajectory& traj1, double flt1,
          const Trajectory& traj2, double flt2,
          double precision=1.e-5);
  TrkPoca(const Trajectory& traj,  double flt,
          const HepPoint3D& pt, double precision=1.e-5);
  ~TrkPoca() {};

  inline double doca() const;                   // distance of closest approach
  /*
  // The following inherited functions are also available:
    const TrkErrCode& status() const;      // did the calculation succeed?
    double flt1() const;                   // path length on traj 1 @ poca
    double flt2() const;
    double precision();                    // In case anyone wants to know:
  */

private:
  double _doca;

  // private functions
  void calcDist(const Trajectory& traj1, const Trajectory& traj2);
  // Preempt
//  TrkPoca&   operator= (const TrkPoca&);
//  TrkPoca(const TrkPoca &);
};

// Inlined functions
double TrkPoca::doca() const                           {return _doca;}

#endif
