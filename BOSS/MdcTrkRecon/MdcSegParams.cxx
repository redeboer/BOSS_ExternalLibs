//--------------------------------------------------------------------------
// File and Version Information:
//      $Id: MdcSegParams.cxx,v 1.3 2011/09/26 01:06:37 zhangy Exp $
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

#include "MdcTrkRecon/MdcSegParams.h"

//------------------------------------------------------------------------
MdcSegParams::MdcSegParams() {
//------------------------------------------------------------------------
  maxChisq = 20.;
  lPlot = 1;
  lHist = 0;
  segRefit = 0;
  nsigAddHit = 3.5;
  useAllAmbig = 0;
  z0cut = 300.0;
  ctcut = 3.0;
  phiDiffDropMult = 0.0001;
  slopeDiffDrop = 0.0001;
  delZ0Cut = 15.;
  delCtCut = 0.6;
  dropDups = 1;
  for (int i=0;i<11;i++) find3[i]=1;
  addHits = 1;
  lPrint = 1;
}

//------------------------------------------------------------------------
MdcSegParams::~MdcSegParams() {
//------------------------------------------------------------------------
}
