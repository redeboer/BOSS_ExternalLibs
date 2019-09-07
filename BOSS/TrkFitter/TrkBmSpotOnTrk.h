//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkBmSpotOnTrk.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//   Defines interface between Svt hit and track, and stores information
//       about fit.  Used by fitters
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//
//      Copyright (C) 1996      Stanford Linear Accelerator Center
//------------------------------------------------------------------------

#ifndef TRKBMSPOTONTRK_H
#define TRKBMSPOTONTRK_H

#include "TrkBase/TrkHitOnTrk.h"
#include "TrkBase/TrkEnums.h"
#include "TrkBase/TrkLineTraj.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/SymMatrix.h"


// Class interface //
class TrkBmSpotOnTrk : public TrkHitOnTrk 
{
public:
  TrkBmSpotOnTrk(const HepPoint3D &ip, const HepSymMatrix &size );
  ~TrkBmSpotOnTrk();

  TrkBmSpotOnTrk* clone(TrkRep*, const TrkDifTraj *t =0) const;
  virtual TrkEnums::TrkViewInfo whatView()     const;
  virtual unsigned layerNumber() const { return 0; }

  const Trajectory*   hitTraj() const;
  const HepPoint3D&     ip() const;

// this hot has no timing information
  virtual bool timeResid(double& t,double& tErr) const { return false; }
  virtual bool timeAbsolute(double& t,double& tErr) const { return false; }

protected:
  TrkBmSpotOnTrk(const TrkBmSpotOnTrk &hitToBeCopied,TrkRep *newRep, const TrkDifTraj *trkTraj=0);
  virtual TrkErrCode updateMeasurement(const TrkDifTraj* traj, bool maintainAmbiguity);
  
  static const TrkLineTraj FindBeamTrajectory( const HepPoint3D &point, const HepSymMatrix &error );
  double GetRms();

private:
  // Make sure these aren't used by mistake:
  TrkBmSpotOnTrk&   operator= (const TrkBmSpotOnTrk&);
  TrkBmSpotOnTrk(const TrkBmSpotOnTrk&);

  const TrkLineTraj _beamTraj;
  const HepPoint3D _ip;
  const HepSymMatrix _size;
};

#endif
