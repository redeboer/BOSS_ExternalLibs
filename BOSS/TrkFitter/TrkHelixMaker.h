//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixMaker.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//   Creates tracks with HelixReps inside them (makeTrack()), or swaps 
//   in a HelixRep for the existing Rep (changeFit()).  In neither case 
//   is a fit performed.  HelixReps have an option of allowing hit-dropping 
//   and ambiguity-flipping while fitting; these can be turned on  
//   by invoking setFlipAndDrop() for that track.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKHELIXMAKER_H
#define TRKHELIXMAKER_H
#include "TrkFitter/TrkSimpleMaker.h"
#include "TrkFitter/TrkHelixRep.h"


class TrkHelixMaker : public TrkSimpleMaker<TrkHelixRep> {
public:
  TrkHelixMaker();
  virtual ~TrkHelixMaker();

  void addZValues(TrkRecoTrk& theTrack, double z0,
                  double tanDip, double chi2);

protected:
  TrkRep* makeRep(TrkRecoTrk& theTrack) const;

private:

  // Preempt 
  TrkHelixMaker&   operator= (const TrkHelixMaker&);
  TrkHelixMaker(const TrkHelixMaker &);
};

#endif
