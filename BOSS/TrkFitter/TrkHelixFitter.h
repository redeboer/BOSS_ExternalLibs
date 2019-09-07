//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: TrkHelixFitter.h,v 1.4 2011/02/17 11:54:59 maqm Exp $
//
// Description:
//  Fit track to helix or circle.  Flags allow fitter to add and drop 
//   hits (i.e. make them active or inactive), and (in the case of 
//   MDC hits) flip their ambiguities.
// Produces an error matrix, but does not (currently) handle multiple 
//  scattering.
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------
#ifndef TRKHELIXFITTER_H
#define TRKHELIXFITTER_H

#include "TrkBase/TrkHitOnTrkUpdater.h"
class TrkRecoTrk;
class TrkSimpTraj;
class TrkHitOnTrk;
class TrkErrCode;
class TrkHotList;

// Class definition//
class TrkHelixFitter: public TrkHitOnTrkUpdater {

public:
  TrkHelixFitter(bool allowFlips=false, bool allowDrops=false);
  virtual ~TrkHelixFitter();
  TrkHelixFitter& operator=(const TrkHelixFitter& right);
  TrkHelixFitter(const TrkHelixFitter &);

  TrkErrCode fit(TrkHotList& hitList, TrkSimpTraj&);
  void setFittingPar(bool allowFlips, bool allowDrops);
  double lastChisq() const {return _lastChisq;}
  static bool m_debug;
  static double nSigmaCut[43];

private:
  bool _allowDrops;
  bool _allowFlips;
  double _lastChisq;

  void setLastChisq(double l)                              {_lastChisq = l;}
};

#endif


