#ifndef MDCXPARAMETERS_H
#define MDCXPARAMETERS_H


class MdcxParameters {
 public:
   virtual ~MdcxParameters() {};
   /// constants
   static const double c;

   /// Track attribute
   static const double maxTrkOmega;
   static const double maxTrkLength;
   static double maxProca;
   static double minTrkProb;

   static const double maxDd0InMerge;
   static const double maxDphi0InMerge;
   static const double maxPdradInMerge;
   static const double maxRcsInMerge;
   static double maxRcsInAddSeg;
   static double nSigAddHitTrk;//yzhang 2010-05-05 
   static double addHitFactor;

   static const double maxAddHitCut;
   static double helixFitSigma;

   //track fiting cut
   static double csmax4;
   static double csmax3;

   /// MDC Geometry
   static const double maxMdcRadius;
   static const double firstMdcAxialRadius;
   static const double maxMdcZLen;

   ///relative to MdcxFindTracks
   static const int nSegCombo = 13;
   static const int    findTrkGroup[nSegCombo][3];
   static const float  maxDp12[nSegCombo];
   static const float  maxDp13[nSegCombo];
   static const int    layerSet2AddSeg[nSegCombo][11];
   static float        dropHitsSigma[43];
   static int debug;
   static const double maxDlen;

 private:
   MdcxParameters();
};

#endif //MDCX_PARAMETERS_H

