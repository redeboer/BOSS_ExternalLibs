// Kludgy class to hold misc segment parameters that are run-time settable
//    just a convenient way of moving them about

// Interface Dependencies ----------------------------------------------

#ifndef MDCSEGPARAMS_H
#define MDCSEGPARAMS_H

//  End Interface Dependencies -----------------------------------------

// Class //
class MdcSegParams {
 public:
  MdcSegParams();
  ~MdcSegParams();
  
  double maxChisq;       // per DOF
  int lPlot;
  //  int allMC;
  int lHist;
  int segRefit;     // refit all segments, using new correction for drift dist
  double nsigAddHit;  // max allowed n sigma to add hit to existing segment
  int useAllAmbig;
  double z0cut;      // for including stereo segs in cluster (rough cut)
  double ctcut;
  double phiDiffDropMult;    // for dropping duplicate segs
  double slopeDiffDrop;    // for dropping duplicate segs
  double delCtCut;       //for grouping stereo segs (invalidStereo)
  double delZ0Cut;
  int dropDups;
  int find3[11];            // search for 3-hit segments for each superlayer
  int addHits;
  int lPrint;

};

#endif







