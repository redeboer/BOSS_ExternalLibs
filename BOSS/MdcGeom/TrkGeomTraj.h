//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkGeomTraj.h,v 1.2 2010/03/25 09:54:17 zhangy Exp $
//
// Description:
//     Base class for all simple, geometric trajectories (i.e. trajs that 
//     don't describe tracks).  Inherits from Trajectory.  Supports 
//     (via accept() function) Visitor pattern for adding functionality.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKGEOMTRAJ_H
#define TRKGEOMTRAJ_H
#include "MdcGeom/Trajectory.h"


class TrkGeomTrajVisitor;

// Class interface //
class TrkGeomTraj : public Trajectory {

public:
  TrkGeomTraj(double lowlim, double hilim);
  virtual ~TrkGeomTraj();

  virtual void accept(TrkGeomTrajVisitor&) const;
  virtual TrkGeomTraj* clone() const = 0;

private:
  // Preempt
  TrkGeomTraj&   operator= (const TrkGeomTraj&);
  TrkGeomTraj(const TrkGeomTraj &);
};

#endif
