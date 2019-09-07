//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcRecoHitOnTrack.h,v 1.4 2010/03/03 23:47:51 zhangy Exp $
//
// Description:
//   Contains drift chamber hit info, as hit is used on a particular track
//   Inherits from TrkHitOnTrk.  The drift distance is stored as an 
//   absolute value, but returned as |drift|*ambig.  Ambiguity stored as +/- 1; 
//   ambiguity = 0 => pick better value @ first call to updateMeasurement.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Authors: Steve Schaffner
//------------------------------------------------------------------------

#ifndef MDCRECOHITONTRACK_H
#define MDCRECOHITONTRACK_H

#include "MdcData/MdcHitOnTrack.h"

class TrkRep;
class TrkDifTraj;
class MdcHit;

// Class definition//
class MdcRecoHitOnTrack : public MdcHitOnTrack {
public:
  MdcRecoHitOnTrack(const MdcHit& baseHit, int ambig, double bunchTime);
  virtual ~MdcRecoHitOnTrack();
  virtual TrkHitOnTrk* clone(TrkRep *, const TrkDifTraj *trkTraj=0) const;

protected:
  MdcRecoHitOnTrack(const MdcRecoHitOnTrack &hitToBeCopied, TrkRep *newRep,
                const TrkDifTraj* trkTraj=0 );

public:
  unsigned                      status()          const;
  const MdcHit*                 mdcHit()          const;
  unsigned                      tdcIndex()        const;
  int                           whichView()       const;

private:

  MdcRecoHitOnTrack(const MdcRecoHitOnTrack&);
  MdcRecoHitOnTrack& operator=(const MdcRecoHitOnTrack&);
};

#endif
