//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixRep.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//      Implementation class for TrkRep using a single helix
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKHELIXREP_HH
#define TRKHELIXREP_HH

#include "TrkFitter/TrkSimpleRep.h"
#include "TrkBase/HelixTraj.h"

class TrkHelixRep : public TrkSimpleRep {
public:
  //******************************************
  // Constructors and such (copy ctor is protected)
  //******************************************
  // Construct from scratch
  TrkHelixRep(const TrkExchangePar& inPar, TrkRecoTrk* myTrack, PdtPid::PidType hypo, const TrkHotList* = 0);
  TrkHelixRep(const TrkExchangePar& inPar, TrkRecoTrk* trk,
              PdtPid::PidType hypo, int nact, int nsv, int ndc,
              double chi2, double startFoundRange, double endFoundRange);
  virtual ~TrkHelixRep();
  // specific clone operation for this class
  virtual TrkHelixRep* clone(TrkRecoTrk* newTrack) const;
  virtual TrkHelixRep* cloneNewHypo(PdtPid::PidType hypo);

  //******************************************
  // Global quantities:
  //******************************************
  virtual TrkDifTraj& traj();
  virtual const TrkDifTraj& traj() const;
  const IfdKey& myKey() const;

  //******************************************
  // Information about track at a given position
  //******************************************
  virtual TrkExchangePar helix(double fltLen) const;

  virtual bool resid(const TrkHitOnTrk *h,
                     double& residual, double& residErr,
                     bool exclude) const;
  //******************************************
  // Printing
  //******************************************
  virtual void printAll(std::ostream& ostr) const;
  virtual void print(std::ostream& ostr) const;

  virtual TrkSimpTraj& simpTraj();
  virtual const TrkSimpTraj& simpTraj() const;

protected:
// copy constructor -- invoked by clone
  TrkHelixRep(const TrkHelixRep& right, TrkRecoTrk* trk, PdtPid::PidType);

private:
  TrkHelixRep& operator=(const TrkHelixRep &right);
  HelixTraj _traj;
};
#endif
