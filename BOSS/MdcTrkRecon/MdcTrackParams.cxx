//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcTrackParams.cxx,v 1.4 2011/09/26 01:06:37 zhangy Exp $
//
// Description:
//     
//
// Environment:
//      Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author(s): Steve Schaffner
//
//------------------------------------------------------------------------

#include "MdcTrkRecon/MdcTrackParams.h"

//------------------------------------------------------------------------
MdcTrackParams::MdcTrackParams() { 
  //------------------------------------------------------------------------
  lPrint = 1;
  lPlot = 1;
  lHist = 0;
  lUseQualCuts = 0;
  minHits = 10;
  maxChisq = 10.;
  maxNmissTrack = 100; 
  maxNmissNorm = 100;  
  nOverlap = 3;
  maxSegChisqO = 10.;
  maxInactiveResid = 1.;
  maxActiveSigma = 5.0;
  pickHitFract = 0.6;
  pickHitMargin = 0.2;
  pickHitFactor = 2.;
  lRemoveInActive = 0;
  pickHitPhiFactor = 5.;
  pickUitlLastLayer = 0;
  nHitDeleted = 10;
  pickSkipExistLayer = 0;
  combineByFitHits = 0;
  nGapGE2 = 99;//yzhang 2011-07-29 
  nGapGE3 = 99;
  maxGapLength = 8;
}

//------------------------------------------------------------------------
MdcTrackParams::~MdcTrackParams() { 
//------------------------------------------------------------------------
}
