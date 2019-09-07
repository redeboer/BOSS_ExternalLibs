//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkLineRep.h,v 1.2 2007/12/07 07:06:34 zhangy Exp $
//
// Description:
//      Implementation class for TrkRep using a 3-d straight line
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Steve Schaffner
//
//------------------------------------------------------------------------

#ifndef TRKLINEREP_HH
#define TRKLINEREP_HH

#include "TrkFitter/TrkSimpleRep.h"
#include "TrkFitter/TrkDifLineTraj.h"

class TrkLineRep : public TrkSimpleRep {
public:
  //******************************************
  // Constructors and such (see protected fcns for copy ctor)
  //******************************************
  // Create from scratch
  TrkLineRep(const TrkExchangePar& inPar, TrkRecoTrk* myTrack, PdtPid::PidType hypo, const TrkHotList* =0);
  // No hits
  TrkLineRep(const TrkExchangePar& inPar, TrkRecoTrk* myTrack,
             PdtPid::PidType hypo, int nAct, int nSvt, int nDch, double chi2,
             double startFoundRange, double endFoundRange);
  virtual ~TrkLineRep();
  // specific clone operation for this class
  virtual TrkLineRep* clone(TrkRecoTrk* newTrack) const;
  virtual TrkLineRep* cloneNewHypo(PdtPid::PidType hypo);

  //******************************************
  // Global quantities:
  //******************************************
  virtual TrkDifTraj& traj();
  virtual const TrkDifTraj& traj() const;

  //******************************************
  // Information about track at a given position
  //******************************************
  virtual TrkExchangePar helix(double fltLen) const;
  
  double arrivalTime(double fltLen) const;
  //******************************************
  // Printing
  //******************************************
  virtual void printAll(std::ostream& ostr) const;
  virtual void print(std::ostream& ostr) const;

protected:
// copy constructor -- invoked by clone
  TrkLineRep(const TrkLineRep& right, TrkRecoTrk* trk, PdtPid::PidType);
  virtual TrkSimpTraj& simpTraj();
  virtual const TrkSimpTraj& simpTraj() const;

private:
  TrkLineRep& operator=(const TrkLineRep &right);
  TrkDifLineTraj _traj;
};
#endif
