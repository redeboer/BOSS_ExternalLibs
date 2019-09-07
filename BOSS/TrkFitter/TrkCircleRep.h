//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkCircleRep.h,v 1.1.1.1 2005/04/21 06:26:56 maqm Exp $
//
// Description:
//      Implementation class for TrkRep using a circle
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKCIRCLEREP_HH
#define TRKCIRCLEREP_HH

#include "TrkFitter/TrkSimpleRep.h"
#include "TrkFitter/TrkCircleTraj.h"

class TrkCircleRep : public TrkSimpleRep {
public:
  //******************************************
  // Constructors and such (see protected fcns for copy ctor)
  //******************************************
  // Can contain hits:
  TrkCircleRep(const TrkExchangePar& inPar, TrkRecoTrk* myTrack, PdtPid::PidType hypo, const TrkHotList* = 0);
  // No hits:
  TrkCircleRep(const TrkExchangePar& inPar, TrkRecoTrk* myTrack, 
               PdtPid::PidType hypo, int nActive, int nSvt, int nDch, double chi2,
               double startFoundRange, double endFoundRange);
  virtual ~TrkCircleRep();
  // specific clone operation for this class
  virtual TrkCircleRep* clone(TrkRecoTrk* newTrack) const;
  virtual TrkCircleRep* cloneNewHypo(PdtPid::PidType hypo);

  //******************************************
  // Global quantities:
  //******************************************
  virtual TrkDifTraj& traj();
  virtual const TrkDifTraj& traj() const;

  //******************************************
  // Information about track at a given position
  //******************************************
  virtual TrkExchangePar helix(double fltLen) const;

  //******************************************
  // Printing
  //******************************************
  virtual void printAll(std::ostream& ostr) const;
  virtual void print(std::ostream& ostr) const;

protected:
// copy constructor -- invoked by clone
  TrkCircleRep(const TrkCircleRep& right, TrkRecoTrk* trk, PdtPid::PidType);
  virtual TrkSimpTraj& simpTraj();
  virtual const TrkSimpTraj& simpTraj() const;

private:
  TrkCircleRep& operator=(const TrkCircleRep &right);  // could be implemented
  TrkCircleTraj _traj;
};
#endif
